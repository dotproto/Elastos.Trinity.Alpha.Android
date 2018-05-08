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

package com.google.android.libraries.feed.host.scheduler;

/** Allows host to change behavior based on Feed requests and their status */
public interface SchedulerApi {
  /**
   * Called when Feed initializes with no data available. The scheduler can decide whether to allow
   * Feed to initiate a request for data, or reject the request.
   *
   * @return True if Feed should request data.
   */
  boolean shouldRequestDataWhenNoDataPresent();

  /** Notify scheduler that new content has been received. */
  void onReceiveNewContent();

  /** Notify scheduler that an error occurred handling a request. */
  void onRequestError(int networkResponseCode);
}
