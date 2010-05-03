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
* Description:  rtcMessages
*
*/

#ifndef RTCMESSAGES_H
#define RTCMESSAGES_H

#include "logger.h"
#include "comms.h"
#include "commsmessage.h"

#include "javauid.h"

using namespace java::comms;

using java::util::Uid;

namespace java
{
namespace captain
{
// Message IDs of all RTC services
const int RTC_MSG_ID_LAUNCH_APPLICATION_REQ         = JAVACAPTAIN_MESSAGE_ID_RANGE_START_C + 1; // 101
const int RTC_MSG_ID_TERMINATE_APPLICATION_REQ      = JAVACAPTAIN_MESSAGE_ID_RANGE_START_C + 2; // 102
const int RTC_MSG_ID_APPLICATION_RUNNING_IND        = JAVACAPTAIN_MESSAGE_ID_RANGE_START_C + 3; // 103
const int RTC_MSG_ID_APPLICATION_TERMINATED_IND     = JAVACAPTAIN_MESSAGE_ID_RANGE_START_C + 4; // 104
const int RTC_MSG_ID_ADD_PUSH_CONNECTION_IND        = JAVACAPTAIN_MESSAGE_ID_RANGE_START_C + 5; // 105

const int RTC_MSG_ID_SET_EXTRA_ARGUMENTS            = JAVACAPTAIN_MESSAGE_ID_RANGE_START_C + 50; // 150
const int RTC_MSG_ID_GET_EXTRA_ARGUMENTS            = JAVACAPTAIN_MESSAGE_ID_RANGE_START_C + 51; // 151
const int RTC_MSG_ID_GET_EXTRA_ARGUMENTS_ACK        = JAVACAPTAIN_MESSAGE_ID_RANGE_START_C + 52; // 152

const int RTC_LAUNCH_TYPE_NORMAL_C                    = 0;
const int RTC_LAUNCH_TYPE_PUSH_C                      = 1;
const int RTC_LAUNCH_TYPE_AUTO_INVOCATION_C           = 2;
const int RTC_LAUNCH_TYPE_BACKGROUND_C                = 3;
const int RTC_LAUNCH_TYPE_DEBUG_C                     = 4;
const int RTC_LAUNCH_TYPE_PREWARM_C                   = 5;

#define PREWARM_UID java::util::Uid(L"prewarm")

const int RTC_LAUNCH_OPTIONS_NONE_C                 = 0x00;
const int RTC_LAUNCH_OPTIONS_RUNNING_IND_REQ_C      = 0x01;
const int RTC_LAUNCH_OPTIONS_TERMINATE_IND_REQ_C    = 0x02;

const char* const RTC_LAUNCH_RUNTIME_MIDP_C         = "midp";

const int RTC_LAUNCH_OPTIONS_INDICATION_MASK        = RTC_LAUNCH_OPTIONS_RUNNING_IND_REQ_C |
        RTC_LAUNCH_OPTIONS_TERMINATE_IND_REQ_C;

const int RTC_TERMINATE_OPTIONS_NONE_C              = 0x00;
const int RTC_TERMINATE_OPTIONS_TERMINATE_IND_REQ_C = 0x02;

const int RTC_TERMINATE_OPTIONS_INDICATION_MASK     = RTC_TERMINATE_OPTIONS_TERMINATE_IND_REQ_C;

// Launch application request
inline void setLaunchApplicationReqParams(CommsMessage& aMessage,
        const Uid& aUid, const int& aType, const int& aOptions,
        const std::string& aRtc = RTC_LAUNCH_RUNTIME_MIDP_C,
        const std::wstring& aApplicationArguments = L"",
        const std::wstring& aRuntimeArguments = L"")
{
    aMessage.setModuleId(PLUGIN_ID_RTC_C);
    aMessage.setMessageId(RTC_MSG_ID_LAUNCH_APPLICATION_REQ);
    aMessage << aUid << aType << aOptions << aRtc;
    aMessage << aApplicationArguments << aRuntimeArguments;

    LOG4(EJavaCaptain, EInfoHeavyLoad,
         "RTC_MSG_ID_LAUNCH_APPLICATION_REQ (uid=%S, rtc=%s, type=%d, options=%d) encoded",
         aUid.toString().c_str(), aRtc.c_str(), aType, aOptions);
}

inline void getLaunchApplicationReqParams(CommsMessage& aMessage,
        Uid& aUid, int& aType, int& aOptions, std::string& aRtc,
        std::wstring& aApplicationArguments, std::wstring& aRuntimeArguments)
{
    if (aMessage.getMessageId() == RTC_MSG_ID_LAUNCH_APPLICATION_REQ)
    {
        aMessage >> aUid >> aType >> aOptions >> aRtc;
        aMessage >> aApplicationArguments >> aRuntimeArguments;

        LOG4(EJavaCaptain, EInfoHeavyLoad,
             "RTC_MSG_ID_LAUNCH_APPLICATION_REQ (uid=%S, rtc=%s, type=%d, options=%d) decoded",
             aUid.toString().c_str(), aRtc.c_str(), aType, aOptions);
    }
    else
    {
        ELOG2(EJavaCaptain, "wrong MessageId!: %d should be %d",
              aMessage.getMessageId(), RTC_MSG_ID_LAUNCH_APPLICATION_REQ);
    }
}

// Terminate application request
inline void setTerminateApplicationReqParams(CommsMessage& aMessage,
        const Uid& aUid, const int& aOptions)
{
    aMessage.setModuleId(PLUGIN_ID_RTC_C);
    aMessage.setMessageId(RTC_MSG_ID_TERMINATE_APPLICATION_REQ);
    aMessage << aUid << aOptions;
    LOG2(EJavaCaptain, EInfoHeavyLoad,
         "RTC_MSG_ID_TERMINATE_APPLICATION_REQ (uid=%S, options=%d) encoded",
         aUid.toString().c_str(), aOptions);
}
inline void getTerminateApplicationReqParams(CommsMessage& aMessage,
        Uid& aUid, int& aOptions)
{
    if (aMessage.getMessageId() == RTC_MSG_ID_TERMINATE_APPLICATION_REQ)
    {
        aMessage >> aUid >> aOptions;
        LOG2(EJavaCaptain, EInfoHeavyLoad,
             "RTC_MSG_ID_TERMINATE_APPLICATION_REQ (uid=%S, options=%d) decoded",
             aUid.toString().c_str(), aOptions);
    }
    else
    {
        ELOG2(EJavaCaptain, "wrong MessageId!: %d should be %d",
              aMessage.getMessageId(), RTC_MSG_ID_TERMINATE_APPLICATION_REQ);
    }
}

// Application running indication
inline void setApplicationRunningIndParams(CommsMessage& aMessage,
        const Uid& aUid, const int& aStatus, const int& aPid = 0)
{
    aMessage.setModuleId(PLUGIN_ID_RTC_C);
    aMessage.setMessageId(RTC_MSG_ID_APPLICATION_RUNNING_IND);
    aMessage << aUid << aStatus << aPid;
    LOG3(EJavaCaptain, EInfoHeavyLoad,
         "RTC_MSG_ID_APPLICATION_RUNNING_IND (uid=%S, status=%d, pid=%d) encoded",
         aUid.toString().c_str(), aStatus, aPid);
}
inline void getApplicationRunningIndParams(CommsMessage& aMessage,
        Uid& aUid, int& aStatus)
{
    if (aMessage.getMessageId() == RTC_MSG_ID_APPLICATION_RUNNING_IND)
    {
        aMessage >> aUid >> aStatus;
        LOG2(EJavaCaptain, EInfoHeavyLoad,
             "RTC_MSG_ID_APPLICATION_RUNNING_IND (uid=%S, status=%d) decoded",
             aUid.toString().c_str(), aStatus);
    }
    else
    {
        ELOG2(EJavaCaptain, "wrong MessageId!: %d should be %d",
              aMessage.getMessageId(), RTC_MSG_ID_APPLICATION_RUNNING_IND);
    }
}

// Application terminated indication
inline void setApplicationTerminatedIndParams(CommsMessage& aMessage,
        const Uid& aUid, const int& aStatus)
{
    aMessage.setModuleId(PLUGIN_ID_RTC_C);
    aMessage.setMessageId(RTC_MSG_ID_APPLICATION_TERMINATED_IND);
    aMessage << aUid << aStatus;
    LOG2(EJavaCaptain, EInfoHeavyLoad,
         "RTC_MSG_ID_APPLICATION_TERMINATED_IND (uid=%S, status=%d) encoded",
         aUid.toString().c_str(), aStatus);
}
inline void getApplicationTerminatedIndParams(CommsMessage& aMessage,
        Uid& aUid, int& aStatus)
{
    if (aMessage.getMessageId() == RTC_MSG_ID_APPLICATION_TERMINATED_IND)
    {
        aMessage >> aUid >> aStatus;
        LOG2(EJavaCaptain, EInfoHeavyLoad,
             "RTC_MSG_ID_APPLICATION_TERMINATED_IND (uid=%S, status=%d) decoded",
             aUid.toString().c_str(), aStatus);
    }
    else
    {
        ELOG2(EJavaCaptain, "wrong MessageId!: %d should be %d",
              aMessage.getMessageId(), RTC_MSG_ID_APPLICATION_TERMINATED_IND);
    }
}

// Indication of adding a new push listener to already running MIDlet
inline void setUpdatePushReqParams(CommsMessage& aMessage, const Uid& aUid)
{
    aMessage.setModuleId(PLUGIN_ID_RTC_PUSH_C);
    aMessage.setMessageId(RTC_MSG_ID_ADD_PUSH_CONNECTION_IND);
    aMessage << aUid;
}

inline void getUpdatePushReqParams(CommsMessage& aMessage, Uid& aUid)
{
    if (aMessage.getMessageId() == RTC_MSG_ID_ADD_PUSH_CONNECTION_IND)
    {
        aMessage >> aUid;
    }
    else
    {
        ELOG2(EJavaCaptain, "wrong MessageId!: %d should be %d",
              aMessage.getMessageId(), RTC_MSG_ID_ADD_PUSH_CONNECTION_IND);
    }
}

inline void setSetExtraArguments(CommsMessage& aMessage,
                                 const std::wstring& aRuntimeArguments,
                                 const std::wstring& aApplicationArguments)
{
    aMessage.setModuleId(PLUGIN_ID_RTC_C);
    aMessage.setMessageId(RTC_MSG_ID_SET_EXTRA_ARGUMENTS);

    aMessage << aRuntimeArguments << aApplicationArguments;
}

inline void getSetExtraArguments(CommsMessage& aMessage,
                                 std::wstring& aRuntimeArguments,
                                 std::wstring& aApplicationArguments)
{
    if (aMessage.getMessageId() == RTC_MSG_ID_SET_EXTRA_ARGUMENTS)
    {
        aMessage >> aRuntimeArguments >> aApplicationArguments;
    }
    else
    {
        ELOG2(EJavaCaptain, "wrong MessageId!: %d should be %d",
              aMessage.getMessageId(), RTC_MSG_ID_SET_EXTRA_ARGUMENTS);
    }
}

inline void setGetExtraArguments(CommsMessage& aMessage)
{
    aMessage.setModuleId(PLUGIN_ID_RTC_C);
    aMessage.setMessageId(RTC_MSG_ID_GET_EXTRA_ARGUMENTS);
}

// Not needed (yet) ; inline void getGetRuntimeAttributes()

inline void setGetExtraArgumentsAck(CommsMessage& aMessage,
                                    const std::wstring& aRuntimeArguments,
                                    const std::wstring& aApplicationArguments)
{
    aMessage.setModuleId(PLUGIN_ID_RTC_C);
    aMessage.setMessageId(RTC_MSG_ID_GET_EXTRA_ARGUMENTS_ACK);

    aMessage << aRuntimeArguments << aApplicationArguments;
}

inline void getGetExtraArgumentsAck(CommsMessage& aMessage,
                                    std::wstring& aRuntimeArguments,
                                    std::wstring& aApplicationArguments)
{
    if (aMessage.getMessageId() == RTC_MSG_ID_GET_EXTRA_ARGUMENTS_ACK)
    {
        aMessage >> aRuntimeArguments >> aApplicationArguments;
    }
    else
    {
        ELOG2(EJavaCaptain, "wrong MessageId!: %d should be %d",
              aMessage.getMessageId(), RTC_MSG_ID_GET_EXTRA_ARGUMENTS_ACK);
    }
}

} // namespace captain
} // namespace java

#endif // RTCMESSAGES_H

