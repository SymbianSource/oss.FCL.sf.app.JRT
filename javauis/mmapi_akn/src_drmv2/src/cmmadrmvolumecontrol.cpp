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
* Description:  This class is used for setting volume to DRMv2 audio player
*
*/


#include <jdebug.h>
#include "cmmadrmvolumecontrol.h"
#include "cmmadrmaudioplayer.h"

CMMADRMVolumeControl::CMMADRMVolumeControl(CMMADRMAudioPlayer* aPlayer)
        : CMMAVolumeControl(aPlayer)
{
    iPlayer = aPlayer;
}

void CMMADRMVolumeControl::ConstructL()
{
    ConstructBaseL();
}
CMMADRMVolumeControl* CMMADRMVolumeControl::NewL(CMMADRMAudioPlayer* aPlayer)
{
    CMMADRMVolumeControl* self = new(ELeave)CMMADRMVolumeControl(aPlayer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

void CMMADRMVolumeControl::DoSetLevelL(TInt aLevel)
{
    CDrmPlayerUtility* utility = iPlayer->DRMUtility();
    TInt maxVolume = utility->MaxVolume();
    utility->SetVolume(aLevel * maxVolume / KMMAVolumeMaxLevel);
}

TInt CMMADRMVolumeControl::DoGetLevelL()
{
    CDrmPlayerUtility* utility = iPlayer->DRMUtility();
    TInt maxVolume = utility->MaxVolume();
    TInt volume = 0;
    User::LeaveIfError(utility->GetVolume(volume));
    // result is in range 0..100
    return (volume * KMMAVolumeMaxLevel / maxVolume);
}

//  END OF FILE
