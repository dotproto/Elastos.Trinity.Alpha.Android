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

package com.google.android.libraries.feed.basicstream;

import static com.google.android.libraries.feed.api.sessionmanager.SessionManager.GLOBAL_SHARED_STATE;
import static com.google.common.truth.Truth.assertThat;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;

import android.app.Activity;
import android.os.Build.VERSION_CODES;
import android.view.View;
import com.google.android.libraries.feed.api.actionparser.ActionParser;
import com.google.android.libraries.feed.api.common.ThreadUtils;
import com.google.android.libraries.feed.api.modelprovider.ModelChild;
import com.google.android.libraries.feed.api.modelprovider.ModelFeature;
import com.google.android.libraries.feed.api.modelprovider.ModelProvider;
import com.google.android.libraries.feed.api.modelprovider.ModelProviderFactory;
import com.google.android.libraries.feed.basicstream.internal.contentmodels.ContentModel;
import com.google.android.libraries.feed.basicstream.internal.contentmodels.PietContentModel;
import com.google.android.libraries.feed.common.testing.FakeClock;
import com.google.android.libraries.feed.host.action.ActionApi;
import com.google.android.libraries.feed.host.config.DebugBehavior;
import com.google.android.libraries.feed.host.imageloader.ImageLoaderApi;
import com.google.android.libraries.feed.host.stream.CardConfiguration;
import com.google.android.libraries.feed.host.stream.StreamConfiguration;
import com.google.android.libraries.feed.piet.host.CustomElementProvider;
import com.google.android.libraries.feed.piet.host.HostBindingProvider;
import com.google.android.libraries.feed.testing.modelprovider.FakeModelCursor;
import com.google.common.collect.Lists;
import com.google.search.now.feed.client.StreamDataProto.StreamFeature;
import com.google.search.now.feed.client.StreamDataProto.StreamSharedState;
import com.google.search.now.ui.stream.StreamStructureProto.Card;
import com.google.search.now.ui.stream.StreamStructureProto.Content;
import com.google.search.now.ui.stream.StreamStructureProto.Content.Type;
import com.google.search.now.ui.stream.StreamStructureProto.PietContent;
import com.google.search.now.wire.feed.ContentIdProto.ContentId;
import com.google.search.now.wire.feed.PietSharedStateItemProto.PietSharedStateItem;
import java.util.ArrayList;
import java.util.List;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.robolectric.Robolectric;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

/** Tests for {@link BasicStream}. */
@RunWith(RobolectricTestRunner.class)
public class BasicStreamTest {

  private static final int START_PADDING = 1;
  private static final int END_PADDING = 2;
  private static final int TOP_PADDING = 3;
  private static final int BOTTOM_PADDING = 4;
  private static final PietContent PIET_CONTENT =
      PietContent.newBuilder().addPietSharedStates(ContentId.newBuilder().setId(123L)).build();
  private static final StreamFeature STREAM_FEATURE_WITH_PIET_CONTENT =
      StreamFeature.newBuilder()
          .setContent(
              Content.newBuilder()
                  .setType(Type.PIET)
                  .setExtension(PietContent.pietContentExtension, PIET_CONTENT))
          .build();
  private static final StreamSharedState STREAM_SHARED_STATE =
      StreamSharedState.newBuilder()
          .setPietSharedStateItem(PietSharedStateItem.getDefaultInstance())
          .build();
  @Mock private StreamConfiguration streamConfiguration;
  @Mock private CardConfiguration cardConfiguration;
  @Mock private ImageLoaderApi imageLoaderApi;
  @Mock private ActionParser actionParser;
  @Mock private ActionApi actionApi;
  @Mock private CustomElementProvider customElementProvider;
  @Mock private ModelProviderFactory modelProviderFactory;
  @Mock private ModelProvider initialModelProvider;
  @Mock private ModelProvider modelProvider;

  private final List<View> headers = new ArrayList<>();
  private final ThreadUtils threadUtils = new ThreadUtils();

  private BasicStream basicStream;

  @Before
  public void setUp() {
    initMocks(this);

    when(streamConfiguration.getPaddingStart()).thenReturn(START_PADDING);
    when(streamConfiguration.getPaddingEnd()).thenReturn(END_PADDING);
    when(streamConfiguration.getPaddingTop()).thenReturn(TOP_PADDING);
    when(streamConfiguration.getPaddingBottom()).thenReturn(BOTTOM_PADDING);
    when(modelProviderFactory.createNew()).thenReturn(initialModelProvider, modelProvider);
    when(initialModelProvider.getSharedState(GLOBAL_SHARED_STATE)).thenReturn(STREAM_SHARED_STATE);
    basicStream =
        new BasicStream(
            Robolectric.setupActivity(Activity.class),
            streamConfiguration,
            cardConfiguration,
            imageLoaderApi,
            actionParser,
            actionApi,
            customElementProvider,
            DebugBehavior.VERBOSE,
            threadUtils,
            headers,
            new FakeClock(),
            modelProviderFactory,
            new HostBindingProvider());
    basicStream.onCreate(null);
  }

  @Test
  public void testOnSessionStart() {
    ModelFeature rootFeature = mock(ModelFeature.class);
    when(initialModelProvider.getRootFeature()).thenReturn(rootFeature);

    // Represents final payload which includes Piet content.
    ModelFeature pietModelFeature = mock(ModelFeature.class);
    when(pietModelFeature.getStreamFeature()).thenReturn(STREAM_FEATURE_WITH_PIET_CONTENT);

    // ModelChild containing Piet content.
    ModelChild pietModelChild = mock(ModelChild.class);
    when(pietModelChild.getModelFeature()).thenReturn(pietModelFeature);

    // Cursor to transverse the content of a card, which is Piet content.
    FakeModelCursor cardCursor = new FakeModelCursor(Lists.newArrayList(pietModelChild));

    // A ModelFeature with a Card payload.
    ModelFeature cardModelFeature = mock(ModelFeature.class);
    when(cardModelFeature.getCursor()).thenReturn(cardCursor);
    when(cardModelFeature.getStreamFeature())
        .thenReturn(StreamFeature.newBuilder().setCard(Card.newBuilder()).build());

    // The root child, which contains one card.
    ModelChild rootChild = mock(ModelChild.class);
    when(rootChild.getModelFeature()).thenReturn(cardModelFeature);
    when(rootChild.getType()).thenReturn(ModelChild.Type.FEATURE);

    // The cursor for the root feature
    FakeModelCursor rootCursor = new FakeModelCursor(Lists.newArrayList(rootChild));
    when(rootFeature.getCursor()).thenReturn(rootCursor);
    basicStream.onSessionStart();

    List<ContentModel> contentModels = basicStream.getAdapter().getContentModels();

    assertThat(contentModels).hasSize(1);
    assertThat(contentModels.get(0)).isInstanceOf(PietContentModel.class);
    assertThat(((PietContentModel) contentModels.get(0)).getPietContent()).isEqualTo(PIET_CONTENT);
  }

  @Test
  public void testOnSessionFinished() {
    basicStream.onSessionFinished();

    verify(initialModelProvider).unregisterObserver(basicStream);
    verify(modelProviderFactory, times(2)).createNew();
    verify(modelProvider).registerObserver(basicStream);
  }

  @Test
  @Config(sdk = VERSION_CODES.JELLY_BEAN)
  public void testPadding_jellyBean() {
    // Padding is setup in constructor.
    View view = basicStream.getView();

    assertThat(view.getPaddingLeft()).isEqualTo(START_PADDING);
    assertThat(view.getPaddingRight()).isEqualTo(END_PADDING);
    assertThat(view.getPaddingTop()).isEqualTo(TOP_PADDING);
    assertThat(view.getPaddingBottom()).isEqualTo(BOTTOM_PADDING);
  }

  @Test
  @Config(sdk = VERSION_CODES.KITKAT)
  public void testPadding_kitKat() {
    // Padding is setup in constructor.
    View view = basicStream.getView();

    assertThat(view.getPaddingStart()).isEqualTo(START_PADDING);
    assertThat(view.getPaddingEnd()).isEqualTo(END_PADDING);
    assertThat(view.getPaddingTop()).isEqualTo(TOP_PADDING);
    assertThat(view.getPaddingBottom()).isEqualTo(BOTTOM_PADDING);
  }
}
