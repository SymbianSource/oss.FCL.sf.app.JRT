/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  PreinstallerStarterMessages
*
*/

#ifndef PREINSTALLERSTARTERMESSAGES_H
#define PREINSTALLERSTARTERMESSAGES_H

#include "logger.h"
#include "commsmessage.h"

namespace java
{
namespace captain
{

using namespace java::util;
using namespace java::comms;

// This event is sent when preinstaller process has exited
const int PREINSTALLER_EVENT_MESSAGE_ID_EXITED_C = 30;

const char PREINSTALLER_EVENT_PROVIDER[] = "preinstaller";


inline void setPreinstallerExitedMessageParams(CommsMessage& aMessage, const int& aExitStatus)
{
    aMessage.setMessageId(PREINSTALLER_EVENT_MESSAGE_ID_EXITED_C);
    aMessage << aExitStatus;
}

inline void getPreinstallerExitedMessageParams(CommsMessage& aMessage, int& aExitStatus)
{
    if (aMessage.getMessageId() == PREINSTALLER_EVENT_MESSAGE_ID_EXITED_C)
    {
        aMessage >> aExitStatus;
    }
    else
    {
        ELOG2(EJavaCaptain, "wrong MessageId!: %d should be %d",
              aMessage.getMessageId(), PREINSTALLER_EVENT_MESSAGE_ID_EXITED_C);
        aExitStatus = 0;
    }
}


} // namespace captain
} // namespace java

#endif // PREINSTALLERSTARTERMESSAGES_H
