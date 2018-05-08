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

package com.google.android.libraries.feed.basicstream.internal;

import static com.google.android.libraries.feed.basicstream.internal.viewholders.ViewHolderType.TYPE_CONTINUATION;
import static com.google.android.libraries.feed.basicstream.internal.viewholders.ViewHolderType.TYPE_HEADER;

import android.content.Context;
import android.support.annotation.VisibleForTesting;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.RecyclerView.ViewHolder;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.widget.FrameLayout;
import com.google.android.libraries.feed.basicstream.internal.contentmodels.ContentModel;
import com.google.android.libraries.feed.basicstream.internal.viewholders.ContinuationViewHolder;
import com.google.android.libraries.feed.basicstream.internal.viewholders.PietViewHolder;
import com.google.android.libraries.feed.common.logging.Logger;
import com.google.android.libraries.feed.host.stream.CardConfiguration;
import com.google.android.libraries.feed.piet.FrameAdapter;
import com.google.android.libraries.feed.piet.PietManager;
import java.util.ArrayList;
import java.util.List;

/** A RecyclerView adapter which can show a list of views with Piet Stream features. */
public class StreamRecyclerViewAdapter extends RecyclerView.Adapter<ViewHolder> {
  private static final String TAG = "StreamRecyclerViewAdapt";

  private final Context context;
  private final CardConfiguration cardConfiguration;
  private final PietManager pietManager;
  // TODO: convert to ModelChild so the adapter can properly add a spinner
  private final List<ContentModel> contentModels;
  private final List<View> headers;

  // Suppress initialization warnings for calling setHasStableIds on RecyclerView.Adapter
  @SuppressWarnings("initialization")
  public StreamRecyclerViewAdapter(
      Context context, CardConfiguration cardConfiguration, PietManager pietManager) {
    this.context = context;
    this.cardConfiguration = cardConfiguration;
    this.pietManager = pietManager;
    headers = new ArrayList<>();
    contentModels = new ArrayList<>();
    setHasStableIds(true);
  }

  @Override
  public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
    if (viewType == TYPE_HEADER) {
      FrameLayout frameLayout = new FrameLayout(parent.getContext());
      frameLayout.setLayoutParams(
          new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
      return new HeaderViewHolder(frameLayout);
    } else if (viewType == TYPE_CONTINUATION) {
      FrameLayout frameLayout = new FrameLayout(parent.getContext());
      frameLayout.setLayoutParams(
          new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT));
      return new ContinuationViewHolder(parent.getContext(), frameLayout);
    }

    FrameLayout cardView = new FrameLayout(context);
    FrameAdapter frameAdapter = pietManager.createPietFrameAdapter(() -> cardView, context);
    cardView.addView(frameAdapter.getFrameContainer());
    return new PietViewHolder(cardConfiguration, cardView, frameAdapter);
  }

  @Override
  public void onBindViewHolder(ViewHolder viewHolder, int i) {
    if (isHeader(i)) {
      injectHeader((HeaderViewHolder) viewHolder, i);
      return;
    }
    Logger.d(TAG, "onBindViewHolder %s", i);
    ContentModel contentModel = contentModels.get(positionToStreamIndex(i));
    contentModel.bind(viewHolder);
  }

  private void injectHeader(HeaderViewHolder headerViewHolder, int position) {
    View header = headers.get(position);
    if (header.getParent() == null) {
      headerViewHolder.frameLayout.addView(header);
    }
  }

  @Override
  public void onViewRecycled(ViewHolder viewHolder) {
    // TODO: Make custom ViewHolder descendent that has an unbind() to avoid code
    // duplication.
    // TODO: Create ViewHolders and ContentModels for headers.
    if (viewHolder instanceof HeaderViewHolder) {
      Logger.d(TAG, "onViewRecycled - HeaderViewHolder");
      ((HeaderViewHolder) viewHolder).frameLayout.removeAllViews();

    } else if (viewHolder instanceof ContinuationViewHolder) {
      Logger.d(TAG, "onViewRecycled - ContinuationViewHolder");
      ((ContinuationViewHolder) viewHolder).unbind();
    } else {
      Logger.d(TAG, "onViewRecycled - PietViewHolder");
      ((PietViewHolder) viewHolder).unbind();
    }
  }

  @Override
  public int getItemCount() {
    return contentModels.size() + headers.size();
  }

  @Override
  public int getItemViewType(int position) {
    // TODO: Create ViewHolders and ContentModels for headers.
    if (isHeader(position)) {
      return TYPE_HEADER;
    }

    return contentModels.get(positionToStreamIndex(position)).getItemViewType();
  }

  @Override
  public long getItemId(int position) {
    if (isHeader(position)) {
      return headers.get(position).hashCode();
    }

    return contentModels.get(positionToStreamIndex(position)).itemId();
  }

  @VisibleForTesting
  public List<ContentModel> getContentModels() {
    return contentModels;
  }

  private boolean isHeader(int position) {
    return position < headers.size();
  }

  private int positionToStreamIndex(int position) {
    return position - headers.size();
  }

  public void setHeaders(List<View> newHeaders) {
    headers.clear();
    headers.addAll(newHeaders);
    notifyDataSetChanged();
  }

  public void setContentModels(List<ContentModel> newContentModels) {
    contentModels.clear();
    addContentModels(newContentModels);
  }

  public void addContentModels(List<ContentModel> newContentModels) {
    contentModels.addAll(newContentModels);
    notifyDataSetChanged();
  }

  static class HeaderViewHolder extends ViewHolder {

    private final FrameLayout frameLayout;

    public HeaderViewHolder(FrameLayout itemView) {
      super(itemView);
      this.frameLayout = itemView;
    }
  }

}
