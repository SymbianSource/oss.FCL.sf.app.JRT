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
* Description:  This class has JNI wrappers for CMMAPlayer
*
*/


#include <jutils.h>
#include "com_nokia_microedition_media_PlayerImpl.h"

#include "cmmaeventsource.h"
#include "cmmaplayer.h"
#include <jdebug.h>

// If eventsource is already disposed, then do nothing
#define CHECK_HANDLE(x, j) { if ( !( x ) || ( ( x )->Players().Count() == 0 ) ) { return ( j ); } }
/*
 *  Wrappers for calling non static methods
 */

TInt VoidFunc(TInt aPlayer, CMMAEventSource* aEventSource,
              TInt(CMMAPlayer::*aFunc)())
{
    CMMAPlayer* p = aEventSource->FindPlayer(aPlayer);
    if (p)
    {
        return (p->*aFunc)();
    }
    else
    {
        return KErrBadHandle;
    }
}

void BoolFuncL(TInt aPlayer, CMMAEventSource* aEventSource,
               void(CMMAPlayer::*aFunc)(TBool),
               TBool aValue)
{
    CMMAPlayer* p = aEventSource->FindPlayer(aPlayer);
    if (p)
    {
        (p->*aFunc)(aValue);
    }
    // ignore call if player is not found
}

void VVoidFuncL(TInt aPlayer, CMMAEventSource* aEventSource, void(CMMAPlayer::*aFunc)())
{
    CMMAPlayer* p = aEventSource->FindPlayer(aPlayer);
    if (p)
    {
        (p->*aFunc)();
    }
    // ignore call if player is not found
}

void IntFunc(TInt aPlayer,
             CMMAEventSource* aEventSource,
             void(CMMAPlayer::*aFunc)(TInt),
             TInt aData)
{
    CMMAPlayer* p = aEventSource->FindPlayer(aPlayer);
    if (p)
    {
        (p->*aFunc)(aData);
    }
    // ignore call if player is not found
}

void TInt64Func(TInt aPlayer, CMMAEventSource* aEventSource,
                void(CMMAPlayer::*aFunc)(TInt64* aData),
                TInt64* aData)
{
    CMMAPlayer* p = aEventSource->FindPlayer(aPlayer);
    if (p)
    {
        (p->*aFunc)(aData);
    }
    else
    {
        *aData = KErrBadHandle;
    }
}



/*
 * Class:     com_nokia_microedition_media_PlayerImpl
 * Method:    _createPlayer
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_PlayerImpl__1initPlayer
(JNIEnv* aJni,
 jobject aObject,
 jobject aListenerObject,
 jint aEventSourceHandle,
 jint aPlayerHandle)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CHECK_HANDLE(eventSource, KErrNone);

    CMMAPlayer* player = JavaUnhand< CMMAPlayer >(aPlayerHandle);

    // init player
    jobject playerObject = aJni->NewWeakGlobalRef(aObject);
    TInt err = eventSource->ExecuteTrap(&CMMAPlayer::StaticInitPlayerL,
                                        player,
                                        playerObject,
                                        aJni);
    if (err != KErrNone)
    {
        aJni->DeleteWeakGlobalRef((jweak)playerObject);
        return err;
    }

    // set player listener
    jobject playerListener = aJni->NewWeakGlobalRef(aListenerObject);
    err = eventSource->ExecuteTrap(&CMMAPlayer::StaticSetPlayerListenerObjectL,
                                   player,
                                   playerListener,
                                   aJni,
                                   (MMMAEventPoster*)eventSource);
    if (err != KErrNone)
    {
        aJni->DeleteWeakGlobalRef((jweak)playerListener);
    }
    return err;
}

/*
 * Class:     com_nokia_microedition_media_PlayerImpl
 * Method:    _start
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_PlayerImpl__1start
(JNIEnv *, jclass, jint aEventSource, jint aPlayer)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CHECK_HANDLE(eventSource, KErrNone);



    TInt err = eventSource->ExecuteTrap(&BoolFuncL,
                                        aPlayer,
                                        eventSource,
                                        &CMMAPlayer::StartL,
                                        (TBool)ETrue);
    // complete java side request in case of leave.
    if (err != KErrNone)
    {
        CMMAPlayer* p = eventSource->FindPlayer(aPlayer);
        p->PostActionCompleted(err);   // java start return
    }

    return err;
}

/*
 * Class:     com_nokia_microedition_media_PlayerImpl
 * Method:    _stop
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_PlayerImpl__1stop
(JNIEnv *, jclass, jint aEventSource, jint aPlayer)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CHECK_HANDLE(eventSource, KErrNone);

    TInt err = eventSource->ExecuteTrap(&BoolFuncL,
                                        aPlayer,
                                        eventSource,
                                        &CMMAPlayer::StopL,
                                        (TBool)ETrue);
    return err;
}

/*
 * Class:     com_nokia_microedition_media_PlayerImpl
 * Method:    _close
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_PlayerImpl__1close
(JNIEnv *, jclass, jint aEventSource, jint aPlayer)
{
    // if eventsource is already disposed, then do nothing

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CHECK_HANDLE(eventSource, KErrNone);

    TInt err = eventSource->ExecuteTrap(&VVoidFuncL,
                                        aPlayer,
                                        eventSource,
                                        &CMMAPlayer::CloseL);
    return err;
}

/*
 * Class:     com_nokia_microedition_media_PlayerImpl
 * Method:    _prefetch
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_PlayerImpl__1prefetch
(JNIEnv *, jclass, jint aEventSource, jint aPlayer)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CHECK_HANDLE(eventSource, KErrNone);

    TInt err = eventSource->ExecuteTrap(&VVoidFuncL, aPlayer,
                                        eventSource,
                                        &CMMAPlayer::PrefetchL);
    return err;
}


/*
 * Class:     com_nokia_microedition_media_PlayerImpl
 * Method:    _realize
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_PlayerImpl__1realize
(JNIEnv *, jclass, jint aEventSource, jint aPlayer)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CHECK_HANDLE(eventSource, KErrNone);

    TInt err = eventSource->ExecuteTrap(&VVoidFuncL,
                                        aPlayer, eventSource,
                                        &CMMAPlayer::RealizeL);
    return err;
}


/*
 * Class:     com_nokia_microedition_media_PlayerImpl
 * Method:    _setLoopCount
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_PlayerImpl__1setLoopCount
(JNIEnv *, jclass, jint aEventSource, jint aPlayer, jint aLoopCount)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CHECK_HANDLE(eventSource, KErrNone);

    eventSource->ExecuteV(&IntFunc,
                          aPlayer, eventSource,
                          &CMMAPlayer::SetLoopCount,
                          aLoopCount);
    return KErrNone;
}

/*
 * Class:     com_nokia_microedition_media_PlayerImpl
 * Method:    _duration
 * Signature: (II)J
 */
JNIEXPORT jlong JNICALL Java_com_nokia_microedition_media_PlayerImpl__1duration
(JNIEnv *, jclass, jint aEventSource, jint aPlayer)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CHECK_HANDLE(eventSource, KErrNone);

    TInt64 duration(0);

    TInt err = eventSource->ExecuteTrap(&TInt64Func,
                                        aPlayer, eventSource,
                                        &CMMAPlayer::GetDuration,
                                        &duration);
    if (err != KErrNone)
    {
        return err;
    }
    return *reinterpret_cast< jlong* >(&duration);
}

/*
 * Class:     com_nokia_microedition_media_PlayerImpl
 * Method:    _setMediaTime
 * Signature: (IIJ)J
 */
JNIEXPORT jlong JNICALL Java_com_nokia_microedition_media_PlayerImpl__1setMediaTime
(JNIEnv *, jclass, jint aEventSource, jint aPlayer, jlong aNow)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CHECK_HANDLE(eventSource, KErrNone);


    TInt64 time = *reinterpret_cast< TInt64* >(&aNow);

    TInt err = eventSource->ExecuteTrap(&TInt64Func,
                                        aPlayer, eventSource,
                                        &CMMAPlayer::SetMediaTimeL,
                                        &time);

    if (err != KErrNone)
    {
        DEBUG_INT("MMA::Java_com_nokia_microedition_media_PlayerImpl__1setMediaTime error %d ",
                  err);
        return err;
    }
    return *reinterpret_cast< jlong* >(&time);
}

/*
 * Class:     com_nokia_microedition_media_PlayerImpl
 * Method:    _deallocate
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_PlayerImpl__1deallocate
(JNIEnv *, jclass, jint aEventSource, jint aPlayer)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CHECK_HANDLE(eventSource, KErrNone);

    TInt err = eventSource->ExecuteTrap(&VVoidFuncL,
                                        aPlayer, eventSource,
                                        &CMMAPlayer::DeallocateL);
    return err;
}

/*
 * Class:     com_nokia_microedition_media_PlayerImpl
 * Method:    _getMediaTime
 * Signature: (II)J
 */
JNIEXPORT jlong JNICALL Java_com_nokia_microedition_media_PlayerImpl__1getMediaTime
(JNIEnv*, jclass, jint aEventSource, jint aPlayer)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CHECK_HANDLE(eventSource, KErrNone);

    TInt64 mediaTime(0);

    TInt err = eventSource->ExecuteTrap(&TInt64Func, aPlayer,
                                        eventSource,
                                        &CMMAPlayer::GetMediaTime,
                                        &mediaTime);
    if (err != KErrNone)
    {
        return err;
    }
    return *reinterpret_cast< jlong* >(&mediaTime);
}

/*
 * Class:     com_nokia_microedition_media_PlayerImpl
 * Method:    _getState
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_PlayerImpl__1getState
(JNIEnv *, jclass, jint aEventSource, jint aPlayer)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CHECK_HANDLE(eventSource, KErrNone);

    CMMAPlayer* player = eventSource->FindPlayer(aPlayer);

    TInt state = CMMAPlayer::EClosed;
    if (player != NULL)
    {
        state = player->State();
    }

    return state;
}

/*
 *
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_PlayerImpl__1addSourceStream
(JNIEnv* aJni, jclass, jint aEventSource, jint aPlayer, jobject aReader)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CHECK_HANDLE(eventSource, KErrNone);

    CMMAPlayer* player = eventSource->FindPlayer(aPlayer);
    CMMASourceStream* sourceStream;

    jobject readerRef = aJni->NewWeakGlobalRef(aReader);

    TInt err = KErrNotFound;
    if (player != NULL)
    {
        err = eventSource->ExecuteTrap(&CMMAPlayer::StaticAddSourceStreamL,
                                       aJni,
                                       player,
                                       eventSource,
                                       readerRef,
                                       &sourceStream);
    }
    if (err != KErrNone)
    {
        aJni->DeleteWeakGlobalRef((jweak)readerRef);
        return err;
    }
    return JavaMakeHandle(sourceStream);
}


JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_PlayerImpl__1getControlsCount
(JNIEnv*, jclass, jint aEventSourceHandle, jint aPlayer)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CHECK_HANDLE(eventSource, KErrNone);


    TInt count = eventSource->Execute(&VoidFunc,
                                      aPlayer, eventSource,
                                      &CMMAPlayer::ControlCount);
    return count;
}


JNIEXPORT jstring JNICALL Java_com_nokia_microedition_media_PlayerImpl__1getControlClassName
(JNIEnv* aJniEnv, jclass, jint aEventSourceHandle, jint aControlHandle)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMAControl* control = JavaUnhand< CMMAControl >(aControlHandle);

    const TDesC* name = eventSource->Execute(CMMAControl::ClassNameJni,
                        control);

    return CreateJavaString(*aJniEnv, *name);
}


JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_PlayerImpl__1getControlHandle
(JNIEnv*, jclass, jint aEventSourceHandle, jint aPlayer, jint aIndex)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CHECK_HANDLE(eventSource, KErrNone);

    CMMAPlayer* player = eventSource->FindPlayer(aPlayer);
    CMMAControl* control = NULL;
    if (player)
    {
        control = eventSource->Execute(CMMAPlayer::StaticControl,
                                       player,
                                       aIndex);
    }
    else
    {
        return KErrBadHandle;
    }

    return JavaMakeHandle(control);
}

LOCAL_C void DisposePlayer(CMMAEventSource* aEventSource,
                           TInt aPlayer)
{
    CMMAPlayer* player = aEventSource->FindPlayer(aPlayer);
    if (player)
    {
        aEventSource->DisposePlayer(player);
    }
    // else already disposed
}

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_PlayerImpl__1dispose
(JNIEnv*, jclass, jint aEventSourceHandle, jint aPlayer)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    // if eventsource is already disposed, then do nothing
    if (!eventSource || eventSource->IsDisposed())
    {
        return;
    }

    eventSource->ExecuteV(&DisposePlayer,
                          eventSource,
                          aPlayer);
}

JNIEXPORT jstring JNICALL Java_com_nokia_microedition_media_PlayerImpl__1getContentType
(JNIEnv* aJni, jclass,
 jint aPlayerHandle)
{
    CMMAPlayer* player = JavaUnhand< CMMAPlayer >(aPlayerHandle);
    jstring contentType = NULL;

    // if content type is null, we just return NULL to Java
    if (player->ContentType())
    {
        // need to create Java String object
        contentType = CreateJavaString(*aJni, *player->ContentType());
    }
    return contentType;
}

//  END OF FILE
