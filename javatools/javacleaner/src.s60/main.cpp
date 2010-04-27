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
* Description: Java platform 2.0 cleaner process (javacleaner.exe)
*              When OMJ is uninstalled, this program is executed to
*              uninstall all OMJ midlets, unregister OMJ
*              specific MIDlet starter and reregister old starter
*/


#include <e32std.h>
#include <e32base.h>
#include <apgcli.h> // for RApaLsSession
#include <e32property.h>
#include <swinstallerinternalpskeys.h>

#include "javauids.h"
#include "logger.h"


/**
 * Start Java Installer process with commandline options
 * that ask it to uninstall all OMJ midlets. Wait until
 * the uninstallation has been done.
 */
static void UninstallAllOmjMidlets()
{
    LOG(EJavaConverters, EInfo, "javacleaner: UninstallAllOmjMidlets: Going to start installer");

    RProcess rJavaInstaller;
    // Just command 'uninstallall', 256 is more than enough
    TBuf<256> commandLine;

    // Build command line used to pass all necessary info to Java Installer
    commandLine = _L("javainstallerstarter");
    commandLine.Append(_L(" uninstallall -forceuninstall -captainmsgs=no"));

    // Run installer silently
    commandLine.Append(_L(" -silent -skipotastatus"));

    // start JavaInstaller
#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
    TBuf<128> processName = _L("j9midps60");
#else // RD_JAVA_S60_RELEASE_5_0_IAD
    TBuf<128> processName = _L("javamidp");
#endif // RD_JAVA_S60_RELEASE_5_0_IAD

    TInt err = rJavaInstaller.Create(processName, commandLine);
    if (KErrNone == err)
    {
        LOG(EJavaConverters, EInfo, "javacleaner: UninstallAllOmjMidlets calling Rendezvous");
        // This call will wait until Java Installer exits (or panics)
        TRequestStatus status;
        rJavaInstaller.Logon(status);

        LOG(EJavaConverters, EInfo, "javacleaner: UninstallAllOmjMidlets calling Resume");
        rJavaInstaller.Resume();

        // now wait until Java Installer exits
        User::WaitForRequest(status);
        if (status.Int() != KErrNone)
        {
            ELOG1(EJavaConverters,
                  "javacleaner: UninstallAllOmjMidlets Installer exited with error %d",
                  status.Int());
        }
    }
    else
    {
        ELOG1(EJavaConverters,
              "javacleaner: UninstallAllOmjMidlets Cannot start Installer, error %d", err);
    }

    LOG(EJavaConverters, EInfo, "javacleaner: UninstallAllOmjMidlets calling RProcess::Close");
    // free resources before returning
    rJavaInstaller.Close();
}


/**
 * Start Java Installer process with commandline option
 * that asks it to delete all PS keys that Java Installer uses and
 * remove all unnecessary directories that OMJ has created.
 * Wait until the uninstallation has been done.
 */
static void CleanOmjDataAndPsKeys()
{
    LOG(EJavaConverters, EInfo, "javacleaner: CleanOmjDataAndPsKeys: Going to start installer");

    RProcess rJavaInstaller;
    // Just command 'removealldata', 256 is more than enough
    TBuf<256> commandLine;

    // Build command line used to pass all necessary info to Java Installer
    commandLine = _L("javainstallerstarter");
    commandLine.Append(_L(" removealldata"));

    // Run installer silently
    commandLine.Append(_L(" -silent"));

    // start JavaInstaller
#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
    TBuf<128> processName = _L("j9midps60");
#else // RD_JAVA_S60_RELEASE_5_0_IAD
    TBuf<128> processName = _L("javamidp");
#endif // RD_JAVA_S60_RELEASE_5_0_IAD

    TInt err = rJavaInstaller.Create(processName, commandLine);
    if (KErrNone == err)
    {
        LOG(EJavaConverters, EInfo, "javacleaner: CleanOmjDataAndPsKeys calling Rendezvous");
        // This call will wait until Java Installer exits (or panics)
        TRequestStatus status;
        rJavaInstaller.Logon(status);

        LOG(EJavaConverters, EInfo, "javacleaner: CleanOmjDataAndPsKeys calling Resume");
        rJavaInstaller.Resume();

        // now wait until Java Installer exits
        User::WaitForRequest(status);
        if (status.Int() != KErrNone)
        {
            ELOG1(EJavaConverters,
                  "javacleaner: CleanOmjDataAndPsKeys Installer exited with error %d",
                  status.Int());
        }
    }
    else
    {
        ELOG1(EJavaConverters,
              "javacleaner: CleanOmjDataAndPsKeys Cannot start Installer, error %d", err);
    }

    LOG(EJavaConverters, EInfo, "javacleaner: CleanOmjDataAndPsKeys calling RProcess::Close");
    // free resources before returning
    rJavaInstaller.Close();
}

/**
 * Return true if really uninstalling OMJ
 */
static TBool IsCleaningNeeded()
{
    TBool clean(EFalse);
    TInt  value(0);
    // Ignore errors, if the value cannot be read, cleaning is not done.
    RProperty::Get(KPSUidSWInstallerUiNotification, KSWInstallerUninstallation, value);
    if (0 == value)
    {
        // Native installer is updating OMJ. Do not clean
        LOG(EJavaConverters, EInfo,
            "javacleaner: IsCleaningNeeded(): Updating OMJ. No cleaning needed.");
        clean = EFalse;
    }
    else
    {
        // Really uninstalling OMJ
        clean = ETrue;
    }

    return clean;
}

/**
 * Create cleanup stack and run the cleaner code inside TRAP harness
 * to log unexpected leaves.
 */
TInt E32Main()
{
    __UHEAP_MARK;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();

    if (IsCleaningNeeded())
    {
        UninstallAllOmjMidlets();
        // Must wait for a moment so that the Java Installer instance
        // started by previous call exits fully
        User::After(500000);
        CleanOmjDataAndPsKeys();
    }

    delete cleanupStack;
    __UHEAP_MARKEND;
    return KErrNone;
}
// eof
