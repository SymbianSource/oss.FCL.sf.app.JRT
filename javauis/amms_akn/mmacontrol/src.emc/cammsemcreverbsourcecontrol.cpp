/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Manipulates the settings of an audio effect reverb source.
*
*/


// INCLUDE FILES
#include <jdebug.h>
#include "cammsemcreverbsourcecontrol.h"
#include "cammsbasereverbcontrol.h"
#include "cammsemcreverbcontrol.h"

// CONSTANTS
const TInt KAMMSDisconnectReverbSource = 2147483647; // From JSR-234

#ifdef _DEBUG
const TInt KAMMSMaxRoomLevel = 0;
#endif // _DEBUG

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSEMCReverbSourceControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSEMCReverbSourceControl* CAMMSEMCReverbSourceControl::NewLC(
    CMMAPlayer* aPlayer,
    CAMMSBaseReverbControl* aBaseReverbControl)
{
    CAMMSEMCReverbSourceControl* self =
        new(ELeave)CAMMSEMCReverbSourceControl(aPlayer, aBaseReverbControl);

    CleanupStack::PushL(self);

    return self;
}

// Destructor
CAMMSEMCReverbSourceControl::~CAMMSEMCReverbSourceControl()
{
    DEBUG("AMMS::CAMMSEMCReverbSourceControl::~CAMMSEMCReverbSourceControl");

    // Perform DeallocateControl, if the state change has not yet performed it.
    DeallocateControl();

}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbSourceControl::SetRoomLevelL
// Sets the object specific level for the reverberant sound.
// -----------------------------------------------------------------------------
//
void CAMMSEMCReverbSourceControl::SetRoomLevelL(TInt aLevel)
{
    DEBUG_INT("AMMS:: CAMMSEMCReverbSourceControl::SetRoomLevelL +: %d", aLevel);
// Check in debug build that aLevel is within valid range.
    __ASSERT_DEBUG(
        (aLevel <= KAMMSMaxRoomLevel) ||
        (aLevel == KAMMSDisconnectReverbSource),
        User::Invariant());

    // With the JSR-234 value Integer.MIN_VALUE, the reflected sound for the
    // given object can be disabled.
    if (aLevel == KMinTInt)
    {
        DEBUG("AMMS::CAMMSEMCReverbSourceControl::SetRoomLevelL(Integer.MIN_VALUE)");
        aLevel = 0;
    }

    // Check the allowed boundaries for room level.
    TInt minLevel = 0;
    TInt maxLevel = 0;
    iRoomLevelControl->LevelRange(minLevel, maxLevel);
    DEBUG_INT2("AMMS::CAMMSEMCReverbSourceControl::SetRoomLevelL boundaries %d, %d",
               minLevel, maxLevel);

    // Check the state of the effect. If it is in disabled state, enable it.
    TBool enabled;
    iRoomLevelControl->IsEnabled(enabled);
    if (!enabled)
    {
        DEBUG("AMMS::CAMMSEMCReverbSourceControl::SetRoomLevelL calling EnableL");
        iRoomLevelControl->Enable();
    }

    // With the JSR-234 value DISCONNECT, the object can be disconnected
    // from the reverb.
    if (aLevel == KAMMSDisconnectReverbSource)
    {
        // Do not call iReverbSource->DisableL(), instead set the room level to
        // _minimum_ value (= smallest negative gain -> maximum attenuation).
        // The CRoomLevel must also be kept enabled.
        aLevel = minLevel;

        DEBUG_INT("AMMS::CAMMSEMCReverbSourceControl::SetRoomLevelL(DISCONNECT) %d",
                  minLevel);
    }
    else
    {
        // Set the room level within allowed boundaries from EMC API
        aLevel = Min(aLevel, maxLevel);
        aLevel = Max(aLevel, minLevel);
        DEBUG_INT("AMMS::CAMMSEMCReverbSourceControl::SetRoomLevelL setting value: %d", aLevel);
    }

    // Sets the RoomLevel level, it will leave if aRoomLevel is not within range
    // of Min and Max
    iRoomLevelControl->SetRoomLevel(aLevel);

    // Apply updated settings to EMC API.
    iRoomLevelControl->Apply();

    DEBUG("AMMS::CAMMSEMCReverbSourceControl::SetRoomLevelL -");
}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbSourceControl::PrepareControlL
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCReverbSourceControl::PrepareControlL()
{
    DEBUG("AMMS::CAMMSEMCReverbSourceControl::PrepareControlL");

    if (!iRoomLevelControl)
    {
        //Create RoomLevel Effect Control
        iStreamControl = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->StreamControl();
        iFactory = (static_cast<CMMAEMCAudioPlayer*>(iMMAPlayer))->MMFactory();

        MEffectControl* temp(NULL);
        User::LeaveIfError(iFactory->CreateEffectControl(KRoomLevelEffectControl, temp));
        iRoomLevelControl  = static_cast<MRoomLevelControl*>(temp);


        //Attach EMC Reverb Control to RoomLevelControl
        MReverbControl* reverbControl = iBaseReverbControl->GetReverbControlL();
        iRoomLevelControl->AttachReverb(*reverbControl);

        //Add Effect to Stream Control
        User::LeaveIfError(iStreamControl->AddEffect(*iRoomLevelControl));
    }


}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbSourceControl::DeallocateControl
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSEMCReverbSourceControl::DeallocateControl()
{
    DEBUG("AMMS::CAMMSEMCReverbSourceControl::DeallocateControl");
    if (iRoomLevelControl)
    {
        /*
        //idealy reverbControl should be checked for its existance and if it is there ,should be detached from RoomLevelControl.
        //But in this case reverbControl is deleted till this position,so need not to do it.
        if(reverbControl)
            {
            iRoomLevelControl->DetachReverb(*reverbControl);
            }
        */
        iStreamControl->RemoveEffect(*iRoomLevelControl);
        MEffectControl* objPtr2 = iRoomLevelControl;
        iFactory->DeleteEffectControl(objPtr2);
        iRoomLevelControl = NULL;
    }




}

const TDesC& CAMMSEMCReverbSourceControl::ClassName() const
{
    return KAMMSEMCReverbSourceControl;
}

// -----------------------------------------------------------------------------
// CAMMSEMCReverbSourceControl::CAMMSEMCReverbSourceControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSEMCReverbSourceControl::CAMMSEMCReverbSourceControl(
    CMMAPlayer* aPlayer,
    CAMMSBaseReverbControl* aBaseReverbControl)
        : CAMMSBaseReverbSourceControl(aPlayer), iBaseReverbControl(aBaseReverbControl)
{

    iMMAPlayer = aPlayer;

}

//  End of File
