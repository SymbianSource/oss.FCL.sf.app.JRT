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
#include <jdebug.h>
#include <JniEnvWrapper.h>
#include <ctsydomainpskeys.h>

#include "cmmaplayer.h"
#include "cmmaeventsource.h"
#include "cmmadurationupdater.h"
#include "cmmavolumecontrol.h"
#include "cmmacallstatemonitor.h"

// CONSTANTS
_LIT(KPanicOutOfMem, "out of memory");

#ifdef _DEBUG // Needed only in ASSER_DEBUG statements.
_LIT(KPanicMethod, "method not found");
#endif

EXPORT_C CMMAPlayer::~CMMAPlayer()
{
    DEBUG("CMMAPlayer::~CMMAPlayer()");

    iSourceStreams.ResetAndDestroy();
    if (iControls.Count() > 0)
        iControls.ResetAndDestroy();
    iStateListeners.Reset();
    delete iActionCompletedEvent;
    delete iOOMErrorEvent;
    delete iDurationUpdater;
    delete iContentType;
    delete iStateObserver;
}


EXPORT_C CMMAPlayer::CMMAPlayer():
        iRepeatNumberOfTimes(0),
        iRepeatForever(EFalse),
        iRepeatCount(0),
        iDuration(KTimeUnknown),
        iState(EUnrealized),
        isPausedByCall(EFalse)
{
}


void CMMAPlayer::ConstructL()
{
    DEBUG("MMA::CMMAPlayer::ConstructL + ");
    iDurationUpdater = CMMADurationUpdater::NewL(*this);
    // Listen to call state changes
    iStateObserver = CMMACallStateMonitor::NewL(KPSUidCtsyCallInformation,
                                            KCTsyCallState,this);
    
    DEBUG("MMA::CMMAPlayer::ConstructL - ");
}


void CMMAPlayer::StaticAddSourceStreamL(JNIEnv* aJniEnv,
                                        CMMAPlayer* aPlayer,
                                        CMMAEventSource* aEventSource,
                                        jobject aReader,
                                        CMMASourceStream** aSourceStream)
{
    DEBUG("MMA::CMMAPlayer::StaticAddSourceStreamL +");

    // JNI interface pointer can't be passed to different thread, so
    // it is needed to get valid JNI interface pointer for Event Server thread
    aJniEnv = JniEnvWrapper::GetValidJniRef();

    *aSourceStream = aPlayer->AddSourceStreamL(aJniEnv,
                     aEventSource,
                     aReader);
    DEBUG("MMA::CMMAPlayer::StaticAddSourceStreamL -");
}



void CMMAPlayer::StaticSetPlayerListenerObjectL(CMMAPlayer* aPlayer,
        jobject aListenerObject,
        JNIEnv* aJni,
        MMMAEventPoster* aPoster)
{

    DEBUG("MMA::CMMAPlayer::StaticSetPlayerListenerObjectL +");

    // JNI interface pointer can't be passed to different thread, so
    // it is needed to get valid JNI interface pointer for Event Server thread
    aJni = JniEnvWrapper::GetValidJniRef();

    aPlayer->SetPlayerListenerObjectL(aListenerObject, aJni, aPoster);

    DEBUG("MMA::CMMAPlayer::StaticSetPlayerListenerObjectL -");
}


void CMMAPlayer::StaticInitPlayerL(CMMAPlayer* aPlayer,
                                   jobject aPlayerObject,
                                   JNIEnv* aJni)
{
    DEBUG("MMA::CMMAPlayer::StaticInitPlayerL +");

    // JNI interface pointer can't be passed to different thread, so
    // it is needed to get valid JNI interface pointer for Event Server thread
    aJni = JniEnvWrapper::GetValidJniRef();

    jmethodID actionCompletedMethod = aJni->GetMethodID(
                                          aJni->GetObjectClass(aPlayerObject),
                                          "actionCompleted",
                                          "(I)V");

    // Sanity Check, something is really wrong if methods cannot be found
    __ASSERT_DEBUG(actionCompletedMethod,
                   User::Panic(KPanicMethod, KErrGeneral));

    aPlayer->iActionCompletedEvent = new(ELeave) CMMAEvent(
        aPlayerObject,
        actionCompletedMethod,
        CMMAEvent::EReusableEvent);

    DEBUG("MMA::CMMAPlayer::StaticInitPlayerL -");
}

CMMAControl* CMMAPlayer::StaticControl(CMMAPlayer* aPlayer, TInt aIndex)
{
    DEBUG("MMA::CMMAPlayer::StaticControl +");
    return aPlayer->Control(aIndex);

}


void CMMAPlayer::RealizeL()
{
    DEBUG("MMA::CMMAPlayer::RealizeL +");
    ChangeState(ERealized);

}


EXPORT_C void CMMAPlayer::CloseL()
{
    DEBUG("MMA::CMMAPlayer::CloseL ");
    PostObjectEvent(CMMAPlayerEvent::EClosed, NULL);

    ChangeState(EClosed);
}


EXPORT_C void CMMAPlayer::GetDuration(TInt64* aDuration)
{
    DEBUG("MMA::CMMAPlayer::GetDuration ");
    *aDuration = iDuration;
}


void CMMAPlayer::SetMediaTimeL(TInt64* aTime)
{
    DEBUG("MMA::CMMAPlayer::SetMediaTimeL ");
    *aTime = KErrNotSupported;
}

void CMMAPlayer::GetMediaTime(TInt64* aMediaTime)
{
    DEBUG("MMA::CMMAPlayer::GetMediaTime ");
    *aMediaTime = KErrNotSupported;
}


EXPORT_C void CMMAPlayer::SetLoopCount(TInt aCount)
{
    DEBUG("MMA::CMMAPlayer::SetLoopCount ");
    iRepeatForever = (aCount == KJavaRepeatForever);
    iRepeatNumberOfTimes = aCount;
    iRepeatCount = 0;
}

HBufC* CMMAPlayer::ContentType() const
{
    DEBUG("MMA::CMMAPlayer::ContentType ");
    return iContentType;
}

EXPORT_C void CMMAPlayer::SetPlayerListenerObjectL(jobject aListenerObject,
        JNIEnv* aJni,
        MMMAEventPoster* aEventPoster)
{
    DEBUG("MMA::CMMAPlayer::SetPlayerListenerObjectL +");
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

    DEBUG("MMA::CMMAPlayer::SetPlayerListenerObjectL -");
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

    User::LeaveIfError(iStateListeners.Append(aListener));

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
    DEBUG("MMA::CMMAPlayer::ResetSourceStreams +");
    int sourceStreamsCount = iSourceStreams.Count();
    for (int i = 0; i < sourceStreamsCount; i++)
    {
        iSourceStreams[ i ]->ResetData();
    }
    DEBUG("MMA::CMMAPlayer::ResetSourceStreams -");
}

EXPORT_C void CMMAPlayer::AddControlL(CMMAControl* aControl)
{

    User::LeaveIfError(iControls.Append(aControl));

}

void CMMAPlayer::PostLongEvent(CMMAPlayerEvent::TEventType aEventType,
                               const TInt64& aLongEventData)
{
    DEBUG("MMA::CMMAPlayer::PostLongEvent ");
    if (!iListenerObject || !iEventPoster)
    {
        DEBUG("MMA::CMMAPlayer::PostLongEvent No listener");
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
    DEBUG("MMA::CMMAPlayer::PostStringEvent ");
    if (!iListenerObject || !iEventPoster)
    {
        DEBUG("MMA::CMMAPlayer::PostStringEvent No listener");
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
    DEBUG("MMA::CMMAPlayer::PostObjectEvent ");

    if (!iListenerObject || !iEventPoster)
    {
        DEBUG("MMA::CMMAPlayer::PostObjectEvent No listener");
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
        CMMAEventSource* aEventSource,
        jobject aReader)
{
    DEBUG("MMA::CMMAPlayer::AddSourceStreamL ");
    CMMASourceStream* sourceStream = CMMASourceStream::NewL(aJNIEnv,
                                     aEventSource,
                                     aReader,
                                     this);

    CleanupStack::PushL(sourceStream);
    User::LeaveIfError(iSourceStreams.Append(sourceStream));
    CleanupStack::Pop(); // sourceStream
    return sourceStream;
}


EXPORT_C void CMMAPlayer::PostActionCompleted(TInt aError)
{
    iActionCompletedEvent->SetEventData(aError);
    iEventPoster->PostEvent(iActionCompletedEvent,
                            CMMAEvent::ENotifyPriority);
}


EXPORT_C void CMMAPlayer::ChangeState(TPlayerState aState)
{
    iState = aState;
    DEBUG_INT("MMA::CMMAPlayer::ChangeState - iStateListeners count is %d", iStateListeners.Count());
    for (TInt i(0); i<iStateListeners.Count(); i++)
    {
        iStateListeners[ i ]->StateChanged(aState);
    }
    DEBUG_INT("MMA::CMMAPlayer::ChangeState - State changed to %d", iState);
}

void CMMAPlayer::ReadCompletedL(TInt /*aStatus*/, const TDesC8& /*aData*/)
{
    // empty implementation
    DEBUG("MMA::CMMAPlayer::ReadCompletedL ");
}
void CMMAPlayer:: DeleteControls()
{
    if (iControls.Count() > 0)
    {
        iControls.ResetAndDestroy();
    }
}


EXPORT_C void CMMAPlayer::HandleCallStateEventL(TUid aUid, TInt aKey)
{
    (void)aUid;     // Suppress warnings about unused arguments in urel builds
    (void)aKey;

    DEBUG("CMMAPlayer::HandleCallStateEvent +");
    DEBUG_INT2( "CMMAVideoUrlPlayer::HandleCallStateEvent(TUid aUid, TInt aKey) = (%d, %d)",aUid.iUid, aKey);

    TInt callState;
    iStateObserver->GetValue(callState);
    DEBUG_INT( "CMMAVideoUrlPlayer::HandleCallStateEvent: state = %d", callState);

    switch (callState)
    {
    case EPSCTsyCallStateUninitialized:
    {
        DEBUG_INT( "CMMAVideoUrlPlayer::HandlePSEvent EPSCTsyCallStateUninitialized istate = %d", iState);
        break;
    }

    case EPSCTsyCallStateAlerting:

    case EPSCTsyCallStateRinging:

    case EPSCTsyCallStateDialling:
 
    case EPSCTsyCallStateAnswering:

    case EPSCTsyCallStateConnected:
    {
        DEBUG_INT( "CMMAVideoUrlPlayer::HandlePSEvent EPSCTsyCallStateConnected iState = %d", iState);
        if(iState == CMMAPlayer::EStarted)
        {
            DEBUG(  "CMMAVideoUrlPlayer::HandlePSEvent EPSCTsyCallStateConnected started state");
            StopL(ETrue);
            isPausedByCall = ETrue;
        }
        break;
    }
    case EPSCTsyCallStateDisconnecting:
    {
      DEBUG_INT( "CMMAVideoUrlPlayer::HandlePSEvent EPSCTsyCallStateDisconnecting istate = %d", iState);
      break;
    }
    case EPSCTsyCallStateNone:
    {
        DEBUG_INT( "CMMAVideoUrlPlayer::HandlePSEvent EPSCTsyCallStateNone istate = %d", iState);
        if( (iState == CMMAPlayer::EPrefetched) && isPausedByCall )
        {
            DEBUG(  "CMMAVideoUrlPlayer::HandlePSEvent EPSCTsyCallStateNone Prefetched state");
            StartL(ETrue);
            isPausedByCall = EFalse;
        }
        break;
    }

    case EPSCTsyCallStateHold:
    {
        DEBUG_INT( "CMMAVideoUrlPlayer::HandlePSEvent EPSCTsyCallStateHold iState = %d", iState);
        break;
    }
    default:
    {
        DEBUG( "CMMAVideoUrlPlayer::HandlePSEvent default");
        // Default is no pause
        break;
    }
    }
}
//  END OF FILE
