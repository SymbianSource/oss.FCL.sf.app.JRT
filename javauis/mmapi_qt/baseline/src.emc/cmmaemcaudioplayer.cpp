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
#include <AudioPreference.h>
#include <logger.h>
#include "cmmaemcaudioplayer.h"

CMMAEMCAudioPlayer* CMMAEMCAudioPlayer::NewLC(
    CMMAEMCResolver* aResolver)
{
    CMMAEMCAudioPlayer* self = new(ELeave) CMMAEMCAudioPlayer(aResolver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

EXPORT_C CMMAEMCAudioPlayer::~CMMAEMCAudioPlayer()
{
}

EXPORT_C CMMAEMCAudioPlayer::CMMAEMCAudioPlayer(
    CMMAEMCResolver* aResolver):
        CMMAEMCPlayerBase(aResolver)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCAudioPlayer::CMMAEMCAudioPlayer");
}

EXPORT_C void CMMAEMCAudioPlayer::ConstructL()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCAudioPlayer::ConstructL +");
    CMMAEMCPlayerBase::ConstructL();
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCAudioPlayer::ConstructL -");
}

EXPORT_C void CMMAEMCAudioPlayer::PrefetchDataL(const TDesC8& /*aData*/)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCAudioPlayer::PrefetchDataL +");

    User::LeaveIfError(iMStreamControl->Prime());
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCAudioPlayer::PrefetchDataL -");
}

EXPORT_C void CMMAEMCAudioPlayer::PrefetchFileL()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCAudioPlayer::Prefetching from file");
}

EXPORT_C void CMMAEMCAudioPlayer::PlayCompleteL(TInt aError)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCAudioPlayer::PlayCompleteL +");
    TInt64 time;
    GetDuration(&time);
    iMediaTime = time;
    iStartedEventTime = 0;

    ChangeState(EPrefetched);   // ready to play again

    // Send 'Stopped' only when stop() is called.
    PostLongEvent(CMMAPlayerEvent::EEndOfMedia, time);

    if (aError == KErrNone)
    {
        iRepeatCount++;

        // priming again for allowing e.g. media time setting
        User::LeaveIfError(iMStreamControl->Prime());

        if (iRepeatForever || iRepeatCount < iRepeatNumberOfTimes)
        {
            StartL();
        }
        else
        {
            iRepeatCount = 0;

            TInt64 position(0);
            User::LeaveIfError(iMStreamControl->SetPosition(position));
        }
    }
    else
    {
        // error has occured, setting correct number of
        // repeats for next start
        SetLoopCount(iRepeatNumberOfTimes);
    }
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCAudioPlayer::PlayCompleteL -");
}

EXPORT_C void CMMAEMCAudioPlayer::RealizeL()
{
    LOG(EJavaMMAPI, EInfo, "CMMAEMCAudioPlayer::RealizeL +");
    // DataSource must have at least 1 stream or
    // we must have file to play
    if ((iSourceStreams.Count() == 0) && !iFileName)
    {
        LOG(EJavaMMAPI, EInfo, "CMMAEMCAudioPlayer::RealizeL : Not Enough Streams");
        User::Leave(KErrNotEnoughStreams);
    }
    User::LeaveIfError(iMStreamControl->Open());

    if (!iActiveSchedulerWait->IsStarted())
    {
        iActiveSchedulerWait->Start();
    }
    CMMAPlayer::RealizeL();
    LOG(EJavaMMAPI, EInfo, "CMMAEMCAudioPlayer::RealizeL -");
}

EXPORT_C void CMMAEMCAudioPlayer::PrefetchL()
{
    LOG(EJavaMMAPI, EInfo, "CMMAEMCAudioPlayer::PrefetchL +");
    __ASSERT_DEBUG((iSourceStreams.Count() > 0) || iFileName, User::Invariant());

    if (iFileName)
    {
        // prefetching the file
        PrefetchFileL();
        // we can go to prefetched state immediately
        ChangeState(EPrefetched);
        PostActionCompleted(KErrNone);
    }
    else
    {
        // Using TDes -- load the whole sound
        iSourceStreams[ 0 ]->ReadAllL();
    }
    // CMMASourceStream will notify with ReadCompleted
    LOG(EJavaMMAPI, EInfo, "CMMAEMCAudioPlayer::PrefetchL -");
}

EXPORT_C const TDesC& CMMAEMCAudioPlayer::Type()
{
    // Should be used in AMMS Implementation to differentiate between
    // this new kind of player with others
    return KMMAEMCAudioPlayer;
}

//
// CMMASourceStreamReader finished read operation
// This is called when ReadL is completed in Prefetch()
//
EXPORT_C void CMMAEMCAudioPlayer::ReadCompletedL(TInt aStatus, const TDesC8& aData)
{
    LOG(EJavaMMAPI, EInfo, "CMMAEMCAudioPlayer::ReadCompletedL +");
    LOG1(EJavaMMAPI, EInfo, "CMMAEMCAudioPlayer::ReadCompletedL: status = %d", aStatus);
    if (aStatus < KErrNone)
    {
        PostActionCompleted(aStatus);
    }
    else
    {
        TRAPD(err, PrefetchDataL(aData));
        if (err == KErrNone)
        {
            ChangeState(EPrefetched);
        }
        PostActionCompleted(err);
    }
    LOG(EJavaMMAPI, EInfo, "CMMAEMCAudioPlayer::ReadCompletedL -");
}

//  END OF FILE
