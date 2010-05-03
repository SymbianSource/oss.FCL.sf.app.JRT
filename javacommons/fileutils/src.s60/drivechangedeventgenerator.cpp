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

    if (KErrNone == iFs.Connect())
    {
        for (driveInfos::const_iterator iter = drives.begin() ;
                iter != drives.end() ; ++iter)
        {
            if (iter->iIsRemovable)
            {
                iDriveObjects.push_back(new DriveChangedActive(*iter, this, &iFs));
            }
        }
    }
}
void DriveChangedEventGenerator::stopEvents()
{
    JELOG2(EJavaFile);
    iFs.Close();
}


// DriveListenerInterface
void DriveChangedEventGenerator::driveChanged(const int& aOperation, const driveInfo& aDriveInfo)
{
    JELOG2(EJavaFile);

    iEventDispatcher->driveChanged(aOperation, aDriveInfo);
}
// change notification will be generated for this directory
_LIT(KNotificationDirectory,"mediachange\\");

DriveChangedEventGenerator::DriveChangedActive::DriveChangedActive(driveInfo aDriveInfo,
        DriveListenerInterface* aEventDispatcher, RFs* aFsSession)
        :CActive(EPriorityNormal),
        iDrive(0),
        iDriveInfo(aDriveInfo),
        iEventDispatcher(aEventDispatcher),
        iFs(aFsSession)
{
    JELOG2(EJavaFile);

    CActiveScheduler::Add(this);

    TChar driveChar = (TChar) iDriveInfo.iRootPath[0];
    iFs->CharToDrive(driveChar, iDrive);

    TPath privatePath;
    iFs->PrivatePath(privatePath);

    // obtain notification path
    iNotificationPath.Append(driveChar);
    iNotificationPath.Append(KDriveDelimiter);
    iNotificationPath.Append(privatePath);
    iNotificationPath.Append(KNotificationDirectory);

    iFs->NotifyChange(ENotifyEntry, iStatus, iNotificationPath);
    SetActive();
}

DriveChangedEventGenerator::DriveChangedActive::~DriveChangedActive()
{
    JELOG2(EJavaFile);

    Cancel();
}

void DriveChangedEventGenerator::DriveChangedActive::RunL()
{
    JELOG2(EJavaFile);

    TDriveInfo driveInfo;
    TInt rc = iFs->Drive(driveInfo, iDrive);
    if (KErrNone == rc)
    {
        LOG(EJavaFile, EInfo, "iFs->Drive() returns KErrNone");
        LOG1(EJavaFile, EInfo, "driveInfo.iType = %d", driveInfo.iType);
    }

    TVolumeInfo volumeInfo;
    int operation = 0;
    rc = iFs->Volume(volumeInfo, iDrive);
    if (KErrNone == rc)
    {
        LOG(EJavaFile, EInfo, "iFs->Volume() returns KErrNone");
        operation = DriveListenerInterface::REMOVABLE_MEDIA_INSERTED_C;
        iDriveInfo.iIsPresent = true;
        iDriveInfo.iId = volumeInfo.iUniqueID;
    }
    else
    {
        LOG1(EJavaFile, EInfo, "iFs->Volume() returns %d", rc);
        operation = DriveListenerInterface::REMOVABLE_MEDIA_REMOVED_C;
        iDriveInfo.iIsPresent = false;
    }

    iEventDispatcher->driveChanged(operation, iDriveInfo);

    iFs->NotifyChange(ENotifyEntry, iStatus, iNotificationPath);
    SetActive();
}

void DriveChangedEventGenerator::DriveChangedActive::DoCancel()
{
    JELOG2(EJavaFile);

    iFs->NotifyChangeCancel();
}
} // namespace fileutils
} // namespace java
