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

import static com.google.android.libraries.feed.api.sessionmanager.SessionManager.GLOBAL_SHARED_STATE;
import static com.google.common.truth.Truth.assertThat;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.verifyNoMoreInteractions;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;

import com.google.android.libraries.feed.api.modelprovider.ModelFeature;
import com.google.android.libraries.feed.api.modelprovider.ModelProvider;
import com.google.android.libraries.feed.basicstream.internal.contentmodels.ContentModel;
import com.google.android.libraries.feed.basicstream.internal.contentmodels.PietContentModel;
import com.google.search.now.feed.client.StreamDataProto.StreamFeature;
import com.google.search.now.feed.client.StreamDataProto.StreamSharedState;
import com.google.search.now.ui.piet.PietProto.Frame;
import com.google.search.now.ui.stream.StreamStructureProto.Content;
import com.google.search.now.ui.stream.StreamStructureProto.Content.Type;
import com.google.search.now.ui.stream.StreamStructureProto.PietContent;
import com.google.search.now.wire.feed.PietSharedStateItemProto.PietSharedStateItem;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.robolectric.RobolectricTestRunner;

/** Tests for {@link ContentDriver}. */
@RunWith(RobolectricTestRunner.class)
public class ContentDriverTest {

  private static final PietContent PIET_CONTENT =
      PietContent.newBuilder().setFrame(Frame.newBuilder().setStylesheetId("id")).build();

  private static final StreamFeature STREAM_FEATURE =
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

  @Mock private ModelFeature modelFeature;
  @Mock private ModelProvider modelProvider;

  private ContentDriver contentDriver;

  @Before
  public void setup() {
    initMocks(this);
    when(modelFeature.getStreamFeature()).thenReturn(STREAM_FEATURE);
    when(modelProvider.getSharedState(GLOBAL_SHARED_STATE)).thenReturn(STREAM_SHARED_STATE);
    contentDriver = new ContentDriver(modelFeature, modelProvider);
  }

  @Test
  public void testGetContentModel() {
    ContentModel contentModel = contentDriver.getContentModel();

    assertThat(contentModel).isInstanceOf(PietContentModel.class);

    assertThat(((PietContentModel) contentModel).getPietContent()).isEqualTo(PIET_CONTENT);
    verify(modelProvider).getSharedState(GLOBAL_SHARED_STATE);
  }

  @Test
  public void testGetContentModel_sharedStateItemIsNull() {
    when(modelProvider.getSharedState(GLOBAL_SHARED_STATE)).thenReturn(null);
    ContentModel contentModel = contentDriver.getContentModel();

    assertThat(contentModel).isNull();
    verify(modelProvider).getSharedState(GLOBAL_SHARED_STATE);
  }

  @Test
  public void testGetContentModel_reusesPreviousContentModel() {
    ContentModel contentModel1 = contentDriver.getContentModel();

    verify(modelFeature).getStreamFeature();

    ContentModel contentModel2 = contentDriver.getContentModel();

    verifyNoMoreInteractions(modelFeature);

    assertThat(contentModel1).isSameAs(contentModel2);
  }
}
