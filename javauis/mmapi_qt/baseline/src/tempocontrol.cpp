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
* Description:  This class has JNI wrappers for CMMATempoControl
*
*/

//#include <jutils.h>
//#include "mmapiutils.h"

#include "com_nokia_microedition_media_control_TempoControl.h"
#include "mmafunctionserver.h"
#include "cmmamiditempocontrol.h"
#include <logger.h>

//
// Wrapper functions which are needed in event source execution.
// If method to be called can leave ExecuteTrap must be used,
// otherwise Execute and ExecuteV can be used.
//
//

/**
 * Local function which can be used to call CMMATempoControl class methods.
 * Type of of the function pointer must be
 * TInt CMMATempoControl::aFunc( TInt aData )
 *
 * @param aTempoControl CMMATempoControl pointer.
 * @param aFunc Pointer to the CMMATempoControl method.
 * @param aData Parameter to passed to the aFunc method
 * @param aReturnValue The return value of the aFunc will
 *                     be assigned to this parameter.
 */
LOCAL_C void ReturnIntParamIntFuncL(CMMAMIDITempoControl* aTempoControl,
                                    TInt(CMMAMIDITempoControl::*aFuncL)(TInt),
                                    TInt aData,
                                    TInt* aReturnValue)
{
    // call TInt CMMATempoControl::aFunc( TInt aData ) method.
    *aReturnValue = (aTempoControl->*aFuncL)(aData);
}

/**
 * Local function which can be used to call CMMATempoControl class methods.
 * Type of of the function pointer must be
 * TInt CMMATempoControl::aFunc()
 *
 * @param aTempoControl CMMATempoControl pointer.
 * @param aFunc Pointer to the CMMATempoControl method.
 * @param aReturnValue The return value of the aFunc will
 *                     be assigned to this parameter.
 */
LOCAL_C void ReturnIntFuncL(CMMAMIDITempoControl* aTempoControl,
                            TInt(CMMAMIDITempoControl::*aFuncL)(),
                            TInt* aReturnValue)
{
    // call TInt CMMATempoControl::aFunc() method.
    *aReturnValue = (aTempoControl->*aFuncL)();
}

//
// JNI functions. Prototypes are generated and commented in Java class
// com_nokia_microedition_media_control_TempoControl
//

/**
 * JNI function from com.nokia.microedition.media.control.TempoControl
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_media_control_TempoControl__1setTempo
(JNIEnv*,
 jobject,
 jint aControlHandle,
 jint aEventSourceHandle,
 jint aTempo)  // parameter boundary is checked in Java side.
{
    // Get pointer to native event source.
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSourceHandle);

    CMMAMIDITempoControl* tempoControl =
        reinterpret_cast< CMMAMIDITempoControl* >(aControlHandle);

    // Value returned from SetTempo method will be assigned to returnValue.
    TInt returnValue = 0;

    // Setting tempo will not leave, it just assigns default
    // value to the returnValue variable.
    TInt error;
    error = eventSource->ExecuteTrap(&ReturnIntParamIntFuncL,
                                     tempoControl,
                                     &CMMAMIDITempoControl::SetTempoL,
                                     aTempo,
                                     &returnValue);

    LOG1(EJavaMMAPI, EInfo, "TempoControl__1setTempo return value %d", returnValue);

    return (error == KErrNone) ? returnValue : error;
}

/**
 * JNI function from com.nokia.microedition.media.control.TempoControl
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_media_control_TempoControl__1getTempo
(JNIEnv*,
 jobject,
 jint aControlHandle,
 jint aEventSourceHandle)
{
    // Get pointer to native event source.
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSourceHandle);

    CMMAMIDITempoControl* tempoControl =
        reinterpret_cast< CMMAMIDITempoControl* >(aControlHandle);

    TInt returnValue = 0;
    TInt error;

    error = eventSource->ExecuteTrap(&ReturnIntFuncL,
                                     tempoControl,
                                     &CMMAMIDITempoControl::TempoL,
                                     &returnValue);

    LOG1(EJavaMMAPI, EInfo, "TempoControl__1getTempo return value %d", returnValue);

    return (error == KErrNone) ? returnValue : error;
}
//  END OF FILE
