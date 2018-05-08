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

package com.google.android.libraries.feed.basicstream.internal.contentmodel;

import static com.google.android.libraries.feed.basicstream.internal.viewholders.ViewHolderType.TYPE_CARD;
import static com.google.common.truth.Truth.assertThat;
import static org.mockito.Mockito.verify;
import static org.mockito.MockitoAnnotations.initMocks;

import com.google.android.libraries.feed.basicstream.internal.contentmodels.PietContentModel;
import com.google.android.libraries.feed.basicstream.internal.viewholders.PietViewHolder;
import com.google.search.now.ui.piet.PietProto.Frame;
import com.google.search.now.ui.piet.PietProto.PietSharedState;
import com.google.search.now.ui.stream.StreamStructureProto.PietContent;
import java.util.ArrayList;
import java.util.List;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.robolectric.RobolectricTestRunner;

/** Tests for {@link PietContentModel}. */
@RunWith(RobolectricTestRunner.class)
public class PietContentModelTest {
  private static final String TEST_ID = "TEST_ID";

  PietContent pietContent =
      PietContent.newBuilder().setFrame(Frame.newBuilder().setStylesheetId(TEST_ID)).build();
  List<PietSharedState> pietSharedStates = new ArrayList<>();

  @Mock PietViewHolder pietViewHolder;

  PietContentModel pietContentModel;

  @Before
  public void setUp() {
    initMocks(this);
    pietContentModel = new PietContentModel(pietSharedStates, pietContent);
  }

  @Test
  public void testBind() {
    pietContentModel.bind(pietViewHolder);

    verify(pietViewHolder).bind(pietContent.getFrame(), pietSharedStates);
  }

  @Test
  public void testUnbind() {
    pietContentModel.unbind(pietViewHolder);

    verify(pietViewHolder).unbind();
  }

  @Test
  public void testItemId() {
    assertThat(pietContentModel.itemId()).isEqualTo(pietContent.hashCode());
  }

  @Test
  public void testGetItemViewType() {
    assertThat(pietContentModel.getItemViewType()).isEqualTo(TYPE_CARD);
  }
}
