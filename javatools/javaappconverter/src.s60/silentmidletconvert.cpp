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


#include <e32base.h>
#include <apgcli.h>
#include <pathinfo.h>
#include <driveinfo.h>

#include "silentmidletconvert.h"
#include "javauids.h"
#include "javacommonutils.h"
#include "logger.h"
#include "javaprocessconstants.h"
#include "javasymbianoslayer.h"


using namespace java::util;

_LIT(KJarFileNameSuffix, ".jar");
_LIT(KJadFileNameSuffix, ".jad");
_LIT(KFLJarFileNameSuffix, ".dm");
_LIT(KSDJarFileNameSuffix, ".dcf");
_LIT(KSMCBackSplash, "\\");

// The directory where the java applications to be converted are searched from
_LIT(KMidletSuiteInstallBasePath, ":\\Private\\102033E6\\MIDlets\\");


/**
 * To create new instance of this class.
 *
 * @param aFs - A reference to the file server.
 * @return Reference to the object of this class.
 * @exception If construction fails.
 *
 */
CSilentMIDletConvert* CSilentMIDletConvert::NewLC(RFs& aFs)
{
    CSilentMIDletConvert* self = new(ELeave) CSilentMIDletConvert(aFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

/**
 * To do 1st phase construction for this object.
 *
 * Adds this active object to the scheduler.
 *
 * @param aFs - A reference to the file server.
 */
CSilentMIDletConvert::CSilentMIDletConvert(RFs& aFs) :
        CActive(EPriorityStandard), iFs(aFs), iNumberOfAppsToInstall(0),
        iDefaultTargetInstallationDrive(-1)
{
    CActiveScheduler::Add(this);
}

/**
 * To do 2nd phase construction for this object.
 *
 * @exception If the method is not able to allocate necessary buffers.
 */
void CSilentMIDletConvert::ConstructL()
{
    JELOG2(EJavaConverters);

    iConvertServer = new(ELeave) ConvertServer();
}

/**
 * Deletes this object.
 * All allocated resources are released.
 */
CSilentMIDletConvert::~CSilentMIDletConvert()
{
    JELOG2(EJavaConverters);
    Cancel();
    iInstallFiles.ResetAndDestroy();
    iInstallFiles.Close();

    iDirs.ResetAndDestroy();
    iDirs.Close();

    delete iConvertServer;
    iConvertServer = NULL;
}

/**
 * To start silent installation.
 */
void CSilentMIDletConvert::Start()
{
    JELOG2(EJavaConverters);
    iState = EFindOutDeviceDrives;
    CompleteRequest();
}

/**
 * To stop whole preinstaller.
 * Stops the active scheduler.
 */
void CSilentMIDletConvert::Exit()
{
    Deque();
    CActiveScheduler::Stop();
}

/**
 * To complete the request for this object.
 *
 * @Postconditions The following conditions are true immediately after
 * returning from this method.
 * - iStatus == KErrNone
 * - IsActive() == ETrue
 */
void CSilentMIDletConvert::CompleteRequest()
{
    JELOG2(EJavaConverters);

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
 * The state logic is:
 *
 * EFindOutDeviceDrives: find out all non-remote drives in the device
 *    - mark externally mountable drives (removable drives)
 *    - mark read only drives (ROM)
 *
 * EFindOutDrivesToBeScannedNow: make list of drives to be scanned
 *    - start with all mounted drives
 *
 * EAppsInInstalledDirectories: go through all directories in iDirs one by one
 *  and find all java applications in the subdirectories of the directory
 *    - if only .jar/.dcf/.dm file is found it is added to iInstallFiles
 *    - if .jad file is found, it is added to iInstallFiles
 *
 * ECheckWhichAppsShouldBeConverted: for each java application check from
 *  AppArc whether it should be converted
 *    - if the application has been installed, convert it
 *
 * EExecuteConvertServer: give list of install files to preinstall server,
 *  start Java Installer in poll mode, start convert server in another thread,
 *  convert server will give the names of the .jad or .jar/.dcf/.dm files to Java Installer
 *  when it polls them,  convert server will log whether each installation
 *  succeeded. After all install files have been polled, convert server will ask
 *  Java Installer to exit. After that this active object moves to next state.
 *
 * EMarkConversionDone: remove 'uids' data file created by javaapppreconverter
 *
 * EExit: free resources and exit
 *
 */
void CSilentMIDletConvert::RunL()
{
    JELOG2(EJavaConverters);

    switch (iState)
    {
    case EFindOutDeviceDrives:
    {
        LOG(EJavaConverters, EInfo,
            "CSilentMIDletConvert::RunL EFindOutDeviceDrives");
        GetAllDeviceDrivesL();
        iState = EFindOutDrivesToBeScannedNow;
        CompleteRequest();
    }
    break;

    case EFindOutDrivesToBeScannedNow:
    {
        LOG(EJavaConverters, EInfo,
            "CSilentMIDletConvert::RunL EFindOutDrivesToBeScannedNow");

        TChar driveChar = 'C';

        // forget old search directories
        iDirs.ResetAndDestroy();

        for (TInt drive = 0; drive < KMaxDrives; drive++)
        {
            // All present drives will be scanned for
            // java applications to be converted
            if (iDriveStatuses[drive] & DriveInfo::EDrivePresent)
            {
                (void)iFs.DriveToChar(drive, driveChar);

                // The base Java application install directory in this
                // drive must be scanned.
                // Reserve memory also for drive letter and terminating zero
                // for logging.
                HBufC *baseInstallDir = HBufC::NewLC(KMidletSuiteInstallBasePath().Length() + 2);

                TPtr dirPtr(baseInstallDir->Des());
                dirPtr.Append(driveChar);
                dirPtr.Append(KMidletSuiteInstallBasePath());

                // Add new search directory
                iDirs.AppendL(baseInstallDir);
                CleanupStack::Pop(baseInstallDir);
            }
        }

        iState = EAppsInInstalledDirectories;
        CompleteRequest();
    }
    break;

    case EAppsInInstalledDirectories:
    {
        LOG(EJavaConverters, EInfo,
            "CSilentMIDletConvert::RunL EAppsInInstalledDirectories");

        GetInstallFilesL(iDirs);

        iState = ECheckWhichAppsShouldBeConverted;
        CompleteRequest();
    }
    break;

    case ECheckWhichAppsShouldBeConverted:
    {
        LOG(EJavaConverters, EInfo,
            "CSilentMIDletConvert::RunL ECheckWhichAppsShouldBeConverted");

        // Check all Jad / Jar files in iInstallFiles and remove those
        // that need not be converted (those that cannot be found from
        // AppArc). Updates also iNumberOfAppsToInstall
        CheckNeedToInstall();

        iState = EExecuteConvertServer;
        CompleteRequest();
    }
    break;

    case EExecuteConvertServer:
    {
        LOG(EJavaConverters, EInfo,
            "CSilentMIDletConvert::RunL EExecuteConvertServer");

        if (iNumberOfAppsToInstall > 0)
        {
            // Install all MIDlet Suites still in iInstallFiles.

            // Pass iInstallFiles to ConvertServer
            iConvertServer->setInstallFiles(iInstallFiles);

            // Start the server
            int err = iConvertServer->start();
            if (0 != err)
            {
                // server cannot be started
                ELOG1(EJavaConverters,
                      "Cannot start convert server, err %d", err);
            }

            // Starts Java Installer, waits until Java Installer exits
            RunJavaInstallerL();

            iState = EMarkConversionDone;
        }
        else
        {
            iState = EMarkConversionDone;
            CompleteRequest();
        }
    }
    break;

    case EMarkConversionDone:
    {
        // If this line is executed, converting old midlets succeeded.
        // Remove the 'uids' data file so that conversion will not be
        // done again if OMJ sis package is installed again to the same device.
        RemoveDataFile();

        iState = EExit;
        CompleteRequest();
    }
    break;

    case EExit:
    {
        LOG(EJavaConverters, EInfo, "CSilentMIDletConvert::RunL EExit");

        // Stops the server if it is running
        iConvertServer->stop();

        FullCleanup();

        // The whole javaappconverter process is stopped.
        Exit();
    }
    break;

    }
}

/**
 * To handle leave from RunL.
 * This method exits this active object using normal state machine
 * After calling this method this active object will exit.
 *
 * @param aError - A reason of error.
 * @return KErrNone.
 */
TInt CSilentMIDletConvert::RunError(TInt aError)
{
    ELOG2(EJavaConverters,
          "CSilentMIDletConvert::RunError(%d) from state %d", aError, iState);

    Cancel();

    iState = EExit;
    CompleteRequest();

    return KErrNone;
}

/**
 * To do cancelling for this object.
 *
 */
void CSilentMIDletConvert::DoCancel()
{
    ELOG1(EJavaConverters,
          "CSilentMIDletConvert::DoCancel from state %d", iState);

    // Check whether convert server must be stopped
    if (iState == EExecuteConvertServer)
    {
        // Stops the server if it is running
        iConvertServer->stop();
    }
}

/**
 * Add .jad files in the directories specified in aDirs to iInstallFiles.
 * If a directory does not contain a .jad file but it contains a .jar/.dcf/.dm file,
 * add that file to iInstallFiles
 *
 * @param aDirs - An array of directories to be scanned.
 * @exception Unable to alloc memory for the internal buffers.
 * @exception Unable to get directory's contents.
 */
void CSilentMIDletConvert::GetInstallFilesL(RPointerArray<HBufC>& aDirs)
{
    JELOG2(EJavaConverters);

    // Number of MIDlet installation base directories to scan,
    // one directory per local drive
    const TInt nBaseDirs = aDirs.Count();

    for (TInt i = 0; i < nBaseDirs; i++)
    {
        TFileName suitePath;

        // Form base directory path, MIDlets have been installed
        // to subdirectories of this directory
        const TPtrC& dir = ((HBufC *)(aDirs[i]))->Des();
        TFileName baseSuitePath = dir;

        CDir *entryList;
        CDir *dirList;
        TInt err = iFs.GetDir(baseSuitePath, KEntryAttMatchMask, ESortNone, entryList, dirList);
        if (KErrNone != err)
        {
            WLOG1WSTR(EJavaConverters,
                      "CSilentMIDletConvert::GetInstallFilesL Cannot list directory %s",
                      (wchar_t *)(baseSuitePath.PtrZ()));

            // If no S60 Java application has been installed to the drive,
            // the directory does not exist but it is OK
            if (KErrPathNotFound != err)
            {
                User::Leave(err);
            }
            continue;
        }

        // Only midlet installation directory entries are meaningfull,
        // discard other possible entries
        delete entryList;

        TInt   nCount = dirList->Count();
        TEntry dirEntry;

        for (TInt nInd = 0; nInd < nCount; nInd++)
        {
            dirEntry = (*dirList)[nInd];
            // Just to make sure this really is subdirectory
            if (dirEntry.IsDir())
            {
                // Is this midlet suite subdir?
                // Midlet suite subdirs look like this "[102b567B]"
                // Must parse the value of the Uid as unsigned int to avoid
                // overflow
                TUint32 nUid;
                TLex   lexer(dirEntry.iName);
                lexer.Inc();
                err = lexer.Val(nUid, EHex);
                if (KErrNone != err)
                {
                    // Not midlet suite subdir, skip it
                    continue;
                }

                // Does the subdirectory contain any Jar files?
                suitePath = baseSuitePath;
                suitePath.Append(dirEntry.iName);
                suitePath.Append(KSMCBackSplash);

                CDir *suiteDirEntryList;
                err = iFs.GetDir(suitePath, KEntryAttMatchMask, ESortNone, suiteDirEntryList);
                if (KErrNone != err)
                {
                    ELOG1WSTR(EJavaConverters,
                              "CSilentMIDletConvert::GetInstallFilesL Cannot list content of suite dir %s",
                              (wchar_t *)(suitePath.PtrZ()));
                    User::Leave(err);
                    return;
                }
                // If there is .jad or Jar file in suiteDirEntryList, adds
                // it to iInstallFiles.
                // Recognizes also DRM protected Jar files (.dm, .dcf)
                AddJadJarToInstallFilesL(suitePath, suiteDirEntryList);
                delete suiteDirEntryList;
            }
        } // for - loop all directory entries in a base installation directories
        delete dirList;

    } // for - loop all base installation directories

    if (iInstallFiles.Count() == 0)
    {
        WLOG(EJavaConverters,
             "CSilentMIDletConvert::GetInstallFilesL No MIDlets to convert");
    }
}

/**
 * Check all Jad / Jar files in iInstallFiles and remove those
 * that need not be converted. Updates also iNumberOfAppsToInstall.
 */
void CSilentMIDletConvert::CheckNeedToInstall()
{
    // It is too late to check from AppArc whether the application is a
    // valid java application or no. The Java registry dll:s have been eclipsed and
    // old apps are now invalid.
    // javaapppreconverter has stored the Uids of the valid midlets to 'uids' file
    // in to the data subdirectory of this process.
    // ReadMidletUids() can be used to read the uids of the valid midlets to array.
    // If uid is in the array, it is valid.
    TFileName uidsDataFilePathName(KUidsImportDataFilePathName);
    RArray<TUid> validUids = ReadMidletUids(uidsDataFilePathName);

    // loop through iInstallFiles one be one
    TInt   nNumberOfAppsToInstall = 0;
    TInt   err;
    TInt   nFiles = iInstallFiles.Count();
    TParse fp;
    for (TInt nInd = 0; nInd < nFiles; nInd++)
    {
        // get the directory path of the current iInstallFiles[nInd]
        err = fp.Set(*(iInstallFiles[nInd]), NULL, NULL);
        if (KErrNone != err)
        {
            WLOG1(EJavaConverters,
                  "CSilentMIDletConvert::CheckNeedToInstall Cannot parse install file name %s",
                  iInstallFiles[nInd]);
            continue;
        }

        TPtrC installDir = fp.DriveAndPath();

        // read Java applications uid(s) from 'uids' file in the directory
        RArray<TUid> appUids = ReadMidletUids(installDir);

        // For each app uid, check whether it is uid of an valid java  application.
        // If yes, make conversion installation for the .jad/Jar file
        // in this directory.
        TInt nIndUids;
        TInt nCountUids = appUids.Count();
        for (nIndUids = 0; nIndUids < nCountUids; nIndUids++)
        {
            err = validUids.Find(appUids[nIndUids]);
            if (KErrNotFound == err)
            {
                LOG1(EJavaConverters, EInfo,
                     "CSilentMIDletConvert::CheckNeedToInstall Midlet with Uid %x "
                     "was not registered to AppArc",
                     appUids[nIndUids].iUid);

                // Do not convert any app in this Midlet suite
                delete iInstallFiles[nInd];
                iInstallFiles[nInd] = NULL;
                break;
            }
            else
            {
                // The application is valid, so it should be converted
                nNumberOfAppsToInstall++;
            }
        }
        appUids.Close();
    }
    validUids.Close();

    // how many applications to convert
    iNumberOfAppsToInstall = nNumberOfAppsToInstall;
}

/**
 * Read the midlet uids from file 'uids' in the directory
 * given in parameter aDir and return them in RArray
 *
 * @param aDir -  directory that contains 'uids' file
 * @return the midlet uids in RArray. Caller must destroy the RArray
 */
RArray<TUid> CSilentMIDletConvert::ReadMidletUids(const TPtrC& aDir)
{
    RArray<TUid> midletUids;
    RFile fileUids;
    TFileName nameUids = aDir;
    nameUids.Append(_L("uids"));

    TInt err = fileUids.Open(iFs, nameUids, EFileRead);
    if (KErrNone != err)
    {
        // cannot open "uids" file
        nameUids.Append(0);
        WLOG1WSTR(EJavaConverters,
                  "CSilentMIDletConvert::ReadMidletUids cannot open uids file in dir %s",
                  (wchar_t *)(&(nameUids[0])));
        WLOG1(EJavaConverters, "Error is %d", err);
        return midletUids;
    }

    TInt fileSize(0);
    err = fileUids.Size(fileSize);
    if (fileSize < 8)
    {
        // valid "uids" file contains at least the uid of the suite and
        // uid of at least one midlet
        fileUids.Close();
        // invalid "uids" file
        nameUids.Append(0);
        WLOG1WSTR(EJavaConverters,
                  "CSilentMIDletConvert::ReadMidletUids invalid uids file in dir %s",
                  (wchar_t *)(&(nameUids[0])));
        return midletUids;
    }

    // reserve buffer for one uid
    TBuf8<4> readBuf;
    TDes8 readBufDes(readBuf);

    // The first uid is suite uid but we are not interested in it. Skip it.
    err = fileUids.Read(readBufDes);
    if (KErrNone != err)
    {
        fileUids.Close();
        // cannot read "uids" file
        nameUids.Append(0);
        WLOG1WSTR(EJavaConverters,
                  "CSilentMIDletConvert::ReadMidletUids cannot read uids file in dir %s",
                  (wchar_t *)(&(nameUids[0])));
        WLOG1(EJavaConverters, "Error is %d", err);
        return midletUids;
    }

    // Now read rest of the file one uid at a time
    TInt  nReadBytes = readBufDes.Length();
    TUint midletUidValue;
    TUint temp;

    while (nReadBytes == 4)
    {
        err = fileUids.Read(readBufDes);
        if (KErrNone != err)
        {
            fileUids.Close();
            // cannot read "uids" file
            nameUids.Append(0);
            WLOG1WSTR(EJavaConverters,
                      "CSilentMIDletConvert::ReadMidletUids(2) cannot from uids file in dir %s",
                      (wchar_t *)(&(nameUids[0])));
            WLOG1(EJavaConverters, "Error is %d", err);
            return midletUids;
        }

        nReadBytes = readBufDes.Length();
        if (nReadBytes == 4)
        {
            // One whole midlet id was read to buffer.
            // 4 bytes per midlet, least significant byte first
            TUid  midletUid;

            midletUidValue = readBufDes[0];
            temp = readBufDes[1];
            midletUidValue += temp<<8;
            temp = readBufDes[2];
            midletUidValue += temp<<16;
            temp = readBufDes[3];
            midletUidValue += temp<<24;

            midletUid.iUid = midletUidValue;
            midletUids.Append(midletUid);
        }
    }

    fileUids.Close();

    return midletUids;
}

/**
 * Scan the content of one directory entry and add the name of
 * .jad / .jar /.dcf/.dm file to iInstallFiles if
 * the directory contains a valid, installed Java application.
 * Recognizes also DRM protected jar files (.dm, .dcf)
 *
 * @param aSuitePathName -  directory to be scanned.
 * @param aSuiteDirEntryList - contents of the directory
 * @exception Unable to alloc memory for the internal buffers.
 */
void CSilentMIDletConvert::AddJadJarToInstallFilesL(
    const TFileName &aSuitePathName,
    const CDir *aSuiteDirEntryList)
{
    JELOG2(EJavaConverters);

    if (NULL == aSuiteDirEntryList)
    {
        return;
    }

    TInt nCount = aSuiteDirEntryList->Count();
    if (0 == nCount)
    {
        return;
    }

    TInt        suffixPos;
    TEntry      dirEntry;
    TBool       jarFileInSuiteDir = EFalse;
    TBool       jadFileInSuiteDir = EFalse;
    TFileName   jadFullPathName;
    TFileName   jarFullPathName;

    for (TInt nInd = 0; nInd < nCount; nInd++)
    {
        dirEntry = (*aSuiteDirEntryList)[nInd];
        // Directory cannot be Jar file.
        // Empty file cannot valid Jar file
        if (dirEntry.IsDir() || (dirEntry.iSize == 0))
        {
            continue;
        }
        // get the suffix of the name
        suffixPos = dirEntry.iName.LocateReverse('.');
        if (suffixPos == KErrNotFound)
        {
            // File name does not contain '.' char
            continue;
        }
        TPtrC suffix(dirEntry.iName.Mid(suffixPos));

        // if the name ends with ".jar" the name is current candidate
        // for the name to be added to iInstallFiles list
        if (suffix.CompareF(KJarFileNameSuffix) == 0)
        {
            jarFullPathName = aSuitePathName;
            jarFullPathName.Append(dirEntry.iName);
            jarFileInSuiteDir = ETrue;
        }
        else if (suffix.CompareF(KJadFileNameSuffix) == 0)
        {
            // If .jad file is found, then it will be added
            // to iInstallFiles list
            jadFullPathName = aSuitePathName;
            jadFullPathName.Append(dirEntry.iName);
            jadFileInSuiteDir = ETrue;
        }
        else if (suffix.CompareF(KFLJarFileNameSuffix) == 0)
        {
            // forward locked and combined delivery DRM protected
            // .jar files have suffix ".dm"
            jarFullPathName = aSuitePathName;
            jarFullPathName.Append(dirEntry.iName);
            jarFileInSuiteDir = ETrue;
        }
        else if (suffix.CompareF(KSDJarFileNameSuffix) == 0)
        {
            // separate delivery DRM protected .jar files have suffix ".dcf"
            jarFullPathName = aSuitePathName;
            jarFullPathName.Append(dirEntry.iName);
            jarFileInSuiteDir = ETrue;
        }
    }

    // If directory contains a Jar file, then add something to iInstallFiles
    if (jarFileInSuiteDir)
    {
        // If directory contains also .jad file, add .jad file name to iInstallFiles
        if (jadFileInSuiteDir)
        {
            // Reserve one char for null terminator
            HBufC* path = HBufC::NewLC(jadFullPathName.Length() + 1);
            TPtr pathPtr(path->Des());
            pathPtr.Append(jadFullPathName);

            LOG1WSTR(EJavaConverters, EInfo,
                     "CSilentMIDletConvert::AddJadJarToInstallFilesL Adding jad file %s",
                     (wchar_t *)(pathPtr.PtrZ()));
            iInstallFiles.Append(path);
            CleanupStack::Pop(path);
        }
        else
        {
            // Reserve one char for null terminator
            HBufC* path = HBufC::NewLC(jarFullPathName.Length() + 1);
            TPtr pathPtr(path->Des());
            pathPtr.Append(jarFullPathName);

            LOG1WSTR(EJavaConverters, EInfo,
                     "CSilentMIDletConvert::AddJadJarToInstallFilesL Adding jar file %s",
                     (wchar_t *)(pathPtr.PtrZ()));
            iInstallFiles.Append(path);
            CleanupStack::Pop(path);
        }
    }
}

/**
 * Start Java Installer in poll mode and then wait until it exits.
 */
void CSilentMIDletConvert::RunJavaInstallerL()
{
    LOG(EJavaConverters, EInfo, "CSilentMIDletConvert::RunJavaInstaller");

    RProcess rJavaInstaller;
    TFileName fileName;
    TInt err;
    // Max one path name, user name and password and some options ->
    // 1536 is enough
    TBuf<1536> commandLine;

    // Build command line used to pass all necessary info to Java Installer
    std::auto_ptr<HBufC> installerStarterDll(
        stringToDes(java::runtime::JAVA_INSTALLER_STARTER_DLL));
    commandLine = installerStarterDll->Des();
    commandLine.Append(_L(" poll -address=convert"));

    // Run installer silently
    commandLine.Append(_L(" -silent"));

    // Convert old S60 applications so that applications uids,
    // private data and RMS data are all preserved
    commandLine.Append(_L(" -convert=yes"));

    // Upgrading MIDlets is allowed
    commandLine.Append(_L(" -upgrade=yes"));

    // No OCSP checks for converted MIDlets
    commandLine.Append(_L(" -ocsp=no"));

    // Allow upgrade even if version number has not increased
    commandLine.Append(_L(" -overwrite=yes"));

    // Downloading .jar is not allowed.
    commandLine.Append(_L(" -download=no"));

    // If upgrade install, automatically upgrade also the data
    commandLine.Append(_L(" -upgrade_data=yes"));

    // If the device has an internal drive with much free disk space,
    // install all converted applications to that drive
    if (iDefaultTargetInstallationDrive > -1)
    {
        TChar targetDrive;
        err = RFs::DriveToChar(iDefaultTargetInstallationDrive, targetDrive);
        if (KErrNone == err)
        {
            commandLine.Append(_L(" -drive="));
            commandLine.Append(targetDrive);
        }
    }

    // start JavaInstaller
    std::auto_ptr<HBufC> installerProcess(
        stringToDes(java::runtime::JAVA_PROCESS));
    err = rJavaInstaller.Create(installerProcess->Des(), commandLine);
    if (KErrNone == err)
    {
        LOG(EJavaConverters, EInfo, "CSilentMIDletConvert::RunJavaInstaller calling Rendezvous");
        // This call will wait until Java Installer exits (or panics)
        rJavaInstaller.Logon(iStatus);

        LOG(EJavaConverters, EInfo, "CSilentMIDletConvert::RunJavaInstaller calling Resume");
        rJavaInstaller.Resume();
    }
    else
    {
        ELOG1(EJavaConverters,
              "CSilentMIDletConvert::RunJavaInstaller Cannot start Installer, error %d", err);
        // CActive will trap the following leave, execution will go to RunError
        User::Leave(err);
    }

    LOG(EJavaConverters, EInfo, "CSilentMIDletConvert::RunJavaInstaller calling RProcess::Close");
    // free resources before returning
    rJavaInstaller.Close();

    // now wait until Java Installer exits
    SetActive();
}

/**
 * To cleanup member variables.
 */
void CSilentMIDletConvert::FullCleanup()
{
    JELOG2(EJavaConverters);

    iDirs.ResetAndDestroy();
    iInstallFiles.ResetAndDestroy();
}


/**
 * Checks all local drives in the device and stores the DriveInfo API drive
 * status information for each drive to iDriveStatuses
 * @exception Cannot get drive list.
 */
void CSilentMIDletConvert::GetAllDeviceDrivesL()
{
    JELOG2(EJavaConverters);

    TDriveList driveList;
    // get all drives
    TInt err = iFs.DriveList(driveList);
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "CSilentMIDletConvert::GetAllDeviceDrives cannot get drive list, err %d", err);
        User::Leave(err);
    }

    // store status of the non-remote, non-substed drives
    TInt64 requiredFreeDiskSpace = KMinimumFreeDiskSpace;
    TUint  status = 0;
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
            ELOG2(EJavaConverters,
                  "CSilentMIDletConvert::GetAllDeviceDrivesL cannot get drive %d status, err %d",
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

        // If the device has an internal (non-removable) drive with much
        // free disk space, use that drive as the target installation drive
        // when making the conversion installations.
        // Skip drives A: to D:, C: drive is not valid target drive because
        // we do not want to fill C: drive unnecessarily
        if (drive < EDriveE)
        {
            continue;
        }
        // drive must be internal and visible to the user but
        // it must NOT be corrupted or in exclusive use or read only
        if ((status & DriveInfo::EDriveInternal) &&
                (!(status & DriveInfo::EDriveCorrupt)) &&
                (!(status & DriveInfo::EDriveInUse)) &&
                (!(status & DriveInfo::EDriveReadOnly)) &&
                (status & DriveInfo::EDriveUserVisible))
        {
            // Is there 'enough' free disk space for conversion
            TVolumeInfo volumeInfo;
            err = iFs.Volume(volumeInfo, drive);
            if (KErrNone == err)
            {
                if (volumeInfo.iFree > requiredFreeDiskSpace)
                {
                    iDefaultTargetInstallationDrive = drive;

                    // If some other internal drive has even more free
                    // disk space, it will be used as the target drive
                    requiredFreeDiskSpace = volumeInfo.iFree;
                }
            }
        }
    }
}

/**
 * Remove the data file that contain information about the midlets to be converted.
 */
void CSilentMIDletConvert::RemoveDataFile()
{
    TFileName dataFile(KUidsImportDataFilePathName);
    dataFile.Append(KUidsImportDataFileName);

    TInt err = iFs.Delete(dataFile);
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "CSilentMIDletConvert::RemoveDataFile: Cannot delete data file, err %d ",
              err);
    }
}
