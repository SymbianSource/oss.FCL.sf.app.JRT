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
* Description:  Group for commit controls
*
*/


// INCLUDE FILES

#include <e32base.h>
#include "cammscommitcontrolgroup.h"
#include "cammsmodule.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSCommitControlGroup::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSCommitControlGroup* CAMMSCommitControlGroup::NewLC(
    CAMMSModule& aSpectator,
    CAMMSModuleContainer& aSoundSource3Ds)
{
    CAMMSCommitControlGroup* self = new(ELeave) CAMMSCommitControlGroup(
        aSpectator,
        aSoundSource3Ds);

    CleanupStack::PushL(self);
    // calls base class ConstructL
    self->ConstructL();

    return self;
}

// Destructor
CAMMSCommitControlGroup::~CAMMSCommitControlGroup()
{
}

// -----------------------------------------------------------------------------
// CAMMSCommitControlGroup::CommitAllControlsL
// Transfers all the pending parameters to the audio processing system
// for all ControlGroups in this controllable
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSCommitControlGroup::CommitAllControlsL()
{
    TInt groupAmount = 0;

    // Go through every module in SoundSource3D
    TInt moduleAmount = iSoundSource3Ds.Count();
    for (TInt i = 0; i < moduleAmount; i++)
    {
        CAMMSModule* module = iSoundSource3Ds.At(i);

        // Go through every ControlGroup in Module
        groupAmount = module->Count();
        for (TInt j = 0; j < groupAmount; j++)
        {
            MAMMSControlGroup* group = module->At(j);
            group->CommitGroupL();
        }
    }

    // And finally go through every ControlGroup in Spectator
    groupAmount = iSpectator.Count();
    for (TInt i = 0; i < groupAmount; i++)
    {
        MAMMSControlGroup* group = iSpectator.At(i);
        group->CommitGroupL();
    }
}

// -----------------------------------------------------------------------------
// CAMMSCommitControlGroup::SetDeferredL
// Sets the mode of the CommitControl
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSCommitControlGroup::SetDeferredL(TBool aDeferred)
{
    TInt groupAmount = 0;
    TCommitMode mode = (aDeferred ? EDeferred : EImmediate);

    // Go through every module in SoundSource3D
    TInt moduleAmount = iSoundSource3Ds.Count();
    for (TInt i = 0; i < moduleAmount; i++)
    {
        CAMMSModule* module = iSoundSource3Ds.At(i);

        // Go through every ControlGroup in Module
        groupAmount = module->Count();
        for (TInt j = 0; j < groupAmount; j++)
        {
            MAMMSControlGroup* group = module->At(j);
            group->SetModeL(mode);
        }
    }

    // And finally go through every ControlGroup in Spectator
    groupAmount = iSpectator.Count();
    for (TInt i = 0; i < groupAmount; i++)
    {
        MAMMSControlGroup* group = iSpectator.At(i);
        group->SetModeL(mode);
    }
}

// -----------------------------------------------------------------------------
// CAMMSCommitControlGroup::ClassName
// Returns class name that identifies this control group.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC16& CAMMSCommitControlGroup::ClassName()
{
    return KAMMSCommitControlClassName;
}

// -----------------------------------------------------------------------------
// CAMMSCommitControlGroup::CommitL
// Transfers all the pending parameters to the audio processing system.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSCommitControlGroup::CommitL(TInt /*aCommit*/)
{
}

// -----------------------------------------------------------------------------
// CAMMSCommitControlGroup::CAMMSCommitControlGroup
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSCommitControlGroup::CAMMSCommitControlGroup(
    CAMMSModule& aSpectator,
    CAMMSModuleContainer& aSoundSource3Ds):
// CommitControlGroup has no associated amms controls therefore
// the name passed as a parameter to the constructor is KNullDesC
// CHANGED for now, passing actual name here even no associated Controls.
        CAMMSAudio3DControlGroup(KAMMSCommitControl),
        iSpectator(aSpectator),
        iSoundSource3Ds(aSoundSource3Ds)
{
}


