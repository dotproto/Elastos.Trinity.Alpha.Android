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

import static org.mockito.Mockito.verify;
import static org.mockito.MockitoAnnotations.initMocks;

import android.view.View.OnClickListener;
import com.google.android.libraries.feed.api.modelprovider.ModelToken;
import com.google.android.libraries.feed.basicstream.internal.contentmodels.ContinuationContentModel;
import com.google.android.libraries.feed.basicstream.internal.util.TokenHandler;
import com.google.android.libraries.feed.basicstream.internal.viewholders.ContinuationViewHolder;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.ArgumentCaptor;
import org.mockito.Mock;
import org.robolectric.RobolectricTestRunner;

/** Tests for {@link ContinuationContentModel}. */
@RunWith(RobolectricTestRunner.class)
public class ContinuationContentModelTest {

  @Mock private ModelToken token;
  @Mock private ContinuationViewHolder viewHolder;
  @Mock private TokenHandler fakeTokenHandler;

  private ContinuationContentModel continuationContentModel;

  @Before
  public void setUp() {
    initMocks(this);
    continuationContentModel = new ContinuationContentModel(fakeTokenHandler, token);
  }

  @Test
  public void testBind() {
    ArgumentCaptor<OnClickListener> clickListenerCaptor =
        ArgumentCaptor.forClass(OnClickListener.class);

    continuationContentModel.bind(viewHolder);

    verify(viewHolder).bind(clickListenerCaptor.capture());

    clickListenerCaptor.getValue().onClick(null);

    verify(fakeTokenHandler).requestTokenHandling(token);
  }

  @Test
  public void testUnbind() {
    continuationContentModel.unbind(viewHolder);
    verify(viewHolder).unbind();
  }
}
