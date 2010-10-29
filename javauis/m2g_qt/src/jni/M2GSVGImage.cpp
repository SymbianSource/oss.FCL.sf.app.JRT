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
#include "com_nokia_microedition_m2g_M2GSVGImage.h"
#include "MM2GSVGProxy.h"

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
// Java_com_nokia_microedition_m2g_M2GSVGImage::_dispatchMouseEvent
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::DispatchMouseEventL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document handle.
 * @param aX X coordinate.
 * @param aY Y coordinate
 * @param aElementHandle Target element handle
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGImage__1dispatchMouseEvent(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aDocumentHandle, jint aX, jint aY)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGImage ) _dispatchMouseEvent - begin");

    TInt elementHandle = M2G_INVALID_HANDLE;
    TInt err = KM2GNotOk;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
    {
        MM2GSVGProxy *  aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err, aProxy->DispatchMouseEventL(
                 STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle),
                 aX, aY, elementHandle););
    }
    M2G_DO_UNLOCK(aJni)

    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGImage ) _dispatchMouseEvent: %d - end", elementHandle);
    return elementHandle;
}
// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGImage::_focusOn
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::FocusOnL method.
 * @since Series S60 3.1
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document handle.
 * @param aElementHandle Target element handle
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGImage__1focusOn(
    JNIEnv* /* aJni */,
    jclass,
    jint aSvgProxyHandle,
    jint aDocumentHandle, jint aElementHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGImage ) _focusOn - begin");

    TInt err = KM2GNotOk;

    if (aSvgProxyHandle)
    {
        MM2GSVGProxy *     aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err, aProxy->FocusOnL(
                 STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle),
                 STATIC_CAST(TM2GSvgElementHandle, aElementHandle)););
    }

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGImage ) _focusOn: %d - end", err);
}


// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGImage::_focusOut
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::FocusOutL method.
 * @since Series S60 3.1
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document handle.
 * @param aElementHandle Target element handle
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGImage__1focusOut(
    JNIEnv* /* aJni */,
    jclass,
    jint aSvgProxyHandle,
    jint aDocumentHandle, jint aElementHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGImage ) _focusOut - begin");
    TInt err = KM2GNotOk;

    if (aSvgProxyHandle)
    {
        MM2GSVGProxy *    aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err,aProxy->FocusOutL(
                 STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle),
                 STATIC_CAST(TM2GSvgElementHandle, aElementHandle)););
    }

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGImage ) _focusOut: %d - end", err);

}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGImage::_getExternalListItem
// EXTENSION
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetExternalListItemL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document pointer.
 * @param aIndex External item index.
 * @param aItem External item
 * @throws Exception if not ok
 * JNI method
 */
JNIEXPORT jstring JNICALL
Java_com_nokia_microedition_m2g_M2GSVGImage__1getExternalListItem(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jint aIndex)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGImage ) _getExternalListItem - begin");
    TPtrC16 lItem;
    TInt err    = KM2GNotOk;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
    {
        MM2GSVGProxy * aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err, aProxy->GetExternalListItemL(
                 STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle),
                 aIndex,
                 lItem););
    }
    M2G_DO_UNLOCK(aJni)

    if ((M2GGeneral::CheckErrorCode(aJni, err) == KM2GOk) && (lItem.Length() > 0))
    {
        return CreateJavaString(*aJni, lItem);
    }

    return NULL;
}
// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGImage::_getExternalListSize
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetExternalListItemL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document handle.
 * @param aListSz List size to be returned.
 */
JNIEXPORT TInt JNICALL
Java_com_nokia_microedition_m2g_M2GSVGImage__1getExternalListSize(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aDocumentHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGImage ) _getExternalListSize - begin");

    TInt err           = KM2GNotOk;
    TInt listSz = 0;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
    {
        MM2GSVGProxy *  aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err, aProxy->GetExternalListSizeL(
                 STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle), listSz););
    }
    M2G_DO_UNLOCK(aJni)

    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGImage ) _getExternalListSize: %d - end", listSz);
    return listSz;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGImage::_getExternalListItem
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetExternalListItemL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document pointer.
 * @param aIndex Index.
 * @param aStrBuf Returns a string
 * @throws Exception if not ok
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGImage__1initViewport(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aDocumentHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGImage ) _initViewport - begin");

    TInt err           = KM2GNotOk;

    if (aSvgProxyHandle)
    {
        MM2GSVGProxy * aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err, aProxy->InitViewportL(
                 STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle)););
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGImage ) _initViewport: %d - end", err);

}

M2G_NS_END
