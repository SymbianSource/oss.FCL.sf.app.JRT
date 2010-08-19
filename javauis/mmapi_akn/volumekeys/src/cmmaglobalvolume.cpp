/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used for global volume
*
*/


// CLASS HEADER
#include "cmmaglobalvolume.h"

// INTERNAL INCLUDES
#include "cmmaplayer.h"
#include "cmmaforeground.h"
#include "cmmavolumekeyslistener.h"
#include "mmaprivatecrkeys.h"

// EXTERNAL INCLUDES
#include <centralrepository.h>
#include <jdebug.h>

// UNNAMED LOCAL NAMESPACE
namespace
{
// Volume level step size
const TInt KMMAVolumeLevelStep = 10;
// Error when setting new volume level for a control
_LIT(KMMAGlobalVolumeSetError, "Can't set volume level");
}


// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CMMAGlobalVolume::NewL
// ---------------------------------------------------------------------------
//
CMMAGlobalVolume* CMMAGlobalVolume::NewL(CMMAForeground* aForeground)
{
    CMMAGlobalVolume* self = new(ELeave) CMMAGlobalVolume(aForeground);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CMMAGlobalVolume::~CMMAGlobalVolume
// ---------------------------------------------------------------------------
//
CMMAGlobalVolume::~CMMAGlobalVolume()
{
    delete iVolumeKeysListener;
    delete iSettingsStore;
    delete iForeground;
    iControlList.Close();
}

// ---------------------------------------------------------------------------
// CMMAGlobalVolume::AddPlayerL
// ---------------------------------------------------------------------------
//
void CMMAGlobalVolume::AddPlayerL(CMMAPlayer* aPlayer)
{
    DEBUG("CMMAGlobalVolume: AddPlayerL +");
    // Find if the player has a volume control
    CMMAVolumeControl* control = FindVolumeControl(aPlayer);

    // Ignore adding new player if it does not support volume control
    if (control)
    {
        TMMAPlayerHolder playerHolder;
        // Create new volume control and level index pair
        playerHolder.iVolumeControl = control;
        playerHolder.iVolumeIndex = control->AddLevelL();
        DEBUG_INT2("CMMAGlobalVolume: AddPlayerL : iVolumeIndex = %d and iLevel = %d",playerHolder.iVolumeIndex,iLevel);
        playerHolder.iPlayer = aPlayer;
        // Set current volume level for the control
        control->SetVolumeLevelL(playerHolder.iVolumeIndex, iLevel);
        // To set the initial Global Volume for each player so that 
        // it will not post any event during the transition to realized state if there 
        // is no change in the global volume between player creation and realization.
        control->InitializeGlobalVolumeLevel(iLevel);
        // Add created pair to the control list
        iControlList.AppendL(playerHolder);
    }
    
    DEBUG("CMMAGlobalVolume: AddPlayerL -");
}

// ---------------------------------------------------------------------------
// CMMAGlobalVolume::RemovePlayer
// ---------------------------------------------------------------------------
//
void CMMAGlobalVolume::RemovePlayer(CMMAPlayer* aPlayer)
{
    DEBUG("CMMAGlobalVolume: RemovePlayer +");
    // Find if the player has a volume control
    CMMAVolumeControl* control = FindVolumeControl(aPlayer);

    // Ignore adding new player if it does not support volume control
    if (control)
    {
        // Check that if this type of volume control can be found from
        // the control list and remove it
        TInt count(iControlList.Count());
        DEBUG_INT("CMMAGlobalVolume: RemovePlayer :No of CMMAVolumeControl instances = %d",count);
                
        for (TInt i(0); i < count; i++)
        {
            const TMMAPlayerHolder& holder = iControlList[ i ];
            if (control == holder.iVolumeControl)
            {
            DEBUG("CMMAGlobalVolume: RemovePlayer  removing the instance of CMMAVolumeControl from the list");
                iControlList.Remove(i);
                break;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// CMMAGlobalVolume::VolumeUp
// ---------------------------------------------------------------------------
//
void CMMAGlobalVolume::VolumeUp()
{
    DEBUG_INT("THREADID = %d : CMMAGlobalVolume: VolumeUp: +", RThread().Id());
    DEBUG_INT("CMMAGlobalVolume: VolumeUp : iLevel = %d", iLevel);
    // Adjust volume if midlet is in foreground and the volume level value
    // is not too high, in this case it cannot be set over KMMAVolumeMaxLevel
    if (iForeground->IsForeground() && (iLevel < KMMAVolumeMaxLevel))
    {
        DEBUG("CMMAGlobalVolume: VolumeUp: Volume up");
        // Check that the current volume level is not increased too much
        TInt level =
            iLevel > (KMMAVolumeMaxLevel - KMMAVolumeLevelStep) ?
            KMMAVolumeMaxLevel - iLevel : iLevel + KMMAVolumeLevelStep;
        // Increase level by new value
        DEBUG_INT("CMMAGlobalVolume: VolumeUp : effective volume to be set to player = %d", level);
        SetControlVolumeLevels(level);
    }
    DEBUG("CMMAGlobalVolume: VolumeUp: -");
}

// ---------------------------------------------------------------------------
// CMMAGlobalVolume::VolumeDown
// ---------------------------------------------------------------------------
//
void CMMAGlobalVolume::VolumeDown()
{
    DEBUG_INT("THREADID = %d : CMMAGlobalVolume: VolumeDown: +", RThread().Id());
    DEBUG_INT("CMMAGlobalVolume: VolumeDown : iLevel = %d", iLevel);
    // Adjust volume if midlet is in foreground and the volume value
    // is not too low, in this case it cannot be set under zero
    if (iForeground->IsForeground() && (iLevel > 0))
    {
        DEBUG("CMMAGlobalVolume: VolumeDown: Volume down");
        // Check that the currnet volume level is not decreased too much
        TInt level =
            iLevel < KMMAVolumeLevelStep ?
            0 : iLevel - KMMAVolumeLevelStep;
        DEBUG_INT("CMMAGlobalVolume: VolumeDown : effective volume to be set to player = %d", level);
        // Decrease level by new value
        SetControlVolumeLevels(level);
    }
    DEBUG("CMMAGlobalVolume: VolumeDown: -");
}

// ---------------------------------------------------------------------------
// CMMAGlobalVolume::FindVolumeControl
// ---------------------------------------------------------------------------
//
CMMAVolumeControl* CMMAGlobalVolume::FindVolumeControl(CMMAPlayer* aPlayer)
{
    TInt count(aPlayer->ControlCount());
    for (TInt i(0); i < count; i++)
    {
        CMMAControl* control = aPlayer->Control(i);
        // Check that if this control supports volume control
        if (control->ClassName() == KMMAVolumeControlName)
        {
            return static_cast< CMMAVolumeControl* >(control);
        }
    }
    // Does not support volume control
    return NULL;
}

// ---------------------------------------------------------------------------
// CMMAGlobalVolume::SetControlVolumeLevels
// ---------------------------------------------------------------------------
//
void CMMAGlobalVolume::SetControlVolumeLevels(TInt aLevel)
{
    TInt count(iControlList.Count());
    DEBUG_INT("CMMAGlobalVolume: SetControlVolumeLevels + count of controls = %d",count);
    // Adjust volume for all current volume controls associated
    for (TInt i(0); i < count; i++)
    {
        const TMMAPlayerHolder& hdr = iControlList[ i ];
        // Set new volume level for this control
        TRAPD(error,
              hdr.iVolumeControl->SetVolumeLevelL(hdr.iVolumeIndex, aLevel));
        DEBUG_INT2("CMMAGlobalVolume: SetControlVolumeLevels level set to control = %d,err = %d",aLevel,error);
        if (error != KErrNone)
        {
            hdr.iPlayer->PostStringEvent(
                CMMAPlayerEvent::EError,
                KMMAGlobalVolumeSetError());
        }
    }

    iLevel = aLevel;
    DEBUG_INT(
        "CMMAGlobalVolume::SetControlVolumeLevels - iLevel = %d", iLevel);

    // Store new volume to MMA global settings. Error cannot be reported
    // in any sophisticated way so we just have to ignore it. Debug builds
    // may panic in this case
    TInt error = iSettingsStore->Set(KMobileMediaVolumeLevel, aLevel);
    DEBUG_INT2("CMMAGlobalVolume::setting the new value to Settings store, value = %d, err = %d", aLevel, error);
    __ASSERT_DEBUG(error == KErrNone, User::Invariant());
}

// ---------------------------------------------------------------------------
// CMMAGlobalVolume::CMMAGlobalVolume
// ---------------------------------------------------------------------------
//
CMMAGlobalVolume::CMMAGlobalVolume(CMMAForeground* aForeground) :
        iLevel(KMMAVolumeMaxLevel),
        iForeground(aForeground)
{
}

// ---------------------------------------------------------------------------
// CMMAGlobalVolume::ConstructL
// ---------------------------------------------------------------------------
//
void CMMAGlobalVolume::ConstructL()
{
    iVolumeKeysListener = CMMAVolumeKeysListener::NewL(this);
    iSettingsStore = CRepository::NewL(KCRUidMobileMedia);

    // Get level from the settings store
    User::LeaveIfError(iSettingsStore->Get(KMobileMediaVolumeLevel, iLevel));
    DEBUG_INT("CMMAGlobalVolume::ConstructL - iLevel got from cenrep = %d", iLevel);
}

// End of file
