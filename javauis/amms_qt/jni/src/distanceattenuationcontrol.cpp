/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  JNI file for DistanceAttenuationControl
*
*/


//#include <jutils.h>

#include "com_nokia_amms_control_audio3d_DistanceAttenuationControl.h"
#include <mmafunctionserver.h>
#include "cammsdistanceattenuationcontrolgroup.h"

/**
 * wrapper for CAMMSDistanceAttenuationControlGroup::MuteAfterMax()
 */
static TBool MuteAfterMax(CAMMSDistanceAttenuationControlGroup* aControl)
{
    return aControl->MuteAfterMax();
}

/**
 * wrapper for CAMMSDistanceAttenuationControlGroup::MinDistance()
 */
static TInt32 MinDistance(CAMMSDistanceAttenuationControlGroup* aControl)
{
    return aControl->MinDistance();
}

/**
 * wrapper for CAMMSDistanceAttenuationControlGroup::MaxDistance()
 */
static TInt32 MaxDistance(CAMMSDistanceAttenuationControlGroup* aControl)
{
    return aControl->MaxDistance();
}

/**
 * wrapper for CAMMSDistanceAttenuationControlGroup::RolloffFactor()
 */
static TUint32 RolloffFactor(CAMMSDistanceAttenuationControlGroup* aControl)
{
    return aControl->RolloffFactor();
}

/**
 * wrapper for CAMMSDistanceAttenuationControlGroup::SetParametersL()
 */
static void SetParametersL(
    CAMMSDistanceAttenuationControlGroup* aControl,
    TInt aMinDistance,
    TInt aMaxDistance,
    TBool aMuteAfterMax,
    TInt aRolloffFactor)
{
    aControl->SetParametersL(
        aMinDistance,
        aMaxDistance,
        aMuteAfterMax,
        aRolloffFactor);
}

/*
 * Class:     com_nokia_amms_control_audio3d_DistanceAttenuationControl
 * Method:    _getMuteAfterMax
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL
Java_com_nokia_amms_control_audio3d_DistanceAttenuationControl__1getMuteAfterMax(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aDistControl)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSDistanceAttenuationControlGroup* distControl =
        static_cast<CAMMSDistanceAttenuationControlGroup*>(
    	    reinterpret_cast< CAMMSControlGroup* >(aDistControl));

    return (jboolean) eventSource->Execute(MuteAfterMax, distControl);
}

/*
 * Class:     com_nokia_amms_control_audio3d_DistanceAttenuationControl
 * Method:    _getMinDistance
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_audio3d_DistanceAttenuationControl__1getMinDistance(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aDistControl)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSDistanceAttenuationControlGroup* distControl =
        static_cast<CAMMSDistanceAttenuationControlGroup*>(
            reinterpret_cast< CAMMSControlGroup* >(aDistControl));

    return (jint) eventSource->Execute(MinDistance, distControl);
}

/*
 * Class:     com_nokia_amms_control_audio3d_DistanceAttenuationControl
 * Method:    _getMaxDistance
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_audio3d_DistanceAttenuationControl__1getMaxDistance(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aDistControl)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSDistanceAttenuationControlGroup* distControl =
        static_cast<CAMMSDistanceAttenuationControlGroup*>(
            reinterpret_cast< CAMMSControlGroup* >(aDistControl));

    return (jint) eventSource->Execute(MaxDistance, distControl);
}

/*
 * Class:     com_nokia_amms_control_audio3d_DistanceAttenuationControl
 * Method:    _getRolloffFactor
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_audio3d_DistanceAttenuationControl__1getRolloffFactor(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aDistControl)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSDistanceAttenuationControlGroup* distControl =
        static_cast<CAMMSDistanceAttenuationControlGroup*>(
            reinterpret_cast< CAMMSControlGroup *>(aDistControl));

    return (jint) eventSource->Execute(RolloffFactor, distControl);
}

/*
 * Class:     com_nokia_amms_control_audio3d_DistanceAttenuationControl
 * Method:    _setParameters
 * Signature: (IIIIZI)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_audio3d_DistanceAttenuationControl__1setParameters(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aDistControl,
    jint aMinDistance,
    jint aMaxDistance,
    jboolean aMuteAfterMax,
    jint aRolloffFactor)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSDistanceAttenuationControlGroup* distControl =
        static_cast<CAMMSDistanceAttenuationControlGroup*>(
            reinterpret_cast< CAMMSControlGroup* >(aDistControl));

    TInt error;
    error = eventSource->ExecuteTrap(
                &SetParametersL,
                distControl,
                (TInt) aMinDistance,
                (TInt) aMaxDistance,
                (TBool) aMuteAfterMax,
                (TInt) aRolloffFactor);

    return error;
}

//  End of File
