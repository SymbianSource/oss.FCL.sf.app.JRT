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
* Description:  This class is used for creating camera players.
*
*/


//  INCLUDE FILES
#include <e32std.h>
#include <logger.h>

#include "cmmacameraplayerfactory.h"
#include "cmmacameraplayer.h"
#include "cmmavideocontrol.h"
#include "cmmavideorecordcontrol.h"
#include "cmmastoptimecontrol.h"
#include "cmmaplayerproperties.h"
#include "cmmammfresolver.h"

_LIT(KDefaultVideo, "video");
//is the main camera
_LIT(KDevcamZero,  "devcam0");
// the secondary camera
_LIT(KDevcamOne,  "devcam1");

_LIT(KEncodingProperty, "encoding");

_LIT(KVideoMp4MimeType, "video/mp4");
_LIT(KVideoH264MimeType, "video/H264");

CMMACameraPlayerFactory* CMMACameraPlayerFactory::NewLC()
{
    CMMACameraPlayerFactory* pFactory =
        new(ELeave) CMMACameraPlayerFactory();
    CleanupStack::PushL(pFactory);
    return pFactory;
}


CMMACameraPlayerFactory::CMMACameraPlayerFactory()
{
}


CMMACameraPlayerFactory::~CMMACameraPlayerFactory()
{
    delete iAudioSettings;
}

CMMAPlayer* CMMACameraPlayerFactory::CreatePlayerL(
    CMMAMMFResolver* aResolver)
{
    LOG( EJavaMMAPI, EInfo, "MMA::CMMACameraPlayerFactory::CreatePlayerL( aResolver )+");
    CMMACameraPlayer* player = CMMACameraPlayer::NewLC(iCameraIndex);

    /* Add VideoControl */
    CMMAVideoControl* videoControl = new(ELeave)CMMAVideoControl(player);

    CleanupStack::PushL(videoControl);
    player->AddControlL(videoControl);
    CleanupStack::Pop(videoControl);

    /* Add RecordControl */
    CMMAVideoRecordControl* videoRecordControl =
        CMMAVideoRecordControl::NewL(player,
                                     aResolver,
                                     iVideoSettings,
                                     iAudioSettings,
                                     player->CameraHandle());
    CleanupStack::PushL(videoRecordControl);
    player->AddControlL(videoRecordControl);
    // ownership transfered to player
    CleanupStack::Pop(videoRecordControl);

    // With record control view finder can't be stopped,
    // because it can not be restarted.
    player->SetViewFinderMode(EFalse);

    CleanupStack::Pop(player);
    return player;
}


void CMMACameraPlayerFactory::MediaIdsL(RArray<TUid>& aMediaIds)
{
    User::LeaveIfError(aMediaIds.Append(KUidMediaTypeVideo));
}

void CMMACameraPlayerFactory::PreparePluginSelectionParametersL(
    CMMAMMFResolver* aResolver,
    CMMFFormatSelectionParameters* aFormatSelection)
{
    // We are creating record type player
    aResolver->SetRequiredRecordFormatSupportL(*aFormatSelection);
}

CMMAPlayer* CMMACameraPlayerFactory::CreatePlayerL(const TDesC&)
{
    // record player cannot be created from conten-type, since this is used only
    // when we have also data
    return NULL;
}


CMMAPlayer* CMMACameraPlayerFactory::CreatePlayerL(const TDesC8&)
{
    // record player cannot be created with header data
    return NULL;
}

CMMAPlayer* CMMACameraPlayerFactory::CreatePlayerL(
    const TDesC& aProtocol,
    const TDesC& aMiddlePart,
    const TDesC& aParameters)
{
    LOG1( EJavaMMAPI, EInfo, "MMA::CMMACameraPlayerFactory::CreatePlayerL aParameters = %S", aParameters.Ptr());
    // check that locator is capture:://audio
    if (aProtocol != KMMACaptureProtocol)
    {
        return NULL;
    }

    iCameraIndex = KErrNotFound;

    // If the device supports multiple cameras then "capture://devcam0" is the
    // main camera pointing outwards from the user. "capture://devcam1" is the
    // secondary camera pointing e.g. towards the user. "capture://video" is
    // the default camera locator pointing to the same camera as "devcam0"
    if ((aMiddlePart == KDefaultVideo) ||
            (aMiddlePart == KDevcamZero))
    {
        // First camera
        iCameraIndex = 0;
    }
    else if (aMiddlePart == KDevcamOne)
    {
        // Second camera.
        iCameraIndex = 1;
    }
    else
    {
        // not supported type
        return NULL;
    }

    CMMAAudioSettings* audioSettings = new(ELeave) CMMAAudioSettings;
    audioSettings->iDataType = KMMFFourCCCodeNULL;
    delete iAudioSettings;
    iAudioSettings = audioSettings;

    if (aParameters.Length() == 0)
    {
        // getting defaults from validator
        iVideoSettings = TMMAParameterValidator::ValidateVideoPropertiesL(aParameters);
    }
    else
    {
        // we accept either video or mixed parameters, so properties must contain encoding 2 times for mixed and
        // 1 time for video.
        // making tmp des without first "encoding"
        TInt lengthWithoutEncoding = aParameters.Length() - KEncodingProperty().Length();
        if (lengthWithoutEncoding < 0)
        {
            User::Leave(KErrArgument);
        }
        TPtrC tmp = aParameters.Right(lengthWithoutEncoding);
        // finding second "encoding"
        TInt videoPropertiesStartPos = tmp.Find(KEncodingProperty);
        if (videoPropertiesStartPos == KErrNotFound)
        {
            // there is not another encoding, so assuming that there is only video parameters
            iVideoSettings = TMMAParameterValidator::ValidateVideoPropertiesL(aParameters);
        }
        else
        {
            // there is mixed parameters
            videoPropertiesStartPos += KEncodingProperty().Length();

            // skipping '&' char
            TPtrC audioProperties = aParameters.Left(videoPropertiesStartPos - 1);
            TPtrC videoProperties = aParameters.Mid(videoPropertiesStartPos);
            iVideoSettings = TMMAParameterValidator::ValidateVideoPropertiesL(videoProperties);

            CMMAAudioSettings* audioSettings = TMMAParameterValidator::ValidateAudioPropertiesL(audioProperties);
            delete iAudioSettings;
            iAudioSettings = audioSettings;
        }
    }

    // if wanted video capture encoding is H264, we must create player with video/mp4
    // because mmf doesn't have controller for H264
    if (iVideoSettings.iEncoding.Compare(KVideoH264MimeType) == 0)
    {
        return CMMAMMFPlayerFactory::CreatePlayerL(KVideoMp4MimeType);
    }

    // Find controller with content-type
    return CMMAMMFPlayerFactory::CreatePlayerL(iVideoSettings.iEncoding);
}

void CMMACameraPlayerFactory::GetSupportedProtocolsL(
    const TDesC& aContentType,
    CDesC16Array& aProtocolArray)
{
    if (IsSupportedContentTypeL(aContentType))
    {
        aProtocolArray.AppendL(KMMACaptureProtocol);
    }
}

//  END OF FILE
