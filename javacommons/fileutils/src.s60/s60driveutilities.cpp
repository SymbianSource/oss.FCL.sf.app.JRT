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
* Description:  DriveUtilities - S60 specific
*
*/

#include <f32file.h>
#include <driveinfo.h>

#include "logger.h"

#include "driveutilities.h"

namespace java
{
namespace fileutils
{
OS_EXPORT void DriveUtilities::getAllDrives(driveInfos& aDriveInfos)
{
    JELOG2(EJavaFile);

    int rc = KErrNone;

    RFs fs;
    if (KErrNone == (rc = fs.Connect()))
    {
        TDriveList  driveList;
        if (KErrNone == (rc = fs.DriveList(driveList)))
        {
            TInt driveCount = KMaxDrives; // driveArray.Count();
            for (TInt index = 0 ; index < driveCount ; index++)
            {
                if (0 != driveList[index])
                {
                    driveInfo di;
                    TChar ret(0);
                    RFs::DriveToChar(index, ret);
                    di.iRootPath    = (wchar_t) ret;
                    di.iRootPath   += L":\\";

                    TUint status = 0;
                    if (KErrNone == (rc = DriveInfo::GetDriveStatus(fs, index, status)))
                    {
                        di.iIsPresent   =  status & DriveInfo::EDrivePresent;
                        di.iIsRemovable =  status & DriveInfo::EDriveRemovable;
                        di.iIsLocal     =!(status & DriveInfo::EDriveRemote);
                        di.iIsReadOnly  =  status & DriveInfo::EDriveReadOnly;
                        di.iIsExternallyMountable  =  status & DriveInfo::EDriveExternallyMountable;
                        TVolumeInfo volumeInfo;
                        if (KErrNone == (rc = fs.Volume(volumeInfo, index)))
                        {
                            di.iId = volumeInfo.iUniqueID;
                        }
                        else
                        {
                            di.iId = 0;
                        }
                    }

                    aDriveInfos.push_back(di);
                }
            }
        }
        fs.Close();
    }

    if (KErrNone != rc)
    {
        ELOG1(EJavaFile, "getAllDrives failed due to SymbianOS error %d", rc);
    }
}
} // end of namespace fileutils
} // end of namespace java
