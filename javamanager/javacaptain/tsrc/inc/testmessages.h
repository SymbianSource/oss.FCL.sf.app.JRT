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
* Description:  TestMessages
*
*/

#ifndef TESTMESSAGES_H
#define TESTMESSAGES_H

#include <assert.h>
#include <string>

#include "logger.h"
#include "javauid.h"
#include "comms.h"
#include "commsmessage.h"

using java::util::Uid;

using namespace java::comms;

namespace java
{
namespace captain
{

const int IPC_MESSAGE_TEST_ACK                          = 100;
inline void setAckParameters(CommsMessage& aMessage, int& success)
{
    aMessage.setMessageId(IPC_MESSAGE_TEST_ACK);
    aMessage << success;
}
inline void getAckParameters(CommsMessage& aMessage, int& success)
{
    assert(aMessage.getMessageId() == IPC_MESSAGE_TEST_ACK);
    aMessage >> success;
}

const int IPC_MESSAGE_TEST_EXTENSIONPLUGIN_LOAD_REQ     = 200;
inline void setLoadExtensionPluginReqParameters(CommsMessage& aMessage, const std::string& pluginName)
{
    aMessage.setMessageId(IPC_MESSAGE_TEST_EXTENSIONPLUGIN_LOAD_REQ);
    aMessage << pluginName;
}
inline void getLoadExtensionPluginReqParameters(CommsMessage& aMessage, std::string& pluginName)
{
    assert(aMessage.getMessageId() == IPC_MESSAGE_TEST_EXTENSIONPLUGIN_LOAD_REQ);
    aMessage >> pluginName;
}

const int IPC_MESSAGE_TEST_EXTENSIONPLUGIN_UNLOAD_REQ   = 300;
inline void setUnloadExtensionPluginReqParameters(CommsMessage& aMessage, const std::string& pluginName)
{
    aMessage.setMessageId(IPC_MESSAGE_TEST_EXTENSIONPLUGIN_UNLOAD_REQ);
    aMessage << pluginName;
}
inline void getUnloadExtensionPluginReqParameters(CommsMessage& aMessage, std::string& pluginName)
{
    assert(aMessage.getMessageId() == IPC_MESSAGE_TEST_EXTENSIONPLUGIN_UNLOAD_REQ);
    aMessage >> pluginName;
}

const int IPC_MESSAGE_TEST_LAUNCH_APP_REQ       = 550;
inline void setLaunchAppParameters(CommsMessage& aMessage, const Uid& aUid,
                                   const int& aType, const std::string& aRtc)
{
    aMessage.setMessageId(IPC_MESSAGE_TEST_LAUNCH_APP_REQ);
    aMessage.setModuleId(PLUGIN_ID_JAVACAPTAIN_TESTER_C);
    aMessage << aUid << aType << aRtc;
}
inline void getLaunchAppParameters(CommsMessage& aMessage, Uid& aUid,
                                   int& aType, std::string& aRtc)
{
    aMessage >> aUid >> aType >> aRtc;
}

const int IPC_MESSAGE_TEST_TERMINATE_APP_REQ    = 551;
inline void setTerminateAppParameters(CommsMessage& aMessage, const Uid& aUid)
{
    aMessage.setMessageId(IPC_MESSAGE_TEST_TERMINATE_APP_REQ);
    aMessage.setModuleId(PLUGIN_ID_JAVACAPTAIN_TESTER_C);
    aMessage << aUid;
}
inline void getTerminateAppParameters(CommsMessage& aMessage, Uid& aUid)
{
    aMessage >> aUid;
}

const int IPC_MESSAGE_TEST_ENABLE_APP_REQ         = 600;
inline void setEnableAppParameters(CommsMessage& aMessage, const Uid& aUid)
{
    aMessage.setMessageId(IPC_MESSAGE_TEST_ENABLE_APP_REQ);
    aMessage.setModuleId(PLUGIN_ID_JAVACAPTAIN_TESTER_C);
    aMessage << aUid;
}
inline void getEnableAppParameters(CommsMessage& aMessage, Uid& aUid)
{
    aMessage >> aUid;
}
const int IPC_MESSAGE_TEST_DISABLE_APP_REQ        = 700;
inline void setDisableAppParameters(CommsMessage& aMessage, const Uid& aUid)
{
    aMessage.setMessageId(IPC_MESSAGE_TEST_DISABLE_APP_REQ);
    aMessage.setModuleId(PLUGIN_ID_JAVACAPTAIN_TESTER_C);
    aMessage << aUid;
}
inline void getDisableAppParameters(CommsMessage& aMessage, Uid& aUid)
{
    aMessage >> aUid;
}

const int IPC_MESSAGE_TEST_PROCESS_START                = 701;
inline void setProcessStartParameters(CommsMessage& aMessage, const std::string& exe, const std::string& params)
{
    aMessage.setMessageId(IPC_MESSAGE_TEST_PROCESS_START);
    aMessage << exe << params;
}
inline void getProcessStartParameters(CommsMessage& aMessage, std::string& exe, std::string& params)
{
    assert(aMessage.getMessageId() == IPC_MESSAGE_TEST_PROCESS_START);
    aMessage >> exe >> params;
}

const int IPC_MESSAGE_TEST_PROCESS_KILL                 = 702;
inline void setProcessKillParameters(CommsMessage& aMessage, const int& aPid)
{
    aMessage.setMessageId(IPC_MESSAGE_TEST_PROCESS_KILL);
    aMessage.setModuleId(PLUGIN_ID_JAVACAPTAIN_TESTER_C);
    aMessage << aPid;
}
inline void getProcessKillParameters(CommsMessage& aMessage, int& aPid)
{
    aMessage >> aPid;
}
const int IPC_MESSAGE_TEST_PROCESS_TERMINATE            = 703;
inline void setProcessTerminateParameters(CommsMessage& aMessage, const int& aPid)
{
    aMessage.setMessageId(IPC_MESSAGE_TEST_PROCESS_TERMINATE);
    aMessage.setModuleId(PLUGIN_ID_JAVACAPTAIN_TESTER_C);
    aMessage << aPid;
}
inline void getProcessTerminateParameters(CommsMessage& aMessage, int& aPid)
{
    aMessage >> aPid;
}

const int IPC_MESSAGE_TEST_START_SECONDS_TIMER          = 710;
const int IPC_MESSAGE_TEST_START_JAVATIME_TIMER         = 711;
const int IPC_MESSAGE_TEST_STOP_TIMER                   = 715;
const int IPC_MESSAGE_TEST_TIMER_ACK                    = 716;
const int IPC_MESSAGE_TEST_TIMEOUT                      = 720;

inline void setTimerStartSecondsParameters(CommsMessage& aMessage, const int& aTimeout)
{
    aMessage.setMessageId(IPC_MESSAGE_TEST_START_SECONDS_TIMER);
    aMessage << aTimeout;
    LOG1(EJavaCaptain, EInfoHeavyLoad,
         "IPC_MESSAGE_TEST_START_SECONDS_TIMER (timeout=%d) encoded", aTimeout);
}
inline void getTimerStartSecondsParameters(CommsMessage& aMessage, int& aTimeout)
{
    assert(aMessage.getMessageId() == IPC_MESSAGE_TEST_START_SECONDS_TIMER);
    aMessage >> aTimeout;
    LOG1(EJavaCaptain, EInfoHeavyLoad,
         "IPC_MESSAGE_TEST_START_SECONDS_TIMER (timeout=%d) decoded", aTimeout);
}

inline void setTimerStartJavaTimeParameters(CommsMessage& aMessage, const long long& aTimeout)
{
    aMessage.setMessageId(IPC_MESSAGE_TEST_START_JAVATIME_TIMER);
    aMessage << aTimeout;
    LOG1(EJavaCaptain, EInfoHeavyLoad,
         "IPC_MESSAGE_TEST_START_JAVATIME_TIMER (timeout=%u) encoded", aTimeout);
}
inline void getTimerStartJavaTimeParameters(CommsMessage& aMessage, long long& aTimeout)
{
    assert(aMessage.getMessageId() == IPC_MESSAGE_TEST_START_JAVATIME_TIMER);
    aMessage >> aTimeout;
    LOG1(EJavaCaptain, EInfoHeavyLoad,
         "IPC_MESSAGE_TEST_START_JAVATIMER_TIMER (timeout=%u) decoded", aTimeout);
}

inline void setTimerStopParameters(CommsMessage& aMessage, const int& aTimerId)
{
    aMessage.setMessageId(IPC_MESSAGE_TEST_STOP_TIMER);
    aMessage << aTimerId;
    LOG1(EJavaCaptain, EInfoHeavyLoad,
         "IPC_MESSAGE_TEST_STOP_TIMER (timerId=0x%x) encoded", aTimerId);
}
inline void getTimerStopParameters(CommsMessage& aMessage, int& aTimerId)
{
    assert(aMessage.getMessageId() == IPC_MESSAGE_TEST_STOP_TIMER);
    aMessage >> aTimerId;
    LOG1(EJavaCaptain, EInfoHeavyLoad,
         "IPC_MESSAGE_TEST_STOP_TIMER (timerId=0x%x) decoded", aTimerId);
}

inline void setTimerAckParameters(CommsMessage& aMessage, const int& aTimerId)
{
    aMessage.setMessageId(IPC_MESSAGE_TEST_TIMER_ACK);
    aMessage << aTimerId;
    LOG1(EJavaCaptain, EInfoHeavyLoad,
         "IPC_MESSAGE_TEST_TIMER_ACK (timerId=0x%x) encoded", aTimerId);
}
inline void getTimerAckParameters(CommsMessage& aMessage, int& aTimerId)
{
    assert(aMessage.getMessageId() == IPC_MESSAGE_TEST_TIMER_ACK);
    aMessage >> aTimerId;
    LOG1(EJavaCaptain, EInfoHeavyLoad,
         "IPC_MESSAGE_TEST_TIMER_ACK (timerId=0x%x) decoded", aTimerId);
}

inline void setTimeoutParameters(CommsMessage& aMessage, const int& aTimerId)
{
    aMessage.setMessageId(IPC_MESSAGE_TEST_TIMEOUT);
    aMessage << aTimerId;
    LOG1(EJavaCaptain, EInfoHeavyLoad,
         "IPC_MESSAGE_TEST_TIMEOUT (timerId=0x%x) encoded", aTimerId);
}
inline void getTimeoutParameters(CommsMessage& aMessage, int& aTimerId)
{
    assert(aMessage.getMessageId() == IPC_MESSAGE_TEST_TIMEOUT);
    aMessage >> aTimerId;
    LOG1(EJavaCaptain, EInfoHeavyLoad,
         "IPC_MESSAGE_TEST_TIMEOUT (timerId=0x%x) decoded", aTimerId);
}


} // namespace captain
} // namespace java


#endif // TESTMESSAGES_H

