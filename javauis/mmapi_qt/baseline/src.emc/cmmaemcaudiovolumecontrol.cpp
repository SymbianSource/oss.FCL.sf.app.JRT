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
#include "StreamControl.h"
#include "EffectControl.h"
#include "MMControlFactory.h"
#include "cmmaemcaudiovolumecontrol.h"
#include "cmmaemcaudioplayer.h"

using multimedia ::KVolumeEffectControl;
using multimedia ::MStreamControl;
using multimedia ::MEffectControl;
using multimedia ::CMultimediaFactory;

EXPORT_C CMMAEMCAudioVolumeControl* CMMAEMCAudioVolumeControl::NewL(CMMAEMCAudioPlayer& aPlayer)
{
    CMMAEMCAudioVolumeControl* self = new(ELeave)CMMAEMCAudioVolumeControl(aPlayer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

EXPORT_C CMMAEMCAudioVolumeControl::CMMAEMCAudioVolumeControl(CMMAEMCAudioPlayer& aPlayer)
        : CMMAVolumeControl(&aPlayer), iPlayer(aPlayer)
{
}

void CMMAEMCAudioVolumeControl::ConstructL()
{
    ConstructBaseL();
    MStreamControl* streamControl = iPlayer.StreamControl();
    CMultimediaFactory* mmFactory = iPlayer.MMFactory();

    MEffectControl* temp(NULL);
    User::LeaveIfError(mmFactory->CreateEffectControl(KVolumeEffectControl, temp));
    iVolCntrl = static_cast<MVolumeControl*>(temp);
    User::LeaveIfError(streamControl->AddEffect(*iVolCntrl));
}

CMMAEMCAudioVolumeControl::~CMMAEMCAudioVolumeControl()
{
    MStreamControl* streamControl = iPlayer.StreamControl();
    CMultimediaFactory* mmFactory = iPlayer.MMFactory();

    if ((NULL != streamControl) && (NULL != iVolCntrl))
    {
        streamControl->RemoveEffect(*iVolCntrl);
    }

    MEffectControl* temp = static_cast<MEffectControl*>(iVolCntrl);

    if (NULL != mmFactory)
    {
        mmFactory->DeleteEffectControl(temp);
    }
}

EXPORT_C void CMMAEMCAudioVolumeControl::DoSetLevelL(TInt aLevel)
{
    TInt maxVolume = 0;
    User::LeaveIfError(iVolCntrl->GetMaxVolume(maxVolume));
    // aLevel is the desired volume in range 0..100
    TInt volLevel = aLevel * maxVolume / KMMAVolumeMaxLevel;
    User::LeaveIfError(iVolCntrl->SetVolume(volLevel));
    User::LeaveIfError(iVolCntrl->Apply());
}

EXPORT_C TInt CMMAEMCAudioVolumeControl::DoGetLevelL()
{
    TInt maxVolume = 0;
    User::LeaveIfError(iVolCntrl->GetMaxVolume(maxVolume));
    __ASSERT_DEBUG(maxVolume != 0, User::Invariant());
    TInt volume = 0;
    User::LeaveIfError(iVolCntrl->GetVolume(volume));
    // result is in range 0..100
    return (volume * KMMAVolumeMaxLevel / maxVolume);
}

//  END OF FILE
