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

import static com.google.common.truth.Truth.assertThat;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;

import android.content.Context;
import android.view.View;
import com.google.android.libraries.feed.piet.host.ActionHandler;
import com.google.search.now.ui.piet.ActionsProto.Action;
import com.google.search.now.ui.piet.ActionsProto.Actions;
import com.google.search.now.ui.piet.BindingRefsProto.ActionsBindingRef;
import com.google.search.now.ui.piet.PietProto.Frame;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.RuntimeEnvironment;

/** Tests of the {@link ViewUtils}. */
@RunWith(RobolectricTestRunner.class)
public class ViewUtilsTest {
  private final Context context = RuntimeEnvironment.application;

  private static final Frame DEFAULT_FRAME = Frame.newBuilder().setTag("Frame").build();
  private static final Action DEFAULT_ACTION = Action.getDefaultInstance();
  private static final Actions DEFAULT_ACTIONS =
      Actions.newBuilder().setOnClickAction(DEFAULT_ACTION).build();
  private static final Actions LONG_CLICK_ACTIONS =
      Actions.newBuilder().setOnLongClickAction(DEFAULT_ACTION).build();
  @Mock private ActionHandler mockActionHandler;
  @Mock private FrameContext mockFrameContext;
  @Mock private View.OnClickListener mockListener;
  @Mock private View.OnLongClickListener mockLongClickListener;

  private final View view = new View(context);

  @Before
  public void setUp() {
    initMocks(this);
    when(mockFrameContext.getFrame()).thenReturn(DEFAULT_FRAME);
  }

  @Test
  public void testDpToPx() {
    assertThat(ViewUtils.dpToPx(1000.0f, context)).isEqualTo(1000.0f);
  }

  @Test
  public void testsetOnClickActions_success() {
    ViewUtils.setOnClickActions(DEFAULT_ACTIONS, view, mockActionHandler, mockFrameContext);

    assertThat(view.hasOnClickListeners()).isTrue();

    view.callOnClick();
    verify(mockActionHandler).handleAction(DEFAULT_ACTION, DEFAULT_FRAME, view, null);
  }

  @Test
  public void testSetOnLongClickActionActions_success() {
    ViewUtils.setOnClickActions(LONG_CLICK_ACTIONS, view, mockActionHandler, mockFrameContext);

    view.performLongClick();
    verify(mockActionHandler).handleAction(DEFAULT_ACTION, DEFAULT_FRAME, view, null);
  }

  @Test
  public void testsetOnClickActions_successBinding() {
    ActionsBindingRef actionsBinding =
        ActionsBindingRef.newBuilder().setBindingId("ACTION!").build();
    when(mockFrameContext.getActionsFromBinding(actionsBinding)).thenReturn(DEFAULT_ACTIONS);

    ViewUtils.setOnClickActions(actionsBinding, view, mockActionHandler, mockFrameContext);

    assertThat(view.hasOnClickListeners()).isTrue();

    view.callOnClick();
    verify(mockActionHandler).handleAction(DEFAULT_ACTION, DEFAULT_FRAME, view, null);
  }

  @Test
  public void testSetOnLongClickActionActions_successBinding() {
    ActionsBindingRef actionsBinding =
        ActionsBindingRef.newBuilder().setBindingId("ACTION!").build();
    when(mockFrameContext.getActionsFromBinding(actionsBinding)).thenReturn(LONG_CLICK_ACTIONS);

    ViewUtils.setOnClickActions(actionsBinding, view, mockActionHandler, mockFrameContext);

    view.performLongClick();
    verify(mockActionHandler).handleAction(DEFAULT_ACTION, DEFAULT_FRAME, view, null);
  }

  @Test
  public void testsetOnClickActions_nullActionsBindingClearsActions() {
    view.setOnClickListener(mockListener);
    assertThat(view.hasOnClickListeners()).isTrue();

    ViewUtils.setOnClickActions(
        (ActionsBindingRef) null, view, mockActionHandler, mockFrameContext);

    assertViewNotClickable();
  }

  @Test
  public void testsetOnClickActions_nullActionsBindingClearsLongActions() {
    view.setOnLongClickListener(mockLongClickListener);
    assertThat(view.isLongClickable()).isTrue();

    ViewUtils.setOnClickActions(
        (ActionsBindingRef) null, view, mockActionHandler, mockFrameContext);

    assertThat(view.isLongClickable()).isFalse();
  }

  @Test
  public void testsetOnClickActions_noOnClickActionActionsDefinedClearsActions() {
    view.setOnClickListener(mockListener);
    assertThat(view.hasOnClickListeners()).isTrue();

    ViewUtils.setOnClickActions(
        Actions.getDefaultInstance(), view, mockActionHandler, mockFrameContext);

    assertViewNotClickable();
  }

  @Test
  public void testsetOnClickActions_noOnLongClickActionActionsDefinedClearsActions() {
    view.setOnLongClickListener(mockLongClickListener);
    assertThat(view.isLongClickable()).isTrue();

    ViewUtils.setOnClickActions(
        Actions.getDefaultInstance(), view, mockActionHandler, mockFrameContext);

    assertThat(view.isLongClickable()).isFalse();
  }

  @Test
  public void testClearOnClickActionActions_success() {
    view.setOnClickListener(mockListener);
    assertThat(view.hasOnClickListeners()).isTrue();

    ViewUtils.clearOnClickActions(view);

    assertViewNotClickable();
  }

  @Test
  public void testClearOnLongClickActionActions_success() {
    view.setOnLongClickListener(mockLongClickListener);
    assertThat(view.isLongClickable()).isTrue();

    ViewUtils.clearOnLongClickActions(view);

    assertThat(view.isLongClickable()).isFalse();
  }

  private void assertViewNotClickable() {
    assertThat(view.hasOnClickListeners()).isFalse();
    assertThat(view.isClickable()).isFalse();
  }
}
