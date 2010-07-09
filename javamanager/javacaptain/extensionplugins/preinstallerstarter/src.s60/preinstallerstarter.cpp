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
* Description:  PreinstallerStarter is Java Captain S60 plugin that starts
*               preinstaller process when the device boots or removable drive
*               is added to the device.
*
*/


#include <e32base.h>
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <apgcli.h> // for RApaLsSession
#endif
#include <hal_data.h>
#include <hal.h>
#include <sysutil.h>

#include "javaprocessconstants.h"
#include "javasymbianoslayer.h"
#include "javauids.h"
#include "logger.h"
#include "coreinterface.h"
#include "booteventprovidermessages.h"
#include "mmceventprovidermessages.h"
#include "preinstallerstartermessages.h"

#include "preinstallerstarter.h"

/**
 * Return pointer to ExtensionPluginInterface implementation for this
 * extension dll
 */
java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
    return new java::captain::PreinstallerStarter();
}

namespace java
{
namespace captain
{

using java::fileutils::driveInfo;
using java::fileutils::DriveListenerInterface;

/**
 * Initialize member varaibles in constructor
 */
PreinstallerStarter::PreinstallerStarter() :
    CActive(EPriorityStandard), mCore(0), mPreinstaller(0)
{
}

/**
 * Destructor
 */
PreinstallerStarter::~PreinstallerStarter()
{
    DoCancel();
}

/**
 * Implement PluginInterface method
 */
void PreinstallerStarter::startPlugin(CoreInterface* core)
{
    mCore = core;
    CActiveScheduler::Add(this);
}

/**
 * Implement PluginInterface method
 */
void PreinstallerStarter::stopPlugin()
{
    mCore = 0;
}

/**
 * Implement ExtensionPluginInterface method
 */
EventConsumerInterface* PreinstallerStarter::getEventConsumer()
{
    return this;
}

/**
 * Handle Java Captain events sent by Boot event provider or
 * MMC event provider. If IAD boot (Java Captain has been started
 * after OMJ sis package has been installed), start preinstaller process
 * with 'iad' paramater, otherwise start preinstaller process
 * without the parameter.
 *
 * Implement EventConsumerInterface method
 */
void PreinstallerStarter::event(const std::string& eventProvider,
                                java::comms::CommsMessage& aMsg)
{
    if (eventProvider == BOOT_EVENT_PROVIDER)
    {
        int bootType = NORMAL_BOOT_C;
        getBootMessageParams(aMsg, bootType);
        LOG1(
            EJavaCaptain,
            EInfo,
            "PreinstallerStarter::event() boot event received (type=%d)",
            bootType);
        switch (bootType)
        {
        case IAD_BOOT_C:
        {
            // This event is received after OMJ has been (IAD) installed
            // to device.

            // Preinstaller will be started with 'iad' command line option
            // so that it knows that it has been started for the first
            // time after java 2.0 IAD installation.
            startPreinstaller(ETrue);
        }
        break;

        case FIRST_DEVICE_BOOT_C:
        case NORMAL_BOOT_C:
        {
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
            registerMidletApplicationTypeHandler();
#endif

            // Start preinstaller normally (without 'iad' option).
            startPreinstaller(EFalse);
        }
        break;

        default:
        {
            WLOG1(EJavaCaptain,
                  "DriveListenerInterface: event() unknown boot event (type=%d)", bootType);
        }
        break;
        }
    }
    else if (eventProvider == MMC_EVENT_PROVIDER)
    {
        int operation = 0;
        driveInfo di;
        getMmcChangedMessageParams(aMsg, operation, di);
        LOG1(
            EJavaCaptain,
            EInfo,
            "PreinstallerStarter::event() mmc event received (operation=%d)",
            operation);

        switch (operation)
        {
            // Preinstaller started reacts only to this one event.
            // All other MMC events are ignored.
        case DriveListenerInterface::REMOVABLE_MEDIA_INSERTED_C:
        {
            startPreinstaller(EFalse);
        }
        break;
        }
    }
}

/**
 * Start preinstaller process if it is not yet running.
 * The name of the executable is javapreinstaller.exe
 * When it is started, it checks all preinstall directories in the device
 * and preinstalls those java applications in the directories that have not
 * yet been installed. After that preinstaller exits.
 *
 * @param aIadBoot when true, starts preinstaller with 'iad' parameter
 */
void PreinstallerStarter::startPreinstaller(TBool aIadBoot)
{
    // Check that the device has enough free memory (800kB) to start preinstaller process
    // and (if needed) also java installer
    TInt freeMemory = 0;
    TInt err = HAL::Get(HALData::EMemoryRAMFree, freeMemory);
    if (KErrNone == err)
    {
        if (freeMemory < 819200)
        {
            if (freeMemory > 40960)
            {
                // The device has enough memory to log the error
                ELOG(EJavaCaptain,
                     "PreinstallerStarter: Not enough free mem to start preinstaller");
            }
            return;
        }
    }
    else
    {
        WLOG1(EJavaCaptain, "PreinstallerStarter: Cannot check free mem, err %d", err);
    }

    // Check that the device has enough also free disk space in C: (800kB) to start
    // preinstaller process
    if (SysUtil::FFSSpaceBelowCriticalLevelL(NULL, 819200))
    {
        if (!SysUtil::FFSSpaceBelowCriticalLevelL(NULL, 40960))
        {
            // The device has enough disk space to log the error
            ELOG(EJavaCaptain,
                 "PreinstallerStarter: Not enough space in phone mem to start preinstaller");
        }
        return;
    }

    // Start preinstaller process
    _LIT(KPreinstallerMatch, "javapreinstaller*");
    TFindProcess finder(KPreinstallerMatch);
    TFullName name;

    // Check all processes found
    while (KErrNone == finder.Next(name))
    {
        // Check the state of the already existing preinstaller process
        RProcess oldPreinstaller;
        TInt ret = oldPreinstaller.Open(name);
        if (KErrNone != ret)
        {
            WLOG1(EJavaCaptain,
                  "PreinstallerStarter: Cannot open existing preinstaller process, error %d", ret);
            oldPreinstaller.Close();
            return;
        }
        else
        {
            TExitType exitType = oldPreinstaller.ExitType();
            if (EExitPending == exitType)
            {
                // Strange, preinstaller is already/still running, do nothing
                WLOG(EJavaCaptain, "PreinstallerStarter: preinstaller is already running.");
                oldPreinstaller.Close();
                return;
            }
            else
            {
                // The old preinstaller process has actually already exited,
                // but it is still in the process list due to an Open C 1.5.1 functionalities.
                LOG1(EJavaCaptain,
                     EInfo,
                     "PreinstallerStarter: old preinstaller process had exited, exit type is %d",
                     exitType);
                oldPreinstaller.Close();
            }
        }
    }
    // No preinstaller processes were found or they were all zombies.

    // Start new preinstaller process.
    {
        // Build command line used to pass all necessary info
        TBuf<128> commandLine;  // Actual len of cmd line will be max 20 characters
        int len = strlen(java::runtime::JAVA_PREINSTALLER_STARTER_DLL);
        TPtr8 ptr8((TUint8 *)java::runtime::JAVA_PREINSTALLER_STARTER_DLL, len, len);
        commandLine.Copy(ptr8);

        if (aIadBoot)
        {
            commandLine.Append(_L(" iad"));
        }

        // start preinstaller
        mPreinstaller = new RProcess();
        TBuf<64> preinstallerProcess;  // Actual len of the process name is 9
        len = strlen(java::runtime::JAVA_PROCESS);
        TPtr8 ptr8Process((TUint8 *)java::runtime::JAVA_PROCESS, len, len);
        preinstallerProcess.Copy(ptr8Process);
        TInt err = mPreinstaller->Create(preinstallerProcess, commandLine);
        if (KErrNone == err)
        {
            // Process has been created.

            // This active object will be activated when preinstaller process exits
            mPreinstaller->Logon(iStatus);

            // Allow the process to run
            mPreinstaller->Resume();
            if (aIadBoot)
            {
                LOG(
                    EJavaCaptain,
                    EInfo,
                    "PreinstallerStarter: started preinstaller with iad parameter");
            }
            else
            {
                LOG(EJavaCaptain, EInfo, "PreinstallerStarter: started preinstaller");
            }

            SetActive();
        }
        else
        {
            delete mPreinstaller;
            mPreinstaller = 0;

            ELOG1(
                EJavaCaptain,
                "PreinstallerStarter: starting preinstaller failed, err %d",
                err);
        }
    }
}

/**
 * Register 'javalauncher.exe' as the midlet application type handler
 * in AppArc. If necessary unregister old handler first.
 */
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void PreinstallerStarter::registerMidletApplicationTypeHandler()
{
    _LIT(KMidpAppArcPlugin, "javalauncher.exe");

    RApaLsSession appLstSession;
    CleanupClosePushL(appLstSession);
    TInt err = appLstSession.Connect();
    if (KErrNone == err)
    {
        TRAP(err, appLstSession.RegisterNonNativeApplicationTypeL(
                 TUid::Uid(KMidletApplicationTypeUid), KMidpAppArcPlugin()));
        if (KErrNone == err)
        {
            // All is well
        }
        else if (KErrAlreadyExists == err)
        {
#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
            // S60 Java has registered MIDlet application type handler StubMIDP2RecogExe.exe,
            // must change that register
            WLOG(EJavaCaptain,
                 "PreinstallerStarter: MIDlet application type is already "
                 "registered (by Java 1.x).");
            TRAP(err, appLstSession.DeregisterNonNativeApplicationTypeL(
                     TUid::Uid(KMidletApplicationTypeUid)));
            TRAP(err, appLstSession.RegisterNonNativeApplicationTypeL(
                     TUid::Uid(KMidletApplicationTypeUid), KMidpAppArcPlugin()));
            if (KErrNone == err)
            {
                WLOG(EJavaCaptain,
                     "PreinstallerStarter: Registered MIDlet application "
                     "type again (for Java 2.x).");
            }
            else
            {
                ELOG1(EJavaCaptain,
                      "PreinstallerStarter: Reregistering MIDlet application "
                      "type failed with err %d",
                      err);
            }
#endif

            // In 9.2 and later environments this situation is OK.
            // We have already made the registration successfully.
        }
        else
        {
            ELOG1(EJavaCaptain,
                  "PreinstallerStarter: Registering MIDlet application type failed with err %d",
                  err);
        }
        CleanupStack::PopAndDestroy(&appLstSession);
    }
    else
    {
        ELOG1(EJavaCaptain,
              "PreinstallerStarter: registerMidletApplicationTypeHandler: "
              "Connecting to AppArc failed with error %d",
              err);
    }
}
#endif

void PreinstallerStarter::RunL()
{
    LOG(EJavaCaptain, EInfo, "PreinstallerStarter::RunL started");

    // Send event to AutoStarter plugin, done only once
    CommsMessage eventMsg;
    setPreinstallerExitedMessageParams(eventMsg, iStatus.Int());
    mCore->getEventDispatcher()->event(PREINSTALLER_EVENT_PROVIDER, eventMsg);

    // process handle is no longer needed
    mPreinstaller->Close();
    delete mPreinstaller;
    mPreinstaller = 0;
}


void PreinstallerStarter::DoCancel()
{
    if (mPreinstaller)
    {
        mPreinstaller->LogonCancel(iStatus);
        // Because exit notification request has been canceled,
        // there is no reason to keep the process handle open.
        mPreinstaller->Close();
        delete mPreinstaller;
        mPreinstaller = 0;
    }
}


} // namespace captain
} // namespace java
