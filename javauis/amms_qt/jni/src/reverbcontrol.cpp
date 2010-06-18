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


//#include <jutils.h>

#include "com_nokia_amms_control_audioeffect_ReverbControl.h"
#include <mmafunctionserver.h>
#include "cammsreverbcontrolgroup.h"

/**
 * wrapper for CAMMSReverbControlGroup::SetReverbLevelL()
 */
static void SetReverbLevelL(
    CAMMSReverbControlGroup* aControl,
    TInt *aLevel)
{
    aControl->SetReverbLevelL(*aLevel);
    *aLevel = aControl->ReverbLevel();
}

/**
 * wrapper for CAMMSReverbControlGroup::SetReverbLevelL()
 */
static void SetReverbTimeL(
    CAMMSReverbControlGroup* aControl,
    TInt aTime)
{
    aControl->SetReverbTimeL(aTime);
}

/*
 * Class:     com_nokia_amms_control_audioeffect_ReverbControl
 * Method:    _getReverbLevel
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audioeffect_ReverbControl__1getReverbLevel(
    JNIEnv*,
    jclass,
    jint,
    jint aControl)
{
    CAMMSReverbControlGroup* control = static_cast< CAMMSReverbControlGroup* >(
                                           reinterpret_cast< CAMMSControlGroup* >(aControl));

    return control->ReverbLevel();
}

/*
 * Class:     com_nokia_amms_control_audioeffect_ReverbControl
 * Method:    _getReverbTime
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audioeffect_ReverbControl__1getReverbTime(
    JNIEnv*, jclass, jint, jint aControl)
{
    CAMMSReverbControlGroup* control = static_cast< CAMMSReverbControlGroup* >(
                                           reinterpret_cast< CAMMSControlGroup *>(aControl));

    return control->ReverbTime();
}

/*
 * Class:     com_nokia_amms_control_audioeffect_ReverbControl
 * Method:    _setReverbTime
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audioeffect_ReverbControl__1setReverbTime(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aControl,
    jint aTime)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSource);

    CAMMSReverbControlGroup* control = static_cast< CAMMSReverbControlGroup* >(
                                           reinterpret_cast< CAMMSControlGroup* >(aControl));

    TInt error;
    error = eventSource->ExecuteTrap(SetReverbTimeL, control, (TInt) aTime);
    return error;
}

/*
 * Class:     com_nokia_amms_control_audioeffect_ReverbControl
 * Method:    _setReverbLevel
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audioeffect_ReverbControl__1setReverbLevel(
    JNIEnv* aJniEnv,
    jclass,
    jint aEventSource,
    jint aControl,
    jint aLevel,
    jintArray aError)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSReverbControlGroup* control = static_cast< CAMMSReverbControlGroup* >(
                                           reinterpret_cast< CAMMSControlGroup* >(aControl));

    TInt error;
    TInt level = aLevel;

    error = eventSource->ExecuteTrap(
                SetReverbLevelL,
                control,
                (TInt *) &level);

    jint javaError[ 1 ] = { error };
    aJniEnv->SetIntArrayRegion(aError, 0, 1, javaError);

    return level;
}


//  End of File
