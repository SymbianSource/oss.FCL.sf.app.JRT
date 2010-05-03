/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DriveChangedEventGenerator - S60 specific
*
*/


#include "logger.h"

#include "drivechangedeventgenerator.h"
#include "driveutilities.h"

namespace java
{
namespace fileutils
{

DriveChangedEventGenerator::DriveChangedEventGenerator(DriveListenerInterface* aEventDispatcher)
        :iEventDispatcher(aEventDispatcher)
{
    JELOG2(EJavaFile);
}

DriveChangedEventGenerator::~DriveChangedEventGenerator()
{
    JELOG2(EJavaFile);
}

void DriveChangedEventGenerator::startEvents()
{
    JELOG2(EJavaFile);

    driveInfos drives;
    DriveUtilities::getAllDrives(drives);

    for (driveInfos::const_iterator iter = drives.begin() ;
            iter != drives.end() ; ++iter)
    {
        if (iter->iIsRemovable)
        {
            // Add implementation
        }
    }
}
void DriveChangedEventGenerator::stopEvents()
{
    JELOG2(EJavaFile);
}


// DriveListenerInterface
void DriveChangedEventGenerator::driveChanged(const int& aOperation, const driveInfo& aDriveInfo)
{
    JELOG2(EJavaFile);
    iEventDispatcher->driveChanged(aOperation, aDriveInfo);
}

} // namespace fileutils
} // namespace java
