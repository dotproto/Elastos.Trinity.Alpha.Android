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

package com.google.android.libraries.feed.basicstream.internal.viewholders;

import static com.google.common.truth.Truth.assertThat;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.verifyNoMoreInteractions;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;

import android.app.Activity;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.view.ViewGroup.MarginLayoutParams;
import android.widget.FrameLayout;
import com.google.android.libraries.feed.host.stream.CardConfiguration;
import com.google.android.libraries.feed.piet.FrameAdapter;
import com.google.search.now.ui.piet.PietProto.Frame;
import com.google.search.now.ui.piet.PietProto.PietSharedState;
import java.util.ArrayList;
import java.util.List;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.InOrder;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.robolectric.Robolectric;
import org.robolectric.RobolectricTestRunner;

/** Tests for {@link PietViewHolder}. */
@RunWith(RobolectricTestRunner.class)
public class PietViewHolderTest {
  private static final int CARD_BOTTOM_PADDING = 42;

  @Mock private CardConfiguration cardConfiguration;
  @Mock private FrameAdapter frameAdapter;

  private PietViewHolder pietViewHolder;
  private FrameLayout frameLayout;
  private final List<PietSharedState> pietSharedStates = new ArrayList<>();

  @Before
  public void setUp() {
    initMocks(this);
    frameLayout = new FrameLayout(Robolectric.setupActivity(Activity.class));
  }

  @Test
  public void testBind_clearsPadding() {
    frameLayout.setPadding(1, 2, 3, 4);

    pietViewHolder = new PietViewHolder(cardConfiguration, frameLayout, frameAdapter);
    pietViewHolder.bind(Frame.getDefaultInstance(), pietSharedStates);

    assertThat(frameLayout.getPaddingLeft()).isEqualTo(0);
    assertThat(frameLayout.getPaddingRight()).isEqualTo(0);
    assertThat(frameLayout.getPaddingTop()).isEqualTo(0);
    assertThat(frameLayout.getPaddingBottom()).isEqualTo(0);
  }

  @Test
  public void testBind_setsBackground() {
    ColorDrawable redBackground = new ColorDrawable(Color.RED);
    when(cardConfiguration.getCardBackground()).thenReturn(redBackground);

    pietViewHolder = new PietViewHolder(cardConfiguration, frameLayout, frameAdapter);
    pietViewHolder.bind(Frame.getDefaultInstance(), pietSharedStates);

    assertThat(frameLayout.getBackground()).isEqualTo(redBackground);
  }

  @Test
  public void testBind_setsTheming() {
    when(cardConfiguration.getCardBottomPadding()).thenReturn((float) CARD_BOTTOM_PADDING);

    pietViewHolder = new PietViewHolder(cardConfiguration, frameLayout, frameAdapter);
    pietViewHolder.bind(Frame.getDefaultInstance(), pietSharedStates);

    assertThat(((MarginLayoutParams) pietViewHolder.itemView.getLayoutParams()).bottomMargin)
        .isEqualTo(CARD_BOTTOM_PADDING);
  }

  @Test
  public void testBind_bindsModel() {
    pietViewHolder = new PietViewHolder(cardConfiguration, frameLayout, frameAdapter);
    pietViewHolder.bind(Frame.getDefaultInstance(), pietSharedStates);

    verify(frameAdapter)
        .bindModel(Frame.getDefaultInstance(), /* shardingControl= */ null, pietSharedStates);
  }

  @Test
  public void testBind_onlyBindsOnce() {
    pietViewHolder = new PietViewHolder(cardConfiguration, frameLayout, frameAdapter);
    pietViewHolder.bind(Frame.getDefaultInstance(), pietSharedStates);

    verify(frameAdapter)
        .bindModel(Frame.getDefaultInstance(), /* shardingControl= */ null, pietSharedStates);

    pietViewHolder.bind(Frame.getDefaultInstance(), pietSharedStates);

    verifyNoMoreInteractions(frameAdapter);
  }

  @Test
  public void testUnbind() {
    pietViewHolder = new PietViewHolder(cardConfiguration, frameLayout, frameAdapter);
    pietViewHolder.bind(Frame.getDefaultInstance(), pietSharedStates);

    pietViewHolder.unbind();
    InOrder inOrder = Mockito.inOrder(frameAdapter);

    inOrder
        .verify(frameAdapter)
        .bindModel(Frame.getDefaultInstance(), /* shardingControl= */ null, pietSharedStates);
    inOrder.verify(frameAdapter).unbindModel();
    inOrder.verifyNoMoreInteractions();
  }
}
