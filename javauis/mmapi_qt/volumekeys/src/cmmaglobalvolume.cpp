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
#include <logger.h>

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
    //delete iSettingsStore;
    delete iForeground;
    iControlList.Close();
}

// ---------------------------------------------------------------------------
// CMMAGlobalVolume::AddPlayerL
// ---------------------------------------------------------------------------
//
void CMMAGlobalVolume::AddPlayerL(CMMAPlayer* aPlayer)
{
    // Find if the player has a volume control
    CMMAVolumeControl* control = FindVolumeControl(aPlayer);

    // Ignore adding new player if it does not support volume control
    if (control)
    {
        TMMAPlayerHolder playerHolder;
        // Create new volume control and level index pair
        playerHolder.iVolumeControl = control;
        playerHolder.iVolumeIndex = control->AddLevelL();
        playerHolder.iPlayer = aPlayer;
        // Set current volume level for the control
        control->SetVolumeLevelL(playerHolder.iVolumeIndex, iLevel);
        // Add created pair to the control list
        iControlList.AppendL(playerHolder);
    }
}

// ---------------------------------------------------------------------------
// CMMAGlobalVolume::RemovePlayer
// ---------------------------------------------------------------------------
//
void CMMAGlobalVolume::RemovePlayer(CMMAPlayer* aPlayer)
{
    // Find if the player has a volume control
    CMMAVolumeControl* control = FindVolumeControl(aPlayer);

    // Ignore adding new player if it does not support volume control
    if (control)
    {
        // Check that if this type of volume control can be found from
        // the control list and remove it
        TInt count(iControlList.Count());
        for (TInt i(0); i < count; i++)
        {
            const TMMAPlayerHolder& holder = iControlList[ i ];
            if (control == holder.iVolumeControl)
            {
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
    LOG1( EJavaMMAPI, EInfo, "THREADID = %d : CMMAGlobalVolume: VolumeUp: +", RThread().Id().Id());
    // Adjust volume if midlet is in foreground and the volume level value
    // is not too high, in this case it cannot be set over KMMAVolumeMaxLevel
    if (iForeground->IsForeground() && (iLevel < KMMAVolumeMaxLevel))
    {
        LOG( EJavaMMAPI, EInfo, "CMMAGlobalVolume: VolumeUp: Volume up");
        // Check that the current volume level is not increased too much
        TInt level =
            iLevel > (KMMAVolumeMaxLevel - KMMAVolumeLevelStep) ?
            KMMAVolumeMaxLevel - iLevel : iLevel + KMMAVolumeLevelStep;
        // Increase level by new value
        SetControlVolumeLevels(level);
    }
    LOG( EJavaMMAPI, EInfo, "CMMAGlobalVolume: VolumeUp: -");
}

// ---------------------------------------------------------------------------
// CMMAGlobalVolume::VolumeDown
// ---------------------------------------------------------------------------
//
void CMMAGlobalVolume::VolumeDown()
{
    LOG1( EJavaMMAPI, EInfo, "THREADID = %d : CMMAGlobalVolume: VolumeDown: +", RThread().Id().Id());
    // Adjust volume if midlet is in foreground and the volume value
    // is not too low, in this case it cannot be set under zero
    if (iForeground->IsForeground() && (iLevel > 0))
    {
        LOG( EJavaMMAPI, EInfo, "CMMAGlobalVolume: VolumeDown: Volume down");
        // Check that the currnet volume level is not decreased too much
        TInt level =
            iLevel < KMMAVolumeLevelStep ?
            0 : iLevel - KMMAVolumeLevelStep;
        // Decrease level by new value
        SetControlVolumeLevels(level);
    }
    LOG( EJavaMMAPI, EInfo, "CMMAGlobalVolume: VolumeDown: -");
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
    // Adjust volume for all current volume controls associated
    for (TInt i(0); i < count; i++)
    {
        const TMMAPlayerHolder& hdr = iControlList[ i ];
        // Set new volume level for this control
        TRAPD(error,
              hdr.iVolumeControl->SetVolumeLevelL(hdr.iVolumeIndex, aLevel));
        if (error != KErrNone)
        {
            hdr.iPlayer->PostStringEvent(
                CMMAPlayerEvent::EError,
                KMMAGlobalVolumeSetError());
        }
    }

    iLevel = aLevel;
    LOG1( EJavaMMAPI, EInfo, 
        "CMMAGlobalVolume::SetControlVolumeLevels - iLevel = %d", iLevel);

    // Store new volume to MMA global settings. Error cannot be reported
    // in any sophisticated way so we just have to ignore it. Debug builds
    // may panic in this case
    TInt error = iSettingsStore->Set(KMobileMediaVolumeLevel, aLevel);
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
    // TO-DO remove TRAP_IGNORE
   // TRAP_IGNORE(err,iSettingsStore = CRepository::NewL(KCRUidMobileMedia));

    // Get level from the settings store
    //User::LeaveIfError(iSettingsStore->Get(KMobileMediaVolumeLevel, iLevel));
}

// End of file
