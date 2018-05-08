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

package com.google.android.libraries.feed.hostimpl.scheduler;

import com.google.android.libraries.feed.host.scheduler.SchedulerApi;

/** Concrete impl of {@link SchedulerApi} */
public class SchedulerApiImpl implements SchedulerApi {

  @Override
  public boolean shouldRequestDataWhenNoDataPresent() {
    return true;
  }

  @Override
  public void onReceiveNewContent() {
    // Do nothing
  }

  @Override
  public void onRequestError(int networkResponseCode) {
    // Do nothing
  }
}
