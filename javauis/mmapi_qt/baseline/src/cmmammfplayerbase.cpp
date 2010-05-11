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
* Description:  This class is used for playing sounds
*
*/


//  INCLUDE FILES
#include <mmf/server/mmfdes.h>
#include <AudioPreference.h>
#include <logger.h>
#include "cmmammfplayerbase.h"
#include "cmmammfresolver.h"


CMMAMMFPlayerBase::~CMMAMMFPlayerBase()
{
    // First delete the control and then close the controller
    // Added after AudioOutputControl
    DeleteControls();
    if (iControllerInfos)
    {
        iControllerInfos->ResetAndDestroy();
    }
    delete iControllerInfos;

    if (iEventMonitor)
    {
        iEventMonitor->Cancel();
    }

    iController.Close();

    delete iEventMonitor;

    delete iFileName;
}


CMMAMMFPlayerBase::CMMAMMFPlayerBase(
    CMMAMMFResolver* aResolver) :
        iMediaTime(KTimeUnknown), iStartedEventTime(0)
{
    // implementation array ownership is transferred
    iControllerInfos = aResolver->ImplementationsOwnership();

    // content type ownership is transferred
    iContentType = aResolver->ContentTypeOwnership();

    // file name ownership is transferred
    iFileName = aResolver->FileNameOwnership();
}


void CMMAMMFPlayerBase::ConstructL()
{
    CMMAPlayer::ConstructL();

    // Create an event monitor
    iEventMonitor =
        CMMFControllerEventMonitor::NewL(*this, iController);
}


EXPORT_C RMMFController& CMMAMMFPlayerBase::Controller()
{
    return iController;
}

EXPORT_C TInt CMMAMMFPlayerBase::DoOpen(TUid aSourceUid,
                                        const TDesC8& aSourceData,
                                        TUid aSinkUid,
                                        const TDesC8& aSinkData,
                                        TMMFPrioritySettings aPrioritySettings)
{
    // Make sure any existing controller is closed.
    iEventMonitor->Cancel();
    iController.Close();

    // Try opening and configuring each controller in turn
    TInt error = KErrNotSupported;
    TInt index = 0;

    // Try controllers until found a good controller or out of list
    while ((error != KErrNone) &&
            (index < iControllerInfos->Count()))
    {
        // Open the controller
        error = iController.Open((*iControllerInfos)[ index ]->Uid(),
                                 aPrioritySettings, ETrue);

        // If the controller was opened without error, start receiving events from it.
        if (!error)
        {
            iEventMonitor->Start();

            // Add the data source to the controller.
            error = iController.AddDataSource(aSourceUid, aSourceData);
        }

        // Add the data sink
        if (!error)
        {
            error = iController.AddDataSink(aSinkUid, aSinkData);
        }

        // If an error occurred in any of the above, close the controller.
        if (error)
        {
            iEventMonitor->Cancel();
            iController.Close();
        }

        index++;
    }

    return error;
}

TBool CMMAMMFPlayerBase::IsFilePlayer()
{
    if (iFileName != NULL)
    {
        return ETrue;
    }
    return EFalse;
}

void CMMAMMFPlayerBase::StartL()
{
	LOG( EJavaMMAPI, EInfo, "CMMAMMFPlayerBase::StartL +");
    iMediaTime = KTimeUnknown;
    User::LeaveIfError(iController.Play());
    LOG( EJavaMMAPI, EInfo, "CMMAMMFPlayerBase::StartL after iController.Play() ");
    // inform java side
    ChangeState(EStarted);
    LOG( EJavaMMAPI, EInfo, "CMMAMMFPlayerBase::StartL after ChangeState ");	 
     PostLongEvent(CMMAPlayerEvent::EStarted, iStartedEventTime);
    PostActionCompletedStart();
    LOG( EJavaMMAPI, EInfo, "CMMAMMFPlayerBase::StartL after PostLongEvent ");	
      
//    PostActionCompleted(KErrNone);   // java start return
    LOG( EJavaMMAPI, EInfo, "CMMAMMFPlayerBase::StartL after PostActionCompleted ");	
}

void CMMAMMFPlayerBase::StopL(TBool aPostEvent)
{
    if (iState == EStarted)
    {
        TInt64 time;
        GetMediaTime(&time);
        iStartedEventTime = time;

        TInt err = KErrNone;
        // AAC controller does not support multiple
        // calls to pause but leave with KErrNotReady.
        // That error is dismissed as player should
        // be paused already in that case.
        if (time == 0)
        {
            LOG( EJavaMMAPI, EInfo, "CMMAMMFPlayerBase::StopL: Position is zero, stopping");
            // Normally pause would be called, but if
            // current time is zero, Stop is called instead.
            // This is done because video playback breaks
            // if pause is called between events
            // KMMFEventCategoryVideoLoadingStarted and
            // KMMFEventCategoryVideoLoadingCompleted
            // (no wurther events are delivered altough
            // playback continues fine).
            // However calling Stop is tolerated in that
            // situation.
            err = iController.Stop();
            if (err == KErrNone)
            {
                err = iController.Prime();
            }
        }
        else
        {
            LOG( EJavaMMAPI, EInfo, "CMMAMMFPlayerBase::StopL: Position not zero, pausing");
            err = iController.Pause();
        }

        if ((err != KErrNone) && (err != KErrNotReady))
        {
            ELOG1( EJavaMMAPI, "CMMAMMFPlayerBase::StopL: pause/stop failed %d, leaving", err);
            User::Leave(err);
        }

        if (aPostEvent)
        {
            PostLongEvent(CMMAPlayerEvent::EStopped, time);
        }
        // go back to prefetched state
        ChangeState(EPrefetched);
    }
}


void CMMAMMFPlayerBase::DeallocateL()
{
    if (iState == EPrefetched)
    {
        // release all resources
        if (iEventMonitor)
        {
            iEventMonitor->Cancel();
        }

        // Change state first to enable AMMS to delete Effect API classes
        ChangeState(ERealized);
        iController.Stop();
        ResetSourceStreams();
    }
}


EXPORT_C void CMMAMMFPlayerBase::GetDuration(TInt64* aDuration)
{
    LOG( EJavaMMAPI, EInfo, "CMMAMMFPlayerBase::GetDuration ");
    if (iDuration == KTimeUnknown)
    {
        LOG( EJavaMMAPI, EInfo, "CMMAMMFPlayerBase::GetDuration Time unknown ");
        TTimeIntervalMicroSeconds duration;
        TInt err = iController.GetDuration(duration);
        if (!err)
        {
            iDuration = duration.Int64();
        }
    }
    *aDuration = iDuration;
    LOG( EJavaMMAPI, EInfo, "CMMAMMFPlayerBase::GetDuration - ");
}

void CMMAMMFPlayerBase::SetMediaTimeL(TInt64* aTime)
{
    LOG( EJavaMMAPI, EInfo, "CMMAMMFPlayerBase::SetMediaTimeL");

    // Negative values are not checked here
    // because it's done already in Java side.

    // Get clip duration
    TTimeIntervalMicroSeconds duration;
    User::LeaveIfError(iController.GetDuration(duration));
    LOG1( EJavaMMAPI, EInfo, "CMMAMMFPlayerBase::SetMediaTimeL iController.GetDuration=%d", duration.Int64());

    TTimeIntervalMicroSeconds position;

    // If the desired media time is beyond the duration,
    // the time is set to the end of the media.
    if (*aTime > duration.Int64())
    {
        position = duration;
    }
    else
    {
        position = *aTime;
    }

    TBool paused = EFalse;
    TInt err = KErrNone;

    if (iState == EStarted)
    {
        paused = ETrue;
        User::LeaveIfError(err = iController.Pause());
        ELOG1( EJavaMMAPI, "CMMAMMFPlayerBase::SetMediaTimeL after iController.Pause = %d", err);
    }

    if (err == KErrNone)
    {
        // The controller must be in the PRIMED or PLAYING state
        User::LeaveIfError(err = iController.SetPosition(position));
        ELOG1( EJavaMMAPI, "CMMAMMFPlayerBase::SetMediaTimeL iController.SetPosition() = %d", err);
    }

    // Reset cached media time, because actual set position may be
    // something else than aTime.
    iMediaTime = KTimeUnknown;

    // Inform about the position change to the StateListeners
    ChangeState(iState);

    // Get the actual media time
    GetMediaTime(aTime);

    iStartedEventTime = iMediaTime;

    if (err == KErrNone)
    {
        if (paused == (TBool)ETrue)
        {
            User::LeaveIfError(err = iController.Play());
            ELOG1( EJavaMMAPI, "CMMAMMFPlayerBase::SetMediaTimeL iController.Play() = %d", err);
        }
    }

    if (err != KErrNone)
    {
        User::Leave(err);
    }
}

void CMMAMMFPlayerBase::GetMediaTime(TInt64* aMediaTime)
{
    LOG( EJavaMMAPI, EInfo, "CMMAMMFPlayerBase::GetMediaTime +");
    TTimeIntervalMicroSeconds position(0);

    if (iMediaTime == KTimeUnknown || iState == EStarted)
    {
        // The controller must be in the PRIMED or PLAYING state
        TInt error(iController.GetPosition(position));

        if (error == KErrNone)
        {
            TInt64 newTime = position.Int64();
            LOG1( EJavaMMAPI, EInfo, "CMMAMMFPlayerBase::GetMediaTime iController.GetPosition : %d", newTime);

            // Sanity check for media time going backwards or beyond the
            // duration.
            // Some native controls may return zero media time for
            // a few moments just before playback will complete.
            if (newTime < iMediaTime ||
                    (iDuration > 0 && newTime > iDuration))
            {
                LOG( EJavaMMAPI, EInfo, "CMMAMMFPlayerBase::GetMediaTime.GetDuration ");
                GetDuration(&iMediaTime);
            }
            else
            {
                LOG( EJavaMMAPI, EInfo, "CMMAMMFPlayerBase::GetMediaTime.else ");
                // set return value
                iMediaTime = newTime;
            }
        }
        else
        {
            LOG1( EJavaMMAPI, EInfo, "CMMAMMFPlayerBase::GetMediaTime: error=%d, returning TIME_UNKNOWN", error);
            // cannot get media time
            iMediaTime = KTimeUnknown;
        }
    }
    *aMediaTime = iMediaTime;
    LOG1( EJavaMMAPI, EInfo, "CMMAMMFPlayerBase::GetMediaTime - %d", *aMediaTime);
}

void CMMAMMFPlayerBase::CloseL()
{
    CMMAPlayer::CloseL();
    if (iEventMonitor)
    {
        iEventMonitor->Cancel();
    }
    // First delete the control and then close the controller
    // Added after AudioOutputControl
    iController.Stop();
    delete iEventMonitor;
    iEventMonitor = NULL;
}

void CMMAMMFPlayerBase::HandleEvent(const TMMFEvent& /*aEvent*/)
{
    // empty implementation
}

//  END OF FILE
