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
* Description: Java platform 2.0 javaappbackconverter process
*
*/


#include <e32std.h>
#include <e32base.h>
#include <e32property.h>
#include <f32file.h>
#include <centralrepository.h>
#include <swinstallerinternalpskeys.h>
#include <AknGlobalMsgQuery.h>

#include "javacommonutils.h"
#include "javauids.h"
#include "backconverter.h"
#include "noarmlogs.h"

_LIT_SECURE_ID(KAppInstUiSecureID, 0x101F875A);
_LIT_SECURE_ID(KAppSisLauncherSecureID, 0x1020473F);


/**
 * Create file server connection.
 * If no data storage file exists, exit.
 * Create active scheduler and start it to execute actual back converter.
 */
static void ExecuteBackConverterL(RFs &aFs)
{
    LOG(EJavaConverters, EInfo, "javaappbackconverter: ExecuteBackConverterL called");

    CActiveScheduler* as = new(ELeave) CActiveScheduler();

    // Install active scheduler
    CActiveScheduler::Install(as);
    CleanupStack::PushL(as);

    // The file server session must be shared so that that AppArc can
    // use the icon file handle opened in back converter
    TInt err = aFs.ShareProtected();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "javaappbackconverter: ExecuteBackConverterL: "
              "Cannot share file server session, err code is %d ", err);
        User::Leave(err);
    }

    // Setup and start the back conversion
    CBackConverter* back = CBackConverter::NewLC(aFs);

    LOG(EJavaConverters,
        EInfo, "javaappbackconverter: ExecuteBackConverterL: Call CBackConverter::Start()");

    back->Start();

    // Start active scheduler
    LOG(EJavaConverters, EInfo, "ExecuteBackConverterL: Starting CActiveScheduler");
    CActiveScheduler::Start();

    LOG(EJavaConverters, EInfo, "javaappbackconverter: ExecuteBackConverterL: Cleaning up");

    CleanupStack::PopAndDestroy(back);
    CleanupStack::PopAndDestroy(as);
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
              "javaappbackconverter: UseCRKeyToUpdateAppShellDataL: "
              "setting cenrep value failed, error %d ",
              err);
    }

    delete pCenRep;
}

/**
 * After OMJ has been uninstalled the menu application of the device
 * does not display the icons of the old, restored java applications.
 * The icons are shown after the device is booted.
 * Tell menu application to scan again all applications so that the icons
 * will be shown.
 */
static void AddOldJavaAppIconsToMenuApp()
{
    LOG(EJavaConverters, EInfo, "javaappbackconverter: AddOldJavaAppIconsToMenuApp called.");

    TRAPD(err, UseCRKeyToUpdateAppShellDataL());
    if (KErrNotFound == err)
    {
        // Only Nokia XPressMusic 5800 device family supports this feature,
        // N97 and later devices no longer support it but they have new Menu
        // application that shows all Java application icons anyway.
        LOG(EJavaConverters, EInfo,
            "javaappbackconverter: AddOldJavaAppIconsToMenuApp: This device does not "
            "support refreshing Menu using CR keys.");
    }
    else if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "javaappbackconverter: AddOldJavaAppIconsToMenuApp: Error: %d", err);
    }
}

/**
 * Kill the running OMJ javaregistry.exe. After killing next call to
 * Registry API restarts the old S60 server process.
 */
static void KillRegistry()
{
    LOG(EJavaConverters, EInfo, "javaappbackconverter: KillRegistry called.");

    TFullName processName;
    _LIT(KJavaRegistryProcess, "javaregistry*");
    TFindProcess finder(KJavaRegistryProcess);

    if (finder.Next(processName) != KErrNotFound)
    {
        RProcess processToKill;
        TInt err = processToKill.Open(finder);

        if (KErrNone != err)
        {
            WLOG1(EJavaConverters, "javaappbackconverter: Process open: %d", err);
        }
        else
        {
            processToKill.Kill(KErrNone);
            processToKill.Close();
        }
    }
    else
    {
        WLOG(EJavaConverters, "javaappbackconverter: Registry process not found");
    }
}

/**
 * Unregister OMJ MIDlet starter, reregister old S60 MIDlet starter
 */
static void ReregisterOldMidletStarter()
{
    LOG(EJavaConverters, EInfo, "javaappbackconverter: ReregisterOldMidletStarter called");

    _LIT(KStubMIDP2RecogExe, "StubMIDP2RecogExe.exe");

    RApaLsSession appLstSession;

    TInt error = appLstSession.Connect();
    if (error != KErrNone)
    {
        ELOG1(EJavaConverters, "javaappbackconverter: ReregisterOldMidletStarter: "
              "RApaLsSession.Connect failed with error %d", error);
        return;
    }
    TRAP(error, appLstSession.RegisterNonNativeApplicationTypeL(
             TUid::Uid(KMidletApplicationTypeUid), KStubMIDP2RecogExe()));
    if (error == KErrNone)
    {
        LOG(EJavaConverters, EInfo, "javaappbackconverter: ReregisterOldMidletStarter: "
            "Registered MIDlet application type for S60 Java.");
    }
    else if (error != KErrAlreadyExists)
    {
        ELOG1(EJavaConverters, "javaappbackconverter: ReregisterOldMidletStarter: "
              "RegisterNonNativeApplicationTypeL failed with error %d", error);
    }
    else
    {
        // OMJ Java has registered MIDlet application type so that it is handled by
        // OMJ binary 'javalauncher.exe', must change that registration
        WLOG(EJavaConverters, "javaappbackconverter: ReregisterOldMidletStarter: "
             "MIDlet application type was registered (by OMJ Java).");
        // Ignore possible error in deregistration
        TRAP(error, appLstSession.DeregisterNonNativeApplicationTypeL(
                 TUid::Uid(KMidletApplicationTypeUid)));
        TRAP(error, appLstSession.RegisterNonNativeApplicationTypeL(
                 TUid::Uid(KMidletApplicationTypeUid), KStubMIDP2RecogExe()));
        if (KErrNone != error)
        {
            ELOG1(EJavaConverters, "javaappbackconverter: ReregisterOldMidletStarter: "
                  "RegisterNonNativeApplicationTypeL failed with error %d", error);
        }
        else
        {
            LOG(EJavaConverters, EInfo, "javaappbackconverter: ReregisterOldMidletStarter: "
                "Registered MIDlet application type again (for S60 Java).");
        }
    }

    appLstSession.Close();
}


/**
 * Show dialog telling that the device must be rebooted
 */
static void showMustRebootDialog()
{
    LOG(EJavaConverters, EInfo, "javaappbackconverter: showMustRebootDialog() Called");

    // Wait for 2 seconds before showing dialog
    User::After(2000000);

    TRAPD(err,
          TRequestStatus status;
          CAknGlobalMsgQuery* globalMsg = CAknGlobalMsgQuery::NewLC();

          // In OMJ 2.x beta .sis uninstallation it is enough to
          // show the text always in english (non-localized)
          _LIT(KJavaPleaseReboot, "To complete the uninstallation process, \
 the phone must be restarted. Until then some applications may not function properly.");

          // tell that device will be rebooted
          globalMsg->ShowMsgQueryL(
              status,
              KJavaPleaseReboot,
              R_AVKON_SOFTKEYS_OK_EMPTY__OK,
              KNullDesC(),
              KNullDesC(),
              0,
              -1,
              CAknQueryDialog::EWarningTone);

          User::WaitForRequest(status);

          CleanupStack::PopAndDestroy(globalMsg);
         );
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "javaappbackconverter: showMustRebootDialog() TRAP_IGNORE exited with err %d", err);
    }
}


/**
 * Create cleanup stack and run the javaappbackconverter code inside TRAP
 * harness to log unexpected leaves.
 */
TInt E32Main()
{
    CTrapCleanup* pCleanupStack = CTrapCleanup::New();
    if (NULL == pCleanupStack)
    {
        ELOG(EJavaConverters, "javaappbackconverter: Could not create cleanup stack!");
        return KErrNoMemory;
    }

    LOG(EJavaConverters, EInfo, "javaappbackconverter: Starting backconverter");

    // Create file server session
    RFs fs;
    TInt err = fs.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "javaappbackconverter: Cannot open RFs connection. Error %d", err);
        return err;
    }

    // Determine whether backconversion should be done or not.
    // It should be done
    // - if this process has been started when java_2_0.sis is uninstalled
    // OR
    // - if this process has been started when s60_omj_restore_midlets.sis is installed

    // The path name of the loaded executable on which this process is based is
    // 'javaappbackconverter.exe' if this process has been started from java_2_0.sis and
    // 'javaapprestorer.exe' if this process has been started from s60_omj_restore_midlets.sis
    _LIT(KJavaRestorerFileName, "javaapprestorer.exe");
    TBool fStartedFromOmjSis(ETrue);
    RProcess thisProcess;
    TFileName filename = thisProcess.FileName();
    if (filename.Find(KJavaRestorerFileName) > 0)
    {
        LOG(EJavaConverters, EInfo,
            "javaappbackconverter: Started from s60_omj_restore_midlets.sis");
        fStartedFromOmjSis = EFalse;
    }
    LOG1WSTR(EJavaConverters, EInfo,
             "javaappbackconverter: Process file name is %s", (wchar_t *)(filename.PtrZ()));

    // Check whether uninstalling or (installing/upgrading)
    TBool fUninstallingSisPackage(EFalse);
    TInt value(0);
    // Ignore errors, if the value cannot be read, assume 'installing .sis'
    RProperty::Get(KPSUidSWInstallerUiNotification, KSWInstallerUninstallation, value);
    if (0 == value)
    {
        // Native installer is installing (or upgrading) a .sis package
        LOG(EJavaConverters, EInfo, "javaappbackconverter: native installer is installing .sis");
    }
    else
    {
        // Native installer is uninstalling a .sis package
        fUninstallingSisPackage = ETrue;
        LOG(EJavaConverters, EInfo, "javaappbackconverter: native installer is uninstalling .sis");
    }

    if (fUninstallingSisPackage && fStartedFromOmjSis)
    {
        // backconversion must be done
        LOG(EJavaConverters, EInfo, "javaappbackconverter: Going to do backconversion.");
    }
    else if ((!fUninstallingSisPackage) && (!fStartedFromOmjSis))
    {
        // backconversion must be done
        LOG(EJavaConverters, EInfo, "javaappbackconverter: Going to restore midlets.");
    }
    else
    {
        LOG(EJavaConverters, EInfo,
            "javaappbackconverter: Back conversion / restoration is not needed.");
        fs.Close();
        return KErrNone;
    }

    // The only time that this application should be executed
    // is when native installer starts it
    if ((User::CreatorSecureId() != KAppInstUiSecureID) &&
            (User::CreatorSecureId() != KAppSisLauncherSecureID))
    {
        ELOG(EJavaConverters,
             "javaappbackconverter: Mismatch in secure ID, only AppInstUi "
             "or SisLauncher can launch this exe.");
        ELOG1(EJavaConverters,
              "javaappbackconverter: Starter has SID %x ", User::CreatorSecureId().iId);
        fs.Close();
        return KErrPermissionDenied;
    }
    else
    {
        LOG(EJavaConverters, EInfo, "javaappbackconverter: Starter SID was OK.");
    }

    ReregisterOldMidletStarter();

    TInt ret = KErrNone;
    TRAP(ret, ExecuteBackConverterL(fs));
    if (ret != KErrNone)
    {
        ELOG1(EJavaConverters, "javaappbackconverter: ExecuteBackConverterL: Main error %d", ret);
    }

    // Kill OMJ Java Registry process. Next call to Java Registry API starts
    // old S60 Java Registry server
    KillRegistry();

    // Start also old SystemAMS.exe to restore old S60 Java environment to functional state
    RProcess rSystemAms;
    _LIT(KSystemAmsProcess, "SystemAMS.exe");
    _LIT(KEmptyCommandLine, "");
    err = rSystemAms.Create(KSystemAmsProcess, KEmptyCommandLine);
    if (KErrNone == err)
    {
        rSystemAms.Resume();
    }
    else
    {
        WLOG1(EJavaConverters,
              "javaappbackconverter: Starting SystemAMS.exe failed, err %d", err);
    }
    rSystemAms.Close();

    AddOldJavaAppIconsToMenuApp();

    fs.Close();

    if (fUninstallingSisPackage)
    {
        showMustRebootDialog();
    }

    delete pCleanupStack;
    return ret;
}
