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
* Description:  This class is used for creating wav player.
*
*/


//  INCLUDE FILES
#include <jdebug.h>
#include "cmmaaudioplayerfactory.h"
#include "cmmaaudioplayer.h"
#include "cmmaaudiovolumecontrol.h"
#include "cmmastoptimecontrol.h"
#include "cmmaaudiometadatacontrol.h"
#include "cmmammfratecontrol.h"

_LIT(KMMAMP3ContentType, "audio/mp3");
_LIT(KMMAMPEGContentType, "audio/mpeg");

CMMAAudioPlayerFactory* CMMAAudioPlayerFactory::NewLC()
{
    CMMAAudioPlayerFactory* pFactory =
        new(ELeave) CMMAAudioPlayerFactory();
    CleanupStack::PushL(pFactory);
    return pFactory;
}

CMMAAudioPlayerFactory::CMMAAudioPlayerFactory()
{
}


CMMAAudioPlayerFactory::~CMMAAudioPlayerFactory()
{
}

CMMAPlayer* CMMAAudioPlayerFactory::CreatePlayerL(const TDesC& aContentType)
{
    CMMAPlayer* player = NULL;
    if (aContentType == KMMAMP3ContentType)
    {
        // If 'mp3' use 'mpeg' to create player, because audio controller
        // does not recognize mp3
        player = CMMAMMFPlayerFactory::CreatePlayerL(KMMAMPEGContentType);
    }
    else
    {
        player = CMMAMMFPlayerFactory::CreatePlayerL(aContentType);
    }
    return player;
}

CMMAPlayer* CMMAAudioPlayerFactory::CreatePlayerL(
    CMMAMMFResolver* aResolver)
{
    CMMAAudioPlayer* player = CMMAAudioPlayer::NewLC(aResolver);

    CMMAAudioVolumeControl* volumeControl = CMMAAudioVolumeControl::NewL(player);
    CleanupStack::PushL(volumeControl);
    player->AddControlL(volumeControl);
    CleanupStack::Pop(volumeControl);

    CMMAAudioMetaDataControl* metaDataControl =
        new(ELeave) CMMAAudioMetaDataControl(player->Controller());
    CleanupStack::PushL(metaDataControl);
    player->AddControlL(metaDataControl);
    CleanupStack::Pop(metaDataControl);

    CMMAStopTimeControl* stopTimeControl = CMMAStopTimeControl::NewL(player);
    CleanupStack::PushL(stopTimeControl);
    player->AddControlL(stopTimeControl);
    CleanupStack::Pop(stopTimeControl);

    CMMAMMFRateControl* rateControl = CMMAMMFRateControl::NewL(player);
    CleanupStack::PushL(rateControl);
    player->AddControlL(rateControl);
    CleanupStack::Pop(rateControl);

    CleanupStack::Pop(player);

    return player;
}

void CMMAAudioPlayerFactory::GetSupportedContentTypesL(const TDesC& aProtocol,
        CDesC16Array& aMimeTypeArray)
{
    // protocol is empty string all types must be returned.
    if (IsSupportedProtocolL(aProtocol) ||
            (aProtocol == KNullDesC))
    {
        // mp3 is not supported in the mmf, but support is
        // added in CreatePlayerL( const TDesC& aContentType ) method.
        aMimeTypeArray.AppendL(KMMAMP3ContentType);

        // all other types from mmf
        CMMAMMFPlayerFactory::GetSupportedContentTypesL(aProtocol,
                aMimeTypeArray);
    }
}

void CMMAAudioPlayerFactory::MediaIdsL(RArray<TUid>& aMediaIds)
{
    CleanupClosePushL(aMediaIds);
    User::LeaveIfError(aMediaIds.Append(KUidMediaTypeAudio));
    CleanupStack::Pop();
}

//  END OF FILE
