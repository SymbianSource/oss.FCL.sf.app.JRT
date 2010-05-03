/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Java platform 2.0 javarestoreconverter process.
*              Java restore executes this when restoring Java 1.x
*              applications that must be converted to Java 2.x
*              applications.
*/


#include <e32std.h>
#include <e32base.h>
#include <f32file.h>

#include "javauids.h"
#include "restoreconvertmidlet.h"
#include "logger.h"

// The secure Uid of javabackup.exe
_LIT_SECURE_ID(KJavaBackupSecureID, 0x1028246F);


/**
 * Allow starting process only from Java Restore,
 * allow running only one instance of the process,
 * create file server connection,
 * create active scheduler and start it
 *
 */
static void RestoreConvertJavaAppsL()
{
    _LIT(KPreinstallerMatch, "javarestoreconverter*");
    TFindProcess find(KPreinstallerMatch);
    TFullName ignoreName;
    find.Next(ignoreName);

    // Can only have one javarestoreconverter. If a second is found exit
    if (find.Next(ignoreName) == KErrNone)
    {
        ELOG(EJavaPreinstaller, "javarestoreconverter.exe already running.");
        User::Leave(KErrAlreadyExists);
    }

    // The only time that this application should be executed
    // is when java restore calls it.
    // Allow starting javarestoreconverter.exe from test programs in
    // debug builds.
#ifndef _DEBUG

    if (User::CreatorSecureId() != KJavaBackupSecureID)
    {
        ELOG(EJavaConverters,
             "ConvertJavaAppsL: Mismatch in secure ID, only java backup can launch this exe.");
        User::Leave(KErrPermissionDenied);
    }
#endif

    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    CActiveScheduler* as = new(ELeave) CActiveScheduler();

    // Install active scheduler
    CActiveScheduler::Install(as);
    CleanupStack::PushL(as);

    // Setup and start the MIDlet conversion
    CRestoreConvertMIDlet* si = CRestoreConvertMIDlet::NewLC(fs);

    LOG(EJavaConverters, EInfo, "RestoreConvertJavaAppsL: Call CRestoreConvertMIDlet::Start()");

    si->Start();

    // Start active scheduler, starts conversion
    LOG(EJavaConverters, EInfo, "RestoreConvertJavaAppsL: Starting CActiveScheduler");
    CActiveScheduler::Start();

    LOG(EJavaConverters, EInfo, "RestoreConvertJavaAppsL: Cleaning up");

    CleanupStack::PopAndDestroy(si);
    CleanupStack::PopAndDestroy(as);
    CleanupStack::PopAndDestroy(&fs); // close connection to file server

    return;
}


/**
 * Create cleanup stack and run the javarestoreconverter code inside TRAP
 * harness to log unexpected leaves.
 */
TInt E32Main()
{
    CTrapCleanup* cleanupStack = CTrapCleanup::New();

    TRAPD(err, RestoreConvertJavaAppsL());
    if (err != KErrNone)
    {
        ELOG1(EJavaConverters, "RestoreConvertJavaAppsL:Main error %d", err);
    }

    delete cleanupStack;
    return err;
}
