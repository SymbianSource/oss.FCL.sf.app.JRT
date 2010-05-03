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
* Description:  amcmessages
*
*/

#ifndef AMCMESSAGES_H
#define AMCMESSAGES_H

#include "logger.h"
#include "comms.h"
#include "commsmessage.h"

using namespace java::comms;

namespace java
{
namespace captain
{
// Message IDs of all installer extension services
const int AMC_MSG_ID_REQUEST    = JAVACAPTAIN_MESSAGE_ID_RANGE_START_C + 10;    // 110
const int AMC_MSG_ID_RESPONSE   = AMC_MSG_ID_REQUEST + 1;                       // 111

// Operations
const int AMC_REQUEST_OPERATION_START   = 1;
const int AMC_REQUEST_OPERATION_STOP    = 2;
const int AMC_REQUEST_OPERATION_UPDATE  = 3;
// Logging helper
static const char* amcRequestOperation2String(const int& aOperation)
{
    switch (aOperation)
    {
    case AMC_REQUEST_OPERATION_START:
        return "OPERATION_START";

    case AMC_REQUEST_OPERATION_STOP:
        return "OPERATION_STOP";

    case AMC_REQUEST_OPERATION_UPDATE:
        return "OPERATION_UPDATE";

    default:
        return "UNKNOWN!";
    }
}

// Options
const int AMC_REQUEST_OPTIONS_NONE      = 0;
const int AMC_REQUEST_OPTIONS_SILENT    = 1;
// Logging helper
static const char* amcRequestOptions2String(const int& aOptions)
{
    switch (aOptions)
    {
    case AMC_REQUEST_OPTIONS_NONE:
        return "OPTIONS_NONE";

    case AMC_REQUEST_OPTIONS_SILENT:
        return "OPTIONS_SILENT";

    default:
        return "UNKNOWN!";
    }
}

inline void getAmcRequestParams(CommsMessage& aMessage, int& aOperation, int& aOptions)
{
    if (aMessage.getMessageId() == AMC_MSG_ID_REQUEST)
    {
        aMessage >> aOperation >> aOptions;
    }
    else
    {
        ELOG2(EJavaCaptain, "wrong MessageId!: %d should be %d",
              aMessage.getMessageId(), AMC_MSG_ID_REQUEST);
    }
}

inline void setAmcRequestParams(CommsMessage& aMessage, const int& aOperation, const int& aOptions)
{
    aMessage.setMessageId(AMC_MSG_ID_REQUEST);
    aMessage.setModuleId(PLUGIN_ID_AMC_C);
    aMessage << aOperation << aOptions;
}

inline void getAmcResponseParameters(CommsMessage& aMessage, int& aNumOfUids)
{
    if (aMessage.getMessageId() == AMC_MSG_ID_RESPONSE)
    {
        aMessage >> aNumOfUids;
    }
    else
    {
        ELOG2(EJavaCaptain, "wrong MessageId!: %d should be %d",
              aMessage.getMessageId(), AMC_MSG_ID_RESPONSE);
    }
}

inline void setAmcResponseParamaters(CommsMessage& aMessage, int aNumOfUids)
{
    aMessage.setMessageId(AMC_MSG_ID_RESPONSE);
    aMessage.setModuleId(PLUGIN_ID_AMC_C);
    aMessage << aNumOfUids;
}

} // namespace captain
} // namespace java

#endif // AMCMESSAGES_H

