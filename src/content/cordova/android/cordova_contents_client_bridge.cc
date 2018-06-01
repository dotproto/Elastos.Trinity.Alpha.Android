// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/cordova/android/cordova_contents_client_bridge.h"

#include <memory>
#include <utility>

#include "base/android/jni_android.h"
#include "base/android/jni_array.h"
#include "base/android/jni_string.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/message_loop/message_loop_current.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/client_certificate_delegate.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "jni/CordovaContentsClientBridge_jni.h"
#include "ui/base/l10n/l10n_util.h"
#include "url/gurl.h"

using base::android::AttachCurrentThread;
using base::android::ConvertJavaStringToUTF16;
using base::android::ConvertUTF8ToJavaString;
using base::android::ConvertUTF16ToJavaString;
using base::android::HasException;
using base::android::JavaRef;
using base::android::ScopedJavaLocalRef;
using base::android::ToJavaArrayOfStrings;
using content::BrowserThread;
using content::WebContents;
using std::vector;

namespace content {

namespace {

const void* const kCordovaContentsClientBridge = &kCordovaContentsClientBridge;

// This class is invented so that the UserData registry that we inject the
// AwContentsClientBridge object does not own and destroy it.
class UserData : public base::SupportsUserData::Data {
 public:
  static CordovaContentsClientBridge* GetContents(
      content::WebContents* web_contents) {
    if (!web_contents)
      return NULL;
    UserData* data = static_cast<UserData*>(
        web_contents->GetUserData(kCordovaContentsClientBridge));
    return data ? data->contents_ : NULL;
  }

  explicit UserData(CordovaContentsClientBridge* ptr) : contents_(ptr) {}

 private:
  CordovaContentsClientBridge* contents_;

  DISALLOW_COPY_AND_ASSIGN(UserData);
};

}  // namespace

// static
void CordovaContentsClientBridge::Associate(WebContents* web_contents,
                                       CordovaContentsClientBridge* handler) {
  web_contents->SetUserData(kCordovaContentsClientBridge,
                            std::make_unique<UserData>(handler));
}

// static
CordovaContentsClientBridge* CordovaContentsClientBridge::FromWebContents(
    WebContents* web_contents) {
  return UserData::GetContents(web_contents);
}


CordovaContentsClientBridge::CordovaContentsClientBridge(JNIEnv* env,
                                               const JavaRef<jobject>& obj)
    : java_ref_(env, obj) {
  DCHECK(!obj.is_null());
  Java_CordovaContentsClientBridge_setNativeCordovaContentsClientBridge(
      env, obj, reinterpret_cast<intptr_t>(this));
}

CordovaContentsClientBridge::~CordovaContentsClientBridge() {
  JNIEnv* env = AttachCurrentThread();

  ScopedJavaLocalRef<jobject> obj = java_ref_.get(env);
  if (!obj.is_null()) {
    // Clear the weak reference from the java peer to the native object since
    // it is possible that java object lifetime can exceed the AwContens.
    Java_CordovaContentsClientBridge_setNativeCordovaContentsClientBridge(env, obj, 0);
  }
}

void CordovaContentsClientBridge::RunJavaScriptDialog(
    content::JavaScriptDialogType dialog_type,
    const GURL& origin_url,
    const base::string16& message_text,
    const base::string16& default_prompt_text,
    content::JavaScriptDialogManager::DialogClosedCallback callback) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  JNIEnv* env = AttachCurrentThread();

  ScopedJavaLocalRef<jobject> obj = java_ref_.get(env);
  if (obj.is_null()) {
    std::move(callback).Run(false, base::string16());
    return;
  }

  int callback_id = pending_js_dialog_callbacks_.Add(
      std::make_unique<content::JavaScriptDialogManager::DialogClosedCallback>(
          std::move(callback)));
  ScopedJavaLocalRef<jstring> jurl(
      ConvertUTF8ToJavaString(env, origin_url.spec()));
  ScopedJavaLocalRef<jstring> jmessage(
      ConvertUTF16ToJavaString(env, message_text));

  switch (dialog_type) {
    case content::JAVASCRIPT_DIALOG_TYPE_ALERT: {

      Java_CordovaContentsClientBridge_handleJsAlert(env, obj, jurl, jmessage,
                                                callback_id);
      break;
    }
    case content::JAVASCRIPT_DIALOG_TYPE_CONFIRM: {

      Java_CordovaContentsClientBridge_handleJsConfirm(env, obj, jurl, jmessage,
                                                  callback_id);
      break;
    }
    case content::JAVASCRIPT_DIALOG_TYPE_PROMPT: {
      ScopedJavaLocalRef<jstring> jdefault_value(
          ConvertUTF16ToJavaString(env, default_prompt_text));
      Java_CordovaContentsClientBridge_handleJsPrompt(env, obj, jurl, jmessage,
                                                 jdefault_value, callback_id);
      break;
    }
    default:
      NOTREACHED();
  }
}

void CordovaContentsClientBridge::ConfirmJsResult(JNIEnv* env,
                                             const JavaRef<jobject>&,
                                             int id,
                                             const JavaRef<jstring>& prompt) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  content::JavaScriptDialogManager::DialogClosedCallback* callback =
      pending_js_dialog_callbacks_.Lookup(id);
  if (!callback) {
    LOG(WARNING) << "Unexpected JS dialog confirm. " << id;
    return;
  }
  base::string16 prompt_text;
  if (!prompt.is_null()) {
    prompt_text = ConvertJavaStringToUTF16(env, prompt);
  }
  std::move(*callback).Run(true, prompt_text);
  pending_js_dialog_callbacks_.Remove(id);
}


void CordovaContentsClientBridge::CancelJsResult(JNIEnv*,
                                            const JavaRef<jobject>&,
                                            int id) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  content::JavaScriptDialogManager::DialogClosedCallback* callback =
      pending_js_dialog_callbacks_.Lookup(id);
  if (!callback) {
    LOG(WARNING) << "Unexpected JS dialog cancel. " << id;
    return;
  }
  std::move(*callback).Run(false, base::string16());
  pending_js_dialog_callbacks_.Remove(id);
}

}  // namespace android_webview
