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
#include "com_nokia_microedition_m2g_M2GManager.h"
#include "CM2GSVGProxy.h"

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
// Java_com_nokia_microedition_m2g_M2GManager::_createSvgEngine
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::CreateSvgEngineL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 * @param aHandle Svg engine handle
*/
/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GManager__1createSvgEngine(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GManager ) _createSvgEngine - begin");

    TInt handle = M2G_INVALID_HANDLE;
    TInt err = KM2GNotOk;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
    {
        TRAP(err,JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle)->CreateSvgEngineL(handle););
    }
    M2G_DO_UNLOCK(aJni)

    handle = M2GGeneral::CheckErrorCodeAndHandle(
                 aJni, err, handle, M2G_INVALID_HANDLE);

    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GManager ) _createSvgEngine: %d - end", handle);
    return handle;
}
// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GManager::_createSvgProxy
// -----------------------------------------------------------------------------
/**
 * Creates an svg proxy.
 * @since Series S60 3.0
 * @param Svg proxy handle
 *
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GManager__1createSvgProxy(
    JNIEnv* aJni,jclass)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GManager ) _createSvgProxy - begin");
    TInt handle = M2G_INVALID_HANDLE;
    TInt    err = KM2GNotOk;

    M2G_DO_LOCK
    TRAP(err, MM2GSVGProxy* proxy = CM2GSVGProxy::NewL(); handle = JavaMakeHandle(proxy););
    M2G_DO_UNLOCK(aJni)

    handle = M2GGeneral::CheckErrorCodeAndHandle(aJni, err, handle, err);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GManager ) _createSvgProxy: %d - end", handle);
    return handle;
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GManager::_deleteSvgEngine
// -----------------------------------------------------------------------------
/**
 * Calls MM2GSVGProxy::DeleteSvgEnginedL method.
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 *
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GManager__1deleteSvgEngine(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle,
    jint aSvgEngineHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GManager ) _deleteSvgEngine - begin");

    TInt err = KM2GNotOk;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
    {
        TRAP(err,JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle)->DeleteSvgEngineL(
                 STATIC_CAST(TM2GSvgEngineHandle, aSvgEngineHandle)););
    }
    M2G_DO_UNLOCK(aJni)

    M2GGeneral::CheckErrorCode(aJni, err);

    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GManager ) _deleteSvgEngine: - end");
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GManager::_deleteSvgProxy
// -----------------------------------------------------------------------------
/**
 * Deletes a proxy object. NOTE: The proxy object has to be deleted inside same
 * thread as it was created, otherwice there will be problem to delete a CFbsBitmap
 * server object inside the SVGTopt componenet see KATA-6KYA9T (SVGEngineJI: SVGTopt
 * crashes when deleting svg document object that contains external resource)
 * @since Series S60 3.0
 * @param aProxy Proxy instance.
 *
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GManager__1deleteSvgProxy(
    JNIEnv* aJni,
    jclass,
    jint aSvgProxyHandle)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GManager ) _deleteSvgProxy - begin");
    TInt            err = KM2GNotOk;

    M2G_DO_LOCK
    if (aSvgProxyHandle)
    {
        MM2GSVGProxy* proxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        TRAP(err,delete proxy;);
    }
    M2G_DO_UNLOCK(aJni)

    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GManager ) _deleteSvgProxy: - end");
}

M2G_NS_END
