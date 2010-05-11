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
* Description:  TunerControl JNI wrapper.
 *
*/


// EXTERNAL INCLUDES
//#include <jutils.h>

// INTERNAL INCLUDES
#include "com_nokia_amms_control_tuner_TunerControlImpl.h"
#include <MMAFunctionServer.h>
#include "CAMMSTunerControl.h"
#include "jstringutils.h"
using namespace java::util;

/**
* Static delegator function for getMinFreq
*/
LOCAL_C void GetMinFreqL(CAMMSTunerControl* aNativeClass,
                         TInt *aMinFreq)
{

    *aMinFreq = aNativeClass->MinFreqL();
}

/**
*  getMinFreq JNI function
*/
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_tuner_TunerControlImpl__1getMinFreq(
    JNIEnv* /*aJni*/,
    jobject /*aJobject*/,
    jint aEventSourceHandle,
    jint aTunerControlHandle)
{
    TInt minFreq;

    MMAFunctionServer* eventSource =
        reinterpret_cast<MMAFunctionServer*>(aEventSourceHandle);

    CAMMSTunerControl* nativeHandle =
        reinterpret_cast<CAMMSTunerControl*>(aTunerControlHandle);

    TInt err = eventSource->ExecuteTrap(GetMinFreqL,
                                        nativeHandle,
                                        &minFreq);

    if (err != KErrNone)
    {
        return err;
    }

    return minFreq;
}


/**
* Static delegator function for getMaxFreq
*/
LOCAL_C void GetMaxFreqL(CAMMSTunerControl* aNativeClass,
                         TInt *aMaxFreq)
{

    *aMaxFreq = aNativeClass->MaxFreqL();
}

/**
*  getMaxFreq JNI function
*/
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_tuner_TunerControlImpl__1getMaxFreq(
    JNIEnv* /*aJni*/,
    jobject /*aJobject*/,
    jint aEventSourceHandle,
    jint aTunerControlHandle)
{
    TInt maxFreq;

    MMAFunctionServer* eventSource =
        reinterpret_cast<MMAFunctionServer*>(aEventSourceHandle);

    CAMMSTunerControl* nativeHandle =
        reinterpret_cast<CAMMSTunerControl*>(aTunerControlHandle);

    TInt err = eventSource->ExecuteTrap(GetMaxFreqL,
                                        nativeHandle,
                                        &maxFreq);

    if (err != KErrNone)
    {
        return err;
    }

    return maxFreq;
}


/**
* Static delegator function for setFrequency
*/
LOCAL_C void SetFreqL(CAMMSTunerControl* aNativeClass,
                      TInt aFreq)
{

    aNativeClass->SetFrequencyL(aFreq);
}

/**
* Static delegator function for getFrequency
*/
LOCAL_C void GetFreqL(CAMMSTunerControl* aNativeClass,
                      TInt *aFreq)
{
    *aFreq = aNativeClass->FrequencyL();
}

/**
*  setFrequency JNI function
*/
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_tuner_TunerControlImpl__1setFrequency(
    JNIEnv* /*aJni*/,
    jobject /*aJobject*/,
    jint aEventSourceHandle,
    jint aTunerControlHandle,
    jint aFreq)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast<MMAFunctionServer*>(aEventSourceHandle);

    CAMMSTunerControl* nativeHandle =
        reinterpret_cast<CAMMSTunerControl*>(aTunerControlHandle);

    TInt err = eventSource->ExecuteTrap(SetFreqL,
                                        nativeHandle,
                                        aFreq);

    if (err != KErrNone)
    {
        return err;
    }

    //get tuned frequency
    TInt tunedFreq;
    err = eventSource->ExecuteTrap(GetFreqL,
                                   nativeHandle,
                                   &tunedFreq);
    if (err != KErrNone)
    {
        return err;
    }

    return tunedFreq;
}

/**
*  getFrequency JNI function
*/
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_tuner_TunerControlImpl__1getFrequency(
    JNIEnv* /*aJni*/,
    jobject /*aJobject*/,
    jint aEventSourceHandle,
    jint aTunerControlHandle)
{
    TInt freq;

    MMAFunctionServer* eventSource =
        reinterpret_cast<MMAFunctionServer*>(aEventSourceHandle);

    CAMMSTunerControl* nativeHandle =
        reinterpret_cast<CAMMSTunerControl*>(aTunerControlHandle);

    TInt err = eventSource->ExecuteTrap(GetFreqL,
                                        nativeHandle,
                                        &freq);
    if (err != KErrNone)
    {
        return err;
    }

    return freq;
}


/**
* Static delegator function for seek
*/
LOCAL_C void SeekL(CAMMSTunerControl* aNativeClass,
                   TInt aStartFreq,
                   TBool aUpwards,
                   TInt* aSeekedFreq)
{

    *aSeekedFreq = aNativeClass->SeekL(aStartFreq, aUpwards);
}

/**
*  seek JNI function
*/
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_tuner_TunerControlImpl__1seek(
    JNIEnv* /*aJni*/,
    jobject /*aJobject*/,
    jint aEventSourceHandle,
    jint aTunerControlHandle,
    jint aStartFreq,
    jboolean aUpwards)
{
    TInt seekedFreq;

    MMAFunctionServer* eventSource =
        reinterpret_cast<MMAFunctionServer*>(aEventSourceHandle);

    CAMMSTunerControl* nativeHandle =
        reinterpret_cast<CAMMSTunerControl*>(aTunerControlHandle);

    TInt err = eventSource->ExecuteTrap(SeekL,
                                        nativeHandle,
                                        aStartFreq,
                                        (TBool) aUpwards,
                                        &seekedFreq);

    if (err != KErrNone)
    {
        return err;
    }

    return seekedFreq;
}


/**
* Static delegator function for getSquelch
*/
LOCAL_C void GetSquelchL(CAMMSTunerControl* aNativeClass,
                         TBool* aSquelch)
{

    *aSquelch = aNativeClass->SquelchL();
}

/**
*  getSquelch JNI function
*/
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_tuner_TunerControlImpl__1getSquelch(
    JNIEnv* /*aJni*/,
    jobject /*aJobject*/,
    jint aEventSourceHandle,
    jint aTunerControlHandle)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast<MMAFunctionServer*>(aEventSourceHandle);

    CAMMSTunerControl* nativeHandle =
        reinterpret_cast<CAMMSTunerControl*>(aTunerControlHandle);

    TBool squelch = EFalse;

    TInt err = eventSource->ExecuteTrap(GetSquelchL,
                                        nativeHandle,
                                        &squelch);

    if (err != KErrNone)
    {
        return err;
    }

    if (!squelch)
    {
        //return 1 if false
        return 1;
    }
    //return 0 if true
    return KErrNone;
}


/**
* Static delegator function for setSquelch
*/
LOCAL_C void SetSquelchL(CAMMSTunerControl* aNativeClass,
                         TBool aSquelch)
{

    aNativeClass->SetSquelchL(aSquelch);
}

/**
*  setSquelch JNI function
*/
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_tuner_TunerControlImpl__1setSquelch(
    JNIEnv* /*aJni*/,
    jobject /*aJobject*/,
    jint aEventSourceHandle,
    jint aTunerControlHandle,
    jboolean aSquelch)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast<MMAFunctionServer*>(aEventSourceHandle);

    CAMMSTunerControl* nativeHandle =
        reinterpret_cast<CAMMSTunerControl*>(aTunerControlHandle);

    TInt err = eventSource->ExecuteTrap(SetSquelchL,
                                        nativeHandle,
                                        (TBool) aSquelch);

    if (err != KErrNone)
    {
        return err;
    }

    return KErrNone;
}


/**
* Static delegator function for getSignalStrength
*/
LOCAL_C void GetSignalStrengthL(CAMMSTunerControl* aNativeClass,
                                TInt *aSignalStrength)
{

    *aSignalStrength = aNativeClass->SignalStrengthL();
}

/**
*  getSignalStrength JNI function
*/
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_tuner_TunerControlImpl__1getSignalStrength(
    JNIEnv* /*aJni*/,
    jobject /*aJobject*/,
    jint aEventSourceHandle,
    jint aTunerControlHandle)
{
    TInt signalStrength;

    MMAFunctionServer* eventSource =
        reinterpret_cast<MMAFunctionServer*>(aEventSourceHandle);

    CAMMSTunerControl* nativeHandle =
        reinterpret_cast<CAMMSTunerControl*>(aTunerControlHandle);

    TInt err = eventSource->ExecuteTrap(GetSignalStrengthL,
                                        nativeHandle,
                                        &signalStrength);
    if (err != KErrNone)
    {
        return err;
    }

    return signalStrength;
}


/**
* Static delegator function for getStereoMode
*/
LOCAL_C void GetStereoModeL(CAMMSTunerControl* aNativeClass,
                            TInt *aStereoMode)
{

    *aStereoMode = aNativeClass->StereoModeL();
}

/**
*  getStereoMode JNI function
*/
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_tuner_TunerControlImpl__1getStereoMode(
    JNIEnv* /*aJni*/,
    jobject /*aJobject*/,
    jint aEventSourceHandle,
    jint aTunerControlHandle)
{
    TInt stereoMode;

    MMAFunctionServer* eventSource =
        reinterpret_cast<MMAFunctionServer*>(aEventSourceHandle);

    CAMMSTunerControl* nativeHandle =
        reinterpret_cast<CAMMSTunerControl*>(aTunerControlHandle);

    TInt err = eventSource->ExecuteTrap(GetStereoModeL,
                                        nativeHandle,
                                        &stereoMode);
    if (err != KErrNone)
    {
        return err;
    }

    return stereoMode;
}


/**
* Static delegator function for setStereoMode
*/
LOCAL_C void SetStereoModeL(CAMMSTunerControl* aNativeClass,
                            TInt aStereoMode)
{
    aNativeClass->SetStereoModeL(aStereoMode);
}

/**
*  setStereoMode JNI function
*/
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_tuner_TunerControlImpl__1setStereoMode(
    JNIEnv* /*aJni*/,
    jobject /*aJobject*/,
    jint aEventSourceHandle,
    jint aTunerControlHandle,
    jint aStereoMode)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast<MMAFunctionServer*>(aEventSourceHandle);

    CAMMSTunerControl* nativeHandle =
        reinterpret_cast<CAMMSTunerControl*>(aTunerControlHandle);

    TInt err = eventSource->ExecuteTrap(SetStereoModeL,
                                        nativeHandle,
                                        aStereoMode);

    return err;
}


/**
* Static delegator function for usePreset
*/
LOCAL_C void UsePresetL(CAMMSTunerControl* aNativeClass,
                        TInt aPreset)
{
    aNativeClass->UsePresetL(aPreset);
}

/**
*  usePreset JNI function
*/
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_tuner_TunerControlImpl__1usePreset(
    JNIEnv* /*aJni*/,
    jobject /*aJobject*/,
    jint aEventSourceHandle,
    jint aTunerControlHandle,
    jint aPreset)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast<MMAFunctionServer*>(aEventSourceHandle);

    CAMMSTunerControl* nativeHandle =
        reinterpret_cast<CAMMSTunerControl*>(aTunerControlHandle);

    TInt err = eventSource->ExecuteTrap(UsePresetL,
                                        nativeHandle,
                                        aPreset);

    return err;
}


/**
* Static delegator function for setPreset__III
*/
LOCAL_C void SetPresetL(CAMMSTunerControl* aNativeClass,
                        TInt aPreset)
{
    aNativeClass->SetPresetL(aPreset);
}

/**
*  setPreset__III JNI function
*/
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_tuner_TunerControlImpl__1setPreset__III(
    JNIEnv* /*aJni*/,
    jobject /*aJobject*/,
    jint aEventSourceHandle,
    jint aTunerControlHandle,
    jint aPreset)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast<MMAFunctionServer*>(aEventSourceHandle);

    CAMMSTunerControl* nativeHandle =
        reinterpret_cast<CAMMSTunerControl*>(aTunerControlHandle);

    TInt err = eventSource->ExecuteTrap(SetPresetL,
                                        nativeHandle,
                                        aPreset);

    return err;
}


/**
* Static delegator function for setPreset__IIIII
*/
LOCAL_C void SetPresetL(CAMMSTunerControl* aNativeClass,
                        TInt aPreset,
                        TInt aFrequency,
                        TInt aStereoMode)
{
    aNativeClass->SetPresetL(aPreset, aFrequency, aStereoMode);
}

/**
*  setPreset__IIIII JNI function
*/
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_tuner_TunerControlImpl__1setPreset__IIIII(
    JNIEnv* /*aJni*/,
    jobject /*aJobject*/,
    jint aEventSourceHandle,
    jint aTunerControlHandle,
    jint aPreset,
    jint aFrequency,
    jint aStereoMode)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast<MMAFunctionServer*>(aEventSourceHandle);

    CAMMSTunerControl* nativeHandle =
        reinterpret_cast<CAMMSTunerControl*>(aTunerControlHandle);

    TInt err = eventSource->ExecuteTrap(SetPresetL,
                                        nativeHandle,
                                        aPreset,
                                        aFrequency,
                                        aStereoMode);

    return err;
}


/**
* Static delegator function for getPresetFrequency
*/
LOCAL_C void GetPresetFreqL(CAMMSTunerControl* aNativeClass,
                            TInt aPreset,
                            TInt *aPresetFreq)
{

    *aPresetFreq = aNativeClass->PresetFrequencyL(aPreset);
}

/**
*  getPresetFrequency JNI function
*/
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_tuner_TunerControlImpl__1getPresetFrequency(
    JNIEnv* /*aJni*/,
    jobject /*aJobject*/,
    jint aEventSourceHandle,
    jint aTunerControlHandle,
    jint aPreset)
{
    TInt presetFreq;

    MMAFunctionServer* eventSource =
        reinterpret_cast<MMAFunctionServer*>(aEventSourceHandle);

    CAMMSTunerControl* nativeHandle =
        reinterpret_cast<CAMMSTunerControl*>(aTunerControlHandle);

    TInt err = eventSource->ExecuteTrap(GetPresetFreqL,
                                        nativeHandle,
                                        aPreset,
                                        &presetFreq);
    if (err != KErrNone)
    {
        return err;
    }

    return presetFreq;
}


/**
* Static delegator function for getPresetStereoMode
*/
LOCAL_C void GetPresetStereoModeL(CAMMSTunerControl* aNativeClass,
                                  TInt aPreset,
                                  TInt *aPresetStereoMode)
{

    *aPresetStereoMode = aNativeClass->PresetStereoModeL(aPreset);
}

/**
*  getPresetStereoMode JNI function
*/
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_tuner_TunerControlImpl__1getPresetStereoMode(
    JNIEnv* /*aJni*/,
    jobject /*aJobject*/,
    jint aEventSourceHandle,
    jint aTunerControlHandle,
    jint aPreset)
{
    TInt presetStereoMode;

    MMAFunctionServer* eventSource =
        reinterpret_cast<MMAFunctionServer*>(aEventSourceHandle);

    CAMMSTunerControl* nativeHandle =
        reinterpret_cast<CAMMSTunerControl*>(aTunerControlHandle);

    TInt err = eventSource->ExecuteTrap(GetPresetStereoModeL,
                                        nativeHandle,
                                        aPreset,
                                        &presetStereoMode);
    if (err != KErrNone)
    {
        return err;
    }

    return presetStereoMode;
}


/**
* Static delegator function for setPresetName
*/
LOCAL_C void SetPresetNameL(CAMMSTunerControl* aNativeClass,
                            TInt aPreset,
                            const TDesC* aPresetName)
{
    aNativeClass->SetPresetNameL(aPreset, aPresetName);
}

/**
*  setPresetName JNI function
*/
JNIEXPORT jint JNICALL
Java_com_nokia_amms_control_tuner_TunerControlImpl__1setPresetName(
    JNIEnv* aJni,
    jobject /*aJobject*/,
    jint aEventSourceHandle,
    jint aTunerControlHandle,
    jint aPreset,
    jstring aPresetName)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast<MMAFunctionServer*>(aEventSourceHandle);

    CAMMSTunerControl* nativeHandle =
        reinterpret_cast<CAMMSTunerControl*>(aTunerControlHandle);

    JStringUtils presetName(*aJni, aPresetName);

    TInt err = eventSource->ExecuteTrap(SetPresetNameL,
                                        nativeHandle,
                                        aPreset,
                                        (const TDesC*)&presetName);

    return err;
}


/**
* Static delegator function for getPresetName
*/
LOCAL_C void GetPresetNameL(CAMMSTunerControl* aNativeClass,
                            TInt aPreset,
                            TPtrC* aPresetName)
{
    aNativeClass->GetPresetNameL(aPreset, aPresetName);
}

/**
*  getPresetName JNI function
*/
JNIEXPORT jstring JNICALL
Java_com_nokia_amms_control_tuner_TunerControlImpl__1getPresetName(
    JNIEnv* aJni,
    jobject /*aJobject*/,
    jint aEventSourceHandle,
    jint aTunerControlHandle,
    jint aPreset,
    jintArray aError)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast<MMAFunctionServer*>(aEventSourceHandle);

    CAMMSTunerControl* nativeHandle =
        reinterpret_cast<CAMMSTunerControl*>(aTunerControlHandle);

    TPtrC presetNamePtr(NULL, 0);
    jstring presetName = NULL;

    TInt err = eventSource->ExecuteTrap(GetPresetNameL,
                                        nativeHandle,
                                        aPreset,
                                        &presetNamePtr);

    if (err == KErrNone)
    {
        presetName = S60CommonUtils::NativeToJavaString(*aJni, presetNamePtr);
        if (!presetName)
        {
            err = KErrNotSupported;
        }
    }

    jint javaErr[ 1 ] = { err };
    aJni->SetIntArrayRegion(aError, 0, 1, javaErr);

    return presetName;
}

// End of file
