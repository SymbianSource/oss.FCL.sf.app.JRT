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
* Description:  JNI file for DopplerControl
*
*/


//#include <jutils.h>

#include "com_nokia_amms_control_audio3d_DopplerControl.h"
#include <mmafunctionserver.h>
#include "cammsdopplercontrolgroup.h"

/**
 * wrapper for CAMMSDopplerControlGroup::SetEnabledL()
 */
static void SetEnabledL(CAMMSDopplerControlGroup* aControl, TBool aEnabled)
{
    aControl->SetEnabledL(aEnabled);
}

/**
 * wrapper for CAMMSDopplerControlGroup::SetVelocitySphericalL()
 */
static void SetVelocitySphericalL(CAMMSDopplerControlGroup* aControl,
                                  TInt aAzimuth, TInt aElevation, TInt aRadius)
{
    aControl->SetVelocitySphericalL(aAzimuth, aElevation, aRadius);
}

/**
 * wrapper for CAMMSDopplerControlGroup::VelocityCartesian()
 */
static void GetVelocityCartesianL(
    CAMMSDopplerControlGroup* aControl,
    TInt aVelocity[ KAMMSVectorComponents ])
{
    aControl->VelocityCartesianL(aVelocity);
}

/**
 * wrapper for CAMMSDopplerControlGroup::Enabled()
 */
static TBool IsEnabled(CAMMSDopplerControlGroup* aControl)
{
    return aControl->Enabled();
}

/**
 * wrapper for CAMMSDopplerControlGroup::SetVelocityCartesianL()
 */
static void SetVelocityCartesianL(
    CAMMSDopplerControlGroup* aControl,
    TInt aX,
    TInt aY,
    TInt aZ)
{
    aControl->SetVelocityCartesianL(aX, aY, aZ);
}

/*
 * Class:     com_nokia_amms_control_audio3d_DopplerControl
 * Method:    _setEnabled
 * Signature: (IIZ)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_audio3d_DopplerControl__1setEnabled(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aDopplerControl,
    jboolean aEnabled)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSDopplerControlGroup* dopplerControl =
        static_cast<CAMMSDopplerControlGroup*>(
            reinterpret_cast< CAMMSControlGroup* >(aDopplerControl));

    TInt error;
    error = eventSource->ExecuteTrap(
                SetEnabledL,
                dopplerControl,
                (TBool) aEnabled);
    return error;
}

/*
 * Class:     com_nokia_amms_control_audio3d_DopplerControl
 * Method:    _setVelocitySpherical
 * Signature: (IIIII)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_audio3d_DopplerControl__1setVelocitySpherical(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aDopplerControl,
    jint aAzimuth,
    jint aElevation,
    jint aRadius)
{
    MMAFunctionServer* eventSource =
		reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSDopplerControlGroup* dopplerControl =
        static_cast<CAMMSDopplerControlGroup*>(
            reinterpret_cast< CAMMSControlGroup *>(aDopplerControl));

    TInt error;
    error = eventSource->ExecuteTrap(SetVelocitySphericalL, dopplerControl,
                                     (TInt) aAzimuth, (TInt) aElevation, (TInt) aRadius);

    return error;
}

/*
 * Class:     com_nokia_amms_control_audio3d_DopplerControl
 * Method:    _getVelocityCartesian
 * Signature: (II)[I
 */
JNIEXPORT jintArray JNICALL
Java_com_nokia_amms_control_audio3d_DopplerControl__1getVelocityCartesian(
    JNIEnv* aJni,
    jclass,
    jint aEventSource,
    jint aDopplerControl,
    jintArray aError)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSource);

    CAMMSDopplerControlGroup* dopplerControl =
        static_cast<CAMMSDopplerControlGroup*>(
            reinterpret_cast< CAMMSControlGroup* >(aDopplerControl));

    TInt velocity[ KAMMSVectorComponents ];
    TInt error = eventSource->ExecuteTrap(GetVelocityCartesianL,
                                          dopplerControl,
                                          velocity);
    // allocate new array
    jintArray javaArray = aJni->NewIntArray(KAMMSVectorComponents);
    if (javaArray)
    {
        aJni->SetIntArrayRegion(
            /*destination*/ javaArray,
            /*first*/ 0,
            /*count*/ KAMMSVectorComponents,
            /*source*/ velocity);
    }

    jint javaError[ 1 ] = { error };
    aJni->SetIntArrayRegion(aError, 0, 1, javaError);

    return javaArray;
}

/*
 * Class:     com_nokia_amms_control_audio3d_DopplerControl
 * Method:    _isEnabled
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL
Java_com_nokia_amms_control_audio3d_DopplerControl__1isEnabled(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aDopplerControl)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSDopplerControlGroup* dopplerControl =
        static_cast<CAMMSDopplerControlGroup*>(
            reinterpret_cast< CAMMSControlGroup* >(aDopplerControl));

    return (jboolean) eventSource->Execute(IsEnabled, dopplerControl);
}

/*
 * Class:     com_nokia_amms_control_audio3d_DopplerControl
 * Method:    _setVelocityCartesian
 * Signature: (IIIII)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_audio3d_DopplerControl__1setVelocityCartesian(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aDopplerControl,
    jint aX,
    jint aY,
    jint aZ)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSDopplerControlGroup* dopplerControl =
        static_cast<CAMMSDopplerControlGroup*>(
            reinterpret_cast< CAMMSControlGroup* >(aDopplerControl));

    TInt error;
    error = eventSource->ExecuteTrap(
                SetVelocityCartesianL,
                dopplerControl,
                (TInt) aX,
                (TInt) aY,
                (TInt) aZ);

    return error;
}

//  End of File
