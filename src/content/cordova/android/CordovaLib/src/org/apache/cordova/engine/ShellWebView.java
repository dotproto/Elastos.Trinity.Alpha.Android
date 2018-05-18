package org.apache.cordova.engine;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import org.apache.cordova.CordovaInterface;
import org.apache.cordova.CordovaWebView;
import org.apache.cordova.CordovaWebViewEngine;

import android.app.Activity;
import android.content.Context;
import android.graphics.drawable.ClipDrawable;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.view.ActionMode;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.AbsoluteLayout;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;
import android.webkit.ValueCallback;

import org.chromium.base.annotations.CalledByNative;
import org.chromium.base.annotations.JNINamespace;
import org.chromium.content.browser.ActivityContentVideoViewEmbedder;
import org.chromium.content.browser.ContentVideoViewEmbedder;
import org.chromium.content.browser.ContentView;
import org.chromium.content.browser.ContentViewClient;
import org.chromium.content.browser.ContentViewCore;
import org.chromium.content.browser.ContentViewRenderView;
import org.chromium.content_public.browser.ActionModeCallbackHelper;
import org.chromium.content_public.browser.LoadUrlParams;
import org.chromium.content_public.browser.NavigationController;
import org.chromium.content_public.browser.WebContents;
import org.chromium.ui.base.WindowAndroid;
import org.chromium.base.BaseSwitches;
import org.chromium.base.CommandLine;
import org.chromium.base.MemoryPressureListener;
import org.chromium.base.annotations.SuppressFBWarnings;
import org.chromium.base.library_loader.LibraryLoader;
import org.chromium.base.library_loader.LibraryProcessType;
import org.chromium.base.library_loader.ProcessInitException;
import org.chromium.content.app.ContentApplication;
import org.chromium.content.browser.BrowserStartupController;
import org.chromium.content.browser.ContentViewCore;
import org.chromium.content.browser.DeviceUtils;
import org.chromium.content.common.ContentSwitches;
import org.chromium.content_public.browser.WebContents;
import org.chromium.ui.base.ActivityWindowAndroid;
import android.content.Intent;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import org.chromium.ui.base.ViewAndroidDelegate;
import org.chromium.content_public.browser.JavaScriptCallback;

@JNINamespace("content")
class ShellWebView extends AbsoluteLayout implements CordovaWebViewEngine.EngineView {
    private static final String TAG = "ShellWebView";

    private static final int WARN = 1;

    private ContentViewCore mContentViewCore;
    private WebContents mWebContents;
    private NavigationController mNavigationController;
    private final ContentViewClient mContentViewClient = new ContentViewClient();
    private ActivityWindowAndroid mWindowAndroid;
    private ContentViewRenderView mContentViewRenderView;
    private ContentView mContentView;
    private ShellViewAndroidDelegate mViewAndroidDelegate;

    private FrameLayout mContentViewHolder;
    private long mNativeShellWebView;

    private boolean finishInitialized = false;

    private String pendingUrl;

    public ShellWebView(Context context) {
        this(context, null);
    }

    public View getContainerView(){
        return mContentViewHolder;
    }

    public ShellWebView(Context context, AttributeSet attrs) {
        super(context, attrs);
        mContentViewHolder = new FrameLayout(context);
        mContentViewCore = new ContentViewCore(context, "");
        mContentView = ContentView.createContentView(getContext(), mContentViewCore);
        mNativeShellWebView = nativeInit();
        nativeSetJavaPeer(mNativeShellWebView, this);
        initView();
    }

    @Override
    public CordovaWebView getCordovaWebView() {
        return null;
    }

    public void initView() {
        Log.e(TAG, "init");
        final boolean listenToActivityState = false;
        mWindowAndroid = new ActivityWindowAndroid(getContext(), listenToActivityState);
        mContentViewRenderView = new ContentViewRenderView(getContext());
        mContentViewRenderView.onNativeLibraryLoaded(mWindowAndroid);
        mWindowAndroid.setAnimationPlaceholderView(
                mContentViewRenderView.getSurfaceView());
        startup();
    }

    private void finishInitialization()
    {
        Log.e(TAG, "finishInitialization");
       nativeCreateWebContent(mNativeShellWebView);

       if(pendingUrl!=null && pendingUrl.length()> 0){
        loadUrl(pendingUrl);
       }
    }

    private void initializationFailed()
    {
        Log.e(TAG, "initializationFailed");
    }

    private void startup(){
        Log.e(TAG, "startup");
        try {
            BrowserStartupController.get(LibraryProcessType.PROCESS_BROWSER)
                    .startBrowserProcessesAsync(
                            true,
                            new BrowserStartupController.StartupCallback() {
                                @Override
                                public void onSuccess(boolean alreadyStarted) {
                                    finishInitialization();
                                }

                                @Override
                                public void onFailure() {
                                    initializationFailed();
                                }
                            });
        } catch (ProcessInitException e) {
            Log.e(TAG, "Unable to load native library.", e);
            System.exit(-1);
        }
    }

    public static String sanitizeUrl(String url) {
        if (url == null) return null;
        if (url.startsWith("www.") || url.indexOf(":") == -1) url = "http://" + url;
        return url;
    }

    public void loadUrl(final String url){
        if (url == null) return;
        if (!finishInitialized){
            pendingUrl = url;
            return;
        }
        Log.e(TAG, "loadUrl:" + url);
        mNavigationController.loadUrl(new LoadUrlParams(sanitizeUrl(url)));
        mContentViewCore.getContainerView().clearFocus();
        mContentViewCore.getContainerView().requestFocus();
    }

    public class ShellViewAndroidDelegate extends ViewAndroidDelegate {
        private final ViewGroup mContainerView;

        public ShellViewAndroidDelegate(ViewGroup containerView) {
            mContainerView = containerView;
        }

        @Override
        public void startContentIntent(Intent intent, String intentUrl, boolean isMainFrame) {

        }

        @Override
        public ViewGroup getContainerView() {
            return mContainerView;
        }
    }

    private ActionMode.Callback defaultActionCallback() {
        final ActionModeCallbackHelper helper =
                mContentViewCore.getActionModeCallbackHelper();

        return new ActionMode.Callback() {
            @Override
            public boolean onCreateActionMode(ActionMode mode, Menu menu) {
                helper.onCreateActionMode(mode, menu);
                return true;
            }

            @Override
            public boolean onPrepareActionMode(ActionMode mode, Menu menu) {
                return helper.onPrepareActionMode(mode, menu);
            }

            @Override
            public boolean onActionItemClicked(ActionMode mode, MenuItem item) {
                return helper.onActionItemClicked(mode, item);
            }

            @Override
            public void onDestroyActionMode(ActionMode mode) {
                helper.onDestroyActionMode();
            }
        };
    }

    @CalledByNative
    private void initContentViewCore(WebContents webContents) {
        Log.e(TAG, "initContentViewCore from native");
        finishInitialized = true;
        Context context = getContext();
        mViewAndroidDelegate = new ShellViewAndroidDelegate(mContentView);
        mContentViewCore.initialize(mViewAndroidDelegate, mContentView, webContents, mWindowAndroid);
        mContentViewCore.setActionModeCallback(defaultActionCallback());
        mContentViewCore.setContentViewClient(mContentViewClient);
        mWebContents = mContentViewCore.getWebContents();
        mNavigationController = mWebContents.getNavigationController();
        if (getParent() != null) mContentViewCore.onShow();

        mContentViewHolder.addView(mContentViewRenderView,
                    new FrameLayout.LayoutParams(
                            FrameLayout.LayoutParams.MATCH_PARENT,
                            FrameLayout.LayoutParams.MATCH_PARENT));

        mContentViewHolder.addView(mContentView,
                new FrameLayout.LayoutParams(
                        FrameLayout.LayoutParams.MATCH_PARENT,
                        FrameLayout.LayoutParams.MATCH_PARENT));
        mContentView.requestFocus();
        mContentViewRenderView.setCurrentContentViewCore(mContentViewCore);
    }

    public String getUrl(){
        if (isDestroyed(WARN)) return null;
        String url =  mWebContents.getUrl();
        if (url == null || url.trim().isEmpty()) return null;
        return url;
    }

    public void stopLoading() {
        if (!isDestroyed(WARN)) mWebContents.stop();
    }

    public void clearCache() {
        //if (!isDestroyed(WARN)) nativeClearCache(true);
    }

    public void clearHistory() {
        if (!isDestroyed(WARN)) mNavigationController.clearHistory();
    }

    public boolean canGoBack() {
        return isDestroyed(WARN) ? false : mNavigationController.canGoBack();
    }

    public boolean goBack() {
        if (!isDestroyed(WARN)) mNavigationController.goBack();
        return true;
    }

    public void evaluateJavascript(String js, final ValueCallback<String> callback){
        JavaScriptCallback jsCallback = null;
        if (callback != null) {
            jsCallback = new JavaScriptCallback() {
                @Override
                public void handleJavaScriptResult(String jsonResult) {
                    callback.onReceiveValue(jsonResult);
                }
            };
        }

        mWebContents.evaluateJavaScript(js, jsCallback);
    }

    private boolean isDestroyed(int warnIfDestroyed) {
        return false;
    }

    public void addJavascriptInterface(final Object obj, final String interfaceName){
        mContentViewCore.addPossiblyUnsafeJavascriptInterface(obj, interfaceName, null);
    }

    public void destroy(){
        mContentViewRenderView.destroy();
        mContentViewRenderView = null;
        mContentViewCore.destroy();
    }

    private static native long nativeInit();
    private native void nativeSetJavaPeer(long nativeShellWebView, ShellWebView sw);
    private native void nativeCreateWebContent(long nativeShellWebView);
}