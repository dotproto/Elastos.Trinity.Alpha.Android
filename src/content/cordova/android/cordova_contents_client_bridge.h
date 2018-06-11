// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_CORDOVA_CONTENTS_CLIENT_BRIDGE_H_
#define CONTENT_CORDOVA_CONTENTS_CLIENT_BRIDGE_H_

#include <memory>

#include "base/android/jni_weak_ref.h"
#include "base/android/scoped_java_ref.h"
#include "base/callback.h"
#include "base/containers/id_map.h"
#include "base/supports_user_data.h"
#include "content/public/browser/certificate_request_result_type.h"
#include "content/public/browser/javascript_dialog_manager.h"
#include "content/public/browser/resource_request_info.h"


#define CORDOVA_EXPORT __attribute__((visibility("default")))

class GURL;

namespace content {
class WebContents;
}



namespace content {

// A class that handles the Java<->Native communication for the
// AwContentsClient. AwContentsClientBridge is created and owned by
// native AwContents class and it only has a weak reference to the
// its Java peer. Since the Java AwContentsClientBridge can have
// indirect refs from the Application (via callbacks) and so can outlive
// webview, this class notifies it before being destroyed and to nullify
// any references.
class CordovaContentsClientBridge {
 public:

  // Adds the handler to the UserData registry.
  static void Associate(content::WebContents* web_contents,
                        CordovaContentsClientBridge* handler);
  static CordovaContentsClientBridge* FromWebContents(
      content::WebContents* web_contents);

  CordovaContentsClientBridge(JNIEnv* env,
                         const base::android::JavaRef<jobject>& obj);
  ~CordovaContentsClientBridge();

  void RunJavaScriptDialog(
      content::JavaScriptDialogType dialog_type,
      const GURL& origin_url,
      const base::string16& message_text,
      const base::string16& default_prompt_text,
      content::JavaScriptDialogManager::DialogClosedCallback callback);

  void ConfirmJsResult(JNIEnv*,
                       const base::android::JavaRef<jobject>&,
                       int id,
                       const base::android::JavaRef<jstring>& prompt);
  void CancelJsResult(JNIEnv*, const base::android::JavaRef<jobject>&, int id);


 private:
  JavaObjectWeakGlobalRef java_ref_;

  base::IDMap<
      std::unique_ptr<content::JavaScriptDialogManager::DialogClosedCallback>>
      pending_js_dialog_callbacks_;
};

}  // namespace android_webview

#endif  // CONTENT_CORDOVA_CONTENTS_CLIENT_BRIDGE_H_
