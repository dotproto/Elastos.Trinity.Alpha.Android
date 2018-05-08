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

package com.google.android.libraries.feed.basicstream.internal.contentmodels;

import static com.google.android.libraries.feed.basicstream.internal.viewholders.ViewHolderType.TYPE_CARD;

import android.support.annotation.VisibleForTesting;
import android.support.v7.widget.RecyclerView.ViewHolder;
import com.google.android.libraries.feed.basicstream.internal.viewholders.PietViewHolder;
import com.google.search.now.ui.piet.PietProto.PietSharedState;
import com.google.search.now.ui.stream.StreamStructureProto.PietContent;
import java.util.List;

/** {@link ContentModel} to render {@link PietContent} in a RecyclerView. */
public class PietContentModel implements ContentModel {

  private final PietContent pietContent;
  private final List<PietSharedState> pietSharedStates;

  public PietContentModel(List<PietSharedState> pietSharedStates, PietContent pietContent) {
    this.pietSharedStates = pietSharedStates;
    this.pietContent = pietContent;
  }

  @Override
  public void bind(ViewHolder viewHolder) {
    ((PietViewHolder) viewHolder).bind(pietContent.getFrame(), pietSharedStates);
  }

  @Override
  public void unbind(ViewHolder viewHolder) {
    ((PietViewHolder) viewHolder).unbind();
  }

  @Override
  public int itemId() {
    return pietContent.hashCode();
  }

  @Override
  public int getItemViewType() {
    return TYPE_CARD;
  }

  @VisibleForTesting
  public PietContent getPietContent() {
    return pietContent;
  }
}
