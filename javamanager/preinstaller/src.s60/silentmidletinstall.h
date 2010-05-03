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
* Description:  OMJ S60 preinstaller process
 *
*/


#ifndef SILENTMIDLETINSTALL_H
#define SILENTMIDLETINSTALL_H

#include <f32file.h>
#include <e32base.h>
#include <javasymbianoslayer.h>

#include "preinstallcommsserver.h"

/**
 * The main active object of java preinstaller.
 *
 * This active object is activated from the process main() after the
 * active scheduler has been created.
 */
OS_NONSHARABLE_CLASS(CSilentMIDletInstall) : public CActive
{
    // This active object goes through all these states in order and
    // exits from EExit state
    enum TSilentInstallState
    {
        EFindOutDeviceDrives,
        EFindOutDirectoriesToBeScannedNow,
        EAppsInPreinstallDirectories,
        ECheckWhichAppsShouldBeInstalled,
        EExecutePreinstallServer,
        EExit
    };

public:
    // the only allowed way to create this object
    static CSilentMIDletInstall* NewLC(RFs& aFs);

    ~CSilentMIDletInstall();

    // Start this AO after it has been constructed
    void Start();
    // Move AO to the next state
    void CompleteRequest();

protected:
    // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    // Exit whole AO
    void Exit();
    // Private constructor. Adds this AO to active scheduler
    CSilentMIDletInstall(RFs& aFs);
    void ConstructL();
    // Add .jad files in the directories specified in aDirs to iJadFiles.
    void GetMIDletFilesL(RPointerArray<HBufC>& aDirs);
    // Start Java Installer in poll mode and then wait until it exits.
    void RunJavaInstallerL();
    // Start Java Installer just to do IntegrityService rollback.
    void RollbackJavaInstaller();
    // Get the entries of the directory in alphabetical order.
    void GetDirEntriesL(const TDesC& aDirectory, const TDesC& aMask,
                        RPointerArray<HBufC>& aVector);
    // Parse MIDlet-Name, MIDlet-Vendor and MIDlet-Version parameters from JAD file.
    TBool ParseJadL(const TDesC& aJadFileName);
    // Checks all local drives in the device and stores the DriveInfo API drive
    // status information for each drive to iDriveStatuses
    void GetAllDeviceDrivesL();
    // Adds the preinstall directory of every local, non-substed drive to iDirs
    void GetDirsToBeScannedL();
    // Check all Jad files in iJadFiles and remove those
    // that need not be preinstalled (already installed[1] or preinstalled[2]
    // or preinstalled and then uninstalled by user[3]).
    void CheckWhichAppsShouldBeInstalledL();
    // If there is something to preinstall start preinstall comms server and
    // installer in poll mode.
    void ExecutePreinstallServerL();
    // Reads the whole content of the Jad file and returns it in
    //  buffer in UCS-2 character set.
    HBufC *GetJadContentL(const TDesC& aJadFileName);
    // Finds the java attribute specified by
    // aAttributeName from aJad and returns the value of that attribute
    // in HBufC.
    HBufC *ParseAttribute(const HBufC *aJad, const TDesC& aAttributeName);
    // Parses the application version string given in aAppVersionString
    //  and returns the corresponding Symbian TAppVersion.
    TAppVersion DesToAppVersion(const HBufC *aAppVersionString);


private:
    // File server handle passed to this AO when it was created.
    // This object does not own it.
    RFs& iFs;
    // The preinstall directories to be scanned
    RPointerArray<HBufC> iDirs;
    // The jad files to be preinstalled
    RPointerArray<HBufC> iJadFiles;
    // Current state of the state machine
    TSilentInstallState iState;
    // True if Java Installer Integrity Server rollback must be done
    TBool iISRollbackNeeded;
    // Comms server used to pass the jad file names to Java Installer
    PreinstallCommsServer* iPreinstallServer;
    // Buffer used for parsing midlet name from jad file
    HBufC* iMIDletName;
    // Buffer used for parsing midlet vendor from jad file
    HBufC* iMIDletVendor;
    // Variable used for parsing midlet version from jad file
    TAppVersion iMIDletVersion;

    // Drive status info for each non-remote, non-substed drive on device
    TUint iDriveStatuses[KMaxDrives];
    // How many applications must be preinstalled
    TInt iNumberOfAppsToInstall;
};

#endif // SILENTMIDLETINSTALL_H
