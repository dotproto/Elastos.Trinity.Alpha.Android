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

import android.support.v7.widget.RecyclerView.ViewHolder;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import com.google.android.libraries.feed.host.stream.CardConfiguration;
import com.google.android.libraries.feed.piet.FrameAdapter;
import com.google.search.now.ui.piet.PietAndroidSupport.ShardingControl;
import com.google.search.now.ui.piet.PietProto.Frame;
import com.google.search.now.ui.piet.PietProto.PietSharedState;
import java.util.List;

/**
 * {@link ViewHolder} for {@link com.google.search.now.ui.stream.StreamStructureProto.PietContent}.
 */
public class PietViewHolder extends ViewHolder {

  private final CardConfiguration cardConfiguration;
  private final FrameLayout cardView;
  private final FrameAdapter frameAdapter;
  private boolean bound;

  public PietViewHolder(
      CardConfiguration cardConfiguration, FrameLayout cardView, FrameAdapter frameAdapter) {
    super(cardView);
    this.cardConfiguration = cardConfiguration;
    this.cardView = cardView;
    this.frameAdapter = frameAdapter;
  }

  public void bind(Frame frame, List<PietSharedState> pietSharedStates) {
    if (bound) {
      return;
    }

    // Need to reset padding here.  Setting a background can affect padding so if we switch from
    // a background which has padding to one that does not, then the padding needs to be removed.
    cardView.setPadding(0, 0, 0, 0);
    cardView.setBackground(cardConfiguration.getCardBackground());
    LinearLayout.LayoutParams lp =
        new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
    lp.bottomMargin = (int) cardConfiguration.getCardBottomPadding();
    cardView.setLayoutParams(lp);

    frameAdapter.bindModel(frame, (ShardingControl) null, pietSharedStates);
    bound = true;
  }

  public void unbind() {
    if (!bound) {
      return;
    }
    frameAdapter.unbindModel();
    bound = false;
  }
}
