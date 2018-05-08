// Copyright 2018 The Feed Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package com.google.android.libraries.feed.feedprotocoladapter;

import com.google.android.libraries.feed.api.protocoladapter.ProtocolAdapter;
import com.google.android.libraries.feed.common.Result;
import com.google.android.libraries.feed.common.Validators;
import com.google.android.libraries.feed.common.logging.Dumpable;
import com.google.android.libraries.feed.common.logging.Dumper;
import com.google.android.libraries.feed.common.logging.Logger;
import com.google.android.libraries.feed.common.time.TimingUtils;
import com.google.android.libraries.feed.common.time.TimingUtils.ElapsedTimeTracker;
import com.google.protobuf.ByteString;
import com.google.search.now.feed.client.StreamDataProto.StreamDataOperation;
import com.google.search.now.feed.client.StreamDataProto.StreamFeature;
import com.google.search.now.feed.client.StreamDataProto.StreamPayload;
import com.google.search.now.feed.client.StreamDataProto.StreamSharedState;
import com.google.search.now.feed.client.StreamDataProto.StreamStructure;
import com.google.search.now.feed.client.StreamDataProto.StreamStructure.Operation;
import com.google.search.now.feed.client.StreamDataProto.StreamToken;
import com.google.search.now.ui.stream.StreamStructureProto.Card;
import com.google.search.now.ui.stream.StreamStructureProto.Cluster;
import com.google.search.now.ui.stream.StreamStructureProto.Content;
import com.google.search.now.ui.stream.StreamStructureProto.Stream;
import com.google.search.now.wire.feed.ContentIdProto.ContentId;
import com.google.search.now.wire.feed.DataOperationProto.DataOperation;
import com.google.search.now.wire.feed.FeedResponseProto.FeedResponse;
import com.google.search.now.wire.feed.PietSharedStateItemProto.PietSharedStateItem;
import com.google.search.now.wire.feed.ResponseProto.Response;
import java.util.ArrayList;
import java.util.List;

/** A ProtocolAdapter which converts from the wire protocol to the internal protocol. */
public class FeedProtocolAdapter implements ProtocolAdapter, Dumpable {

  private static final String TAG = "FeedProtocolAdapter";

  private final TimingUtils timingUtils;
  private int nextTokenId = 1;

  // Operation counts for #dump(Dumpable)
  private int responseHandlingCount = 0;
  private int convertContentIdCount = 0;

  public FeedProtocolAdapter(TimingUtils timingUtils) {
    this.timingUtils = timingUtils;
  }

  @Override
  public String getStreamContentId(ContentId contentId) {
    convertContentIdCount++;
    return createContentId(contentId);
  }

  @Override
  public Result<List<StreamDataOperation>> createModel(Response response) {
    ElapsedTimeTracker totalTimeTracker = timingUtils.getElapsedTimeTracker(TAG);
    List<StreamDataOperation> streamDataOperations = new ArrayList<>();
    responseHandlingCount++;

    List<DataOperation> dataOperations =
        response.getExtension(FeedResponse.feedResponse).getDataOperationList();
    for (DataOperation operation : dataOperations) {
      // The operations defined in stream_data.proto and data_operation.proto have the same
      // integer value
      Operation streamOperation = Operation.forNumber(operation.getOperation().getNumber());
      String contentId;
      if (streamOperation == Operation.CLEAR_ALL) {
        streamDataOperations.add(createDataOperation(Operation.CLEAR_ALL, null, null).build());
        continue;
      } else if (streamOperation == Operation.REMOVE) {
        if (operation.getMetadata().hasContentId()) {
          contentId = createContentId(operation.getMetadata().getContentId());
          String parentId = null;
          if (operation.getFeature().hasParentId()) {
            parentId = createContentId(operation.getFeature().getParentId());
          }
          streamDataOperations.add(
              createDataOperation(Operation.REMOVE, contentId, parentId).build());

        } else {
          Logger.w(TAG, "REMOVE defined without a ContentId identifying the item to remove");
        }
        continue;
      } else if (operation.getMetadata().hasContentId()) {
        contentId = createContentId(operation.getMetadata().getContentId());
      } else {
        // This is an error state, every card should have a content id
        Logger.e(TAG, "ContentId not defined for DataOperation");
        continue;
      }

      if (operation.hasFeature()) {
        StreamFeature.Builder streamFeature = StreamFeature.newBuilder();
        streamFeature.setContentId(contentId);
        String parentId = null;
        if (operation.getFeature().hasParentId()) {
          parentId = createContentId(operation.getFeature().getParentId());
          streamFeature.setParentId(parentId);
        }
        switch (operation.getFeature().getRenderableUnit()) {
          case STREAM:
            streamFeature.setStream(operation.getFeature().getExtension(Stream.streamExtension));
            break;
          case CARD:
            streamFeature.setCard(operation.getFeature().getExtension(Card.cardExtension));
            break;
          case CONTENT:
            streamFeature.setContent(operation.getFeature().getExtension(Content.contentExtension));
            break;
          case CLUSTER:
            streamFeature.setCluster(operation.getFeature().getExtension(Cluster.clusterExtension));
            break;
          default:
            throw new RuntimeException("Unknown feature payload");
        }
        streamDataOperations.add(
            createFeatureDataOperation(streamOperation, contentId, parentId, streamFeature.build())
                .build());
      } else if (operation.hasPietSharedState()) {
        PietSharedStateItem item =
            PietSharedStateItem.newBuilder()
                .setPietSharedState(operation.getPietSharedState())
                .build();
        StreamSharedState state =
            StreamSharedState.newBuilder()
                .setPietSharedStateItem(item)
                .setContentId(contentId)
                .build();
        streamDataOperations.add(
            createSharedStateDataOperation(streamOperation, contentId, state).build());
      }

      if (operation.getMetadata().getSemanticProperties().hasSemanticPropertiesData()) {
        streamDataOperations.add(
            createSemanticDataOperation(
                    contentId,
                    operation.getMetadata().getSemanticProperties().getSemanticPropertiesData())
                .build());
      }
    }

    // if there is a continuation token it should be the last thing added to the root.
    ByteString newContinuationToken =
        response.getExtension(FeedResponse.feedResponse).getNextPageToken();
    if (newContinuationToken != null && !newContinuationToken.isEmpty()) {
      String contentId =
          createContentId(
              ContentId.newBuilder()
                  .setContentDomain("token")
                  .setId(nextTokenId++)
                  .setTable("feature")
                  .build());
      StreamToken streamToken =
          Validators.checkNotNull(createStreamToken(contentId, newContinuationToken));
      streamDataOperations.add(
          createTokenDataOperation(contentId, streamToken.getParentId(), streamToken).build());
    }
    totalTimeTracker.stop("", "convertWireProtocol", "mutations", dataOperations.size());
    return Result.success(streamDataOperations);
  }

  /*@Nullable*/
  private StreamToken createStreamToken(String tokenId, ByteString continuationToken) {
    if (continuationToken.isEmpty()) {
      return null;
    }
    StreamToken.Builder tokenBuilder = StreamToken.newBuilder();
    // TODO: This needs to be the real root id.  How do we get the root id?
    String rootContentId =
        createContentId(
            ContentId.newBuilder()
                .setContentDomain("stream_root")
                .setId(0)
                .setTable("feature")
                .build());
    tokenBuilder.setParentId(rootContentId);
    tokenBuilder.setContentId(tokenId);
    tokenBuilder.setNextPageToken(continuationToken);
    return tokenBuilder.build();
  }

  private StreamDataOperation.Builder createFeatureDataOperation(
      Operation operation,
      /*@Nullable*/ String contentId,
      /*@Nullable*/ String parentId,
      StreamFeature streamFeature) {
    StreamDataOperation.Builder dataOperation = createDataOperation(operation, contentId, parentId);
    dataOperation.setStreamPayload(StreamPayload.newBuilder().setStreamFeature(streamFeature));
    return dataOperation;
  }

  private StreamDataOperation.Builder createSharedStateDataOperation(
      Operation operation, /*@Nullable*/ String contentId, StreamSharedState sharedState) {
    StreamDataOperation.Builder dataOperation = createDataOperation(operation, contentId, null);
    dataOperation.setStreamPayload(StreamPayload.newBuilder().setStreamSharedState(sharedState));
    return dataOperation;
  }

  private StreamDataOperation.Builder createTokenDataOperation(
      /*@Nullable*/ String contentId, /*@Nullable*/ String parentId, StreamToken streamToken) {
    StreamDataOperation.Builder dataOperation =
        createDataOperation(Operation.UPDATE_OR_APPEND, contentId, parentId);
    dataOperation.setStreamPayload(StreamPayload.newBuilder().setStreamToken(streamToken));
    return dataOperation;
  }

  private StreamDataOperation.Builder createDataOperation(
      Operation operation, /*@Nullable*/ String contentId, /*@Nullable*/ String parentId) {
    StreamDataOperation.Builder streamDataOperation = StreamDataOperation.newBuilder();
    StreamStructure.Builder streamStructure = StreamStructure.newBuilder();
    streamStructure.setOperation(operation);
    if (contentId != null) {
      streamStructure.setContentId(contentId);
    }
    if (parentId != null) {
      streamStructure.setParentContentId(parentId);
    }
    streamDataOperation.setStreamStructure(streamStructure);
    return streamDataOperation;
  }

  private StreamDataOperation.Builder createSemanticDataOperation(
      String contentId, ByteString semanticData) {
    return StreamDataOperation.newBuilder()
        .setStreamPayload(StreamPayload.newBuilder().setSemanticData(semanticData))
        .setStreamStructure(StreamStructure.newBuilder().setContentId(contentId));
  }

  private String createContentId(ContentId contentId) {
    // Using String concat for performance reasons.  This is called a lot for large feed responses.
    return contentId.getTable() + "::" + contentId.getContentDomain() + "::" + contentId.getId();
  }

  @Override
  public void dump(Dumper dumper) {
    dumper.title(TAG);
    dumper.forKey("responseHandlingCount").value(responseHandlingCount);
    dumper.forKey("convertContentIdCount").value(convertContentIdCount).compactPrevious();
  }
}
