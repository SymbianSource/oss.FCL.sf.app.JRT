/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Simple application for testing Sif API
*              (install, uninstall, get component info)
*/


#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <e32property.h>
#include <stdio.h>
#include <s60commonutils.h>

#include <usif/sif/sif.h>


#include "logger.h"

using namespace Usif;

_LIT(KTestMIDlet, "E:\\stopwatch10midp2.jad");
_LIT(KTestMIDlet2, "E:\\stopwatch11midp2.jar");
_LIT(KTestMIDlet3, "E:\\Private\\10281e17\\SimpleRMS.jar");
_LIT(KUserName, "user");
_LIT(KPassWord, "password");
_LIT(KEmptyString, "");
_LIT(KSourceUrl, "\\sourceurl\\foo");
_LIT(KCharSet, "charset");
_LIT(KJadMimeType, "text/vnd.sun.j2me.app-descriptor");
_LIT(KJarMimeType, "application/java-archive");


/**
 * Installs a component by file name
 */
static void sifSimplestInstallL()
{
    LOG(EJavaConverters, EInfo,
        "testsifapi: sifInstallL: Called");

    RSoftwareInstall installer;
    TInt err = installer.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testsifapi: sifInstallL: Cannot connect to RSoftwareInstall, err %d", err);
        User::Leave(err);
    }
    CleanupClosePushL(installer);

    TRequestStatus status;

    installer.Install(KTestMIDlet, status);

    User::WaitForRequest(status);

    LOG1(EJavaConverters, EInfo,
        "testsifapi: sifInstallL: The return status of install operation was %d", status.Int());

    // free resources before returning
    CleanupStack::PopAndDestroy(1);
}


/**
 * This test case requires that you start Java Installer
 * manually before executing this one and keep it running
 * this test case ends.
 *
 *
 * Keep the installer running until 1) installation has failed
 * 2) uninstallation has failed and 3) getComponentInfo had failed
 * because the installer is already running.
 */
static void sifInstallerAlreadyRunningL()
{
    LOG(EJavaConverters, EInfo,
        "testsifapi: sifInstallerAlreadyRunningL: Called");

    RSoftwareInstall installer;
    TInt err = installer.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testsifapi: sifInstallerAlreadyRunningL: Cannot connect to RSoftwareInstall, err %d", err);
        User::Leave(err);
    }
    CleanupClosePushL(installer);

/*
    LOG(EJavaConverters, EInfo,
        "testsifapi: sifInstallerAlreadyRunningL: Starting the first installer process");
    TRequestStatus status;
    installer.Install(KTestMIDlet, status);

    // wait for second
    User::After(1000000);
*/

    LOG(EJavaConverters, EInfo,
        "testsifapi: sifInstallerAlreadyRunningL: Starting the concurrent installation");
    // Try to start concurrent installation, will fail
    TRequestStatus status2;
    installer.Install(KTestMIDlet2, status2, EFalse);
    User::WaitForRequest(status2);
    LOG1(EJavaConverters, EInfo,
        "testsifapi: sifInstallerAlreadyRunningL: The return status of concurrent install operation was %d", status2.Int());

    LOG(EJavaConverters, EInfo,
        "testsifapi: sifInstallerAlreadyRunningL: Starting the concurrent uninstallation");
    // Try to start concurrent uninstallation, will fail
    TRequestStatus status3;
    installer.Uninstall(15, status3, EFalse);
    User::WaitForRequest(status3);
    LOG1(EJavaConverters, EInfo,
        "testsifapi: sifInstallerAlreadyRunningL: The return status of concurrent uninstall operation was %d", status3.Int());

    LOG(EJavaConverters, EInfo,
        "testsifapi: sifInstallerAlreadyRunningL: Starting the concurrent get component info");
    // Try to start concurrent GetComponentInfo, will fail
    TRequestStatus status4;
    CComponentInfo *info = CComponentInfo::NewL();
    installer.GetComponentInfo(KTestMIDlet, *info, status4);
    User::WaitForRequest(status4);
    delete info;
    LOG1(EJavaConverters, EInfo,
        "testsifapi: sifInstallerAlreadyRunningL: The return status of concurrent GetComponentInfo operation was %d", status4.Int());

/*
    User::WaitForRequest(status);

    LOG1(EJavaConverters, EInfo,
        "testsifapi: sifInstallerAlreadyRunningL: The return status of install operation was %d", status.Int());
*/

    // free resources before returning
    CleanupStack::PopAndDestroy(1);
}

/**
 * Installs a component by file handle using opaque arguments/results
 */
static TInt sifByHandleAndArgsAndResultsInstallL()
{
    // Open file handle to the installation file
    RFs fs;
    TInt err = fs.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testsifapi: sifByHandleAndArgsAndResultsInstallL: Cannot connect to RFs, err %d", err);
        User::Leave(err);
    }
    CleanupClosePushL(fs);

    // If the file server session is not shared, USIF server crashes with KERN-EXEC 44
    err = fs.ShareProtected();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testsifapi: sifByHandleAndArgsAndResultsInstallL: Cannot share RFs, err %d", err);
        User::Leave(err);
    }

// TEMP TEST
    RFile installFile;
    err = installFile.Open(fs, KTestMIDlet3, EFileShareReadersOnly | EFileRead);
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testsifapi: sifByHandleAndArgsAndResultsInstallL: Cannot open "
              "E:\\Private\\10281e17\\SimpleRMS.jar, err %d", err);
        User::Leave(err);
    }
    CleanupClosePushL(installFile);

    LOG(EJavaConverters, EInfo,
        "testsifapi: sifByHandleAndArgsAndResultsInstallL: installFile handle opened");

    RSoftwareInstall installer;
    err = installer.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testsifapi: sifByHandleAndArgsAndResultsInstallL: Cannot connect to RSoftwareInstall, err %d", err);
        User::Leave(err);
    }
    CleanupClosePushL(installer);

    LOG(EJavaConverters, EInfo,
        "testsifapi: sifByHandleAndArgsAndResultsInstallL: RSoftwareInstall connected");

    TRequestStatus status;
    COpaqueNamedParams *arguments = COpaqueNamedParams::NewL();
    CleanupStack::PushL(arguments);
    COpaqueNamedParams *results = COpaqueNamedParams::NewL();
    CleanupStack::PushL(results);

    // Silent installation request
    arguments->AddIntL(KSifInParam_InstallSilently, 1);

    // drive E:
    RArray<TInt> intArray;
    CleanupClosePushL(intArray);
    intArray.AppendL(4);
    arguments->AddIntArrayL(KSifInParam_Drive, intArray);
    CleanupStack::PopAndDestroy(&intArray);

    // TSifPolicy::EAllowed == Yes
    arguments->AddIntL(KSifInParam_PerformOCSP, Usif::EAllowed);
    arguments->AddIntL(KSifInParam_IgnoreOCSPWarnings, Usif::EAllowed);

    arguments->AddIntL(KSifInParam_AllowUpgrade, Usif::EAllowed);
    arguments->AddIntL(KSifInParam_AllowUntrusted, Usif::EAllowed);
    arguments->AddIntL(KSifInParam_AllowOverwrite, Usif::EAllowed);
    arguments->AddIntL(KSifInParam_AllowDownload, Usif::EAllowed);

    arguments->AddStringL(KSifInParam_UserName, KUserName);
    arguments->AddStringL(KSifInParam_Password, KPassWord);

    arguments->AddStringL(KSifInParam_SourceUrl, KSourceUrl);

    arguments->AddIntL(KSifInParam_IAP, 3);

    arguments->AddStringL(KSifInParam_Charset, KEmptyString);

    arguments->AddStringL(KSifInParam_MimeType, KJarMimeType);


    LOG(EJavaConverters, EInfo,
        "testsifapi: sifByHandleAndArgsAndResultsInstallL: arguments created");

    installer.Install(installFile, *arguments, *results, status);

    User::WaitForRequest(status);

    LOG1(EJavaConverters, EInfo,
        "testsifapi: sifByHandleAndArgsAndResultsInstallL: The return status of install operation was %d", status.Int());


    TInt componentId = 0;
    intArray = results->IntArrayByNameL(KSifOutParam_ComponentId);
    if (intArray.Count() > 0)
    {
        componentId = intArray[0];
        LOG1(EJavaConverters, EInfo,
            "testsifapi: sifByHandleAndArgsAndResultsInstallL: Component id was %d", componentId);
    }
    else
    {
        LOG(EJavaConverters, EInfo,
            "testsifapi: sifByHandleAndArgsAndResultsInstallL: No component id was returned");
    }

    // free resources before returning
    CleanupStack::PopAndDestroy(5);

    return componentId;
}

/**
 * Installs a component by file handle using other opaque arguments than
 * sifByHandleAndArgsAndResultsInstallL()
 */
static TInt secondSifByFileAndArgsAndResultsInstallL()
{
    RSoftwareInstall installer;
    TInt err = installer.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testsifapi: secondSifByFileAndArgsAndResultsInstallL: Cannot connect to RSoftwareInstall, err %d", err);
        User::Leave(err);
    }
    CleanupClosePushL(installer);

    LOG(EJavaConverters, EInfo,
        "testsifapi: secondSifByFileAndArgsAndResultsInstallL: RSoftwareInstall connected");

    TRequestStatus status;
    COpaqueNamedParams *arguments = COpaqueNamedParams::NewL();
    CleanupStack::PushL(arguments);
    COpaqueNamedParams *results = COpaqueNamedParams::NewL();
    CleanupStack::PushL(results);

    // Silent installation request
    arguments->AddIntL(KSifInParam_InstallSilently, 1);

    // illegal drive number 33
    RArray<TInt> intArray;
    CleanupClosePushL(intArray);
    intArray.AppendL(33);
    arguments->AddIntArrayL(KSifInParam_Drive, intArray);
    CleanupStack::PopAndDestroy(&intArray);

    // Usif::ENotAllowed == No
    arguments->AddIntL(KSifInParam_PerformOCSP, Usif::ENotAllowed);
    arguments->AddIntL(KSifInParam_IgnoreOCSPWarnings, Usif::ENotAllowed);

    //arguments->AddIntL(KSifInParam_AllowUpgrade, Usif::ENotAllowed);
    //arguments->AddIntL(KSifInParam_AllowUntrusted, Usif::ENotAllowed);
    //arguments->AddIntL(KSifInParam_AllowOverwrite, Usif::ENotAllowed);
    //arguments->AddIntL(KSifInParam_AllowDownload, Usif::ENotAllowed);

    arguments->AddStringL(KSifInParam_UserName, KEmptyString);
    arguments->AddStringL(KSifInParam_Password, KEmptyString);

    arguments->AddStringL(KSifInParam_SourceUrl, KEmptyString);

    arguments->AddIntL(KSifInParam_SNAP, 8);

    arguments->AddStringL(KSifInParam_Charset, KCharSet);

    arguments->AddStringL(KSifInParam_MimeType, KJarMimeType);

    // forcecancel argument value is ignored, forcecancel is set if the value length > 0
    //arguments->AddStringL(_L("-forcecancel"), KCharSet);

    LOG(EJavaConverters, EInfo,
        "testsifapi: secondSifByFileAndArgsAndResultsInstallL: arguments created");

    installer.Install(KTestMIDlet2, *arguments, *results, status);

    User::WaitForRequest(status);

    LOG1(EJavaConverters, EInfo,
        "testsifapi: secondSifByFileAndArgsAndResultsInstallL: The return status of install operation was %d", status.Int());

    TInt componentId = 0;
    TRAP(err, intArray = results->IntArrayByNameL(KSifOutParam_ComponentId));
    if (err == KErrNone && intArray.Count() > 0)
    {
        componentId = intArray[0];
        LOG1(EJavaConverters, EInfo,
            "testsifapi: secondSifByFileAndArgsAndResultsInstallL: Component id was %d", componentId);
    }
    else
    {
        LOG1(EJavaConverters, EInfo,
             "testsifapi: secondSifByFileAndArgsAndResultsInstallL: No component id was returned, err=%d", err);
    }

    // free resources before returning
    CleanupStack::PopAndDestroy(3);

    return componentId;
}

static void sifUninstallL(TInt &aComponentId)
{
    LOG1(EJavaConverters, EInfo,
        "testsifapi: sifUninstallL: Uninstall component (id %d)", aComponentId);

    RSoftwareInstall installer;
    TInt err = installer.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testsifapi: sifUninstallL: Cannot connect to RSoftwareInstall, err %d", err);
        User::Leave(err);
    }
    CleanupClosePushL(installer);

    TRequestStatus status;
    COpaqueNamedParams *arguments = COpaqueNamedParams::NewL();
    CleanupStack::PushL(arguments);
    COpaqueNamedParams *results = COpaqueNamedParams::NewL();
    CleanupStack::PushL(results);

    // Silent uninstallation request
    arguments->AddIntL(KSifInParam_InstallSilently, 1);

    installer.Uninstall(aComponentId, *arguments, *results, status, EFalse);

    User::WaitForRequest(status);

    LOG1(EJavaConverters, EInfo,
        "testsifapi: sifUninstallL: The return status of uinstall operation was %d", status.Int());

    // free resources before returning
    CleanupStack::PopAndDestroy(3);
}


static void logComponentInfoNode(const CComponentInfo::CNode& aNode)
{
    std::wstring buf((wchar_t*) aNode.SoftwareTypeName().Ptr(), aNode.SoftwareTypeName().Length());
    LOG1WSTR(EJavaConverters, EInfo,
        "SoftwareTypeName is %s",
        buf.c_str());

    std::wstring buf2((wchar_t*) aNode.ComponentName().Ptr(), aNode.ComponentName().Length());
    LOG1WSTR(EJavaConverters, EInfo,
        "ComponentName is %s",
        buf2.c_str());

    std::wstring buf3((wchar_t*) aNode.Version().Ptr(), aNode.Version().Length());
    LOG1WSTR(EJavaConverters, EInfo,
        "Version is %s",
        buf3.c_str());

    std::wstring buf4((wchar_t*) aNode.Vendor().Ptr(), aNode.Vendor().Length());
    LOG1WSTR(EJavaConverters, EInfo,
        "Vendor is %s",
        buf4.c_str());

    LOG1(EJavaConverters, EInfo,
        "ScomoState is %d",
        aNode.ScomoState());

    LOG1(EJavaConverters, EInfo,
        "Install status of component is %d",
        aNode.InstallStatus());

    LOG1(EJavaConverters, EInfo,
        "Component id is %d",
        aNode.ComponentId());

    std::wstring buf5((wchar_t*) aNode.GlobalComponentId().Ptr(), aNode.GlobalComponentId().Length());
    LOG1WSTR(EJavaConverters, EInfo,
        "GlobalComponentId is %s",
        buf5.c_str());

    LOG1(EJavaConverters, EInfo,
        "Authenticity is %d",
        aNode.Authenticity());

    LOG1(EJavaConverters, EInfo,
        "MaxInstalledSize is %d",
        aNode.MaxInstalledSize());
}

static void logApplicationInfo(const CComponentInfo::CApplicationInfo& aApplication)
{
    LOG1(EJavaConverters, EInfo, "Application uid is 0x%x", aApplication.AppUid());

    std::wstring buf((wchar_t*) aApplication.Name().Ptr(), aApplication.Name().Length());
    LOG1WSTR(EJavaConverters, EInfo, "Application name is %s", buf.c_str());
}

static void sifGetComponentInfoL()
{
    LOG(EJavaConverters, EInfo,
        "testsifapi: sifGetComponentInfoL: Called");

    RSoftwareInstall installer;
    TInt err = installer.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testsifapi: sifGetComponentInfoL: Cannot connect to RSoftwareInstall, err %d", err);
        User::Leave(err);
    }
    CleanupClosePushL(installer);

    TRequestStatus status;
    CComponentInfo *info = CComponentInfo::NewL();
    CleanupStack::PushL(info);

    installer.GetComponentInfo(KTestMIDlet, *info, status);

    User::WaitForRequest(status);

    LOG1(EJavaConverters, EInfo,
        "testsifapi: sifGetComponentInfoL: The return status of get "
        "component info operation was %d", status.Int());

    LOG(EJavaConverters, EInfo,
        "testsifapi: sifGetComponentInfoL: Logging root node (suite)");

    logComponentInfoNode(info->RootNodeL());

    LOG(EJavaConverters, EInfo,
        "testsifapi: sifGetComponentInfoL: Logging child nodes (MIDlets)");

    TInt nMIDlets = info->RootNodeL().Children().Count();
    TInt nInd;
    for (nInd = 0; nInd < nMIDlets; nInd++)
    {
        logComponentInfoNode(*(info->RootNodeL().Children()[nInd]));
    }

    nMIDlets = info->RootNodeL().Applications().Count();
    for (nInd = 0; nInd < nMIDlets; nInd++)
    {
        logApplicationInfo(*(info->RootNodeL().Applications()[nInd]));
    }

    CleanupStack::PopAndDestroy(info);


    User::After(1000000);

    // Get component info also from jar
    TRequestStatus status2;
    CComponentInfo *info2 = CComponentInfo::NewL();
    CleanupStack::PushL(info2);

    installer.GetComponentInfo(KTestMIDlet2, *info2, status2);

    User::WaitForRequest(status2);

    LOG1(EJavaConverters, EInfo,
        "testsifapi: sifGetComponentInfoL: The return status of get "
        "component info operation 2 was %d", status.Int());

    LOG(EJavaConverters, EInfo,
        "testsifapi: sifGetComponentInfoL: Logging root node (suite2)");

    logComponentInfoNode(info2->RootNodeL());

    LOG(EJavaConverters, EInfo,
        "testsifapi: sifGetComponentInfoL: Logging child nodes (MIDlets2)");

    TInt nMIDlets2 = info2->RootNodeL().Children().Count();
    TInt nInd2;
    for (nInd2 = 0; nInd2 < nMIDlets2; nInd2++)
    {
        logComponentInfoNode(*(info2->RootNodeL().Children()[nInd2]));
    }

    nMIDlets2 = info2->RootNodeL().Applications().Count();
    for (nInd2 = 0; nInd2 < nMIDlets2; nInd2++)
    {
        logApplicationInfo(*(info2->RootNodeL().Applications()[nInd2]));
    }

    CleanupStack::PopAndDestroy(info2);


    // free resources before returning
    CleanupStack::PopAndDestroy(1);
}


static void sifActivationTestL(TInt &aComponentId)
{
    LOG1(EJavaConverters, EInfo,
        "testsifapi: sifActivationTestL: Deactivate component (id %d)", aComponentId);

    RSoftwareInstall installer;
    TInt err = installer.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testsifapi: sifActivationTestL: Cannot connect to RSoftwareInstall, err %d", err);
        User::Leave(err);
    }
    CleanupClosePushL(installer);

    TRequestStatus status;

    installer.Deactivate(aComponentId, status);

    User::WaitForRequest(status);

    LOG1(EJavaConverters, EInfo,
        "testsifapi: sifActivationTestL: The return status of deactivation operation was %d", status.Int());

    User::After(1000000);


    LOG1(EJavaConverters, EInfo,
        "testsifapi: sifActivationTestL: Activate component (id %d)", aComponentId);

    TRequestStatus status2;

    installer.Activate(aComponentId, status2);

    User::WaitForRequest(status2);

    LOG1(EJavaConverters, EInfo,
        "testsifapi: sifActivationTestL: The return status of activation operation was %d", status2.Int());

    // free resources before returning
    CleanupStack::PopAndDestroy(1);
}


int cancelFunction(void *installer)
{
    LOG(EJavaConverters, EInfo,
        "testsifapi: cancelFunction: Called");

    // TEMP TEST
    // This thread does not have active scheduler,
    // create and install it
    CActiveScheduler* as = new CActiveScheduler();
    if (NULL == as)
    {
        ELOG(EJavaConverters,
            "testsifapi: cancelFunction: Cannot create active scheduler");
            return 1;
    }
    CActiveScheduler::Install(as);

    // Wait for 6 seconds so that the operation to be cancelled
    // has had time to really do something already
    User::After(6000000);

    LOG(EJavaConverters, EInfo,
        "testsifapi: cancelFunction: Waiting for 6 seconds done");

    ((RSoftwareInstall *)installer)->CancelOperation();

    LOG(EJavaConverters, EInfo, "testsifapi: cancelFunction: Starting CActiveScheduler");
    CActiveScheduler::Start();

    LOG(EJavaConverters, EInfo,
        "testsifapi: cancelFunction: CancelOperation() was called");

    delete as;
    return 0;
}

static void cancelFromOtherThread(RSoftwareInstall &aInstaller)
{
    RThread cancelThread;

    TInt err = cancelThread.Create(_L("CancelThread"), cancelFunction, 0x4000, 0, &aInstaller);
    if (err == KErrNone)
    {
        cancelThread.Resume();
    }
    else
    {
        ELOG1(EJavaConverters,
              "testsifapi: cancelFromOtherThread: Cannot create cancel thread, err %d", err);
    }
    cancelThread.Close();
}

static void cancelFromSameThread(RSoftwareInstall &aInstaller)
{
    LOG(EJavaConverters, EInfo, "testsifapi: cancelFromSameThread: Called");

    // Wait for 13 seconds so that the operation to be cancelled
    // has had time to really do something already
    User::After(13000000);

    LOG(EJavaConverters, EInfo,
        "testsifapi: cancelFromSameThread: Waited 13 seconds");

    aInstaller.CancelOperation();

    LOG(EJavaConverters, EInfo,
        "testsifapi: cancelFromSameThread: CancelOperation() returned");
}


static void cancelInstallL()
{
    LOG(EJavaConverters, EInfo,
        "testsifapi: cancelInstall: Called");

    RSoftwareInstall installer;
    TInt err = installer.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testsifapi: sifInstallL: Cannot connect to RSoftwareInstall, err %d", err);
        User::Leave(err);
    }
    CleanupClosePushL(installer);

    TRequestStatus status;

    installer.Install(KTestMIDlet, status);

    LOG(EJavaConverters, EInfo,
        "testsifapi: cancelInstall: Install started");

//    cancelFromOtherThread(installer);

    cancelFromSameThread(installer);

    LOG(EJavaConverters, EInfo,
        "testsifapi: cancelInstall: cancelFrom<X>Thread returned");

    User::WaitForRequest(status);

    LOG1(EJavaConverters, EInfo,
        "testsifapi: cancelInstall: The return status of install operation was %d", status.Int());

    // free resources before returning
    CleanupStack::PopAndDestroy(1);

}

static void cancelNoOperationL()
{
    LOG(EJavaConverters, EInfo,
        "testsifapi: cancelNoOperationL: Called");

    RSoftwareInstall installer;
    TInt err = installer.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
            "testsifapi: cancelNoOperationL: Cannot connect to RSoftwareInstall, err %d", err);
        User::Leave(err);
    }
    CleanupClosePushL(installer);

    cancelFromSameThread(installer);

    LOG(EJavaConverters, EInfo,
        "testsifapi: cancelNoOperationL: cancelFrom<X>Thread returned");

    // free resources before returning
    CleanupStack::PopAndDestroy(1);

}

/**
 * Create cleanup stack and run the cleaner code inside TRAP harness
 * to log unexpected leaves.
 */
TInt E32Main()
{
//    __UHEAP_MARK;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();

    TInt err = KErrNone;


    LOG(EJavaConverters, EInfo, "testsifapi: starting cancelNoOperationL");
    TRAP(err, cancelNoOperationL());
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters, "testsifapi: cancelNoOperationL leaved with err %d", err);
    }

    // Wait for a moment
    User::After(500000);


    LOG(EJavaConverters, EInfo, "testsifapi: starting cancelInstallL");
    TRAP(err, cancelInstallL());
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters, "testsifapi: cancelInstallL leaved with err %d", err);
    }

    // Wait for a moment
    User::After(500000);


    LOG(EJavaConverters, EInfo, "testsifapi: starting sifSimplestInstallL");
    TRAP(err, sifSimplestInstallL());
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters, "testsifapi: sifSimplestInstallL leaved with err %d", err);
    }

    // Wait for a moment
    User::After(500000);



    // This test case must be executed sepatately, while manually started
    // Java Installer is running
    LOG(EJavaConverters, EInfo, "testsifapi: starting sifInstallerAlreadyRunningL");
    TRAP(err, sifInstallerAlreadyRunningL());
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters, "testsifapi: sifInstallerAlreadyRunningL leaved with err %d", err);
    }

    // Wait for a moment
    User::After(500000);


    TInt componentId = 0;
    TInt componentId2 = 0;


    LOG(EJavaConverters, EInfo, "testsifapi: starting sifByHandleAndArgsAndResultsInstallL");
    TRAP(err, componentId = sifByHandleAndArgsAndResultsInstallL());
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
            "testsifapi: sifByHandleAndArgsAndResultsInstallL leaved with err %d", err);
    }
    // Wait for a moment
    User::After(500000);


    // This installation will fail because MIDlet is untrusted and installing
    // untrusted is denied AND because charset is illegal
    LOG(EJavaConverters, EInfo, "testsifapi: starting secondSifByFileAndArgsAndResultsInstallL");
    TRAP(err, componentId2 = secondSifByFileAndArgsAndResultsInstallL());
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
            "testsifapi: secondSifByFileAndArgsAndResultsInstallL leaved with err %d", err);
    }
    // Wait for a moment
    User::After(500000);


    // if this fails, it leaks memory
    LOG(EJavaConverters, EInfo, "testsifapi: starting sifGetComponentInfoL");
    TRAP(err, sifGetComponentInfoL());
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters, "testsifapi: sifGetComponentInfoL leaved with err %d", err);
    }

    // Wait for a moment
    User::After(500000);

    LOG(EJavaConverters, EInfo, "testsifapi: starting sifActivationTestL");
    TRAP(err, sifActivationTestL(componentId));
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
            "testsifapi: sifActivationTestL leaved with err %d", err);
    }

    // Wait for a moment
    User::After(500000);


    // TODO: if this fails, it leaks memory
    if (0 != componentId)
    {
        LOG(EJavaConverters, EInfo, "testsifapi: starting sifUninstallL");
        TRAP(err, sifUninstallL(componentId));
        if (KErrNone != err)
        {
            ELOG1(EJavaConverters,
                "testsifapi: sifUninstallL leaved with err %d", err);
        }

        // Wait for a moment
        User::After(500000);
    }


    delete cleanupStack;
//    __UHEAP_MARKEND;
//    __UHEAP_MARKENDC(1);
    return KErrNone;
}
