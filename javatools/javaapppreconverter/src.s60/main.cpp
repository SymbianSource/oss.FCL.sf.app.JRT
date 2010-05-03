/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Java platform 2.0 javaapppreconverter process
*
*/


#include <e32std.h>
#include <e32base.h>
#include <f32file.h>

#include "javauids.h"
#include "preconverter.h"
#include "noarmlogs.h"

_LIT_SECURE_ID(KAppInstUiSecureID, KAppInstUiUid);
_LIT_SECURE_ID(KAppSisLauncherSecureID, KAppSisLauncherUid);

/**
 * Create file server connection.
 * If data storage already exists, exit
 * Create active scheduler and start it.
 */
static void ExecutePreConverterL()
{
    JELOG2(EJavaConverters);

    // Do not check the starter in emulator environment to make
    // testing possible.
#ifndef __WINSCW__
    // The only time that this application should be executed
    // is when native installer starts it
    if ((User::CreatorSecureId() != KAppInstUiSecureID) &&
            (User::CreatorSecureId() != KAppSisLauncherSecureID))
    {
        ELOG(EJavaConverters,
             "javaapppreconverter: ExecutePreConverterL: Mismatch in secure "
             "ID, only AppInstUi or SisLauncher can launch this exe.");
        ELOG1(EJavaConverters,
              "javaapppreconverter: ExecutePreConverterL: Starter has SID %x ",
              User::CreatorSecureId().iId);
        User::Leave(KErrPermissionDenied);
    }
#endif

    CActiveScheduler* as = new(ELeave) CActiveScheduler();

    // Install active scheduler
    CActiveScheduler::Install(as);
    CleanupStack::PushL(as);

    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    // Setup and start the preconversion
    CPreConverter* pre = CPreConverter::NewLC(fs);

    LOG(EJavaConverters,
        EInfo, "javaapppreconverter: ExecutePreConverterL: Call CPreConverter::Start()");

    pre->Start();

    // Start active scheduler
    LOG(EJavaConverters,
        EInfo, "javaapppreconverter: ExecutePreConverterL: Starting CActiveScheduler");
    CActiveScheduler::Start();

    LOG(EJavaConverters, EInfo, "javaapppreconverter: ExecutePreConverterL: Cleaning up");

    CleanupStack::PopAndDestroy(pre);
    CleanupStack::PopAndDestroy(&fs); // close connection to file server
    CleanupStack::PopAndDestroy(as);
}

/**
 * Create cleanup stack and run the javaapppreconverter code inside TRAP
 * harness to log unexpected leaves.
 */
TInt E32Main()
{
    CTrapCleanup* cleanupStack = CTrapCleanup::New();

    TInt error = KErrNone;
    TRAP(error,ExecutePreConverterL());
    if (error != KErrNone)
    {
        ELOG1(EJavaConverters, "javaapppreconverter: ExecutePreConverterL:Main error %d", error);
    }

    delete cleanupStack;
    return error;
}
