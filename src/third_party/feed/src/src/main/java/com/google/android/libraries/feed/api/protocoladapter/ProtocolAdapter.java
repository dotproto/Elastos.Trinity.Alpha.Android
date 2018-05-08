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

package com.google.android.libraries.feed.api.protocoladapter;

import com.google.android.libraries.feed.common.Result;
import com.google.search.now.feed.client.StreamDataProto.StreamDataOperation;
import com.google.search.now.wire.feed.ContentIdProto.ContentId;
import com.google.search.now.wire.feed.ResponseProto.Response;
import java.util.List;

/** Converts the wire protocol (protos sent from the server) into an internal representation. */
public interface ProtocolAdapter {
  /**
   * Create the internal protocol from a wire protocol response definition. The wire protocol is
   * turned into a List of {@link StreamDataOperation} which are sent to the SessionManager.
   */
  Result<List<StreamDataOperation>> createModel(Response response);

  /** Convert a wire protocol ContentId into the {@code String} version. */
  String getStreamContentId(ContentId contentId);
}
