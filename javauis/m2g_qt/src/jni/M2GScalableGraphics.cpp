/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "com_nokia_microedition_m2g_M2GScalableGraphics.h"
#include "CM2GRenderContext.h"
#include "MM2GSVGProxy.h" // TM2GRenderRect
#include <graphics.h>

M2G_NS_START
#include "M2GUtils.h"

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

JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GScalableGraphics__1bind(
    JNIEnv* aJni,
    jobject,
    jint aRenderContextHandle,
    jint aTargetHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GScalableGraphics ) _bind - begin");

    TInt err = KM2GNotOk;

    M2G_DO_LOCK

    if (aRenderContextHandle)
    {
        MM2GRenderContext* rc = JavaUnhand<MM2GRenderContext>(aRenderContextHandle);
        TRAP(err,rc->BindL(aTargetHandle););
    }

    M2G_DO_UNLOCK(aJni)

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

JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GScalableGraphics__1createRenderContext(
    JNIEnv* aJni,
    jobject,
    jint aSvgProxyHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GScalableGraphics ) _createRenderContext - begin");
    TInt handle = M2G_INVALID_HANDLE;
    TInt  err = KM2GNotOk;

    M2G_DO_LOCK

    if (aSvgProxyHandle)
    {
        TRAP(err, MM2GRenderContext* rchandle = CM2GRenderContext::NewL(JavaUnhand<MM2GSVGProxy> (aSvgProxyHandle));  handle = JavaMakeHandle(rchandle););
    }
    M2G_DO_UNLOCK(aJni)

    handle = M2GGeneral::CheckErrorCodeAndHandle(aJni, err, handle, err);

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GScalableGraphics ) _createRenderContext: %d - end", handle);

    return handle;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GScalableGraphics::_deleteRenderContext
// -----------------------------------------------------------------------------
/**
 * Class: com_nokia_microedition_m2g_M2GScalableGraphics
 * Method: deleteRenderContext
 * Signature:
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GScalableGraphics__1deleteRenderContext(
    JNIEnv* aJni,
    jobject,
    jint aRenderContextHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GScalableGraphics ) _deleteRenderContext - begin");

    M2G_DO_LOCK

    if (aRenderContextHandle)
    {
        MM2GRenderContext* rc = JavaUnhand<MM2GRenderContext>(aRenderContextHandle);
        delete rc;
    }
    M2G_DO_UNLOCK(aJni)

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

JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GScalableGraphics__1release(
    JNIEnv* aJni,
    jobject,
    jint /*aSurfaceHandle*/,
    jint aRenderContextHandle)
{

    // Release used target surface
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GScalableGraphics ) _release - begin");
    TInt err = KM2GNotOk;
    M2G_DO_LOCK

    if (aRenderContextHandle)
    {
        MM2GRenderContext* rc = JavaUnhand<MM2GRenderContext>(aRenderContextHandle);
        TRAP(err,rc->ReleaseL(););
    }
    M2G_DO_UNLOCK(aJni)

    M2GGeneral::CheckErrorCode(aJni, err);

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

JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GScalableGraphics__1renderLCDUI(
    JNIEnv* aJni,
    jobject,
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

    M2G_DO_LOCK

    if (aRenderContextHandle && aDocumentHandle)
    {
        MM2GRenderContext* rc = JavaUnhand<MM2GRenderContext>(aRenderContextHandle);
        TRAP(err,rc->RenderLCDUIL(aDocumentHandle, aCurrentTime, aSvgW, aSvgH, rr););
    }
    M2G_DO_UNLOCK(aJni)

    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GScalableGraphics ) _render: %d - end", err);
    return err;
}


JNIEXPORT jintArray JNICALL
Java_com_nokia_microedition_m2g_M2GScalableGraphics__1renderESWT(
    JNIEnv* aJni,
    jobject,
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
    TInt err = KM2GNotOk;
    jintArray returnDataJava  = aJni->NewIntArray(10);

    M2G_DO_LOCK

    TInt returnData[10];

    if (aRenderContextHandle && aDocumentHandle)
    {
        MM2GRenderContext* rc = JavaUnhand<MM2GRenderContext>(aRenderContextHandle);
        TRAP(err,rc->RenderESWTL(aDocumentHandle,aCurrentTime,aSvgW,aSvgH,rr,aUseNativeClear,returnData););
    }
    M2G_DO_UNLOCK(aJni)


    if (returnDataJava != NULL)
        aJni->SetIntArrayRegion(returnDataJava, 0, 10, const_cast<TInt*>(returnData));

    M2GGeneral::CheckErrorCode(aJni, err);
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
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GScalableGraphics__1setRenderingQuality(
    JNIEnv* aJni,
    jobject,
    jint aRenderContextHandle, jint aMode)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GScalableGraphics ) _setRenderingQuality - begin");
    TInt err = KM2GNotOk;

    M2G_DO_LOCK

    if (aRenderContextHandle)
    {
        MM2GRenderContext* rc = JavaUnhand<MM2GRenderContext>(aRenderContextHandle);
        TRAP(err,rc->SetRenderingQualityL(aMode););
    }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GScalableGraphics__1setTransparency(
    JNIEnv* aJni,
    jobject,
    jint aRenderContextHandle,
    jfloat aAlpha)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GScalableGraphics ) _setTransparency - begin");
    TInt err = KM2GNotOk;

    M2G_DO_LOCK

    if (aRenderContextHandle)
    {
        MM2GRenderContext* rc = JavaUnhand<MM2GRenderContext>(aRenderContextHandle);
        TRAP(err,rc->SetTransparency((TReal32)aAlpha););
    }
    M2G_DO_UNLOCK(aJni)

    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GScalableGraphics ) _setTransparency - end");
}

M2G_NS_END



