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
* Description:  This class has JNI wrappers for CMMARateControl
*
*/

#include <jutils.h>

#include "com_nokia_microedition_media_control_RateControl.h"
#include "cmmaeventsource.h"
#include "cmmaratecontrol.h"
#include <jdebug.h>

//
// Wrapper functions which are needed in event source execution.
// If method to be called can leave ExecuteTrap must be used,
// otherwise Execute and ExecuteV can be used.
//
//

/**
 * Local function which can be used to call CMMARateControl class methods.
 * Type of of the function pointer must be
 * TInt CMMARateControl::aFunc( TInt aData )
 *
 * @param aRateControl CMMARateControl pointer.
 * @param aFunc Pointer to the CMMARateControl method.
 * @param aData Parameter to passed to the aFunc method
 * @param aReturnValue The return value of the aFunc will
 *                     be assigned to this parameter.
 */
LOCAL_C void ReturnIntParamIntFuncL(CMMARateControl* aRateControl,
                                    TInt(CMMARateControl::*aFuncL)(TInt),
                                    TInt aData,
                                    TInt* aReturnValue)
{
    // call TInt CMMARateControl::aFunc( TInt aData ) method.
    *aReturnValue = (aRateControl->*aFuncL)(aData);
}

/**
 * Local function which can be used to call CMMARateControl class methods.
 * Type of of the function pointer must be
 * TInt CMMARateControl::aFunc()
 *
 * @param aRateControl CMMARateControl pointer.
 * @param aFunc Pointer to the CMMARateControl method.
 * @param aReturnValue The return value of the aFunc will
 *                     be assigned to this parameter.
 */
LOCAL_C void ReturnIntFuncL(CMMARateControl* aRateControl,
                            TInt(CMMARateControl::*aFuncL)(),
                            TInt* aReturnValue)
{
    // call TInt CMMARateControl::aFunc() method.
    *aReturnValue = (aRateControl->*aFuncL)();
}

//
// JNI functions. Prototypes are generated and commented in Java class
// com_nokia_microedition_media_control_RateControl
//

/**
 * JNI function from com.nokia.microedition.media.control.RateControl
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_media_control_RateControl__1getMinRate
(JNIEnv*,
 jobject,
 jint aControlHandle,
 jint aEventSourceHandle)
{
    // Get pointer to native event source.
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMARateControl* RateControl =
        JavaUnhand< CMMARateControl >(aControlHandle);

    TInt returnValue = 0;

    // Get value or the default.
    TInt error;
    error = eventSource->ExecuteTrap(&ReturnIntFuncL,
                                     RateControl,
                                     &CMMARateControl::MinRateL,
                                     &returnValue);

    DEBUG_INT("RateControl__1getMinRate return value %d", returnValue);

    return (error == KErrNone) ? returnValue : error;
}

/**
 * JNI function from com.nokia.microedition.media.control.RateControl
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_media_control_RateControl__1getRate
(JNIEnv*,
 jobject,
 jint aControlHandle,
 jint aEventSourceHandle)
{
    // Get pointer to native event source.
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMARateControl* RateControl =
        JavaUnhand< CMMARateControl >(aControlHandle);


    TInt returnValue = 0;

    // Get actual rate or the default value to the returnValue.
    TInt error;
    error = eventSource->ExecuteTrap(&ReturnIntFuncL,
                                     RateControl,
                                     &CMMARateControl::RateL,
                                     &returnValue);

    DEBUG_INT("RateControl__1getRate return value %d", returnValue);

    return (error == KErrNone) ? returnValue : error;
}

/**
 * JNI function from com.nokia.microedition.media.control.RateControl
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_media_control_RateControl__1setRate
(JNIEnv*,
 jobject,
 jint aControlHandle,
 jint aEventSourceHandle,
 jint aRate)
{
    // Get pointer to native event source.
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMARateControl* RateControl =
        JavaUnhand< CMMARateControl >(aControlHandle);

    // Return value will be actual rate set or default value if rate is not
    // available.
    TInt returnValue = 0;

    TInt error;
    error = eventSource->ExecuteTrap(&ReturnIntParamIntFuncL,
                                     RateControl,
                                     &CMMARateControl::SetRateL,
                                     aRate,
                                     &returnValue);

    DEBUG_INT("RateControl__1setRate return value %d", returnValue);

    return (error == KErrNone) ? returnValue : error;
}

/**
 * JNI function from com.nokia.microedition.media.control.RateControl
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_media_control_RateControl__1getMaxRate
(JNIEnv*,
 jobject,
 jint aControlHandle,
 jint aEventSourceHandle)
{
    // Get pointer to native event source.
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMARateControl* RateControl =
        JavaUnhand< CMMARateControl >(aControlHandle);

    TInt returnValue = 0;
    TInt error;

    error = eventSource->ExecuteTrap(&ReturnIntFuncL,
                                     RateControl,
                                     &CMMARateControl::MaxRateL,
                                     &returnValue);
    DEBUG_INT("RateControl__1getMaxRate return value %d", returnValue);

    return (error == KErrNone) ? returnValue : error;
}

//  END OF FILE
