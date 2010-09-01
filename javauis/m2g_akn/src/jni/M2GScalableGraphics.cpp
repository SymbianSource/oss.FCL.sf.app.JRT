/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  JNI methods
*
*/

// INCLUDE FILES
#include "CM2GEventSource.h"
#include "com_nokia_microedition_m2g_M2GScalableGraphics.h"
#include "CM2GRenderContext.h"
#include "MM2GSVGProxy.h" // TM2GRenderRect
#include <methodwrappers.h>

M2G_NS_START

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

// ================================ FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GScalableGraphics::_bind
// -----------------------------------------------------------------------------
/**
 * Calls MM2GRenderContext::BindL method.
 * @since Series S60 3.0
 * @param aRenderContextHandle Render context handle.
 * @param aTargetHandle Target graphics handle
 * @throws exception if not ok
 */
LOCAL_C void DoBindL(TInt aRenderContextHandle, TInt aTargetHandle, TBool aUiToolkit)
{
    MM2GRenderContext* rc = JavaUnhand<MM2GRenderContext>(aRenderContextHandle);
    rc->BindL(aTargetHandle, aUiToolkit);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GScalableGraphics__1bind(
    JNIEnv* aJni,
    jobject,
    jint aEventSourceHandle,
    jint aRenderContextHandle,
    jint aTargetHandle,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GScalableGraphics ) _bind - begin");
    TInt err = KM2GNotOk;
    if (aUiToolkit)  // If the current toolkit is eSWT.
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aRenderContextHandle)
        {
            typedef void (MM2GRenderContext::*pBindL)(TInt&, TInt);
            pBindL DoBindL = &MM2GRenderContext::BindL;
            MM2GRenderContext* rc = JavaUnhand<MM2GRenderContext>(aRenderContextHandle);
            TMethodWrapper2<MM2GRenderContext, TInt& ,TInt > DoBind(
                *rc,
                DoBindL,
                aTargetHandle,
                aUiToolkit);
            err = client->Execute(DoBind);
        }
    }
    else
    {
        TInt aUiToolkit = 0; // The current toolkit is LCDUI
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);

        if (eventSource && aRenderContextHandle)
        {

            err = eventSource->ExecuteTrap(
                      &DoBindL,
                      aRenderContextHandle,
                      aTargetHandle,
                      aUiToolkit);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GScalableGraphics ) _bind: %d - end", err);
    return err;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GScalableGraphics::_createRenderContext
// -----------------------------------------------------------------------------
/**
 * Calls MM2GRenderContext::NewL method.
 * @since Series S60 3.0
 * @param aSvgProxyHandle Proxy instance.
 * @param aHandle Render context handle.
 * @throws exception if not ok
 */
LOCAL_C void DoCreateRenderContextL(TInt* aHandle, MM2GSVGProxy* aSvgProxyHandle)
{
    MM2GRenderContext* handle = CM2GRenderContext::NewL(aSvgProxyHandle);
    *aHandle = JavaMakeHandle(handle);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GScalableGraphics__1createRenderContext(
    JNIEnv* aJni,
    jobject,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GScalableGraphics ) _createRenderContext - begin");
    TInt handle = M2G_INVALID_HANDLE;
    TInt  err = KM2GNotOk;
    if (aUiToolkit)  // If the current toolkit is eSWT.
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (*pDoCreateRenderContextL)(TInt*,MM2GSVGProxy*);
            pDoCreateRenderContextL lDoCreateRenderContextL = DoCreateRenderContextL;
            TSwtM2gMethodWrapper2<TInt,MM2GSVGProxy> DoCreateRenderContext(
                lDoCreateRenderContextL,
                &handle,
                JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle)
            );
            err = client->Execute(DoCreateRenderContext);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);

        if (eventSource && aSvgProxyHandle)
        {
            err =   eventSource->ExecuteTrap(
                        &DoCreateRenderContextL,
                        &handle,
                        JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle));
        }
    }
    handle = M2GGeneral::CheckErrorCodeAndHandle(
                 aJni, err, handle, err);

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GScalableGraphics ) _createRenderContext: %d - end", handle);
    return handle;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GScalableGraphics::_deleteRenderContext
// -----------------------------------------------------------------------------
/**
 * Deletes the object
 * @since Series S60 3.0
 * @param aRenderContextHandle Render context handle
 */
LOCAL_C void DoDeleteRenderContext(TInt aRenderContextHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GScalableGraphics ) DoDeleteRenderContext");
    MM2GRenderContext* rc = JavaUnhand<MM2GRenderContext>(aRenderContextHandle);
    delete rc;
}
/**
 * Class: com_nokia_microedition_m2g_M2GScalableGraphics
 * Method: deleteRenderContext
 * Signature:
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GScalableGraphics__1deleteRenderContext(
    JNIEnv*,
    jobject,
    jint aEventSourceHandle,
    jint aRenderContextHandle,
    jboolean aUIToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GScalableGraphics ) _deleteRenderContext - begin");
    // TInt err = KM2GNotOk;
    if (aUIToolkit)  // If the current toolkit is eSWT.
    {
        MSwtClient* client  = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aRenderContextHandle)
        {
            typedef void (*pDoDeleteRenderContext)(TInt aRenderContextHandle);
            pDoDeleteRenderContext lDoDeleteRenderContext = DoDeleteRenderContext;
            _TSwtM2gMethodWrapper1<TInt> DoDeleteRenderContext(
                lDoDeleteRenderContext,
                aRenderContextHandle
            );
            /*err = */
            client->Execute(DoDeleteRenderContext);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aRenderContextHandle)
        {
            eventSource->ExecuteV(
                &DoDeleteRenderContext,
                aRenderContextHandle);
        }
    }
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GScalableGraphics ) _deleteRenderContext - end");
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GScalableGraphics::_release
// -----------------------------------------------------------------------------
/**
 * Calls MM2GRenderContext::ReleaseL method.
 * @since Series S60 3.0
 * @param aRenderContextHandle Context handle
 * @return KM2GOk if ok
 */
LOCAL_C void DoReleaseL(TInt aRenderContextHandle)
{
    MM2GRenderContext* rc = JavaUnhand<MM2GRenderContext>(aRenderContextHandle);
    rc->ReleaseL();
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GScalableGraphics__1release(
    JNIEnv* aJni,
    jobject,
    jint aEventSourceHandle,
    jint aRenderContextHandle,
    jboolean aUIToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GScalableGraphics ) _release - begin");
    CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
    TInt err = KM2GNotOk;
    if (aUIToolkit)  // If the current toolkit is eSWT.
    {
        MSwtClient* client  = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aRenderContextHandle)
        {
            typedef void (MM2GRenderContext::*pReleaseL)();
            pReleaseL ReleaseL = &MM2GRenderContext::ReleaseL;
            MM2GRenderContext* rc = JavaUnhand<MM2GRenderContext>(aRenderContextHandle);
            TMethodWrapper0<MM2GRenderContext> Release(
                *rc,
                ReleaseL
            );
            err = client->Execute(Release);
        }
    }
    else
    {
        if (eventSource && aRenderContextHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoReleaseL,
                      aRenderContextHandle);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GScalableGraphics ) _release: %d - end", err);
    return err;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GScalableGraphics::_render
// -----------------------------------------------------------------------------
/**
 * Calls MM2GRenderContext::RenderL method.
 * @since Series S60 3.0
 * @param aRenderContextHandle Context handle
 * @param aDocumentHandle Document handle.
 * @param aSvgW Svg width.
 * @param aSvgH Svg heigth.
 * @param aCurrentTime Current time.
 * @throws Exception if not ok
 */
LOCAL_C void DoRenderL(
    TInt aRenderContextHandle,
    TInt aDocumentHandle,
    TM2GRenderRect* aRr,
    TInt aSvgW,
    TInt aSvgH,
    float* aCurrentTime
)
{
    MM2GRenderContext* rc = JavaUnhand<MM2GRenderContext>(aRenderContextHandle);

    rc->RenderLCDUIL(aDocumentHandle, *aCurrentTime, aSvgW, aSvgH, *aRr);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GScalableGraphics__1renderLCDUI(
    JNIEnv* aJni,
    jobject,
    jint aEventSourceHandle,
    jint aRenderContextHandle,
    jint aDocumentHandle,
    jint aX, jint aY,
    jint aClipX, jint aClipY, jint aClipW, jint aClipH,
    jint aSvgW, jint aSvgH,
    jfloat aCurrentTime)
{
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GScalableGraphics ) _render, time:%f - begin", aCurrentTime);

    TM2GRenderRect rr(aX, aY, aClipX, aClipY, aClipW, aClipH);

    TInt err = KM2GNotOk;

    CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
    if (eventSource && aRenderContextHandle && aDocumentHandle)
    {

        err = eventSource->ExecuteTrap(&DoRenderL,
                                       aRenderContextHandle,
                                       aDocumentHandle,
                                       &rr,
                                       aSvgW,
                                       aSvgH,
                                       &aCurrentTime);
    }

    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GScalableGraphics ) _render: %d - end", err);

    return err;
}
JNIEXPORT jintArray JNICALL
Java_com_nokia_microedition_m2g_M2GScalableGraphics__1renderESWT(
    JNIEnv* aJni,
    jobject,
    jint aEventSourceHandle,
    jint aRenderContextHandle,
    jint aDocumentHandle,
    jint aX, jint aY,
    jint aClipX, jint aClipY, jint aClipW, jint aClipH,
    jint aSvgW, jint aSvgH,
    jfloat aCurrentTime,
    jint aUseNativeClear
)
{
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GScalableGraphics ) _render, time:%f - begin", aCurrentTime);
    TM2GRenderRect rr(aX, aY, aClipX, aClipY, aClipW, aClipH);
    // TInt err = KM2GNotOk;
    jintArray returnDataJava = aJni->NewIntArray(10);
    MSwtClient* client  = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
    if (client && aRenderContextHandle && aDocumentHandle)
    {
        MM2GRenderContext* rc = JavaUnhand<MM2GRenderContext>(aRenderContextHandle);
        typedef void (MM2GRenderContext::*pRenderESWTL)(
            TM2GSvgDocumentHandle&,
            const TReal32,
            TInt aSvgW,
            TInt aSvgH,
            TM2GRenderRect&,
            MSwtClient*,
            TBool,
            TInt*);
        pRenderESWTL RenderESWTL = &MM2GRenderContext::RenderESWTL;
        TInt returnData[10];
        TMethodWrapper8<MM2GRenderContext,
        TInt&,const TReal32,TInt,TInt,
        TM2GRenderRect&,MSwtClient*,TBool,TInt*> RenderESWT(
            *rc,
            RenderESWTL,
            aDocumentHandle,
            aCurrentTime,
            aSvgW,
            aSvgH,
            rr,
            client,
            aUseNativeClear,
            returnData
        );
        /*err = */
        client->Execute(RenderESWT);
        if (returnDataJava != NULL)
            aJni->SetIntArrayRegion(returnDataJava, 0, 10, const_cast<TInt*>(returnData));
    }
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GScalableGraphics ) _render: %d - end", err);
    return returnDataJava;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GScalableGraphics::_setRenderingQuality
// -----------------------------------------------------------------------------
/**
 * Calls MM2GRenderContext::SetRenderingQuality method.
 * @since Series S60 3.0
 * @param aRenderContextHandle Context handle
 * @param aMode
 * @throws Exception if not ok
 */
LOCAL_C void DoSetRenderingQualityL(
    TInt aRenderContextHandle, TInt aMode)
{
    MM2GRenderContext* rc = JavaUnhand<MM2GRenderContext>(aRenderContextHandle);
    rc->SetRenderingQualityL(aMode);
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GScalableGraphics__1setRenderingQuality(
    JNIEnv* aJni,
    jobject,
    jint aEventSourceHandle,
    jint aRenderContextHandle, jint aMode, jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GScalableGraphics ) _setRenderingQuality - begin");
    TInt err = KM2GNotOk;
    if (aUiToolkit)  // If the current Ui Toolkit is eSWT.
    {
        MSwtClient* client  = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aRenderContextHandle)
        {
            typedef void (MM2GRenderContext::*pSetRenderingQualityL)(TInt);
            pSetRenderingQualityL SetRenderingQualityL = &MM2GRenderContext::SetRenderingQualityL;
            MM2GRenderContext* rc = JavaUnhand<MM2GRenderContext>(aRenderContextHandle);
            TMethodWrapper1<MM2GRenderContext,TInt> SetRenderingQuality(
                *rc,
                SetRenderingQualityL,
                aMode);
            err = client->Execute(SetRenderingQuality);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aRenderContextHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoSetRenderingQualityL,
                      aRenderContextHandle, aMode);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GScalableGraphics ) _setRenderingQuality - end");
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GScalableGraphics::_setTransparency
// -----------------------------------------------------------------------------
/**
 * Calls MM2GRenderContext::SetTransparency method.
 * @since Series S60 3.0
 * @param aRenderContextHandle Context handle
 * @param aAplha -
 */
LOCAL_C void DoSetTransparencyL(
    TInt aRenderContextHandle,
    TReal32* aAlpha)
{
    MM2GRenderContext* rc = JavaUnhand<MM2GRenderContext>(aRenderContextHandle);
    rc->SetTransparency(*aAlpha);
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GScalableGraphics__1setTransparency(
    JNIEnv* aJni,
    jobject,
    jint aEventSourceHandle,
    jint aRenderContextHandle,
    jfloat aAlpha,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GScalableGraphics ) _setTransparency - begin");
    TInt err = KM2GNotOk;
    if (aUiToolkit)  // If the current Ui Toolkit is eSWT.
    {
        MSwtClient* client  = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aRenderContextHandle)
        {
            typedef void (MM2GRenderContext::*pSetTransparency)(TReal32);
            pSetTransparency lSetTransparency = &MM2GRenderContext::SetTransparency;
            MM2GRenderContext* rc = JavaUnhand<MM2GRenderContext>(aRenderContextHandle);
            TMethodWrapper1<MM2GRenderContext,TReal32> SetTransparency(
                *rc,
                lSetTransparency,
                aAlpha);
            err = client->Execute(SetTransparency);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aRenderContextHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoSetTransparencyL,
                      aRenderContextHandle,
                      REINTERPRET_CAST(TReal32*, &aAlpha));
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GScalableGraphics ) _setTransparency - end");
}

M2G_NS_END



