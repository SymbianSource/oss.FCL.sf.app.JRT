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

#ifndef QT_NO_WEBKIT
#include <QWebView>
#include <QWebFrame>
#include <QWebHistory>
#endif

#include "org_eclipse_swt_internal_qt_webkit_OS_0005fwebkit.h"
#include "swt.h"
#include "swtlog.h"
#include "swtapplication.h"
#include "jniutils.h"

#define OS_NATIVE(func) Java_org_eclipse_swt_internal_qt_webkit_OS_1webkit_##func

using namespace Java::eSWT;

//
// QWebView
//
JNIEXPORT jint JNICALL OS_NATIVE( QWebView_1new )
    ( JNIEnv* aJniEnv , jclass )
    {
#ifndef QT_NO_WEBKIT
    QWebView* browser = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
        browser = new QWebView();
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( browser );
#endif
#ifdef QT_NO_WEBKIT
    swtApp->jniUtils().Throw( aJniEnv, ESwtErrorNotImplemented );
    return 0;
#endif
    }

JNIEXPORT void JNICALL OS_NATIVE( QWebView_1back )
    ( JNIEnv* aJniEnv , jclass, jint
#ifndef QT_NO_WEBKIT
aHandle
#endif
)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
#ifndef QT_NO_WEBKIT
        HANDLE_TO_POINTER( QWebView*, browser, aHandle );
        browser->back();
#endif
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWebView_1forward )
    ( JNIEnv* aJniEnv , jclass, jint
#ifndef QT_NO_WEBKIT
aHandle
#endif
)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
#ifndef QT_NO_WEBKIT
        HANDLE_TO_POINTER( QWebView*, browser, aHandle );
        browser->forward();
#endif
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWebView_1reload )
    ( JNIEnv* aJniEnv , jclass, jint
#ifndef QT_NO_WEBKIT
aHandle
#endif
)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
#ifndef QT_NO_WEBKIT
        HANDLE_TO_POINTER( QWebView*, browser, aHandle );
        browser->reload();
#endif
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWebView_1setHtml )
    ( JNIEnv* aJniEnv , jclass, jint
#ifndef QT_NO_WEBKIT
aHandle
#endif
, jstring
#ifndef QT_NO_WEBKIT
aText
#endif
)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
#ifndef QT_NO_WEBKIT
        HANDLE_TO_POINTER( QWebView*, browser, aHandle );
        browser->setHtml( swtApp->jniUtils().JavaStringToQString( aJniEnv, aText ) );
#endif
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWebView_1setUrl )
    ( JNIEnv* aJniEnv , jclass, jint
#ifndef QT_NO_WEBKIT
aHandle
#endif
, jstring
#ifndef QT_NO_WEBKIT
aText
#endif
)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
#ifndef QT_NO_WEBKIT
        HANDLE_TO_POINTER( QWebView*, browser, aHandle );
        browser->setUrl( QUrl( swtApp->jniUtils().JavaStringToQString( aJniEnv, aText ) ) );
#endif
        }
    SWT_CATCH
    }

JNIEXPORT void JNICALL OS_NATIVE( QWebView_1stop )
    ( JNIEnv* aJniEnv , jclass, jint
#ifndef QT_NO_WEBKIT
aHandle
#endif
)
    {
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
#ifndef QT_NO_WEBKIT
        HANDLE_TO_POINTER( QWebView*, browser, aHandle );
        browser->stop();
#endif
        }
    SWT_CATCH
    }

JNIEXPORT jstring JNICALL OS_NATIVE( QWebView_1swt_1backUrl )
    ( JNIEnv* aJniEnv , jclass, jint
#ifndef QT_NO_WEBKIT
aHandle
#endif
)
    {
    jstring res = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
#ifndef QT_NO_WEBKIT
        HANDLE_TO_POINTER( QWebView*, browser, aHandle );
        QWebHistory* history = browser->history();
        if ( history && history->canGoBack() )
            {
            res = swtApp->jniUtils().QStringToJavaString( aJniEnv, history->backItem().url().toString() );
            }
#endif
        }
    SWT_CATCH
    return res;
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QWebView_1swt_1canGoBack )
    ( JNIEnv* aJniEnv , jclass, jint
#ifndef QT_NO_WEBKIT
aHandle
#endif
)
    {
    jboolean res = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
#ifndef QT_NO_WEBKIT
        HANDLE_TO_POINTER( QWebView*, browser, aHandle );
        QWebHistory* history = browser->history();
        if ( history )
            {
            res = history->canGoBack();
            }
#endif
        }
    SWT_CATCH
    return res;
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QWebView_1swt_1canGoForward )
    ( JNIEnv* aJniEnv , jclass, jint
#ifndef QT_NO_WEBKIT
aHandle
#endif
)
    {
    jboolean res = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
#ifndef QT_NO_WEBKIT
        HANDLE_TO_POINTER( QWebView*, browser, aHandle );
        QWebHistory* history = browser->history();
        if ( history )
            {
            res = history->canGoForward();
            }
#endif
        }
    SWT_CATCH
    return res;
    }

JNIEXPORT jboolean JNICALL OS_NATIVE( QWebView_1swt_1evaluateJavaScript )
    ( JNIEnv* aJniEnv , jclass, jint
#ifndef QT_NO_WEBKIT
aHandle
#endif
, jstring
#ifndef QT_NO_WEBKIT
aText
#endif
)
    {
    jboolean res = false;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
#ifndef QT_NO_WEBKIT
        HANDLE_TO_POINTER( QWebView*, browser, aHandle );
        QWebPage* page = browser->page();
        if ( page )
            {
            QWebFrame* frame = page->currentFrame();
            if ( !frame )
                {
                frame = page->mainFrame();
                }
            if ( frame )
                {
                res = ( frame->evaluateJavaScript( swtApp->jniUtils().JavaStringToQString( aJniEnv, aText ) ) ).toBool();
                }
            }
#endif
        }
    SWT_CATCH
    return res;
    }

JNIEXPORT jstring JNICALL OS_NATIVE( QWebView_1swt_1forwardUrl )
    ( JNIEnv* aJniEnv , jclass, jint
#ifndef QT_NO_WEBKIT
aHandle
#endif
)
    {
    jstring res = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
#ifndef QT_NO_WEBKIT
        HANDLE_TO_POINTER( QWebView*, browser, aHandle );
        QWebHistory* history = browser->history();
        if ( history && history->canGoForward() )
            {
            res = swtApp->jniUtils().QStringToJavaString( aJniEnv, history->forwardItem().url().toString() );
            }
#endif
        }
    SWT_CATCH
    return res;
    }

JNIEXPORT jstring JNICALL OS_NATIVE( QWebView_1url )
    ( JNIEnv* aJniEnv , jclass, jint
#ifndef QT_NO_WEBKIT
aHandle
#endif
)
    {
    jstring res = NULL;
    SWT_TRY
        {
        SWT_LOG_JNI_CALL();
#ifndef QT_NO_WEBKIT
        HANDLE_TO_POINTER( QWebView*, browser, aHandle );
        res = swtApp->jniUtils().QStringToJavaString( aJniEnv, browser->url().toString() );
#endif
        }
    SWT_CATCH
    return res;
    }
