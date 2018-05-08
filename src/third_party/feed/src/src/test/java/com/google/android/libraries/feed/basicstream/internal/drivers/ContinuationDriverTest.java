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

package com.google.android.libraries.feed.basicstream.internal.drivers;

import static com.google.common.truth.Truth.assertThat;
import static org.mockito.MockitoAnnotations.initMocks;

import com.google.android.libraries.feed.api.modelprovider.ModelToken;
import com.google.android.libraries.feed.basicstream.internal.contentmodels.ContentModel;
import com.google.android.libraries.feed.basicstream.internal.contentmodels.ContinuationContentModel;
import com.google.android.libraries.feed.basicstream.internal.util.TokenHandler;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.robolectric.RobolectricTestRunner;

/** Tests for {@link ContinuationDriver}.j */
@RunWith(RobolectricTestRunner.class)
public class ContinuationDriverTest {
  private static final ContinuationContentModel CONTINUATION_CONTENT_MODEL =
      new ContinuationContentModel(null, null);

  @Mock private ModelToken modelToken;
  @Mock private TokenHandler tokenHandler;

  private ContinuationDriverForTest continuationDriver;

  @Before
  public void setup() {
    initMocks(this);

    continuationDriver =
        new ContinuationDriverForTest(tokenHandler, modelToken, CONTINUATION_CONTENT_MODEL);
  }

  @Test
  public void testGetContentModel() {
    ContentModel continuationContentModel = continuationDriver.getContentModel();

    assertThat(continuationContentModel).isEqualTo(CONTINUATION_CONTENT_MODEL);
    assertThat(continuationDriver.modelTokenArgument).isEqualTo(modelToken);
    assertThat(continuationDriver.tokenHandlerArgument).isEqualTo(tokenHandler);
  }

  @Test
  public void testGetContentModel_reusesPreviousContentModel() {
    ContentModel continuationContentModel = continuationDriver.getContentModel();

    assertThat(continuationDriver.getContentModel()).isSameAs(continuationContentModel);
  }

  private class ContinuationDriverForTest extends ContinuationDriver {

    private ContinuationContentModel continuationContentModel;
    private TokenHandler tokenHandlerArgument;
    private ModelToken modelTokenArgument;

    ContinuationDriverForTest(
        TokenHandler tokenHandler,
        ModelToken modelToken,
        ContinuationContentModel continuationContentModel) {
      super(tokenHandler, modelToken);
      this.continuationContentModel = continuationContentModel;
    }

    @Override
    ContinuationContentModel createContinuationContentModel(
        TokenHandler tokenHandler, ModelToken modelToken) {
      this.tokenHandlerArgument = tokenHandler;
      this.modelTokenArgument = modelToken;
      return continuationContentModel;
    }
  }
}
