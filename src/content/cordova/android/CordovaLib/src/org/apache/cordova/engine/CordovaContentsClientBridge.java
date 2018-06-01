// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.apache.cordova.engine;

import android.content.Context;
import android.os.Handler;

import org.chromium.base.Callback;
import org.chromium.base.ThreadUtils;
import org.chromium.base.annotations.CalledByNative;
import org.chromium.base.annotations.CalledByNativeUnchecked;
import org.chromium.base.annotations.JNINamespace;
import org.chromium.net.NetError;

import java.util.HashMap;
import java.util.Map;
import android.webkit.JsResult;
import android.webkit.JsPromptResult;

/**
 * This class handles the JNI communication logic for the the AwContentsClient class.
 * Both the Java and the native peers of AwContentsClientBridge are owned by the
 * corresponding AwContents instances. This class and its native peer are connected
 * via weak references. The native AwContentsClientBridge sets up and clear these weak
 * references.
 */
@JNINamespace("content")
public class CordovaContentsClientBridge {
    private static final String TAG = "CordovaContentsClientBridge";

    private ShellWebViewClient mClient;
    private Context mContext;
    // The native peer of this object.
    private long mNativeContentsClientBridge;


    public CordovaContentsClientBridge(Context context, ShellWebViewClient client) {
        assert client != null;
        mContext = context;
        mClient = client;
    }

    // Used by the native peer to set/reset a weak ref to the native peer.
    @CalledByNative
    private void setNativeCordovaContentsClientBridge(long nativeCordovaContentsClientBridge) {
        mNativeContentsClientBridge = nativeCordovaContentsClientBridge;
    }


    class JsResultHandler implements JsResult.ResultReceiver {
        private CordovaContentsClientBridge mBridge;
        private final int mId;

        private final JsPromptResult mPromptResult = new JsPromptResult(this);

        JsResultHandler(CordovaContentsClientBridge bridge, int id) {
            mBridge = bridge;
            mId = id;
        }

        public JsPromptResult getPromptResult() {
            return mPromptResult;
        }

        @Override
        public void onJsResultComplete(JsResult result){
            if (mPromptResult.getResult()){
                ThreadUtils.runOnUiThread(() -> {
                    if (mBridge != null) mBridge.confirmJsResult(mId, 
                        mPromptResult.getStringResult());
                    mBridge = null;
                });
            }else{
                ThreadUtils.runOnUiThread(() -> {
                    if (mBridge != null) mBridge.cancelJsResult(mId);
                    mBridge = null;
                });
            }
        }
    }


    @CalledByNative
    private void handleJsAlert(final String url, final String message, final int id) {
        // Post the application callback back to the current thread to ensure the application
        // callback is executed without any native code on the stack. This so that any exception
        // thrown by the application callback won't have to be propagated through a native call
        // stack.
        new Handler().post(() -> {
            JsResultHandler handler = new JsResultHandler(CordovaContentsClientBridge.this, id);
            mClient.onJsAlert(url, message, handler.getPromptResult());
        });
    }

    @CalledByNative
    private void handleJsConfirm(final String url, final String message, final int id) {
        // Post the application callback back to the current thread to ensure the application
        // callback is executed without any native code on the stack. This so that any exception
        // thrown by the application callback won't have to be propagated through a native call
        // stack.
        new Handler().post(() -> {
            JsResultHandler handler = new JsResultHandler(CordovaContentsClientBridge.this, id);
            mClient.onJsConfirm(url, message, handler.getPromptResult());
        });
    }

    @CalledByNative
    private void handleJsPrompt(
            final String url, final String message, final String defaultValue, final int id) {
        // Post the application callback back to the current thread to ensure the application
        // callback is executed without any native code on the stack. This so that any exception
        // thrown by the application callback won't have to be propagated through a native call
        // stack.
        new Handler().post(() -> {
            JsResultHandler handler = new JsResultHandler(CordovaContentsClientBridge.this, id);
            mClient.onJsPrompt(url, message, defaultValue, handler.getPromptResult());
        });
    }


    /*@CalledByNativeUnchecked
    private boolean shouldOverrideUrlLoading(
            String url, boolean hasUserGesture, boolean isRedirect, boolean isMainFrame) {
        return true;
    }*/

    void confirmJsResult(int id, String prompt) {
        if (mNativeContentsClientBridge == 0) return;
        nativeConfirmJsResult(mNativeContentsClientBridge, id, prompt);
    }

    void cancelJsResult(int id) {
        if (mNativeContentsClientBridge == 0) return;
        nativeCancelJsResult(mNativeContentsClientBridge, id);
    }

    //--------------------------------------------------------------------------------------------
    //  Native methods
    //--------------------------------------------------------------------------------------------

    private native void nativeConfirmJsResult(long nativeCordovaContentsClientBridge, int id,
            String prompt);
    private native void nativeCancelJsResult(long nativeCordovaContentsClientBridge, int id);
}
