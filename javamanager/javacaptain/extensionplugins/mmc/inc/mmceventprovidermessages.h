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
* Description:  MmcEventProviderMessages
*
*/

#ifndef MMCEVENTPROVIDERMESSAGES_H
#define MMCEVENTPROVIDERMESSAGES_H

#include "logger.h"
#include "commsmessage.h"
#include "driveutilities.h"

namespace java
{
namespace captain
{

using namespace java::util;
using namespace java::comms;
using namespace java::fileutils;

const int MMC_EVENT_MESSAGE_ID_CHANGED_C = 20;

const char MMC_EVENT_PROVIDER[] = "mmc";

inline void getMmcChangedMessageParams(CommsMessage& aMessage,
                                       int& aOperation, driveInfo& aDriveInfo)
{
    if (aMessage.getMessageId() == MMC_EVENT_MESSAGE_ID_CHANGED_C)
    {
        aMessage >> aOperation;
        aMessage >> aDriveInfo.iRootPath;

        int tempInt = 0;
        aMessage >> tempInt;
        aDriveInfo.iIsPresent = (tempInt == 1 ? true : false);

        aMessage >> tempInt;
        aDriveInfo.iIsRemovable = (tempInt == 1 ? true : false);

        aMessage >> tempInt;
        aDriveInfo.iIsLocal = (tempInt == 1 ? true : false);

        aMessage >> tempInt;
        aDriveInfo.iId = tempInt;
    }
    else
    {
        ELOG2(EJavaCaptain, "wrong MessageId!: %d should be %d",
              aMessage.getMessageId(), MMC_EVENT_MESSAGE_ID_CHANGED_C);
    }
}

inline void setMmcChangedMessageParams(CommsMessage& aMessage,
                                       const int& aOperation, const driveInfo& aDriveInfo)
{
    aMessage.setMessageId(MMC_EVENT_MESSAGE_ID_CHANGED_C);
    aMessage << aOperation;
    aMessage << aDriveInfo.iRootPath;
    aMessage << (aDriveInfo.iIsPresent ? 1 : 0);
    aMessage << (aDriveInfo.iIsRemovable ? 1 : 0);
    aMessage << (aDriveInfo.iIsLocal ? 1 : 0);
    aMessage << (int) aDriveInfo.iId;
}



} // namespace captain
} // namespace java

#endif // MMCEVENTPROVIDERMESSAGES_H
