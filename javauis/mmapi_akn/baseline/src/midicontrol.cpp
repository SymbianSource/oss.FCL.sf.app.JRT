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
* Description:  This class has JNI wrappers for CMMAMIDIControl
*
*/

#include <jutils.h>

#include "com_nokia_microedition_media_control_MIDIControl.h"
#include "cmmaeventsource.h"
#include "cmmamidicontrol.h"
#include <jdebug.h>

/**
 * Local function which is used to call CMMAMIDIControl class method.
 * Type of of the function pointer must be
 * void CMMAMIDIControl::aFunc( TInt, TInt, TInt )
 *
 * @param aMIDIControl CMMAMIDIControl pointer.
 * @param aFunc Pointer to the CMMAMIDIControl method.
 * @param aParam1 Parameter passed to the aFunc.
 * @param aParam2 Parameter passed to the aFunc.
 * @param aParam3 Parameter passed to the aFunc.
 */
LOCAL_C void ReturnVoidParam3IntFuncL(
    CMMAMIDIControl* aMIDIControl,
    void (CMMAMIDIControl::*aFuncL)(TInt, TInt, TInt),
    TInt aParam1,
    TInt aParam2,
    TInt aParam3)
{
    // call TInt CMMAMIDIControl::aFunc( TInt, TInt, TInt ) method.
    (aMIDIControl->*aFuncL)(aParam1, aParam2, aParam3);
}

/**
 * Implements generated JNI function prototype.
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_media_control_MIDIControl__1setProgram
(
    JNIEnv*,
    jobject,
    jint aControlHandle,
    jint aEventSourceHandle,
    jint aChannel,
    jint aBank,
    jint aProgram)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMAMIDIControl* midiControl =
        JavaUnhand< CMMAMIDIControl >(aControlHandle);

    TInt error;
    error = eventSource->ExecuteTrap(&ReturnVoidParam3IntFuncL,
                                     midiControl,
                                     &CMMAMIDIControl::SetProgramL,
                                     aChannel,
                                     aBank,
                                     aProgram);

    return error;
}

/**
 * Local function which is used to call CMMAMIDIControl class method.
 * Type of of the function pointer must be
 * TInt CMMAMIDIControl::aFunc( TInt )
 *
 * @param aMIDIControl CMMAMIDIControl pointer.
 * @param aFunc Pointer to the CMMAMIDIControl method.
 * @param aData Parameter passed to the aFunc.
 * @param aReturnValue The return value of the aFunc will
 *                     be assigned to this parameter.
 */
LOCAL_C void ReturnIntParamIntFuncL(
    CMMAMIDIControl* aMIDIControl,
    TInt(CMMAMIDIControl::*aFuncL)(TInt aData),
    TInt aData,
    TInt* aReturnValue)
{
    // call TInt CMMAMIDIControl::aFunc( TDesC* aData ) method.
    *aReturnValue = (aMIDIControl->*aFuncL)(aData);
}

/**
 * Implements generated JNI function prototype.
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_media_control_MIDIControl__1getChannelVolume
(JNIEnv*,
 jobject,
 jint aControlHandle,
 jint aEventSourceHandle,
 jint aChannel)
{
    // Get pointer to native event source.
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMAMIDIControl* midiControl =
        JavaUnhand< CMMAMIDIControl >(aControlHandle);


    TInt returnValue = 0;
    TInt error;

    error = eventSource->ExecuteTrap(&ReturnIntParamIntFuncL,
                                     midiControl,
                                     &CMMAMIDIControl::ChannelVolumeL,
                                     aChannel,
                                     &returnValue);

    return (error == KErrNone) ? returnValue : error;
}

/**
 * Local function which is used to call CMMAMIDIControl class method.
 * Type of of the function pointer must be
 * void CMMAMIDIControl::aFunc( TInt, TInt )
 *
 * @param aMIDIControl CMMAMIDIControl pointer.
 * @param aFunc Pointer to the CMMAMIDIControl method.
 * @param aParam1 Parameter passed to the aFunc.
 * @param aParam2 Parameter passed to the aFunc.
 */
LOCAL_C void ReturnVoidParamIntIntFuncL(
    CMMAMIDIControl* aMIDIControl,
    void (CMMAMIDIControl::*aFuncL)(TInt, TInt),
    TInt aParam1,
    TInt aParam2)
{
    // call TInt CMMAMIDIControl::aFunc( TInt, TInt ) method.
    (aMIDIControl->*aFuncL)(aParam1, aParam2);
}

/**
 * Implements generated JNI function prototype.
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_media_control_MIDIControl__1setChannelVolume
(JNIEnv*,
 jobject,
 jint aControlHandle,
 jint aEventSourceHandle,
 jint aChannel,
 jint aVolume)
{
    // Get pointer to native event source.
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMAMIDIControl* midiControl =
        JavaUnhand< CMMAMIDIControl >(aControlHandle);

    TInt error = eventSource->ExecuteTrap(&ReturnVoidParamIntIntFuncL,
                                          midiControl,
                                          &CMMAMIDIControl::SetChannelVolumeL,
                                          aChannel,
                                          aVolume);

    return error;
}

/**
 * Local function which can be used to call CMMAMIDIControl class methods.
 * Type of of the function pointer must be
 * TInt CMMAMIDIControl::aFunc( TDesC* aData )
 *
 * @param aMIDIControl CMMAMIDIControl pointer.
 * @param aFunc Pointer to the CMMAMIDIControl method.
 * @param aData Parameter passed to the aFunc.
 * @param aReturnValue The return value of the aFunc will
 *                     be assigned to this parameter.
 */
LOCAL_C void SendMIDIEventFuncL(
    CMMAMIDIControl* aMIDIControl,
    TInt(CMMAMIDIControl::*aFuncL)(const TDesC8* aData),
    const TDesC8* aData,
    TInt* aReturnValue)
{
    // call TInt CMMAMIDIControl::aFunc( TDesC* aData ) method.
    *aReturnValue = (aMIDIControl->*aFuncL)(aData);
}

/**
 * Implements generated JNI function prototype.
 */
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_media_control_MIDIControl__1sendMidiEvent
(JNIEnv* aJni,
 jobject,
 jint aControlHandle,
 jint aEventSourceHandle,
 jbyteArray aData,
 jint aOffset,
 jint aLength)
{
    // Get pointer to native event source.
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMAMIDIControl* midiControl =
        JavaUnhand< CMMAMIDIControl >(aControlHandle);

    // Get pointer to Java data
    jbyte* data = aJni->GetByteArrayElements(aData, NULL);

    // Assert only in debug builds, array size must be checked in Java.
    __ASSERT_DEBUG(aJni->GetArrayLength(aData) >= aOffset + aLength &&
                   aOffset >= 0 &&
                   aLength > 0,
                   User::Invariant());

    // Create descriptor from Java data starting from offset
    jbyte* tempJbyte = &data[ aOffset ];
    TUint8* tempPtr = (TUint8*) tempJbyte;
    TPtrC8 dataPtr(tempPtr, aLength);

    TInt returnValue = 0;

    TInt err = eventSource->ExecuteTrap(&SendMIDIEventFuncL,
                                        midiControl,
                                        &CMMAMIDIControl::SendMIDIEventL,
                                        (const TDesC8*)(&dataPtr),
                                        &returnValue);

    // Java bytes are not needed anymore
    aJni->ReleaseByteArrayElements(aData, data, 0);

    if (err != KErrNone)
    {
        // return error code to Java
        returnValue = err;
    }

    return returnValue;
}

JNIEXPORT jint JNICALL
Java_com_nokia_microedition_media_control_MIDIControl__1reInitializeMidi
(JNIEnv* aJni,
 jobject,
 jint aControlHandle,
 jint aEventSourceHandle,
 jbyteArray aMidiSequence,
 jint aOffset,
 jint aLength)
{
    // Get pointer to native event source.
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMAMIDIControl* midiControl =
        JavaUnhand< CMMAMIDIControl >(aControlHandle);

    // Assert only in debug builds, array size must be checked in Java.
    __ASSERT_DEBUG(aJni->GetArrayLength(aMidiSequence) >= aOffset + aLength &&
                   aOffset >= 0 &&
                   aLength > 0,
                   User::Invariant());

    // Get pointer to Java data
    jbyte* data = aJni->GetByteArrayElements(aMidiSequence, NULL);

    // Create descriptor from Java data starting from offset
    jbyte* tempJbyte = &data[ aOffset ];
    TUint8* tempPtr = (TUint8*) tempJbyte;
    TPtrC8 dataPtr(tempPtr, aLength);

    // SendMIDIEventFuncL has suitable enough footprint for this call too.
    // Return value is not used but needs to be there for method footprint.

    TInt returnValue = 0;
    TInt err = eventSource->ExecuteTrap(&SendMIDIEventFuncL,
                                        midiControl,
                                        &CMMAMIDIControl::ReInitializeMidiL,
                                        (const TDesC8*)(&dataPtr),
                                        &returnValue);

    // Release Java bytes
    aJni->ReleaseByteArrayElements(aMidiSequence, data, 0);
    return err;
}



//  END OF FILE
