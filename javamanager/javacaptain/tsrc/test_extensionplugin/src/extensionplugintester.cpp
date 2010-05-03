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
* Description:  ExtensionPluginTester
*
*/

#include <unistd.h>

#include "comms.h"
#include "commsendpoint.h"
#include "commsmessage.h"
#include "logger.h"

#include "rtcinterface.h"
#include "pmcinterface.h"
#include "coreinterface.h"

#include "testmessages.h"
#include "testuids.h"

#include "extensionplugintester.h"

#ifdef __SYMBIAN32__
java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#else
extern "C" java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#endif
    return new java::captain::ExtensionPluginTester();
}

using namespace java::util;

namespace java
{
namespace captain
{

ExtensionPluginTester::ExtensionPluginTester()
        :mCore(0), mPush1Installed(false), mEventTimerId(0)
{
    JELOG2(EJavaCaptain);
}

ExtensionPluginTester::~ExtensionPluginTester()
{
    JELOG2(EJavaCaptain);
    mTimeoutRequesters.clear();
}

void ExtensionPluginTester::startPlugin(CoreInterface* core)
{
    JELOG2(EJavaCaptain);
    core->getComms()->registerListener(java::comms::PLUGIN_ID_JAVACAPTAIN_TESTER_C, this);
    mCore = core;
//    mEventTimerId = mCore->getTimerServer()->timerCreateSeconds(1, this);
}

void ExtensionPluginTester::stopPlugin()
{
    JELOG2(EJavaCaptain);
    mCore->getComms()->unregisterListener(java::comms::PLUGIN_ID_JAVACAPTAIN_TESTER_C, this);
    mCore->getTimerServer()->timerCancel(mEventTimerId);
    mCore = 0;
}

EventConsumerInterface* ExtensionPluginTester::getEventConsumer()
{
    JELOG2(EJavaCaptain);
    return this;
}

void ExtensionPluginTester::timerTimeout(const int& aTimerId)
{
    JELOG2(EJavaCaptain);
    LOG1(EJavaCaptain, EInfo, "timerTimeout(0x%x)", aTimerId);
    if (aTimerId == mEventTimerId)
    {
        CommsMessage msg;
        mCore->getEventDispatcher()->event("ExtensionPluginTester", msg);
    }
    else
    {
        timeoutRequesters::iterator iter = mTimeoutRequesters.find(aTimerId);
        if (iter != mTimeoutRequesters.end())
        {
            CommsMessage msg;
            setTimeoutParameters(msg, aTimerId);
            msg.setReceiver(iter->second);
            int rc = mCore->getComms()->send(msg);
            LOG1(EJavaCaptain, EInfo, "send(timeout_msg) returns %d", rc);
            mTimeoutRequesters.erase(iter);
        }
    }

    return;
}

void ExtensionPluginTester::event(const std::string& eventProvider,
                                  java::comms::CommsMessage& /*aMsg*/)
{
    JELOG2(EJavaCaptain);
    LOG1(EJavaCaptain, EInfo, "ExtensionPluginTester::event() from %s", eventProvider.c_str());
    if (eventProvider == "ExtensionPluginTester")
    {
        mEventTimerId = mCore->getTimerServer()->timerCreateSeconds(1, this);
    }
}

ApplicationManagementEventsInterface* ExtensionPluginTester::getApplicationManagementListener()
{
    JELOG2(EJavaCaptain);
    return this;
}

void ExtensionPluginTester::amAdded(const uids_t& uids)
{
    JELOG2(EJavaCaptain);
    uids_t::const_iterator iter = uids.begin();
    while (iter != uids.end())
    {
        if ((*iter) == uid_push1)
        {
            mPush1Installed = true;
            mCore->getRtc()->launch(rtcLaunchInfo(uid_push1, RTC_LAUNCH_TYPE_PUSH_C, "test1"));
        }

        LOG1WSTR(EJavaCaptain, EInfo, "amAdded(%s)", iter->toString());
        iter++;
    }
}

void ExtensionPluginTester::amUpdated(const uids_t& uids)
{
    JELOG2(EJavaCaptain);
    uids_t::const_iterator iter = uids.begin();
    while (iter != uids.end())
    {
        if ((*iter) == uid_push1)
        {
            mCore->getRtc()->launch(rtcLaunchInfo(uid_push1, RTC_LAUNCH_TYPE_PUSH_C, "test1"));
        }

        LOG1WSTR(EJavaCaptain, EInfo, "amUpdated(%s)", iter->toString());
        iter++;
    }
}

void ExtensionPluginTester::amDeleted(const uids_t& uids)
{
    JELOG2(EJavaCaptain);
    uids_t::const_iterator iter = uids.begin();
    while (iter != uids.end())
    {
        if ((*iter) == uid_push1)
        {
            mPush1Installed = false;
            mCore->getRtc()->terminate(rtcTerminateInfo(uid_push1));
        }

        LOG1WSTR(EJavaCaptain, EInfo, "amDeleted(%s)", iter->toString());
        iter++;
    }
}

ApplicationRuntimeEventsInterface* ExtensionPluginTester::getApplicationRuntimeListener()
{
    JELOG2(EJavaCaptain);
    return this;
}

void ExtensionPluginTester::arLaunched(const Uid& /*aUID*/, const int& /*aRuntimeCommsAddress*/)
{
    JELOG2(EJavaCaptain);
}

void ExtensionPluginTester::arTerminated(const Uid& aUID, const int& /*aExitCode*/)
{
    JELOG2(EJavaCaptain);

    if (aUID == uid_push1 && mPush1Installed)
    {
        mCore->getRtc()->launch(rtcLaunchInfo(uid_push1, RTC_LAUNCH_TYPE_PUSH_C, "test1"));
    }
}

java::comms::CommsListener* ExtensionPluginTester::getCommsListener()
{
    JELOG2(EJavaCaptain);
    return this;
}

void ExtensionPluginTester::processMessage(CommsMessage& message)
{
    JELOG2(EJavaCaptain);

    int statusInt = 0;
    CommsMessage reply;
    reply.setModuleId(message.getModuleId());
    reply.setReceiver(message.getSender());
    reply.setMessageRef(message.getMessageRef());

    switch (message.getMessageId())
    {
    case IPC_MESSAGE_TEST_EXTENSIONPLUGIN_LOAD_REQ:
    {
        std::string pluginName;
        getLoadExtensionPluginReqParameters(message, pluginName);
        ExtensionPluginInterface* interface = mCore->loadExtensionPlugin(pluginName);
        statusInt = interface ? 0 : 1;
    }
    break;

    case IPC_MESSAGE_TEST_EXTENSIONPLUGIN_UNLOAD_REQ:
    {
        std::string pluginName;
        getUnloadExtensionPluginReqParameters(message, pluginName);
        mCore->unloadExtensionPlugin(pluginName);
    }
    break;

    case IPC_MESSAGE_TEST_LAUNCH_APP_REQ:
    {
        Uid uidi;
        int type = RTC_LAUNCH_TYPE_NORMAL_C;
        std::string rtc = "midp";
        getLaunchAppParameters(message, uidi, type, rtc);
        statusInt = (mCore->getRtc()->launch(rtcLaunchInfo(uidi, type, rtc)) ? 0 : 1);
    }
    break;

    case IPC_MESSAGE_TEST_TERMINATE_APP_REQ:
    {
        Uid uidi;
        getTerminateAppParameters(message, uidi);
        statusInt = (mCore->getRtc()->terminate(rtcTerminateInfo(uidi)) ? 0 : 1);
    }
    break;

    case IPC_MESSAGE_TEST_ENABLE_APP_REQ:
    {
        Uid uidi;
        getEnableAppParameters(message, uidi);
        mCore->getRtc()->enable(uidi);
    }
    break;

    case IPC_MESSAGE_TEST_DISABLE_APP_REQ:
    {
        Uid uidi;
        getDisableAppParameters(message, uidi);
        mCore->getRtc()->disable(uidi);
    }
    break;

    case IPC_MESSAGE_TEST_PROCESS_START:
    {
        std::string exe, params;
        getProcessStartParameters(message, exe, params);
        cmdLine_t cmdLine;
        cmdLine.push_back(exe);
        cmdLine.push_back(params);
        statusInt = mCore->getPmc()->launch(cmdLine, 0);
    }
    break;

    case IPC_MESSAGE_TEST_PROCESS_KILL:
    {
        int pidi = 0;
        getProcessKillParameters(message, pidi);
        statusInt = mCore->getPmc()->kill(pidi);
    }
    break;

    case IPC_MESSAGE_TEST_PROCESS_TERMINATE:
    {
        int pidi = 0;
        getProcessTerminateParameters(message, pidi);
        statusInt = mCore->getPmc()->terminate(pidi);
    }
    break;

    case IPC_MESSAGE_TEST_START_SECONDS_TIMER:
    {
        int timeout;
        getTimerStartSecondsParameters(message, timeout);
        int timerId = mCore->getTimerServer()->timerCreateSeconds(timeout, this);
        mTimeoutRequesters.insert(std::make_pair(timerId, message.getSender()));
        LOG2(EJavaCaptain, EInfo, "timerCreateSeconds(%d) returns %#x", timeout, timerId);
        setTimerAckParameters(reply, timerId);
        mCore->getComms()->send(reply);
    }
    return;

    case IPC_MESSAGE_TEST_START_JAVATIME_TIMER:
    {
        long long timeout;
        getTimerStartJavaTimeParameters(message, timeout);
        int timerId = mCore->getTimerServer()->timerCreateJavaTime(JavaTime(timeout), this);
        mTimeoutRequesters.insert(std::make_pair(timerId, message.getSender()));
        LOG2(EJavaCaptain, EInfo, "timerCreateJavaTime(%u) returns %#x", timeout, timerId);
        setTimerAckParameters(reply, timerId);
        mCore->getComms()->send(reply);
    }
    return;

    case IPC_MESSAGE_TEST_STOP_TIMER:
    {
        int timerId;
        getTimerStopParameters(message, timerId);
        mCore->getTimerServer()->timerCancel(timerId);
        timeoutRequesters::iterator iter = mTimeoutRequesters.find(timerId);
        if (iter != mTimeoutRequesters.end())
        {
            mTimeoutRequesters.erase(iter);
        }
        LOG1(EJavaCaptain, EInfo, "timerCancel(%#x) called", timerId);
        setTimerAckParameters(reply, timerId);
        mCore->getComms()->send(reply);
    }
    return;
    }

    setAckParameters(reply, statusInt);
    mCore->getComms()->send(reply);
}

} // namespace captain
} // namespace java

