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
* Description:  OMJ S60 environment post updater process.
*               This process is executed when OMJ is installed, after
*               the native installer exits.
*               Register new Java 2.0 midlet starter to AppArc.
*               Execute javaappconverter.exe and javausersettingsconfigurator.exe.
*               Destroy one PS key owned by the native installer.
*               Create new PS keys used by new Java Installer.
*               Refresh midlet icons shown in menu application.
*
*/


#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <w32std.h>
#include <centralrepository.h>
#include <swinstapi.h>
#include <apgcli.h> // for RApaLsSession

#include <javadomainpskeys.h>

#include "javaprocessconstants.h"
#include "javauids.h"
#include "logger.h"


_LIT8(KAppJavaType, "application/java");


/**
 * Ask javainstaller to create the new PS keys
 * that other processes can later use to get information of
 * the progress of the installation of Java applications.
 */
static void CreateNewJavaInstallerPSKeys()
{
    LOG(EJavaConverters, EInfo, "javapostupdater: CreateNewJavaInstallerPSKeys() was called");

    RProcess rJavaInstaller;
    TFileName fileName;
    // Pass just 'register' command
    TBuf<128> commandLine;

    // Build command line used to pass all necessary info to Java Installer
    TInt len = strlen(java::runtime::JAVA_INSTALLER_STARTER_DLL);
    TPtr8 ptr8InstallerDll((TUint8 *)java::runtime::JAVA_INSTALLER_STARTER_DLL, len, len);
    commandLine.Copy(ptr8InstallerDll);

    commandLine.Append(_L(" register"));

    LOG(EJavaConverters, EInfo,
        "javapostupdater: CreateNewJavaInstallerPSKeys() starting Java "
        "Installer for registering PS keys");

    // start JavaInstaller
    TBuf<64> installerProcess;  // Actual len of the process name is 9
    len = strlen(java::runtime::JAVA_PROCESS);
    TPtr8 ptr8Process((TUint8 *)java::runtime::JAVA_PROCESS, len, len);
    installerProcess.Copy(ptr8Process);

    TInt err = rJavaInstaller.Create(installerProcess, commandLine);
    if (KErrNone == err)
    {
        LOG(EJavaConverters, EInfo,
            "javapostupdater: CreateNewJavaInstallerPSKeys() calling Resume");
        rJavaInstaller.Resume();
    }

    LOG(EJavaConverters, EInfo,
        "javapostupdater: CreateNewJavaInstallerPSKeys() calling RProcess::Close");
    // free resources before returning
    rJavaInstaller.Close();
}


/**
 * Make a dummy call to Installer using SwiUI::RSWInstSilentLauncher
 * so that OMJ appinstuiplugin gets called from AppInstUi process
 * and can remove one conflicting P&S key created by old S60 Java Installer
 */
static void DeleteOldJavaInstallerPSKeyL()
{
    SwiUI::RSWInstSilentLauncher   installer;
    SwiUI::TUninstallOptions       options;
    SwiUI::TUninstallOptionsPckg   pckg;

    // use default options
    pckg = options;

    LOG(EJavaConverters, EInfo, "javapostupdater: DeleteOldJavaInstallerPSKey() called");

    TInt err = installer.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "javapostupdater: DeleteOldJavaInstallerPSKey: RSWInstSilentLauncher "
              "connect error %d", err);
        return;
    }

    CleanupClosePushL(installer);

    // Calling AppInstUi silent install API immediately after OMJ has been
    // installed may fail because the AppInstUI has not yet exited or new OMJ
    // ECOM plugin is not yet in use.
    // Make the calls in a loop and wait a couple of seconds between
    const TUid illegalAppUid = {KRandomIllegalAppUid};
    TInt nRetries = 7;
    do
    {
        // This call returns KErrCancel even if the PS key was destroyed
        err = installer.SilentUninstall(illegalAppUid, pckg, KAppJavaType);
        if ((KErrNone != err) && (KErrCancel != err))
        {
            WLOG1(EJavaConverters,
                  "javapostupdater: DeleteOldJavaInstallerPSKey: SilentUninstall error %d", err);
        }

        // Wait 3 sec so that AppInstUi has time to finish previous operation
        // and exit so that new ECom OMJ specific plugin will be loaded
        User::After(3000000);
        nRetries--;
    }
    while ((KErrCancel != err) && (KErrNone != err) && (nRetries > 0));

    if (nRetries == 0)
    {
        ELOG1(EJavaConverters,
              "javapostupdater: DeleteOldJavaInstallerPSKey: Could not delete PS key, error %d",
              err);
    }
    else
    {
        LOG(EJavaConverters,
            EInfo, "javapostupdater: DeleteOldJavaInstallerPSKey() PS Key was deleted");
    }


    CleanupStack::PopAndDestroy(&installer);
}


/**
 * Start javaappconverter.exe process and wait until it exits. After that
 * start javausersettingsconfigurator.exe process and wait until it exits.
 */
static void runConverters()
{
    JELOG2(EJavaPreinstaller);

    // Start converter process
    RProcess converter;
    TInt err = converter.Create(_L("javaappconverter.exe"), KNullDesC);
    if (KErrNone == err)
    {
        // Process has been created
        LOG(EJavaConverters, EInfo,
            "runConverter: created javaappconverter process");

        // Get ready to wait until the process ends
        TRequestStatus status;
        converter.Logon(status);

        // Make the main thread of the process runnable
        converter.Resume();
        LOG(EJavaConverters, EInfo,
            "runConverter: started javaappconverter");

        // Wait until the process ends.
        User::WaitForRequest(status);
        converter.Close();

        if (status.Int() == KErrAlreadyExists)
        {
            // Old S60 midlets have already been converted.
            // This is an OMJ upgrade installation.
            LOG(EJavaConverters, EInfo,
                "runConverter: no need to execute javausersettingsconfigurator process");
            return;
        }
        else
        {
            LOG1(EJavaConverters, EInfo,
                 "runConverter: javaappconverter ended with status %d", status.Int());
        }
    }
    else
    {
        ELOG1(
            EJavaConverters,
            "runConverter: starting javaappconverter failed, err %d",
            err);
        return;
    }

    // Configure the user settings of the newly installed applications
    // (based on how they were configured in the legacy java)
    RProcess userSettingsTool;
    err = userSettingsTool.Create(_L("javausersettingsconfigurator.exe"), KNullDesC);
    if (err == KErrNone)
    {
        // Process has been created
        LOG(EJavaConverters, EInfo,
            "runConverter: created javausersettingsconfigurator process");

        // Get ready to wait until the process ends
        TRequestStatus status;
        userSettingsTool.Logon(status);

        // Make the main thread of the process runnable
        userSettingsTool.Resume();
        LOG(EJavaConverters, EInfo,
            "runConverter: started javausersettingsconfigurator");

        // Wait until the process ends.
        User::WaitForRequest(status);

        LOG1(EJavaConverters, EInfo,
             "runConverter: javausersettingsconfigurator ended with status %d", status.Int());

        userSettingsTool.Close();
    }
    else
    {
        ELOG1(
            EJavaConverters,
            "startConverter: starting javausersettingsconfigurator failed, err %d",
            err);
    }
}

/**
 * Register 'javalauncher.exe' as the midlet application type handler
 * in AppArc. If necessary unregister old handler first.
 */
static void registerMidletApplicationTypeHandler()
{
    _LIT(KMidpAppArcPlugin, "javalauncher.exe");

    RApaLsSession appLstSession;
    TInt err = appLstSession.Connect();
    if (KErrNone == err)
    {
        TRAP(err, appLstSession.RegisterNonNativeApplicationTypeL(
                 TUid::Uid(KMidletApplicationTypeUid), KMidpAppArcPlugin()));
        if (KErrNone == err)
        {
            // All is well
            LOG(EJavaConverters, EInfo,
                "registerMidletApplicationTypeHandler: Registered MIDlet application "
                "type for Java 2.x");
        }
        else if (KErrAlreadyExists == err)
        {
            // Java 1.x has registered MIDlet application type handler StubMIDP2RecogExe.exe,
            // must change that registration
            LOG(EJavaConverters, EInfo,
                "registerMidletApplicationTypeHandler: MIDlet application type was already "
                "registered by Java 1.x");
            TRAP_IGNORE(appLstSession.DeregisterNonNativeApplicationTypeL(
                            TUid::Uid(KMidletApplicationTypeUid)));
            TRAP(err, appLstSession.RegisterNonNativeApplicationTypeL(
                     TUid::Uid(KMidletApplicationTypeUid), KMidpAppArcPlugin()));
            if (KErrNone == err)
            {
                LOG(EJavaConverters, EInfo,
                    "registerMidletApplicationTypeHandler: Registered MIDlet application "
                    "type again (for Java 2.x).");
            }
            else
            {
                ELOG1(EJavaConverters,
                      "registerMidletApplicationTypeHandler: Reregistering MIDlet application "
                      "type failed with err %d",
                      err);
            }
        }
        else
        {
            ELOG1(EJavaConverters,
                  "registerMidletApplicationTypeHandler: Registering MIDlet "
                  "application type failed with err %d",
                  err);
        }

        // Launch rescan of all applications so that all java applications
        // will be shown in Menu
        TRAP(err, appLstSession.PrepareNonNativeApplicationsUpdatesL());
        if (KErrNone != err)
        {
            ELOG1(EJavaConverters,
                  "registerMidletApplicationTypeHandler: PrepareNonNativeApplicationsUpdatesL error %d",
                  err);
        }
        else
        {
            LOG(EJavaConverters, EInfo,
                "registerMidletApplicationTypeHandler: PrepareNonNativeApplicationsUpdatesL OK");
        }

        TRAP(err, appLstSession.CommitNonNativeApplicationsUpdatesL());
        if (KErrNone != err)
        {
            ELOG1(EJavaConverters,
                  "registerMidletApplicationTypeHandler: CommitNonNativeApplicationsUpdatesL error %d",
                  err);
        }
        else
        {
            LOG(EJavaConverters, EInfo,
                "registerMidletApplicationTypeHandler: CommitNonNativeApplicationsUpdatesL OK");
        }

        appLstSession.Close();
    }
    else
    {
        ELOG1(EJavaConverters,
              "registerMidletApplicationTypeHandler: Connecting to AppArc failed with error %d",
              err);
    }
}

/**
 * Menu application has a central repository key that can be used in
 * devices belonging to Nokia 5800 XpressMusic device family to request
 * that the Menu application would rescan all applications it contains.
 */
static void UseCRKeyToUpdateAppShellDataL()
{
    const TUid KCRUidMenu = { 0x101F8847 };
    const TUint32 KMenuUpdateData = 0x00000001;

    // Update Central Rep
    CRepository *pCenRep = CRepository::NewL(KCRUidMenu);

    TInt value = 1;
    TInt err = pCenRep->Set(KMenuUpdateData, value);
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "javapostupdater: UseCRKeyToUpdateAppShellDataL: "
              "setting cenrep value failed, error %d ",
              err);
    }

    delete pCenRep;
}

/**
 * If OMJ is upgraded the Application Shell (menu application) of the
 * Nokia 5800 XpressMusic device family does not
 * display the icons of the java applications that have been installed to
 * only OMJ environment. The icons are shown after the device is booted.
 * Tell menu application to scan again all applications so that the icons
 * will be shown.
 */
static void AddAllJavaAppIconsToMenuApp()
{
    LOG(EJavaConverters, EInfo, "javapostupdater: AddAllJavaAppIconsToMenuApp called.");

    TRAPD(err, UseCRKeyToUpdateAppShellDataL());
    if (KErrNotFound == err)
    {
        // Only Nokia XPressMusic 5800 device family supports this feature,
        // N97 and later devices no longer support it but they have new Menu
        // application that shows all Java application icons anyway.
        LOG(EJavaConverters, EInfo,
            "javapostupdater: AddAllJavaAppIconsToMenuApp: This device does not "
            "support refreshing Menu using CR keys.");
    }
    else if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "javapostupdater: AddAllJavaAppIconsToMenuApp: Error: %d", err);
    }
}

/**
 * Create cleanup stack and run the post updater code inside TRAP harness
 * to log unexpected leaves.
 */
TInt E32Main()
{
    __UHEAP_MARK;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();

    LOG(EJavaConverters, EInfo, "javapostupdater process starts");

    // Java 2.0 midlet starter handles midlet application type
    registerMidletApplicationTypeHandler();

    // Convert existing midlets to Java 2.0 midlets
    runConverters();

    // Delete one old PS key owned by native installer (appinstui)
    TRAPD(error, DeleteOldJavaInstallerPSKeyL());
    if (KErrNone != error)
    {
        ELOG1(EJavaConverters,
              "javapostupdater: DeleteOldJavaInstallerPSKeyL() leaved with %d", error);
    }

    // Create new PS keys that Java 2.0 Java Installer will use
    CreateNewJavaInstallerPSKeys();

    // Make sure all Java application icons are shown in menu.
    AddAllJavaAppIconsToMenuApp();

    delete cleanupStack;
    __UHEAP_MARKEND;
    return error;
}
