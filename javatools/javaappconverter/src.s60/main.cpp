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
* Description: Java platform 2.0 javaappconverter process
*
*/


#include <e32std.h>
#include <e32base.h>
#include <f32file.h>

#include "javauids.h"
#include "silentmidletconvert.h"
#include "logger.h"

_LIT_SECURE_ID(KJavaPostUpdaterSecureID, KJavaPostUpdaterUid);


/**
 * If uids file exists, the old S60 midlets have not yet been
 * converted for OMJ java environment.
 */
static TBool isConversionNeeded(RFs& aFs)
{
    TFileName importDataFile(KUidsImportDataFilePathName);
    importDataFile.Append(KUidsImportDataFileName);

    TUint attributes;
    TInt  err = aFs.Att(importDataFile, attributes);
    if (KErrNone != err)
    {
        return EFalse;
    }
    else
    {
        return ETrue;
    }
}


/**
 * Allow starting process only from Java Captain,
 * allow running only one instance of the process,
 * create file server connection,
 * create active scheduler and start it
 *
 * @return ETrue if conversion was done
 */
static TBool convertJavaAppsL()
{
    JELOG2(EJavaConverters);

    TBool conversionExecuted = EFalse;

    _LIT(KPreinstallerMatch, "javaappconverter*");
    TFindProcess find(KPreinstallerMatch);
    TFullName ignoreName;
    find.Next(ignoreName);

    // Can only have one javaappconverter. If a second is found exit
    if (find.Next(ignoreName) == KErrNone)
    {
        ELOG(EJavaPreinstaller, "javaappconverter.exe already running.");
        return ETrue;
    }

    // The only time that this application should be executed
    // is when javapostupdater calls it.
    if (User::CreatorSecureId() != KJavaPostUpdaterSecureID)
    {
        ELOG(EJavaConverters,
             "ConvertJavaAppsL: Mismatch in secure ID, only javapostupdater can launch this exe.");
        User::Leave(KErrPermissionDenied);
    }

    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    if (isConversionNeeded(fs))
    {
        CActiveScheduler* as = new(ELeave) CActiveScheduler();

        // Install active scheduler
        CActiveScheduler::Install(as);
        CleanupStack::PushL(as);

        // Setup and start the MIDlet conversion
        CSilentMIDletConvert* si = CSilentMIDletConvert::NewLC(fs);

        LOG(EJavaConverters, EInfo, "ConvertJavaAppsL: Call CSilentMIDletConvert::Start()");

        si->Start();

        // Start active scheduler, starts conversion
        LOG(EJavaConverters, EInfo, "ConvertJavaAppsL: Starting CActiveScheduler");
        CActiveScheduler::Start();

        LOG(EJavaConverters, EInfo, "ConvertJavaAppsL: Cleaning up");

        CleanupStack::PopAndDestroy(si);
        CleanupStack::PopAndDestroy(as);

        conversionExecuted = ETrue;
    }
    else
    {
        LOG(EJavaConverters, EInfo,
            "ConvertJavaAppsL: Conversion has already been done.");
    }

    CleanupStack::PopAndDestroy(&fs); // close connection to file server

    return conversionExecuted;
}


/**
 * Create cleanup stack and run the javaappconverter code inside TRAP harness
 * to log unexpected leaves.
 */
TInt E32Main()
{
    CTrapCleanup* cleanupStack = CTrapCleanup::New();

    TBool conversionExecuted(EFalse);
    TRAPD(err, conversionExecuted = convertJavaAppsL());
    if (err != KErrNone)
    {
        ELOG1(EJavaConverters, "ConvertJavaAppsL:Main error %d", err);
    }
    if (!conversionExecuted)
    {
        // Use this error code to tell the process that started us,
        // that we did nothing because conversion was already done.
        err = KErrAlreadyExists;
    }

    delete cleanupStack;
    return err;
}
// eof
