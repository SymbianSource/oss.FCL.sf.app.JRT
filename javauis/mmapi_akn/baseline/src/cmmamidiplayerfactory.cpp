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
* Description:  This class is used for creating MIDI player.
*
*/


//  INCLUDE FILES
#include <jdebug.h>

#include "cmmamidiplayerfactory.h"
#include "cmmamidiplayer.h"
#include "cmmamidivolumecontrol.h"
#include "cmmamidistoptimecontrol.h"
#include "cmmamidicontrol.h"
#include "cmmamiditempocontrol.h"
#include "cmmamidipitchcontrol.h"
#include "cmmammfresolver.h"
#include "cmmamidimetadatacontrol.h"


_LIT(KMMAMidi, "midi");
_LIT(KDeviceMidiContentType, "audio/midi");

CMMAMIDIPlayerFactory* CMMAMIDIPlayerFactory::NewLC()
{
    CMMAMIDIPlayerFactory* pFactory =
        new(ELeave) CMMAMIDIPlayerFactory();
    CleanupStack::PushL(pFactory);
    return pFactory;
}

CMMAMIDIPlayerFactory::CMMAMIDIPlayerFactory()
{
}


CMMAMIDIPlayerFactory::~CMMAMIDIPlayerFactory()
{
}

CMMAPlayer* CMMAMIDIPlayerFactory::CreatePlayerL(
    CMMAMMFResolver* aResolver)
{
    // if we get this far then MIDI is supported
    HBufC* contentType = aResolver->ContentType();
    HBufC* tmpFileName = aResolver->FileNameOwnership();
    TFileName fileName;
    if (tmpFileName)
    {
        fileName.Copy(tmpFileName->Des());
    }
    else
    {
        fileName = KNullDesC;
    }
    delete tmpFileName;
    CMMAPlayer* player = CreateMidiFilePlayerL(*contentType, fileName);
    return player;
}


void CMMAMIDIPlayerFactory::MediaIdsL(RArray<TUid>& aMediaIds)
{
    CleanupClosePushL(aMediaIds);
    User::LeaveIfError(aMediaIds.Append(KUidMediaTypeMidi));
    CleanupStack::Pop();
}

CMMAPlayer* CMMAMIDIPlayerFactory::CreatePlayerL(
    const TDesC& aProtocol,
    const TDesC& aMiddlePart,
    const TDesC& aProperties)
{
    CMMAPlayer* player = NULL;
    if ((aProtocol == KMMADeviceProtocol) &&
            (aMiddlePart == KMMAMidi))
    {
        // locator was device://midi
        player = CreateMidiSynthPlayerL(KDeviceMidiContentType);
    }
    else
    {
        // other locator, if it is midi file CMMAMMFPlayerFactory
        // calls CreatePlayerL( CMMAMMFResolver* aResolver ) method.
        player = CMMAMMFPlayerFactory::CreatePlayerL(aProtocol,
                 aMiddlePart,
                 aProperties);
    }
    return player;
}

void CMMAMIDIPlayerFactory::GetSupportedProtocolsL(
    const TDesC& aContentType,
    CDesC16Array& aProtocolArray)
{
    if (IsSupportedContentTypeL(aContentType))
    {
        aProtocolArray.AppendL(KMMADeviceProtocol);
    }
}

CMMAPlayer* CMMAMIDIPlayerFactory::CreateMidiFilePlayerL(const TDesC& aContentType,
        TFileName aFileName)
{
    CMMAMIDIPlayer* player = CMMAMIDIPlayer::NewLC(aContentType, aFileName);

    CMMAMIDIVolumeControl* volumeControl = CMMAMIDIVolumeControl::NewL(player);
    CleanupStack::PushL(volumeControl);
    player->AddControlL(volumeControl);
    CleanupStack::Pop(volumeControl);

    CMMAMIDIStopTimeControl* stopTimeControl = CMMAMIDIStopTimeControl::NewL(player);
    CleanupStack::PushL(stopTimeControl);
    player->AddControlL(stopTimeControl);
    CleanupStack::Pop(stopTimeControl);

    CMMAMIDIControl* midiControl = CMMAMIDIControl::NewL(player);
    CleanupStack::PushL(midiControl);
    player->AddControlL(midiControl);
    CleanupStack::Pop(midiControl);

    CMMAMIDITempoControl* tempoControl = CMMAMIDITempoControl::NewL(player);
    CleanupStack::PushL(tempoControl);
    player->AddControlL(tempoControl);
    CleanupStack::Pop(tempoControl);

    CMMAMIDIPitchControl* pitchControl = CMMAMIDIPitchControl::NewL(player);
    CleanupStack::PushL(pitchControl);
    player->AddControlL(pitchControl);
    CleanupStack::Pop(pitchControl);

    CMMAMIDIMetaDataControl* metaDataControl =
        new(ELeave) CMMAMIDIMetaDataControl(player);
    CleanupStack::PushL(metaDataControl);
    player->AddControlL(metaDataControl);
    CleanupStack::Pop(metaDataControl);

    CleanupStack::Pop(player);
    return player;
}

CMMAPlayer* CMMAMIDIPlayerFactory::CreateMidiSynthPlayerL(const TDesC& aContentType)
{
    CMMAMIDIPlayer* player = CMMAMIDIPlayer::NewLC(aContentType, KNullDesC());

    CMMAMIDIVolumeControl* volumeControl = CMMAMIDIVolumeControl::NewL(player);
    CleanupStack::PushL(volumeControl);
    player->AddControlL(volumeControl);
    CleanupStack::Pop(volumeControl);

    CMMAMIDIControl* midiControl = CMMAMIDIControl::NewL(player);
    CleanupStack::PushL(midiControl);
    player->AddControlL(midiControl);
    CleanupStack::Pop(midiControl);

    CMMAMIDIPitchControl* pitchControl = CMMAMIDIPitchControl::NewL(player);
    CleanupStack::PushL(pitchControl);
    player->AddControlL(pitchControl);
    CleanupStack::Pop(pitchControl);

    CleanupStack::Pop(player);
    return player;
}

//  END OF FILE

