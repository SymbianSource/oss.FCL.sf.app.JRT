/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Java platform 2.0 javarestoreconverter process
*
*/


#include <apacmdln.h>
#include <apgcli.h>
#include <driveinfo.h>
#include <e32base.h>
#include <pathinfo.h>
#include <utf.h>
#include <zipfile.h>

#include "restoreconvertmidlet.h"

#include "javacommonutils.h"
#include "javaprocessconstants.h"
#include "javastorage.h"
#include "javastorageentry.h"
#include "javastoragenames.h"
#include "javasymbianoslayer.h"
#include "javauids.h"
#include "logger.h"


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


_LIT(KJarFileNameSuffix, ".jar");
_LIT(KJadFileNameSuffix, ".jad");
_LIT(KFLJarFileNameSuffix, ".dm");
_LIT(KSDJarFileNameSuffix, ".dcf");
_LIT(KSMCBackSplash, "\\");
_LIT(KDriveText, "drive");
_LIT(KMidletName, "MIDlet-Name");
_LIT(KMidletVendor, "MIDlet-Vendor");
_LIT(KManifestEntryName, "META-INF/MANIFEST.MF");

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
CRestoreConvertMIDlet* CRestoreConvertMIDlet::NewLC(RFs& aFs)
{
    CRestoreConvertMIDlet* self = new(ELeave) CRestoreConvertMIDlet(aFs);
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
CRestoreConvertMIDlet::CRestoreConvertMIDlet(RFs& aFs) :
        CActive(EPriorityStandard), iFs(aFs), iNumberOfAppsToInstall(0)
{
    CActiveScheduler::Add(this);
}

/**
 * To do 2nd phase construction for this object.
 *
 * @exception If the method is not able to allocate necessary buffers.
 */
void CRestoreConvertMIDlet::ConstructL()
{
    iConvertServer = new(ELeave) RestoreServer();
    iMIDletName    = HBufC::NewL(KMaxBufferSize);
    iMIDletVendor  = HBufC::NewL(KMaxBufferSize);
}

/**
 * Deletes this object.
 * All allocated resources are released.
 */
CRestoreConvertMIDlet::~CRestoreConvertMIDlet()
{
    Cancel();
    iInstallFiles.ResetAndDestroy();
    iInstallFiles.Close();

    iUninstallUids.clear();

    iDirs.ResetAndDestroy();
    iDirs.Close();

    iIsJad.Reset();

    delete iMIDletName;
    iMIDletName = NULL;

    delete iMIDletVendor;
    iMIDletVendor = NULL;

    delete iConvertServer;
    iConvertServer = NULL;
}

/**
 * To start silent installation.
 */
void CRestoreConvertMIDlet::Start()
{
    iState = EFindOutDeviceDrives;
    CompleteRequest();
}

/**
 * To stop whole preinstaller.
 * Stops the active scheduler.
 */
void CRestoreConvertMIDlet::Exit()
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
void CRestoreConvertMIDlet::CompleteRequest()
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
 * The state logic is:
 *
 * EFindOutDeviceDrives:
 *    - find out all non-remote drives in the device
 *    - mark externally mountable drives (removable drives)
 *    - mark read only drives (ROM)
 *
 * EGetDriveToBeScanned:
 *    - the drive to be scanned has been given as command line argument,
 *      store it to iDrive
 *
 * EAppsInInstalledDirectories:
 *    - go through all directories in iDirs one by one
 *      and find all java applications in the subdirectories of the directory
 *    - if only .jar/.dcf/.dm file is found it is added to iInstallFiles
 *    - if .jad file is found, it is added to iInstallFiles
 *
 * EUninstallFromOMJ:
 *   - uninstall all OMJ java applications in drive iDrive using Java Installer
 *     command line options 'uninstallall -drive=<iDrive>'
 *   - then for each java application in iInstallFiles try to find it from
 *     Java Storage based on MIDlet suite name and vendor name. If the MIDlet
 *     is still installed to OMJ (to some other drive than iDrive), it must
 *     be uninstalled from OMJ to reset the state of MIDlet (RMS data etc)
 *     to original state. The actual uninstallation is not done in this
 *     state, the uids of the applications to be uninstalled are just added to
 *     iUninstallUids in string format
 *
 * EExecuteRestoreServer:
 *  - give list of uninstall uids and list of install files to restore server
 *  - start restore server in another thread (it will listen to Comms messages)
 *  - start Java Installer in poll mode
 *  - when Java Installer polls for operations to be executed, the restore server
 *    will first ask it to uninstall each Java application in the list of uninstall uids,
 *    then restore server will ask Java Installer to install each .jad or .jar/.dcf/.dm file
 *    in the list of install files.
 *    Restore server will log whether each uninstallation / installation succeeds.
 *  - after all uninstallations and installations have been done, restore server will ask
 *    Java Installer to exit.
 *  - after that this active object moves to next state.
 *
 * EExit: free resources and exit
 *
 */
void CRestoreConvertMIDlet::RunL()
{
    switch (iState)
    {
    case EFindOutDeviceDrives:
    {
        LOG(EJavaConverters, EInfo,
            "CRestoreConvertMIDlet::RunL EFindOutDeviceDrives");
        GetAllDeviceDrivesL();
        iState = EGetDriveToBeScanned;
        CompleteRequest();
    }
    break;

    case EGetDriveToBeScanned:
    {
        LOG(EJavaConverters, EInfo,
            "CRestoreConvertMIDlet::RunL EGetDriveToBeScanned");

        HBufC *pBufCmdLine = HBufC::NewLC(User::CommandLineLength() + 10);
        TPtr args = pBufCmdLine->Des();
        User::CommandLine(args);
        LOG1WSTR(EJavaConverters, EInfo,
                 "Command line arguments of javarestoreconverter.exe are : %s",
                 (wchar_t *)(args.PtrZ()));

        // Check which drive is to be restored now from command line.
        TDriveNumber drive = EDriveC;
        TInt idx = args.Find(KDriveText());
        if (KErrNotFound == idx)
        {
            LOG1WSTR(EJavaConverters, EError,
                     "no -drive<X> argument in javarestoreconveter.exe command line (%s)",
                     (wchar_t *)(args.PtrZ()));
            User::Leave(KErrNotFound);
        }

        TChar driveChar = args[idx + KDriveText().Length()];
        char argLetter = args[idx + KDriveText().Length()];
        if (argLetter >= 'A' && argLetter <= 'J')
        {
            drive = (TDriveNumber)(argLetter - 'A');
        }
        else if (argLetter >= 'a' && argLetter <= 'j')
        {
            drive = (TDriveNumber)(argLetter - 'a');
        }
        else
        {
            User::Leave(KErrArgument);
        }
        CleanupStack::PopAndDestroy(pBufCmdLine);
        iDrive = drive;

        LOG1(EJavaConverters, EInfo, "Drive is %d", drive);

        // forget old search directories
        iDirs.ResetAndDestroy();

        // The specified drive will be scanned for
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
        else
        {
            // Restoring MIDlets to drive X but drive X is not present
            ELOG1(EJavaConverters,
                  "javarestoreconverter.exe: Drive number %d is not present", drive);
            User::Leave(KErrDisMounted);
        }

        iState = EAppsInInstalledDirectories;
        CompleteRequest();
    }
    break;

    case EAppsInInstalledDirectories:
    {
        LOG(EJavaConverters, EInfo,
            "CRestoreConvertMIDlet::RunL EAppsInInstalledDirectories");

        GetInstallFilesL(iDirs);


        iState = EUninstallFromOMJ;
        CompleteRequest();
    }
    break;

    case EUninstallFromOMJ:
    {
        LOG(EJavaConverters, EInfo,
            "CRestoreConvertMIDlet::RunL EUninstallFromOMJ");

        // Uninstall all Java applications in current drive
        UninstallAllFromDriveL(iDrive);

        // Check all Jad / Jar files in iInstallFiles and
        // add the Uids of those MIDlets still installed into OMJ
        // to iUninstallUids
        FindRemainingMIDletsToBeUninstalledL();

        iState = EExecuteRestoreServer;
        CompleteRequest();
    }
    break;

    case EExecuteRestoreServer:
    {
        LOG(EJavaConverters, EInfo,
            "CRestoreConvertMIDlet::RunL EExecuteRestoreServer");

        if (iNumberOfAppsToInstall > 0)
        {
            // Uninstall all MIDlet suites in iUninstallUids and then
            // install all MIDlet suites in iInstallFiles.

            // Tell RestoreServer what should be uninstalled and what
            // should be converted
            iConvertServer->setOperations(iInstallFiles, iUninstallUids);

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

            iState = EExit;
        }
        else
        {
            iState = EExit;
            CompleteRequest();
        }
    }
    break;

    case EExit:
    {
        LOG(EJavaConverters, EInfo, "CRestoreConvertMIDlet::RunL EExit");

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
TInt CRestoreConvertMIDlet::RunError(TInt aError)
{
    ELOG2(EJavaConverters,
          "CRestoreConvertMIDlet::RunError(%d) from state %d", aError, iState);

    Cancel();

    iState = EExit;
    CompleteRequest();

    return KErrNone;
}

/**
 * To do cancelling for this object.
 *
 */
void CRestoreConvertMIDlet::DoCancel()
{
    ELOG1(EJavaConverters,
          "CRestoreConvertMIDlet::DoCancel from state %d", iState);

    // Check whether restore server must be stopped
    if (iState == EExecuteRestoreServer)
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
void CRestoreConvertMIDlet::GetInstallFilesL(RPointerArray<HBufC>& aDirs)
{
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
                      "CRestoreConvertMIDlet::GetInstallFilesL Cannot list directory %s",
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
                    LOG1WSTR(EJavaConverters, EError,
                             "CRestoreConvertMIDlet::GetInstallFilesL Cannot list content of suite dir %s",
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
             "CRestoreConvertMIDlet::GetInstallFilesL No MIDlets to convert");
    }

    // how many applications to restore
    iNumberOfAppsToInstall = iInstallFiles.Count();
}

/**
 *  For each java application in iInstallFiles try to find it from
 *  Java Storage based on MIDlet suite name and vendor name. If the MIDlet
 *  is still installed to OMJ (to some other drive than iDrive), it must
 *  be uninstalled from OMJ to reset the state of MIDlet (RMS data etc)
 *  to original state. Add the uids of the applications to be uninstalled
 *  to iUninstallUids in string format
 */
void CRestoreConvertMIDlet::FindRemainingMIDletsToBeUninstalledL()
{
    TBool namesParsed;
    for (TInt nInd = 0; nInd < iNumberOfAppsToInstall; nInd++)
    {
        // Get the MIDlet-Name and MIDlet-Vendor from iInstallFiles[nInd]
        if (iIsJad[nInd] == 1)
        {
            namesParsed = ParseJadL(*iInstallFiles[nInd]);
        }
        else
        {
            // If file is .jar file, get the names from Manifest
            namesParsed = ParseJarL(*iInstallFiles[nInd]);
        }

        // Search for the MIDlet suite from Java Storage table APPLICATION_PACKAGE_TABLE
        // If it is found, add the suite Uid in string format to iUninstallUids
        if (namesParsed)
        {
            TPtr suite = iMIDletName->Des();
            TPtr vendor = iMIDletVendor->Des();
            std::wstring suiteUid = IsSuiteInstalled(suite, vendor);
            if (!suiteUid.empty())
            {
                iUninstallUids.push_back(suiteUid);
            }
        }
    }
}

/**
 * Parse MIDlet-Name and MIDlet-Vendor parameters from JAD file.
 * Parameters are stored to iMIDletName and iMIDletVendor
 *
 * @param ETrue if parsing succeeds otherwise EFalse.
 */
TBool CRestoreConvertMIDlet::ParseJadL(const TDesC& aJadFileName)
{
    HBufC *jadContent = NULL;
    // Trap leave thrown if reading jad content fails
    TRAPD(err, jadContent = GetJadContentL(aJadFileName));
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "CRestoreConvertMIDlet::ParseJadL Reading Jad content failed, error %d",
              err);
        return EFalse;
    }
    CleanupStack::PushL(jadContent);

    HBufC *midletName = ParseAttribute(jadContent, KMidletName);
    if (NULL == midletName)
    {
        ELOG(EJavaConverters,
             "CRestoreConvertMIDlet::ParseJadL Parsing midlet name failed.");
        CleanupStack::PopAndDestroy(jadContent);
        return EFalse;
    }
    // store midlet name to member variable and log it
    TPtr namePtr(iMIDletName->Des());
    namePtr.Copy(*midletName);
    LOG1WSTR(EJavaConverters, EInfo,
             "CRestoreConvertMIDlet::ParseJadL MIDlet-Name %s",
             (wchar_t *)(namePtr.PtrZ()));
    delete midletName;

    HBufC *midletVendor = ParseAttribute(jadContent, KMidletVendor);
    if (NULL == midletVendor)
    {
        ELOG(EJavaConverters,
             "CRestoreConvertMIDlet::ParseJadL Parsing midlet vendor failed.");
        CleanupStack::PopAndDestroy(jadContent);
        return EFalse;
    }
    // store midlet vendor to member variable and log it
    TPtr vendorPtr(iMIDletVendor->Des());
    vendorPtr.Copy(*midletVendor);
    LOG1WSTR(EJavaConverters, EInfo,
             "CRestoreConvertMIDlet::ParseJadL MIDlet-Vendor %s",
             (wchar_t *)(vendorPtr.PtrZ()));
    delete midletVendor;

    CleanupStack::PopAndDestroy(jadContent);
    return ETrue;
}


/**
 * Reads the whole content of the Jad file and returns it in
 * buffer in Symbian Unicode character set.
 * @param[in] aJadFile
 * @return pointer to HBufC that contains the Jad file,
 * ownership is transferred to caller
 * @exception If jad file content cannot be read
 */
HBufC *CRestoreConvertMIDlet::GetJadContentL(const TDesC& aJadFileName)
{
    RFile jadFile;
    TInt err = jadFile.Open(iFs, aJadFileName, EFileRead);
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
}


/**
 * Parse MIDlet-Name and MIDlet-Vendor parameters from Manifest inside
 * a .jar file.
 * Parameters are stored to iMIDletName and iMIDletVendor
 *
 * @param ETrue if parsing succeeds otherwise EFalse.
 */
TBool CRestoreConvertMIDlet::ParseJarL(const TDesC& aJarFileName)
{
    HBufC *manifestContent = NULL;
    // Trap leave thrown if reading Manifest content fails
    TRAPD(err, manifestContent = GetManifestContentL(aJarFileName));
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "CRestoreConvertMIDlet::ParseJarL Reading Manifest failed, error %d",
              err);
        return EFalse;
    }
    CleanupStack::PushL(manifestContent);

    HBufC *midletName = ParseAttribute(manifestContent, KMidletName);
    if (NULL == midletName)
    {
        ELOG(EJavaConverters,
             "CRestoreConvertMIDlet::ParseJarL Parsing midlet name failed.");
        CleanupStack::PopAndDestroy(manifestContent);
        return EFalse;
    }
    // store midlet name to member variable and log it
    TPtr namePtr(iMIDletName->Des());
    namePtr.Copy(*midletName);
    LOG1WSTR(EJavaConverters, EInfo,
             "CRestoreConvertMIDlet::ParseJarL MIDlet-Name %s",
             (wchar_t *)(namePtr.PtrZ()));
    delete midletName;

    HBufC *midletVendor = ParseAttribute(manifestContent, KMidletVendor);
    if (NULL == midletVendor)
    {
        ELOG(EJavaConverters,
             "CRestoreConvertMIDlet::ParseJarL Parsing midlet vendor failed.");
        CleanupStack::PopAndDestroy(manifestContent);
        return EFalse;
    }
    // store midlet vendor to member variable and log it
    TPtr vendorPtr(iMIDletVendor->Des());
    vendorPtr.Copy(*midletVendor);
    LOG1WSTR(EJavaConverters, EInfo,
             "CRestoreConvertMIDlet::ParseJarL MIDlet-Vendor %s",
             (wchar_t *)(vendorPtr.PtrZ()));
    delete midletVendor;

    CleanupStack::PopAndDestroy(manifestContent);
    return ETrue;
}


/**
 * Reads the whole content of the Manifest inside Jar file
 * and returns it in buffer in Symbian Unicode character set.
 * @param[in] aJarFile
 * @return pointer to HBufC that contains the Manifest,
 * ownership is transferred to caller
 * @exception If Manifest content cannot be read
 */
HBufC *CRestoreConvertMIDlet::GetManifestContentL(const TDesC& aJarFileName)
{
    CZipFile* zipFile = CZipFile::NewL(iFs, aJarFileName);
    CleanupStack::PushL(zipFile);

    // Seek manifest file
    CZipFileMember* zippedFile =
        zipFile->CaseSensitiveOrCaseInsensitiveMemberL(KManifestEntryName());
    if (!zippedFile)
    {
        ELOG(EJavaConverters,
             "CRestoreConvertMIDlet::GetManifestContentL Package is missing manifest");
        User::Leave(KErrNotFound);
    }
    CleanupStack::PushL(zippedFile);

    TUint uncompressedSize = zippedFile->UncompressedSize();
    if ((TUint)uncompressedSize >= (KMaxTInt/2))
    {
        ELOG(EJavaConverters, "CRestoreConvertMIDlet::GetManifestContentL Invalid manifest");
        User::Leave(KErrCorrupt);
    }

    // Buffer to read Manifest into
    HBufC8* resultData = HBufC8::NewLC(uncompressedSize);

    RZipFileMemberReaderStream* zippedStream = 0;
    TInt err = zipFile->GetInputStreamL(zippedFile, zippedStream);
    User::LeaveIfError(err);
    CleanupStack::PushL(zippedStream);

    TPtr8 ptr(resultData->Des());
    User::LeaveIfError(zippedStream->Read(ptr, uncompressedSize));

    CleanupStack::PopAndDestroy(zippedStream);

    // Manifest buffer when converted to UCS-2
    HBufC *manifestContent = HBufC16::NewL(uncompressedSize);
    TPtr16 ucsPtr(manifestContent->Des());
    err = CnvUtfConverter::ConvertToUnicodeFromUtf8(ucsPtr, ptr);
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "CRestoreConvertMIDlet::GetManifestContentL UTF-8 to unicode"
              " conversion failed: %d", err);
        User::Leave(err);
    }
    CleanupStack::PopAndDestroy(resultData);
    CleanupStack::PopAndDestroy(zippedFile);
    CleanupStack::PopAndDestroy(zipFile);

    // Return to caller
    return manifestContent;
}


std::wstring CRestoreConvertMIDlet::IsSuiteInstalled(TPtr& aSuiteName, TPtr& aVendorName)
{
    std::wstring suiteName(desToWstring(aSuiteName));
    std::wstring vendorName(desToWstring(aVendorName));
    std::wstring suiteUid;

    // Find application uid based on names from Java Storage
    JavaStorage *js = JavaStorage::createInstance();

    try
    {
        js->open(JAVA_DATABASE_NAME);

        JavaStorageEntry attribute;
        JavaStorageApplicationEntry_t findPattern;
        JavaStorageApplicationList_t  foundEntries;

        // Get ID from APPLICATION_PACKAGE_TABLE  based on PACKAGE_NAME and VENDOR
        attribute.setEntry(PACKAGE_NAME, suiteName);
        findPattern.insert(attribute);
        attribute.setEntry(VENDOR, vendorName);
        findPattern.insert(attribute);
        attribute.setEntry(ID, L"");
        findPattern.insert(attribute);

        js->search(APPLICATION_PACKAGE_TABLE , findPattern, foundEntries);

        // Anything found?
        if (foundEntries.size() > 0)
        {
            // The application package has been found, get the ID of the package
            suiteUid = foundEntries.front().begin()->entryValue();
            LOG1WSTR(EJavaConverters, EInfo,
                     "CRestoreConvertMIDlet::IsSuiteInstalled: Found suite by name. Uid is %s",
                     suiteUid.c_str());
        }
    }
    catch (JavaStorageException& e)
    {
        ELOG1(EJavaConverters,
              "CRestoreConvertMIDlet::IsSuiteInstalled: Java Storage exception %s", e.what());
    }

    try
    {
        js->close();
    }
    catch (JavaStorageException& e2)
    {
        WLOG1(EJavaConverters,
              "CRestoreConvertMIDlet::IsSuiteInstalled: Java Storage exception "
              "when closing storage %s", e2.what());
    }

    delete js;

    // suiteUid is empty if suite was not found
    return suiteUid;
}


/**
 * Finds the java attribute specified by aAttributeName
 * from aBuf and returns the value of that attribute
 * in HBufC.
 * @param[in] aBuf contents of .jad / Manifest file
 * @param[in] aAttributeName the name of a java attribute
 * @return the value of the attribute. Caller gets the ownership of the
 * returned HBufC.
 * If the attribute is not found, returns NULL
 */
HBufC *CRestoreConvertMIDlet::ParseAttribute(const HBufC *aBuf, const TDesC& aAttributeName)
{
    TInt    nInd(0);
    TBool   fullNameFound(EFalse);
    TUint32 ch;

    // Start parsing from the beginning
    TPtrC parsePtr = aBuf->Mid(nInd);

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
        // it was at the beginning
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

        // Check whether buffer ends after attribute name
        if (nInd + aAttributeName.Length() >= parsePtr.Length())
        {
            // Buffer ends immediately after the found
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
    // the Jad file / Manifest.
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
 * Uninstall all Java 2.x MIDlets from drive given in aDrive
 *
 * @param aDrive uninstallation drive
 */
void CRestoreConvertMIDlet::UninstallAllFromDriveL(TDriveNumber &aDrive)
{
    // Execute Java Installer with uninstallall -drive=X command line options

    LOG1(EJavaConverters, EInfo,
         "CRestoreConvertMIDlet::UninstallAllFromDriveL Going to uninstall "
         "MIDlets from drive number %d", aDrive);

    RProcess rJavaInstaller;
    TBuf<256> commandLine;

    // Build command line used to pass all necessary info to Java Installer
    commandLine = _L("javainstallerstarter");
    commandLine.Append(_L(" uninstallall -forceuninstall -captainmsgs=no"));

    // Run installer silently, do not send uninstall notification messages
    commandLine.Append(_L(" -silent -skipotastatus -drive="));

    // Add drive letter
    commandLine.Append((TChar)('A' + aDrive));

    // start JavaInstaller
#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
    TBuf<128> processName = _L("j9midps60");
#else // RD_JAVA_S60_RELEASE_5_0_IAD
    TBuf<128> processName = _L("javamidp");
#endif // RD_JAVA_S60_RELEASE_5_0_IAD

    TInt err = rJavaInstaller.Create(processName, commandLine);
    if (KErrNone == err)
    {
        LOG(EJavaConverters, EInfo, "CRestoreConvertMIDlet::UninstallAllFromDriveL calling Rendezvous");
        // This call will wait until Java Installer exits (or panics)
        TRequestStatus status;
        rJavaInstaller.Logon(status);

        LOG(EJavaConverters, EInfo, "CRestoreConvertMIDlet::UninstallAllFromDriveL calling Resume");
        rJavaInstaller.Resume();

        // now wait until Java Installer exits
        User::WaitForRequest(status);
        if (status.Int() != KErrNone)
        {
            ELOG1(EJavaConverters,
                  "CRestoreConvertMIDlet::UninstallAllFromDriveL Installer exited with error %d",
                  status.Int());
        }
    }
    else
    {
        ELOG1(EJavaConverters,
              "CRestoreConvertMIDlet::UninstallAllFromDriveL Cannot start Installer, error %d", err);
    }

    LOG(EJavaConverters, EInfo, "CRestoreConvertMIDlet::UninstallAllFromDriveL calling RProcess::Close");
    // free resources before returning
    rJavaInstaller.Close();
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
void CRestoreConvertMIDlet::AddJadJarToInstallFilesL(
    const TFileName &aSuitePathName,
    const CDir *aSuiteDirEntryList)
{
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
                     "CRestoreConvertMIDlet::AddJadJarToInstallFilesL Adding jad file %s",
                     (wchar_t *)(pathPtr.PtrZ()));
            iInstallFiles.Append(path);
            CleanupStack::Pop(path);
            iIsJad.AppendL(1);
        }
        else
        {
            // Reserve one char for null terminator
            HBufC* path = HBufC::NewLC(jarFullPathName.Length() + 1);
            TPtr pathPtr(path->Des());
            pathPtr.Append(jarFullPathName);

            LOG1WSTR(EJavaConverters, EInfo,
                     "CRestoreConvertMIDlet::AddJadJarToInstallFilesL Adding jar file %s",
                     (wchar_t *)(pathPtr.PtrZ()));
            iInstallFiles.Append(path);
            CleanupStack::Pop(path);
            iIsJad.AppendL(0);
        }
    }
}

/**
 * Start Java Installer in poll mode and then wait until it exits.
 */
void CRestoreConvertMIDlet::RunJavaInstallerL()
{
    LOG(EJavaConverters, EInfo, "CRestoreConvertMIDlet::RunJavaInstaller");

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
    commandLine.Append(_L(" -silent -skipotastatus -silentconversion"));

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

    // MIDlets must be restored to the original drive
    if (iDrive > -1)
    {
        TChar targetDrive;
        err = RFs::DriveToChar(iDrive, targetDrive);
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
        LOG(EJavaConverters, EInfo, "CRestoreConvertMIDlet::RunJavaInstaller calling Rendezvous");
        // This call will wait until Java Installer exits (or panics)
        rJavaInstaller.Logon(iStatus);

        LOG(EJavaConverters, EInfo, "CRestoreConvertMIDlet::RunJavaInstaller calling Resume");
        rJavaInstaller.Resume();
    }
    else
    {
        ELOG1(EJavaConverters,
              "CRestoreConvertMIDlet::RunJavaInstaller Cannot start Installer, error %d", err);
        // CActive will trap the following leave, execution will go to RunError
        User::Leave(err);
    }

    LOG(EJavaConverters, EInfo, "CRestoreConvertMIDlet::RunJavaInstaller calling RProcess::Close");
    // free resources before returning
    rJavaInstaller.Close();

    // now wait until Java Installer exits
    SetActive();
}

/**
 * To cleanup member variables.
 */
void CRestoreConvertMIDlet::FullCleanup()
{
    iDirs.ResetAndDestroy();
    iInstallFiles.ResetAndDestroy();
    iUninstallUids.clear();
    iIsJad.Reset();
}


/**
 * Checks all local drives in the device and stores the DriveInfo API drive
 * status information for each drive to iDriveStatuses
 * @exception Cannot get drive list.
 */
void CRestoreConvertMIDlet::GetAllDeviceDrivesL()
{
    TDriveList driveList;
    // get all drives
    TInt err = iFs.DriveList(driveList);
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters,
              "CRestoreConvertMIDlet::GetAllDeviceDrives cannot get drive list, err %d", err);
        User::Leave(err);
    }

    // store status of the non-remote, non-substed drives
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
                  "CRestoreConvertMIDlet::GetAllDeviceDrivesL cannot get drive %d status, err %d",
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
