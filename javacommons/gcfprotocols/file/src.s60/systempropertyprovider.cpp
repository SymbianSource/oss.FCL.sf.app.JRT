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
* Description:  Provides System properties such as Memory Card drive name.
 *                Paths to specific system properties based on the device.
 *
 *
*/


#include <f32file.h>
#include <driveinfo.h>
#include <pathinfo.h>

#include "logger.h"

#include "javasymbianoslayer.h"

#include "systempropertyprovider.h"
#include "com_nokia_mj_impl_file_FileConstants.h"

using namespace java::file;

_LIT(KSystemDir, "system");
_LIT(KPrivateDir, "private");
_LIT(KSeparator, "\\");
_LIT(KColon, ":");

void SystemPropertyProvider::GetMemoryCardPath(TFileName& aPath)
{
    TChar drive;
    DriveInfo::GetDefaultDrive(DriveInfo::EDefaultRemovableMassStorage, drive);
    aPath.Append(drive);
    aPath.Append(KColon);
    aPath.Append(KSeparator);
}

void SystemPropertyProvider::GetTemporaryDrive(TFileName& aPath)
{
    TChar drive;
    DriveInfo::GetDefaultDrive(DriveInfo::EDefaultRam, drive);
    aPath.Append(drive);
    aPath.Append(KColon);
    aPath.Append(KSeparator);
}

void SystemPropertyProvider::GetDefaultRoot(TFileName& aPath)
{
    TChar drive;
    DriveInfo::GetDefaultDrive(DriveInfo::EDefaultPhoneMemory, drive);
    aPath.Append(drive);
    aPath.Append(KColon);
    aPath.Append(KSeparator);
}

void SystemPropertyProvider::GetRomDrive(TFileName& aPath)
{
    TChar drive;
    DriveInfo::GetDefaultDrive(DriveInfo::EDefaultRom, drive);
    aPath.Append(drive);
    aPath.Append(KColon);
    aPath.Append(KSeparator);
}

void SystemPropertyProvider::GetPathOfProperty(TInt aProperty,
        TFileName& aPath, TBool aAddRoot)
{
    if ((aProperty
            != com_nokia_mj_impl_file_FileConstants_MEMORY_CARD_PROPERTY)
            && aAddRoot)
    {
        aPath = PathInfo::PhoneMemoryRootPath();
    }

    switch (aProperty)
    {
    case com_nokia_mj_impl_file_FileConstants_PHOTOS_PROPERTY:
    {
        aPath.Append(PathInfo::ImagesPath());
        return;
    }
    case com_nokia_mj_impl_file_FileConstants_VIDEOS_PROPERTY:
    {
        aPath.Append(PathInfo::VideosPath());
        return;
    }
    case com_nokia_mj_impl_file_FileConstants_GRAPHICS_PROPERTY:
    {
        aPath.Append(PathInfo::GmsPicturesPath());
        return;
    }
    case com_nokia_mj_impl_file_FileConstants_TONES_PROPERTY:
    {
        aPath.Append(PathInfo::SoundsPath());
        return;
    }
    case com_nokia_mj_impl_file_FileConstants_MUSIC_PROPERTY:
    {
        aPath.Append(PathInfo::SoundsPath());
        return;
    }
    case com_nokia_mj_impl_file_FileConstants_RECORDINGS_PROPERTY:
    {
        aPath.Append(PathInfo::DigitalSoundsPath());
        return;
    }
    case com_nokia_mj_impl_file_FileConstants_MEMORY_CARD_PROPERTY:
    {
        GetMemoryCardPath(aPath);
        return;
    }
    default:
    {
        aPath = NULL;
    }
    }
}

void SystemPropertyProvider::GetRestrictedPathsL(HBufC*& aRetBuf)
{
    JELOG2(EJavaFile);

    TChar separator = '*';
    TInt drive;

    aRetBuf = HBufC::New(KMaxFileName);
    TPtr16 pointer(aRetBuf->Des());
    RBuf16 buf(aRetBuf);

    // Create the default path structure for default phone memory
    DriveInfo::GetDefaultDrive(DriveInfo::EDefaultPhoneMemory, drive);

    CDesCArray* paths = 0;
    paths = PathInfo::GetListOfPathsLC(drive);

    TInt count(paths->MdcaCount());
    for (TInt i = 1; i < count; ++i)
    {
        TPtrC16 prt = paths->MdcaPoint(i);
        buf.Append(prt);
        buf.Append(separator);
    }
    buf.ZeroTerminate();
    CleanupStack::PopAndDestroy(paths);
}

void SystemPropertyProvider::GetForbiddenPaths(HBufC*& aRetBuf)
{
    // Note: Memory allocation can be improved. This can be done in future
    // release of S60 where we better string manipulation utils are expected.

    // System/private need to be added
    aRetBuf = HBufC::NewMax(KMaxFileName * 26);

    TDriveList driveList;
    TChar driveChar;
    TChar colon = ':';
    TChar separator = '*';
    TChar slash = '/';

    RFs session;
    int err = session.Connect();

    if (KErrNone == err)
    {
        err = session.DriveList(driveList);
        if (KErrNone == err)
        {
            (aRetBuf->Des()).Zero();
            for (TInt driveNumber = EDriveA; driveNumber <= EDriveZ; driveNumber++)
            {
                if (driveNumber == EDriveC)
                {
                    continue;
                }

                if (driveList[driveNumber])
                {
                    err = session.DriveToChar(driveNumber, driveChar);
                    if (KErrNone == err)
                    {
                        // append system dir
                        (aRetBuf->Des()).Append(driveChar);
                        (aRetBuf->Des()).Append(colon);
                        (aRetBuf->Des()).Append(slash);
                        (aRetBuf->Des()).Append(KSystemDir);
                        (aRetBuf->Des()).Append(separator);

                        // append private dir
                        (aRetBuf->Des()).Append(driveChar);
                        (aRetBuf->Des()).Append(colon);
                        (aRetBuf->Des()).Append(slash);
                        (aRetBuf->Des()).Append(KPrivateDir);
                        (aRetBuf->Des()).Append(separator);
                    }
                }
            }
        }
    }
    session.Close();
}
