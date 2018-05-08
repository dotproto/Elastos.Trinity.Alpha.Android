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

package com.google.android.libraries.feed.testing.conformance.network;

import static com.google.common.truth.Truth.assertThat;

import android.net.Uri;
import com.google.android.libraries.feed.common.testing.RequiredConsumer;
import com.google.android.libraries.feed.host.network.HttpRequest;
import com.google.android.libraries.feed.host.network.HttpRequest.HttpMethod;
import com.google.android.libraries.feed.host.network.HttpResponse;
import com.google.android.libraries.feed.host.network.NetworkClient;
import org.junit.Test;

public abstract class NetworkClientConformanceTest {

  protected NetworkClient networkClient;

  @Test
  public void send_get() {
    HttpRequest request =
        new HttpRequest(new Uri.Builder().path("http://www.google.com/").build(), HttpMethod.GET);
    RequiredConsumer<HttpResponse> responseConsumer =
        new RequiredConsumer<>(
            response -> {
              assertThat(response.getResponseBody()).isNotNull();
              assertThat(response.getResponseCode()).isNotNull();
            });

    networkClient.send(request, responseConsumer);
    assertThat(responseConsumer.isCalled()).isTrue();
  }

  @Test
  public void send_post() {
    HttpRequest request =
        new HttpRequest(
            new Uri.Builder().path("http://www.google.com/").build(),
            HttpMethod.POST,
            "helloWorld".getBytes());
    RequiredConsumer<HttpResponse> responseConsumer =
        new RequiredConsumer<>(
            response -> {
              assertThat(response.getResponseBody()).isNotNull();
              assertThat(response.getResponseCode()).isNotNull();
            });

    networkClient.send(request, responseConsumer);
    assertThat(responseConsumer.isCalled()).isTrue();
  }

  @Test
  public void send_put() {
    HttpRequest request =
        new HttpRequest(
            new Uri.Builder().path("http://www.google.com/").build(),
            HttpMethod.PUT,
            "helloWorld".getBytes());
    RequiredConsumer<HttpResponse> responseConsumer =
        new RequiredConsumer<>(
            response -> {
              assertThat(response.getResponseBody()).isNotNull();
              assertThat(response.getResponseCode()).isNotNull();
            });

    networkClient.send(request, responseConsumer);
    assertThat(responseConsumer.isCalled()).isTrue();
  }

  @Test
  public void send_delete() {
    HttpRequest request =
        new HttpRequest(
            new Uri.Builder().path("http://www.google.com/").build(), HttpMethod.DELETE);
    RequiredConsumer<HttpResponse> responseConsumer =
        new RequiredConsumer<>(
            response -> {
              assertThat(response.getResponseBody()).isNotNull();
              assertThat(response.getResponseCode()).isNotNull();
            });

    networkClient.send(request, responseConsumer);
    assertThat(responseConsumer.isCalled()).isTrue();
  }
}
