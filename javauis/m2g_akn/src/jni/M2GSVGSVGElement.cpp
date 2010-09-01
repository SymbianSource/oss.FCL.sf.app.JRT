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
#include "com_nokia_microedition_m2g_M2GSVGSVGElement.h"
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
// Java_com_nokia_microedition_m2g_M2GSVGImage::_getMediaTime
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetMediaTimeL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document handle
 * @param aSeconds Media time to be returned.
 */
LOCAL_C void DoGetMediaTimeL(
    MM2GSVGProxy* aProxy,
    TInt aDocumentHandle,
    TReal32* aSeconds)
{
    aProxy->GetMediaTimeL(
        STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle),
        *aSeconds);
}

/**
 * JNI method
 */
JNIEXPORT jfloat JNICALL
Java_com_nokia_microedition_m2g_M2GSVGSVGElement__1getMediaTime(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jboolean aUiToolKit)
{
    M2G_DEBUG_0("JNI ( M2GSVGSVGElement ) _getMediaTime - begin");
    TReal32 seconds = 0;
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pGetMediaTimeL)(const TM2GSvgDocumentHandle&, TReal32&);
            pGetMediaTimeL GetMediaTimeL = &MM2GSVGProxy::GetMediaTimeL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgDocumentHandle lDocumentHandle = STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle);
            TMethodWrapper2<MM2GSVGProxy, const TM2GSvgDocumentHandle&, TReal32& > GetMediaTime(
                *aProxy,
                GetMediaTimeL,
                lDocumentHandle,
                seconds);
            err = client->Execute(GetMediaTime);
            M2GGeneral::CheckErrorCode(aJni, err);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoGetMediaTimeL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aDocumentHandle,
                      &seconds);
            M2GGeneral::CheckErrorCode(aJni, err);
        }
    }
    M2G_DEBUG_1("JNI ( M2GSVGSVGElement ) _getMediaTime: %f - end", seconds);
    return STATIC_CAST(jfloat, seconds);
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GSVGImage::_setMediaTime
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::SetMediaTimeL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document handle
 * @param aSeconds Media time
 */
LOCAL_C void DoSetMediaTimeL(
    MM2GSVGProxy* aProxy,
    TInt aDocumentHandle,
    TReal32* aSeconds)
{
    aProxy->SetMediaTimeL(
        STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle),
        *aSeconds);
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGSVGElement__1setMediaTime(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jfloat aSeconds,
    jboolean aUiToolKit
)
{
    M2G_DEBUG_0("JNI ( M2GSVGSVGElement ) _setMediaTime - begin");
    TInt err = KM2GNotOk;
    if (aUiToolKit)  // If eSWT is the current toolkit
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && aSvgProxyHandle)
        {
            typedef void (MM2GSVGProxy::*pSetMediaTimeL)(const TM2GSvgDocumentHandle&,const  TReal32&);
            pSetMediaTimeL SetMediaTimeL = &MM2GSVGProxy::SetMediaTimeL;
            MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
            TM2GSvgDocumentHandle lDocumentHandle = STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle);
            TReal32* lseconds = REINTERPRET_CAST(TReal32*, &aSeconds);
            TMethodWrapper2<MM2GSVGProxy, const TM2GSvgDocumentHandle&,const TReal32& > SetMediaTime(
                *aProxy,
                SetMediaTimeL,
                lDocumentHandle,
                *lseconds);
            err = client->Execute(SetMediaTime);
            M2GGeneral::CheckErrorCode(aJni, err);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && aSvgProxyHandle)
        {
            err = eventSource->ExecuteTrap(
                      &DoSetMediaTimeL,
                      JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                      aDocumentHandle,
                      REINTERPRET_CAST(TReal32*, &aSeconds));
            M2GGeneral::CheckErrorCode(aJni, err);
        }
    }
    M2G_DEBUG_1("JNI ( M2GSVGSVGElement ) _setMediaTime: %f - end", aSeconds);
}
