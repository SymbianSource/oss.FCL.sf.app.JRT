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
* Description:  This class is used for streaming audio.
*
*/


//  INCLUDE FILES
#include <AudioPreference.h>
#include <logger.h>

#include "cmmaaudiostreamplayer.h"
#include "cmmadatasourcestream.h"
#include "mmafunctionserver.h"
#include "cmmastreamhandler.h"

_LIT(KMMAStreamErrorMessage, "Internal error: %d");


CMMAAudioStreamPlayer* CMMAAudioStreamPlayer::NewLC(
    CMMAEMCResolver* aResolver)
{
    LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::NewLC +");
    CMMAAudioStreamPlayer* self = new(ELeave) CMMAAudioStreamPlayer(aResolver);
    CleanupStack::PushL(self);
    self->ConstructL();
    LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::NewLC -");
    return self;
}

CMMAAudioStreamPlayer::~CMMAAudioStreamPlayer()
{
    LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::~CMMAAudioStreamPlayer +");

    if (iMStreamControl->GetState() > MStreamControl::CLOSED)
    {
        iMStreamControl->Close();
    }

    delete iStreamHandler;

    LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::~CMMAAudioStreamPlayer -");
}


CMMAAudioStreamPlayer::CMMAAudioStreamPlayer(
    CMMAEMCResolver* aResolver):
        CMMAEMCAudioPlayer(aResolver)
{
    LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::CMMAAudioStreamPlayer");
}

void CMMAAudioStreamPlayer::ConstructL()
{
    LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::ConstructL +");
    iControllerPrimed = EFalse;
    CMMAEMCAudioPlayer::ConstructL();
    iMetaDataUtility = CMetaDataUtility::NewL();
    iStreamHandler = CMMAStreamHandler::NewL(*this,
                     *iMStreamControl,
                     *iMDataBufferSource,
                     *iFactory,
                     *iMetaDataUtility);
    iActiveSchedulerWait = new(ELeave)CActiveSchedulerWait();
    LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::ConstructL -");
}

CMMASourceStream* CMMAAudioStreamPlayer::AddSourceStreamL(JNIEnv* aJNIEnv,
        MMAFunctionServer* aEventSource,
        jobject aReader)
{
    LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::AddSourceStreamL +");
    CMMADataSourceStream* sourceStream = CMMADataSourceStream::NewLC(aJNIEnv,
                                         aEventSource,
                                         aReader,
                                         this,
                                         aEventSource);
    User::LeaveIfError(iSourceStreams.Append(sourceStream));
    CleanupStack::Pop(sourceStream);
    iStreamHandler->SetSourceStream(sourceStream);
    LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::AddSourceStreamL -");
    return sourceStream;
}

CMetaDataUtility* CMMAAudioStreamPlayer::MetaDataUtilityOwnership()
{
    CMetaDataUtility* temp = iMetaDataUtility;
    iMetaDataUtility = NULL;
    return temp;
}

void CMMAAudioStreamPlayer::DeallocateL()
{
    LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::DeallocateL +");
    iStreamHandler->Stop();
    iControllerPrimed = EFalse;
    CMMAEMCPlayerBase::DeallocateL();
    LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::DeallocateL -");
}

void CMMAAudioStreamPlayer::PrefetchL()
{
    LOG( EJavaMMAPI, EInfo, "CMMAAudioStreamPlayer::PrefetchL +");
    __ASSERT_DEBUG(iSourceStreams.Count() > 0, User::Invariant());
    iStreamHandler->Prepare(*iMimeType);
    LOG( EJavaMMAPI, EInfo, "CMMAAudioStreamPlayer::PrefetchL -");
}

void CMMAAudioStreamPlayer::StartL()
{
    LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::StartL +");
    if (iStreamHandler->LastBufferWritten() &&
            (iMStreamControl ->GetState() == MStreamControl::PAUSED))
    {
        LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::StartL If outer+");
        TInt64 time;
        GetMediaTime(&time);
        TInt err = iMStreamControl->Start();
        if (err == KErrNone && iState != EStarted)
        {   LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::StartL If inner+");
            // move to started state and post started event            
            ChangeState(EStarted);
            PostLongEvent(CMMAPlayerEvent::EStarted, time);
        }
        else
        {   LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::StartL else inner+");
            HandleError(err);
        }
    }
    else
    {   LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::StartL else outer+");
        iStreamHandler->Start();
    }

    LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::StartL -");
}

void CMMAAudioStreamPlayer::StopL(TBool aPostEvent)
{
    LOG1( EJavaMMAPI, EInfo, "CMMAAudioStreamPlayer::Stop state %d", iState);
    if (iState == EStarted)
    {
        User::LeaveIfError(Pause());
        // go back to prefetched state
        ChangeState(EPrefetched);
        if (aPostEvent)
        {
            TInt64 time;
            GetMediaTime(&time);
            PostLongEvent(CMMAPlayerEvent::EStopped, time);
        }
    }
    LOG( EJavaMMAPI, EInfo, "CMMAAudioStreamPlayer::Stop OK");
}

TInt CMMAAudioStreamPlayer::Pause()
{
    LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::Pause +");
    iStreamHandler->Pause();
    LOG1( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer:: iStreamControl State = %d",iMStreamControl->GetState());

    TInt err = KErrNone;
    if (iMStreamControl->GetState() != MStreamControl::PAUSED)
    {
        err = iMStreamControl->Pause();
        ELOG1( EJavaMMAPI, "MMA::CMMAAudioStreamPlayer:: iStreamControl Pause error = %d", err);
        if ((!iActiveSchedulerWait->IsStarted()) && (err == KErrNone))
        {
            iActiveSchedulerWait->Start();
        }
    }
    LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::Pause -");
    return err;
}

void CMMAAudioStreamPlayer::PlayCompleteL(TInt aError)
{
    ELOG1( EJavaMMAPI, "MMA::CMMAAudioStreamPlayer::PlayCompleteL error %d",
              aError);

    // Before controller is started it must be primed
    iControllerPrimed = EFalse;

    TInt64 time;
    GetDuration(&time);

    // Send 'Stopped' only when stop() is called.
    PostLongEvent(CMMAPlayerEvent::EEndOfMedia, time);

    ChangeState(EPrefetched);   // ready to play again

    if (aError == KErrNone)
    {
        iRepeatCount++;

        if (iRepeatForever || iRepeatCount < iRepeatNumberOfTimes)
        {
            StartL();
        }
        else
        {
            iRepeatCount = 0;
        }
    }
    else
    {
        // error has occured, setting correct number of
        // repeats for next start
        SetLoopCount(iRepeatNumberOfTimes);
    }
}

void CMMAAudioStreamPlayer::GetDuration(TInt64* aDuration)
{
    CMMAPlayer::GetDuration(aDuration);
}

void CMMAAudioStreamPlayer::PrepareComplete(TInt aError)
{
    ELOG1( EJavaMMAPI, "MMA::CMMAAudioStreamPlayer::PrepareComplete error %d",
              aError);

    if (aError == KErrNone)
    {
        ChangeState(EPrefetched);
    }
    PostActionCompleted(aError);   // java prefetch return
}

void CMMAAudioStreamPlayer::StartComplete(TInt aError)
{
    ELOG1( EJavaMMAPI, "MMA::CMMAAudioStreamPlayer::StartComplete error %d",
              aError);

    // do not start if player is deallocated or closed
    // RateControl start can start controller in started state
    if ((iState != EStarted) &&
            (iState != EPrefetched))
    {
        PostActionCompleted(KErrNone);   // java start return
        return;
    }

    TInt err = aError;
    if (!iControllerPrimed)
    {
        // Prime must be called when player is started first time or restarted
        err = iMStreamControl->Prime();

        ELOG1( EJavaMMAPI, "MMA::CMMAAudioStreamPlayer::StartComplete prime error %d",
                  err);
    }
    else
    {
        err = KErrNone;
    }

    if (iControllerPrimed && (iState == EPrefetched))
    {

        TInt64 time;
        if (err == KErrNone)
        {
            // must be primed before media time can be get
            GetMediaTime(&time);
            err = iMStreamControl->Start();
            ELOG1( EJavaMMAPI, "MMA::CMMAAudioStreamPlayer::StartComplete play error %d",
                      err);

        }

        // RateControl can start controller in started state, then Java event is
        // not sent
        if (err == KErrNone && iState != EStarted)
        { // move to started state and post started event
            PostLongEvent(CMMAPlayerEvent::EStarted, time);
            ChangeState(EStarted);
        }
        else
        { // post error event
            HandleError(aError);
            PostActionCompleted(aError);   // java start return
        }
    }

}

void CMMAAudioStreamPlayer::HandleError(TInt aError)
{
    ELOG1( EJavaMMAPI, "MMA::CMMAAudioStreamPlayer::HandleError error %d",
              aError);

    TName errorMessage;
    errorMessage.Format(KMMAStreamErrorMessage, aError);
    PostStringEvent(CMMAPlayerEvent::EError, errorMessage);
}

void CMMAAudioStreamPlayer::Event(MControl* aControl, TUint aEventType, TAny* aEventObject)
{

    switch (aEventType)
    {

    case MStreamControlObserver::KStateChangedEvent:
    {
        MStateChangedEvent* evt = (MStateChangedEvent*)aEventObject;
        MStreamControl* control1 = (MStreamControl*)(aControl);
        switch (control1->GetState())
        {
        case MStreamControl::CLOSED:
        {
            iPrevStreamControlState = MStreamControl::CLOSED;
            LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::Event:KStateChangedEvent :CLOSED");
        }
        break;

        case MStreamControl::INITIALIZED:
        {
            LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::Event:KStateChangedEvent :INITIALIZED");
            switch (iPrevStreamControlState)
            {
            case MStreamControl::CLOSED:
            {
                iPrevStreamControlState = MStreamControl::INITIALIZED;
                if (iActiveSchedulerWait->IsStarted())
                {
                    iActiveSchedulerWait->AsyncStop();
                }
            }
            break;

            case MStreamControl::INITIALIZED:
            {
                iPrevStreamControlState = MStreamControl::INITIALIZED;
                LOG( EJavaMMAPI, EInfo, "inner Switch case: MStreamControl::INITIALIZED ");
                ELOG1( EJavaMMAPI, "MMA::CMMAAudioStreamPlayer::Event:KStateChangedEvent:ErrorCode = %d ",evt->GetErrorCode());
                // error occured during prime operation
                // move player back to prefetched state
                if (iState == EStarted)
                {
                    ChangeState(EPrefetched);
                }
                PostActionCompleted(evt->GetErrorCode());   // java start return
            }
            break;

            case MStreamControl::PRIMED:
            {
                iPrevStreamControlState = MStreamControl::INITIALIZED;
                LOG( EJavaMMAPI, EInfo, "inner Switch case: MStreamControl::PRIMED ");

            }
            break;

            case MStreamControl::EXECUTING:
            {
                iPrevStreamControlState = MStreamControl::INITIALIZED;
                LOG( EJavaMMAPI, EInfo, "inner Switch case: MStreamControl::EXECUTING ");
                ELOG1( EJavaMMAPI, "MMA::CMMAAudioStreamPlayer::Event:KStateChangedEvent:ErrorCode = %d ",evt->GetErrorCode());
                if (KErrEof == evt->GetErrorCode())
                {
                    TRAPD(error, PlayCompleteL(KErrNone));
                    if (KErrNone != error)
                    {
                        ELOG1( EJavaMMAPI, "MMA::CMMAAudioStreamPlayer::Event:PlayCompleteL Error = %d", error);
                    }
                }
            }
            break;

            case MStreamControl::BUFFERING:
            {
                iPrevStreamControlState = MStreamControl::INITIALIZED;
                LOG( EJavaMMAPI, EInfo, "inner Switch case: MStreamControl::BUFFERING ");
            }
            break;

            case MStreamControl::PAUSED:
            {
                iPrevStreamControlState = MStreamControl::INITIALIZED;
                LOG( EJavaMMAPI, EInfo, "inner Switch case: MStreamControl::PAUSED ");
            }
            break;
            }
        }
        break;

        case MStreamControl::PRIMED:
        {
            LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::Event:KStateChangedEvent :PRIMED");
            iPrevStreamControlState = MStreamControl::PRIMED;
            iControllerPrimed = ETrue;
            TInt64 time;
            // must be primed before media time can be get
            GetMediaTime(&time);
            TInt err = iMStreamControl->Start();
            ELOG1( EJavaMMAPI, "MMA::CMMAAudioStreamPlayer::Event play error %d",
                      err);

            // RateControl can start controller in started state, then Java event is
            // not sent
            if (err == KErrNone && iState != EStarted)
            { // move to started state and post started event
                PostLongEvent(CMMAPlayerEvent::EStarted, time);
                ChangeState(EStarted);
            }
            else
            {
                PostLongEvent(CMMAPlayerEvent::EStarted, time);
            }
        }
        break;

        case MStreamControl::EXECUTING:
        {
            LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::Event:KStateChangedEvent :EXECUTING");
            iPrevStreamControlState = MStreamControl::EXECUTING;
            PostActionCompleted(KErrNone);   // java start return
        }
        break;

        case MStreamControl::BUFFERING:
        {
            iPrevStreamControlState = MStreamControl::BUFFERING;
            LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::Event:KStateChangedEvent :BUFFERING");
        }
        break;

        case MStreamControl::PAUSED:
        {
            iPrevStreamControlState = MStreamControl::PAUSED;
            if (iActiveSchedulerWait->IsStarted())
            {
                iActiveSchedulerWait->AsyncStop();
            }
            LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::Event:KStateChangedEvent :PAUSED");
        }
        break;

        default:
            LOG( EJavaMMAPI, EInfo, "MMA::CMMAAudioStreamPlayer::Event:KStateChangedEvent :DEFAULT");
            break;
        }
    }
    break;

    case MControlObserver::KErrorEvent:
    {
        MErrorCode* evt = (MErrorCode*)aEventObject;
        if (KErrNone != evt->GetErrorCode())
        {
            ELOG1( EJavaMMAPI, "MMA::CMMAAudioStreamPlayer::Event:KErrorEvent, err = %d", evt->GetErrorCode());
        }
    }
    break;
    }
}
//  END OF FILE
