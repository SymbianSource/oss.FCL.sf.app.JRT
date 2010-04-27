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


#ifndef RESTORECONVERTMIDLET_H
#define RESTORECONVERTMIDLET_H

#include <f32file.h>
#include <e32base.h>
#include <javasymbianoslayer.h>

#include "restoreserver.h"

/**
 * The main active object of Java Restore Converter that is executed once
 * to when old S60 MIDlets have been restored to device and they must be
 * conversion installed to OMJ Java applications
 *
 * This active object is activated from the process main() after the
 * active scheduler has been created.
 */
class CRestoreConvertMIDlet : public CActive
{
    enum TRestorationState
    {
        EFindOutDeviceDrives,
        EGetDriveToBeScanned,
        EAppsInInstalledDirectories,
        EUninstallFromOMJ,
        EExecuteRestoreServer,
        EExit
    };

public:
    static CRestoreConvertMIDlet* NewLC(RFs& aFs);

    void CompleteRequest();
    ~CRestoreConvertMIDlet();
    void Start();

protected:
    // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    void Exit();
    CRestoreConvertMIDlet(RFs& aFs);
    void ConstructL();
    void GetInstallFilesL(RPointerArray<HBufC>& aDirs);
    void FindRemainingMIDletsToBeUninstalledL();
    void UninstallAllFromDriveL(TDriveNumber &aDrive);
    void RunJavaInstallerL();
    void RemoveDataFile();
    void FullCleanup();
    void AddJadJarToInstallFilesL(
        const TFileName &aSuitePathName,
        const CDir *aSuiteDirEntryList);
    void GetAllDeviceDrivesL();

    // Parse MIDlet-Name and MIDlet-Vendor parameters from .jad file.
    TBool ParseJadL(const TDesC& aJadFileName);
    // Parse MIDlet-Name and MIDlet-Vendor parameters from Manifest.
    TBool ParseJarL(const TDesC& aJarFileName);
    // Return the uid of the suite if it has been installed,
    // wstring is empty if suite was not found
    std::wstring IsSuiteInstalled(TPtr& aSuiteName, TPtr& aVendorName);
    // Reads the whole content of the Jad file and returns it in
    // buffer in UCS-2 character set.
    HBufC *GetJadContentL(const TDesC& aJadFileName);
    // Reads the whole content of the Manifest inside the Jar file
    // and returns it in buffer in UCS-2 character set.
    HBufC *GetManifestContentL(const TDesC& aJarFileName);
    // Finds the java attribute specified by aAttributeName
    // from aBuf and returns the value of that attribute
    // in HBufC.
    HBufC *ParseAttribute(const HBufC *aBuf, const TDesC& aAttributeName);

private:
    RFs& iFs;
    TDriveNumber iDrive;
    RArray<TInt> iIsJad;
    RPointerArray<HBufC> iDirs;
    RPointerArray<HBufC> iInstallFiles;
    std::vector<std::wstring> iUninstallUids;
    // Buffer used for parsing midlet name from jad / Manifest
    HBufC* iMIDletName;
    // Buffer used for parsing midlet vendor from jad / Manifest
    HBufC* iMIDletVendor;
    TInt iFileIndex;
    TRestorationState iState;
    RestoreServer* iConvertServer;

    // contains the drive status info for each non-remote, non-substed drive
    // on device
    TUint iDriveStatuses[KMaxDrives];
    TInt iNumberOfAppsToInstall;
};

#endif // RESTORECONVERTMIDLET_H
