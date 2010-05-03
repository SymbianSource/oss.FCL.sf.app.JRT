/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Creates CAMMSPriorityControl and adds it to the player.
*
*/


#include <jutils.h>
#include <jdebug.h>

#include "com_nokia_amms_AMMSPlugin.h"
#include <cmmaeventsource.h>
#include <cmmaplayer.h>
#include "cammsprioritycontrol.h"
#include "cammsaudiooutputcontrol.h"

/**
 * Creates CAMMSPriorityControl and adds it to the player.
 * Returns the created control through the given parameter.
 */
static void CreatePriorityControlL(CMMAPlayer* aPlayer,
                                   CAMMSPriorityControl** aControl)
{
    DEBUG("CreatePriorityControlL +");

    CAMMSPriorityControl* control = CAMMSPriorityControl::NewLC(aPlayer);

    aPlayer->AddControlL(control);

    CleanupStack::Pop(control);

    *aControl = control;

    DEBUG("CreatePriorityControlL -");
}

/**
* Creates CAMMSAudioOutputControl and adds it to the player.
* Returns the created control through the given parameter.
*/
static void CreateAudioOutputControlL(CMMAPlayer* aPlayer,
                                      CAMMSAudioOutputControl** aControl)
{
    DEBUG("CreatePriorityControlL +");

    CAMMSAudioOutputControl* control = CAMMSAudioOutputControl::NewLC(aPlayer);

    aPlayer->AddControlL(control);

    CleanupStack::Pop(control);

    *aControl = control;

    DEBUG("CAMMSAudioOutputControl -");
}
/*
 * Class:     com_nokia_amms_AMMSPlugin
 * Method:    _createNativePriorityControl
 *
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_AMMSPlugin__1createNativePriorityControl(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aPlayer)
{
    DEBUG("AMMS: Java_com_nokia_amms_AMMSPlugin__1createNativePriorityControl + ");

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CMMAPlayer* player = JavaUnhand< CMMAPlayer >(aPlayer);


    // Create control and add it to the player.
    CAMMSPriorityControl* control = NULL;

    TInt handle =
        eventSource->ExecuteTrap(&CreatePriorityControlL, player, &control);

    __ASSERT_DEBUG(control, User::Invariant());

    if (handle == KErrNone)
    {
        handle = JavaMakeHandle(control);
    }

    DEBUG("AMMS: Java_com_nokia_amms_AMMSPlugin__1createNativePriorityControl - ");

    return handle;
}

/*
* Class:     com_nokia_amms_AMMSPlugin
* Method:    _createNativeAudioOutputControl
*
*/

JNIEXPORT jint JNICALL Java_com_nokia_amms_AMMSPlugin__1createNativeAudioOutputControl(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aPlayer)
{
    DEBUG("AMMS: Java_com_nokia_amms_AMMSPlugin__1createNativeAudioOutputControl + ");

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CMMAPlayer* player = JavaUnhand< CMMAPlayer >(aPlayer);


    // Create control and add it to the player.
    CAMMSAudioOutputControl* control = NULL;

    TInt handle =
        eventSource->ExecuteTrap(&CreateAudioOutputControlL, player, &control);

    __ASSERT_DEBUG(control, User::Invariant());

    if (handle == KErrNone)
    {
        handle = JavaMakeHandle(control);
    }

    DEBUG("AMMS: Java_com_nokia_amms_AMMSPlugin__1createNativeAudioOutputControl - ");

    return handle;
}
//  End of File
