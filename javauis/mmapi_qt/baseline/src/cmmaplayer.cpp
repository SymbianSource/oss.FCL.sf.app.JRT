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
* Description:  This class is base class for all players.
*
*/

//  INCLUDE FILES
#include <logger.h>

#include <JniEnvWrapper.h>

#include "cmmaplayer.h"
#include "mmafunctionserver.h"
#include "cmmadurationupdater.h"
#include "cmmavolumecontrol.h"

// CONSTANTS
_LIT(KPanicOutOfMem, "out of memory");

#ifdef _DEBUG // Needed only in ASSER_DEBUG statements.
_LIT(KPanicMethod, "method not found");
#endif

CMMAPlayer::~CMMAPlayer()
{
    LOG(EJavaMMAPI, EInfo, "CMMAPlayer::~CMMAPlayer()");

    iSourceStreams.ResetAndDestroy();
    if (iControls.Count() > 0)
        iControls.ResetAndDestroy();
    iStateListeners.Reset();
    delete iActionCompletedEvent;
    delete iOOMErrorEvent;
    delete iDurationUpdater;
    delete iContentType;
}


CMMAPlayer::CMMAPlayer():
        iRepeatNumberOfTimes(0),
        iRepeatForever(EFalse),
        iRepeatCount(0),
        iDuration(KTimeUnknown),
        iState(EUnrealized)
{
}


void CMMAPlayer::ConstructL()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::ConstructL + ");
    iDurationUpdater = CMMADurationUpdater::NewL(*this);
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::ConstructL - ");
}


void CMMAPlayer::StaticAddSourceStreamL(JNIEnv* aJniEnv,
                                        CMMAPlayer* aPlayer,
                                        MMAFunctionServer* aEventSource,
                                        jobject aReader,
                                        CMMASourceStream** aSourceStream)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::StaticAddSourceStreamL +");

    // JNI interface pointer can't be passed to different thread, so
    // it is needed to get valid JNI interface pointer for Event Server thread
    aJniEnv = aEventSource-> getValidJniEnv();

    *aSourceStream = aPlayer->AddSourceStreamL(aJniEnv,
                     aEventSource,
                     aReader);
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::StaticAddSourceStreamL -");
}



void CMMAPlayer::StaticSetPlayerListenerObjectL(CMMAPlayer* aPlayer,
        MMAFunctionServer* aEventsource,
        jobject aListenerObject,
        JNIEnv* aJni,
        MMMAEventPoster* aPoster)
{

    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::StaticSetPlayerListenerObjectL +");

    // JNI interface pointer can't be passed to different thread, so
    // it is needed to get valid JNI interface pointer for Event Server thread
    aJni = aEventsource-> getValidJniEnv();

    aPlayer->SetPlayerListenerObjectL(aListenerObject, aJni, aPoster);

    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::StaticSetPlayerListenerObjectL -");
}


void CMMAPlayer::StaticInitPlayerL(CMMAPlayer* aPlayer,
                                   MMAFunctionServer* aEventsource,
                                   jobject aPlayerObject,
                                   JNIEnv* aJni)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::StaticInitPlayerL +");

    // JNI interface pointer can't be passed to different thread, so
    // it is needed to get valid JNI interface pointer for Event Server thread
    aJni = aEventsource-> getValidJniEnv();

    jmethodID actionCompletedMethod = aJni->GetMethodID(
                                          aJni->GetObjectClass(aPlayerObject),
                                          "actionCompleted",
                                          "(I)V");

    jmethodID actionCompletedFileMethod = aJni->GetMethodID(
                                              aJni->GetObjectClass(aPlayerObject),
                                              "actionCompletedFile",
                                              "()V");

    jmethodID actionCompletedStartMethod = aJni->GetMethodID(
                                               aJni->GetObjectClass(aPlayerObject),
                                               "actionCompletedStart",
                                               "()V");

    // Sanity Check, something is really wrong if methods cannot be found
    __ASSERT_DEBUG(actionCompletedMethod,
                   User::Panic(KPanicMethod, KErrGeneral));

    aPlayer->iActionCompletedEvent = new(ELeave) CMMAEvent(
        aPlayerObject,
        actionCompletedMethod,
        CMMAEvent::EReusableEvent);

    aPlayer->iActionCompletedFileEvent = new(ELeave) CMMAEvent(
        aPlayerObject,
        actionCompletedFileMethod,
        CMMAEvent::EReusableEvent);

    aPlayer->iActionCompletedStartEvent = new(ELeave) CMMAEvent(
        aPlayerObject,
        actionCompletedStartMethod,
        CMMAEvent::EReusableEvent);

    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::StaticInitPlayerL -");
}

CMMAControl* CMMAPlayer::StaticControl(CMMAPlayer* aPlayer, TInt aIndex)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::StaticControl +");
    return aPlayer->Control(aIndex);

}


void CMMAPlayer::RealizeL()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::RealizeL +");
    ChangeState(ERealized);

}


void CMMAPlayer::CloseL()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::CloseL ");
    ChangeState(EClosed);
    PostObjectEvent(CMMAPlayerEvent::EClosed, NULL);
}


void CMMAPlayer::GetDuration(TInt64* aDuration)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::GetDuration ");
    *aDuration = iDuration;
}


void CMMAPlayer::SetMediaTimeL(TInt64* aTime)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::SetMediaTimeL ");
    *aTime = KErrNotSupported;
}

void CMMAPlayer::GetMediaTime(TInt64* aMediaTime)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::GetMediaTime ");
    *aMediaTime = KErrNotSupported;
}


EXPORT_C void CMMAPlayer::SetLoopCount(TInt aCount)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::SetLoopCount ");
    iRepeatForever = (aCount == KJavaRepeatForever);
    iRepeatNumberOfTimes = aCount;
    iRepeatCount = 0;
}

HBufC* CMMAPlayer::ContentType() const
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::ContentType ");
    return iContentType;
}

void CMMAPlayer::SetPlayerListenerObjectL(jobject aListenerObject,
        JNIEnv* aJni,
        MMMAEventPoster* aEventPoster)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::SetPlayerListenerObjectL +");
    iListenerObject = aListenerObject;
    iEventPoster = aEventPoster;

    jclass listenerClass = aJni->GetObjectClass(aListenerObject);

    iPostEvent = aJni->GetMethodID(
                     listenerClass,
                     "postEvent",
                     "(Ljava/lang/String;Ljava/lang/Object;)V");

    iPostObjectEvent = aJni->GetMethodID(listenerClass,
                                         "postObjectEvent",
                                         "(ILjava/lang/Object;)V");

    iPostLongEvent = aJni->GetMethodID(listenerClass,
                                       "postLongEvent",
                                       "(IJ)V");

    iPostStringEvent = aJni->GetMethodID(listenerClass,
                                         "postStringEvent",
                                         "(ILjava/lang/String;)V");

    iPostControlEvent = aJni->GetMethodID(listenerClass,
                                          "postControlEvent",
                                          "(ILjava/lang/String;)V");


    // Sanity Check, something is really wrong if methods cannot be found
    __ASSERT_DEBUG(iPostEvent &&
                   iPostObjectEvent &&
                   iPostLongEvent &&
                   iPostStringEvent &&
                   iPostControlEvent,
                   User::Panic(KPanicMethod, KErrGeneral));

    iOOMErrorEvent = new(ELeave) CMMAPlayerEvent(iListenerObject,
            iPostStringEvent,
            CMMAEvent::EReusableEvent);
    iOOMErrorEvent->SetStringEventL(CMMAPlayerEvent::EError,
                                    KPanicOutOfMem);

    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::SetPlayerListenerObjectL -");
}


EXPORT_C TInt CMMAPlayer::ControlCount()
{
    return iControls.Count();
}


EXPORT_C CMMAControl* CMMAPlayer::Control(TInt aIndex)
{
    return iControls[ aIndex ];
}

void CMMAPlayer::RefreshControls()
{

    for (TInt index = 0; index < iControls.Count(); index++)
    {
        CMMAControl* control = iControls[ index ];
        control->RefreshControl();
    }

}

EXPORT_C  void CMMAPlayer::AddStateListenerL(MMMAPlayerStateListener* aListener)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::AddStateListenerL +");
    TInt err = iStateListeners.Append(aListener);
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::AddStateListenerL - err = %d ",err);
    User::LeaveIfError(err);
    //User::LeaveIfError(iStateListeners.Append(aListener));

}

EXPORT_C void CMMAPlayer::RemoveStateListener(MMMAPlayerStateListener* aListener)
{

    TInt index = iStateListeners.Find(aListener);

    if (index != KErrNotFound)
    {
        iStateListeners.Remove(index);
    }

}

const TDesC& CMMAPlayer::Type()
{
    // by default player has not the type
    return KNullDesC;
}

void CMMAPlayer::SetContentType(HBufC* aContentType)
{

    delete iContentType;
    iContentType = aContentType;

}

void CMMAPlayer::ResetSourceStreams()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::ResetSourceStreams +");
    int sourceStreamsCount = iSourceStreams.Count();
    for (int i = 0; i < sourceStreamsCount; i++)
    {
        iSourceStreams[ i ]->ResetData();
    }
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::ResetSourceStreams -");
}

EXPORT_C void CMMAPlayer::AddControlL(CMMAControl* aControl)
{

    User::LeaveIfError(iControls.Append(aControl));

}

void CMMAPlayer::PostLongEvent(CMMAPlayerEvent::TEventType aEventType,
                               const TInt64& aLongEventData)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::PostLongEvent ");
    if (!iListenerObject || !iEventPoster)
    {
        LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::PostLongEvent No listener");
        // return since player is not ready for posting any events and is not initialized
        return;
    }

    CMMAPlayerEvent* playerEvent = new CMMAPlayerEvent(iListenerObject,
            iPostLongEvent);
    if (!playerEvent)
    {
        // creation of player event failed, informing Java
        iEventPoster->PostEvent(iOOMErrorEvent);
        return;
    }
    playerEvent->SetLongEvent(aEventType, aLongEventData);

    // event poster takes ownership of event
    iEventPoster->PostEvent(playerEvent);
}


EXPORT_C void CMMAPlayer::PostStringEvent(CMMAPlayerEvent::TEventType aEventType,
        const TDesC& aStringEventData)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::PostStringEvent ");
    if (!iListenerObject || !iEventPoster)
    {
        LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::PostStringEvent No listener");
        // return since player is not ready for posting any events and is not initialized
        return;
    }

    CMMAPlayerEvent* playerEvent = new CMMAPlayerEvent(iListenerObject,
            iPostStringEvent);
    if (!playerEvent)
    {
        // creation of player event failed, informing Java
        iEventPoster->PostEvent(iOOMErrorEvent);
        return;
    }

    TRAPD(err, playerEvent->SetStringEventL(aEventType, aStringEventData));
    if (err != KErrNone)
    {
        // string setting failed, informing Java
        iEventPoster->PostEvent(iOOMErrorEvent);
        return;
    }

    // event poster takes ownership of event
    iEventPoster->PostEvent(playerEvent);
}


EXPORT_C void CMMAPlayer::PostObjectEvent(CMMAPlayerEvent::TEventType aEventType,
        const jobject aEventData)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::PostObjectEvent ");

    if (!iListenerObject || !iEventPoster)
    {
        LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::PostObjectEvent No listener");
        // return since player is not ready for posting any events and is not initialized
        return;
    }

    CMMAPlayerEvent* playerEvent = new CMMAPlayerEvent(iListenerObject,
            iPostObjectEvent);

    if (!playerEvent)
    {
        // creation of player event failed, informing Java
        iEventPoster->PostEvent(iOOMErrorEvent);
        return;
    }

    TRAPD(err, playerEvent->SetObjectEventL(aEventType, aEventData));
    if (err != KErrNone)
    {
        // creation of player event failed, informing Java
        iEventPoster->PostEvent(iOOMErrorEvent);
        return;
    }

    // event poster takes ownership of event
    iEventPoster->PostEvent(playerEvent);
}

EXPORT_C CMMASourceStream* CMMAPlayer::AddSourceStreamL(JNIEnv* aJNIEnv,
        MMAFunctionServer* aEventSource,
        jobject aReader)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::AddSourceStreamL ");
    CMMASourceStream* sourceStream = CMMASourceStream::NewL(aJNIEnv,
                                     aEventSource,
                                     aReader,
                                     this);

    CleanupStack::PushL(sourceStream);
    User::LeaveIfError(iSourceStreams.Append(sourceStream));
    CleanupStack::Pop(); // sourceStream
    return sourceStream;
}


void CMMAPlayer::PostActionCompleted(TInt aError)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::PostActionCompleted ");
    iActionCompletedEvent->SetEventData(aError);
    iEventPoster->PostEvent(iActionCompletedEvent,
                            CMMAEvent::ENotifyPriority);
}

void CMMAPlayer::PostActionCompletedFile()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::PostActionCompletedFile ");
    // iActionCompletedFileEvent->SetEventData(aError);
    iEventPoster->PostEvent(iActionCompletedFileEvent,
                            CMMAEvent::ENotifyPriority);
}


void CMMAPlayer::PostActionCompletedStart()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::PostActionCompletedStart ");
    iEventPoster->PostEvent(iActionCompletedStartEvent,
                            CMMAEvent::ENotifyPriority);
}

void CMMAPlayer::ChangeState(TPlayerState aState)
{
    iState = aState;
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::ChangeState - iStateListeners count is %d", iStateListeners.Count());
    for (TInt i(0); i<iStateListeners.Count(); i++)
    {
        iStateListeners[ i ]->StateChanged(aState);
    }
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::ChangeState - State changed to %d", iState);
}

void CMMAPlayer::ReadCompletedL(TInt /*aStatus*/, const TDesC8& /*aData*/)
{
    // empty implementation
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAPlayer::ReadCompletedL ");
}
void CMMAPlayer:: DeleteControls()
{
    if (iControls.Count() > 0)
    {
        iControls.ResetAndDestroy();
    }
}
//  END OF FILE
