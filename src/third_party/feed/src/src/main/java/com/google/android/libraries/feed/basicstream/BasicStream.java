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

import static com.google.android.libraries.feed.common.Validators.checkNotNull;

import android.content.Context;
import android.os.Build.VERSION;
import android.os.Build.VERSION_CODES;
import android.os.Bundle;
import android.support.annotation.VisibleForTesting;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.View;
import com.google.android.libraries.feed.api.actionparser.ActionParser;
import com.google.android.libraries.feed.api.common.ThreadUtils;
import com.google.android.libraries.feed.api.modelprovider.ModelProvider;
import com.google.android.libraries.feed.api.modelprovider.ModelProviderFactory;
import com.google.android.libraries.feed.api.modelprovider.ModelProviderObserver;
import com.google.android.libraries.feed.api.modelprovider.ModelToken;
import com.google.android.libraries.feed.api.stream.ContentChangedListener;
import com.google.android.libraries.feed.api.stream.ScrollListener;
import com.google.android.libraries.feed.api.stream.Stream;
import com.google.android.libraries.feed.basicstream.internal.StreamActionApiImpl;
import com.google.android.libraries.feed.basicstream.internal.StreamRecyclerViewAdapter;
import com.google.android.libraries.feed.basicstream.internal.contentmodels.ContentModel;
import com.google.android.libraries.feed.basicstream.internal.drivers.StreamDriver;
import com.google.android.libraries.feed.basicstream.internal.piet.PietAssetProvider;
import com.google.android.libraries.feed.basicstream.internal.piet.PietCustomElementProvider;
import com.google.android.libraries.feed.basicstream.internal.piet.PietHostBindingProvider;
import com.google.android.libraries.feed.basicstream.internal.util.TokenHandler;
import com.google.android.libraries.feed.common.logging.Logger;
import com.google.android.libraries.feed.common.time.Clock;
import com.google.android.libraries.feed.host.action.ActionApi;
import com.google.android.libraries.feed.host.config.DebugBehavior;
import com.google.android.libraries.feed.host.imageloader.ImageLoaderApi;
import com.google.android.libraries.feed.host.stream.CardConfiguration;
import com.google.android.libraries.feed.host.stream.StreamConfiguration;
import com.google.android.libraries.feed.piet.PietManager;
import com.google.android.libraries.feed.piet.host.CustomElementProvider;
import com.google.android.libraries.feed.piet.host.HostBindingProvider;
import java.util.List;

/**
 * A basic implementation of a Feed {@link Stream} that is just able to render a vertical stream of
 * cards.
 */
public class BasicStream implements Stream, ModelProviderObserver {

  private static final String TAG = "BasicStream";

  private final RecyclerView recyclerView;
  private final CardConfiguration cardConfiguration;
  private final ThreadUtils threadUtils;
  private final PietManager pietManager;
  private final ModelProviderFactory modelProviderFactory;

  private ModelProvider modelProvider;
  private Context context;
  private List<View> headers;
  private StreamConfiguration streamConfiguration;
  private StreamRecyclerViewAdapter adapter;
  private StreamDriver streamDriver;

  /*@Nullable*/ private StreamTokenHandler streamTokenHandler;

  public BasicStream(
      Context context,
      StreamConfiguration streamConfiguration,
      CardConfiguration cardConfiguration,
      ImageLoaderApi imageLoaderApi,
      ActionParser actionParser,
      ActionApi actionApi,
      /*@Nullable*/ CustomElementProvider customElementProvider,
      DebugBehavior debugBehavior,
      ThreadUtils threadUtils,
      List<View> headers,
      Clock clock,
      ModelProviderFactory modelProviderFactory,
      /*@Nullable*/ HostBindingProvider hostBindingProvider) {
    this.cardConfiguration = cardConfiguration;
    this.threadUtils = threadUtils;
    this.pietManager =
        new PietManager(
            debugBehavior,
            new PietAssetProvider(imageLoaderApi, cardConfiguration, clock),
            (action, frame, view, veLoggingToken) ->
                actionParser.parseAction(
                    action,
                    new StreamActionApiImpl(context, actionApi, actionParser),
                    view,
                    veLoggingToken),
            new PietCustomElementProvider(context, customElementProvider),
            new PietHostBindingProvider(hostBindingProvider));
    recyclerView = new RecyclerView(context);
    recyclerView.setLayoutManager(new LinearLayoutManager(context));
    this.context = context;
    this.headers = headers;
    this.modelProviderFactory = modelProviderFactory;
    this.streamConfiguration = streamConfiguration;
  }

  @Override
  public void onCreate(/*@Nullable*/ Bundle savedInstanceState) {
    // TODO: Implement save/restore.
    adapter = new StreamRecyclerViewAdapter(context, cardConfiguration, pietManager);
    adapter.setHeaders(headers);
    recyclerView.setAdapter(adapter);
    recyclerView.setClipToPadding(false);
    if (VERSION.SDK_INT > VERSION_CODES.JELLY_BEAN) {
      recyclerView.setPaddingRelative(
          streamConfiguration.getPaddingStart(),
          streamConfiguration.getPaddingTop(),
          streamConfiguration.getPaddingEnd(),
          streamConfiguration.getPaddingBottom());
    } else {
      recyclerView.setPadding(
          streamConfiguration.getPaddingStart(),
          streamConfiguration.getPaddingTop(),
          streamConfiguration.getPaddingEnd(),
          streamConfiguration.getPaddingBottom());
    }
    modelProvider = modelProviderFactory.createNew();
    modelProvider.registerObserver(this);
  }

  @Override
  public void onShow() {}

  @Override
  public void onActive() {}

  @Override
  public void onInactive() {}

  @Override
  public void onHide() {}

  @Override
  public void onDestroy() {}

  @Override
  public Bundle getSavedInstanceState() {
    // TODO: Implement save/restore.
    return new Bundle();
  }

  @Override
  public View getView() {
    return recyclerView;
  }

  @VisibleForTesting
  StreamRecyclerViewAdapter getAdapter() {
    return adapter;
  }

  @Override
  public void setHeaderViews(List<View> headers) {
    adapter.setHeaders(headers);
  }

  @Override
  public void setStreamContentVisibility(boolean visible) {}

  @Override
  public void trim() {}

  @Override
  public void smoothScrollBy(int dx, int dy) {}

  @Override
  public int getChildTopAt(int position) {
    return POSITION_NOT_KNOWN;
  }

  @Override
  public boolean isChildAtPositionVisible(int position) {
    return checkNotNull(recyclerView.getLayoutManager())
            .findViewByPosition(position)
            .getVisibility()
        == View.VISIBLE;
  }

  @Override
  public void addScrollListener(ScrollListener listener) {}

  @Override
  public void removeScrollListener(ScrollListener listener) {}

  @Override
  public void addOnContentChangedListener(ContentChangedListener listener) {}

  @Override
  public void removeOnContentChangedListener(ContentChangedListener listener) {}

  @Override
  public void triggerRefresh() {
    // This invalidates the modelProvider, which results in onSessionFinished() then
    // onSessionStart() being called, leading to recreating the entire stream.
    modelProvider.triggerRefresh();
  }

  private void updateAdapter(ModelProvider modelProvider) {
    if (streamTokenHandler != null) {
      // Prevents a previous StreamDriver from successfully interacting with the TokenHandler
      streamTokenHandler.disable();
      streamTokenHandler = null;
    }

    streamTokenHandler = new StreamTokenHandler();
    streamDriver = new StreamDriver(modelProvider, threadUtils, streamTokenHandler);

    List<ContentModel> contentModels = streamDriver.getContentModels();

    Logger.i(TAG, "updateAdapter - New Features: %s", contentModels.size());
    adapter.setContentModels(contentModels);
  }

  @Override
  public void onSessionStart() {
    threadUtils.checkMainThread();
    updateAdapter(modelProvider);
  }

  @Override
  public void onSessionFinished() {
    modelProvider.unregisterObserver(this);
    modelProvider = modelProviderFactory.createNew();
    modelProvider.registerObserver(this);
  }

  @Override
  public void onRootSet() {
    // TODO: Handle onRootSet
  }

  private class StreamTokenHandler implements TokenHandler {

    private boolean enabled;

    StreamTokenHandler() {
      enabled = true;
    }

    @Override
    public void tokenConsumed() {
      if (enabled) {
        adapter.setContentModels(streamDriver.getContentModels());
      }
    }

    @Override
    public void requestTokenHandling(ModelToken token) {
      if (enabled) {
        modelProvider.handleToken(token);
      }
    }

    public void disable() {
      enabled = true;
    }
  }
}
