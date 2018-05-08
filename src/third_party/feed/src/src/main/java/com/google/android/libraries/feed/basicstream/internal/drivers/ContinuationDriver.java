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

import com.google.android.libraries.feed.api.modelprovider.ModelToken;
import com.google.android.libraries.feed.basicstream.internal.contentmodels.ContentModel;
import com.google.android.libraries.feed.basicstream.internal.contentmodels.ContinuationContentModel;
import com.google.android.libraries.feed.basicstream.internal.util.TokenHandler;

/** {@link FeatureDriver} for the more button. */
public class ContinuationDriver implements FeatureDriver {

  private final ModelToken modelToken;
  private final TokenHandler tokenHandlerArgument;
  /*@Nullable*/ private ContinuationContentModel continuationContentModel;

  ContinuationDriver(TokenHandler tokenHandler, ModelToken modelToken) {
    this.tokenHandlerArgument = tokenHandler;
    this.modelToken = modelToken;
  }

  @Override
  public ContentModel getContentModel() {
    if (continuationContentModel == null) {
      continuationContentModel = createContinuationContentModel(tokenHandlerArgument, modelToken);
    }

    return continuationContentModel;
  }

  ContinuationContentModel createContinuationContentModel(
      TokenHandler tokenHandler, ModelToken modelToken) {
    return new ContinuationContentModel(tokenHandler, modelToken);
  }
}
