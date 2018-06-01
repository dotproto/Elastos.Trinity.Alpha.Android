package org.apache.cordova.engine;

import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ApplicationInfo;
import android.os.Build;
import android.view.View;
import android.webkit.ValueCallback;
import android.util.Log;
import org.apache.cordova.CordovaBridge;
import org.apache.cordova.CordovaInterface;
import org.apache.cordova.CordovaPreferences;
import org.apache.cordova.CordovaResourceApi;
import org.apache.cordova.CordovaWebView;
import org.apache.cordova.CordovaWebViewEngine;
import org.apache.cordova.ICordovaCookieManager;
import org.apache.cordova.LOG;
import org.apache.cordova.NativeToJsMessageQueue;
import org.apache.cordova.PluginManager;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class ShellWebViewEngine implements CordovaWebViewEngine {

    public static final String TAG = "ShellWebViewEngine";

    protected final ShellWebView webView;
    protected CordovaPreferences preferences;
    protected CordovaBridge bridge;
    protected CordovaWebViewEngine.Client client;
    protected CordovaWebView parentWebView;
    protected CordovaInterface cordova;
    protected PluginManager pluginManager;
    protected CordovaResourceApi resourceApi;
    protected NativeToJsMessageQueue nativeToJsMessageQueue;
    private BroadcastReceiver receiver;

    public ShellWebViewEngine(Context context, CordovaPreferences preferences) {
        this(new ShellWebView(context), preferences);
    }

    public ShellWebViewEngine(ShellWebView webView) {
        this(webView, null);
    }

    public ShellWebViewEngine(ShellWebView webView, CordovaPreferences preferences) {
        this.preferences = preferences;
        this.webView = webView;
    }

    @Override
    public void init(CordovaWebView parentWebView, CordovaInterface cordova, Client client,
              CordovaResourceApi resourceApi, PluginManager pluginManager,
              NativeToJsMessageQueue nativeToJsMessageQueue){
        Log.e(TAG, "init");
        if (this.cordova != null) {
            throw new IllegalStateException();
        }
    // Needed when prefs are not passed by the constructor
        if (preferences == null) {
            preferences = parentWebView.getPreferences();
        }
        this.parentWebView = parentWebView;
        this.cordova = cordova;
        this.client = client;
        this.resourceApi = resourceApi;
        this.pluginManager = pluginManager;
        this.nativeToJsMessageQueue = nativeToJsMessageQueue;
        webView.init(this);

        initWebViewSettings();

        nativeToJsMessageQueue.addBridgeMode(new NativeToJsMessageQueue.OnlineEventsBridgeMode(new NativeToJsMessageQueue.OnlineEventsBridgeMode.OnlineEventsBridgeModeDelegate() {
            @Override
            public void setNetworkAvailable(boolean value) {
            //sometimes this can be called after calling webview.destroy() on destroy()

            }

            @Override
            public void runOnUiThread(Runnable r) {
                ShellWebViewEngine.this.cordova.getActivity().runOnUiThread(r);
            }
        }));

        if(Build.VERSION.SDK_INT > Build.VERSION_CODES.JELLY_BEAN_MR2)
            nativeToJsMessageQueue.addBridgeMode(new NativeToJsMessageQueue.EvalBridgeMode(this, cordova));
        bridge = new CordovaBridge(pluginManager, nativeToJsMessageQueue);
        exposeJsInterface(webView, bridge);
    }

    @Override
    public CordovaWebView getCordovaWebView() {
        return parentWebView;
    }

    @Override
    public ICordovaCookieManager getCookieManager() {
        return null;
    }

    @Override
    public View getView() {
        return webView.getContainerView();
    }

    private void initWebViewSettings() {}

    @SuppressLint("AddJavascriptInterface")
    private static void exposeJsInterface(ShellWebView webView, CordovaBridge bridge) {
        if ((Build.VERSION.SDK_INT < Build.VERSION_CODES.JELLY_BEAN_MR1)) {
            LOG.i(TAG, "Disabled addJavascriptInterface() bridge since Android version is old.");
            // Bug being that Java Strings do not get converted to JS strings automatically.
            // This isn't hard to work-around on the JS side, but it's easier to just
            // use the prompt bridge instead.
            return;
        }
        SystemExposedJsApi exposedJsApi = new SystemExposedJsApi(bridge);
        webView.addJavascriptInterface(exposedJsApi, "_cordovaNative");
    }

    @Override
    public void loadUrl(final String url, boolean clearNavigationStack) {
        webView.loadUrl(url);
    }

    @Override
    public String getUrl() {
        return webView.getUrl();
    }

    @Override
    public void stopLoading() {
        webView.stopLoading();
    }

    @Override
    public void clearCache() {
        //webView.clearCache(true);
    }

    @Override
    public void clearHistory() {
        webView.clearHistory();
    }

    @Override
    public boolean canGoBack() {
        return webView.canGoBack();
    }

    @Override
    public boolean goBack() {
        // Check webview first to see if there is a history
        // This is needed to support curPage#diffLink, since they are added to parentEngine's history, but not our history url array (JQMobile behavior)
        if (webView.canGoBack()) {
            webView.goBack();
            return true;
        }
        return false;
    }

    @Override
    public void setPaused(boolean value) {
    }

    @Override
    public void destroy() {
        //webView.chromeClient.destroyLastDialog();
        webView.destroy();
        // unregister the receiver
        if (receiver != null) {
            try {
                webView.getContext().unregisterReceiver(receiver);
            } catch (Exception e) {
                LOG.e(TAG, "Error unregistering configuration receiver: " + e.getMessage(), e);
            }
        }
    }

    @Override
    public void evaluateJavascript(String js, ValueCallback<String> callback) {
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            webView.evaluateJavascript(js, callback);
        }
        else
        {
            LOG.d(TAG, "This webview is using the old bridge");
        }
    }


}