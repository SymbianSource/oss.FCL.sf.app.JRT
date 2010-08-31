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
* Description:  This class is used for setting volume to audio player
*
*/


#include <logger.h>
#include "cmmaaudiovolumecontrol.h"
#include "cmmaaudioplayer.h"


CMMAAudioVolumeControl::CMMAAudioVolumeControl(CMMAAudioPlayer* aPlayer)
        : CMMAVolumeControl(aPlayer),
        iAudioPlayDeviceCommands(aPlayer->Controller())
{
}

void CMMAAudioVolumeControl::ConstructL()
{
    ConstructBaseL();
}

EXPORT_C CMMAAudioVolumeControl* CMMAAudioVolumeControl::NewL(CMMAAudioPlayer* aPlayer)
{
    CMMAAudioVolumeControl* self = new(ELeave)CMMAAudioVolumeControl(aPlayer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}


void CMMAAudioVolumeControl::DoSetLevelL(TInt aLevel)
{
    TInt maxVolume = 0;
    User::LeaveIfError(iAudioPlayDeviceCommands.GetMaxVolume(maxVolume));
    // aLevel is the desired volume in range 0..100
    User::LeaveIfError(iAudioPlayDeviceCommands.SetVolume(
                           aLevel * maxVolume / KMMAVolumeMaxLevel));
}

TInt CMMAAudioVolumeControl::DoGetLevelL()
{
    TInt maxVolume = 0;
    User::LeaveIfError(iAudioPlayDeviceCommands.GetMaxVolume(maxVolume));
    __ASSERT_DEBUG(maxVolume != 0, User::Invariant());
    TInt volume = 0;
    User::LeaveIfError(iAudioPlayDeviceCommands.GetVolume(volume));
    // result is in range 0..100
    return (volume * KMMAVolumeMaxLevel / maxVolume);
}

//  END OF FILE
