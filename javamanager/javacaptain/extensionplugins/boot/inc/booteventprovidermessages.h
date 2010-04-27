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
* Description:  BootEventProviderMessages
*
*/

#ifndef BOOTEVENTPROVIDERMESSAGES_H
#define BOOTEVENTPROVIDERMESSAGES_H

#include "logger.h"
#include "commsmessage.h"

namespace java
{
namespace captain
{

using namespace java::util;
using namespace java::comms;

const int BOOT_EVENT_MESSAGE_ID_BOOT_C = 1;

const char BOOT_EVENT_PROVIDER[] = "boot";

typedef enum
{
    NORMAL_BOOT_C           = 0x00,
    FIRST_DEVICE_BOOT_C     = 0x01, // device is started first time after flashing
    IAD_BOOT_C              = 0x02  // device is started after IAD installation
} bootEventType_t;

inline void setBootMessageParams(CommsMessage& aMessage, const int& aBootTypeMask)
{
    aMessage.setMessageId(BOOT_EVENT_MESSAGE_ID_BOOT_C);
    aMessage << aBootTypeMask;
}

inline void getBootMessageParams(CommsMessage& aMessage, int& aBootTypeMask)
{
    if (aMessage.getMessageId() == BOOT_EVENT_MESSAGE_ID_BOOT_C)
    {
        aMessage >> aBootTypeMask;
    }
    else
    {
        ELOG2(EJavaCaptain, "wrong MessageId!: %d should be %d",
              aMessage.getMessageId(), BOOT_EVENT_MESSAGE_ID_BOOT_C);
        aBootTypeMask = 0;
    }
}



} // namespace captain
} // namespace java

#endif // BOOTEVENTPROVIDERMESSAGES_H
