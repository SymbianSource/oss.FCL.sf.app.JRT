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
* Description:  Test2Runtime
*
*/


#include "logger.h"

#include "javaprocessconstants.h"
#include "commsendpoint.h"

#include "coreinterface.h"
#include "pmcinterface.h"
#include "rtcmessages.h"

#include "test2runtime.h"

const int EXIT_TIMER_TIMEOUT    = 2;
const int LAUNCH_TIMER_TIMEOUT  = 5;

namespace java
{
namespace captain
{

Test2Runtime::Test2Runtime(const Uid& aUID,
                           CoreInterface* aCore)
        :mUid(aUID), mCore(aCore), mPid(-1),
        mRunning(false), mExiting(false),
        mRuntimeAddress(0), mTimerId(0)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
}

Test2Runtime::~Test2Runtime()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
}

const char* const APP_UID_ARGUMENT          = "-appid";
const char* const PUSH_ARGUMENT             = "-push";
const char* const PREWARM_ARGUMENT          = "-prewarm";
const char* const AUTO_INVOCATION_ARGUMENT  = "-autoinvocation";
const char* const DEBUG_ARGUMENT            = "-debug";
const char* const BG_START_ARGUMENT         = "-background";

// RtcRuntimeInterface
bool Test2Runtime::launch(const rtcLaunchInfo& aLaunchInfo)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    bool launchSuccess = false;

    std::vector<std::string> params;

    params.push_back("javatestruntime");
    params.push_back(java::runtime::JAVA_MIDP_STARTER_DLL); //The name of the dll, that runs MIDlets
    params.push_back(APP_UID_ARGUMENT);

    char* appUID = java::util::JavaCommonUtils::wstringToUtf8(mUid.toString());
    params.push_back(appUID);
    delete [] appUID;

    switch (aLaunchInfo.mLaunchType)
    {
    case RTC_LAUNCH_TYPE_PUSH_C:
        params.push_back(PUSH_ARGUMENT);
        break;

    case RTC_LAUNCH_TYPE_AUTO_INVOCATION_C:
        params.push_back(AUTO_INVOCATION_ARGUMENT);
        break;

    case RTC_LAUNCH_TYPE_BACKGROUND_C:
        params.push_back(BG_START_ARGUMENT);
        break;

    case RTC_LAUNCH_TYPE_NORMAL_C: // fall through
    default:
        break;

    }

    mPid = mCore->getPmc()->launch(params, 0);

    if (mPid > 0)
    {
        launchSuccess = true;
        startTimer(LAUNCH_TIMER_TIMEOUT);
    }

    return launchSuccess;
}

int Test2Runtime::pid() const
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    return mPid;
}

Uid Test2Runtime::uid() const
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    return mUid;
}

bool Test2Runtime::isRunning() const
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    return mRunning;
}

bool Test2Runtime::terminate(const rtcTerminateInfo& /*aTerminateInfo*/)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    bool alreadyTerminated = false;

    CommsMessage msg;
    setTerminateApplicationReqParams(msg, mUid, RTC_TERMINATE_OPTIONS_NONE_C);
    msg.setReceiver(mRuntimeAddress);

    if (0 == mCore->getComms()->send(msg))
    {
        startTimer(EXIT_TIMER_TIMEOUT);
        alreadyTerminated = false;
        mExiting = true;
    }
    else
    {
        if (mPid > 0 && mCore)
        {
            mCore->getPmc()->kill(mPid);
            alreadyTerminated = false;
            mExiting = true;
        }
        else
        {
            alreadyTerminated = true;
        }
    }

    return alreadyTerminated;
}

// ProcessManagementEventsInterface
void Test2Runtime::pmcTerminated(const int& /*aPid*/, const int& /*exitCode*/)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    stopTimer();

    mPid = -1;
    mRuntimeAddress = 0;
    mRunning = false;
    mExiting = false;
}

void Test2Runtime::runningInd(const int& aRuntimeAddress, const int& /*aStatus*/)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    mRuntimeAddress = aRuntimeAddress;

    if (!mExiting)
    {
        stopTimer();
        mRunning = true;
    }
}

void Test2Runtime::terminatedInd(const int& /*aStatus*/)
{
    if (!mExiting)
    {
        startTimer(EXIT_TIMER_TIMEOUT);
    }
}

// CommsListener
void Test2Runtime::processMessage(CommsMessage& aMessage)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    switch (aMessage.getMessageId())
    {
    default:
        ELOG1(EJavaCaptain, "Unknown message forwarded to Test2Runtime %d", aMessage.getMessageId());

    }
}

void Test2Runtime::startTimer(const int& aTimeout)
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
void Test2Runtime::stopTimer()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    if (0 != mTimerId)
    {
        mCore->getTimerServer()->timerCancel(mTimerId);
        mTimerId = 0;
    }
}


// TimerServerEventsInterface methods
void Test2Runtime::timerTimeout(const int& aTimerId)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    if (aTimerId == mTimerId)
    {
        mTimerId = 0;
        if (mPid > 0 && mCore)
        {
            mCore->getPmc()->kill(mPid);
        }
    }
    else
    {
        LOG1WSTR(EJavaCaptain, EError, "timerTimeout(%s)", mUid.toString());
        ELOG2(EJavaCaptain, "Wrong timerId! ok:%d FAIL:%d", mTimerId, aTimerId);
    }
}

} // namespace captain
} // namespace java

