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
* Description:  signalhandlermessages
*
*/

#ifndef SIGNALHANDLERMESSAGES_H
#define SIGNALHANDLERMESSAGES_H

#include "logger.h"
#include "comms.h"
#include "commsmessage.h"

using namespace java::comms;

namespace java
{
namespace captain
{
// Message IDs of all installer extension services
const int SIG_MSG_ID_SIGCHLD    = JAVACAPTAIN_MESSAGE_ID_RANGE_START_C + 30;    // 130
const int SIG_MSG_ID_SIGALRM    = SIG_MSG_ID_SIGCHLD + 1;                       // 131
const int SIG_MSG_ID_SIGINT     = SIG_MSG_ID_SIGALRM + 1;                       // 132

inline void setSignalChildMessage(CommsMessage& aMessage, const int& aPid, const int& aStatus)
{
    aMessage.setModuleId(PLUGIN_ID_SIGNAL_C);
    aMessage.setMessageId(SIG_MSG_ID_SIGCHLD);
    aMessage << aPid << aStatus;
}
inline void getSignalChildMessage(CommsMessage& aMessage, int& aPid, int& aStatus)
{
    if (aMessage.getMessageId() == SIG_MSG_ID_SIGCHLD)
    {
        aMessage >> aPid >> aStatus;
    }
    else
    {
        ELOG2(EJavaCaptain, "wrong MessageId!: %d should be %d",
              aMessage.getMessageId(), SIG_MSG_ID_SIGCHLD);
    }
}

inline void setSignalAlarmMessage(CommsMessage& aMessage)
{
    aMessage.setModuleId(PLUGIN_ID_SIGNAL_C);
    aMessage.setMessageId(SIG_MSG_ID_SIGALRM);
}
inline void getSignalAlarmMessage(CommsMessage& aMessage)
{
    if (aMessage.getMessageId() != SIG_MSG_ID_SIGALRM)
    {
        ELOG2(EJavaCaptain, "wrong MessageId!: %d should be %d",
              aMessage.getMessageId(), SIG_MSG_ID_SIGCHLD);
    }
}

inline void setSignalInterruptMessage(CommsMessage& aMessage)
{
    aMessage.setModuleId(PLUGIN_ID_SIGNAL_C);
    aMessage.setMessageId(SIG_MSG_ID_SIGINT);
}
inline void getSignalInterruptMessage(CommsMessage& aMessage)
{
    if (aMessage.getMessageId() != SIG_MSG_ID_SIGINT)
    {
        ELOG2(EJavaCaptain, "wrong MessageId!: %d should be %d",
              aMessage.getMessageId(), SIG_MSG_ID_SIGINT);
    }
}

} // namespace captain
} // namespace java

#endif // SIGNALHANDLERMESSAGES_H

