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
* Description:  DriveChangedEventGenerator - Linux specific
*
*/

#ifndef DRIVECHANGEDEVENTGENERATOR_H
#define DRIVECHANGEDEVENTGENERATOR_H

#include "driveutilities.h"

namespace java
{
namespace fileutils
{

class DriveChangedEventGenerator : public DriveListenerInterface
{
public:
    DriveChangedEventGenerator(DriveListenerInterface* aEventDispatcher);
    virtual ~DriveChangedEventGenerator();

    virtual void startEvents();
    virtual void stopEvents();

    // DriveListenerInterface
    virtual void driveChanged(const int& aOperation, const driveInfo& aDriveInfo);

private:
    DriveListenerInterface*         iEventDispatcher;
};

} // namespace fileutils
} // namespace java

#endif // DRIVECHANGEDEVENTGENERATOR_H
