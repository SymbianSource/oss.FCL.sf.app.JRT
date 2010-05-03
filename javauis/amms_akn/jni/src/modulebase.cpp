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

#include <jutils.h>
#include <jdebug.h>

#include <cmmaeventsource.h>
#include <cmmaplayer.h>

// Generated JNI header.
#include "com_nokia_amms_ModuleBase.h"

#include "cammsmodule.h"
#include "cammsglobalmanager.h"

/**
 * Calls CAMMSModule::AddPlayerL method.
 * @param aModule Module instance.
 * @param aPlayer Player to add.
 */
LOCAL_C void AddPlayerL(CAMMSModule* aModule, CMMAPlayer* aPlayer)
{
    aModule->AddPlayerL(aPlayer);
}

/**
 * JNI function.
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_ModuleBase__1addPlayer
(JNIEnv* /*aJniEnv*/,
 jclass,
 jint aEventSourceHandle,
 jint aModuleHandle,
 jint aPlayerHandle)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CAMMSModule* module =
        JavaUnhand< CAMMSModule >(aModuleHandle);

    CMMAPlayer* player =
        JavaUnhand< CMMAPlayer >(aPlayerHandle);

    // call CAMMSModule::AddPlayerL through local AddPlayerL function.
    return eventSource->ExecuteTrap(&AddPlayerL,
                                    module,
                                    player);
}

/**
 * Calls CAMMSModule::RemovePlayerL method.
 * @param aModule Module instance.
 * @param aPlayer Player to remove.
 * @return KErrNotFound if player does not exist in the module.
 */
LOCAL_C TInt RemovePlayer(CAMMSModule* aModule, CMMAPlayer* aPlayer)
{
    return aModule->RemovePlayer(aPlayer);
}

/**
 * JNI function.
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_ModuleBase__1removePlayer
(JNIEnv* /*aJniEnv*/,
 jclass,
 jint aEventSourceHandle,
 jint aModuleHandle,
 jint aPlayerHandle)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CAMMSModule* module =
        JavaUnhand< CAMMSModule >(aModuleHandle);

    CMMAPlayer* player =
        JavaUnhand< CMMAPlayer >(aPlayerHandle);

    // call CAMMSModule::RemovePlayerL through local RemovePlayerL function.
    return eventSource->Execute(&RemovePlayer,
                                module,
                                player);
}

/**
 * JNI function.
 */
JNIEXPORT void JNICALL Java_com_nokia_amms_ModuleBase__1dispose
(JNIEnv*,
 jclass,
 jint aManagerHandle,
 jint aModuleHandle)
{
    DEBUG_INT("AMMS::ModuleBase.cpp::finalize %d", aModuleHandle);

    // Module to dispose
    CAMMSModule* module =
        JavaUnhand< CAMMSModule >(aModuleHandle);

    CAMMSGlobalManager* manager =
        JavaUnhand< CAMMSGlobalManager >(aManagerHandle);

    manager->DisposeModule(module);
}
//  End of File



