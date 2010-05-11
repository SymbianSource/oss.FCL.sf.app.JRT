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
#include <logger.h>
#include "cmmaaudiostreamplayerfactory.h"
#include "cmmaaudiostreamplayer.h"
#include "cmmaemcaudiovolumecontrol.h"
#include "cmmastoptimecontrol.h"
#include <MetaDataUtility.h>

#include "cmmaaudiostreammetadatacontrol.h"
#include "cmmaemcresolver.h"

#ifdef __MMA_GENERIC_RATE_CONTROL__
#include "cmmaaudiostreamratecontrol.h"
#endif // __MMA_GENERIC_RATE_CONTROL__

// CONSTANTS
_LIT(KMimetypeAMR,"audio/amr");
_LIT(KMimetypeAMRWB,"audio/amr-wb");
_LIT(KMimetypeAAC, "audio/aac");
_LIT(KMimetype3GPP, "audio/3gpp");
_LIT(KMimetype3GPP2, "audio/3gpp2");
_LIT(KMimetypeMPEG, "audio/mpeg");
_LIT(KMimetypeMP4, "audio/mp4");
_LIT(KMimetypeXMSWMA, "audio/x-ms-wma");
_LIT(KMimetypeRM, "audio/x-pn-realaudio");

CMMAAudioStreamPlayerFactory* CMMAAudioStreamPlayerFactory::NewLC()
{
    CMMAAudioStreamPlayerFactory* pFactory =
        new(ELeave) CMMAAudioStreamPlayerFactory();
    CleanupStack::PushL(pFactory);
    return pFactory;
}

CMMAAudioStreamPlayerFactory::CMMAAudioStreamPlayerFactory()
{
    iHeaderData = NULL;
}


CMMAAudioStreamPlayerFactory::~CMMAAudioStreamPlayerFactory()
{
}

CMMAPlayer* CMMAAudioStreamPlayerFactory::CreatePlayerL(
    const TDesC& /*aContentType*/)
{
    // only http/https protocol is supported
    return NULL;
}

CMMAPlayer* CMMAAudioStreamPlayerFactory::CreatePlayerL(
    const TDesC& aProtocol,
    const TDesC& aMiddlePart,
    const TDesC& aParameters)
{
    // only http and https protocols are supported
    if ((aProtocol != KMMAHttpProtocol) && (aProtocol != KMMAHttpsProtocol))
    {
        return NULL;
    }

    return CMMAEMCPlayerFactory::CreatePlayerL(aProtocol,
            aMiddlePart,
            aParameters);
}

CMMAPlayer* CMMAAudioStreamPlayerFactory::CreatePlayerL(
    const TDesC8& /*aHeaderData*/)
{
    // only http/https protocol is supported
    return NULL;
}

CMMAPlayer* CMMAAudioStreamPlayerFactory::CreatePlayerL(
    CMMAEMCResolver* aResolver)
{
    aResolver->SetSourceInfoL(iHeaderData);

    HBufC* contentType = aResolver->ContentType();
    if (!contentType ||
            ((contentType->Des() != KMimetypeAMR) &&
             (contentType->Des() != KMimetypeAMRWB) &&
             (contentType->Des() != KMimetypeAAC) &&
             (contentType->Des() != KMimetypeXMSWMA) &&
             (contentType->Des() != KMimetypeRM) &&
             (contentType->Des() != KMimetypeMPEG) &&
             (contentType->Des() != KMimetype3GPP2) &&
             (contentType->Des() != KMimetype3GPP) &&
             (contentType->Des() != KMimetypeMP4)))
    {
        return NULL;
    }

    CMMAAudioStreamPlayer* player = CMMAAudioStreamPlayer::NewLC(aResolver);

    CMMAEMCAudioVolumeControl* volumeControl = CMMAEMCAudioVolumeControl::NewL(*player);
    CleanupStack::PushL(volumeControl);
    player->AddControlL(volumeControl);
    CleanupStack::Pop(volumeControl);

    CMMAStopTimeControl* stopTimeControl = CMMAStopTimeControl::NewL(player);
    CleanupStack::PushL(stopTimeControl);
    player->AddControlL(stopTimeControl);
    CleanupStack::Pop(stopTimeControl);

#ifdef __MMA_GENERIC_RATE_CONTROL__
    CMMAAudioStreamRateControl* rateControl = CMMAAudioStreamRateControl::NewL(player);
    CleanupStack::PushL(rateControl);
    player->AddControlL(rateControl);
    CleanupStack::Pop(rateControl);
#endif // __MMA_GENERIC_RATE_CONTROL__

    CMMAAudioStreamMetaDataControl* metadatacontrol =
        new(ELeave) CMMAAudioStreamMetaDataControl(player->MetaDataUtilityOwnership());
    CleanupStack::PushL(metadatacontrol);
    player->AddControlL(metadatacontrol);
    CleanupStack::Pop(metadatacontrol);

    // delete isourceinfo;
    // no longer required, can be destroyed here
    if (iHeaderData)
    {
        delete iHeaderData;
        iHeaderData = NULL;
    }

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
    User::LeaveIfError(aMediaIds.Append(KUidMediaTypeAudio));
}

void CMMAAudioStreamPlayerFactory::SetSourceInfoL(const TUint8* aHeader, TInt aLength)
{
    iHeaderData = HBufC8::NewL(aLength);
    iHeaderData->Des().Copy(aHeader, aLength);
}

//  END OF FILE
