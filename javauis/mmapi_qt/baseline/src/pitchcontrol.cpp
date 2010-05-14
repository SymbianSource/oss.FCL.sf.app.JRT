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
* Description:  This class has JNI wrappers for CMMAPitchControl
*
*/

//#include <jutils.h>
//#include "mmapiutils.h"

#include "com_nokia_microedition_media_control_PitchControl.h"
#include "mmafunctionserver.h"
#include "cmmamidipitchcontrol.h"
#include <logger.h>

static const int KPitchOffset = 1000000;

/**
 * Local function which can be used to call CMMAPitchControl class methods.
 * Type of of the function pointer must be
 * TInt CMMAPitchControl::aFunc( TInt aData )
 *
 * @param aPitchControl CMMAPitchControl pointer.
 * @param aFunc Pointer to the CMMAPitchControl method.
 * @param aData Parameter to passed to the aFunc method
 * @param aReturnValue The return value of the aFunc will
 *                     be assigned to this parameter.
 */
LOCAL_C void ReturnIntParamIntFuncL(CMMAMIDIPitchControl* aPitchControl,
                                    TInt(CMMAMIDIPitchControl::*aFuncL)(TInt),
                                    TInt aData,
                                    TInt* aReturnValue)
{
    // call TInt CMMAPitchControl::aFunc( TInt aData ) method.
    *aReturnValue = (aPitchControl->*aFuncL)(aData);
}

/**
 * Local function which can be used to call CMMAPitchControl class methods.
 * Type of of the function pointer must be
 * TInt CMMAPitchControl::aFunc()
 *
 * @param aPitchControl CMMAPitchControl pointer.
 * @param aFunc Pointer to the CMMAPitchControl method.
 * @param aReturnValue The return value of the aFunc will
 *                     be assigned to this parameter.
 */
LOCAL_C void ReturnIntFuncL(CMMAMIDIPitchControl* aPitchControl,
                            TInt(CMMAMIDIPitchControl::*aFuncL)(),
                            TInt* aReturnValue)
{
    // call TInt CMMAPitchControl::aFunc() method.
    *aReturnValue = (aPitchControl->*aFuncL)();
}

//
// JNI functions. Prototypes are generated and commented in Java class
// com_nokia_microedition_media_control_PitchControl
//

/**
 * JNI function from com.nokia.microedition.media.control.PitchControl
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_media_control_PitchControl__1setPitch
(JNIEnv*,
 jobject,
 jint aControlHandle,
 jint aEventSourceHandle,
 jint aPitch)  // parameter boundary is checked in Java side.
{
    // Get pointer to native event source.
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSourceHandle);

    CMMAMIDIPitchControl* PitchControl =
        reinterpret_cast< CMMAMIDIPitchControl *>(aControlHandle);

    // Value returned from SetPitch method will be assigned to returnValue.
    TInt returnValue = 0;

    // Setting Pitch will not leave, it just assigns default
    // value to the returnValue variable.
    TInt error;
    error = eventSource->ExecuteTrap(&ReturnIntParamIntFuncL,
                                     PitchControl,
                                     &CMMAMIDIPitchControl::SetPitchL,
                                     aPitch,
                                     &returnValue);

    LOG1( EJavaMMAPI, EInfo, "PitchControl__1setPitch return value %d", returnValue);

    return (error == KErrNone) ? (returnValue+KPitchOffset) : error;
}

/**
 * JNI function from com.nokia.microedition.media.control.PitchControl
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_media_control_PitchControl__1getMinPitch
(JNIEnv*,
 jobject,
 jint aControlHandle,
 jint aEventSourceHandle)
{
    // Get pointer to native event source.
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSourceHandle);

    CMMAMIDIPitchControl* PitchControl =
        reinterpret_cast< CMMAMIDIPitchControl* >(aControlHandle);

    TInt returnValue = 0;

    // Get value or the default.
    TInt error;
    error = eventSource->ExecuteTrap(&ReturnIntFuncL,
                                     PitchControl,
                                     &CMMAMIDIPitchControl::MinPitchL,
                                     &returnValue);

    LOG1( EJavaMMAPI, EInfo, "PitchControl__1setPitch return value %d", returnValue);

    return (error == KErrNone) ? (returnValue+KPitchOffset) : error;
}


/**
 * JNI function from com.nokia.microedition.media.control.PitchControl
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_media_control_PitchControl__1getMaxPitch
(JNIEnv*,
 jobject,
 jint aControlHandle,
 jint aEventSourceHandle)
{
    // Get pointer to native event source.
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSourceHandle);

    CMMAMIDIPitchControl* PitchControl =
        reinterpret_cast< CMMAMIDIPitchControl* >(aControlHandle);        

    TInt returnValue = 0;
    TInt error;

    error = eventSource->ExecuteTrap(&ReturnIntFuncL,
                                     PitchControl,
                                     &CMMAMIDIPitchControl::MaxPitchL,
                                     &returnValue);
    LOG1( EJavaMMAPI, EInfo, "PitchControl__1getMaxRate return value %d", returnValue);

    return (error == KErrNone) ? (returnValue+KPitchOffset) : error;
}

/**
 * JNI function from com.nokia.microedition.media.control.PitchControl
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_media_control_PitchControl__1getPitch
(JNIEnv*,
 jobject,
 jint aControlHandle,
 jint aEventSourceHandle)
{
    // Get pointer to native event source.
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSourceHandle);

    CMMAMIDIPitchControl* PitchControl =
        reinterpret_cast< CMMAMIDIPitchControl* >(aControlHandle);

    TInt returnValue = 0;
    TInt error;

    error = eventSource->ExecuteTrap(&ReturnIntFuncL,
                                     PitchControl,
                                     &CMMAMIDIPitchControl::PitchL,
                                     &returnValue);

    LOG1( EJavaMMAPI, EInfo, "PitchControl__1getPitch return value %d", returnValue);

    return (error == KErrNone) ? (returnValue+KPitchOffset) : error;
}
//  END OF FILE
