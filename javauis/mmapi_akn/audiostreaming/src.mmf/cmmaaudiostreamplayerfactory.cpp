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
* Description:  This class is used for creating streaming audio player.
*
*/


//  INCLUDE FILES
#include <jdebug.h>
#include "CMMAAudioStreamPlayerFactory.h"
#include "CMMAAudioStreamPlayer.h"
#include "CMMAAudioVolumeControl.h"
#include "CMMAStopTimeControl.h"
#include "CMMAMMFResolver.h"
#include "CMMAAudioMetaDataControl.h"
#include "CMMAAudioStreamRateControl.h"

// CONSTANTS
_LIT(KMMAAMRMimetype, "audio/amr");
_LIT(KMMAAMRWBMimetype, "audio/amr-wb");

CMMAAudioStreamPlayerFactory* CMMAAudioStreamPlayerFactory::NewLC()
{
    CMMAAudioStreamPlayerFactory* pFactory =
        new(ELeave) CMMAAudioStreamPlayerFactory();
    CleanupStack::PushL(pFactory);
    return pFactory;
}

CMMAAudioStreamPlayerFactory::CMMAAudioStreamPlayerFactory()
{
}


CMMAAudioStreamPlayerFactory::~CMMAAudioStreamPlayerFactory()
{
}

CMMAPlayer* CMMAAudioStreamPlayerFactory::CreatePlayerL(
    const TDesC& /*aContentType*/)
{
    // only http protocol is supported
    return NULL;
}

CMMAPlayer* CMMAAudioStreamPlayerFactory::CreatePlayerL(
    const TDesC& aProtocol,
    const TDesC& aMiddlePart,
    const TDesC& aParameters)
{
    // only http protocol is supported
    if (aProtocol != KMMAHttpProtocol)
    {
        return NULL;
    }

    return CMMAMMFPlayerFactory::CreatePlayerL(aProtocol,
            aMiddlePart,
            aParameters);
}

CMMAPlayer* CMMAAudioStreamPlayerFactory::CreatePlayerL(
    const TDesC8& /*aHeaderData*/)
{
    // only http protocol is supported
    return NULL;
}

CMMAPlayer* CMMAAudioStreamPlayerFactory::CreatePlayerL(
    CMMAMMFResolver* aResolver)
{
    // only amr streaming is supported
    HBufC* contentType = aResolver->ContentType();
    if (!contentType ||
            ((contentType->Des() != KMMAAMRMimetype) &&
             (contentType->Des() != KMMAAMRWBMimetype)))
    {
        return NULL;
    }

    CMMAAudioStreamPlayer* player = CMMAAudioStreamPlayer::NewLC(aResolver);

    CMMAAudioVolumeControl* volumeControl = CMMAAudioVolumeControl::NewL(player);
    CleanupStack::PushL(volumeControl);
    player->AddControlL(volumeControl);
    CleanupStack::Pop(volumeControl);

// Amr stream player should support METADATACONTROL so adding this control to the player
    CMMAAudioMetaDataControl* metaDataControl =
        new(ELeave) CMMAAudioMetaDataControl(player->Controller());
    CleanupStack::PushL(metaDataControl);
    player->AddControlL(metaDataControl);
    CleanupStack::Pop(metaDataControl);

    CMMAStopTimeControl* stopTimeControl = CMMAStopTimeControl::NewL(player);
    CleanupStack::PushL(stopTimeControl);
    player->AddControlL(stopTimeControl);
    CleanupStack::Pop(stopTimeControl);

    CMMAAudioStreamRateControl* rateControl = CMMAAudioStreamRateControl::NewL(player);
    CleanupStack::PushL(rateControl);
    player->AddControlL(rateControl);
    CleanupStack::Pop(rateControl);

    CleanupStack::Pop(player);

    return player;
}

void CMMAAudioStreamPlayerFactory::GetSupportedContentTypesL(const TDesC& /*aProtocol*/,
        CDesC16Array& /*aMimeTypeArray*/)
{
    // streaming player does not add new content types
}

void CMMAAudioStreamPlayerFactory::MediaIdsL(RArray<TUid>& aMediaIds)
{
    CleanupClosePushL(aMediaIds);
    User::LeaveIfError(aMediaIds.Append(KUidMediaTypeAudio));
    CleanupStack::Pop();
}

//  END OF FILE
