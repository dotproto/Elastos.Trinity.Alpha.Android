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

package com.google.android.libraries.feed.piet;

import static com.google.android.libraries.feed.common.Validators.checkNotNull;
import static com.google.android.libraries.feed.common.Validators.checkState;

import android.content.Context;
import android.support.annotation.VisibleForTesting;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.widget.LinearLayout;
import com.google.android.libraries.feed.piet.AdapterFactory.AdapterKeySupplier;
import com.google.search.now.ui.piet.ElementsProto.BindingContext;
import com.google.search.now.ui.piet.ElementsProto.Element;
import com.google.search.now.ui.piet.ElementsProto.ElementList;
import com.google.search.now.ui.piet.ElementsProto.TemplateInvocation;
import com.google.search.now.ui.piet.PietAndroidSupport.ShardingControl;
import com.google.search.now.ui.piet.PietProto.PietSharedState;
import com.google.search.now.ui.piet.PietProto.Template;
import java.util.List;

/** A {@link ElementContainerAdapter} which manages {@code TemplateInvocation} slices. */
class TemplateInvocationAdapter
    extends ElementContainerAdapter<LinearLayout, ElementListAdapter, TemplateInvocation> {

  private static final String TAG = "TemplateInvocationAdapter";

  /*@Nullable*/ private Template template = null;
  /*@Nullable*/ private List<PietSharedState> pietSharedStates = null;

  TemplateInvocationAdapter(Context context, AdapterParameters parameters) {
    super(context, parameters, createView(context));
  }

  @Override
  TemplateInvocation getModelFromElement(Element baseElement) {
    throw new IllegalArgumentException(
        "Templates must be re-bound; cannot be extracted from an Element.");
  }

  @Override
  public void onCreateAdapter(
      TemplateInvocation model, Element baseElement, FrameContext frameContext) {
    Template modelTemplate =
        checkNotNull(
            frameContext.getTemplate(model.getTemplateId()),
            "Template was not found: %s",
            model.getTemplateId());

    // Adapter has already been created.
    if (template != null) {
      if (!templateEquals(template, modelTemplate)) {
        throw new IllegalArgumentException(
            "Adapter was constructed already with different template");
      } else {
        // We're recycling something with a compatible template; return without doing anything.
        return;
      }
    }

    template = modelTemplate;
    pietSharedStates = frameContext.getPietSharedStates();
    setKey(createKey());

    // TODO: Need to support Sharding.
    if (model.getBindingContextsCount() > 0 && modelTemplate.hasElementList()) {
      ViewGroup viewGroup = checkNotNull(getBaseView());
      ElementListAdapter listAdapter = null;
      for (BindingContext bindingContext : model.getBindingContextsList()) {
        // Create an ElementListAdapter for each of the bindingContexts - this adapter holds a list
        // of lists
        FrameContext localFrameContext = frameContext.bindTemplate(modelTemplate, bindingContext);
        listAdapter = getAdapterForChildList(modelTemplate.getElementList(), localFrameContext);
        setLayoutParamsOnChild(listAdapter);
        View listView = listAdapter.getView();
        if (listView != null) {
          viewGroup.addView(listView);
        }
      }
    }
  }

  @Override
  void onBindModel(TemplateInvocation model, Element baseElement, FrameContext frameContext) {
    onBindModel(model, frameContext, null);
  }

  void onBindModel(
      TemplateInvocation model,
      FrameContext frameContext,
      /*@Nullable*/ ShardingControl shardingControl) {
    Template modelTemplate =
        checkNotNull(
            frameContext.getTemplate(model.getTemplateId()),
            "Template was not found: %s",
            model.getTemplateId());
    if (!templateEquals(modelTemplate, template)) {
      throw new IllegalArgumentException("Model template does not match adapter template");
    }

    // TODO: Need to support Sharding.
    if (model.getBindingContextsCount() > 0 && modelTemplate.hasElementList()) {
      checkState(
          model.getBindingContextsCount() == childAdapters.size(),
          "Number of binding contexts mismatch: got %s bcs, had %s adapters",
          model.getBindingContextsCount(),
          childAdapters.size());
      for (int i = 0; i < model.getBindingContextsCount(); i++) {
        BindingContext bindingContext = model.getBindingContexts(i);
        FrameContext localFrameContext = frameContext.bindTemplate(modelTemplate, bindingContext);
        childAdapters.get(i).bindModel(modelTemplate.getElementList(), localFrameContext);
      }
    }
  }

  @Override
  public void releaseAdapter() {
    // Because we recycle templates as a unit, we don't actually want to do anything here - we want
    // to keep the existing layout and styles so we can quickly re-bind content to the template.
  }

  /** Create an Adapter for the {@code ElementList}. */
  private ElementListAdapter getAdapterForChildList(ElementList list, FrameContext frameContext) {
    ElementListAdapter listAdapter =
        getParameters().elementAdapterFactory.createElementListAdapter(list, frameContext);
    listAdapter.createAdapter(list, frameContext);
    addChildAdapter(listAdapter);
    return listAdapter;
  }

  @VisibleForTesting
  static LinearLayout createView(Context context) {
    LinearLayout viewGroup = new LinearLayout(context);
    viewGroup.setOrientation(LinearLayout.VERTICAL);
    viewGroup.setLayoutParams(
        new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT));
    return viewGroup;
  }

  private RecyclerKey createKey() {
    return new TemplateKey(
        checkNotNull(template, "Template is null; not created yet?"), pietSharedStates);
  }

  private void setLayoutParamsOnChild(ElementAdapter<?, ?> childAdapter) {
    int width = childAdapter.getComputedWidthPx();
    width = width == ElementAdapter.DIMENSION_NOT_SET ? LayoutParams.MATCH_PARENT : width;
    int height = childAdapter.getComputedHeightPx();
    height = height == ElementAdapter.DIMENSION_NOT_SET ? LayoutParams.WRAP_CONTENT : height;

    childAdapter.setLayoutParams(new LinearLayout.LayoutParams(width, height));
  }

  /**
   * Determines whether two templates are compatible for recycling. We're going to call the hash
   * code good enough for performance reasons (.equals() is expensive), and hope we don't get a lot
   * of collisions.
   */
  @SuppressWarnings("ReferenceEquality")
  static boolean templateEquals(/*@Nullable*/ Template template1, /*@Nullable*/ Template template2) {
    if (template1 == template2) {
      return true;
    } else if (template1 == null || template2 == null) {
      return false;
    }
    return template1.hashCode() == template2.hashCode();
  }

  static class TemplateKeySupplier
      implements AdapterKeySupplier<TemplateInvocationAdapter, TemplateInvocation> {
    @Override
    public String getAdapterTag() {
      return TAG;
    }

    @Override
    public TemplateInvocationAdapter getAdapter(Context context, AdapterParameters parameters) {
      return new TemplateInvocationAdapter(context, parameters);
    }

    @Override
    public RecyclerKey getKey(FrameContext frameContext, TemplateInvocation model) {
      Template template =
          checkNotNull(
              frameContext.getTemplate(model.getTemplateId()),
              "Template was not found: ",
              model.getTemplateId());
      return new TemplateKey(template, frameContext.getPietSharedStates());
    }
  }

  /** Wrap the Template proto object as the recycler key. */
  static class TemplateKey extends RecyclerKey {
    private final Template template;
    /*@Nullable*/ private final List<PietSharedState> pietSharedStates;

    TemplateKey(Template template, /*@Nullable*/ List<PietSharedState> pietSharedStates) {
      this.template = template;
      this.pietSharedStates = pietSharedStates;
    }

    /** Equals checks the hashCode of template and sharedState to avoid expensive proto equals. */
    @SuppressWarnings("ReferenceEquality")
    @Override
    public boolean equals(/*@Nullable*/ Object o) {
      if (this == o) {
        return true;
      }
      if (o == null || getClass() != o.getClass()) {
        return false;
      }

      TemplateKey that = (TemplateKey) o;

      if (!templateEquals(template, that.template)) {
        return false;
      }
      if (that.pietSharedStates == null || this.pietSharedStates == null) {
        return this.pietSharedStates == that.pietSharedStates;
      } else {
        if (this.pietSharedStates.size() != that.pietSharedStates.size()) {
          return false;
        }
        return this.pietSharedStates.hashCode() == that.pietSharedStates.hashCode();
      }
    }

    @Override
    public int hashCode() {
      int result = template.hashCode();
      result = 31 * result + (pietSharedStates != null ? pietSharedStates.hashCode() : 0);
      return result;
    }
  }
}
