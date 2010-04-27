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

#include "cmmavideourlplayerfactory.h"
#include "cmmavideourlplayer.h"
#include "cmmavideocontrol.h"
#include "cmmaaudiovolumecontrol.h"
#include "cmmammfratecontrol.h"

//  CONSTANTS
const TInt KDefaultGranularity = 8;
_LIT(KProtocolRtsp, "rtsp");
_LIT(KRtspMimeType, "application/sdp");

_LIT(KAudioXPnRmMimeType, "application/x-pn-realmedia");
_LIT(KAudioRmMimeType, "application/vnd.rn-realmedia");
_LIT(KAudio3gppMimeType, "audio/3gpp");
_LIT(KAudio3gpp2MimeType, "audio/3gpp2");
_LIT(KAudioAmrMimeType, "audio/amr");
_LIT(KAudioMp4MimeType, "audio/mp4");
_LIT(KAudioMp4LatmMimeType, "audio/mp4-latm");
_LIT(KAudioQcelpMimeType, "audio/qcelp");
_LIT(KAudioVndQcelpMimeType, "audio/vnd.qcelp");
_LIT(KVideo3GppMimeType, "video/3gpp");
_LIT(KVideo3Gpp2MimeType, "video/3gpp2");
_LIT(KVideoMp4MimeType, "video/mp4");

_LIT(KProtocolSeparator, "://");
_LIT(KParameterSeparator, "?");

CMMAVideoUrlPlayerFactory::CMMAVideoUrlPlayerFactory()
{
}


CMMAVideoUrlPlayerFactory* CMMAVideoUrlPlayerFactory::NewLC()
{
    CMMAVideoUrlPlayerFactory* pFactory =
        new(ELeave) CMMAVideoUrlPlayerFactory();
    CleanupStack::PushL(pFactory);
    return pFactory;
}


CMMAVideoUrlPlayerFactory::~CMMAVideoUrlPlayerFactory()
{
    delete iUrl;
}


CMMAPlayer* CMMAVideoUrlPlayerFactory::CreatePlayerL(const TDesC& /*aContentType*/)
{
    // Only creating with locator is supported
    return NULL;
}
CMMAPlayer* CMMAVideoUrlPlayerFactory::CreatePlayerL(const TDesC& aProtocol,
        const TDesC& aMiddlePart,
        const TDesC& aParameters)
{
    // Is it rtsp:// protocol
    if (aProtocol != KProtocolRtsp)
    {
        return NULL;
    }
    CMMFFormatSelectionParameters* fSelect =
        CMMFFormatSelectionParameters::NewLC();

    // combining the locator
    HBufC* remadeLocator = HBufC::NewLC(aProtocol.Length() +
                                        KProtocolSeparator().Length() +
                                        aMiddlePart.Length() +
                                        KParameterSeparator().Length() +
                                        aParameters.Length());
    TPtr loc = remadeLocator->Des();
    loc.Append(aProtocol);
    loc.Append(KProtocolSeparator);
    loc.Append(aMiddlePart);
    if (aParameters != KNullDesC)
    {
        loc.Append(KParameterSeparator);
        loc.Append(aParameters);
    }
    delete iUrl;
    iUrl = remadeLocator;
    CleanupStack::Pop(remadeLocator);

    // RTSP URLs without file extension:
    // In case of file extensionless RTSP URL a predefined mime type
    // is used to select controller. It is assumed that this mime
    // type selects the controller that is capable to play any file
    // extensionless RTSP URLs.
    // To determine whether there is a file extension, it is checked whether
    // there is a dot at all or if there is a slash after last dot in the
    // URL. If there is a slash after last dot or if there is no dot at all,
    // then there is no file extension.
    TInt dotPos = aMiddlePart.LocateReverse('.');
    TInt slashPos = aMiddlePart.LocateReverse('/');
    if ((dotPos == KErrNotFound) || (slashPos > dotPos))
    {
        HBufC8* mimeBuf = HBufC8::NewLC(KRtspMimeType().Length());
        TPtr8 mimePtr = mimeBuf->Des();
        mimePtr.Copy(KRtspMimeType());
        fSelect->SetMatchToMimeTypeL(mimePtr);
        CleanupStack::PopAndDestroy(mimeBuf);
    }
    else
    {
        // Match to file name, using only middle part of the locator
        fSelect->SetMatchToUriL(loc);
    }

    CMMAPlayer* player = CMMAMMFPlayerFactory::CreatePlayerL(fSelect);

    CleanupStack::PopAndDestroy(fSelect);
    return player;
}

CMMAPlayer* CMMAVideoUrlPlayerFactory::CreatePlayerL(
    const TDesC8& /*aHeaderData*/)
{
    // We do not try to match headerdata
    return NULL;
}


void CMMAVideoUrlPlayerFactory::GetSupportedContentTypesL(
    const TDesC& aProtocol,
    CDesC16Array& aMimeTypes)
{
    if ((aProtocol == KNullDesC) ||
            (aProtocol == KProtocolRtsp))
    {
        aMimeTypes.AppendL(KAudio3gppMimeType);
        aMimeTypes.AppendL(KAudio3gpp2MimeType);
        aMimeTypes.AppendL(KAudioAmrMimeType);
        aMimeTypes.AppendL(KAudioMp4MimeType);
        aMimeTypes.AppendL(KAudioMp4LatmMimeType);
        aMimeTypes.AppendL(KAudioQcelpMimeType);
        aMimeTypes.AppendL(KAudioVndQcelpMimeType);
        aMimeTypes.AppendL(KVideo3GppMimeType);
        aMimeTypes.AppendL(KVideo3Gpp2MimeType);
        aMimeTypes.AppendL(KVideoMp4MimeType);
        aMimeTypes.AppendL(KAudioRmMimeType);
        aMimeTypes.AppendL(KAudioXPnRmMimeType);
        // plus RTSP content types
        aMimeTypes.AppendL(KRtspMimeType);
    }
}


void CMMAVideoUrlPlayerFactory::GetSupportedProtocolsL(
    const TDesC& aContentType,
    CDesC16Array& aProtocolArray)
{
    CDesC16ArraySeg* supportedCTs
    = new(ELeave) CDesC16ArraySeg(KDefaultGranularity);
    CleanupStack::PushL(supportedCTs);
    GetSupportedContentTypesL(KNullDesC, *supportedCTs);
    TInt ignore(0);
    if ((supportedCTs->Find(aContentType, ignore) == 0) ||
            (aContentType == KNullDesC))
    {
        aProtocolArray.AppendL(KProtocolRtsp);
    }
    CleanupStack::PopAndDestroy(supportedCTs);
}

CMMAPlayer* CMMAVideoUrlPlayerFactory::CreatePlayerL(
    CMMAMMFResolver* aResolver)
{
    CMMAVideoUrlPlayer* player = CMMAVideoUrlPlayer::NewLC(aResolver,
                                 *iUrl);

    CMMAVideoControl* videoControl = new(ELeave) CMMAVideoControl(player);
    CleanupStack::PushL(videoControl);
    player->AddControlL(videoControl);
    CleanupStack::Pop(videoControl);

    CMMAAudioVolumeControl* audioVolumeControl = CMMAAudioVolumeControl::NewL(player);
    CleanupStack::PushL(audioVolumeControl);
    player->AddControlL(audioVolumeControl);
    CleanupStack::Pop(audioVolumeControl);

    CMMAMMFRateControl* rateControl = CMMAMMFRateControl::NewL(player);
    CleanupStack::PushL(rateControl);
    player->AddControlL(rateControl);
    CleanupStack::Pop(rateControl);

    CleanupStack::Pop(); // player
    return player;
}

void CMMAVideoUrlPlayerFactory::MediaIdsL(RArray<TUid>& aMediaIds)
{
    CleanupClosePushL(aMediaIds);
    User::LeaveIfError(aMediaIds.Append(KUidMediaTypeVideo));
    CleanupStack::Pop();
}

CMMFPluginSelectionParameters::TMediaIdMatchType
CMMAVideoUrlPlayerFactory::MediaIdMatchType()
{
    // We are now getting only Audio Controllers
    return CMMFPluginSelectionParameters::EAllowOtherMediaIds;
}

//  END OF FILE
