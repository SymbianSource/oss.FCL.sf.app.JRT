/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  JNI class for Sound.java.
*
*/


#include <e32def.h> // MAKE_TINT64 Warning fix

#include "com_nokia_mid_sound_Sound.h"
#include "CMIDSound.h"
#include "javajniutils.h"
#include "javacommonutils.h"
#include "logger.h"

/*
 * Class:     com_nokia_mid_sound_Sound
 * Method:    _dispose
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mid_sound_Sound__1dispose
(JNIEnv* /* aJni */, jobject /* aSound */, jint aHandle)
{
    JELOG2(EJavaUI);
    CMIDSound* sound = reinterpret_cast<CMIDSound*>(aHandle);
    delete sound;
}

/*
 * Class:     com_nokia_mid_sound_Sound
 * Method:    _create
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_sound_Sound__1create
(JNIEnv* aJni, jobject aSound)
{
    JELOG2(EJavaUI);

    TInt handle(0);
    CMIDSound* sound = 0;
    TRAPD(err,
    {
        sound = CMIDSound::NewL(*aJni, aSound);
        if (sound != 0)
        {
            sound->InitProfileListener();
        }
    });

    if (err != KErrNone)
    {

        java::util::JniUtils::throwNewException(aJni, "java/lang/RuntimeException" ,
                                                "Failed to create Native Peer " +
                                                java::util::JavaCommonUtils::intToString(err));
    }
    else
    {

        handle = reinterpret_cast<TInt>(sound);
    }
    return handle;
}

/*
 * Class:     com_nokia_mid_sound_Sound
 * Method:    _init
 * Signature: (III[BIJ)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_sound_Sound__1init
(JNIEnv* aJni, jobject, jint aHandle, jint aType, jbyteArray aData,
 jint aFrequency, jlong aDuration)
{
    JELOG2(EJavaUI);
    TInt freq(aFrequency);
    TInt64 duration = *reinterpret_cast<TInt64*>(&aDuration);

    CMIDSound* sound = reinterpret_cast<CMIDSound*>(aHandle);

    TPtrC8 ptr;
    jbyte* data = NULL;

    if (aType != com_nokia_mid_sound_Sound_FORMAT_BEEP)
    {
        if (!aData)
        {
            return KErrGeneral;
        }
        data = aJni->GetByteArrayElements(aData,0);
        jint length = aJni->GetArrayLength(aData);
        ptr.Set((TUint8*)data, length);
    }

    // Had to make SetBeep() due to number of maximum parameters in ExecuteTrap.
    sound->SetBeep(freq, duration);
    TInt err = sound->Init(aType, (const TDesC8*)&ptr);

    if (aType != com_nokia_mid_sound_Sound_FORMAT_BEEP)
    {
        aJni->ReleaseByteArrayElements(aData, data, 0);
    }
    return err;
}

/*
 * Class:     com_nokia_mid_sound_Sound
 * Method:    _release
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mid_sound_Sound__1release
(JNIEnv*, jobject, jint aHandle)
{
    JELOG2(EJavaUI);
    CMIDSound* sound = reinterpret_cast<CMIDSound*>(aHandle);
    sound->Release();
}


/*
 * Class:     com_nokia_mid_sound_Sound
 * Method:    _play
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_sound_Sound__1play
(JNIEnv*, jobject, jint aHandle, jint aLoop)
{
    JELOG2(EJavaUI);
    CMIDSound* sound = reinterpret_cast<CMIDSound*>(aHandle);
    TInt err = sound->Play(aLoop);
    return err;
}

/*
 * Class:     com_nokia_mid_sound_Sound
 * Method:    _stop
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mid_sound_Sound__1stop
(JNIEnv*, jobject, jint aHandle)
{
    JELOG2(EJavaUI);
    CMIDSound* sound = reinterpret_cast<CMIDSound*>(aHandle);
    sound->Stop();
}

/*
 * Class:     com_nokia_mid_sound_Sound
 * Method:    _resume
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mid_sound_Sound__1resume
(JNIEnv*, jobject, jint aHandle)
{
    JELOG2(EJavaUI);
    CMIDSound* sound = reinterpret_cast<CMIDSound*>(aHandle);
    sound->Resume();
}

/*
 * Class:     com_nokia_mid_sound_Sound
 * Method:    _setVolume
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mid_sound_Sound__1setVolume
(JNIEnv*, jobject, jint aHandle, jint aVolume)
{
    JELOG2(EJavaUI);
    CMIDSound* sound = reinterpret_cast<CMIDSound*>(aHandle);
    sound->SetVolume(aVolume);
}

/*
 * Class:     com_nokia_mid_sound_Sound
 * Method:    _volume
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_sound_Sound__1volume
(JNIEnv*, jobject, jint aHandle)
{
    JELOG2(EJavaUI);
    CMIDSound* sound = reinterpret_cast<CMIDSound*>(aHandle);
    TInt volume = sound->SoundVolume();
    return(volume);
}

/*
 * Class:     com_nokia_mid_sound_Sound
 * Method:    _getState
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_sound_Sound__1getState
(JNIEnv*, jobject, jint aHandle)
{
    JELOG2(EJavaUI);
    CMIDSound* sound = reinterpret_cast<CMIDSound*>(aHandle);
    TInt state = sound->PlayerState();
    return state;
}
