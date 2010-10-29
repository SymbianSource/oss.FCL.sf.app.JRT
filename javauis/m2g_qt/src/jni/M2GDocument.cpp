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
#include "com_nokia_microedition_m2g_M2GDocument.h"
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
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1createDocument(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jstring aData)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _createDocument - begin");

    TInt handle = M2G_INVALID_HANDLE;
    TInt err    = KM2GNotOk;
    JStringUtils data(*aJni, aData);

    TPtrC16* bData = STATIC_CAST(TPtrC16*, &data);

    M2G_DO_LOCK
    if (aSvgProxyHandle)
    {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err,aProxy->CreateDocumentL(*bData, handle););
    }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1createElementNS(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jshort aType,
    jint aDocumentHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _createElementNS - begin");

    // Execute native engine method
    TInt handle = M2G_INVALID_HANDLE;
    TInt err = KM2GNotOk;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
    {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err,aProxy->CreateElementNsL(
                 aType,
                 aDocumentHandle,
                 handle););

    }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1deleteDocument(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aDocumentHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _deleteDocument - begin");
    TInt err = KM2GNotOk;

    M2G_DO_LOCK
    // Extract the SVGProxy handle
    if (aSvgProxyHandle)
    {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err,aProxy->DeleteDocumentL(aDocumentHandle););
    }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1getElementById(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jstring aId)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _getElementById - begin");
    TInt handle = M2G_INVALID_HANDLE;
    TInt err    = KM2GNotOk;

    JStringUtils id(*aJni, aId);
    TPtrC16* lId = STATIC_CAST(TPtrC16*, &id);

    M2G_DO_LOCK
    if (aSvgProxyHandle)
    {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err,aProxy->GetElementByIdL(aDocumentHandle, *lId, handle););
    }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1getViewportHeight(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aDocumentHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _getViewportHeight - begin");
    TInt err    = KM2GNotOk;
    TInt height = 0;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
    {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err,aProxy->GetViewportHeightL(aDocumentHandle, height););
    }
    M2G_DO_UNLOCK(aJni)

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
 * Class: com_nokia_microedition_m2g_M2GDocument
 * Method: _getViewportWidth
 * Signature:
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1getViewportWidth(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aDocumentHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _getViewportWidth - begin");
    TInt   err = KM2GNotOk;
    TInt width = 0;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
    {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err,aProxy->GetViewportWidthL(aDocumentHandle, width););
    }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1isElementInDOM(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jint aElementHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI _isElementInDOM - begin");
    TInt isElementInDom = -1;
    TInt            err = KM2GNotOk;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
    {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err,aProxy->IsElementInDomL(
                 aDocumentHandle, aElementHandle, isElementInDom););
    }
    M2G_DO_UNLOCK(aJni)

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
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1requestCompleted(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jstring aURI,
    jbyteArray aResourceData)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _requestCompleted - begin");

    //RJString uri(*aJni, aURI);
    JStringUtils uri(*aJni, aURI);
    TInt err = KM2GNotOk;
    TPtrC8 lResData8;
    jbyte* resBytes = NULL;

    if (aResourceData)
    {
        resBytes  = aJni->GetByteArrayElements(aResourceData, NULL);
        lResData8.Set(REINTERPRET_CAST(TUint8*, resBytes), aJni->GetArrayLength(aResourceData));
    }

    TInt completed = -1;
    TPtrC16* pUri = STATIC_CAST(TPtrC16*, &uri);

    M2G_DO_LOCK

    if (aSvgProxyHandle)
    {

        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);

        TRAP(err,    aProxy->RequestCompletedL(
                 aDocumentHandle,
                 *pUri,
                 lResData8,
                 completed););
    }
    M2G_DO_UNLOCK(aJni)

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
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1setViewportHeight(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jint aHeight)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _setViewportHeight - begin");
    TInt err = KM2GNotOk;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
    {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err,aProxy->SetViewportHeightL(
                 (TM2GSvgDocumentHandle)aDocumentHandle, aHeight););
    }
    M2G_DO_UNLOCK(aJni)

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
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GDocument__1setViewportWidth(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jint aWidth)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _setViewportWidth - begin");
    TInt err = KM2GNotOk;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
    {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err,    aProxy->SetViewportWidthL(
                 (TM2GSvgDocumentHandle)aDocumentHandle, aWidth););
    }
    M2G_DO_UNLOCK(aJni)

    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GDocument ) _setViewportWidth - end");
}

M2G_NS_END
