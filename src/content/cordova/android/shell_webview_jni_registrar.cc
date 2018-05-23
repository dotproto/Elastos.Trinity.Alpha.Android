// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/cordova/android/shell_webview_jni_registrar.h"

#include "base/android/jni_android.h"
#include "base/android/jni_registrar.h"
#include "base/macros.h"

#include "content/cordova/android/shell_webview.h"

namespace {

static base::android::RegistrationMethod kShellWebviewRegistrationMethods[] = {
    {"ShellWebView", content::ShellWebView::Register},
};

}  // namespace

namespace content {
namespace android {

bool RegisterShellWebviewJni(JNIEnv* env) {
  return RegisterNativeMethods(env, kShellWebviewRegistrationMethods,
                               arraysize(kShellWebviewRegistrationMethods));
}

}  // namespace android
}  // namespace content
