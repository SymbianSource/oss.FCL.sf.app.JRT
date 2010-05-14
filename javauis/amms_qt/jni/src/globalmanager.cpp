/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


//#include <jutils.h>
#include <logger.h>

#include <mmafunctionserver.h>

#include "com_nokia_amms_GlobalManagerImpl.h"
#include "cammsglobalmanager.h"

/**
 * Creates new CAMMSGlobalManager instance which is added to
 * MMAFunctionServer for cleanup.
 * @param aHandle will contain handle to created object.
 * @param aEventSource MMA event source instance.
 */
LOCAL_C void CreateManagerL(TInt* aHandle,
                            MMAFunctionServer* aEventSource)
{
    CAMMSGlobalManager* manager = CAMMSGlobalManager::NewLC();

    // Make Java handle
    *aHandle = reinterpret_cast<TInt>(manager);

    MMAFunctionServer::StaticAddObjectFromHandleL(aEventSource,
            *aHandle);

    CleanupStack::Pop(manager);
}

/**
 * JNI function.
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_GlobalManagerImpl__1createGlobalManager
(JNIEnv*,
 jclass,
 jint aEventSourceHandle)
{
    LOG( EJavaAMMS, EInfo, "AMMS::GlobalManager.cpp::createGlobalManager +");

    // Development time check.
    __ASSERT_DEBUG(aEventSourceHandle > 0, User::Invariant());

    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSourceHandle);

    // handle will be set in CreateManagerL function.
    TInt handle = KErrNotReady;

    // Call CreateManagerL. If it leaves handle will not be set
    TInt error = eventSource->ExecuteTrap(&CreateManagerL,
                                          &handle,
                                          eventSource);

    if (error != KErrNone)
    {
        // executed function leaved. Set error to be returned to java.
        handle = error;
    }

    LOG1( EJavaAMMS, EInfo, "AMMS::GlobalManager.cpp::createGlobalManager return %d",
              handle);

    return handle;
}

/**
 * Calls CAMMSGlobalManager::InitL method.
 *
 * @param aManager CAMMSGlobalManager instance.
 * @param aEventSource MMAFunctionServer instance.
 */
LOCAL_C void InitManagerL(CAMMSGlobalManager* aManager,
                          MMAFunctionServer* aEventSource)
{
    aManager->InitL(aEventSource->Players());

    // Receive notifications when new players are created or deleted.
    aEventSource->SetPlayerInstanceObserver(aManager);
}

/**
 * JNI function.
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_GlobalManagerImpl__1init
(JNIEnv*,
 jclass,
 jint aEventSourceHandle,
 jint aGlobalManagerHandle)
{
    LOG( EJavaAMMS, EInfo, "AMMS::GlobalManager.cpp::init +");

    // Development time check.
    __ASSERT_DEBUG(aEventSourceHandle > 0 &&
                   aGlobalManagerHandle > 0, User::Invariant());

    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSourceHandle);

    CAMMSGlobalManager* manager =
        reinterpret_cast< CAMMSGlobalManager* >(aGlobalManagerHandle);

    // Call InitManagerL.
    TInt error = eventSource->ExecuteTrap(&InitManagerL,
                                          manager,
                                          eventSource);

    ELOG1( EJavaAMMS, "AMMS::GlobalManager.cpp::init error code %d", error);
    return error;
}

/**
 * Calls CAMMSGlobalManager::CreateSoundSource3DL method.
 * @param aManager CAMMSGlobalManager instance.
 * @param aHandle will contain handle to created object.
 */
LOCAL_C void CreateSoundSource3DL(CAMMSGlobalManager* aManager, TInt* aHandle)
{
    CAMMSModule* module = aManager->CreateSoundSource3DL();

    // Make Java handle
    *aHandle = reinterpret_cast<TInt>(module);
}

/**
 * JNI function.
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_GlobalManagerImpl__1createSoundSource3D
(JNIEnv*,
 jclass,
 jint aEventSourceHandle,
 jint aGlobalManagerHandle)
{
    MMAFunctionServer* eventSource =
		reinterpret_cast< MMAFunctionServer* >(aEventSourceHandle);

    CAMMSGlobalManager* manager =
        reinterpret_cast< CAMMSGlobalManager* >(aGlobalManagerHandle);

    // handle will be set in CreateSoundSource3DL function.
    TInt handle = KErrNotReady;

    // Call CreateSoundSource3DL. If it leaves handle will not be set
    TInt error = eventSource->ExecuteTrap(&CreateSoundSource3DL,
                                          manager,
                                          &handle);

    if (error != KErrNone)
    {
        // executed function leaved. Set error to be returned to java.
        handle = error;
    }
    ELOG1( EJavaAMMS, "AMMS::GlobalManager.cpp::createSoundSource3D return value %d",
              error);
    return handle;
}

/**
 * Calls CAMMSGlobalManager::CreateEffectModuleL method.
 * @param aManager CAMMSGlobalManager instance.
 * @param aHandle will contain handle to created object.
 */
LOCAL_C void CreateEffectModuleL(CAMMSGlobalManager* aManager, TInt* aHandle)
{
    CAMMSModule* module = aManager->CreateEffectModuleL();

    // Make Java handle
    *aHandle = reinterpret_cast<TInt>(module);
}

/**
 * JNI function.
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_GlobalManagerImpl__1createEffectModule
(JNIEnv*,
 jclass,
 jint aEventSourceHandle,
 jint aGlobalManagerHandle)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSourceHandle);

    CAMMSGlobalManager* manager =
        reinterpret_cast< CAMMSGlobalManager* >(aGlobalManagerHandle);

    // handle will be set in CreateEffectModuleL function.
    TInt handle = KErrNotReady;

    // Call CreateEffectModuleL. If it leaves handle will not be set
    TInt error = eventSource->ExecuteTrap(&CreateEffectModuleL,
                                          manager,
                                          &handle);

    if (error != KErrNone)
    {
        // executed function leaved. Set error to be returned to java.
        handle = error;
    }
    ELOG1( EJavaAMMS, "AMMS::GlobalManager.cpp::createEffectModule return value %d",
              error);
    return handle;
}

/**
 * Calls CAMMSGlobalManager::Spectator method.
 * @param aManager CAMMSGlobalManager instance.
 * @param aHandle will contain handle to created object.
 */
LOCAL_C void CreateSpectatorL(CAMMSGlobalManager* aManager, TInt* aHandle)
{
    CAMMSModule* spectator = aManager->Spectator();

    // Make Java handle
    *aHandle = reinterpret_cast<TInt>(spectator);
}

/**
 * JNI function.
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_GlobalManagerImpl__1createSpectator
(JNIEnv*,
 jclass,
 jint aEventSourceHandle,
 jint aGlobalManagerHandle)
{
    LOG( EJavaAMMS, EInfo, "AMMS::GlobalManager.cpp::createSpectator +");
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSourceHandle);

    CAMMSGlobalManager* manager =
        reinterpret_cast< CAMMSGlobalManager* >(aGlobalManagerHandle);

    // handle will be set in CreateSpectatorL function.
    TInt handle = KErrNotReady;

    // Call CreateSpectatorL. If it leaves handle will not be set
    TInt error = eventSource->ExecuteTrap(&CreateSpectatorL,
                                          manager,
                                          &handle);

    if (error != KErrNone)
    {
        // executed function leaved. Set error to be returned to java.
        handle = error;
    }
    LOG1( EJavaAMMS, EInfo, "AMMS::GlobalManager.cpp::createSpectator error code %d", handle);
    return handle;
}

//  End of File
