/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of JavaVersionBackupUtil
*
*/


#include <javaenvinfo.h>
#include "javaversionbackuputil.h"
#include "logger.h"


#include <s32mem.h>
#include <e32base.h>

#define KOMJIdentifier 0x20000F3D

using namespace Java;
using namespace java::backup;

// ======== MEMBER FUNCTIONS ========

void JavaVersionBackupUtil::WriteJavaVersionL(RDesWriteStream& aStream)
{
    JELOG2(EBackup);

    // Get the Java version number from the environment API's

    TVersion version = Java::JavaEnvInfo::Version();

    PLOG3(EBackup, "JavaEnvInfo::Version: major = %d, minor = %d, build = %d",
          version.iMajor, version.iMinor, version.iBuild);

    /* Put OMJ Package UID in the front indicating that it is an
       OMJ environment from which backup was taken.  */
    aStream.WriteInt32L(KOMJIdentifier);

    // write the version numbers to the stream
    aStream.WriteInt8L(version.iMajor);
    aStream.WriteInt8L(version.iMinor);
    aStream.WriteInt16L(version.iBuild);

    ILOG(EBackup, "Version information written to stream");
}

TUint JavaVersionBackupUtil::CheckJavaVersionL(RDesReadStream& aStream, TDriveNumber aDrive, TBool versionCheck)
{
    JELOG2(EBackup);
    TInt32 omjID = 0;
    if (versionCheck)
    {
        // Read the OMJSpecific ID only on first call
        omjID = aStream.ReadInt32L();
    }

    if (omjID != KOMJIdentifier)
    {
        _LIT(KJavaRestoreConverterExe, "javarestoreconverter.exe");

        RProcess rProcess;

        TChar targetDrive;
        _LIT(KText , "");
        TBuf<15> commandLine(KText);
        RFs session;
        TInt err = session.Connect();
        if (KErrNone != err)
        {
            ELOG1(EBackup,
                  "JavaVersionBackupUtil::CheckJavaVersionL: Session Connection failed, error %d", err);
            CleanupStack::PopAndDestroy(&aStream);
            User::Leave(err);
        }

        err = session.DriveToChar(aDrive, targetDrive);
        if (KErrNone == err)
        {
            _LIT(KDrive, " -drive");
            commandLine.Append(KDrive);
            commandLine.Append(targetDrive);
        }

        err = rProcess.Create(KJavaRestoreConverterExe, commandLine);
        if (KErrNone == err)
        {
            TRequestStatus status;
            rProcess.Logon(status);
            rProcess.Resume();

            // now wait until javarestoreconverter exits
            User::WaitForRequest(status);
            if (status.Int() != KErrNone)
            {
                // Installing old JRT 1.x MIDlets to JRT 2.x failed
                err = status.Int();
                ELOG1(EBackup,
                      "JavaVersionBackupUtil::CheckJavaVersionL: Installation failed, error %d", err);
                CleanupStack::PopAndDestroy(&aStream);
                User::Leave(err);
            }
            else
            {
                LOG(EBackup,EInfo,
                    "JavaVersionBackupUtil::CheckJavaVersionL: Installing Midlets successful");
            }
        }
        else
        {
            // Cannot even start javarestoreconverter
            ELOG1(EBackup,
                  "JavaVersionBackupUtil::CheckJavaVersionL: Cannot start Installer, error %d", err);
            CleanupStack::PopAndDestroy(&aStream);
            User::Leave(err);
        }
        rProcess.Close();
        return 1;

    }
    else
    {
        TInt8 major = aStream.ReadInt8L();
        TInt8 minor = aStream.ReadInt8L();
        TInt16 build = aStream.ReadInt16L();

        TVersion version(major, minor, build);

        PLOG3(EBackup, "JavaVersion read from Stream: major = %d, minor = %d, build = %d",
              version.iMajor, version.iMinor, version.iBuild);

        // read current version and compare it with the read version
        TVersion curVersion = Java::JavaEnvInfo::Version();

        PLOG3(EBackup," Current JavaVersion: major = %d, minor = %d, build = %d",
              curVersion.iMajor, curVersion.iMinor, curVersion.iBuild);
        return 0;

    }
}

