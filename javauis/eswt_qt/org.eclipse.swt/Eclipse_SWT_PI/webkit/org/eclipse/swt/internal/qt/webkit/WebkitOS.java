/*******************************************************************************
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/
package org.eclipse.swt.internal.qt.webkit;

import org.eclipse.swt.internal.Library;

/**
 * Native methods requiring QtWebkit. They are separated to avoid
 * loading the related libraries until they are needed. 
 */
public final class WebkitOS {
    static {
        Library.loadLibrary("eswtqtwebkit");
    }

    //
    // QWebView
    //
    public static final native int QWebView_new();
    public static final native void QWebView_back(int handle);
    public static final native void QWebView_forward(int handle);
    public static final native void QWebView_reload(int handle);
    public static final native void QWebView_setHtml(int handle, String html);
    public static final native void QWebView_setUrl(int handle, String url);
    public static final native void QWebView_stop(int handle);
    public static final native String QWebView_swt_backUrl(int handle);
    public static final native boolean QWebView_swt_canGoBack(int handle);
    public static final native boolean QWebView_swt_canGoForward(int handle);
    public static final native boolean QWebView_swt_evaluateJavaScript(int handle, String script);
    public static final native String QWebView_swt_forwardUrl(int handle);
    public static final native String QWebView_url(int handle);
}
