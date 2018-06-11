// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include "content/cordova/android/shell_webview.h"

#include "base/logging.h"
#include "jni/ShellWebView_jni.h"
#include <stddef.h>
#include "content/shell/browser/shell_content_browser_client.h"
#include "content/shell/browser/shell_browser_context.h"

namespace content {

void ShellWebView::PlatformSetContents() {
  LOG(ERROR) << "ShellWebView::PlatformSetContents";
  JNIEnv* env = base::android::AttachCurrentThread();
  ScopedJavaLocalRef<jobject> obj = java_ref_.get(env);
  if (obj.is_null())return;
  Java_ShellWebView_initContentViewCore(env, obj,
                                       web_contents()->GetJavaWebContents());
}

// Register native methods
/*bool ShellWebView::Register(JNIEnv* env) {
  return RegisterNativesImpl(env);
}*/

ShellWebView::ShellWebView(){

}

ShellWebView::~ShellWebView(){

}

static jlong JNI_ShellWebView_Init(JNIEnv* env,
                  const JavaParamRef<jclass>& clazz) {
  return reinterpret_cast<intptr_t>(new ShellWebView());
}

void ShellWebView::SetJavaPeer(JNIEnv* env,
      const base::android::JavaParamRef<jobject>& obj,
      const base::android::JavaParamRef<jobject>& shell_webview,
      const base::android::JavaParamRef<jobject>& contents_client_bridge){
  java_ref_ = JavaObjectWeakGlobalRef(env, shell_webview);
  contents_client_bridge_.reset(
      new CordovaContentsClientBridge(env, contents_client_bridge));
}

void ShellWebView::CreateWebContent(JNIEnv* env,
                 const JavaParamRef<jobject>& obj) {
  LOG(ERROR) << "ShellWebView::CreateWebContent";
  ShellBrowserContext* browserContext =
      ShellContentBrowserClient::Get()->browser_context();
      LOG(ERROR) << "ShellWebView::CreateWebContent2";
  SetWebContents(WebContents::Create(
      WebContents::CreateParams(browserContext)));
  CordovaContentsClientBridge::Associate(web_contents(),
                                    contents_client_bridge_.get());
  web_contents()->SetDelegate(this);
  PlatformSetContents();

}


JavaScriptDialogManager* ShellWebView::GetJavaScriptDialogManager(
    WebContents* source) {
  if (!dialog_manager_) {
    dialog_manager_.reset(new ShellJavaScriptDialogManager);
  }
  return dialog_manager_.get();
}

}  // namespace content
