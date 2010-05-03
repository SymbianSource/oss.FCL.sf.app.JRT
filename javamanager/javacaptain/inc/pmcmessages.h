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
* Description:  pmcMessages
*
*/

#ifndef PMCMESSAGES_H
#define PMCMESSAGES_H

#include "logger.h"
#include "comms.h"
#include "commsmessage.h"

using namespace java::comms;

namespace java
{
namespace captain
{
const int PMC_MSG_ID_PROCESS_TERMINATED_IND         = JAVACAPTAIN_MESSAGE_ID_RANGE_START_C + 20; // 120
const int PMC_MSG_ID_PROCESS_INITIALISED_IND        = JAVACAPTAIN_MESSAGE_ID_RANGE_START_C + 21; // 121

// Process terminated indication
inline void setProcessTerminatedIndParams(CommsMessage& aMessage,
        const int& aPid)
{
    aMessage.setMessageId(PMC_MSG_ID_PROCESS_TERMINATED_IND);
    aMessage.setModuleId(PLUGIN_ID_PMC_C);
    aMessage << aPid;
}
inline void getProcessTerminatedIndParams(CommsMessage& aMessage,
        int& aPid)
{
    if (aMessage.getMessageId() == PMC_MSG_ID_PROCESS_TERMINATED_IND)
    {
        aMessage >> aPid;
    }
    else
    {
        ELOG2(EJavaCaptain, "wrong MessageId!: %d should be %d",
              aMessage.getMessageId(), PMC_MSG_ID_PROCESS_TERMINATED_IND);
    }
}


// Process initialised indication
inline void setProcessInitialisedIndParams(CommsMessage& aMessage,
        const int& aPid)
{
    aMessage.setMessageId(PMC_MSG_ID_PROCESS_INITIALISED_IND);
    aMessage.setModuleId(PLUGIN_ID_PMC_C);
    aMessage << aPid;
}
inline void getProcessInitialisedIndParams(CommsMessage& aMessage,
        int& aPid)
{
    if (aMessage.getMessageId() == PMC_MSG_ID_PROCESS_INITIALISED_IND)
    {
        aMessage >> aPid;
    }
    else
    {
        ELOG2(EJavaCaptain, "wrong MessageId!: %d should be %d",
              aMessage.getMessageId(), PMC_MSG_ID_PROCESS_INITIALISED_IND);
    }
}

} // namespace captain
} // namespace java

#endif // PMCMESSAGES_H

