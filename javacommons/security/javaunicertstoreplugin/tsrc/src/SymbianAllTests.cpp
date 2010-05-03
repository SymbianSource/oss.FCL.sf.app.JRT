/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Platform.h"
#include "CommandLineTestRunner.h"

#include <e32cons.h>            // Console

_LIT(KTextConsoleTitle, "Console");
_LIT(KTextPressAnyKey, " [press any key]\n");

LOCAL_D CConsoleBase* console;  // write all messages to this

LOCAL_C void MainL()
{
    CommandLineTestRunner::RunAllTests(0, NULL);
}


LOCAL_C void DoStartL()
{
    // Create active scheduler (to run active objects)
    CActiveScheduler* scheduler = new(ELeave) CActiveScheduler();
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);

    MainL();

    // Delete active scheduler
    CleanupStack::PopAndDestroy(scheduler);
}

GLDEF_C TInt E32Main()
{

    // Create cleanup stack
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();

    // Create output console
    TRAPD(createError, console = Console::NewL(KTextConsoleTitle, TSize(KConsFullScreen,KConsFullScreen)));
    if (createError)
        return createError;

    // Run application code inside TRAP harness, wait keypress when terminated
    TRAPD(mainError, DoStartL());
    console->Printf(KTextPressAnyKey);
    console->Getch();

    delete console;
    delete cleanup;
    __UHEAP_MARKEND;

    return KErrNone;
}

#include "AllTests.h"