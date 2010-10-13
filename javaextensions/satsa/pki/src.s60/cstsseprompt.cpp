/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



// INCLUDE FILES
#include "cstsseprompt.h"
#include <AknGlobalNote.h>
#include <avkon.rsg>
#include <caosynchronizer.h>



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSSEPrompt::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSSEPrompt* CSTSSEPrompt::NewLC()
{
    CSTSSEPrompt* self = new(ELeave) CSTSSEPrompt;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// Destructor
CSTSSEPrompt::~CSTSSEPrompt()
{
    delete iGlobalNote;
    delete iSynchronizer;
}

// -----------------------------------------------------------------------------
// CSTSSEPrompt::DisplayPromptL
// Displays the prompt
// -----------------------------------------------------------------------------
//
void CSTSSEPrompt::DisplayPromptL(const TDesC& aPrompt)
{
    iGlobalNote->ShowNoteL(
        iSynchronizer->iStatus,
        EAknGlobalConfirmationNote,
        aPrompt);
    iSynchronizer->ExecuteL((TTimeIntervalMicroSeconds32) 0);
}

// -----------------------------------------------------------------------------
// CSTSSEPrompt::CSTSSEPrompt
// c++ constructor, may not leave
// -----------------------------------------------------------------------------
//
CSTSSEPrompt::CSTSSEPrompt()
{

}

// -----------------------------------------------------------------------------
// CSTSSEPrompt::ConstructL
// Second phase constructor, may leave
// -----------------------------------------------------------------------------
void CSTSSEPrompt::ConstructL()
{
    iGlobalNote = CAknGlobalNote::NewL();
    iGlobalNote->SetSoftkeys(R_AVKON_SOFTKEYS_OK_EMPTY);
    iSynchronizer = CAOSynchronizer::NewL();
}


