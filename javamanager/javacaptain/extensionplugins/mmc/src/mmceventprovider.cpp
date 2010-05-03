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
* Description:  MmcEventProvider
*
*/

#include "logger.h"

#include "coreinterface.h"
//#include "eventproviderinterface.h"
#include "eventconsumerinterface.h"

#include "mmceventprovider.h"
#include "mmceventprovidermessages.h"

namespace java
{
namespace captain
{

MmcEventProvider::MmcEventProvider()
        :mCore(0), mDOServer(0)
{
    JELOG2(EJavaCaptain);
}

MmcEventProvider::~MmcEventProvider()
{
    JELOG2(EJavaCaptain);
}

void MmcEventProvider::startPlugin(CoreInterface* aCore)
{
    JELOG2(EJavaCaptain);
    mCore = aCore;

    mDOServer = DriveUtilities::getDriveObserverServer();
    if (mDOServer)
    {
        mDOServer->registerListener(this);
        mDOServer->startServer(mCore->getComms());
    }
}

void MmcEventProvider::stopPlugin()
{
    JELOG2(EJavaCaptain);
    if (mDOServer)
    {
        mDOServer->stopServer();
        delete mDOServer;
        mDOServer = 0;
    }
    mCore = 0;
}

void MmcEventProvider::driveChanged(const int& aOperation, const driveInfo& aDriveInfo)
{
    JELOG2(EJavaCaptain);
    switch (aOperation)
    {
    case DriveListenerInterface::REMOVABLE_MEDIA_INSERTED_C:
        LOG1(EJavaCaptain, EInfo, "driveChanged() operation=REMOVABLE_MEDIA_INSERTED_C drive.id==%u", aDriveInfo.iId);
        break;

    case DriveListenerInterface::REMOVABLE_MEDIA_REMOVED_C:
        LOG1(EJavaCaptain, EInfo, "driveChanged() operation=REMOVABLE_MEDIA_REMOVED_C drive.id==%u", aDriveInfo.iId);
        break;

    default:
        LOG1(EJavaCaptain, EInfo, "driveChanged() operation=UNKNOWN drive.id==%u", aDriveInfo.iId);
        break;
    }

    CommsMessage msg;
    setMmcChangedMessageParams(msg, aOperation, aDriveInfo);
    mCore->getEventDispatcher()->event(MMC_EVENT_PROVIDER, msg);
}

} // namespace captain
} // namespace java

