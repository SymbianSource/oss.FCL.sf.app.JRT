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

#ifndef DRIVECHANGEDEVENTGENERATOR_H
#define DRIVECHANGEDEVENTGENERATOR_H

#include <list>

#include <e32def.h>
#include <e32base.h>
#include <e32cmn.h>
#include <f32file.h>

#include "javaosheaders.h"

#include "driveutilities.h"

class RFs;

namespace java
{
namespace fileutils
{

OS_NONSHARABLE_CLASS(DriveChangedEventGenerator) : public DriveListenerInterface
{
public:
    DriveChangedEventGenerator(DriveListenerInterface* aEventDispatcher);
    virtual ~DriveChangedEventGenerator();

    virtual void startEvents();
    virtual void stopEvents();

    // DriveListenerInterface
    virtual void driveChanged(const int& aOperation, const driveInfo& aDriveInfo);

private:

    OS_NONSHARABLE_CLASS(DriveChangedActive) : public CActive
    {
public:
        DriveChangedActive(driveInfo aDriveInfo,
        DriveListenerInterface* aEventDispatcher,
        RFs* aFsSession);
        virtual ~DriveChangedActive();

private:
        virtual void RunL();
        virtual void DoCancel();

        TInt                    iDrive;
        driveInfo               iDriveInfo;
        TPath                   iNotificationPath;

        DriveListenerInterface* iEventDispatcher;
        RFs*                    iFs;
    };

    RFs                             iFs;
    DriveListenerInterface*         iEventDispatcher;
    std::list<DriveChangedActive*>  iDriveObjects;
};

} // namespace fileutils
} // namespace java

#endif // DRIVECHANGEDEVENTGENERATOR_H
