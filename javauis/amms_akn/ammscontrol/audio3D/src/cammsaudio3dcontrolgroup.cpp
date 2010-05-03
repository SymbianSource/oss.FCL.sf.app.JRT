/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Group for 3D audio controls
*
*/


// INCLUDE FILES
#include "cammsaudio3dcontrolgroup.h"

// ============================ MEMBER FUNCTIONS ===============================

// Destructor
CAMMSAudio3DControlGroup::~CAMMSAudio3DControlGroup()
{
}

// -----------------------------------------------------------------------------
// CAMMSAudio3DControlGroup::UpdateL
// Update the controls depending on commit mode
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSAudio3DControlGroup::UpdateL(TInt aCommit)
{
    // Add new variables that have to be committed to member variable.
    iCommit |= aCommit;

    if (iCommitMode == EImmediate)
    {
        // Commit new values, and remove them from the member variable.
        CommitL(aCommit);
        iCommit &= ~aCommit;
    }
}

// -----------------------------------------------------------------------------
// CAMMSAudio3DControlGroup::SetModeL
// Sets the mode of the CommitControl.
// Can be called only from CAMMSCommitControlGroup
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSAudio3DControlGroup::SetModeL(TCommitMode aMode)
{
    // When switching back from the deferred mode to the immediate mode
    // (setDeferred(false)) all the pending parameters from the buffer are
    // transmitted to the audio processing system automatically.
    if (aMode == EImmediate)
    {
        CommitL(iCommit);
        iCommit = 0;
    }
    iCommitMode = aMode;
}

// -----------------------------------------------------------------------------
// CAMMSAudio3DControlGroup::CommitGroupL
// Commits all the controls in the group
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAMMSAudio3DControlGroup::CommitGroupL()
{
    // In case the mode is changed to EImmediate in the middle of commit process,
    // SetModeL method is implemented so that it takes care of all pending
    // parameter committing.
    if (iCommitMode == EDeferred)
    {
        CommitL(iCommit);
        iCommit = 0;
    }
}

// -----------------------------------------------------------------------------
// CAMMSAudio3DControlGroup::CAMMSAudio3DControlGroup
// C++ default constructor can NOT contain any code, that
// might leave.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAMMSAudio3DControlGroup::CAMMSAudio3DControlGroup(
    const TDesC& aName,
    TAMMSControlTypes aControlType) :
        CAMMSControlGroup(aName, aControlType)
{
    // default values
    iCommitMode = EImmediate;
    iCommit = 0;
}

// -----------------------------------------------------------------------------
// CAMMSAudio3DControlGroup::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CAMMSAudio3DControlGroup::ConstructL()
{
    CAMMSControlGroup::ConstructL();
}



