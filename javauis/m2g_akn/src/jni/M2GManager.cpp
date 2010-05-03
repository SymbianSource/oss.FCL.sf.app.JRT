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
#include "com_nokia_microedition_m2g_M2GManager.h"
#include "CM2GSVGProxy.h"
#include "CM2GEventSource.h"
#include <swtcliententry.h>

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
LOCAL_C void DoCreateSvgEngineL(MM2GSVGProxy* aProxy, TInt* aHandle)
{
    aProxy->CreateSvgEngineL(*aHandle);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GManager__1createSvgEngine(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jboolean /*aUiToolkit*/)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GManager ) _createSvgEngine - begin");
    CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
    // Execute native engine method
    TInt handle = M2G_INVALID_HANDLE;
    if (eventSource && aSvgProxyHandle)
    {
        TInt err = eventSource->ExecuteTrap(
                       &DoCreateSvgEngineL,
                       JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                       &handle);
        handle = M2GGeneral::CheckErrorCodeAndHandle(
                     aJni, err, handle, M2G_INVALID_HANDLE);
    }
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
 */
LOCAL_C void DoCreateSvgProxyL(TInt* aHandle)
{
    MM2GSVGProxy* proxy = CM2GSVGProxy::NewL();
    *aHandle = JavaMakeHandle(proxy);
}

/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GManager__1createSvgProxy(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GManager ) _createSvgProxy - begin");
    TInt handle = M2G_INVALID_HANDLE;
    TInt    err = KM2GNotOk;
    if (aUiToolkit)
    {
        MSwtClient* client = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client)
        {
            typedef void (*pDoCreateSvgProxyL)(TInt* aHandle);
            pDoCreateSvgProxyL lDoCreateSvgProxyL = DoCreateSvgProxyL;
            TSwtM2gMethodWrapper1<TInt> DoCreateSvgProxy(
                lDoCreateSvgProxyL,
                &handle
            );
            err = client->Execute(DoCreateSvgProxy);
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);

        if (eventSource)
        {
            err = eventSource->ExecuteTrap(&DoCreateSvgProxyL, &handle);

        }
    }
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
 */
LOCAL_C void DoDeleteSvgEngineL(MM2GSVGProxy* aProxy, TInt aSvgEngineHandle)
{
    aProxy->DeleteSvgEngineL(
        STATIC_CAST(TM2GSvgEngineHandle, aSvgEngineHandle));
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GManager__1deleteSvgEngine(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jint aSvgEngineHandle,
    jboolean /*aUiToolkit*/)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GManager ) _deleteSvgEngine - begin");
    CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
    if (eventSource && aSvgProxyHandle && aSvgEngineHandle)
    {
        TInt err = eventSource->ExecuteTrap(
                       &DoDeleteSvgEngineL,
                       JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle),
                       aSvgEngineHandle);
        M2GGeneral::CheckErrorCode(aJni, err);
    }
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
 */
LOCAL_C void DoDeleteSvgProxyL(MM2GSVGProxy* aProxy)
{
    delete aProxy;
}

/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GManager__1deleteSvgProxy(
    JNIEnv* aJni,
    jclass,
    jint aEventSourceHandle,
    jint aSvgProxyHandle,
    jboolean aUiToolkit)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GManager ) _deleteSvgProxy - begin");
    TInt            err = KM2GNotOk;
    if (aUiToolkit)
    {
        MM2GSVGProxy* proxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        MSwtClient* client  = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client && proxy)
        {
            typedef void (*pDoDeleteSvgProxyL)(MM2GSVGProxy* aProxy);
            pDoDeleteSvgProxyL lDoDeleteSvgProxyL = DoDeleteSvgProxyL;
            TSwtM2gMethodWrapper1<MM2GSVGProxy> DoDeleteSvgProxy(
                lDoDeleteSvgProxyL,
                proxy
            );
            err = client->Execute(DoDeleteSvgProxy);
        }
    }
    else
    {
        MM2GSVGProxy* proxy = JavaUnhand<MM2GSVGProxy>(aSvgProxyHandle);
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource && proxy)
        {
            err = eventSource->ExecuteTrap(&DoDeleteSvgProxyL, proxy);
        }
    }
    M2GGeneral::CheckErrorCode(aJni, err);
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GManager ) _deleteSvgProxy: - end");
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GManager::_finalizeEventSource
// -----------------------------------------------------------------------------
/**
 * JNI method
 */
JNIEXPORT void JNICALL
Java_com_nokia_microedition_m2g_M2GManager__1finalizeEventSource(
    JNIEnv *aJni, jclass, jint aEventSourceHandle, jboolean aUiToolkit)
{
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GManager ) _finalizeEventSource: %d", aEventSourceHandle);
    if (aUiToolkit)
    {
        MSwtClient* client  = reinterpret_cast< MSwtClient* >(aEventSourceHandle);
        if (client)
        {
            delete client;
            client = NULL;
        }
    }
    else
    {
        CM2GEventSource* eventSource = JavaUnhand< CM2GEventSource >(aEventSourceHandle);
        if (eventSource)
        {
            eventSource->Dispose(*aJni);
        }
        eventSource = NULL;
    }
}

// -----------------------------------------------------------------------------
// Java_com_nokia_microedition_m2g_M2GManager::_initEventSource
// -----------------------------------------------------------------------------
/**
 * JNI method
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GManager__1initEventSource(
    JNIEnv *aJni,
    jclass aPeer,
    jint aServer)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GManager ) _initEventSource - begin");
    TRAPD(eventSourceHandle, eventSourceHandle = CM2GEventSource::NewL(
                *aJni, aPeer, aServer));
    M2GGeneral::CheckHandle(aJni, eventSourceHandle);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GManager ) _initEventSource - end: %d", eventSourceHandle);
    return eventSourceHandle;
}

JNIEXPORT jint JNICALL
Java_com_nokia_microedition_m2g_M2GManager__1initSwtEventSource(
    JNIEnv *aJni,
    jclass /*aPeer*/)
{
    M2G_DEBUG_0("M2G_DEBUG: JNI ( M2GManager ) _initEventSource - begin");

    MSwtClient* client = NULL;
    TRAP_IGNORE(client = SWT::CreateClientL());
    if (!client)
    {
        return KErrNotFound;
    }
    M2GGeneral::CheckHandle(aJni, (int)client);
    M2G_DEBUG_1("M2G_DEBUG: JNI ( M2GManager ) _initEventSource - end: %d", eventSourceHandle);
    return (int)client;
}
M2G_NS_END
