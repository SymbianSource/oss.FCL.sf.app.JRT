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


//#include <jutils.h>
#include <logger.h>

#include "com_nokia_amms_AMMSPlugin.h"
#include <mmafunctionserver.h>
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
    LOG( EJavaAMMS, EInfo, "CreatePriorityControlL +");

    CAMMSPriorityControl* control = CAMMSPriorityControl::NewLC(aPlayer);

    aPlayer->AddControlL(control);

    CleanupStack::Pop(control);

    *aControl = control;

    LOG( EJavaAMMS, EInfo, "CreatePriorityControlL -");
}

/**
* Creates CAMMSAudioOutputControl and adds it to the player.
* Returns the created control through the given parameter.
*/
static void CreateAudioOutputControlL(CMMAPlayer* aPlayer,
                                      CAMMSAudioOutputControl** aControl)
{
    LOG( EJavaAMMS, EInfo, "CreatePriorityControlL +");

    CAMMSAudioOutputControl* control = CAMMSAudioOutputControl::NewLC(aPlayer);

    aPlayer->AddControlL(control);

    CleanupStack::Pop(control);

    *aControl = control;

    LOG( EJavaAMMS, EInfo, "CAMMSAudioOutputControl -");
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
    LOG( EJavaAMMS, EInfo, "AMMS: Java_com_nokia_amms_AMMSPlugin__1createNativePriorityControl + ");

    MMAFunctionServer* eventSource =
    	reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CMMAPlayer* player = reinterpret_cast< CMMAPlayer* >(aPlayer);


    // Create control and add it to the player.
    CAMMSPriorityControl* control = NULL;

    TInt handle =
        eventSource->ExecuteTrap(&CreatePriorityControlL, player, &control);

    __ASSERT_DEBUG(control, User::Invariant());

    if (handle == KErrNone)
    {
        handle = reinterpret_cast<TInt>(control);
    }

    LOG( EJavaAMMS, EInfo, "AMMS: Java_com_nokia_amms_AMMSPlugin__1createNativePriorityControl - ");

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
    LOG( EJavaAMMS, EInfo, "AMMS: Java_com_nokia_amms_AMMSPlugin__1createNativeAudioOutputControl + ");

    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CMMAPlayer* player = reinterpret_cast< CMMAPlayer* >(aPlayer);


    // Create control and add it to the player.
    CAMMSAudioOutputControl* control = NULL;

    TInt handle =
        eventSource->ExecuteTrap(&CreateAudioOutputControlL, player, &control);

    __ASSERT_DEBUG(control, User::Invariant());

    if (handle == KErrNone)
    {
       handle = reinterpret_cast<TInt>(control);
    }

    LOG( EJavaAMMS, EInfo, "AMMS: Java_com_nokia_amms_AMMSPlugin__1createNativeAudioOutputControl - ");

    return handle;
}
//  End of File
