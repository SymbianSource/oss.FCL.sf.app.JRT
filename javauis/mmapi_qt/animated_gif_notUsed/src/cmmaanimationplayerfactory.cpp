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
* Description:  This class is used for creating animation player.
*
*/


//  INCLUDE FILES
#include <logger.h>

#include "cmmavideocontrol.h"
#include "cmmaanimationratecontrol.h"
#include "cmmaanimationstoptimecontrol.h"
#include "cmmaanimationframepositioningcontrol.h"
#include "cmmaanimationplayer.h"
#include "cmmaanimationplayerfactory.h"

// Animation type info
_LIT(KMMAAnimationMimeTypeGIF, "image/gif");
_LIT(KMMAAnimationSuffixGIF, "gif");

// header data and length
_LIT8(KMMAAnimationGIFVersion, "GIF89a");
const TUint8 KMMAGifVersionLength = 6;

CMMAAnimationPlayerFactory* CMMAAnimationPlayerFactory::NewLC()
{
    CMMAAnimationPlayerFactory* pFactory =
        new(ELeave) CMMAAnimationPlayerFactory();
    CleanupStack::PushL(pFactory);
    return pFactory;
}

CMMAAnimationPlayerFactory::CMMAAnimationPlayerFactory()
{
}


CMMAAnimationPlayerFactory::~CMMAAnimationPlayerFactory()
{
}

// Creates new player according to a content type.
CMMAPlayer* CMMAAnimationPlayerFactory::CreatePlayerL(
    const TDesC& aContentType)
{
    CMMAPlayer* player = NULL;
    if (aContentType == KMMAAnimationMimeTypeGIF)
    {
        player = CreateAnimationPlayerL();
    }
    return player;
}

// Creates new player according to a locator
CMMAPlayer* CMMAAnimationPlayerFactory::CreatePlayerL(
    const TDesC& aProtocol,
    const TDesC& aMiddlePart,
    const TDesC& /*aParameters*/)
{
    CMMAPlayer* player = NULL;
    if (aProtocol == KMMAFileProtocol)
    {
        // filename ends to gif suffix
        if (aMiddlePart.Right(KMMAAnimationSuffixGIF().Length()) ==
                KMMAAnimationSuffixGIF())
        {
            player = CreateAnimationPlayerL(aMiddlePart);
        }
        else
        {
            // try to recognize from headerdata
            RFs fs;
            CleanupClose< RFs >::PushL(fs);
            User::LeaveIfError(fs.Connect());

            RFile file;
            CleanupClose< RFile >::PushL(file);

            User::LeaveIfError(file.Open(fs, aMiddlePart, EFileRead));

            TBuf8< KMMAGifVersionLength > header;
            User::LeaveIfError(file.Read(header));

            CleanupStack::PopAndDestroy(2);   // fs, file

            if (header == KMMAAnimationGIFVersion())
            {
                player = CreateAnimationPlayerL(aMiddlePart);
            }
        }
    }

    return (CMMAPlayer*) player;
}

// Creates new player according to a header data
CMMAPlayer* CMMAAnimationPlayerFactory::CreatePlayerL(
    const TDesC8& aHeaderData)
{
    CMMAPlayer* player = NULL;
    TPtrC8 header = aHeaderData.Left(KMMAAnimationGIFVersion().Length());
    if (header == KMMAAnimationGIFVersion())
    {
        player = CreateAnimationPlayerL();
    }
    return player;
}

// Gets player's supported content types.
void CMMAAnimationPlayerFactory::GetSupportedContentTypesL(
    const TDesC& aProtocol,
    CDesC16Array& aMimeTypeArray)
{
    if (aProtocol == KNullDesC ||
            aProtocol == KMMAHttpProtocol ||
            aProtocol == KMMAHttpsProtocol ||
            aProtocol == KMMAFileProtocol)
    {
        aMimeTypeArray.AppendL(KMMAAnimationMimeTypeGIF);
    }
}

// Gets player's supported protocols for the content type.
void CMMAAnimationPlayerFactory::GetSupportedProtocolsL(
    const TDesC& aContentType,
    CDesC16Array& aProtocolArray)
{
    if (aContentType == KNullDesC ||
            aContentType == KMMAAnimationMimeTypeGIF)
    {
        aProtocolArray.AppendL(KMMAHttpProtocol);
        aProtocolArray.AppendL(KMMAHttpsProtocol);
        aProtocolArray.AppendL(KMMAFileProtocol);
    }
}

CMMAAnimationPlayer* CMMAAnimationPlayerFactory::CreateAnimationPlayerL(const TDesC& aFileName)
{
    CMMAAnimationPlayer* player = CMMAAnimationPlayer::NewLC(aFileName);
    AddControlsL(player);
    CleanupStack::Pop(); // player
    return player;
}

CMMAAnimationPlayer* CMMAAnimationPlayerFactory::CreateAnimationPlayerL()
{
    CMMAAnimationPlayer* player = CMMAAnimationPlayer::NewLC();
    AddControlsL(player);
    CleanupStack::Pop(); // player
    return player;
}

void CMMAAnimationPlayerFactory::AddControlsL(CMMAAnimationPlayer* aPlayer)
{
    CMMAVideoControl* videoControl = new(ELeave) CMMAVideoControl(aPlayer);
    CleanupStack::PushL(videoControl);
    aPlayer->AddControlL(videoControl);
    CleanupStack::Pop(videoControl);

    CMMAAnimationStopTimeControl* stopTimeControl =
        CMMAAnimationStopTimeControl::NewL(aPlayer);
    CleanupStack::PushL(stopTimeControl);
    aPlayer->AddControlL(stopTimeControl);
    CleanupStack::Pop(stopTimeControl);

    // Own RateControl
    CMMAAnimationRateControl* rateControl =
        CMMAAnimationRateControl::NewL(aPlayer);
    CleanupStack::PushL(rateControl);
    aPlayer->AddControlL(rateControl);
    CleanupStack::Pop(rateControl);

    // FramePositioningControl is only supported for file locator
    if (aPlayer->IsFilePlayer())
    {
        CMMAAnimationFramePositioningControl* animationFramePositioningControl =
            CMMAAnimationFramePositioningControl::NewL(aPlayer);
        CleanupStack::PushL(animationFramePositioningControl);
        aPlayer->AddControlL(animationFramePositioningControl);
        CleanupStack::Pop(animationFramePositioningControl);
    }

}
//  END OF FILE
