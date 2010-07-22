/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  OMJ Symbian preinstaller process
*
*/


#include <e32base.h>
#include <pathinfo.h>
#include <driveinfo.h>
#include <javastorageentry.h>
#include <javastorage.h>
#include <javastoragenames.h>
#include <utf.h>

#include "silentmidletinstall.h"
#include "javacommonutils.h"
#include "javauid.h"
#include "javauids.h"
#include "logger.h"
#include "javaprocessconstants.h"
#include "javasymbianoslayer.h"

using namespace java::storage;
using namespace java::util;


// MIDlet name and vendor max size is 255,
// +1 added for terminating zero needed for logging
const TInt KMaxBufferSize = 256;

// character constants needed when parsing java attributes
const TUint32 HT = 9;  // horizontal tab
const TUint32 LF = 10; // line feed
const TUint32 CR = 13; // carriage return
const TUint32 SP = 32; // space
const TUint32 COLON = 58; // ':'

const int INSTALLED = 0;
const int NO_PREINSTALL = 2;

_LIT(KMidletName, "MIDlet-Name");
_LIT(KMidletVendor, "MIDlet-Vendor");
_LIT(KMidletVersion, "MIDlet-Version");
_LIT(KMidletUid, "Nokia-MIDlet-UID-1");

_LIT(KJadExtension, "*.jad");

// Java Installer Integrity Service journal file name
_LIT(KISJournalFile, "\\private\\102033E6\\installer\\is\\isjournal.dat");

// The directory where the java applications to be preinstalled are searched
// from
_LIT(KPreinstallDir, ":\\resource\\java\\preinstall\\");


/**
 * To create new instance of this class.
 *
 * @param aFs - A reference to the file server.
 * @return Reference to the object of this class.
 * @exception If construction fails.
 */
CSilentMIDletInstall* CSilentMIDletInstall::NewLC(RFs& aFs)
{
    CSilentMIDletInstall* self = new(ELeave) CSilentMIDletInstall(aFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

/**
 * To do 1st phase construction for this object.
 * Adds this active object to the scheduler.
 *
 * @param aFs - A reference to the file server.
 */
CSilentMIDletInstall::CSilentMIDletInstall(RFs& aFs) :
        CActive(EPriorityStandard), iFs(aFs)
{
    CActiveScheduler::Add(this);
}

/**
 * To do 2nd phase construction for this object.
 *
 * @exception If the method is not able to allocate necessary buffers.
 */
void CSilentMIDletInstall::ConstructL()
{
    iMIDletName = HBufC::NewL(KMaxBufferSize);
    iMIDletVendor = HBufC::NewL(KMaxBufferSize);
    iMIDletUid = HBufC::NewL(KMaxBufferSize);
    iPreinstallServer = new(ELeave) PreinstallCommsServer();

    iNumberOfAppsToInstall = 0;
    iISRollbackNeeded = EFalse;
}

/**
 * Deletes this object.
 * All allocated resources are released.
 */
CSilentMIDletInstall::~CSilentMIDletInstall()
{
    iJadFiles.ResetAndDestroy();
    iJadFiles.Close();

    iDirs.ResetAndDestroy();
    iDirs.Close();

    delete iMIDletName;
    iMIDletName = NULL;

    delete iMIDletVendor;
    iMIDletVendor = NULL;

    delete iMIDletUid;
    iMIDletUid = NULL;

    delete iPreinstallServer;
    iPreinstallServer = NULL;
}

/**
 * To start silent preinstallations.
 */
void CSilentMIDletInstall::Start()
{
    iState = EFindOutDeviceDrives;

    // Check if an explicit roll-back of a previous installation is needed
    // in the case there is nothing to pre-install (if there is something
    // to pre-install, the potential roll-back is done automatically)
    TUint attrs;
    TInt err = iFs.Att(KISJournalFile, attrs);
    LOG1(EJavaPreinstaller, EInfo,
         "Checking IS journal file \\private\\102033E6\\installer\\is\\isjournal.dat, status %d", err);
    if ((KErrNotFound == err) || (KErrPathNotFound == err))
    {
        iISRollbackNeeded = EFalse;
    }
    else
    {
        iISRollbackNeeded = ETrue;
        // If JavaInstaller is running, then existence of the Java Installer
        // integrity service directory is ok and rollback is not needed
        TFullName processName;
        _LIT(KJavaInstallerProcess, "*Installer*");
        TFindProcess finder(KJavaInstallerProcess);
        err = finder.Next(processName);
        if (err == KErrNone)
        {
            iISRollbackNeeded = EFalse;
            WLOG(EJavaPreinstaller,
                 "Java Installer is running while checking need to rollback");
        }
    }

    CompleteRequest();
}

/**
 * To stop whole preinstaller.
 * Stops the active scheduler.
 */
void CSilentMIDletInstall::Exit()
{
    CActiveScheduler::Stop();
}

/**
 * Completes the current request for this object and
 * sets this active object to active state.
 *
 * @Postconditions The following conditions are true immediately after
 * returning from this method.
 * - iStatus == KErrNone
 * - IsActive() == ETrue
 */
void CSilentMIDletInstall::CompleteRequest()
{
    TRequestStatus *status = &iStatus;
    User::RequestComplete(status, KErrNone);
    if (!IsActive())
    {
        SetActive();
    }
}

/**
 * To run this active object.
 *
 * This object goes through the following states in the order the states
 * are listed below:
 *
 * EFindOutDeviceDrives: find out all non-remote drives in the device
 *    - mark externally mountable drives (removable drives)
 *    - mark read only drives (ROM)
 *
 * EFindOutDrivesToBeScannedNow: make list of drives to be scanned
 *    - start with all mounted drives
 *
 * EAppsInPreinstallDirectories: go through all drives in the list and
 *  find all java applications in the preinstall directory
 *    - only the .jad file is checked but remember that downloading .jar is not
 *      allowed when preinstalling so the .jar file referred to in the .jad
 *      file must be in the device
 *
 * ECheckWhichAppsShouldBeInstalled: for each java application check from
 *  Java Storage whether it should be installed
 *    - if same or newer version already installed, do not install
 *    - if the user has uninstalled same or newer version, do not install
 *    - unsigned java applications can be installed only in debug builds
 *
 * EExecutePreinstallServer:
 *  - store list of .jad files to preinstall server object,
 *  - start Java Installer in poll mode,
 *  - start preinstall server in another thread and wait until Java Installer exits
 *      - preinstall server will give the names of the .jad files to
 *        Java Installer when it polls them,
 *      - preinstall server will log whether each installation  succeeded.
 *      - After all .jad files have been polled, preinstall server will ask
 *        Java Installer to exit.
 *  - when Java Installer exits, this active object moves to next state.
 *
 * EExit:
 *  - stop preinstall server thread
 *  - notify possibly waiting processes that preinstallation has been done
 *  - exit
 *
 */
void CSilentMIDletInstall::RunL()
{
    switch (iState)
    {
    case EFindOutDeviceDrives:
    {
        LOG(EJavaPreinstaller, EInfo,
            "CSilentMIDletInstall:RunL EFindOutDeviceDrives");
        GetAllDeviceDrivesL();
        iState = EFindOutDirectoriesToBeScannedNow;
        CompleteRequest();
    }
    break;

    case EFindOutDirectoriesToBeScannedNow:
    {
        LOG(EJavaPreinstaller, EInfo,
            "CSilentMIDletInstall:RunL EFindOutDirectoriesToBeScannedNow");

        GetDirsToBeScannedL();

        iState = EAppsInPreinstallDirectories;
        CompleteRequest();
    }
    break;

    case EAppsInPreinstallDirectories:
    {
        LOG(EJavaPreinstaller, EInfo,
            "CSilentMIDletInstall:RunL EAppsInPreinstallDirectories");

        GetMIDletFilesL(iDirs);

        iState = ECheckWhichAppsShouldBeInstalled;
        CompleteRequest();
    }
    break;

    case ECheckWhichAppsShouldBeInstalled:
    {
        LOG(EJavaPreinstaller, EInfo,
            "CSilentMIDletInstall:RunL ECheckWhichAppsShouldBeInstalled");

        CheckWhichAppsShouldBeInstalledL();

        iState = EExecutePreinstallServer;
        CompleteRequest();
    }
    break;

    case EExecutePreinstallServer:
    {
        LOG(EJavaPreinstaller, EInfo,
            "CSilentMIDletInstall:RunL EExecutePreinstallServer");

        // This method advances the state machine to EExit state
        ExecutePreinstallServerL();
    }
    break;

    case EExit:
    {
        LOG(EJavaPreinstaller, EInfo,
            "CSilentMIDletInstall:RunL EExit");

        // Stop the server if it is running
        iPreinstallServer->stop();

        // Java Captain starts preinstaller each time a mountable drive
        // is added to the device. There is no need to wait for mount
        // events in this process.

        // Stop the whole preinstaller process.
        Exit();
    }
    break;

    } // switch
}

/**
 * To handle leave from RunL.
 * This method re-activates this active object.
 * After calling this method this active object is in stable state and
 * ready to continue its execution
 *
 * @param aError - A reason of error.
 * @return KErrNone.
 */
TInt CSilentMIDletInstall::RunError(TInt aError)
{
    ELOG2(EJavaPreinstaller,
          "CSilentMIDletInstall::RunError(%d) from state %d", aError, iState);

    Cancel();

    // Make preinstaller to exit nicely.
    iState = EExit;
    CompleteRequest();

    return KErrNone;
}

/**
 * Cancel this object. Stops preinstall server if needed.
 */
void CSilentMIDletInstall::DoCancel()
{
    ELOG1(EJavaPreinstaller,
          "CSilentMIDletInstall::DoCancel from state %d", iState);

    // Check whether preinstall server must be stopped
    if (iState == EExecutePreinstallServer)
    {
        // Stops the server if it is running
        iPreinstallServer->stop();
    }
}

/**
 * Add .jad files in the directories specified in aDirs to iJadFiles.
 *
 * Goes  all directories in iDirs one by one and adds the
 * valid .jad files in alphabetical order to iJadFiles.
 *
 * @param aDirs - An array of directories to be scanned.
 * @exception Unable to alloc memory for the internal buffers.
 * @exception Unable to get directory's contents.
 */
void CSilentMIDletInstall::GetMIDletFilesL(RPointerArray<HBufC>& aDirs)
{
    JELOG2(EJavaPreinstaller);

    const TInt num = aDirs.Count();

    // Read JAD files.
    for (TInt i = 0; i < num; i++)
    {
        const TPtrC& dir = ((HBufC *)(aDirs[i]))->Des();

        HBufC* mask= NULL;
        TPtr maskPtr(NULL,0,0);

        // Create JAD mask, e.g., "z:\resource\java\preinstall\*.jad".
        // Reserve space also for terminating zero for logging the info
        mask = HBufC::NewLC(dir.Length() + KJadExtension().Length() + 1);
        maskPtr.Set(mask->Des());
        maskPtr.Append(dir);
        maskPtr.Append(KJadExtension);
        LOG1WSTR(EJavaPreinstaller, EInfo,
                 "CSilentMIDletInstall::GetMIDletFilesL Looking for jad files from directory %s",
                 (wchar_t *)(maskPtr.PtrZ()));
        GetDirEntriesL(dir, *mask, iJadFiles);
        CleanupStack::PopAndDestroy(mask);
    }

    if (iJadFiles.Count() == 0)
    {
        ILOG(EJavaPreinstaller,
             "CSilentMIDletInstall:GetMIDletFilesL No MIDlets to preinstall");
    }
}

/**
 * Get the entries of the directory in alphabetical order.
 *
 * @param aDirectory - A directory to be scanned.
 * @param aMask - A filter to be used for scanning.
 * @param aVector - An output vector for contents.
 * @exception Unable to alloc memory for the internal buffers.
 */
void CSilentMIDletInstall::GetDirEntriesL(const TDesC& aDirectory,
        const TDesC& aMask, RPointerArray<HBufC>& aVector)
{
    JELOG2(EJavaPreinstaller);

    // Get entries in alphabetical order.

    CDir* entries= NULL;
    TInt err = iFs.GetDir(aMask, KEntryAttMaskSupported, ESortByName, entries);

    if (err != KErrNone)
    {
        if (err != KErrPathNotFound)
        {
            ELOG1(EJavaPreinstaller,
                  "CSilentMIDletInstall::GetDirEntriesL Dir error (%d)", err);
        }
        delete entries;
        return;
    }

    CleanupStack::PushL(entries);

    // Add full file names to the vector.

    TInt num = entries->Count();
    for (TInt i = 0; i < num; i++)
    {
        const TDesC& name = (*entries)[i].iName;
        // Reserve one char for null terminator
        HBufC* path = HBufC::NewLC(aDirectory.Length() + name.Length() + 1);
        TPtr pathPtr(path->Des());
        pathPtr.Append(aDirectory);
        pathPtr.Append(name);
        LOG1WSTR(EJavaPreinstaller, EInfo,
                 "CSilentMIDletInstall::GetDirEntriesL Adding file %s",
                 (wchar_t *)(pathPtr.PtrZ()));
        aVector.Append(path);
        CleanupStack::Pop(path);
    }

    CleanupStack::PopAndDestroy(entries);
}

/**
 * Start Java Installer in poll mode and then wait until it exits.
 */
void CSilentMIDletInstall::RunJavaInstallerL()
{
    JELOG2(EJavaPreinstaller);

    LOG(EJavaPreinstaller, EInfo, "CSilentMIDletInstall::RunJavaInstaller");

    RProcess rJavaInstaller;
    TFileName fileName;
    // Max one path name, user name and password and some options ->
    // 1536 is enough
    TBuf<1536> commandLine;

    // Build command line used to pass all necessary info to Java Installer
    TInt len = strlen(java::runtime::JAVA_INSTALLER_STARTER_DLL);
    TPtr8 ptr8InstallerDll((TUint8 *)java::runtime::JAVA_INSTALLER_STARTER_DLL, len, len);
    commandLine.Copy(ptr8InstallerDll);

    commandLine.Append(_L(" poll -address=preinstall"));

    // Upgrading MIDlets is allowed
    commandLine.Append(_L(" -upgrade=yes"));

    // No OCSP checks for preinstalled MIDlets
    commandLine.Append(_L(" -ocsp=no"));

#ifdef _DEBUG
    // Allow installing unsigned apps in debug builds
    commandLine.Append(_L(" -untrusted=yes"));
#else
    // Deny preinstalling untrusted midlets in release builds
    commandLine.Append(_L(" -untrusted=no"));
#endif

    // Do upgrade if version number has not increased
    commandLine.Append(_L(" -overwrite=no"));

    // Both the JAD and JAR file must be present when preinstalling MIDlets,
    // downloading is not allowed.
    commandLine.Append(_L(" -download=no"));

    // If upgrade install, retain the data (RMS and private files) of the previous version
    commandLine.Append(_L(" -upgrade_data=yes"));

    // start JavaInstaller
    TBuf<64> installerProcess;  // Actual len of the process name is 9
    len = strlen(java::runtime::JAVA_PROCESS);
    TPtr8 ptr8Process((TUint8 *)java::runtime::JAVA_PROCESS, len, len);
    installerProcess.Copy(ptr8Process);

    TInt err = rJavaInstaller.Create(installerProcess, commandLine);
    if (KErrNone == err)
    {
        LOG(EJavaPreinstaller, EInfo, "CSilentMIDletInstall::RunJavaInstaller calling Logon");
        // This call will wait until Java Installer exits (or panics)
        rJavaInstaller.Logon(iStatus);

        LOG(EJavaPreinstaller, EInfo, "CSilentMIDletInstall::RunJavaInstaller calling Resume");
        rJavaInstaller.Resume();
    }
    else
    {
        ELOG1(EJavaPreinstaller,
              "CSilentMIDletInstall::RunJavaInstaller Cannot start Installer, error %d", err);
        // CActive will trap the following leave, execution will go to RunError
        User::Leave(err);
    }

    LOG(EJavaPreinstaller, EInfo, "CSilentMIDletInstall::RunJavaInstaller calling RProcess::Close");
    // free resources before returning
    rJavaInstaller.Close();

    // now wait until Java Installer exits
    SetActive();
}

/**
 * Start Java Installer just to do IntegrityService rollback.
 * Do not wait for the process exit.
 */
void CSilentMIDletInstall::RollbackJavaInstaller()
{
    JELOG2(EJavaPreinstaller);

    RProcess rJavaInstaller;
    TFileName fileName;
    // Pass just 'rollback' command
    TBuf<128> commandLine;

    // Build command line used to pass all necessary info to Java Installer
    TInt len = strlen(java::runtime::JAVA_INSTALLER_STARTER_DLL);
    TPtr8 ptr8InstallerDll((TUint8 *)java::runtime::JAVA_INSTALLER_STARTER_DLL, len, len);
    commandLine.Copy(ptr8InstallerDll);

    commandLine.Append(_L(" rollback"));

    WLOG(EJavaPreinstaller,
         "CSilentMIDletInstall::RollbackJavaInstaller starting Java Installer for rollback");

    // start JavaInstaller
    TBuf<64> installerProcess;  // Actual len of the process name is 9
    len = strlen(java::runtime::JAVA_PROCESS);
    TPtr8 ptr8Process((TUint8 *)java::runtime::JAVA_PROCESS, len, len);
    installerProcess.Copy(ptr8Process);

    TInt err = rJavaInstaller.Create(installerProcess, commandLine);
    if (KErrNone == err)
    {
        LOG(EJavaPreinstaller, EInfo,
            "CSilentMIDletInstall::RollbackJavaInstaller calling Resume");
        rJavaInstaller.Resume();
    }

    LOG(EJavaPreinstaller, EInfo,
        "CSilentMIDletInstall::RollbackJavaInstaller calling RProcess::Close");
    // free resources before returning
    rJavaInstaller.Close();
}

/**
 * Parse MIDlet-Name, MIDlet-Vendor and MIDlet-Version parameters from JAD file.
 * Parameters are used to determine whether to pre-install MIDlet or not.
 * Parse also optional Nokia-MIDlet-UID-1 in case it is needed.
 *
 * @param ETrue if parsing succeeds otherwise EFalse.
 */
TBool CSilentMIDletInstall::ParseJadL(const TDesC& aJadFileName)
{
    HBufC *jadContent = NULL;
    // Trap leave thrown if reading jad content fails
    TRAPD(err, jadContent = GetJadContentL(aJadFileName));
    if (KErrNone != err)
    {
        ELOG1(EJavaPreinstaller,
              "CSilentMIDletInstall::ParseJadL Reading Jad content failed, error %d",
              err);
        return EFalse;
    }
    CleanupStack::PushL(jadContent);

    HBufC *midletName = ParseAttribute(jadContent, KMidletName);
    if (NULL == midletName)
    {
        ELOG(EJavaPreinstaller,
             "CSilentMIDletInstall::ParseJadL Parsing midlet name failed.");
        CleanupStack::PopAndDestroy(jadContent);
        return EFalse;
    }
    // store midlet name to member variable and log it
    TPtr namePtr(iMIDletName->Des());
    namePtr.Copy(*midletName);
    LOG1WSTR(EJavaPreinstaller, EInfo,
             "CSilentMIDletInstall::ParseJadL MIDlet-Name %s",
             (wchar_t *)(namePtr.PtrZ()));
    delete midletName;

    HBufC *midletVendor = ParseAttribute(jadContent, KMidletVendor);
    if (NULL == midletVendor)
    {
        ELOG(EJavaPreinstaller,
             "CSilentMIDletInstall::ParseJadL Parsing midlet vendor failed.");
        CleanupStack::PopAndDestroy(jadContent);
        return EFalse;
    }
    // store midlet vendor to member variable and log it
    TPtr vendorPtr(iMIDletVendor->Des());
    vendorPtr.Copy(*midletVendor);
    LOG1WSTR(EJavaPreinstaller, EInfo,
             "CSilentMIDletInstall::ParseJadL MIDlet-Vendor %s",
             (wchar_t *)(vendorPtr.PtrZ()));
    delete midletVendor;

    HBufC *midletVersion = ParseAttribute(jadContent, KMidletVersion);
    if (NULL == midletVersion)
    {
        ELOG(EJavaPreinstaller,
             "CSilentMIDletInstall::ParseJadL Parsing midlet version failed.");
        CleanupStack::PopAndDestroy(jadContent);
        return EFalse;
    }
    // Convert version to TAppVersion, store it and log it
    iMIDletVersion = DesToAppVersion(midletVersion);
    delete midletVersion;

    LOG3(EJavaPreinstaller, EInfo,
         "CSilentMIDletInstall::ParseJadL MIDlet-Version is %d.%d.%d",
         iMIDletVersion.iMajor, iMIDletVersion.iMinor, iMIDletVersion.iBuild);

    HBufC *midletUid = ParseAttribute(jadContent, KMidletUid);
    if (NULL == midletUid)
    {
        // Ok, all MIDlets do not predefine Symbian Uid
        iMIDletUid->Des().Zero();
    }
    else
    {
        // store midlet Uid to member variable and log it
        TPtr uidPtr(iMIDletUid->Des());
        uidPtr.Copy(*midletUid);
        LOG1WSTR(EJavaPreinstaller, EInfo,
            "CSilentMIDletInstall::ParseJadL Nokia-MIDlet-UID-1 %s",
            (wchar_t *)(uidPtr.PtrZ()));
        delete midletUid;
    }

    CleanupStack::PopAndDestroy(jadContent);
    return ETrue;
}

/**
 * If there is something to preinstall start preinstall comms server and
 * installer in poll mode.
 * Otherwise start installer with rollback option if installer
 * integrity service rollback is needed.
 */
void CSilentMIDletInstall::ExecutePreinstallServerL()
{
    if (iNumberOfAppsToInstall > 0)
    {
        // Install all MIDlet Suites still in iJadFiles.

        // Pass iJadFiles to PreinstallServer
        iPreinstallServer->setJadFiles(iJadFiles);

        // Start the comms server
        int err = iPreinstallServer->start();
        if (0 != err)
        {
            // server cannot be started
            ELOG1(EJavaPreinstaller,
                  "Cannot start preinstall server, err %d", err);
        }
        else
        {
            // Starts Java Installer and waits until it exits
            RunJavaInstallerL();
        }

        iState = EExit;
    }
    else
    {
        // Rollback must be done by launching Java Installer
        // separately with rollback option.
        // (Normally rollback is done during normal installation.)
        if (iISRollbackNeeded)
        {
            RollbackJavaInstaller();
            iISRollbackNeeded = EFalse;
        }

        iState = EExit;
        CompleteRequest();
    }
}

/**
 * Check all Jad files in iJadFiles and remove those
 * that need not be preinstalled (already installed[1] or preinstalled[2]
 * or preinstalled and then uninstalled by user[3]).
 */
void CSilentMIDletInstall::CheckWhichAppsShouldBeInstalledL()
{
    TBool skipInstall(ETrue);
    iNumberOfAppsToInstall = 0;

    // throws STL C++ exception if fails, catched by CActive and
    // execution goes to RunError
    std::auto_ptr<JavaStorage> js(JavaStorage::createInstance());
    // throws STL C++ exception if fails, catched by CActive and
    // execution goes to RunError
    js->open(JAVA_DATABASE_NAME);

    // In Java Storage there is 'preinstall' table that contains
    // the name, vendor and version of every java application that is currently
    // installed to the device or that has been preinstalled to the device
    // but removed by the user.
    // The table contains also the installation state of each application.
    // If the application has been removed the user, the application is in state
    // NO_PREINSTALL.
    // Do not preinstall application if it is found from this table with state
    // NO_PREINSTALL.
    // Do not preinstall application if it is found from this table
    // and the version number of the application is the same or less
    // than the version number in the table.
    // If the application has been removed by javaupgradeupp or it has
    // not yet been installed, the application is not in the table at all.
    // Do not preinstall the application if there is a native application
    // with the same Uid as the predefined Uid of the first Java
    // application in the application suite.

    TBool            apaSessionConnected(EFalse);
    RApaLsSession    apaSession;
    JavaStorageEntry attribute;
    JavaStorageApplicationEntry_t findPattern;
    JavaStorageApplicationList_t  foundEntries;

    for (TInt i = 0; i < iJadFiles.Count(); i++)
    {
        if (ParseJadL(*iJadFiles[i]))
        {
            skipInstall = ETrue;

            TPtr namePtr(iMIDletName->Des());
            TPtr vendorPtr(iMIDletVendor->Des());
            int  installState = INSTALLED;

            // Search by NAME and VENDOR
            attribute.setEntry(NAME, desToWstring(namePtr));
            findPattern.insert(attribute);
            attribute.setEntry(VENDOR, desToWstring(vendorPtr));
            findPattern.insert(attribute);

            // All information in table is returned (whole row)

            js->search(PREINSTALL_TABLE, findPattern, foundEntries);
            findPattern.clear();

            // Anything found?
            if (foundEntries.size() > 0)
            {
                // We like to know if the application INSTALL_STATE is NO_PREINSTALL
                attribute.setEntry(INSTALL_STATE, L"");

                // Check the INSTALL_STATE of the first (there should be only one)
                // found application
                JavaStorageApplicationEntry_t::const_iterator findIterator =
                    foundEntries.front().find(attribute);
                if (findIterator != (foundEntries.front()).end())
                {
                    installState = JavaCommonUtils::wstringToInt(findIterator->entryValue());
                }
                else
                {
                    ELOG1WSTR(EJavaPreinstaller,
                              "CheckWhichAppsShouldBeInstalledL: No INSTALL_STATE info "
                              "in Storage for application %s", desToWstring(namePtr));
                }

                if (installState == NO_PREINSTALL)
                {
                    // This application must not be preinstalled
                    LOG1WSTR(EJavaPreinstaller, EInfo,
                        "CheckWhichAppsShouldBeInstalledL: User has removed application %s "
                        "It must not be preinstalled again.", desToWstring(namePtr));
                }
                else
                {
                    // We like to know application VERSION
                    attribute.setEntry(VERSION, L"");

                    // Check the version of the first (there should be only one)
                    // found application
                    findIterator = foundEntries.front().find(attribute);
                    if (findIterator != (foundEntries.front()).end())
                    {
                        // Application has been installed at sometime
                        // but if we have a newer version of the application
                        // we will install this newer version.
                        if (iMIDletVersion > wstringToAppVersion(findIterator->entryValue()))
                        {
                            skipInstall = EFalse;
                        }
                        else
                        {
                            LOG1WSTR(EJavaPreinstaller, EInfo,
                                     "CheckWhichAppsShouldBeInstalledL: Application %s "
                                     "has already been installed", desToWstring(namePtr));
                        }
                    }
                    else
                    {
                        skipInstall = EFalse;
                        ELOG1WSTR(EJavaPreinstaller,
                                  "CheckWhichAppsShouldBeInstalledL: No version info "
                                  "in Storage for application %s", desToWstring(namePtr));
                    }
                }
            }
            else
            {
                skipInstall = EFalse;

                LOG(EJavaPreinstaller, EInfo,
                    "CheckWhichAppsShouldBeInstalledL: Application has not "
                    "been installed previously");

                // Check whether there is a native application installed
                // with the same Uid as the predefined Uid of the first
                // Java application in the suite.

                // The string value of attribute Nokia-MIDlet-UID-1 from
                // Jad file is in iMIDletUid if it was defined.
                TInt err = KErrNone;
                if (iMIDletUid->Length() > 0)
                {
                    if (!apaSessionConnected)
                    {
                        err = apaSession.Connect();
                        apaSessionConnected = ETrue;
                    }

                    // If the presence of a possible native app cannot be
                    // determined, let Java Installer try to install
                    // the Java application
                    if (KErrNone == err)
                    {
                        skipInstall = IsNativeAppPresent(apaSession);
                    }
                }
            }

            foundEntries.clear();
        }
        else
        {
            // If Jad parsing fails don't preinstall this
            skipInstall = ETrue;
            TPtr16 ptrJadName = iJadFiles[i]->Des();
            ELOG1WSTR(EJavaPreinstaller,
                "CheckWhichAppsShouldBeInstalledL: Parsing JAD %s failed",
                desToWstring(ptrJadName));
        }

        if (skipInstall)
        {
            delete iJadFiles[i];
            iJadFiles[i] = NULL;
        }
        else
        {
            iNumberOfAppsToInstall++;
        }
    }

    if (apaSessionConnected)
    {
        apaSession.Close();
    }

    js->close();
}

/**
 * Adds the preinstall directory of every local, non-substed drive to iDirs
 */
void CSilentMIDletInstall::GetDirsToBeScannedL()
{
    TChar driveChar;

    for (TInt drive = 0; drive < KMaxDrives; drive++)
    {
        // All present local drives are scanned for
        // java applications to be preinstalled
        if (iDriveStatuses[drive] & DriveInfo::EDrivePresent)
        {
            // The preinstall directory in this drive must be scanned.
            // Reserve memory also for drive letter and terminating zero
            // for logging.
            HBufC *preinstallDir = HBufC::NewLC(KPreinstallDir().Length() + 2);
            TPtr dirPtr(preinstallDir->Des());

            (void)iFs.DriveToChar(drive, driveChar);
            dirPtr.Append(driveChar);
            dirPtr.Append(KPreinstallDir());

            // Add new search directory
            iDirs.AppendL(preinstallDir);
            CleanupStack::Pop(preinstallDir);
        }
    }
}

/**
 * Checks all local drives in the device and stores the DriveInfo API drive
 * status information for each drive to iDriveStatuses
 * @exception Cannot get drive list.
 */
void CSilentMIDletInstall::GetAllDeviceDrivesL()
{
    JELOG2(EJavaPreinstaller);

    TDriveList driveList;
    // get all drives
    TInt err = iFs.DriveList(driveList);
    if (KErrNone != err)
    {
        ELOG1(EJavaPreinstaller,
              "GetAllDeviceDrives cannot get drive list, err %d", err);
        User::Leave(err);
    }

    // store status of the non-remote, non-substed drives
    TUint status = 0;
    for (TInt drive = 0; drive < KMaxDrives; drive++)
    {
        iDriveStatuses[drive] = 0;

        if (driveList[drive] == 0)
        {
            // no such drive in this device
            continue;
        }

        err = DriveInfo::GetDriveStatus(iFs, drive, status);
        if (KErrNone != err)
        {
            ELOG2(EJavaPreinstaller,
                  "GetAllDeviceDrivesL cannot get drive %d status, err %d",
                  drive, err);
            User::Leave(err);
        }
        // D drive is temporary RAM drive, skip it
        // Drives J to Y are substed or remote drives, skip them
        if ((drive == EDriveD) || ((drive >= EDriveJ) && (drive <= EDriveY)))
        {
            continue;
        }

        iDriveStatuses[drive] = status;
    }
}

/**
 * Reads the whole content of the Jad file and returns it in
 * buffer in Symbian Unicode character set.
 * @param[in] aJarFile
 * @return pointer to HBufC that contains the Jad file,
 * ownership is transferred to caller
 * @exception If jad file content cannot be read
 */
HBufC *CSilentMIDletInstall::GetJadContentL(const TDesC& aJadFileName)
{
    JELOG2(EJavaPreinstaller);

    TInt err;
    RFile jadFile;

    err = jadFile.Open(iFs, aJadFileName, EFileRead);
    User::LeaveIfError(err);
    CleanupClosePushL(jadFile);

    // Reserve buffer for Jad in UTF-8 char set
    TInt jadSize = 0;
    err = jadFile.Size(jadSize);
    User::LeaveIfError(err);
    HBufC8 *bufUtf8Jad  = HBufC8::NewL(jadSize);
    CleanupStack::PushL(bufUtf8Jad);

    // Read the content in Utf8 char set
    TPtr8 tmpPtr(bufUtf8Jad->Des());
    err = jadFile.Read(tmpPtr, jadSize);
    User::LeaveIfError(err);

    // Convert to Unicode
    HBufC *bufUnicodeJad =
        CnvUtfConverter::ConvertToUnicodeFromUtf8L(*bufUtf8Jad);

    CleanupStack::PopAndDestroy(bufUtf8Jad);
    CleanupStack::PopAndDestroy(&jadFile);

    // Return to caller
    return bufUnicodeJad;

} // GetJadContentL

/**
 * Finds the java attribute specified by
 * aAttributeName from aJad and returns the value of that attribute
 * in HBufC.
 * @param[in] aJad contents of Jad file
 * @param[in] aAttributeName the name of a java attribute
 * @return the value of the attribute. Caller gets the ownership of the
 * returned HBufC.
 * If the attribute is not found, returns NULL
 */
HBufC *CSilentMIDletInstall::ParseAttribute(const HBufC *aJad, const TDesC& aAttributeName)
{
    JELOG2(EJavaPreinstaller);

    TInt    nInd(0);
    TBool   fullNameFound(EFalse);
    TUint32 ch;

    // Start parsing from the beginning of the Jad file
    TPtrC parsePtr = aJad->Mid(nInd);

    do
    {
        // Find attribute name
        nInd = parsePtr.Find(aAttributeName);
        if (nInd < 0)
        {
            // Returns NULL if the attribute cannot be found
            return NULL;
        }

        // Check that the attribute name was preceded by line break or
        // it was at the beginning of the Jad file
        if (nInd == 0)
        {
            fullNameFound = ETrue;
        }
        else
        {
            ch = parsePtr[nInd-1];
            if ((ch == CR) || (ch == LF))
            {
                fullNameFound = ETrue;
            }
            else
            {
                // Name was just a part of longer string (not 'word match')
                fullNameFound = EFalse;
                // Skip to the last character of the found match.
                // We can skip because we are insterested only in 'word' matches
                // so the next cannot start inside the area we are skipping now.
                parsePtr.Set(parsePtr.Mid(nInd + aAttributeName.Length() - 1));
                continue;
            }
        }

        // Check whether Jad file ends after attribute name
        if (nInd + aAttributeName.Length() >= parsePtr.Length())
        {
            // Jad file ends immediately after the found
            // attribute name instance. No attribute value
            return NULL;
        }

        // Check that there is a white space character or colon after
        // attribute name
        ch = parsePtr[nInd + aAttributeName.Length()];
        if ((ch == COLON) || (ch == SP) || (ch == HT))
        {
            fullNameFound = ETrue;
        }
        else
        {
            // Name was just a part of longer string (not 'word match')
            fullNameFound = EFalse;
            // Skip to the next character after the found match
            parsePtr.Set(parsePtr.Mid(nInd + aAttributeName.Length()));
            continue;
        }
    }
    while (!fullNameFound);

    // Skip to the end of the attribute name and find ':' after the name.
    // The skipped characters must be white space chacraters, otherwise
    // the attribute name is illegal and Java Installer will not accept
    // the Jad file.
    parsePtr.Set(parsePtr.Mid(nInd + aAttributeName.Length() - 1));
    nInd = parsePtr.Locate(COLON);
    if (nInd < 0)
    {
        return NULL;
    }
    nInd++;

    // Parse attribute value (CR or LF ends)
    TInt nEndInd = parsePtr.Locate(CR);
    TInt nTmpInd = parsePtr.Locate(LF);

    if (KErrNotFound == nEndInd)
    {
        nEndInd = parsePtr.Length() - 1;
    }
    if (KErrNotFound == nTmpInd)
    {
        nTmpInd = parsePtr.Length() - 1;
    }

    if (nTmpInd < nEndInd)
    {
        nEndInd = nTmpInd;
    }

    if (nEndInd < nInd)
    {
        return NULL;
    }

    TPtrC attributeValue = parsePtr.Mid(nInd, (nEndInd - nInd));

    // Remove possible white space from the beginning and end of the value
    HBufC *bufValue = attributeValue.Alloc();
    if (NULL == bufValue)
    {
        return NULL;
    }
    TPtr value = bufValue->Des();
    value.Trim();

    return bufValue;
} // parseAttribute

/**
 * Parses the application version string given in aAppVersionString
 * and returns the corresponding Symbian TAppVersion.
 * @param[in] aAppVersionString version string to be parsed
 * @return application version object
 * If parsing cannot be done, returns application version 0.0.0
 */
TAppVersion CSilentMIDletInstall::DesToAppVersion(const HBufC *aAppVersionString)
{
    JELOG2(EJavaPreinstaller);

    TInt err;
    TAppVersion midletVersion;
    TAppVersion midletVersionZero;
    TLex versionParser(*aAppVersionString);
    err = versionParser.Val(midletVersion.iMajor);
    if (KErrNone != err)
    {
        WLOG1(EJavaPreinstaller,
              "DesToAppVersion cannot parse midlet major version, error %d", err);
        return midletVersionZero;
    }

    TChar dot = versionParser.Get();
    if (dot != '.')
    {
        WLOG(EJavaPreinstaller,
             "DesToAppVersion suspicious midlet version, no dot after major version");
        // return at least the major version
        return midletVersion;
    }
    err = versionParser.Val(midletVersion.iMinor);
    if (KErrNone != err)
    {
        WLOG1(EJavaPreinstaller,
              "DesToAppVersion cannot parse midlet minor version, error %d", err);
        // return at least the major version
        return midletVersion;
    }

    dot = versionParser.Get();
    if (dot != '.')
    {
        // return the major and minor version
        return midletVersion;
    }
    err = versionParser.Val(iMIDletVersion.iBuild);
    if (KErrNone != err)
    {
        WLOG1(EJavaPreinstaller,
              "DesToAppVersion cannot parse midlet build version, error %d", err);
        // return at least the major and minor version
        return midletVersion;
    }

    if (!versionParser.Eos())
    {
        WLOG(EJavaPreinstaller,
             "DesToAppVersion suspicious midlet version, end has extra non number characters");
    }

    return midletVersion;
} // DesToAppVersion


/**
 * Parses the Uid in iMIDletUid and returns ETrue if there is a native
 * application with the same Uid installed into the device.
 * Returns EFalse in case of any error.
 *
 * @param[in] aApaSession open AppArc session
 * @return EFalse if no native application with Uid in iMIDletUid
 */
TBool CSilentMIDletInstall::IsNativeAppPresent(const RApaLsSession& aApaSession)
{
    std::wstring uidValue((wchar_t *)(iMIDletUid->Ptr()), iMIDletUid->Length());
    TUid appUid;
    Uid javaUid(uidValue);
    TInt err = uidToTUid(javaUid, appUid);

    if (KErrNone != err)
    {
        WLOG1(EJavaPreinstaller,
            "CSilentMIDletInstall::IsNativeAppPresent: Cannot convert %S to TUid",
            uidValue.c_str());
        return EFalse;
    }

    TUid appTypeUid;
    err = aApaSession.GetAppType(appTypeUid, appUid);
    if (KErrNone != err)
    {
        if (KErrNotFound == err)
        {
            // No such application in device
            return EFalse;
        }

        // Cannot check presence from AppArc
        ELOG1(EJavaPreinstaller,
            "CSilentMIDletInstall::IsNativeAppPresent: RApaLsSession GetAppType error %d", err);
        return EFalse;
    }

    if (appTypeUid.iUid != KMidletApplicationTypeUid)
    {
        // The application is present and it is not a MIDlet
        return ETrue;
    }

    return EFalse;
} // IsNativeAppPresent

