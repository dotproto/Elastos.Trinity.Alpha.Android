ifndef CONTENT_CORDOVA_SHELL_WEBVIEW_H_
#define CONTENT_CORDOVA_SHELL_WEBVIEW_H_

#include <jni.h>

#include "base/android/jni_android.h"
#include "base/android/scoped_java_ref.h"

#include "base/callback_forward.h"
#include "base/memory/ref_counted.h"
#include "base/strings/string_piece.h"
#include "build/build_config.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/browser/web_contents_observer.h"
#include "ipc/ipc_channel.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/native_widget_types.h"

namespace content {

class ShellWebView{
public:

	WebContents* web_contents() const { return web_contents_.get(); }
	void SetWebContents(WebContents* web_contents){web_contents_.reset(web_contents)}
    static jlong Init(JNIEnv* env,
                  const JavaParamRef<jclass>& clazz);
    static bool Register(JNIEnv* env);
    void CreatewebContent(JNIEnv* env,
                 const JavaParamRef<jclass>& clazz, jlong shellwebviewPtr);
    void PlatformSetContents();

private:
    std::unique_ptr<WebContents> web_contents_;

}


}  // namespace content

#endif  // CONTENT_CORDOVA_SHELL_WEBVIEW_H_