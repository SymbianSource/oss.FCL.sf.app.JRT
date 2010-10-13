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
//_LIT(KTestMIDlet, "C:\\data\\installs\\DS_Snow.jad");

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

    RFile installFile;
    err = installFile.Open(fs, KTestMIDlet, EFileShareReadersOnly | EFileRead);
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "testsifapi: sifByHandleAndArgsAndResultsInstallL: Cannot open "
              "E:\\stopwatch10midp2.jad, err %d", err);
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

    LOG(EJavaConverters, EInfo,
        "testsifapi: sifByHandleAndArgsAndResultsInstallL: arguments created");

    installer.Install(installFile, *arguments, *results, status);

    User::WaitForRequest(status);

    LOG1(EJavaConverters, EInfo,
        "testsifapi: sifByHandleAndArgsAndResultsInstallL: The return status of install operation was %d", status.Int());


    TInt componentId = 0;
    TBool idExisted = results->GetIntByNameL(KSifOutParam_ComponentId, componentId);
    if ( idExisted )
    {
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

    // free resources before returning
    CleanupStack::PopAndDestroy(2);
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

    // Wait for 6 seconds so that the operation to be cancelled
    // has had time to really do something already
    User::After(6000000);

    LOG(EJavaConverters, EInfo,
        "testsifapi: cancelFunction: Waiting for 6 seconds done");

    ((RSoftwareInstall *)installer)->CancelOperation();

    LOG(EJavaConverters, EInfo,
        "testsifapi: cancelFunction: CancelOperation() was called");

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


/**
 * Create cleanup stack and run the cleaner code inside TRAP harness
 * to log unexpected leaves.
 */
TInt E32Main()
{
/*
    ELOG2(EJavaConverters,
        "testsifapi: Starting testing long long long long long long long long long long "
        "long long long long long long long long long long long long long long long long "
        "long long long long long long long long long long long long long long long long "
        "long long long long long long long long long long long long long buffer %d %d", 0, 1);

    LOG(EJavaConverters, EInfo,
        "testsifapi: sifByHandleAndArgsAndResultsInstallL: Called");
*/

    __UHEAP_MARK;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();

    TInt err = KErrNone;


    /*
        TRAP(err, cancelInstallL());
        if (KErrNone != err)
        {
            ELOG1(EJavaConverters, "testsifapi: cancelInstallL leaved with err %d", err);
        }

        // Wait for a moment
        User::After(500000);
    */


    TRAP(err, sifSimplestInstallL());
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters, "testsifapi: sifSimplestInstallL leaved with err %d", err);
    }

    // Wait for a moment
    User::After(500000);


    TInt componentId = 0;

    TRAP(err, componentId = sifByHandleAndArgsAndResultsInstallL());
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
            "testsifapi: sifByHandleAndArgsAndResultsInstallL leaved with err %d", err);
    }
    // Wait for a moment
    User::After(500000);

    // TODO: if this fails, it leaks memory
    TRAP(err, sifGetComponentInfoL());
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters, "testsifapi: sifGetComponentInfoL leaved with err %d", err);
    }

    // Wait for a moment
    User::After(500000);

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
    __UHEAP_MARKENDC(1);
    return KErrNone;
}
