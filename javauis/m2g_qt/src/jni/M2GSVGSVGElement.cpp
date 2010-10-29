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
#include "com_nokia_microedition_m2g_M2GSVGSVGElement.h"
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
// Java_com_nokia_microedition_m2g_M2GSVGImage::_getMediaTime
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::GetMediaTimeL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aDocumentHandle Document handle
 * @param aSeconds Media time to be returned.
 */
JNIEXPORT jfloat JNICALL
Java_com_nokia_microedition_m2g_M2GSVGSVGElement__1getMediaTime(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aDocumentHandle)
{
    M2G_DEBUG_0("JNI ( M2GSVGSVGElement ) _getMediaTime - begin");
    TReal32 seconds = 0;
    TInt err = KM2GNotOk;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
    {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err, aProxy->GetMediaTimeL(STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle),seconds););
    }
    M2G_DO_UNLOCK(aJni)

    M2GGeneral::CheckErrorCode(aJni, err);

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
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GSVGSVGElement__1setMediaTime(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aDocumentHandle,
    jfloat aSeconds
)
{
    M2G_DEBUG_0("JNI ( M2GSVGSVGElement ) _setMediaTime - begin");
    TInt err = KM2GNotOk;

    TReal32* lseconds = REINTERPRET_CAST(TReal32*, &aSeconds);

    M2G_DO_LOCK
    if (aSvgProxyHandle)
    {
        MM2GSVGProxy* aProxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err, aProxy->SetMediaTimeL(
                 STATIC_CAST(TM2GSvgDocumentHandle, aDocumentHandle),
                 *lseconds););
    }
    M2G_DO_UNLOCK(aJni)

    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_1("JNI ( M2GSVGSVGElement ) _setMediaTime: %f - end", aSeconds);
}
