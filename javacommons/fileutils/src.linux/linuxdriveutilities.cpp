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
* Description:  DriveUtilities - Linux specific
*
*/


#include "logger.h"

#include "driveutilities.h"

namespace java
{
namespace fileutils
{
OS_EXPORT void DriveUtilities::getAllDrives(driveInfos& aDriveInfos)
{
    JELOG2(EJavaFile);

    driveInfo di;
    di.iRootPath              = L"/tmp/";
    di.iIsPresent             = true;
    di.iIsRemovable           = false;
    di.iIsLocal               = true;
    di.iIsReadOnly            = false;
    di.iId                    = 0x12345;
    di.iIsExternallyMountable = false;

    aDriveInfos.push_back(di);
}
} // end of namespace fileutils
} // end of namespace java
