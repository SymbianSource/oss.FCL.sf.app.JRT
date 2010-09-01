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
#include "cammsreverbsourcecontrol.h"
#include "cammsbasereverbcontrol.h"

// CONSTANTS
const TInt KAMMSDisconnectReverbSource = 2147483647; // From JSR-234


#ifdef _DEBUG
const TInt KAMMSMaxRoomLevel = 0;
#endif // _DEBUG

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSReverbSourceControl::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSReverbSourceControl* CAMMSReverbSourceControl::NewLC(
    CMMAPlayer* aPlayer,
    CAMMSBaseReverbControl* aReverbControl)
{
    CAMMSReverbSourceControl* self =
        new(ELeave)CAMMSReverbSourceControl(aPlayer, aReverbControl);

    CleanupStack::PushL(self);

    return self;
}

// Destructor
CAMMSReverbSourceControl::~CAMMSReverbSourceControl()
{
    DEBUG("AMMS::CAMMSReverbSourceControl::~CAMMSReverbSourceControl");

    // Perform DeallocateControl, if the state change has not yet performed it.
    DeallocateControl();
    delete iReverbSource;
}

// -----------------------------------------------------------------------------
// CAMMSReverbSourceControl::SetRoomLevelL
// Sets the object specific level for the reverberant sound.
// -----------------------------------------------------------------------------
//
void CAMMSReverbSourceControl::SetRoomLevelL(TInt aLevel)
{
    DEBUG_INT("AMMS::CAMMSReverbSourceControl::SetRoomLevelL +: %d", aLevel);

    // Check in debug build that aLevel is within valid range.
    __ASSERT_DEBUG(
        (aLevel <= KAMMSMaxRoomLevel) ||
        (aLevel == KAMMSDisconnectReverbSource),
        User::Invariant());

    // With the JSR-234 value Integer.MIN_VALUE, the reflected sound for the
    // given object can be disabled.
    if (aLevel == KMinTInt)
    {
        DEBUG("AMMS::CAMMSReverbSourceControl::SetRoomLevelL(Integer.MIN_VALUE)");
        aLevel = 0;
    }

    // Check the allowed boundaries for room level.
    TInt32 minLevel = 0;
    TInt32 maxLevel = 0;
    iReverbSource->LevelRange(minLevel, maxLevel);
    DEBUG_INT2("AMMS::CAMMSReverbSourceControl::SetRoomLevelL boundaries %d, %d",
               minLevel, maxLevel);

    // Check the state of the effect. If it is in disabled state, enable it.
    if (!iReverbSource->IsEnabled())
    {
        DEBUG("AMMS::CAMMSReverbSourceControl::SetRoomLevelL calling EnableL");
        iReverbSource->EnableL();
    }

    // With the JSR-234 value DISCONNECT, the object can be disconnected
    // from the reverb.
    if (aLevel == KAMMSDisconnectReverbSource)
    {
        // Do not call iReverbSource->DisableL(), instead set the room level to
        // _minimum_ value (= smallest negative gain -> maximum attenuation).
        // The CRoomLevel must also be kept enabled.
        aLevel = minLevel;

        DEBUG_INT("AMMS::CAMMSReverbSourceControl::SetRoomLevelL(DISCONNECT) %d",
                  minLevel);
    }
    else
    {
        // Set the room level within allowed boundaries from Effect API
        aLevel = Min(aLevel, maxLevel);
        aLevel = Max(aLevel, minLevel);
        DEBUG_INT("AMMS::CAMMSReverbSourceControl::SetRoomLevelL setting value: %d", aLevel);
    }

    // Sets the RoomLevel level, it will leave if aRoomLevel is not within range
    // of Min and Max
    iReverbSource->SetRoomLevelL((TInt32)aLevel);

    // Apply updated settings to Effect API.
    iReverbSource->ApplyL();

    DEBUG("AMMS::CAMMSReverbSourceControl::SetRoomLevelL -");
}

// -----------------------------------------------------------------------------
// CAMMSReverbSourceControl::PrepareControlL
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSReverbSourceControl::PrepareControlL()
{
    DEBUG("AMMS::CAMMSReverbSourceControl::PrepareControlL");

    // Perform the action only for the first time, skip if called afterwards
    if (!iReverbSource)
    {
        CEnvironmentalReverbUtility* reverbUtility = NULL;
        iReverbControl->GetEnvironmentalReverbUtilityL(&reverbUtility);
        // Reverb utility must exist, otherwise room level creation will fail.
        __ASSERT_DEBUG(reverbUtility, User::Invariant());

        CEnvironmentalReverb* reverb =
            &(reverbUtility->EnvironmentalReverb());

        CCustomCommandUtility* customCommandUtility =
            CreateCustomCommandUtilityL();

        // Effect API takes the ownership of customCommandUtility.
        iReverbSource = CRoomLevel::NewL(customCommandUtility, *reverb);
    }
}

// -----------------------------------------------------------------------------
// CAMMSReverbSourceControl::DeallocateControl
// Function which is called after the correct state is set in Player.
// -----------------------------------------------------------------------------
//
void CAMMSReverbSourceControl::DeallocateControl()
{
    if (iReverbSource)
    {
        DEBUG("AMMS::CAMMSReverbSourceControl::DeallocateControl");

        // Disable the Effect API Control
        TRAPD(err, iReverbSource->DisableL());
        if (err != KErrNone)
        {
            // The only even theoritically possible error code here would be
            // KErrAccessDenied which is a result from Effect API calling ApplyL
            // method without having update rights, but since the Effect
            // is already created, that situation can be discarded here.
        }

        // Delete the Effect API class (it deletes CustomCommandUtility)
        delete iReverbSource;
        iReverbSource = NULL;
    }
}

const TDesC& CAMMSReverbSourceControl::ClassName() const
{
    return KAMMSReverbSourceControl;
}

// -----------------------------------------------------------------------------
// CAMMSReverbSourceControl::CAMMSReverbSourceControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSReverbSourceControl::CAMMSReverbSourceControl(
    CMMAPlayer* aPlayer,
    CAMMSBaseReverbControl* aReverbControl)
        : CAMMSBaseReverbSourceControl(aPlayer), iReverbControl(aReverbControl)
{
}

//  End of File
