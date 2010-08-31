/*******************************************************************************
 * Copyright (c) 2008, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - QT implementation
 *******************************************************************************/
package org.eclipse.swt.browser;

import org.eclipse.swt.SWT;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.webkit.WebkitOS;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Listener;

class QtWebKit extends WebBrowser {
    
    private static final String ADD_WIDGET_KEY = "org.eclipse.swt.internal.addWidget";
    private static final String REMOVE_WIDGET_KEY = "org.eclipse.swt.internal.removeWidget";
    private static final String SET_CANVAS_STATE_KEY = "org.eclipse.swt.internal.setCanvasState";
    private static final String SET_NO_TRAVERSE_STATE_KEY = "org.eclipse.swt.internal.setNoTraverseState";
    private static final String SET_NO_KEY_PROPAGATE_STATE_KEY = "org.eclipse.swt.internal.setNoKeyPropagateState";
    private static final String SET_NO_MOUSE_PROPAGATE_STATE_KEY = "org.eclipse.swt.internal.setNoMousePropagateState";
    private static final String SET_EMBEDDED_SCROLLBARS_STATE_KEY = "org.eclipse.swt.internal.setEmbeddedScrollbarsState";
    private boolean ignoreDispose = false;
    private int webViewHandle;

/**
 * @see WebBrowser
 */
public void create(Composite parent, int style) {
    
    // At this point, browser.handle is the view port of QScrollArea, set by Composite
    int scrollAreaHandle = Internal_PackageSupport.topHandle(browser);
    int viewPortHandle = Internal_PackageSupport.handle(browser);
    webViewHandle = WebkitOS.QWebView_new();
    OS.QWidget_setParent(webViewHandle, scrollAreaHandle);
    
    int layout = OS.QVBoxLayout_new(0);
    OS.QLayout_setContentsMargins(layout, 0, 0, 0, 0);
    OS.QLayout_setSpacing(layout, 0);
    OS.QLayout_addWidget(layout, webViewHandle);
    OS.QWidget_setLayout(scrollAreaHandle, layout);
    OS.QWidget_setVisible(webViewHandle, true);
    OS.QLayout_activate(layout);
    OS.QLayout_update(layout);
    
    Display display = browser.getDisplay();
    display.setData(ADD_WIDGET_KEY, new Object[] {new Integer(webViewHandle), browser});
    Internal_PackageSupport.setHandle(browser, webViewHandle);
    
    // Just in case focus ends up in the browser "internal" parents
    OS.QWidget_setFocusProxy(scrollAreaHandle, webViewHandle);
    OS.QWidget_setFocusProxy(viewPortHandle, webViewHandle);
    
    Listener listener = new Listener() {
        public void handleEvent(Event e) {
            switch (e.type) {
                case SWT.Dispose: {
                    if (ignoreDispose) {
                        ignoreDispose = false;
                        break;
                    }
                    ignoreDispose = true;
                    browser.notifyListeners (e.type, e);
                    e.type = SWT.NONE;
                    e.display.setData(REMOVE_WIDGET_KEY, new Object[] {new Integer(webViewHandle)});
                    break;
                }
            }
        }
    };
    browser.addListener(SWT.Dispose, listener);
    
    browser.setData(SET_CANVAS_STATE_KEY, new Boolean(false));
    browser.setData(SET_NO_TRAVERSE_STATE_KEY, new Boolean(true));
    browser.setData(SET_NO_KEY_PROPAGATE_STATE_KEY, new Boolean(true));
    browser.setData(SET_NO_MOUSE_PROPAGATE_STATE_KEY, new Boolean(true));
    browser.setData(SET_EMBEDDED_SCROLLBARS_STATE_KEY, new Boolean(true));
    
    int handler1 = OS.SignalForwarder_new(webViewHandle, this, OS.QSIGNAL_WEBVIEW_LOADFINISHED);
    OS.QObject_connectOrThrow(webViewHandle, "loadFinished(bool)", 
        handler1, "widgetSignal(bool)", OS.QT_AUTOCONNECTION);
    int handler2 = OS.SignalForwarder_new(webViewHandle, this, OS.QSIGNAL_WEBVIEW_LOADPROGRESS);
    OS.QObject_connectOrThrow(webViewHandle, "loadProgress(int)", 
        handler2, "widgetSignal(int)", OS.QT_AUTOCONNECTION);
    int handler3 = OS.SignalForwarder_new(webViewHandle, this, OS.QSIGNAL_WEBVIEW_LOADSTARTED);
    OS.QObject_connectOrThrow(webViewHandle, "loadStarted()", 
        handler3, "widgetSignal()", OS.QT_AUTOCONNECTION);
    int handler4 = OS.SignalForwarder_new(webViewHandle, this, OS.QSIGNAL_WEBVIEW_STATUSBARMESSAGE);
    OS.QObject_connectOrThrow(webViewHandle, "statusBarMessage(const QString&)", 
        handler4, "widgetSignal(const QString&)", OS.QT_AUTOCONNECTION);
    int handler5 = OS.SignalForwarder_new(webViewHandle, this, OS.QSIGNAL_WEBVIEW_TITLECHANGED);
    OS.QObject_connectOrThrow(webViewHandle, "titleChanged(const QString&)", 
        handler5, "widgetSignal(const QString&)", OS.QT_AUTOCONNECTION);
    int handler6 = OS.SignalForwarder_new(webViewHandle, this, OS.QSIGNAL_WEBVIEW_URLCHANGED);
    OS.QObject_connectOrThrow(webViewHandle, "urlChanged(const QUrl&)", 
        handler6, "widgetSignal(const QUrl&)", OS.QT_AUTOCONNECTION);
    int handler7 = OS.SignalForwarder_new(webViewHandle, this, OS.QSIGNAL_WEBVIEW_LINKCLICKED);
    OS.QObject_connectOrThrow(webViewHandle, "linkClicked(const QUrl&)", 
        handler7, "widgetSignal(const QUrl&)", OS.QT_AUTOCONNECTION);
}

/**
 * @see WebBrowser
 */
public boolean back() {
    if (WebkitOS.QWebView_swt_canGoBack(webViewHandle)) {
        if (sendLocationEventChanging((int)System.currentTimeMillis(), 
            WebkitOS.QWebView_swt_backUrl(webViewHandle))) {
            WebkitOS.QWebView_back(webViewHandle);
            return true;
        }
    }
    return false;
}

/**
 * @see WebBrowser
 */
public boolean execute(String script) {
    return WebkitOS.QWebView_swt_evaluateJavaScript(webViewHandle, script);
}

/**
 * @see WebBrowser
 */
public boolean forward() {
    if (WebkitOS.QWebView_swt_canGoForward(webViewHandle)) {
        if (sendLocationEventChanging((int)System.currentTimeMillis(), 
            WebkitOS.QWebView_swt_forwardUrl(webViewHandle))) {
            WebkitOS.QWebView_forward(webViewHandle);
            return true;
        }
    }
    return false;
}

/**
 * @see WebBrowser
 */
public String getBrowserType() {
    return "qtwebkit";
}

/**
 * @see WebBrowser
 */
public String getText() { // Not eSWT API
    return null;
}

/**
 * @see WebBrowser
 */
public String getUrl() {
    return WebkitOS.QWebView_url(webViewHandle);
}

/**
 * @see WebBrowser
 */
public boolean isBackEnabled() {
    return WebkitOS.QWebView_swt_canGoBack(webViewHandle);
}

/**
 * @see WebBrowser
 */
public boolean isForwardEnabled() {
    return WebkitOS.QWebView_swt_canGoForward(webViewHandle);
}

/**
 * @see WebBrowser
 */
public void refresh() {
    if (sendLocationEventChanging((int)System.currentTimeMillis(), 
        WebkitOS.QWebView_url(webViewHandle))) {
        WebkitOS.QWebView_reload(webViewHandle);
    }
}

/**
 * @see WebBrowser
 */
public boolean setText(String html) {
    if (sendLocationEventChanging((int)System.currentTimeMillis(), "about:blank")) {
        WebkitOS.QWebView_setHtml(webViewHandle, html);
        return true;
    }
    else {
        return false;
    }
}

/**
 * @see WebBrowser
 */
public boolean setUrl(String url) {
    if (sendLocationEventChanging((int)System.currentTimeMillis(), url)) {
        WebkitOS.QWebView_setUrl(webViewHandle, url);
        return true;
    }
    else {
        return false;
    }
}

/**
 * @see WebBrowser
 */
public void stop() {
    WebkitOS.QWebView_stop(webViewHandle);
}

final boolean eventProcess( int widgetHandle, int eventType, int time, 
    int arg1, int arg2, int arg3, int arg4, int arg5, String arg6 ){
    if (browser.isDisposed()) {
        return false;
    }
    
    if (widgetHandle != webViewHandle) {
        return false;
    }
    
    switch (eventType) {
    case OS.QSIGNAL_WEBVIEW_LOADFINISHED:
        sendProgressEventCompleted(time);
        break;
    case OS.QSIGNAL_WEBVIEW_LOADPROGRESS:
        sendProgressEventChanged(time, arg1);
        break;
    case OS.QSIGNAL_WEBVIEW_LOADSTARTED:
        sendProgressEventChanged(time, 0);
        break;
    case OS.QSIGNAL_WEBVIEW_LINKCLICKED:
        sendLocationEventChanging(time, arg6);
        break;
    case OS.QSIGNAL_WEBVIEW_STATUSBARMESSAGE:
        sendStatusTextEventChanged(time, arg6);
        break;
    case OS.QSIGNAL_WEBVIEW_TITLECHANGED:
        sendTitleEventChanged(time, arg6);
        break;
    case OS.QSIGNAL_WEBVIEW_URLCHANGED:
        sendLocationEventChanged(time, arg6);
        break;
    default:
        break;
    }
    
    return false;
}

private void sendProgressEventChanged(int time, int current) {
    ProgressEvent e = new ProgressEvent(browser);
    e.display = browser.getDisplay();
    e.widget = browser;
    e.time = time;
    e.current = current; // %
    e.total = 100; // %
    for (int i = 0; i < progressListeners.length; i++) {
        progressListeners[i].changed(e);
    }
}

private void sendProgressEventCompleted(int time) {
    ProgressEvent e = new ProgressEvent(browser);
    e.display = browser.getDisplay();
    e.widget = browser;
    e.time = time;
    e.current = 100; // %
    e.total = 100; // %
    for (int i = 0; i < progressListeners.length; i++) {
        progressListeners[i].completed(e);
    }
}

private void sendStatusTextEventChanged(int time, String text) {
    StatusTextEvent e = new StatusTextEvent(browser);
    e.display = browser.getDisplay();
    e.widget = browser;
    e.time = time;
    e.text = text;
    for (int i = 0; i < statusTextListeners.length; i++) {
        statusTextListeners[i].changed(e);
    }
}

private void sendTitleEventChanged(int time, String text) {
    TitleEvent e = new TitleEvent(browser);
    e.display = browser.getDisplay();
    e.widget = browser;
    e.time = time;
    e.title = text;
    for (int i = 0; i < titleListeners.length; i++) {
        titleListeners[i].changed(e);
    }
}

private void sendLocationEventChanged(int time, String text) {
    LocationEvent e = new LocationEvent(browser);
    e.display = browser.getDisplay();
    e.widget = browser;
    e.time = time;
    e.location = text;
    e.doit = true;
    e.top = true;
    for (int i = 0; i < locationListeners.length; i++) {
        locationListeners[i].changed(e);
    }
}

private boolean sendLocationEventChanging(int time, String text) {
    LocationEvent e = new LocationEvent(browser);
    e.display = browser.getDisplay();
    e.widget = browser;
    e.time = time;
    e.location = text;
    e.doit = true;
    boolean globalDoIt = true;
    for (int i = 0; i < locationListeners.length; i++) {
        locationListeners[i].changing(e);
        globalDoIt &= e.doit;
    }
    return globalDoIt;
}
}
