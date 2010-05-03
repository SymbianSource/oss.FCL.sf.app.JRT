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

#include "com_nokia_amms_control_audio3d_LocationControl.h"
#include <cmmaeventsource.h>
#include "cammslocationcontrolgroup.h"
#include "ammsconstants.h"

/**
 * wrapper for CAMMSLocationControlGroup::Cartesian()
 */
static void GetCartesianL(
    CAMMSLocationControlGroup* control,
    TInt aLocation[ KAMMSVectorComponents ])
{
    control->CartesianL(aLocation);
}

/**
 * wrapper for CAMMSLocationControlGroup::SetCartesianL()
 */
static void SetCartesianL(
    CAMMSLocationControlGroup* control,
    TInt aX,
    TInt aY,
    TInt aZ)
{
    control->SetCartesianL(aX, aY, aZ);
}

/**
 * wrapper for CAMMSLocationControlGroup::SetSphericalL()
 */
static void SetSphericalL(
    CAMMSLocationControlGroup* control,
    TInt aAzimuth,
    TInt aElevation,
    TInt aRadius)
{
    control->SetSphericalL(aAzimuth, aElevation, aRadius);
}

/*
 * Class:     com_nokia_amms_control_audio3d_LocationControl
 * Method:    _setSpherical
 * Signature: (IIIII)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audio3d_LocationControl__1setSpherical(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aControl,
    jint aAzimuth,
    jint aElevation,
    jint aRadius)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CAMMSLocationControlGroup* control = static_cast<CAMMSLocationControlGroup*>(
                                             JavaUnhand< CAMMSControlGroup >(aControl));

    TInt error;
    error = eventSource->ExecuteTrap(SetSphericalL, control,
                                     aAzimuth, aElevation, aRadius);

    return error;
}

/*
 * Class:     com_nokia_amms_control_audio3d_LocationControl
 * Method:    _getCartesian
 * Signature: (II)LI
 */
JNIEXPORT jintArray JNICALL Java_com_nokia_amms_control_audio3d_LocationControl__1getCartesian(
    JNIEnv* aJni,
    jclass,
    jint aEventSource,
    jint aControl,
    jintArray aError)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CAMMSLocationControlGroup* control = static_cast<CAMMSLocationControlGroup*>(
                                             JavaUnhand< CAMMSControlGroup >(aControl));

    TInt location[ KAMMSVectorComponents ];
    TInt error = eventSource->ExecuteTrap(GetCartesianL,
                                          control,
                                          location);
    // allocate new array
    jintArray javaArray = aJni->NewIntArray(KAMMSVectorComponents);
    if (javaArray)
    {
        aJni->SetIntArrayRegion(
            /*destination*/ javaArray,
            /*first*/0,
            /*count*/KAMMSVectorComponents,
            /*source*/ &location[ 0 ]);
    }

    jint javaError[ 1 ] = { error };
    aJni->SetIntArrayRegion(aError, 0, 1, javaError);

    return javaArray;
}

/*
 * Class:     com_nokia_amms_control_audio3d_LocationControl
 * Method:    _setCartesian
 * Signature: (IIIII)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audio3d_LocationControl__1setCartesian(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aControl,
    jint aX,
    jint aY,
    jint aZ)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CAMMSLocationControlGroup* control = static_cast<CAMMSLocationControlGroup*>(
                                             JavaUnhand< CAMMSControlGroup >(aControl));

    TInt error;
    error = eventSource->ExecuteTrap(
                SetCartesianL,
                control,
                aX,
                aY,
                aZ);

    return error;
}


//  End of File
