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
* Description:  Synchronizes operations with Agenda server
 *
*/


// INCLUDE FILES
#include "cpimagnserverwait.h"
#include "logger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMAgnServerWait::CPIMAgnServerWait
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMAgnServerWait::CPIMAgnServerWait() :
        CTimer(EPriorityStandard)
{
    JELOG2(EPim);
    CActiveScheduler::Add(this);
}

// -----------------------------------------------------------------------------
// CPIMAgnServerWait::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMAgnServerWait* CPIMAgnServerWait::NewL()
{
    JELOG2(EPim);
    CPIMAgnServerWait* self = NewLC();
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CPIMAgnServerWait::NewL
// Two-phased constructor. Puts the object to cleanup stack.
// -----------------------------------------------------------------------------
//
CPIMAgnServerWait* CPIMAgnServerWait::NewLC()
{
    JELOG2(EPim);
    CPIMAgnServerWait* self = new(ELeave) CPIMAgnServerWait;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

void CPIMAgnServerWait::ConstructL()
{
    JELOG2(EPim);
    CTimer::ConstructL();
}
// Destructor
CPIMAgnServerWait::~CPIMAgnServerWait()
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMAgnServerWait::WaitCompleteL
//
// -----------------------------------------------------------------------------
//
TInt CPIMAgnServerWait::WaitCompleteL(TTimeIntervalMicroSeconds32 aTimeout)
{
    JELOG2(EPim);
    After(aTimeout);
    CActiveScheduler::Start();
    Cancel();
    return 0;
}
void CPIMAgnServerWait::RunL()
{
    JELOG2(EPim);
    CActiveScheduler::Stop();
}

TInt CPIMAgnServerWait::RunError(TInt /*aError*/)
{
    JELOG2(EPim);
    return 0;
}

// -----------------------------------------------------------------------------
// CPIMAgnServerWait::Progress
// Handles progress notifications from Agenda server.
// -----------------------------------------------------------------------------
//
void CPIMAgnServerWait::Progress(TInt /* aPercentageCompleted */)
{
    JELOG2(EPim);
    // Ignored.
}

// -----------------------------------------------------------------------------
// CPIMAgnServerWait::Completed
// Handles completion notifications from Agenda server.
// Completes the request in the synchronizer.
// -----------------------------------------------------------------------------
//
void CPIMAgnServerWait::Completed(TInt /*aError*/)
{
    JELOG2(EPim);
    CActiveScheduler::Stop();
}

// -----------------------------------------------------------------------------
// CPIMAgnServerWait::NotifyProgress
// Informs Agenda server that we are not interested in progress notifications.
// -----------------------------------------------------------------------------
//
TBool CPIMAgnServerWait::NotifyProgress()
{
    JELOG2(EPim);
    return EFalse;
}

//  End of File
