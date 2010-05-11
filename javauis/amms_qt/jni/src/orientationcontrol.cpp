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
* Description:  JNI file for OrientationControl
*
*/


//#include <jutils.h>

#include "com_nokia_amms_control_audio3d_OrientationControl.h"
#include <mmafunctionserver.h>
#include "cammsorientationcontrolgroup.h"

/**
 * wrapper for CAMMSOrientationControlGroup::OrientationVectors()
 */
static void GetOrientationVectorsL(
    CAMMSOrientationControlGroup* control,
    TInt aOrientation[ KAMMSTwoVectorComponents ])
{
    control->OrientationVectorsL(aOrientation);
}

/**
 * wrapper for CAMMSOrientationControlGroup::SetOrientationVectorsL()
 */
static void SetOrientationVectorsL(
    CAMMSOrientationControlGroup* control,
    TInt aFrontVector[ KAMMSVectorComponents ],
    TInt aAboveVector[ KAMMSVectorComponents ])
{
    control->SetOrientationVectorsL(aFrontVector, aAboveVector);
}

/**
 * wrapper for CAMMSOrientationControlGroup::SetOrientationL()
 */
static void SetOrientationL(
    CAMMSOrientationControlGroup* control,
    TInt aHeading,
    TInt aPitch,
    TInt aRoll)
{
    control->SetOrientationL(aHeading, aPitch, aRoll);
}

/*
 * Class:     com_nokia_amms_control_audio3d_OrientationControl
 * Method:    _setOrientation
 * Signature:
 */
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_audio3d_OrientationControl__1setOrientation(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aControl,
    jint aHeading,
    jint aPitch,
    jint aRoll)
{
    MMAFunctionServer* eventSource =
       reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSOrientationControlGroup* control =
        static_cast< CAMMSOrientationControlGroup* >(
            reinterpret_cast< CAMMSControlGroup* >(aControl));

    TInt error;
    error = eventSource->ExecuteTrap(SetOrientationL, control,
                                     aHeading, aPitch, aRoll);

    return error;
}

/*
 * Class:     com_nokia_amms_control_audio3d_OrientationControl
 * Method:    _getOrientationVectros
 * Signature:
 */
JNIEXPORT jintArray JNICALL
Java_com_nokia_amms_control_audio3d_OrientationControl__1getOrientationVectors(
    JNIEnv* aJni,
    jclass,
    jint aEventSource,
    jint aControl,
    jintArray aError)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSOrientationControlGroup* control =
        static_cast< CAMMSOrientationControlGroup* >(
            reinterpret_cast< CAMMSControlGroup* >(aControl));

    TInt orientation[ KAMMSTwoVectorComponents ];
    TInt error;
    error = eventSource->ExecuteTrap(
                GetOrientationVectorsL,
                control,
                orientation);

    // Java side method can not throw exception, store possible error here
    // aError is an array of 1 element allocated at the java side
    aJni->SetIntArrayRegion(
        /*destination*/ aError,
        /*first*/ 0,
        /*count*/ 1,
        /*source*/ &error);

    // allocate new array
    jintArray javaArray = aJni->NewIntArray(KAMMSTwoVectorComponents);

    if (javaArray)
    {
        aJni->SetIntArrayRegion(
            /*destination*/ javaArray,
            /*first*/0,
            /*count*/KAMMSTwoVectorComponents,
            /*source*/ &orientation[ 0 ]);
    }

    return javaArray;
}

/*
 * Class:     com_nokia_amms_control_audio3d_OrientationControl
 * Method:    _setOrientation
 * Signature:
 */
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_audio3d_OrientationControl__1setOrientationVectors(
    JNIEnv* aJni,
    jclass,
    jint aEventSource,
    jint aControl,
    jintArray aFrontVector,
    jintArray aAboveVector)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSource);

    CAMMSOrientationControlGroup* control =
        static_cast< CAMMSOrientationControlGroup* >(
           reinterpret_cast< CAMMSControlGroup *>(aControl));

    TInt i;

    // copy java front vector array to native front vector array
    TInt frontVector[ KAMMSVectorComponents ];
    TInt count = aJni->GetArrayLength(aFrontVector);
    jint* elements = aJni->GetIntArrayElements(aFrontVector, NULL);
    for (i = 0; i < count; i++)
    {
        frontVector[ i ] = elements[ i ];
    }
    aJni->ReleaseIntArrayElements(aFrontVector, elements, 0);

    // copy java above vector array to native above vector array
    TInt aboveVector[ KAMMSVectorComponents ];
    count = aJni->GetArrayLength(aAboveVector);
    elements = aJni->GetIntArrayElements(aAboveVector, NULL);
    for (i = 0; i < count; i++)
    {
        aboveVector[ i ] = elements[ i ];
    }
    aJni->ReleaseIntArrayElements(aAboveVector, elements, 0);

    TInt error;
    error = eventSource->ExecuteTrap(
                SetOrientationVectorsL,
                control,
                frontVector,
                aboveVector);

    return error;
}


//  End of File
