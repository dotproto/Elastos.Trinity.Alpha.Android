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

import android.content.Context;
import android.util.DisplayMetrics;
import android.util.TypedValue;
import android.view.View;
import com.google.android.libraries.feed.piet.host.ActionHandler;
import com.google.search.now.ui.piet.ActionsProto.Actions;
import com.google.search.now.ui.piet.BindingRefsProto.ActionsBindingRef;

/** Utility class, providing useful methods to interact with Views. */
public class ViewUtils {
  private static final String TAG = "ViewUtils";

  /** Convert DP to PX */
  public static float dpToPx(float dp, Context context) {
    DisplayMetrics metrics = context.getResources().getDisplayMetrics();
    return TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, dp, metrics);
  }

  /** Attaches the onClick action from actions to the view, executed by the handler. */
  static void setOnClickActions(
      Actions actions, View view, ActionHandler handler, FrameContext frameContext) {
    if (actions.hasOnLongClickAction()) {
      view.setOnLongClickListener(
          v -> {
            handler.handleAction(
                actions.getOnLongClickAction(), frameContext.getFrame(), view, null);
            return true;
          });
    } else {
      clearOnLongClickActions(view);
    }
    if (actions.hasOnClickAction()) {
      view.setOnClickListener(
          v -> {
            handler.handleAction(actions.getOnClickAction(), frameContext.getFrame(), view, null);
          });
    } else {
      clearOnClickActions(view);
    }
  }

  /**
   * Attaches the onClick action bound by actionsBindingRef to the view, executed by the handler.
   */
  static void setOnClickActions(
      /*@Nullable*/ ActionsBindingRef actionsBindingRef,
      View view,
      ActionHandler handler,
      FrameContext frameContext) {
    if (actionsBindingRef == null) {
      clearOnClickActions(view);
      clearOnLongClickActions(view);
      return;
    }
    Actions actions = frameContext.getActionsFromBinding(actionsBindingRef);
    setOnClickActions(actions, view, handler, frameContext);
  }

  static void clearOnLongClickActions(View view) {
    view.setOnLongClickListener(null);
    view.setLongClickable(false);
  }

  /** Sets clickability to false. */
  static void clearOnClickActions(View view) {
    if (view.hasOnClickListeners()) {
      view.setOnClickListener(null);
    }

    view.setClickable(false);
  }

  /** Private constructor to prevent instantiation. */
  private ViewUtils() {}
}
