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
* Description:  This class is used for creating DRM v2 player.
*
*/


//  INCLUDE FILES
#include <logger.h>

#include <DRMCommon.h>
#include <DRMHelper.h>
#include <caf/stringattributeset.h>

#include "cmmadrmplayerfactory.h"
#include "cmmadrmaudioplayer.h"
#include "cmmadrmvolumecontrol.h"
#include "cmmastoptimecontrol.h"
#include "cmmadrmmetadatacontrol.h"
#include "cmmadrmratecontrol.h"
#include "cmmavideoplayer.h"

_LIT(KMMAAudio, "audio");
// this (get from caf) cannot be recognized is it audio or video
_LIT(KMMARMCafMimeType, "application/vnd.rn-realmedia");
// drm v2 file suffix
_LIT(KMMADRMv2SuffixODF, "odf");

// ---------------------------------------------------------------------------
// CMMADRMPlayerFactory::NewLC
// ---------------------------------------------------------------------------
//
CMMADRMPlayerFactory* CMMADRMPlayerFactory::NewLC(CMMAVideoPlayerFactory* aVideoPlayerFactory)
{
    CMMADRMPlayerFactory* pFactory =
        new(ELeave) CMMADRMPlayerFactory(aVideoPlayerFactory);
    CleanupStack::PushL(pFactory);
    return pFactory;
}

// ---------------------------------------------------------------------------
// CMMADRMPlayerFactory::CMMADRMPlayerFactory
// ---------------------------------------------------------------------------
//
CMMADRMPlayerFactory::CMMADRMPlayerFactory(CMMAVideoPlayerFactory* aVideoPlayerFactory)
        :iVideoPlayerFactory(aVideoPlayerFactory)
{
}

// ---------------------------------------------------------------------------
// CMMADRMPlayerFactory::~CMMADRMPlayerFactory
// ---------------------------------------------------------------------------
//
CMMADRMPlayerFactory::~CMMADRMPlayerFactory()
{
}

// ---------------------------------------------------------------------------
// CMMADRMPlayerFactory::CreatePlayerL
// ---------------------------------------------------------------------------
//
CMMAPlayer* CMMADRMPlayerFactory::CreatePlayerL(const TDesC& /*aContentType*/)
{
    // only file is supported
    return NULL;
}

// ---------------------------------------------------------------------------
// CMMADRMPlayerFactory::CreatePlayerL
// ---------------------------------------------------------------------------
//
CMMAPlayer* CMMADRMPlayerFactory::CreatePlayerL(
    const TDesC& aProtocol,
    const TDesC& aMiddlePart,
    const TDesC& /*aProperties*/)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMADRMPlayerFactory::CreatePlayerL +");
    CMMAPlayer* player = NULL;
    if (aProtocol == KMMAFileProtocol)
    {
        // we are most likely going to play this file
        ContentAccess::TIntent intent = ContentAccess::EPlay;

        ContentAccess::CContent* contentObj = ContentAccess::CContent::NewL(aMiddlePart);
        CleanupStack::PushL(contentObj);

        ContentAccess::CData* dataObj = NULL;
        TRAPD(openContentErr, dataObj = contentObj->OpenContentL(intent));
        CleanupStack::PushL(dataObj);

        ELOG1(EJavaMMAPI, "MMA::CMMADRMPlayerFactory::CreatePlayerL openContentErr: %d", openContentErr);
        if (KErrCA_LowerLimit <= openContentErr && openContentErr <= KErrCA_UpperLimit)
        {
            // handle error, possible update rights
            UpdateRightsL(openContentErr, aMiddlePart);
            User::Leave(openContentErr);
        }
        else
        {
            User::LeaveIfError(openContentErr);
        }

        ContentAccess::RStringAttributeSet stringAttributeSet;
        CleanupClosePushL(stringAttributeSet);
        stringAttributeSet.AddL(ContentAccess::EMimeType);

        User::LeaveIfError(dataObj->GetStringAttributeSet(stringAttributeSet));

        TBuf<KMaxName> mimeType;
        TInt err = stringAttributeSet.GetValue(ContentAccess::EMimeType, mimeType);
        if (err == KErrNone)
        {
            LOG1(EJavaMMAPI, EInfo, "MMA::CMMADRMPlayerFactory::CreatePlayerL, no err, mime type = %S", mimeType.PtrZ());
            // we use 16bit mimeType
            HBufC* mimeTypeBuf = HBufC::NewLC(mimeType.Length());
            mimeTypeBuf->Des().Copy(mimeType);

            if (((mimeTypeBuf->Length() >= KMMAAudio().Length()) &&
                    (mimeTypeBuf->Left(KMMAAudio().Length()) == KMMAAudio)) ||
                    ((KErrNone == mimeTypeBuf->Compare(KMMARMCafMimeType)) &&
                     (aMiddlePart.Right(KMMADRMv2SuffixODF().Length()) ==
                      KMMADRMv2SuffixODF())))
            {
                // if content-type starts with "audio" or
                // if it's KMMARMCafMimeType and file suffix is ODF
                // then create audio player
                player = CreateAudioPlayerL(*mimeTypeBuf, aMiddlePart);
            }
            else
            {
                // otherwise try video player
                player = CreateVideoPlayerL(*mimeTypeBuf, aMiddlePart);
            }
            CleanupStack::PopAndDestroy(mimeTypeBuf);
        }
        else
        {
            ELOG1(EJavaMMAPI, "MMA::CMMADRMPlayerFactory::CreatePlayerL get mime err: %d", err);
            LOG1(EJavaMMAPI, EInfo, "MMA::CMMADRMPlayerFactory::CreatePlayerL mime type = %S", mimeType.PtrZ());
            User::Leave(err);
        }
        CleanupStack::PopAndDestroy(3); //dataObj, contentObj, stringAttributeSet
    }
    LOG(EJavaMMAPI, EInfo, "MMA::CMMADRMPlayerFactory::CreatePlayerL -");
    return player;
}

// ---------------------------------------------------------------------------
// CMMADRMPlayerFactory::CreatePlayerL
// ---------------------------------------------------------------------------
//
CMMAPlayer* CMMADRMPlayerFactory::CreatePlayerL(const TDesC8& /*aHeaderData*/)
{
    // only file is supported
    return NULL;
}

// ---------------------------------------------------------------------------
// CMMADRMPlayerFactory::GetSupportedContentTypesL
// ---------------------------------------------------------------------------
//
void CMMADRMPlayerFactory::GetSupportedContentTypesL(const TDesC& /*aProtocol*/,
        CDesC16Array& /*aMimeTypeArray*/)
{
    // DRM Supports same content-types as others, no need to add
}

// ---------------------------------------------------------------------------
// CMMADRMPlayerFactory::GetSupportedProtocolsL
// ---------------------------------------------------------------------------
//
void CMMADRMPlayerFactory::GetSupportedProtocolsL(
    const TDesC& /*aContentType*/,
    CDesC16Array& /*aProtocolArray*/)
{
    // DRM Supports same protocols as others, no need to add
}

// ---------------------------------------------------------------------------
// CMMADRMPlayerFactory::UpdateRightsL
// ---------------------------------------------------------------------------
//
void CMMADRMPlayerFactory::UpdateRightsL(TInt aError,
        const TDesC& aFileName)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMADRMPlayerFactory::UpdateRightsL +");
    CDRMHelper* helper = CDRMHelper::NewLC();
    TInt code = helper->HandleErrorL(aError, aFileName);
    if (code == 0)
    {
        User::Leave(aError);
    }
    CleanupStack::PopAndDestroy(helper);
}

// ---------------------------------------------------------------------------
// CMMADRMPlayerFactory::CreateAudioPlayerL
// ---------------------------------------------------------------------------
//
CMMAPlayer* CMMADRMPlayerFactory::CreateAudioPlayerL(const TDesC& aContentType,
        const TDesC& aFileName)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMADRMPlayerFactory::CreateAudioPlayerL +");
    CMMADRMAudioPlayer* player = CMMADRMAudioPlayer::NewLC(aContentType,
                                 aFileName);

    CMMADRMVolumeControl* volumeControl = CMMADRMVolumeControl::NewL(player);
    CleanupStack::PushL(volumeControl);
    player->AddControlL(volumeControl);
    CleanupStack::Pop(volumeControl);

    CMMAStopTimeControl* stopTimeControl = CMMAStopTimeControl::NewL(player);
    CleanupStack::PushL(stopTimeControl);
    player->AddControlL(stopTimeControl);
    CleanupStack::Pop(stopTimeControl);

    CMMADRMMetaDataControl* metaDataControl =
        new(ELeave)CMMADRMMetaDataControl(player);
    CleanupStack::PushL(metaDataControl);
    player->AddControlL(metaDataControl);
    CleanupStack::Pop(metaDataControl);

    CMMADRMRateControl* drmRateControl = CMMADRMRateControl::NewL(player);
    CleanupStack::PushL(drmRateControl);
    player->AddControlL(drmRateControl);
    CleanupStack::Pop(drmRateControl);

    CleanupStack::Pop(player);
    return player;
}

// ---------------------------------------------------------------------------
// CMMADRMPlayerFactory::CreateVideoPlayerL
// ---------------------------------------------------------------------------
//
CMMAPlayer* CMMADRMPlayerFactory::CreateVideoPlayerL(const TDesC& aContentType,
        const TDesC& aFileName)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMADRMPlayerFactory::CreateVideoPlayerL +");
    CMMAPlayer* player =
        iVideoPlayerFactory->CreatePlayerWithFileL(aContentType, &aFileName);

    return player;
}

//  END OF FILE
