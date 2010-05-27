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
* Description:  This class implements FramePositioningControl for animation
*
*/


//  INCLUDE FILES
#include <logger.h>
#include <e32base.h>

#include "MIHLImageViewer.h"
#include "cmmaanimationframepositioningcontrol.h"

CMMAAnimationFramePositioningControl*
CMMAAnimationFramePositioningControl::NewL(CMMAAnimationPlayer* aPlayer)
{
    CMMAAnimationFramePositioningControl* self =
        new(ELeave) CMMAAnimationFramePositioningControl(aPlayer);
    return self;
}

CMMAAnimationFramePositioningControl::
CMMAAnimationFramePositioningControl(CMMAAnimationPlayer* aPlayer)
        : CMMAFramePositioningControl(aPlayer), iPlayer(aPlayer)
{
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAAnimationFramePositioningControl::CMMAAnimationFramePositioningControl");
}

CMMAAnimationFramePositioningControl::~CMMAAnimationFramePositioningControl()
{
    LOG(EJavaMMAPI, EInfo, "MMA:CMMAAnimationFramePositioningControl::~CMMAAnimationFramePositioningControl");
}

TInt CMMAAnimationFramePositioningControl::SeekL(TInt aFrameNumber)
{
    LOG(EJavaMMAPI, EInfo, "CMMAAnimationFramePositioningControl::SeekL");
    MIHLImageViewer* viewer = iPlayer->Viewer();
    if (!viewer)
    {
        return KErrNotFound;
    }
    TInt frameNumber = FindFrame(viewer, aFrameNumber);
    TInt64 mediaTime = iPlayer->MediaTimeForFrame(frameNumber);
    // adjust wanted media time to get right frame (equal value returns one too small)
    mediaTime++;
    iPlayer->SetMediaTimeL(&mediaTime);
    frameNumber = viewer->AnimationFrame();
    return frameNumber;
}

TInt CMMAAnimationFramePositioningControl::SkipL(TInt aFramesToSkip)
{
    LOG(EJavaMMAPI, EInfo, "CMMAAnimationFramePositioningControl::SkipL");
    MIHLImageViewer* viewer = iPlayer->Viewer();
    if (!viewer)
    {
        return KErrNotFound;
    }

    TInt frameNumber = viewer->AnimationFrame();

    SeekL(frameNumber + aFramesToSkip);

    // Calculate number of frames skipped
    return viewer->AnimationFrame() - frameNumber;
}

void CMMAAnimationFramePositioningControl::MapFrameToTimeL(
    TInt aFrameNumber,
    TInt64* aMediaTime)
{
    LOG(EJavaMMAPI, EInfo, "CMMAAnimationFramePositioningControl::MapFrameToTimeL");
    MIHLImageViewer* viewer = iPlayer->Viewer();
    if (!viewer || (aFrameNumber < 0) ||
            (aFrameNumber >= viewer->AnimationFrameCount()))
    {
        *aMediaTime = KErrNotFound;
    }
    else
    {
        *aMediaTime = iPlayer->MediaTimeForFrame(aFrameNumber);
    }
}
TInt CMMAAnimationFramePositioningControl::MapTimeToFrameL(
    TInt64* aMediaTime)
{
    LOG(EJavaMMAPI, EInfo, "CMMAAnimationFramePositioningControl::MapTimeToFrameL");
    MIHLImageViewer* viewer = iPlayer->Viewer();
    TInt64 duration;
    iPlayer->GetDuration(&duration);
    if (!viewer || (*aMediaTime < 0) ||
            (duration == KErrNotFound))
    {
        return KErrNotFound;
    }
    return iPlayer->FindFrame(*aMediaTime);
}

TInt CMMAAnimationFramePositioningControl::FindFrame(MIHLImageViewer* aViewer, TInt aFrameNumber)
{
    TInt frame = 0;
    TInt count = aViewer->AnimationFrameCount();
    if (aFrameNumber >= count)
    {
        frame = count - 1;
    }
    else if (aFrameNumber > 0)
    {
        frame = aFrameNumber;
    }
    return frame;
}

//  END OF FILE
