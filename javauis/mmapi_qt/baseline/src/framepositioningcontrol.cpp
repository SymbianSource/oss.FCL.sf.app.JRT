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
* Description:  This class has JNI wrappers for CMMAFramePositioningControl
*
*/

//#include <jutils.h>
//#include "mmapiutils.h"

#include "com_nokia_microedition_media_control_FramePositioningControl.h"
#include "mmafunctionserver.h"
#include "cmmaframepositioningcontrol.h"
#include <logger.h>

/**
 * Local function which can be used to call CMMAFramePositioningControl class methods.
 * Type of of the function pointer must be
 * TInt CMMAFramePositioningControl::aFunc( TInt )
 *
 * @param aFramePositioningControl CMMAFramePositioningControl pointer.
 * @param aFuncL Pointer to the CMMAFramePositioningControl method.
 * @param aParam Parameter value for aFuncL method.
 * @param aReturnValue The return value of the aFunc will
 *                     be assigned to this parameter.
 */
LOCAL_C void ReturnIntFuncIntL(CMMAFramePositioningControl* aFramePositioningControl,
                               TInt(CMMAFramePositioningControl::*aFuncL)(TInt aParam),
                               TInt aParam,
                               TInt* aReturnValue)
{
    // call TInt CMMAFramePositioningControl::aFunc( TInt ) method.
    *aReturnValue = (aFramePositioningControl->*aFuncL)(aParam);
}

//
// JNI functions. Prototypes are generated and commented in Java class
// com_nokia_microedition_media_control_FramePositioningControl
//

/**
 * JNI function from com.nokia.microedition.media.control.FramePositioningControl
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_media_control_FramePositioningControl__1seek
(JNIEnv*,
 jobject,
 jint aControlHandle,
 jint aEventSourceHandle,
 jint aFrameNumber)
{
    LOG( EJavaMMAPI, EInfo, "FramePositioningControl__1seek");

    // Get pointer to native event source.
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSourceHandle);
    CMMAFramePositioningControl* FramePositioningControl =
        reinterpret_cast< CMMAFramePositioningControl* >(aControlHandle);

    // Will leave on error. Otherwise returnValue contains frame number
    // to which the implementation has seeked.
    TInt returnValue = 0;
    TInt error = eventSource->ExecuteTrap(&ReturnIntFuncIntL,
                                          FramePositioningControl,
                                          &CMMAFramePositioningControl::SeekL,
                                          aFrameNumber,
                                          &returnValue);
   LOG1( EJavaMMAPI, EInfo, "FramePositioningControl__1seek error= %d",error);

    return (error == KErrNone) ? returnValue : error;
}

/**
 * JNI function from com.nokia.microedition.media.control.FramePositioningControl
 */
JNIEXPORT jintArray JNICALL
Java_com_nokia_microedition_media_control_FramePositioningControl__1skip
(JNIEnv* aJniEnv,
 jobject,
 jint aControlHandle,
 jint aEventSourceHandle,
 jintArray aValues)
{
    LOG( EJavaMMAPI, EInfo, "FramePositioningControl__1skip");

    // Get pointer to native event source.
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSourceHandle);

    CMMAFramePositioningControl* FramePositioningControl =
        reinterpret_cast< CMMAFramePositioningControl* >(aControlHandle);

    TInt value[ 2 ];
    aJniEnv->GetIntArrayRegion(aValues, 0, 2, &value[ 0 ]);

    // Will leave on error. Otherwise returnValue contains number of frames
    // skipped.
    value[ 1 ] = eventSource->ExecuteTrap(&ReturnIntFuncIntL,
                                          FramePositioningControl,
                                          &CMMAFramePositioningControl::SkipL,
                                          value[ 0 ],
                                          &value[ 0 ]);

    if (aValues)
    {
        aJniEnv->SetIntArrayRegion(aValues, 0, 2, &value[ 0 ]);
    }

    return aValues;
}

/**
 * Local function which can be used to call CMMAFramePositioningControl class methods.
 * Type of of the function pointer must be
 * void CMMAFramePositioningControl::aFunc( TInt, TInt64* )
 *
 * @param aFramePositioningControl CMMAFramePositioningControl pointer.
 * @param aFuncL Pointer to the CMMAFramePositioningControl method.
 * @param aParam Parameter value for aFuncL
 * @param aReturnValue The return value of the aFunc will
 *                     be assigned to this parameter.
 */
LOCAL_C void VoidFuncInt64L(CMMAFramePositioningControl* aFramePositioningControl,
                            void (CMMAFramePositioningControl::*aFuncL)(
                                TInt aParam,
                                TInt64* aReturnValue),
                            TInt aParam,
                            TInt64* aReturnValue)
{
    // call void CMMAFramePositioningControl::aFunc( TInt, TInt64 ) method.
    (aFramePositioningControl->*aFuncL)(aParam, aReturnValue);
}

/**
 * JNI function from com.nokia.microedition.media.control.FramePositioningControl
 */
JNIEXPORT jlong JNICALL
Java_com_nokia_microedition_media_control_FramePositioningControl__1mapFrameToTime
(JNIEnv*,
 jobject,
 jint aControlHandle,
 jint aEventSourceHandle,
 jint aFrameNumber)
{
    LOG( EJavaMMAPI, EInfo, "FramePositioningControl__1mapFrameToTime");

    // Get pointer to native event source.
    MMAFunctionServer* eventSource =
		reinterpret_cast< MMAFunctionServer *>(aEventSourceHandle);
    
	CMMAFramePositioningControl* FramePositioningControl =
        reinterpret_cast< CMMAFramePositioningControl* >(aControlHandle);

    // Will leave on error. Media time of frame in question.
    TInt64 returnValue(0);
    TInt error = eventSource->ExecuteTrap(&VoidFuncInt64L,
                                          FramePositioningControl,
                                          &CMMAFramePositioningControl::MapFrameToTimeL,
                                          aFrameNumber,
                                          &returnValue);

    return (error == KErrNone) ? returnValue : error;
}

/**
 * Local function which can be used to call CMMAFramePositioningControl class methods.
 * Type of of the function pointer must be
 * TInt CMMAFramePositioningControl::aFunc( TInt64* )
 *
 * @param aFramePositioningControl CMMAFramePositioningControl pointer.
 * @param aFuncL Pointer to the CMMAFramePositioningControl method.
 * @param aParam Parameter value for aFuncL method.
 * @param aReturnValue The return value of the aFunc will
 *                     be assigned to this parameter.
 */
LOCAL_C void ReturnIntFuncInt64L(CMMAFramePositioningControl* aFramePositioningControl,
                                 TInt(CMMAFramePositioningControl::*aFuncL)(TInt64* aParam),
                                 TInt64* aParam,
                                 TInt* aReturnValue)
{
    // call TInt CMMAFramePositioningControl::aFunc() method.
    *aReturnValue = (aFramePositioningControl->*aFuncL)(aParam);
}

/**
 * JNI function from com.nokia.microedition.media.control.FramePositioningControl
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_media_control_FramePositioningControl__1mapTimeToFrame
(JNIEnv*,
 jobject,
 jint aControlHandle,
 jint aEventSourceHandle,
 jlong aMediaTime)
{
    LOG( EJavaMMAPI, EInfo, "FramePositioningControl__1mapFrameToTime");

    // Get pointer to native event source.
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSourceHandle);
    CMMAFramePositioningControl* FramePositioningControl =
        reinterpret_cast< CMMAFramePositioningControl* >(aControlHandle);

    // Will leave on error. Media time of frame in question.
    TInt returnValue = 0;
    TInt error = eventSource->ExecuteTrap(&ReturnIntFuncInt64L,
                                          FramePositioningControl,
                                          &CMMAFramePositioningControl::MapTimeToFrameL,
                                          &aMediaTime,
                                          &returnValue);

    return (error == KErrNone) ? returnValue : error;
}
//  END OF FILE
