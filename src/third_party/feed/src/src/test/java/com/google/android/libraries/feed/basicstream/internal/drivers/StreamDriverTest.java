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

package com.google.android.libraries.feed.basicstream.internal.drivers;

import static com.google.common.truth.Truth.assertThat;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;

import com.google.android.libraries.feed.api.common.ThreadUtils;
import com.google.android.libraries.feed.api.modelprovider.ModelChild;
import com.google.android.libraries.feed.api.modelprovider.ModelChild.Type;
import com.google.android.libraries.feed.api.modelprovider.ModelFeature;
import com.google.android.libraries.feed.api.modelprovider.ModelProvider;
import com.google.android.libraries.feed.api.modelprovider.ModelToken;
import com.google.android.libraries.feed.api.modelprovider.TokenCompleted;
import com.google.android.libraries.feed.api.modelprovider.TokenCompletedObserver;
import com.google.android.libraries.feed.basicstream.internal.contentmodels.ContentModel;
import com.google.android.libraries.feed.basicstream.internal.util.TokenHandler;
import com.google.android.libraries.feed.testing.modelprovider.FakeModelCursor;
import com.google.search.now.feed.client.StreamDataProto.StreamFeature;
import com.google.search.now.ui.stream.StreamStructureProto.Card;
import com.google.search.now.ui.stream.StreamStructureProto.Cluster;
import java.util.ArrayList;
import java.util.List;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.ArgumentCaptor;
import org.mockito.Captor;
import org.mockito.Mock;
import org.robolectric.RobolectricTestRunner;

/** Tests for {@link StreamDriver}. */
@RunWith(RobolectricTestRunner.class)
public class StreamDriverTest {

  private StreamDriver streamDriver;

  @Mock private ModelFeature streamFeature;
  @Mock private ContentModel continuationContentModel;
  @Mock private ContentModel clusterContentModel;
  @Mock private ContentModel cardContentModel;
  @Mock private ModelProvider modelProvider;
  @Mock private ContinuationDriver continuationDriver;
  @Mock private ClusterDriver clusterDriver;
  @Mock private CardDriver cardDriver;

  @Captor private ArgumentCaptor<TokenCompletedObserver> tokenCompletedObserverCaptor;

  @Before
  public void setup() {
    initMocks(this);
    ThreadUtils threadUtils = new ThreadUtils();
    TokenHandler tokenHandler =
        new TokenHandler() {
          @Override
          public void tokenConsumed() {}

          @Override
          public void requestTokenHandling(ModelToken token) {}
        };

    when(continuationDriver.getContentModel()).thenReturn(continuationContentModel);
    when(clusterDriver.getContentModel()).thenReturn(clusterContentModel);
    when(cardDriver.getContentModel()).thenReturn(cardContentModel);
    when(modelProvider.getRootFeature()).thenReturn(streamFeature);

    streamDriver = new StreamDriverForTest(modelProvider, threadUtils, tokenHandler);
  }

  @Test
  public void testContinuationToken_createsContinuationContentModel() {
    FakeModelCursor cursor = new FakeCursorBuilder().addToken().build();
    when(streamFeature.getCursor()).thenReturn(cursor);

    List<ContentModel> contentModels = streamDriver.getContentModels();
    assertThat(contentModels).hasSize(1);
    assertThat(contentModels.get(0)).isEqualTo(continuationContentModel);
  }

  @Test
  public void testContinuationToken_tokenHandling() {
    FakeModelCursor cursorWithToken = new FakeCursorBuilder().addToken().build();
    when(streamFeature.getCursor()).thenReturn(cursorWithToken);

    // Causes StreamDriver to build a list of children based on the children from the cursor.
    streamDriver.getContentModels();

    ModelToken token = cursorWithToken.getChildAt(0).getModelToken();

    verify(token).registerObserver(tokenCompletedObserverCaptor.capture());

    // Indicates the token has been successfully handled by the ModelProvider, leading to adding
    // children from the token's cursor.
    tokenCompletedObserverCaptor
        .getValue()
        .onChange(new TokenCompleted(new FakeCursorBuilder().addCluster().build()));

    List<ContentModel> contentModels = streamDriver.getContentModels();
    assertThat(contentModels).hasSize(1);
    assertThat(contentModels.get(0)).isEqualTo(clusterContentModel);

    // If the above two assertions pass, this is also guaranteed to pass. This is just to explicitly
    // check that the continuationContentModel has been removed.
    assertThat(contentModels).doesNotContain(continuationContentModel);
  }

  @Test
  public void testContinuationToken_tokenChildrenAddedAtTokenPosition() {
    FakeModelCursor streamCursor = new FakeCursorBuilder().addCluster().addToken().build();
    when(streamFeature.getCursor()).thenReturn(streamCursor);

    List<ContentModel> contentModels = streamDriver.getContentModels();
    assertThat(contentModels).hasSize(2);
    assertThat(contentModels).containsExactly(clusterContentModel, continuationContentModel);

    ModelToken token = streamCursor.getChildAt(1).getModelToken();

    verify(token).registerObserver(tokenCompletedObserverCaptor.capture());

    // Indicates the token has been successfully handled by the ModelProvider, leading to adding
    // children from the token's cursor.
    tokenCompletedObserverCaptor
        .getValue()
        .onChange(new TokenCompleted(new FakeCursorBuilder().addCluster().addToken().build()));

    contentModels = streamDriver.getContentModels();
    assertThat(contentModels).hasSize(3);
    assertThat(contentModels)
        .containsExactly(clusterContentModel, clusterContentModel, continuationContentModel);
  }

  @Test
  public void testContinuationToken_tokenChildrenAddedAtTokenPosition_tokenNotAtEnd() {
    FakeModelCursor streamCursor =
        new FakeCursorBuilder().addCluster().addToken().addCluster().build();
    when(streamFeature.getCursor()).thenReturn(streamCursor);

    List<ContentModel> contentModels = streamDriver.getContentModels();
    assertThat(contentModels).hasSize(3);
    assertThat(contentModels)
        .containsExactly(clusterContentModel, continuationContentModel, clusterContentModel);

    ModelToken token = streamCursor.getChildAt(1).getModelToken();

    verify(token).registerObserver(tokenCompletedObserverCaptor.capture());

    // Indicates the token has been successfully handled by the ModelProvider, leading to adding
    // children from the token's cursor.
    tokenCompletedObserverCaptor
        .getValue()
        .onChange(
            new TokenCompleted(
                new FakeCursorBuilder().addCluster().addCard().addCluster().build()));

    contentModels = streamDriver.getContentModels();
    assertThat(contentModels).hasSize(5);
    assertThat(contentModels)
        .containsExactly(
            clusterContentModel,
            clusterContentModel,
            cardContentModel,
            clusterContentModel,
            clusterContentModel);
  }

  private class StreamDriverForTest extends StreamDriver {

    StreamDriverForTest(
        ModelProvider modelProvider, ThreadUtils threadUtils, TokenHandler tokenHandler) {
      super(modelProvider, threadUtils, tokenHandler);
    }

    @Override
    ContinuationDriver createContinuationDriver(TokenHandler tokenHandler, ModelToken modelToken) {
      return continuationDriver;
    }

    @Override
    ClusterDriver createClusterDriver(ModelFeature modelFeature) {
      return clusterDriver;
    }

    @Override
    CardDriver createCardDriver(ModelFeature modelFeature) {
      return cardDriver;
    }
  }

  private static class FakeCursorBuilder {

    List<ModelChild> cursorChildren = new ArrayList<>();

    FakeCursorBuilder addCard() {
      ModelFeature cardFeature = mock(ModelFeature.class);
      when(cardFeature.getStreamFeature())
          .thenReturn(StreamFeature.newBuilder().setCard(Card.getDefaultInstance()).build());

      ModelChild cardChild = mock(ModelChild.class);
      when(cardChild.getType()).thenReturn(Type.FEATURE);
      when(cardChild.getModelFeature()).thenReturn(cardFeature);

      cursorChildren.add(cardChild);

      return this;
    }

    FakeCursorBuilder addCluster() {
      ModelFeature clusterFeature = mock(ModelFeature.class);
      when(clusterFeature.getStreamFeature())
          .thenReturn(StreamFeature.newBuilder().setCluster(Cluster.getDefaultInstance()).build());

      ModelChild clusterChild = mock(ModelChild.class);
      when(clusterChild.getType()).thenReturn(Type.FEATURE);
      when(clusterChild.getModelFeature()).thenReturn(clusterFeature);

      cursorChildren.add(clusterChild);

      return this;
    }

    FakeCursorBuilder addToken() {
      ModelChild tokenChild = mock(ModelChild.class);
      ModelToken modelToken = mock(ModelToken.class);
      when(tokenChild.getType()).thenReturn(Type.TOKEN);
      when(tokenChild.getModelToken()).thenReturn(modelToken);

      cursorChildren.add(tokenChild);

      return this;
    }

    public FakeModelCursor build() {
      return new FakeModelCursor(cursorChildren);
    }
  }
}
