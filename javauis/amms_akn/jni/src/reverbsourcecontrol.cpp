/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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

#include "com_nokia_amms_control_audioeffect_ReverbSourceControl.h"
#include <cmmaeventsource.h>
#include "cammsreverbsourcecontrolgroup.h"


/**
 * wrapper for CAMMSReverbSourceControlGroup::SetRoomLevelL()
 */
static void SetRoomLevelL(
    CAMMSReverbSourceControlGroup* aControl,
    TInt aLevel)
{
    aControl->SetRoomLevelL(aLevel);
}

/*
 * Class:     com_nokia_amms_control_audioeffect_ReverbSourceControl
 * Method:    _getRoomLevel
 * Signature:
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audioeffect_ReverbSourceControl__1getRoomLevel(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aControl)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CAMMSReverbSourceControlGroup* control =
        static_cast< CAMMSReverbSourceControlGroup* >(
            JavaUnhand< CAMMSControlGroup >(aControl));

    return control->RoomLevel();
}

/*
 * Class:     com_nokia_amms_control_audioeffect_ReverbSourceControl
 * Method:    _setRoomLevel
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audioeffect_ReverbSourceControl__1setRoomLevel(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aControl,
    jint aLevel)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CAMMSReverbSourceControlGroup* control =
        static_cast< CAMMSReverbSourceControlGroup* >(
            JavaUnhand< CAMMSControlGroup >(aControl));

    TInt error;
    error = eventSource->ExecuteTrap(SetRoomLevelL, control, (TInt) aLevel);
    return error;
}

//  End of File
