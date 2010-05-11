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
* Description:  Player delegate to handle RTSP live streaming
*
*/


//  INCLUDE FILES
#include <logger.h>

#include "cmmavideourlplayer.h"

CMMAVideoUrlPlayer::CMMAVideoUrlPlayerLiveStreamDelegate*
CMMAVideoUrlPlayer::CMMAVideoUrlPlayerLiveStreamDelegate::NewL(CMMAVideoUrlPlayer& aPlayer)
{
    CMMAVideoUrlPlayerLiveStreamDelegate* self =
        new(ELeave) CMMAVideoUrlPlayerLiveStreamDelegate(aPlayer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

CMMAVideoUrlPlayer::CMMAVideoUrlPlayerLiveStreamDelegate::
CMMAVideoUrlPlayerLiveStreamDelegate(CMMAVideoUrlPlayer& aPlayer) :
        CMMAVideoUrlPlayerDelegate(aPlayer), iMediaStartTime(0),
        iStoppedAtTime(0)
{
}

void CMMAVideoUrlPlayer::CMMAVideoUrlPlayerLiveStreamDelegate::ConstructL()
{
    iActiveSchedulerWait = new(ELeave)CActiveSchedulerWait;
}

CMMAVideoUrlPlayer::CMMAVideoUrlPlayerLiveStreamDelegate::~CMMAVideoUrlPlayerLiveStreamDelegate()
{
    delete iActiveSchedulerWait;
}

void CMMAVideoUrlPlayer::CMMAVideoUrlPlayerLiveStreamDelegate::StartL()
{
    // start can't be called to not ready player
    ASSERT(iPlayer.iState == EPrefetched);


    iPlayer.PrefetchL();

    // Refresh all controls again after second prefetch
    iPlayer.RefreshControls();

    // Completed in VideoPrepareComplete-event
    if (!iActiveSchedulerWait->IsStarted())
    {
        LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: StartL() ASW Start1");
        iActiveSchedulerWait->Start();
    }

    // Prime() is called sometimes twice since it's needed also when
    // restarting the playing
    User::LeaveIfError(iPlayer.iController.Prime());
    User::LeaveIfError(iPlayer.iController.Play());

    // Completed in VideoLoadingComplete-event
    if (!iActiveSchedulerWait->IsStarted())
    {
        LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: StartL() ASW Start2");
        iActiveSchedulerWait->Start();
    }
    iPlayer.PostActionCompleted(KErrNone);   // java start return
}

void CMMAVideoUrlPlayer::CMMAVideoUrlPlayerLiveStreamDelegate::StopL(TBool aPostEvent)
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: StopL");
    if (iPlayer.iState == EStarted)
    {
        LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: StopL : Started ");
        GetMediaTime(&iStoppedAtTime);
        if (aPostEvent)
        {
            LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: StopL : Postevent ");
            iPlayer.PostLongEvent(CMMAPlayerEvent::EStopped, iStoppedAtTime);
        }
        // go back to prefetched state
        iPlayer.ChangeState(EPrefetched);

        // Call stop instead of Pause as per the suggestions from helix
        // Pause has no meaning for live streaming
        iPlayer.iController.Stop();
    }
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: StopL - ");
}

void CMMAVideoUrlPlayer::CMMAVideoUrlPlayerLiveStreamDelegate::GetMediaTime(TInt64* aMediaTime)
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer::CMMAVideoUrlPlayerLiveStreamDelegate::GetMediaTime +");
    if (iPlayer.iState == EStarted)
    {
        LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer::CMMAVideoUrlPlayerLiveStreamDelegate::GetMediaTime Started Playerbase call");
        iPlayer.CMMAMMFPlayerBase::GetMediaTime(aMediaTime);
        *aMediaTime -= iMediaStartTime;
    }
    else
    {
        LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer::CMMAVideoUrlPlayerLiveStreamDelegate::GetMediaTime Stopped");
        *aMediaTime = iStoppedAtTime;
    }
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer::CMMAVideoUrlPlayerLiveStreamDelegate::GetMediaTime -");
}

void CMMAVideoUrlPlayer::CMMAVideoUrlPlayerLiveStreamDelegate::HandleEvent(const TMMFEvent& aEvent)
{
    LOG1( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: HandleEvent %d", aEvent.iEventType.iUid);
    ELOG1( EJavaMMAPI, "MMA:CMMAVideoUrlPlayer: Live stream: HandleEvent error code: %d", aEvent.iErrorCode);

    TInt err = aEvent.iErrorCode;

    if ((aEvent.iEventType == KMMFEventCategoryVideoLoadingComplete) &&
            (iPlayer.iState == EPrefetched))
    {
        LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: KMMFEventCategoryVideoLoadingComplete +");
        if (err == KErrNone)
        {
            TTimeIntervalMicroSeconds position(0);
            TInt error(iPlayer.iController.GetPosition(position));
            if (error == KErrNone)
            {
                iMediaStartTime = position.Int64();
            }

            // inform java side. Always start playback from zero for
            // live stream.
            iPlayer.PostLongEvent(CMMAPlayerEvent::EStarted, 0);

            // Buffering takes a long time, so player state is not changed
            // until playback really starts.
            iPlayer.ChangeState(EStarted);
        }
        if (iActiveSchedulerWait->IsStarted())
        {
            LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: KMMFEventCategoryVideoLoadingComplete ASW Stop");
            iActiveSchedulerWait->AsyncStop();
        }
        if (err != KErrNone)
        {
            LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: KMMFEventCategoryVideoLoadingComplete Error Inform Parent");
            iPlayer.HandleEventToParent(aEvent);
        }
        LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: KMMFEventCategoryVideoLoadingComplete -");
    }
    else if (aEvent.iEventType == KMMFEventCategoryVideoPrepareComplete)
    {
        LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: KMMFEventCategoryVideoPrepareComplete +");
        // going to prefetch state, after Play
        // KMMFEventCategoryVideoLoadingComplete event will be received

        if (err == KErrNone)
        {
            iPlayer.PrepareDisplay();
        }

        // For live streams: complete prefetch.
        // if activeschedulerwait is started, then
        // do not complete prefetch as we are here
        // as a result of calling StartL and not for
        // PrefetchL.
        if (!(iActiveSchedulerWait->IsStarted()))
        {
            iPlayer.CompletePrefetch(err);
        }

        if (iActiveSchedulerWait->IsStarted())
        {
            LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: KMMFEventCategoryVideoPrepareComplete ASW Stop");
            iActiveSchedulerWait->AsyncStop();
        }
        LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: KMMFEventCategoryVideoPrepareComplete -");
    }
    else if (aEvent.iEventType == KMMFEventCategoryVideoPlayerGeneralError)
    {
        LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: KMMFEventCategoryVideoPlayerGeneralError +");
        // For live streams: KMMFEventCategoryVideoPlayerGeneralError means helix is closed
        // side if player is in prefetched state.
        if (iActiveSchedulerWait->IsStarted())
        {
            LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: KMMFEventCategoryVideoPlayerGeneralError ASW Stop");
            iActiveSchedulerWait->AsyncStop();
        }

        // usually error condition -45 (KErrSessionClosed) or -33 (KErrTimedOut)
        if (err != KErrNone)
        {
            LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: KMMFEventCategoryVideoPlayerGeneralError Inform Parent");
            iPlayer.HandleEventToParent(aEvent);
        }
        LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: KMMFEventCategoryVideoPlayerGeneralError -");
    }
    else
    {
        LOG1( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: + 0x%X", aEvent.iEventType.iUid);
        // For live streams: KErrSessionClosed is not posted to Java
        // side if player is in prefetched state.
        if ((iPlayer.iState != EPrefetched) ||
                (aEvent.iErrorCode != KErrSessionClosed))
        {
            LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: Error Inform Parent");
            // All other events.
            iPlayer.HandleEventToParent(aEvent);
        }
        LOG1( EJavaMMAPI, EInfo, "MMA:CMMAVideoUrlPlayer: Live stream: - 0x%X", aEvent.iEventType.iUid);
    }

}

//  END OF FILE
