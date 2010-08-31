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
* Description:  Group for effect controls
*
*/


// INCLUDE FILES
#include <logger.h>
#include "cammseffectcontrolgroup.h"
#include "cammseffectcontrol.h"


// ============================ MEMBER FUNCTIONS ===============================

// Destructor
CAMMSEffectControlGroup::~CAMMSEffectControlGroup()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::~ +");

    delete iPresetNames;
    delete iActiveSchedulerWait;

    if (iEmptyPlayerUtility)
    {
        iEmptyPlayerUtility->Close();
        delete iEmptyPlayerUtility;
    }

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::~ -");
}

// -----------------------------------------------------------------------------
// CAMMSEffectControlGroup::PresetL
// Gets the current preset.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEffectControlGroup::GetPresetL(TDes& aPreset)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::GetPresetL +");

    // Return KNullDesC if no preset is set.
    if (iPresetIndex >= 0)
    {
        aPreset = (*iPresetNames)[ iPresetIndex ];    // CSI: 2 Wrong index means implementation error #
    }
    else
    {
        aPreset = KNullDesC;
    }

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::GetPresetL -");
}

// -----------------------------------------------------------------------------
// CAMMSEffectControlGroup::PresetNamesL
// Gets the available preset names
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const CDesCArray& CAMMSEffectControlGroup::PresetNamesL()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::PresetNamesL");

    return *iPresetNames;
}

// -----------------------------------------------------------------------------
// CAMMSEffectControlGroup::Scope
// Returns the scope in which the effect is present.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAMMSEffectControlGroup::TEffectScope CAMMSEffectControlGroup::Scope()
{
    return iScope;
}

// -----------------------------------------------------------------------------
// CAMMSEffectControlGroup::Enabled
// Returns true if the effect is enabled and false otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CAMMSEffectControlGroup::Enabled()
{
    return iEnabled;
}

// -----------------------------------------------------------------------------
// CAMMSEffectControlGroup::EnforcedL
// Returns the current enforced setting of the effect.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CAMMSEffectControlGroup::Enforced()
{
    return iEnforced;
}

// -----------------------------------------------------------------------------
// CAMMSEffectControlGroup::SetEnabledL
// Enables/disables the effect.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEffectControlGroup::SetEnabledL(TBool aEnabled)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::SetEnabledL +");

    TInt count = ControlCount();
    for (TInt i = 0; i < count; i++)
    {
        TypeSafeControl(i)->SetEnabledL(aEnabled);
    }

    iEnabled = aEnabled;

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::SetEnabledL -");
}

// -----------------------------------------------------------------------------
// CAMMSEffectControlGroup::SetEnforcedL
// Enforces the effect to be in use.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEffectControlGroup::SetEnforcedL(TBool aEnforced)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::SetEnforcedL +");

    TInt count = ControlCount();
    for (TInt i = 0; i < count; i++)
    {
        TypeSafeControl(i)->SetEnforcedL(aEnforced);
    }

    iEnforced = aEnforced;

    // remember that enforce was set when the group was not yet initialized
    if (!iInitialized)
    {
        iInitialEnforceSet = ETrue;
    }

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::SetEnforcedL -");
}

// -----------------------------------------------------------------------------
// CAMMSEffectControlGroup::SetPresetL
// Sets the effect according to the given preset.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEffectControlGroup::SetPresetL(const TDesC& aPreset)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::SetPresetL +");

    // Leave if the given preset does not exist.
    TInt presetIndex = -1;
    if (!(iPresetNames->Find(aPreset, presetIndex) == 0))
    {
        User::Leave(KErrNotFound);
    }

    // Set new preset to the controls.
    TInt count = ControlCount();
    for (TInt i = 0; i < count; i++)
    {
        TypeSafeControl(i)->SetPresetL(aPreset);
    }

    // Change preset.
    iPresetIndex = presetIndex;

    // Announce a preset change.
    PresetChangedL();

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::SetPresetL -");
}

// -----------------------------------------------------------------------------
// CAMMSEffectControlGroup::SetScopeL
// Sets the scope of the effect.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEffectControlGroup::SetScopeL(TEffectScope aScope)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::SetScopeL +");

    TInt count = ControlCount();
    for (TInt i = 0; i < count; i++)
    {
        TypeSafeControl(i)->SetScopeL(aScope);
    }

    iScope = aScope;

    // remember that scope was set when the group was not yet initialized
    if (!iInitialized)
    {
        iInitialScopeSet = ETrue;
    }

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::SetScopeL -");
}


// -----------------------------------------------------------------------------
// CAMMSEffectControlGroup::MapcInitComplete
// Called when file KAMMSEmptyGroupSoundPath has been opened.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEffectControlGroup::MapcInitComplete(TInt aError,
        const TTimeIntervalMicroSeconds& /*aDuration*/)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::MapcInitComplete +");

    __ASSERT_DEBUG(iActiveSchedulerWait->IsStarted(), User::Invariant());

    iEmptyPlayerUtilityError = aError;

    // Stop waiting in PrepareEmptyGroupUtilityL() function.
    iActiveSchedulerWait->AsyncStop();

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::MapcInitComplete -");
}

// -----------------------------------------------------------------------------
// CAMMSEffectControlGroup::MapcPlayComplete
// Called when KAMMSEmptyGroupSoundPath has been played.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEffectControlGroup::MapcPlayComplete(TInt /*aError*/)
{
    // No implementation needed.
}

// -----------------------------------------------------------------------------
// CAMMSEffectControlGroup::TypeSafeControl
// Gets control. Ownership is not tranferred.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAMMSEffectControl*
CAMMSEffectControlGroup::TypeSafeControl(TInt aIndex) const
{
    return static_cast<CAMMSEffectControl*>(Control(aIndex));
}

// -----------------------------------------------------------------------------
// CAMMSEffectControlGroup::NotifyPlayerAddedL
// Called by when a new player is added
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEffectControlGroup::NotifyPlayerAddedL(
    CMMAPlayer *aPlayer,
    CMMAControl* aControl)
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::NotifyPlayerAddedL +");

    CAMMSControlGroup::NotifyPlayerAddedL(aPlayer, aControl);

    CAMMSEffectControl* control =
        static_cast<CAMMSEffectControl*>(aControl);

    // set current preset if any
    if (iPresetIndex >= 0)
    {
        control->SetPresetL((*iPresetNames)[ iPresetIndex ]);      // CSI: 2 Wrong index means implementation error #
    }

    // handle default values if this is the first control added into empty and
    // uninitialized group
    if (!iInitialized)
    {

        InitializeL();

        // if parameters, whose default values are not known, have not been set,
        // ask them from the first control added to the empty group
        if (!iInitialEnforceSet)
        {

            iEnforced = control->Enforced();

        }

        if (!iInitialScopeSet)
        {

            iScope = (TEffectScope) control->Scope();

        }

        iInitialized = ETrue;
    }

    // set current parameters

    control->SetEnabledL(iEnabled);

    control->SetEnforcedL(iEnforced);

    control->SetScopeL(iScope);

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::NotifyPlayerAddedL -");
}

// -----------------------------------------------------------------------------
// CAMMSEffectControlGroup::PresetChangedL
// Called when the current preset changes
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEffectControlGroup::PresetChangedL()
{
}

// -----------------------------------------------------------------------------
// CAMMSEffectControlGroup::InitializeL
// Finish initialization (after the 1st player is added)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEffectControlGroup::InitializeL()
{
}

// -----------------------------------------------------------------------------
// CAMMSEffectControlGroup::PrepareEmptyGroupUtilitiesL
// Creates an utilities that can be used to obtain preset names and preset data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEffectControlGroup::PrepareEmptyGroupUtilitiesL()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::PrepareEmptyGroupUtilitiesL +");

    if (!iEmptyPlayerUtility)
    {
        iEmptyPlayerUtility = CMdaAudioPlayerUtility::NewL(*this,
                              EMdaPriorityMin, EMdaPriorityPreferenceNone);

        iEmptyPlayerUtility->OpenFileL(KAMMSEmptyGroupSoundPath);

        __ASSERT_DEBUG(!iActiveSchedulerWait->IsStarted(), User::Invariant());

        // Wait until MapcInitComplete() has been called.
        iActiveSchedulerWait->Start();  // CSI: 10 iActiveSchedulerWait cannot be started, also checked in debug build #

        ELOG1( EJavaAMMS, 
            "AMMS::CAMMSEffectControlGroup::PrepareEmptyGroupUtilitiesL, err=%d",
            iEmptyPlayerUtilityError);

        // Leave if file opening returned an error.
        User::LeaveIfError(iEmptyPlayerUtilityError);
    }

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::PrepareEmptyGroupUtilitiesL -");
}

// -----------------------------------------------------------------------------
// CAMMSEffectControlGroup::DeleteEmptyGroupUtilities
// Deletes utilities that are used to obtain preset names and preset data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEffectControlGroup::DeleteEmptyGroupUtilities()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::DeleteEmptyGroupUtilities +");

    if (iEmptyPlayerUtility)
    {
        iEmptyPlayerUtility->Close();

        delete iEmptyPlayerUtility;
        iEmptyPlayerUtility = NULL;
    }

    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::DeleteEmptyGroupUtilities -");
}

// -----------------------------------------------------------------------------
// CAMMSEffectControlGroup::CAMMSEffectControlGroup
// C++ default constructor can NOT contain any code, that
// might leave.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAMMSEffectControlGroup::CAMMSEffectControlGroup(const TDesC& aName) :
        CAMMSControlGroup(aName)
{
    iScope = CAMMSEffectControlGroup::EScopeLiveOnly;
    iEnabled = EFalse;
    iEnforced = ETrue;
    iPresetIndex = -1;  // no preset set
}

// -----------------------------------------------------------------------------
// CAMMSEffectControlGroup::ConstructL
// Symbian 2nd phase constructor can leave.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSEffectControlGroup::BaseConstructL()
{
    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::BaseConstructL +");

    CAMMSControlGroup::ConstructL();

    iActiveSchedulerWait = new(ELeave)CActiveSchedulerWait;
    iPresetNames = new(ELeave)CDesCArrayFlat(KAMMSPresetGranularity);

    // If the Effect API implementation does not support the effect,
    // the function leaves with KErrNotSupported. The leaving can be ignored
    // in this case, the result is that the list of supported presets
    // remain empty.
    TRAPD(err, GetPresetNamesL(*iPresetNames));

    ELOG1( EJavaAMMS, "AMMS::CAMMSEffectControlGroup::BaseConstructL, err %d", err);

    // In case of an error, delete possible utilities to save memory.
    if (err != KErrNone)
    {
        DeleteEmptyGroupUtilities();
    }

    // Leave if some error occured (other than KErrNotSupported).
    if ((err != KErrNone) && (err != KErrNotSupported))
    {
        User::Leave(err);
    }


    LOG( EJavaAMMS, EInfo, "AMMS::CAMMSEffectControlGroup::BaseConstructL -");
}

//  End of File
