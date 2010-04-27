/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  javaregserverscheduler implementation
*
*/


// INCLUDES
#include "javaregserverscheduler.h"
#include "javaregserver.h"

using namespace Java::Manager::Registry;

// LOCAL CONSTANTS
_LIT(KNameJavaRegServerThread, "JavaRegServerThread");


// ---------------------------------------------------------------------------
// ThreadStart
// ---------------------------------------------------------------------------
TInt CJavaRegServerScheduler::ThreadStart()
{
    TInt err = KErrNoMemory;

    // get cleanup stack
    CTrapCleanup* cleanup = CTrapCleanup::New();

    // initialize all up to and including starting scheduler
    if (cleanup)
    {
        TRAP(err, CreateAndRunServerL());
        delete cleanup;
    }

    return err;
}


// ---------------------------------------------------------------------------
// CreateAndRunServerL
// ---------------------------------------------------------------------------
void CJavaRegServerScheduler::CreateAndRunServerL()
{
    // Rename thread
    TInt err = User::RenameThread(KNameJavaRegServerThread);
    __ASSERT_DEBUG(err == KErrNone, User::Invariant());

    // construct active scheduler
    CJavaRegServerScheduler*  self = new(ELeave) CJavaRegServerScheduler;
    CleanupStack::PushL(self);
    CActiveScheduler::Install(self);

    // construct server
    self->iServer = CJavaRegServer::NewL();

    // Initialisation complete, now signal the client
    RProcess::Rendezvous(KErrNone);

    CActiveScheduler::Start();

    // Destroy the scheduler
    CleanupStack::PopAndDestroy(self);
}


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CJavaRegServerScheduler::~CJavaRegServerScheduler()
{
    delete iServer;
}

// End of File

