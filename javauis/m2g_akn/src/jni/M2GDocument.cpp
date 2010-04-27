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
#include "com_nokia_microedition_m2g_M2GDocument.h"
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
// Java_com_nokia_microedition_m2g_M2GDocument::_createDocument
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::CreateDocumentL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aData Document data
 * @param aHandle Document handle to be returned
 * @throws Exception if not ok
 */
LOCAL_C void DoCreateDocumentL(
    MM2GSVGProxy* aProxy,
    TPtrC16* aData,
    TInt* aHandle)
{
    aProxy->CreateDocumentL(*aData, *aHandle);
}

/**
 *
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1createDocument(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jstring aData,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _createDocument - begin");
    TInt handle = M2G_INVALID_HANDLE;
    TInt err    = KM2GNotOk;
    RJString data(*aJni, aData);

    if (aUiToolkit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pCreateDocumentL)(const TPtrC16& ,TInt&);
            pCreateDocumentL CreateDocumentL = &MM2GSVGProxy::CreateDocumentL;
            TPtrC16* aData = STATIC_CAST(TPtrC16*, &data);
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TMethodWrapper2<MM2GSVGProxy, const TPtrC16&, TInt& > DoCreateDocument(
                *aProxy,
                CreateDocumentL,
                *aData,
                handle);
            err = client->Execute(DoCreateDocument);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);

        // RJString is a subclass of TPtrC16
        RJString data(*aJni, aData);

        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoCreateDocumentL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      STATIC_CAST(TPtrC16*, &data),
                      &handle);
        }

    }
    handle = M2GGeneral::CheckErrorCodeAndHandle(aJni, err, handle, M2G_INVALID_HANDLE);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GDocument ) _createDocument: %d - end", handle);
    return handle;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GDocument::_createElementNS
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::CreateElementNsL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aType Element type id.
 * @param aDocumentHandle Document handle.
 * @param aHandle Element handle to be returned
 * @throws Exception if not ok
 */
LOCAL_C void DoCreateElementNsL(
    MM2GSVGProxy* aProxy,
    TInt16 aType,
    TInt aDocumentHandle,
    TInt* aHandle)
{
    aProxy->CreateElementNsL(
        aType,
        aDocumentHandle,
        *aHandle);
}

/**
 *
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1createElementNS(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jshort aType,
    jint aDocumentHandle,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _createElementNS - begin");

    // Execute native engine method
    TInt handle = M2G_INVALID_HANDLE;
    TInt err = KM2GNotOk;
    if (aUiToolkit)  // If eSWT is the current toolkit
    {
        // get access to function server
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            // Original function prototype
            typedef void (MM2GSVGProxy::*pCreateElementNsL)(const TInt16& ,const TInt&, TInt&);
            pCreateElementNsL CreateElementNsL = &MM2GSVGProxy::CreateElementNsL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TMethodWrapper3<MM2GSVGProxy, const TInt16&, const TInt&, TInt& > DoCreateElementNs(
                *aProxy,
                CreateElementNsL,
                aType,
                aDocumentHandle,
                handle);
            // call 'DoCreateElementNsL' on aSvgProxyHandle
            err = client->Execute(DoCreateElementNs);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoCreateElementNsL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aType,
                      aDocumentHandle,
                      &handle);

        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GDocument ) _createElementNS: %d - end", handle);
    return handle;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GDocument::_deleteDocument
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::DeleteDocumentL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document handle
 * @throws Exception if not ok
 */
LOCAL_C void DoDeleteDocumentL(MM2GSVGProxy* aProxy, TInt aDocumentHandle)
{
    aProxy->DeleteDocumentL(aDocumentHandle);
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1deleteDocument(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _deleteDocument - begin");
    TInt err = KM2GNotOk;
    if (aUiToolkit)  // If eSWT is the current toolkit
    {
        // get access to function server
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);

        if (client && aSvgProxyHandle)
        {
            // Original function prototype
            typedef void (MM2GSVGProxy::*pDeleteDocumentL)(const TInt&);

            // Get member function address
            pDeleteDocumentL DeleteDocumentL = &MM2GSVGProxy::DeleteDocumentL;

            // Extract the SVGProxy handle
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);

            // Use the Method wrapper for 1 Arg
            TMethodWrapper1<MM2GSVGProxy, const TInt& > DeleteDocument(
                *aProxy,
                DeleteDocumentL,
                aDocumentHandle);
            // call 'DeleteDocumentL' on aSvgProxyHandle
            err = client->Execute(DeleteDocument);
        }

    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle && aDocumentHandle)
        {
            // Execute native engine method
            err = eventSource->ExecuteTrap(
                      &DoDeleteDocumentL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aDocumentHandle);

        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _deleteDocument - end");
}


// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GDocument::_getElementById
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetElementByIdL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document pointer.
 * @param aId Element ID.
 * @param aHandle Element handle to be returned.
 */
LOCAL_C void DoGetElementByIdL(
    MM2GSVGProxy* aProxy,
    TInt aDocumentHandle,
    TPtrC16* aId,
    TInt* aHandle)
{
    aProxy->GetElementByIdL(aDocumentHandle, *aId, *aHandle);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1getElementById(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jstring aId,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _getElementById - begin");
    TInt handle = M2G_INVALID_HANDLE;
    TInt err    = KM2GNotOk;
    RJString id(*aJni, aId);
    if (aUiToolkit)
    {
        // get access to function server
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetElementByIdL)(const TInt&, const TPtrC16& ,TInt&);
            pGetElementByIdL GetElementByIdL = &MM2GSVGProxy::GetElementByIdL;
            TPtrC16* lId = STATIC_CAST(TPtrC16*, &id);
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TMethodWrapper3<MM2GSVGProxy, const TInt&, const TPtrC16&, TInt& > GetElementById(
                *aProxy,
                GetElementByIdL,
                aDocumentHandle,
                *lId,
                handle);
            err = client->Execute(GetElementById);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetElementByIdL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aDocumentHandle,
                      STATIC_CAST(TPtrC16*, &id),
                      &handle);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GDocument ) _getElementById: %d - end", handle);
    return handle;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GDocument::_getViewportHeight
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetViewportHeightL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document pointer.
 * @param aHeight Height to be returned.
 */
LOCAL_C void DoGetViewportHeightL(
    MM2GSVGProxy* aProxy,
    TInt aDocumentHandle,
    TInt* aHeight)
{
    aProxy->GetViewportHeightL(aDocumentHandle, *aHeight);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1getViewportHeight(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _getViewportHeight - begin");
    TInt err    = KM2GNotOk;
    TInt height = 0;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            // Original function prototype
            typedef void (MM2GSVGProxy::*pGetViewportHeightL)(const TInt& ,TInt&);
            pGetViewportHeightL GetViewportHeightL = &MM2GSVGProxy::GetViewportHeightL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TMethodWrapper2<MM2GSVGProxy, const TInt& ,TInt& > GetViewportHeight(
                *aProxy,
                GetViewportHeightL,
                aDocumentHandle,
                height);
            err = client->Execute(GetViewportHeight);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);

        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetViewportHeightL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aDocumentHandle,
                      &height);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GDocument ) _getViewportHeight: %d - end", height);
    return height;
}


// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GDocument::_getViewportWidth
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetViewportWidthL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document pointer.
 * @param aWidth Width to be returned.
 */
LOCAL_C void DoGetViewportWidthL(
    MM2GSVGProxy* aProxy,
    TInt aDocumentHandle,
    TInt* aWidth)
{
    aProxy->GetViewportWidthL(aDocumentHandle, *aWidth);
}

/**
 * Class: com_nokia_microedition_m2g_M2GDocument
 * Method: _getViewportWidth
 * Signature:
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1getViewportWidth(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _getViewportWidth - begin");
    TInt   err = KM2GNotOk;
    TInt width = 0;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetViewportWidthL)(const TInt& ,TInt&);
            pGetViewportWidthL GetViewportWidthL = &MM2GSVGProxy::GetViewportWidthL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TMethodWrapper2<MM2GSVGProxy, const TInt& ,TInt& > GetViewportWidth(
                *aProxy,
                GetViewportWidthL,
                aDocumentHandle,
                width);
            err = client->Execute(GetViewportWidth);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);

        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetViewportWidthL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aDocumentHandle,
                      &width);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GDocument ) _getViewportWidth: %d - end", width);
    return width;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GDocument::_isElementInDom
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::IsElementInDomL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document pointer.
 * @param aElementHandle Element pointer.
 * @param aIsElementInDom Result
 */
LOCAL_C void DoIsElementInDomL(
    MM2GSVGProxy* aProxy,
    TInt aDocumentHandle,
    TInt aElementHandle,
    TInt* aIsElementInDom)
{
    aProxy->IsElementInDomL(
        aDocumentHandle, aElementHandle, *aIsElementInDom);
}

/**
 * JNI method.
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1isElementInDOM(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jint aElementHandle,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI _isElementInDOM - begin");
    TInt isElementInDom = -1;
    TInt            err = KM2GNotOk;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pIsElementInDomL)(const TInt& ,const TInt&, TInt&);
            pIsElementInDomL IsElementInDomL = &MM2GSVGProxy::IsElementInDomL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TMethodWrapper3<MM2GSVGProxy, const TInt& ,const TInt&, TInt& > IsElementInDom(
                *aProxy,
                IsElementInDomL,
                aDocumentHandle,
                aElementHandle,
                isElementInDom);
            err = client->Execute(IsElementInDom);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);

        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoIsElementInDomL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aDocumentHandle,
                      aElementHandle,
                      &isElementInDom);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_1("M2G_DEBUG: JNI _isElementInDOM: %d - end", isElementInDom);
    return isElementInDom;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GDocument::_requestCompleted
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::RequestCompletedL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document pointer.
 * @param aURI Resource uri
 * @param aResourceData Resource data
 * @param aCompleted Result
 */
LOCAL_C void DoRequestCompletedL(
    MM2GSVGProxy* aProxy,
    TM2GSvgDocumentHandle aDocumentHandle,
    TPtrC16* aURI,
    TPtrC8* aResourceData,
    TInt* aCompleted)
{
    aProxy->RequestCompletedL(
        aDocumentHandle,
        *aURI,
        *aResourceData,
        *aCompleted);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GDocument ) DoRequestCompletedL: %d - end", *aCompleted);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1requestCompleted(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jstring aURI,
    jbyteArray aResourceData,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _requestCompleted - begin");

    CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
    RJString uri(*aJni, aURI);
    TInt err = KM2GNotOk;
    TPtrC8 lResData8;
    jbyte* resBytes = NULL;

    if (aResourceData)
    {
        resBytes  = aJni->GetByteArrayElements(aResourceData, NULL);
        lResData8.Set(REINTERPRET_CAST(TUint8*, resBytes), aJni->GetArrayLength(aResourceData));
    }

    TInt completed = -1;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            TPtrC16* pUri = STATIC_CAST(TPtrC16*, &uri);
            typedef void (MM2GSVGProxy::*pRequestCompletedL)(const TInt& ,const TPtrC16&, const TPtrC8&, TInt&);
            pRequestCompletedL RequestCompletedL = &MM2GSVGProxy::RequestCompletedL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TMethodWrapper4<MM2GSVGProxy, const TInt& ,const TPtrC16&, const TPtrC8&, TInt& > RequestCompleted(
                *aProxy,
                RequestCompletedL,
                aDocumentHandle,
                *pUri,
                lResData8,
                completed);
            err = client->Execute(RequestCompleted);
        }
    }
    else
    {
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoRequestCompletedL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aDocumentHandle,
                      STATIC_CAST(TPtrC16*, &uri),
                      &lResData8,
                      &completed);

        }

    }
    M2GGeneral::CheckErrorCode(aJni, err);
    if (aResourceData)
    {
        aJni->ReleaseByteArrayElements(aResourceData, resBytes, JNI_ABORT);   // don't copy back
    }

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GDocument ) _requestCompleted: %d - end", completed);

    return completed;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GDocument::_setViewportHeight
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::SetViewportHeightL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document pointer.
 * @param aHeight Height
 */
LOCAL_C void DoSetViewportHeightL(
    MM2GSVGProxy* aProxy,
    TInt aDocumentHandle,
    TInt aHeight)
{
    aProxy->SetViewportHeightL(
        (TM2GSvgDocumentHandle)aDocumentHandle, aHeight);
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1setViewportHeight(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jint aHeight,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _setViewportHeight - begin");
    TInt err = KM2GNotOk;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pSetViewportHeightL)(const TInt& ,TInt);
            pSetViewportHeightL SetViewportHeightL = &MM2GSVGProxy::SetViewportHeightL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TMethodWrapper2<MM2GSVGProxy, const TInt& ,TInt > SetViewportHeight(
                *aProxy,
                SetViewportHeightL,
                aDocumentHandle,
                aHeight);
            err = client->Execute(SetViewportHeight);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoSetViewportHeightL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aDocumentHandle,
                      aHeight);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _setViewportHeight - end");
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GDocument::_setViewportWidth
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::SetViewportWidthL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document pointer.
 * @param aWidth Width
 */
LOCAL_C void DoSetViewportWidthL(
    MM2GSVGProxy* aProxy,
    TInt aDocumentHandle,
    TInt aWidth)
{
    aProxy->SetViewportWidthL(
        (TM2GSvgDocumentHandle)aDocumentHandle, aWidth);
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1setViewportWidth(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jint aWidth,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _setViewportWidth - begin");
    TInt err = KM2GNotOk;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pSetViewportWidthL)(const TInt& ,TInt);
            pSetViewportWidthL SetViewportWidthL = &MM2GSVGProxy::SetViewportWidthL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TMethodWrapper2<MM2GSVGProxy, const TInt& ,TInt > SetViewportWidth(
                *aProxy,
                SetViewportWidthL,
                aDocumentHandle,
                aWidth);
            err = client->Execute(SetViewportWidth);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoSetViewportWidthL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aDocumentHandle,
                      aWidth);

        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _setViewportWidth - end");
}

M2G_NS_END
