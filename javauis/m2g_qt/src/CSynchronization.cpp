/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  M2GCore function call synchronization for J9
*
*/

// INCLUDE FILES
#include "CSynchronization.h"

NONSHARABLE_CLASS(M2gGlobals)
{
public:
    M2gGlobals() : mSync(0) {}

public:
    CSynchronization* mSync;
};

#if defined(__WINSCW__)

#include <pls.h>
M2gGlobals* getM2gGlobals()
{
    // Access the PLS of this process.
    return Pls<M2gGlobals>(TUid::Uid(0x2002DCBD));
}

#else

static M2gGlobals* sGlobals = 0;

M2gGlobals* getM2gGlobals()
{
    if (sGlobals == 0)
    {
        sGlobals = new M2gGlobals();
    }
    return sGlobals;
}
#endif


// STATIC MEMBERS
/*static*/ //CSynchronization* CSynchronization::iSelf = NULL;

// -----------------------------------------------------------------------------
// CSynchronization::InstanceL
// -----------------------------------------------------------------------------
/*static*/ CSynchronization* CSynchronization::InstanceL()
{
    static M2gGlobals* globals = getM2gGlobals();
    if (!globals->mSync)
    {
        globals->mSync = CSynchronization::NewL();
    }
    return globals->mSync;
}

// -----------------------------------------------------------------------------
// CSynchronization::NewL
// -----------------------------------------------------------------------------
/*static*/ CSynchronization* CSynchronization::NewL()
{
    CSynchronization* self = new(ELeave) CSynchronization();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// -----------------------------------------------------------------------------
// CSynchronization::ConstructL
// -----------------------------------------------------------------------------
void CSynchronization::ConstructL()
{
    User::LeaveIfError(iGuard.CreateLocal());
}

// -----------------------------------------------------------------------------
// CSynchronization::CSynchronization
// -----------------------------------------------------------------------------
CSynchronization::CSynchronization() : iErrorCode(0)
{
}

// -----------------------------------------------------------------------------
// CSynchronization::~CSynchronization
// -----------------------------------------------------------------------------
CSynchronization::~CSynchronization()
{
    iGuard.Close();
}

// -----------------------------------------------------------------------------
// CSynchronization::Lock
// -----------------------------------------------------------------------------
void CSynchronization::Lock()
{
    iGuard.Wait();
    iErrorCode = 0;
}

// -----------------------------------------------------------------------------
// CSynchronization::Unlock
// -----------------------------------------------------------------------------
void CSynchronization::Unlock()
{
    iErrorCode = 0;
    iGuard.Signal();
}

// -----------------------------------------------------------------------------
// CSynchronization::SetErrorCode
// -----------------------------------------------------------------------------
void CSynchronization::SetErrorCode(TInt aCode)
{
    iErrorCode = aCode;
}

// -----------------------------------------------------------------------------
// CSynchronization::GetErrorCode
// -----------------------------------------------------------------------------
TInt CSynchronization::GetErrorCode()
{
    return iErrorCode;
}
