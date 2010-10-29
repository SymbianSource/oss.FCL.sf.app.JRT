/*
* Copyright (c) 2008 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  OMJ S60 preinstaller process
 *
*/


#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <e32property.h>
#include <javadomainpskeys.h>

#include "javauids.h"
#include "javaoslayer.h"
#include "javasymbianoslayer.h"
#include "silentmidletinstall.h"
#include "logger.h"

using namespace java::util;


_LIT_SECURE_ID(KJavaCaptainSecureID, KJavaCaptainUid);

#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
// This file is used to indicate when preinstallation has been done and
// converting old S60 midlets to OMJ midlets can start.
_LIT(KConversionOngoing, "D:\\OMJConverting.dat");  // codescanner::driveletters
#endif


/**
 * The PS keys can be created from this process because javapreinstaller
 * has the same Symbian secure ID as javainstaller and so javainstaller
 * can later destroy the keys if necessary.
 */
static void createJavaInstallerPSKeys()
{
    LOG(EJavaPreinstaller, EInfo,
        "createJavaInstallerPSKeys: Going to create Java Installer PS keys");

    // any process can read the values of the PS keys
    _LIT_SECURITY_POLICY_PASS(KReadPolicy);
    // update allowed by processes with write system data capability
    _LIT_SECURITY_POLICY_C1(KWritePolicy, ECapabilityWriteDeviceData);

    TInt err = RProperty::Define(KUidSystemCategory, KPSUidJavaLatestInstallation,
                                 RProperty::EInt, KReadPolicy, KWritePolicy);
    if (KErrAlreadyExists == err)
    {
        err = KErrNone; // Ignore KErrAlreadyExists status.
    }
    if (err != KErrNone)
    {
        ELOG1(EJavaPreinstaller, "createJavaInstallerPSKeys: "
              "creating KPSUidJavaLatestInstallation failed with error %d", err);
    }

    err = RProperty::Define(KUidSystemCategory, KPSUidJavaLatestInstallationProgress,
                            RProperty::EInt, KReadPolicy, KWritePolicy);
    if (KErrAlreadyExists == err)
    {
        err = KErrNone; // Ignore KErrAlreadyExists status.
    }
    if (err != KErrNone)
    {
        ELOG1(EJavaPreinstaller, "createJavaInstallerPSKeys: "
              "creating KPSUidJavaLatestInstallationProgress failed with error %d", err);
    }

    err = RProperty::Define(KUidSystemCategory, KPSUidJavaLatestInstallationState,
                            RProperty::EInt, KReadPolicy, KWritePolicy);
    if (KErrAlreadyExists == err)
    {
        err = KErrNone; // Ignore KErrAlreadyExists status.
    }
    if (err != KErrNone)
    {
        ELOG1(EJavaPreinstaller, "createJavaInstallerPSKeys: "
              "creating KPSUidJavaLatestInstallationState failed with error %d", err);
    }
}

/**
 * Create file server connection,
 * create active scheduler,
 * add CSilentMIDletInstall active object to it and start it
 */
static void startPreinstallationL()
{
    LOG(EJavaPreinstaller, EInfo, "startPreinstallationL called");

    CActiveScheduler* as = new(ELeave) CActiveScheduler();

    // Install active scheduler
    CActiveScheduler::Install(as);
    CleanupStack::PushL(as);

    RFs fs;  // codescanner::rfs
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    // Setup the MIDlet preinstaller and add it to the active scheduler
    CSilentMIDletInstall* si = CSilentMIDletInstall::NewLC(fs);

    LOG(EJavaPreinstaller, EInfo, "startPreinstallationL: Call CSilentMIDletInstall::Start()");

    // Initialize the MIDlet preinstaller
    si->Start();

    // Start active scheduler. Preinstallation starts.
    LOG(EJavaPreinstaller, EInfo, "startPreinstallationL: Starting CActiveScheduler");
    CActiveScheduler::Start(); // codescanner::activestart

    // Now preinstallation has been done
    LOG(EJavaPreinstaller, EInfo, "startPreinstallationL: Cleaning up");
#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
    // Notify possibly waiting 'javaupdater.exe' process that preinstallation
    // has been done and that it can continue.
    TInt err = fs.Delete(KConversionOngoing);
    LOG1(EJavaPreinstaller,
         EInfo,
         "startPreinstallationL: Delete flag file returned status code %d",
         err);
#endif

    CleanupStack::PopAndDestroy(si);
    CleanupStack::PopAndDestroy(&fs); // close connection to file server
    CleanupStack::PopAndDestroy(as);
}


#ifdef RD_JAVA_S60_RELEASE_5_0_ROM
/**
 * If 'javaafterflashconverter.exe' is running, wait until it exits
 * so that converter and preinstaller don't try to use Java Installer
 * simultaneously.
 */
static void waitUntilAfterFlashConverterExits()
{
    LOG(EJavaPreinstaller, EInfo, "waitUntilAfterFlashConverterExits called");

    TFullName processName;
    _LIT(KJavaAFConverterProcess, "javaafterflashconverter*");
    TFindProcess finder(KJavaAFConverterProcess);

    if (finder.Next(processName) != KErrNotFound)
    {
        RProcess afConverterProcess;  // codescanner::resourcenotoncleanupstack
        TInt err = afConverterProcess.Open(finder);

        if (KErrNone != err)
        {
            WLOG1(EJavaPreinstaller,
                "waitUntilAfterFlashConverterExits: Process open err: %d", err);
        }
        else
        {
            if (EExitPending == afConverterProcess.ExitType())
            {
                // Converter is still running. Wait until it exits.
                LOG(EJavaPreinstaller, EInfo,
                    "waitUntilAfterFlashConverterExits going to wait until converter exits");
                TRequestStatus status;
                afConverterProcess.Logon(status);
                User::WaitForRequest(status);  // codescanner::userWaitForRequest
            }

            afConverterProcess.Close();
        }
    }
}
#endif


/**
 * Allow starting process only from Java Captain.
 * Execute actual preinstaller code in CSilentMIDletInstall.
 */
#ifdef RD_JAVA_S60_RELEASE_5_0_ROM
void preinstallerMainL(int argc, const char *argv[])
#else
void preinstallerMainL()
#endif
{
    // The only time that this application should be executed
    // is when Java Captain calls it.
    if (User::CreatorSecureId() != KJavaCaptainSecureID)
    {
        ELOG(EJavaPreinstaller,
             "preinstallerMainL: Mismatch in secure ID, only Java Captain can launch this exe.");
        User::Leave(KErrPermissionDenied);
    }

    // Create the PS keys that Java Installer will update already now
    // so that other processes starting during device boot can start
    // immediately listening to the keys.
    createJavaInstallerPSKeys();

#ifdef RD_JAVA_S60_RELEASE_5_0_ROM
    if (argc > 1)
    {
        // The max size of a preinstaller command line parameter is 80
        TBufC8<80> param((const TUint8 *)(argv[1]));  // codescanner::accessArrayElementWithoutCheck2
        if (param == _L8("waitafconversion"))
        {
            waitUntilAfterFlashConverterExits();
        }
   }
#endif

    startPreinstallationL();
}

/**
 * Using this kind of dllMain() allows starting preinstaller from
 * the same starter process as installer and runtime (so that they have the same
 * Symbian secure Uid and can access the same data cage).
 */
#ifdef RD_JAVA_S60_RELEASE_5_0_ROM
int dllMain(int argc, const char *argv[])
#else
int dllMain(int /* argc */, const char */*argv*/[])
#endif
{
    JavaOsLayer::startUpTrace("PREINSTALLER main() start", -1, -1);

    User::RenameProcess(_L("javapreinstaller"));
    User::RenameThread(_L("JavaPreinstallerMainThread"));

    LOG(EJavaPreinstaller, EInfo, "dllMain: Renaming process and thread done");

    // Make sure that this thread has always cleanup stack
    CTrapCleanup* cleanupStack = CTrapCleanup::New();

#ifdef RD_JAVA_S60_RELEASE_5_0_ROM
    TRAPD(err, preinstallerMainL(argc, argv));
#else
    TRAPD(err, preinstallerMainL());
#endif
    if (KErrNone != err)
    {
        ELOG1(EJavaPreinstaller, "dllMain: preinstallerMainL leaved with error %d", err);
    }

    delete cleanupStack;

    LOG1(EJavaPreinstaller, EInfo, "PREINSTALLER main() exit = %d", err);
    JavaOsLayer::startUpTrace("PREINSTALLER main() end", -1, -1);
    return err;
}

EXPORT_C FuncPtr findDllMethod(const char* funcName)
{
    FuncPtr ptr = 0;
    if (strcmp(funcName, "dllMain") == 0)
    {
        ptr = (FuncPtr)dllMain;
    }
    return ptr;
}
// eof
