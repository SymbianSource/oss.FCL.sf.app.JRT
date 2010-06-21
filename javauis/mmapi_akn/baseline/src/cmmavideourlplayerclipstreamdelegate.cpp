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
* Description:  Player delegate to handle RTSP live streaming
*
*/


//  INCLUDE FILES
#include <jdebug.h>

#include "cmmavideourlplayer.h"

CMMAVideoUrlPlayer::CMMAVideoUrlPlayerClipStreamDelegate::
CMMAVideoUrlPlayerClipStreamDelegate(CMMAVideoUrlPlayer& aPlayer) :
        CMMAVideoUrlPlayerDelegate(aPlayer)
{
    // Nothing to be done.
}

CMMAVideoUrlPlayer::CMMAVideoUrlPlayerClipStreamDelegate::~CMMAVideoUrlPlayerClipStreamDelegate()
{
    // Nothing to be done.
}

void CMMAVideoUrlPlayer::CMMAVideoUrlPlayerClipStreamDelegate::StartL(TBool aPostEvent)
{
    iPlayer.CMMAMMFPlayerBase::StartL(aPostEvent);
}

void CMMAVideoUrlPlayer::CMMAVideoUrlPlayerClipStreamDelegate::StopL(TBool aPostEvent)
{
    if (iPlayer.iState == EStarted)
    {
        User::LeaveIfError(iPlayer.iController.Pause());

        if (aPostEvent)
        {
            TInt64 time;
            iPlayer.GetMediaTime(&time);
            iPlayer.PostLongEvent(CMMAPlayerEvent::EStopped, time);
        }
        // go back to prefetched state
        iPlayer.ChangeState(EPrefetched);
    }
}

void CMMAVideoUrlPlayer::CMMAVideoUrlPlayerClipStreamDelegate::GetMediaTime(TInt64* aMediaTime)
{
    iPlayer.CMMAMMFPlayerBase::GetMediaTime(aMediaTime);
}

void CMMAVideoUrlPlayer::CMMAVideoUrlPlayerClipStreamDelegate::HandleEvent(const TMMFEvent& aEvent)
{
    DEBUG_INT("MMA:CMMAVideoUrlPlayer: Clip stream: HandleEvent %d", aEvent.iEventType.iUid);
    DEBUG_INT("MMA:CMMAVideoUrlPlayer: Clip stream: HandleEvent error code: %d", aEvent.iErrorCode);

    TInt err = aEvent.iErrorCode;

    if ((aEvent.iEventType == KMMFEventCategoryVideoLoadingComplete) &&
            (iPlayer.iState == ERealized))
    {
        // Call pause only when doing prefetch.
        // Loading complete will come also when looping.
        if (err == KErrNone)
        {
            err = iPlayer.iController.Pause();
        }

        // VideoLoadingComplete-event only completes prefetch sequence
        // for non-live streams.
        iPlayer.CompletePrefetch(err);
    }
    else if (aEvent.iEventType == KMMFEventCategoryVideoPrepareComplete)
    {
        // going to prefetch state, after Play
        // KMMFEventCategoryVideoLoadingComplete event will be received

        if (err == KErrNone)
        {
            iPlayer.PrepareDisplay();

            // Buffering is done only for non-live streams.
            err = iPlayer.iController.Play();
        }

        // For non-live streams: if error is other than
        // KErrNone, then complete prefetch.
        if (err != KErrNone)
        {
            // prefetch didn't succeed
            iPlayer.CompletePrefetch(err);
        }
    }
    else
    {
        // All other events.
        iPlayer.HandleEventToParent(aEvent);
    }
}

//  END OF FILE
