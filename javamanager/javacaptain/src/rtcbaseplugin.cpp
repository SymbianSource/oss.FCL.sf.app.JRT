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
* Description:  RtcBasePlugin
*
*/


#include "logger.h"
#include "javacommonutils.h"

#include "commsendpoint.h"
#include "commsclientendpoint.h"
#include "commsmessage.h"

#include "rtcbaseplugin.h"
#include "rtcmessages.h"
#include "coremessages.h"

#include "coreinterface.h"
#include "timerserverinterface.h"
#include "pmcinterface.h"

namespace java
{
namespace captain
{

const int EXIT_TIMER_TIMEOUT    = 5;
const int LAUNCH_TIMER_TIMEOUT  = 5;
const int LAUNCH_TIMER_TIMEOUT_PREWARM  = 60;

RtcBasePlugin::RtcBasePlugin(const Uid& aUID,
                             CoreInterface* aCore)
        :mUID(aUID),
        mCore(aCore),
        mState(APPLICATION_STATE_IDLE_C),
        mRuntimeAddress(0),
        mPid(-1),
        mTimerId(0),
        mPendingOperation(PENDING_OPERATION_NONE_C)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
}

RtcBasePlugin::~RtcBasePlugin()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    runtime_init();

    mCore = 0;
}
// Already routed on upper level using pid
void RtcBasePlugin::pmcTerminated(const int& /*aPid*/, const int& /*exitCode*/)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    mPid = -1;
    mRuntimeAddress = 0;

    switchState(APPLICATION_STATE_IDLE_C);
}

// TimerServerEventsInterface methods
void RtcBasePlugin::timerTimeout(const int& aTimerId)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    LOG1WSTR(EJavaCaptain, EError, "RtcBasePlugin::timerTimeout(%s)", mUID.toString());
    LOG1(EJavaCaptain, EInfo, "runtime in state: %s", E2S_state(mState));

    if (aTimerId == mTimerId)
    {
        mTimerId = 0;
        runtime_init();
    }
    else
    {
        ELOG2(EJavaCaptain, "Wrong timerId! ok:%d FAIL:%d", mTimerId, aTimerId);
    }
}

bool RtcBasePlugin::launch(const rtcLaunchInfo& aLaunchInfo)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    LOG1WSTR(EJavaCaptain, EInfoHeavyLoad, "RtcBasePlugin::launch(%s)", mUID.toString());
    if (mUID == PREWARM_UID)
    {
        mUID = aLaunchInfo.mUID;
    }
    else if (mUID != aLaunchInfo.mUID)
    {
        ELOG(EJavaCaptain, "launch() called for a wrong instance");
        return false; // not launched
    }

    mRuntimeArguments = aLaunchInfo.mRuntimeArguments;
    mApplicationArguments = aLaunchInfo.mApplicationArguments;

    bool launchSuccess = false;

    switch (mState)
    {
    case APPLICATION_STATE_IDLE_C:
        if (runtime_launch(aLaunchInfo.mLaunchType))
        {
            launchSuccess = true;
            switchState(APPLICATION_STATE_LAUNCHED_C);
        }
        break;

    case APPLICATION_STATE_LAUNCHED_C:
    case APPLICATION_STATE_EXITING_C:
        launchSuccess = true;
        setPendingLaunch(aLaunchInfo.mLaunchType);
        break; // Just be happy

    case APPLICATION_STATE_RUNNING_C:
        launchSuccess = true;
        if (!runtime_relaunch(aLaunchInfo.mLaunchType))   // if fails
        {
            runtime_init(); // ????
            // Launch a new instance
            if (runtime_launch(aLaunchInfo.mLaunchType))
            {
                switchState(APPLICATION_STATE_LAUNCHED_C);
            }
            else
            {
                ELOG(EJavaCaptain, "Application launch failed!");
                runtime_init();
                switchState(APPLICATION_STATE_IDLE_C);
                launchSuccess = false;
            }
        }
        break;

    default:
        ELOG1(EJavaCaptain, "Invalid state -> %s", E2S_state(mState));
        break;
    }

    return launchSuccess;
}

bool RtcBasePlugin::terminate(const rtcTerminateInfo& aTerminateInfo)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    LOG1WSTR(EJavaCaptain, EInfoHeavyLoad, "terminate(%s)", mUID.toString());

    if (mUID != aTerminateInfo.mUID)
    {
        ELOG(EJavaCaptain, "terminate() called for a wrong instance");
        return true; // already terminated
    }

    bool isAlreadyTerminated = false;

    switch (mState)
    {
    case APPLICATION_STATE_IDLE_C:
        isAlreadyTerminated = true;
        break;

    case APPLICATION_STATE_LAUNCHED_C:
        setPendingTerminate();
        break;

    case APPLICATION_STATE_RUNNING_C:
        if (runtime_terminate())   // terminate requested
        {
            switchState(APPLICATION_STATE_EXITING_C);
        }
        else // request failed, application probably already dead/zombie
        {
            runtime_init();
            switchState(APPLICATION_STATE_IDLE_C);
            isAlreadyTerminated = true;
        }
        break;

    case APPLICATION_STATE_EXITING_C:
        break;

    default:
        ELOG1(EJavaCaptain, "Illegal state %d", mState);
        break;
    }

    return isAlreadyTerminated;
}

void RtcBasePlugin::switchState(const ApplicationState& newState)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    LOG1WSTR(EJavaCaptain, EInfo, "switchState() and mUID is %s", mUID.toString());
    LOG2(EJavaCaptain, EInfo, "switchState %s -> %s", E2S_state(mState), E2S_state(newState));

    mState = newState;

    switch (newState)
    {
    case  APPLICATION_STATE_IDLE_C:
        stopTimer();
        if (mPid > 0 && mCore)
        {
            mCore->getPmc()->kill(mPid);
        }
        handlePendingOperations();
        break;

    case APPLICATION_STATE_LAUNCHED_C:
        if (mUID == PREWARM_UID)
        {
            startTimer(LAUNCH_TIMER_TIMEOUT_PREWARM);
        }
        else
        {
            startTimer(LAUNCH_TIMER_TIMEOUT);
        }
        break;

    case APPLICATION_STATE_RUNNING_C:
        stopTimer();
        handlePendingOperations();
        break;

    case APPLICATION_STATE_EXITING_C:
        startTimer(EXIT_TIMER_TIMEOUT);
        break;

    default:
        ELOG1(EJavaCaptain, "Invalid new state requested %s", E2S_state(newState));
        break;
    }
}

void RtcBasePlugin::runningInd(const int& aRuntimeAddress, const int& /*aStatus*/)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    // Assumes that sender address is always latest and valid.
    mRuntimeAddress = aRuntimeAddress;

    switch (mState)
    {
        // Only handled once after launch othervise igoned
    case APPLICATION_STATE_LAUNCHED_C:
        switchState(APPLICATION_STATE_RUNNING_C);
        break;

        // Waking up a little bit too late, give another chance to do the right thing
    case APPLICATION_STATE_EXITING_C:
        runtime_terminate();
        startTimer(EXIT_TIMER_TIMEOUT);
        break;

    default:
        break;
    }
}

void RtcBasePlugin::terminatedInd(const int& /*aStatus*/)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    // Only handled if not already in EXITING state,
    // othervise ignored and deemed as an extra terminated_ind
    if (mState != APPLICATION_STATE_EXITING_C)
    {
        switchState(APPLICATION_STATE_EXITING_C);
    }
}

void RtcBasePlugin::processMessage(CommsMessage& message)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    switch (message.getMessageId())
    {
    case CORE_MSG_ID_DO_THREAD_DUMP:
        LOG(EJavaCaptain, EInfo, "sending DO_THREAD_DUMP message to runtime");
        sendMessageToRuntime(message.getModuleId(), message);
        break;

    case RTC_MSG_ID_GET_EXTRA_ARGUMENTS:
        if (message.getSender() == mRuntimeAddress && isRunning())
        {
            CommsMessage reply;
            reply.replyTo(message);
            setGetExtraArgumentsAck(reply, mRuntimeArguments, mApplicationArguments);
            LOG2(EJavaCaptain, EInfo, "reply to get runtime attributes: mUid=%S mRuntimeArguments=%S",
                 mUID.toString().c_str(), mRuntimeArguments.c_str());
            sendMessageToRuntime(message.getModuleId(), reply);
        }
        break;

    default:
        ELOG1(EJavaCaptain, "Unknown message forwarded to rtcplugin %d", message.getMessageId());
        break;
    }
}

int RtcBasePlugin::sendMessageToRuntime(const int& moduleId, CommsMessage& message)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    message.setReceiver(mRuntimeAddress);
    message.setModuleId(moduleId);

    return mCore->getComms()->send(message);
}

bool RtcBasePlugin::isRunning() const
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    return mState == APPLICATION_STATE_RUNNING_C;
}

void RtcBasePlugin::runtime_init()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    if (mPid > 0 && mCore)
    {
        LOG1(EJavaCaptain, EInfo, "Killing process with uid %S",
             mUID.toString().c_str());
        mCore->getPmc()->kill(mPid);
    }
}

bool RtcBasePlugin::runtime_launch(const int& aLaunchType)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    std::vector<std::string> params;
    mPid = mCore->getPmc()->launch(generateCommandLine(aLaunchType, params), 0);
    return mPid > 0;
}

bool RtcBasePlugin::runtime_relaunch(const int& aLaunchType)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    int status = 0;
    CommsMessage msg;

    switch (aLaunchType)
    {
    case RTC_LAUNCH_TYPE_PUSH_C:
        setUpdatePushReqParams(msg, mUID);
        status = sendMessageToRuntime(PLUGIN_ID_RTC_PUSH_C, msg);
        LOG1(EJavaCaptain, EInfo, "sendUpdatePushMessageToRuntime() returns %d", status);
        break;

    case RTC_LAUNCH_TYPE_PREWARM_C: // do nothing
        break;

    default:
        setLaunchApplicationReqParams(msg, mUID, aLaunchType,
                                      RTC_LAUNCH_OPTIONS_NONE_C, RTC_LAUNCH_RUNTIME_MIDP_C, mApplicationArguments);
        status = sendMessageToRuntime(PLUGIN_ID_RTC_C, msg);
        LOG1(EJavaCaptain, EInfo,
             "RtcBasePlugin::runtime_relaunch: sendMessageToRuntime() returns %d", status);
        break;
    }

    return status == 0;
}

bool RtcBasePlugin::runtime_terminate()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    CommsMessage terminateMsg;
    setTerminateApplicationReqParams(terminateMsg, mUID, 0);
    int status = sendMessageToRuntime(PLUGIN_ID_RTC_C, terminateMsg);
    LOG1(EJavaCaptain, EInfo, "sendMessageToRuntime() returns %d", status);
    return status == 0;
}

void RtcBasePlugin::startTimer(const int& aTimeout)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    // Only cancel if already started
    if (0 != mTimerId)
    {
        mCore->getTimerServer()->timerCancel(mTimerId);
        mTimerId = 0;
    }
    mTimerId = mCore->getTimerServer()->timerCreateSeconds(aTimeout, this);
}
void RtcBasePlugin::stopTimer()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    if (0 != mTimerId)
    {
        mCore->getTimerServer()->timerCancel(mTimerId);
        mTimerId = 0;
    }
}

void RtcBasePlugin::setPendingLaunch(const int& aType)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    switch (aType)
    {
    case RTC_LAUNCH_TYPE_NORMAL_C:
        mPendingOperation = PENDING_OPERATION_LAUNCH_NORMAL_C;
        break;

    case RTC_LAUNCH_TYPE_PUSH_C:
        mPendingOperation = PENDING_OPERATION_LAUNCH_PUSH_C;
        break;

    case RTC_LAUNCH_TYPE_AUTO_INVOCATION_C:
        mPendingOperation = PENDING_OPERATION_LAUNCH_AUTO_INVOCATION_C;
        break;

    case RTC_LAUNCH_TYPE_BACKGROUND_C:
        mPendingOperation = PENDING_OPERATION_LAUNCH_BACKGROUND_C;
        break;

    case RTC_LAUNCH_TYPE_DEBUG_C:
        mPendingOperation = PENDING_OPERATION_LAUNCH_DEBUG_C;
        break;

    default:
        ELOG1(EJavaCaptain, "Unknown pending launch type! (%d)", aType);
        mPendingOperation = PENDING_OPERATION_NONE_C;
        break;
    }
}

void RtcBasePlugin::setPendingTerminate()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    mPendingOperation = PENDING_OPERATION_TERMINATE_C;
}

void RtcBasePlugin::handlePendingOperations()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    switch (mPendingOperation)
    {
    case PENDING_OPERATION_LAUNCH_NORMAL_C:
        mPendingOperation = PENDING_OPERATION_NONE_C;
        launch(rtcLaunchInfo(mUID, RTC_LAUNCH_TYPE_NORMAL_C));
        break;
    case PENDING_OPERATION_LAUNCH_PUSH_C:
        mPendingOperation = PENDING_OPERATION_NONE_C;
        launch(rtcLaunchInfo(mUID, RTC_LAUNCH_TYPE_PUSH_C));
        break;
    case PENDING_OPERATION_LAUNCH_AUTO_INVOCATION_C:
        mPendingOperation = PENDING_OPERATION_NONE_C;
        launch(rtcLaunchInfo(mUID, RTC_LAUNCH_TYPE_AUTO_INVOCATION_C));
        break;
    case PENDING_OPERATION_LAUNCH_BACKGROUND_C:
        mPendingOperation = PENDING_OPERATION_NONE_C;
        launch(rtcLaunchInfo(mUID, RTC_LAUNCH_TYPE_BACKGROUND_C));
        break;
    case PENDING_OPERATION_LAUNCH_DEBUG_C:
        mPendingOperation = PENDING_OPERATION_NONE_C;
        launch(rtcLaunchInfo(mUID, RTC_LAUNCH_TYPE_DEBUG_C));
        break;
    case PENDING_OPERATION_TERMINATE_C:
        mPendingOperation = PENDING_OPERATION_NONE_C;
        terminate(rtcTerminateInfo(mUID));
        break;
    case PENDING_OPERATION_NONE_C:
    default:
        mPendingOperation = PENDING_OPERATION_NONE_C;
        break;
    }
}

} // namespace captain
} // namespace java

