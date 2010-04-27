/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Creates cammsaudiooutputcontrol.
*
*/


#include <jutils.h>
#include <jdebug.h>
#include "com_nokia_mid_impl_media_AudioOutputControl.h"
#include <cmmaeventsource.h>
#include "cammsaudiooutputcontrol.h"

/**
* wrapper for CAMMSAudioOutputControl::GetPreference()
*/
static TInt GetPreference(CAMMSAudioOutputControl* aControl)
{
    TInt val = aControl->GetAudioOutput();
    return val;
}

/**
* wrapper for CAMMSAudioOutputControl::SetPreferenceL()
*/
static TInt SetPreference(CAMMSAudioOutputControl* aControl, TInt aPreference)
{
    return aControl->SetAudioOutput(aPreference);
}

/**
* wrapper for CAMMSAudioOutputControl::GetCurrent()
*/
static void GetCurrent(CAMMSAudioOutputControl* aControl)
{
    aControl->GetCurrentPreference();
}

/**
* wrapper for CAMMSAudioOutputControl::SetJavaAudioOutputObj()
*/
static void SetJavaAudioOutputObj(CAMMSAudioOutputControl* aControl,jobject obj)
{
    aControl->SetJavaAudioOutputObject(obj);
}

/*
* Class:     Java_com_nokia_mj_impl_media_AudioOutputControl
* Method:    _getOutputMode
*
*/

JNIEXPORT jint JNICALL Java_com_nokia_mid_impl_media_AudioOutputControl__1getOutputMode(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aControl)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);
    CAMMSAudioOutputControl* control = static_cast< CAMMSAudioOutputControl* >(
                                           JavaUnhand< CAMMSControl >(aControl));
    __ASSERT_DEBUG(control, User::Invariant());
    return eventSource->Execute(GetPreference, control);
}

/*
* Class:     Java_com_nokia_mj_impl_media_AudioOutputControl
* Method:    _setOutputMode
*
*/
JNIEXPORT jint JNICALL Java_com_nokia_mid_impl_media_AudioOutputControl__1setOutputMode(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aControl,
    jint aPreference)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CAMMSAudioOutputControl* control = static_cast< CAMMSAudioOutputControl* >(
                                           JavaUnhand< CAMMSControl >(aControl));
    __ASSERT_DEBUG(control, User::Invariant());
    TInt error = eventSource->Execute(
                     &SetPreference, control, (TInt) aPreference);
    DEBUG_INT("AMMS::AudioOutputControl_JNI::setOutputMode =  %d", error);
    return error;
}


/*
* Class:     Java_com_nokia_mj_impl_media_AudioOutputControl
* Method:    _getCurrent
*
*/

JNIEXPORT jint JNICALL Java_com_nokia_mid_impl_media_AudioOutputControl__1getCurrent(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aControl)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CAMMSAudioOutputControl* control = static_cast< CAMMSAudioOutputControl* >(
                                           JavaUnhand< CAMMSControl >(aControl));
    __ASSERT_DEBUG(control, User::Invariant());
    TInt err = eventSource->ExecuteTrap(GetCurrent, control);
    DEBUG_INT("AMMS::AudioOutputControl_JNI::getCurrent =  %d", err);
    return err;
}


/*
* Class:     com_nokia_mj_impl_media_AudioOutputControl
* Method:    _setAudioOutputObject
* Signature: (IILjava/lang/Object;)I
*/
JNIEXPORT jint JNICALL Java_com_nokia_mid_impl_media_AudioOutputControl__1setAudioOutputObject
(JNIEnv *jni, jclass, jint aEventSource, jint aControl, jobject audioOutputObj)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);
    CAMMSAudioOutputControl* control = static_cast< CAMMSAudioOutputControl* >(
                                           JavaUnhand< CAMMSControl >(aControl));
    __ASSERT_DEBUG(control, User::Invariant());
    jobject obj = jni->NewWeakGlobalRef(audioOutputObj);
    TInt err = eventSource->ExecuteTrap(SetJavaAudioOutputObj,control ,obj);
    DEBUG_INT("AMMS::AudioOutputControl_JNI::setAudioOutputObject =  %d", err);
    return err;
}



//  End of File
