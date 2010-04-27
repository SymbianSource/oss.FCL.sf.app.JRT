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
#include "com_nokia_microedition_m2g_M2GSVGImage.h"
#include "MM2GSVGProxy.h"
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
LOCAL_C void DoDispatchMouseEventL(
    MM2GSVGProxy* aProxy,
    TInt aDocumentHandle,
    TInt aX,
    TInt aY,
    TInt* aElementHandle)
{
    aProxy->DispatchMouseEventL(
        STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle),
        aX, aY, *aElementHandle);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GSVGImage__1dispatchMouseEvent(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aDocumentHandle, jint aX, jint aY, jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGImage ) _dispatchMouseEvent - begin");

    TInt elementHandle = M2G_INVALID_HANDLE;
    TInt err = KM2GNotOk;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient *>(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pDispatchMouseEventL)(const TM2GSvgDocumentHandle&,
                    TInt,
                    TInt,
                    TM2GSvgElementHandle&);
            pDispatchMouseEventL DispatchMouseEventL = &MM2GSVGProxy::DispatchMouseEventL;
            MM2GSVGProxy *                 aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgDocumentHandle lDocumentHandle = STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle);
            TMethodWrapper4<MM2GSVGProxy,const TM2GSvgDocumentHandle&,TInt,TInt,TM2GSvgElementHandle& >
            DispatchMouseEvent(
                *aProxy,
                DispatchMouseEventL,
                lDocumentHandle,
                aX,
                aY,
                elementHandle
            );
            err = client->Execute(DispatchMouseEvent);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoDispatchMouseEventL,
                      JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle),
                      aDocumentHandle, aX, aY, &elementHandle);
        }
    }
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
 */
LOCAL_C void DoFocusOnL(
    MM2GSVGProxy* aProxy,
    TInt aDocumentHandle,
    TInt aElementHandle)
{
    aProxy->FocusOnL(
        STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle),
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle));
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGImage__1focusOn(
    JNIEnv* /* aJni */,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aDocumentHandle, jint aElementHandle, jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGImage ) _focusOn - begin");

    // TInt elementHandle = M2G_INVALID_HANDLE;
    // TInt err = KM2GNotOk;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient *>(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pFocusOnL)(
                const TM2GSvgDocumentHandle&,
                const TM2GSvgElementHandle&);
            pFocusOnL FocusOnL = &MM2GSVGProxy::FocusOnL;
            MM2GSVGProxy *                 aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgDocumentHandle lDocumentHandle = STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle);
            TM2GSvgElementHandle  lElementHandle  = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TMethodWrapper2<MM2GSVGProxy,
            const TM2GSvgDocumentHandle&,
            const TM2GSvgElementHandle& > FocusOn(
                *aProxy,
                FocusOnL,
                lDocumentHandle,
                lElementHandle
            );
            /*err = */
            client->Execute(FocusOn);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);

        if (eventSource && aSvgProxyHandle)
        {
            /*err =*/
            eventSource->ExecuteTrap(
                &DoFocusOnL,
                JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle),
                aDocumentHandle, aElementHandle);

        }
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
 */
LOCAL_C void DoFocusOutL(
    MM2GSVGProxy* aProxy,
    TInt aDocumentHandle,
    TInt aElementHandle)
{
    aProxy->FocusOutL(
        STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle),
        STATIC_CAST(TM2GSvgElementHandle, aElementHandle));
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGImage__1focusOut(
    JNIEnv* /* aJni */,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aDocumentHandle, jint aElementHandle, jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGImage ) _focusOut - begin");

    // TInt elementHandle = M2G_INVALID_HANDLE;
    // TInt err = KM2GNotOk;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient *>(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pFocusOutL)(
                const TM2GSvgDocumentHandle&,
                const TM2GSvgElementHandle&);
            pFocusOutL FocusOutL = &MM2GSVGProxy::FocusOutL;
            MM2GSVGProxy *                 aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgDocumentHandle lDocumentHandle = STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle);
            TM2GSvgElementHandle  lElementHandle  = STATIC_CAST(TM2GSvgElementHandle, aElementHandle);
            TMethodWrapper2<MM2GSVGProxy,
            const TM2GSvgDocumentHandle&,
            const TM2GSvgElementHandle& > FocusOut(
                *aProxy,
                FocusOutL,
                lDocumentHandle,
                lElementHandle
            );
            /*err = */
            client->Execute(FocusOut);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);

        if (eventSource && aSvgProxyHandle)
        {
            /*err = */
            eventSource->ExecuteTrap(
                &DoFocusOutL,
                JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle),
                aDocumentHandle, aElementHandle);
        }
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
 */
LOCAL_C void DoGetExternalListItemL(
    MM2GSVGProxy* aProxy,
    TInt aDocumentHandle,
    TInt aIndex,
    TPtrC16* aItem)
{
    TInt result = aProxy->GetExternalListItemL(
                      STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle),
                      aIndex,
                      *aItem);
}

/**
 * JNI method
 */
JNIEXPORT jstring JNICALL
Java_com_nokia_microedition_m2g_M2GSVGImage__1getExternalListItem(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jint aIndex,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGImage ) _getExternalListItem - begin");
    TPtrC16 lItem;
    TInt err    = KM2GNotOk;
    TInt result = KM2GNotOk;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient *>(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef TInt(MM2GSVGProxy::*pGetExternalListItemL)(
                const TM2GSvgDocumentHandle&,
                TInt,
                TPtrC16&);
            pGetExternalListItemL GetExternalListItemL = &MM2GSVGProxy::GetExternalListItemL;
            MM2GSVGProxy *                 aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            // TM2GSvgDocumentHandle lDocumentHandle = STATIC_CAST( TM2GSvgDocumentHandle, aDocumentHandle );
            TMethodWrapperR3<TInt,
            TInt,
            MM2GSVGProxy,
            const TM2GSvgDocumentHandle&,
            TInt,
            TPtrC16&>  GetExternalListItem(
                result,
                *aProxy,
                GetExternalListItemL,
                aDocumentHandle,
                aIndex,
                lItem
            );
            err = client->Execute(GetExternalListItem);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);

        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetExternalListItemL,
                      JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle),
                      aDocumentHandle,
                      aIndex,
                      &lItem);
            M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGImage ) _getExternalListItem: %d - end", err);
        }
    }
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
LOCAL_C void DoGetExternalListSizeL(
    MM2GSVGProxy* aProxy,
    TInt aDocumentHandle, TInt* aListSz)
{
    aProxy->GetExternalListSizeL(
        STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle), *aListSz);
}

/**
 * JNI method
 */
JNIEXPORT TInt JNICALL
Java_com_nokia_microedition_m2g_M2GSVGImage__1getExternalListSize(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGImage ) _getExternalListSize - begin");

    // TInt elementHandle = M2G_INVALID_HANDLE;
    TInt err           = KM2GNotOk;
    TInt listSz = 0;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient *>(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetExternalListSizeL)(
                const TM2GSvgDocumentHandle&,
                TInt&);
            pGetExternalListSizeL GetExternalListSizeL = &MM2GSVGProxy::GetExternalListSizeL;
            MM2GSVGProxy *                 aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgDocumentHandle lDocumentHandle = STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle);
            TMethodWrapper2<MM2GSVGProxy,
            const TM2GSvgDocumentHandle&,
            TInt& > GetExternalListSize(
                *aProxy,
                GetExternalListSizeL,
                lDocumentHandle,
                listSz
            );
            err = client->Execute(GetExternalListSize);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetExternalListSizeL,
                      JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle),
                      aDocumentHandle, &listSz);
        }
    }
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
 */
LOCAL_C void DoInitViewportL(
    MM2GSVGProxy* aProxy,
    TInt aDocumentHandle)
{
    aProxy->InitViewportL(
        STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle));
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGImage__1initViewport(
    JNIEnv*,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GSVGImage ) _initViewport - begin");
    // TInt elementHandle = M2G_INVALID_HANDLE;
    // TInt err           = KM2GNotOk;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient *>(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pInitViewportL)(
                const TM2GSvgDocumentHandle&);
            pInitViewportL InitViewportL = &MM2GSVGProxy::InitViewportL;
            MM2GSVGProxy *                 aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgDocumentHandle lDocumentHandle = STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle);
            TMethodWrapper1<MM2GSVGProxy,
            const TM2GSvgDocumentHandle&
            > InitViewport(
                *aProxy,
                InitViewportL,
                lDocumentHandle
            );
            /*err = */
            client->Execute(InitViewport);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            /*err =*/
            eventSource->ExecuteTrap(
                &DoInitViewportL,
                JavaUnhand< MM2GSVGProxy >(aSvgProxyHandle),
                aDocumentHandle);
        }
    }
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GSVGImage ) _initViewport: %d - end", err);

}

M2G_NS_END
