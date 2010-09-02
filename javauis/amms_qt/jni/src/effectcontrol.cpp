/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  JNI for EffectControl
*
*/


//#include <jutils.h>
#include <logger.h>

#include "com_nokia_amms_control_EffectControl.h"
#include <mmafunctionserver.h>
#include "cammseffectcontrolgroup.h"
#include "jstringutils.h"
#include "s60commonutils.h"
using namespace java::util;

/**
 * wrapper for CAMMSEffectControlGroup::PresetNamesL()
 */
LOCAL_C void GetPresetNamesL(CAMMSEffectControlGroup* aControl,
                             JNIEnv* aJni,
                             jobjectArray* aArray)
{
    // JNI interface pointer can't be passed to different thread, so
    // it is needed to get valid JNI interface pointer for Event Server thread
    //aJni = JniEnvWrapper::GetValidJniRef();

    // Create new java String array and copy values from the values array
    *aArray = MMAPIUtils::CopyToNewJavaStringArrayL(*aJni, aControl->PresetNamesL());
}

/**
 * wrapper for CAMMSEffectControlGroup::PresetL()
 */
static void GetPresetL(
    CAMMSEffectControlGroup* aControl,
    TDes* aPreset)
{
    aControl->GetPresetL(*aPreset);
}

/**
 * wrapper for CAMMSEffectControlGroup::SetEnabledL()
 */
static void SetEnabledL(CAMMSEffectControlGroup* aControl, TBool aEnabled)
{
    aControl->SetEnabledL(aEnabled);
}

/**
 * wrapper for CAMMSEffectControlGroup::SetScopeL()
 */
static void SetScopeL(CAMMSEffectControlGroup* aControl, TInt aScope)
{
    aControl->SetScopeL((CAMMSEffectControlGroup::TEffectScope) aScope);
}

/**
 * wrapper for CAMMSEffectControlGroup::SetEnforcedL()
 */
static void SetEnforcedL(CAMMSEffectControlGroup* aControl, TBool aEnforced)
{
    aControl->SetEnforcedL(aEnforced);
}

/**
 * wrapper for CAMMSEffectControlGroup::SetPresetL()
 */
static void SetPresetL(CAMMSEffectControlGroup* aControl, TDesC* aPreset)
{
    aControl->SetPresetL(*aPreset);
}

/*
 * Class:     com_nokia_amms_control_EffectControl
 * Method:    _isEnabled
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_EffectControl__1isEnabled(
    JNIEnv*, jclass, jint, jint aControl)
{
    
    CAMMSEffectControlGroup* control = static_cast<CAMMSEffectControlGroup*>(
                                           reinterpret_cast< CAMMSControlGroup* >(aControl));

    return control->Enabled();
}

/*
 * Class:     com_nokia_amms_control_EffectControl
 * Method:    _isEnforced
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_EffectControl__1isEnforced(
    JNIEnv*, jclass, jint, jint aControl)
{
  
    CAMMSEffectControlGroup* control = static_cast<CAMMSEffectControlGroup*>(
                                           reinterpret_cast< CAMMSControlGroup* >(aControl));

    return control->Enforced();
}

/*
 * Class:     com_nokia_amms_control_EffectControl
 * Method:    _getPresetNames
 * Signature: (II[Ljava/lang/String;)I
 */
JNIEXPORT jobjectArray JNICALL Java_com_nokia_amms_control_EffectControl__1getPresetNames(
    JNIEnv* aJni,
    jclass,
    jint aEventSource,
    jint aControl)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSEffectControlGroup* control = static_cast<CAMMSEffectControlGroup*>(
                                           reinterpret_cast< CAMMSControlGroup* >(aControl));

    jobjectArray presetNames = NULL;

    TInt error = eventSource->ExecuteTrap(GetPresetNamesL,
                                          control,
                                          aJni,
                                          &presetNames);

    // If an error happened, return null to Java
    if (error != KErrNone)
    {
        presetNames = NULL;
    }
    return presetNames;
}

/*
 * Class:     com_nokia_amms_control_EffectControl
 * Method:    _getPreset
 * Signature: (II[Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_EffectControl__1getPreset(
    JNIEnv* aJni,
    jclass,
    jint aEventSource,
    jint aControl,
    jobjectArray aPreset)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSource);

    CAMMSEffectControlGroup* control = static_cast<CAMMSEffectControlGroup*>(
											reinterpret_cast< CAMMSControlGroup* >(aControl));

    TInt error;
    TBuf< KAMMSMaxPresetNameLength > preset;

    error = eventSource->ExecuteTrap(GetPresetL, control, (TDes*) &preset);

    if (error == KErrNone)
    {
        // return NULL if there is no preset set in the native class
        if (preset == KNullDesC)
        {
            aJni->SetObjectArrayElement(aPreset, 0, NULL);
        }
        else
        {
            jstring javaStr = S60CommonUtils::NativeToJavaString(*aJni, preset);
            if (!javaStr)
            {
                return KErrNoMemory;
            }

            aJni->SetObjectArrayElement(aPreset, 0, javaStr);
        }
    }

    return error;
}

/*
 * Class:     com_nokia_amms_control_EffectControl
 * Method:    _setScope
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_EffectControl__1setScope(
    JNIEnv*, jclass, jint aEventSource, jint aControl, jint aScope)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSource);

    CAMMSEffectControlGroup* control = static_cast<CAMMSEffectControlGroup*>(
                                           reinterpret_cast< CAMMSControlGroup *>(aControl));

    TInt error;
    error = eventSource->ExecuteTrap(SetScopeL, control, aScope);
    return error;
}

/*
 * Class:     com_nokia_amms_control_EffectControl
 * Method:    _setEnabled
 * Signature: (IIZ)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_EffectControl__1setEnabled(
    JNIEnv*, jclass, jint aEventSource, jint aControl, jboolean aEnabled)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSEffectControlGroup* control = static_cast<CAMMSEffectControlGroup*>(
                                           reinterpret_cast< CAMMSControlGroup* >(aControl));

    TInt error;
    error = eventSource->ExecuteTrap(SetEnabledL, control, (TBool) aEnabled);
    return error;
}

/*
 * Class:     com_nokia_amms_control_EffectControl
 * Method:    _getScope
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_EffectControl__1getScope(
    JNIEnv*, jclass, jint, jint aControl)
{
    CAMMSEffectControlGroup* control = static_cast<CAMMSEffectControlGroup*>(
                                           reinterpret_cast< CAMMSControlGroup* >(aControl));

    return control->Scope();
}

/*
 * Class:     com_nokia_amms_control_EffectControl
 * Method:    _setPreset
 * Signature: (IILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_EffectControl__1setPreset(
    JNIEnv* aJni, jclass, jint aEventSource, jint aControl, jstring aPreset)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSource);

    CAMMSEffectControlGroup* control = static_cast<CAMMSEffectControlGroup*>(
                                           reinterpret_cast< CAMMSControlGroup* >(aControl));

    TInt error;
    JStringUtils preset(*aJni, aPreset);
    error = eventSource->ExecuteTrap(SetPresetL, control, (TDesC*) &preset);

    ELOG1( EJavaAMMS, "AMMS:JNI:EffectControl:setPreset, err=%d", error);

    return error;
}

/*
 * Class:     com_nokia_amms_control_EffectControl
 * Method:    _setEnforced
 * Signature: (IIZ)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_EffectControl__1setEnforced(
    JNIEnv*, jclass, jint aEventSource, jint aControl, jboolean aEnforced)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSEffectControlGroup* control = static_cast<CAMMSEffectControlGroup*>(
                                           reinterpret_cast< CAMMSControlGroup *>(aControl));

    TInt error;
    error = eventSource->ExecuteTrap(
                SetEnforcedL,
                control,
                (TBool) aEnforced);
    return error;
}



//  End of File
