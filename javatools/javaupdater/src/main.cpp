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
* Description:  OMJ S60 environment updater process.
*               When OMJ is installed, this program is executed to allow
*               new Java environment taken into use.
*/


#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <apgtask.h>
#include <w32std.h>
#include <swinstapi.h>
#include <centralrepository.h>
#include <hal.h>

#include "javauids.h"
#include "systemamstraderprivateclient.h"
#include "logger.h"

// This file is used to indicate when converting old S60 midlets
// to OMJ midlets has been done.
_LIT(KConversionOngoing, "D:\\OMJConverting.dat");


/**
 * Kill old and running javaregistry.exe. After killing next call of
 * Registry API re-starts the new server process.
 */
static void KillRegistry()
{
    JELOG2(EJavaConverters);
    TFullName processName;
    _LIT(KJavaRegistryProcess, "javaregistr*");
    TFindProcess finder(KJavaRegistryProcess);

    if (finder.Next(processName) != KErrNotFound)
    {
        RProcess processToKill;
        TInt err = processToKill.Open(finder);

        if (KErrNone != err)
        {
            WLOG1(EJavaConverters,
                  "javaupdater: KillRegistry: Process open: %d", err);
        }
        else
        {
            LOG(EJavaConverters, EInfo, "javaupdater: KillRegistry: Killing javaregistry");
            processToKill.Kill(KErrNone);
            processToKill.Close();
        }
    }
    else
    {
        WLOG(EJavaConverters, "javaupdater: KillRegistry: Registry process not found");
    }
}

/**
 * Kill running System AMS processes.
 */
static void KillSystemAms()
{
    JELOG2(EJavaConverters);

    TBool connectionOK(EFalse);
    TRequestStatus status(KRequestPending);
    RSystemAMSTraderPrivateClient amsClient;

    connectionOK = amsClient.ShutdownJavaEnv(status);
    if (connectionOK)
    {
        // systemams process in this device supports nice shutdown.
        // Wait until the S60 java environment has been shutdown
        LOG(EJavaConverters,
            EInfo, "javaupdater: KillSystemAms: Waiting for systemams nice shutdown");
        User::WaitForRequest(status);
        LOG(EJavaConverters,
            EInfo, "javaupdater: KillSystemAms: Systemams has shutdown");
        amsClient.Close();
    }
    else
    {
        // must kill SystemAMS trader and SystemAMS itself
        TFullName processName;
        _LIT(KJavaSystemAmsTraderProcess, "SystemAMS.EXE*");
        TFindProcess finder(KJavaSystemAmsTraderProcess);

        if (finder.Next(processName) == KErrNone)
        {
            RProcess processToKill;
            TInt err = processToKill.Open(finder);

            if (KErrNone != err)
            {
                WLOG1(EJavaConverters,
                      "javaupdater: KillSystemAms: Cannot open SystemAMS trader process: %d", err);
            }
            else
            {
                LOG(EJavaConverters, EInfo,
                    "javaupdater:KillSystemAms: Killing SystemAMS trader");
                processToKill.Kill(KErrNone);
                processToKill.Close();
            }
        }

        // now kill SystemAMS itself
        _LIT(KJavaSystemAmsProcess, "SystemAMSCore.EXE*");
        finder.Find(KJavaSystemAmsProcess);

        if (finder.Next(processName) == KErrNone)
        {
            RProcess processToKill;
            TInt err = processToKill.Open(finder);

            if (KErrNone != err)
            {
                WLOG1(EJavaConverters,
                      "javaupdater: KillSystemAms: Cannot open SystemAMS process: %d", err);
            }
            else
            {
                LOG(EJavaConverters, EInfo, "javaupdater: KillSystemAms Killing SystemAMS ");
                processToKill.Kill(KErrNone);
                processToKill.Close();
            }
        }
    }
}

/**
 * Try to kill old javapreinstaller
 */
static void KillOldPreinstaller()
{
    JELOG2(EJavaConverters);

    TFullName processName;
    _LIT(KJavaOldPreinstallerProcess, "MIDP2SilentMIDletInstall.exe*");
    TFindProcess finder(KJavaOldPreinstallerProcess);

    if (finder.Next(processName) == KErrNone)
    {
        RProcess processToKill;
        TInt err = processToKill.Open(finder);

        if (KErrNone != err)
        {
            WLOG1(EJavaConverters,
                  "javaupdater: KillOldPreinstallerL: Process open: %d", err);
        }
        else
        {
            LOG(EJavaConverters, EInfo, "javaupdater: Killing MIDP2SilentMIDletInstall");
            processToKill.Kill(KErrNone);
            processToKill.Close();
        }
    }
}

/**
 * Start OMJ trader (systemams.exe) in IAD mode.
 * It will start Java Captain in IAD mode and
 * it will start Java Preinstaller in IAD mode.
 */
_LIT(KOmjTraderExe, "systemams.exe");
_LIT(KOmjTraderArg, "iad");
static void StartOmjTrader()
{
    JELOG2(EJavaConverters);
    RProcess proc;
    int rc = proc.Create(KOmjTraderExe, KOmjTraderArg);
    if (rc == KErrNone)
    {
        proc.Resume();
        LOG(EJavaConverters, EInfo, "javaupdater: StartOmjTrader systemams.exe was started ok");
    }
    else
    {
        ELOG1(EJavaConverters, "javaupdater: StartOmjTrader failed: %d", rc);
    }
    proc.Close();
}

/**
 * Copies (and configures) the certificates from legacy java
 */
_LIT(KJavaCertfictatesConfiguratorExe, "javacertificatesconfigurator.exe");
static void ConfigureCertificates()
{
    JELOG2(EJavaConverters);
    RProcess proc;
    int rc = proc.Create(KJavaCertfictatesConfiguratorExe, KNullDesC());
    if (rc == KErrNone)
    {
        proc.Resume();
        TRequestStatus status;
        proc.Logon(status);
        User::WaitForRequest(status);
    }
    else
    {
        ELOG1(EJavaConverters, "javaupdater: ConfigureCertificates failed: %d", rc);
    }
    proc.Close();
}

/**
 * Wait until preinstallation of OMJ midlets has been done.
 */
static void WaitUntilPreinstallerReady()
{
    // Create a temporary file that javapreinstaller.exe will
    // then destroy after reinstallation has been done
    TUint value;
    RFs   fs;
    RFile flagFile;
    TBool flagFileExists(ETrue);
    TInt err = fs.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "javaupdater: WaitUntilPreinstallerReady: Cannot connect to RFs, err %d",
              err);
        return;
    }
    err = flagFile.Replace(fs, KConversionOngoing, EFileShareAny|EFileWrite);
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "javaupdater: WaitUntilPreinstallerReady: Cannot create conversion flag file, err %d",
              err);
        fs.Close();
        return;
    }
    flagFile.Close();

    // Now wait until the file is destroyed
    do
    {
        TRequestStatus status;
        fs.NotifyChange(ENotifyFile, status, KConversionOngoing);
        User::WaitForRequest(status);
        LOG1(EJavaConverters,
             EInfo,
             "javaupdater: WaitUntilPreinstallerReady: Wait ended with status %d",
             status.Int());

        // Check whether the flag file has been destroyed
        err = fs.Att(KConversionOngoing, value);
        if ((KErrNotFound == err) || (KErrPathNotFound == err))
        {
            flagFileExists = EFalse;
        }
        else if (KErrNone == err)
        {
            flagFileExists = ETrue;
            LOG(EJavaConverters,
                EInfo,
                "javaupdater: WaitUntilPreinstallerReady: Flag file still exists, must wait more");
        }
        else
        {
            WLOG1(EJavaConverters,
                  "javaupdater: WaitUntilPreinstallerReady: Unexpected error %d, stop waiting",
                  err);
            flagFileExists = EFalse;
        }

    }
    while (flagFileExists);

    fs.Close();
}

/**
 * Create cleanup stack and run the cleaner code inside TRAP harness
 * to log unexpected leaves.
 */
TInt E32Main()
{
    __UHEAP_MARK;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    KillRegistry();
    KillSystemAms();
    KillOldPreinstaller();
    ConfigureCertificates();
    StartOmjTrader();
    WaitUntilPreinstallerReady();
    delete cleanupStack;
    __UHEAP_MARKEND;
    return KErrNone;
}
