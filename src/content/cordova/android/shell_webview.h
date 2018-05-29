#ifndef CONTENT_CORDOVA_SHELL_WEBVIEW_H_
#define CONTENT_CORDOVA_SHELL_WEBVIEW_H_

#include <jni.h>

#include "base/android/jni_android.h"
#include "base/android/scoped_java_ref.h"
#include "base/android/jni_weak_ref.h"
#include "base/callback_forward.h"
#include "base/memory/ref_counted.h"
#include "base/strings/string_piece.h"
#include "build/build_config.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/browser/web_contents_observer.h"
#include "ipc/ipc_channel.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/native_widget_types.h"

using base::android::JavaParamRef;
using base::android::ScopedJavaLocalRef;

namespace content {

class ShellWebView{
public:
  ShellWebView();
  ~ShellWebView();

	WebContents* web_contents() const { return web_contents_.get(); }

	void SetWebContents(std::unique_ptr<WebContents> web_contents){
    web_contents_ = std::move(web_contents);
  }

  //  static bool Register(JNIEnv* env);

	void PlatformSetContents();

  void SetJavaPeer(JNIEnv* env,
      const base::android::JavaParamRef<jobject>& obj,
      const base::android::JavaParamRef<jobject>& shell_webview);

  void CreateWebContent(JNIEnv* env,
                 const JavaParamRef<jobject>& obj);

private:
    std::unique_ptr<WebContents> web_contents_;
    JavaObjectWeakGlobalRef java_ref_;
    //base::android::ScopedJavaGlobalRef<jobject> java_object_;

};


}  // namespace content

#endif  // CONTENT_CORDOVA_SHELL_WEBVIEW_H_