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


#ifndef SILENTMIDLETCONVERT_H
#define SILENTMIDLETCONVERT_H

#include <f32file.h>
#include <e32base.h>
#include <javasymbianoslayer.h>

#include "convertserver.h"


// This binary data file contains the uids of the old valid java applications
// in this device
_LIT(KUidsImportDataFilePathName, "C:\\private\\2002121C\\data\\");
_LIT(KUidsImportDataFileName, "uids");

// Default target installation disk for conversion should have at least
// 10 MB of free disk space (10 * 1024 * 1204)
const TInt64 KMinimumFreeDiskSpace = 10485760;


/**
 * The main active object of Java App Converter that is executed once
 * to convert old S60 MIDlets to OMJ Java applications
 *
 * This active object is activated from the process main() after the
 * active scheduler has been created.
 */
class CSilentMIDletConvert : public CActive
{
    enum TConversionState
    {
        EFindOutDeviceDrives,
        EFindOutDrivesToBeScannedNow,
        EAppsInInstalledDirectories,
        ECheckWhichAppsShouldBeConverted,
        EExecuteConvertServer,
        EMarkConversionDone,
        EExit
    };

public:
    static CSilentMIDletConvert* NewLC(RFs& aFs);

    void CompleteRequest();
    ~CSilentMIDletConvert();
    void Start();

protected:
    // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    void Exit();
    CSilentMIDletConvert(RFs& aFs);
    void ConstructL();
    void GetInstallFilesL(RPointerArray<HBufC>& aDirs);
    void CheckNeedToInstall();
    RArray<TUid> ReadMidletUids(const TPtrC& aDir);
    void RunJavaInstallerL();
    void RemoveDataFile();
    void FullCleanup();
    void AddJadJarToInstallFilesL(
        const TFileName &aSuitePathName,
        const CDir *aSuiteDirEntryList);
    void GetAllDeviceDrivesL();
    TBool HasBeenInstalled(const TDesC& aFilePath);


private:
    RFs& iFs;
    RPointerArray<HBufC> iDirs;
    RPointerArray<HBufC> iInstallFiles;
    TInt iFileIndex;
    TConversionState iState;
    ConvertServer* iConvertServer;

    // contains the drive status info for each non-remote, non-substed drive
    // on device
    TUint iDriveStatuses[KMaxDrives];
    TInt iNumberOfAppsToInstall;

    // If the device has an internal drive with much free disk space,
    // the converted java applications will be installed to that drive.
    // This member variable contains the number of that drive, e.g. EDriveE
    // or -1 if the device does not have such drive
    TInt iDefaultTargetInstallationDrive;
};

#endif // SILENTMIDLETCONVERT_H
