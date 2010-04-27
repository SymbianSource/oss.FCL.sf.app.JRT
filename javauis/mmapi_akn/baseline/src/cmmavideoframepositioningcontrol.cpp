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
* Description:  This class implements FramePositioningControl for video player
*
*/


//  INCLUDE FILES
#include <jdebug.h>
#include <e32base.h>
#include <e32math.h>

#include "cmmavideoframepositioningcontrol.h"

namespace
{
const TInt32 KMMAFramePosMicroSecMultiplier = 1000000;
}

CMMAVideoFramePositioningControl*
CMMAVideoFramePositioningControl::NewL(CMMAVideoPlayer* aPlayer)
{
    CMMAVideoFramePositioningControl* self =
        new(ELeave) CMMAVideoFramePositioningControl(aPlayer);
    return self;

}
CMMAVideoFramePositioningControl::
CMMAVideoFramePositioningControl(CMMAVideoPlayer* aPlayer)
        : CMMAFramePositioningControl(aPlayer), iPlayer(aPlayer)
{
    DEBUG("MMA:CMMAVideoFramePositioningControl::CMMAVideoFramePositioningControl");
}

CMMAVideoFramePositioningControl::~CMMAVideoFramePositioningControl()
{
    DEBUG("MMA:CMMAVideoFramePositioningControl::~CMMAVideoFramePositioningControl");
}

TInt CMMAVideoFramePositioningControl::SeekL(TInt aFrameNumber)
{
    DEBUG_INT("CMMAVideoFramePositioningControl::SeekL, aFrameNumber: %d", aFrameNumber);

    RMMFController& controller = iPlayer->Controller();

    // Controller is paused before changing position to increase
    // accuracy
    TBool playerStarted = EFalse;
    if (iPlayer->State() == CMMAPlayer::EStarted)
    {
        playerStarted = ETrue;
    }

    if (playerStarted)
    {
        User::LeaveIfError(controller.Pause());
    }

    // Frame number must be positive
    TInt frameNumber = aFrameNumber;
    if (frameNumber < 0)
    {
        frameNumber = 0;
    }

    // Find out framerate of video
    TReal32 frameRate = 0;
    GetAndCheckFrameRateL(frameRate);

    // Calculate new media time
    TReal tempReal = ((TReal32)frameNumber *
                      KMMAFramePosMicroSecMultiplier) / frameRate;
    User::LeaveIfError(Math::Round(tempReal, tempReal, 0));
    TInt64 newMediaTime = tempReal;

    // New media time is clamped to duration
    TInt err = ClampMediaTime(newMediaTime);
    // Clamp fails if duration of content is not known.
    // Two additional resorts are (in preferred order):
    // 1) Return current frame position if it can be calculated.
    // 2) Or seek to 0.

    // First resort
    if (err != KErrNone)
    {
        iPlayer->GetMediaTime(&newMediaTime);
        // Second resort
        if (newMediaTime < 0)
        {
            newMediaTime = 0;
        }
    }
    iPlayer->SetMediaTimeL(&newMediaTime);
    // Error condition
    if (newMediaTime < KErrNotFound)
    {
        User::Leave(KErrNotFound);
    }

    // Calculate actually set frame number
    tempReal = (((TReal32)newMediaTime / KMMAFramePosMicroSecMultiplier) *
                (TReal32) frameRate);
    User::LeaveIfError(Math::Round(tempReal, tempReal, 0));

    // Restart controller
    if (playerStarted)
    {
        User::LeaveIfError(controller.Play());
    }

    return tempReal;
}

TInt CMMAVideoFramePositioningControl::SkipL(TInt aFramesToSkip)
{
    DEBUG_INT("CMMAVideoFramePositioningControl::SkipL, aFramesToSkip: %d", aFramesToSkip);

    RMMFController& controller = iPlayer->Controller();

    // Controller is paused before changing position to increase
    // accuracy
    TBool playerStarted = EFalse;
    if (iPlayer->State() == CMMAPlayer::EStarted)
    {
        playerStarted = ETrue;
    }

    if (playerStarted)
    {
        User::LeaveIfError(controller.Pause());
    }

    // Find out framerate of video
    TReal32 frameRate = 0;
    GetAndCheckFrameRateL(frameRate);

    // Find out current media time
    TInt64 currentMediaTime = 0;
    iPlayer->GetMediaTime(&currentMediaTime);

    // Error condition
    if (currentMediaTime < KErrNone)
    {
        User::Leave(KErrNotFound);
    }

    // Find out current frame number
    TReal tempReal =
        ((TReal32) currentMediaTime / KMMAFramePosMicroSecMultiplier) *
        frameRate;
    User::LeaveIfError(Math::Round(tempReal, tempReal, 0));
    TInt currentFrameNumber = tempReal;

    // Calculate new media time
    TInt64 newMediaTime = currentMediaTime +
                          (((TReal32) aFramesToSkip / frameRate) * KMMAFramePosMicroSecMultiplier);

    // New media time is clamped to duration
    TInt err = ClampMediaTime(newMediaTime);

    // If clamping fails, no frames are skipped
    TInt framesSkipped = 0;
    if (err == KErrNone)
    {
        iPlayer->SetMediaTimeL(&newMediaTime);
        // Error condition
        if (newMediaTime < KErrNone)
        {
            User::Leave(KErrNotFound);
        }

        // Calculate actual amount of frames skipped
        tempReal = ((((TReal32) newMediaTime / KMMAFramePosMicroSecMultiplier) *
                     (TReal32) frameRate) - currentFrameNumber);

        User::LeaveIfError(Math::Round(tempReal, tempReal, 0));
        framesSkipped = tempReal;
    }

    // Restart controller
    if (playerStarted)
    {
        User::LeaveIfError(controller.Play());
    }

    return framesSkipped;
}

void CMMAVideoFramePositioningControl::MapFrameToTimeL(TInt aFrameNumber, TInt64* aMediaTime)
{
    DEBUG_INT("CMMAVideoFramePositioningControl::MapFrameToTimeL, aFrameNumber: %d", aFrameNumber);
    // Find out framerate of video
    TReal32 frameRate = 0;
    GetAndCheckFrameRateL(frameRate);

    // Find out frame mediatime
    TInt64 frameMediaTime =
        ((TReal32)aFrameNumber / (TReal32)frameRate) *
        KMMAFramePosMicroSecMultiplier;

    TInt64 duration;
    iPlayer->GetDuration(&duration);

    if (frameMediaTime < 0)
    {
        frameMediaTime = KErrNotFound;
    }
    else if (frameMediaTime > duration)
    {
        frameMediaTime = KErrNotFound;
        // With some medias last frame media time is few hundred microsec's
        // over duration. This is because framerate is not accurate.
        // If given frame number is last frame and frameMediaTime > duration,
        // return duration
        // Find out last frame
        TReal lastFrame = (frameRate * (TReal32)duration) /
                          KMMAFramePosMicroSecMultiplier;
        User::LeaveIfError(Math::Round(lastFrame, lastFrame, 0));
        if (aFrameNumber == lastFrame)
        {
            frameMediaTime = duration;
        }
    }
    *aMediaTime = frameMediaTime;
}

TInt CMMAVideoFramePositioningControl::MapTimeToFrameL(TInt64* aMediaTime)
{
    DEBUG_INT("CMMAVideoFramePositioningControl::MapTimeToFrameL, aMediaTime: %d", *aMediaTime);
    TInt64 mediaTime = *aMediaTime;

    // If given media time is < 0 or > duration, cannot map time to frame
    // conversion fails, -1 is returned
    TInt64 duration;
    iPlayer->GetDuration(&duration);
    if (mediaTime > duration || mediaTime < 0)
    {
        return KErrNotFound;
    }

    // Find out framerate of video
    TReal32 frameRate = 0;
    GetAndCheckFrameRateL(frameRate);

    // Calculate frame number from media time
    TReal tempReal = (TInt)((TReal64)(mediaTime * frameRate) /
                            KMMAFramePosMicroSecMultiplier);
    User::LeaveIfError(Math::Round(tempReal, tempReal, 0));

    return tempReal;
}


void CMMAVideoFramePositioningControl::GetAndCheckFrameRateL(
    TReal32& aFrameRate)
{
    DEBUG("CMMAVideoFramePositioningControl::GetAndCheckFrameRateL");
    RMMFVideoControllerCustomCommands customCommands =
        RMMFVideoControllerCustomCommands(iPlayer->Controller());

    User::LeaveIfError(customCommands.GetFrameRate(aFrameRate));
    DEBUG_INT("CMMAVideoFramePositioningControl::GetAndCheckFrameRateL, aFrameRate: %d", aFrameRate);
    if (aFrameRate <= 0)
    {
        // zero framerate is not accepted because cannot
        // divide by zero. Negative framerate is not
        // acceptable aswell.
        DEBUG("CMMAVideoFramePositioningControl::GetAndCheckFrameRateL: invalid framerate");
        User::Leave(KErrNotFound);
    }
}

//  END OF FILE
