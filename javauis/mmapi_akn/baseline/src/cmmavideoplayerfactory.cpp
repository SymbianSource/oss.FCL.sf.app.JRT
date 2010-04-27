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
* Description:  This class is used for creating video player.
*
*/


//  INCLUDE FILES
#include <jdebug.h>

#include "cmmavideoplayerfactory.h"
#include "cmmastoptimecontrol.h"
#include "cmmavideoplayer.h"
#include "cmmavideocontrol.h"
#include "cmmaaudiovolumecontrol.h"
#include "cmmammfresolver.h"
#include "cmmavideoframepositioningcontrol.h"
#include "cmmammfratecontrol.h"


// audio content type
_LIT(KMMAAudio, "audio");
// Length of KMMAAudio descriptor
const TInt KMMAAudioLength = 5;

CMMAVideoPlayerFactory* CMMAVideoPlayerFactory::NewLC()
{
    CMMAVideoPlayerFactory* pFactory =
        new(ELeave) CMMAVideoPlayerFactory();
    CleanupStack::PushL(pFactory);
    return pFactory;
}

CMMAVideoPlayerFactory::CMMAVideoPlayerFactory()
{
}


CMMAVideoPlayerFactory::~CMMAVideoPlayerFactory()
{
}

#ifdef RD_JAVA_OMA_DRM_V2
CMMAPlayer* CMMAVideoPlayerFactory::CreatePlayerWithFileL(const TDesC& aContentType,
        const TDesC* aFileName)
{
    iFileName = aFileName;
    return CMMAMMFPlayerFactory::CreatePlayerL(aContentType);
}
#endif // RD_JAVA_OMA_DRM_V2

CMMAPlayer* CMMAVideoPlayerFactory::CreatePlayerL(
    CMMAMMFResolver* aResolver)
{
    HBufC* cType = aResolver->ContentType();

    // Video player may not accept audio content types.
    if (cType &&
            cType->Length() >= KMMAAudioLength &&
            (aResolver->ContentType()->Mid(0, KMMAAudioLength) == KMMAAudio))
    {
        return NULL;
    }

#ifdef RD_JAVA_OMA_DRM_V2
    if (iFileName)
    {
        aResolver->SetFileNameL(iFileName);
        iFileName = NULL;
    }
#endif // RD_JAVA_OMA_DRM_V2

    CMMAVideoPlayer* player = CMMAVideoPlayer::NewLC(aResolver);

    CMMAVideoControl* videoControl = new(ELeave) CMMAVideoControl(player);
    CleanupStack::PushL(videoControl);
    player->AddControlL(videoControl);
    CleanupStack::Pop(videoControl);

    CMMAAudioVolumeControl* audioVolumeControl = CMMAAudioVolumeControl::NewL(player);
    CleanupStack::PushL(audioVolumeControl);
    player->AddControlL(audioVolumeControl);
    CleanupStack::Pop(audioVolumeControl);

    CMMAStopTimeControl* stopTimeControl = CMMAStopTimeControl::NewL(player);
    CleanupStack::PushL(stopTimeControl);
    player->AddControlL(stopTimeControl);
    CleanupStack::Pop(stopTimeControl);

    // FramePositioningControl is only supported for file locator
    if (player->IsFilePlayer())
    {
        CMMAVideoFramePositioningControl* framePositioningControl =
            CMMAVideoFramePositioningControl::NewL(player);
        CleanupStack::PushL(framePositioningControl);
        player->AddControlL(framePositioningControl);
        CleanupStack::Pop(framePositioningControl);
    }

    CMMAMMFRateControl* rateControl = CMMAMMFRateControl::NewL(player);
    CleanupStack::PushL(rateControl);
    player->AddControlL(rateControl);
    CleanupStack::Pop(rateControl);


    CleanupStack::Pop(); // player
    return player;
}


void CMMAVideoPlayerFactory::MediaIdsL(RArray<TUid>& aMediaIds)
{
    CleanupClosePushL(aMediaIds);
    User::LeaveIfError(aMediaIds.Append(KUidMediaTypeVideo));
    User::LeaveIfError(aMediaIds.Append(KUidMediaTypeAudio));
    CleanupStack::Pop();

}
//  END OF FILE
