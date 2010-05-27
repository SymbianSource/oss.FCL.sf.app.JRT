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

#include "com_nokia_amms_control_audioeffect_EqualizerControl.h"
#include <mmafunctionserver.h>
#include "cammsequalizercontrolgroup.h"

/**
 * wrapper for CAMMSEqualizerControlGroup::GetBandLevelL()
 */
static void GetBandLevelL(
    CAMMSEqualizerControlGroup *aControl,
    TInt aBand,
    TInt *aLevel)
{
    aControl->GetBandLevelL(aBand, *aLevel);
}

/**
 * wrapper for CAMMSEqualizerControlGroup::CenterFreqL()
 */
static void GetCenterFreqL(
    CAMMSEqualizerControlGroup *aControl,
    TInt aBand,
    TInt *aFrequency)
{
    aControl->GetCenterFreqL(aBand, *aFrequency);
}

/**
 * wrapper for CAMMSEqualizerControlGroup::SetBandLevelL()
 */
static void SetBandLevelL(
    CAMMSEqualizerControlGroup *aControl,
    TInt aLevel,
    TInt aBand)
{
    aControl->SetBandLevelL(aLevel, aBand);
}

/**
 * wrapper for CAMMSEqualizerControlGroup::SetBassL()
 */
static void SetBassL(CAMMSEqualizerControlGroup *aControl, TInt *aLevel, TInt *aSetLevel)
{
    aControl->SetBassL(*aLevel, *aSetLevel);
}

/**
 * wrapper for CAMMSEqualizerControlGroup::SetTrebleL()
 */
static void SetTrebleL(CAMMSEqualizerControlGroup *aControl, TInt *aLevel, TInt *aSetLevel)
{
    aControl->SetTrebleL(*aLevel, *aSetLevel);
}

/*
 * Class:     com_nokia_amms_control_audioeffect_EqualizerControl
 * Method:    _getBand
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audioeffect_EqualizerControl__1getBand(
    JNIEnv*, jclass, jint, jint aControl, jint aFrequency)
{
    CAMMSEqualizerControlGroup* control =
        static_cast<CAMMSEqualizerControlGroup*>(
            reinterpret_cast< CAMMSControlGroup* >(aControl));

    return control->Band(aFrequency);
}

/*
 * Class:     com_nokia_amms_control_audioeffect_EqualizerControl
 * Method:    _getTreble
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audioeffect_EqualizerControl__1getTreble(
    JNIEnv*, jclass, jint, jint aControl)
{
    CAMMSEqualizerControlGroup* control =
        static_cast<CAMMSEqualizerControlGroup*>(
            reinterpret_cast< CAMMSControlGroup *>(aControl));

    return control->Treble();
}

/*
 * Class:     com_nokia_amms_control_audioeffect_EqualizerControl
 * Method:    _setBass
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audioeffect_EqualizerControl__1setBass(
    JNIEnv*, jclass, jint aEventSource, jint aControl, jint aLevel)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSource);

    CAMMSEqualizerControlGroup* control =
        static_cast<CAMMSEqualizerControlGroup*>(
            reinterpret_cast< CAMMSControlGroup* >(aControl));

    TInt error;
    TInt level = aLevel;
    TInt setLevel;

    error = eventSource->ExecuteTrap(SetBassL, control, &level, &setLevel);
    return (error < KErrNone) ? error : setLevel;
}

/*
 * Class:     com_nokia_amms_control_audioeffect_EqualizerControl
 * Method:    _getCenterFreq
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audioeffect_EqualizerControl__1getCenterFreq(
    JNIEnv*, jclass, jint aEventSource, jint aControl, jint aBand)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSEqualizerControlGroup* control =
        static_cast<CAMMSEqualizerControlGroup*>(
            reinterpret_cast< CAMMSControlGroup *>(aControl));

    TInt error;
    TInt freq;

    error = eventSource->ExecuteTrap(
                GetCenterFreqL,
                control,
                (TInt) aBand,
                &freq);

    return (error < KErrNone) ? error : freq;
}

/*
 * Class:     com_nokia_amms_control_audioeffect_EqualizerControl
 * Method:    _setBandLevel
 * Signature: (IIII)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audioeffect_EqualizerControl__1setBandLevel(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aControl,
    jint aLevel,
    jint aBand)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSource);

    CAMMSEqualizerControlGroup* control =
        static_cast<CAMMSEqualizerControlGroup*>(
            reinterpret_cast< CAMMSControlGroup* >(aControl));

    TInt error;
    error = eventSource->ExecuteTrap(
                SetBandLevelL,
                control,
                (TInt) aLevel,
                (TInt) aBand);

    return error;
}

/*
 * Class:     com_nokia_amms_control_audioeffect_EqualizerControl
 * Method:    _setTreble
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audioeffect_EqualizerControl__1setTreble(
    JNIEnv*, jclass, jint aEventSource, jint aControl, jint aLevel)
{
    MMAFunctionServer* eventSource =
            reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSEqualizerControlGroup* control =
        static_cast<CAMMSEqualizerControlGroup*>(
            reinterpret_cast< CAMMSControlGroup* >(aControl));

    TInt error;
    TInt level = aLevel;
    TInt setLevel;

    error = eventSource->ExecuteTrap(SetTrebleL, control, &level, &setLevel);
    return (error < KErrNone) ? error : setLevel;
}

/*
 * Class:     com_nokia_amms_control_audioeffect_EqualizerControl
 * Method:    _getNumberOfBands
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audioeffect_EqualizerControl__1getNumberOfBands(
    JNIEnv*,
    jclass,
    jint,
    jint aControl)
{
    CAMMSEqualizerControlGroup* control =
        static_cast<CAMMSEqualizerControlGroup*>(
            reinterpret_cast< CAMMSControlGroup* >(aControl));

    return control->NumberOfBands();
}

/*
 * Class:     com_nokia_amms_control_audioeffect_EqualizerControl
 * Method:    _getMinBandLevel
 * Signature: (IILI)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audioeffect_EqualizerControl__1getMinBandLevel(
    JNIEnv*,
    jclass,
    jint,
    jint aControl)
{
    CAMMSEqualizerControlGroup* control =
        static_cast<CAMMSEqualizerControlGroup*>(
            reinterpret_cast< CAMMSControlGroup* >(aControl));


    return control->MinBandLevel();
}

/*
 * Class:     com_nokia_amms_control_audioeffect_EqualizerControl
 * Method:    _getBass
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audioeffect_EqualizerControl__1getBass(
    JNIEnv*, jclass, jint, jint aControl)
{
    CAMMSEqualizerControlGroup* control =
        static_cast<CAMMSEqualizerControlGroup*>(
            reinterpret_cast< CAMMSControlGroup* >(aControl));

    return control->Bass();
}

/*
 * Class:     com_nokia_amms_control_audioeffect_EqualizerControl
 * Method:    _getMaxBandLevel
 * Signature: (IILI)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audioeffect_EqualizerControl__1getMaxBandLevel(
    JNIEnv*, jclass, jint, jint aControl)
{
    CAMMSEqualizerControlGroup* control =
        static_cast<CAMMSEqualizerControlGroup*>(
            reinterpret_cast< CAMMSControlGroup *>(aControl));

    return control->MaxBandLevel();
}

/*
 * Class:     com_nokia_amms_control_audioeffect_EqualizerControl
 * Method:    _getBandLevel
 * Signature: (IIILI)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audioeffect_EqualizerControl__1getBandLevel(
    JNIEnv* aJni,
    jclass,
    jint aEventSource,
    jint aControl,
    jint aBand,
    jintArray aLevel)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSource);

    CAMMSEqualizerControlGroup* control =
        static_cast<CAMMSEqualizerControlGroup*>(
            reinterpret_cast< CAMMSControlGroup* >(aControl));

    TInt error;
    TInt level;

    error = eventSource->ExecuteTrap(
                GetBandLevelL,
                control,
                (TInt) aBand,
                &level);

    if (error == KErrNone)
    {
        // aLevel is an array of 1 element allocated at the java side
        aJni->SetIntArrayRegion(
            /*destination*/ aLevel,
            /*first*/ 0,
            /*count*/ 1,
            /*source*/ &level);
    }
    return error;
}


//  End of File
