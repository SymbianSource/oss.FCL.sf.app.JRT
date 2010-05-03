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

#ifndef RTCBASEPLUGIN_H
#define RTCBASEPLUGIN_H

#include <list>

#include "scopedlocks.h"
#include "commsmessage.h"

#include "rtcinterface.h"
#include "timerserverinterface.h"
#include "rtcruntimeinterface.h"
#include "pmcinterface.h"

using java::comms::CommsMessage;

namespace java
{
namespace captain
{

class CoreInterface;
class ApplicationRuntimeEventsInterface;

class RtcBasePlugin : public RtcRuntimeInterface,
        public TimerServerEventsInterface
{
public:
    RtcBasePlugin(const Uid& aUID, CoreInterface* aCore);
    virtual ~RtcBasePlugin();

    virtual bool launch(const rtcLaunchInfo& aLaunchInfo);
    virtual int  pid() const
    {
        return mPid;
    }
    virtual Uid uid() const
    {
        return mUID;
    }
    virtual bool isRunning() const;
    virtual int runtimeAddress() const
    {
        return mRuntimeAddress;
    }

    virtual bool terminate(const rtcTerminateInfo& aTerminateInfo);

    virtual void runningInd(const int& aRuntimeAddress, const int& aStatus);
    virtual void terminatedInd(const int& aStatus);

    // ProcessManagementEventsInterface
    virtual void pmcTerminated(const int& pid, const int& exitCode);

    // TimerServerEventsInterface methods
    virtual void timerTimeout(const int& aTimerId);

    // CommsListener, called by Host Rtc not by comms
    virtual void processMessage(CommsMessage& message);

protected:
    enum ApplicationState
    {
        APPLICATION_STATE_IDLE_C,
        APPLICATION_STATE_LAUNCHED_C,
        APPLICATION_STATE_RUNNING_C,
        APPLICATION_STATE_EXITING_C
    };
    const char* E2S_state(const RtcBasePlugin::ApplicationState& state)
    {
        switch (state)
        {
        case APPLICATION_STATE_IDLE_C:
            return "IDLE_C";
        case APPLICATION_STATE_LAUNCHED_C:
            return "LAUNCHED_C";
        case APPLICATION_STATE_RUNNING_C:
            return "RUNNING_C";
        case APPLICATION_STATE_EXITING_C:
            return "EXITING_C";
        }

        return "UKNOWN!";
    }

    virtual void switchState(const ApplicationState& newState);

    virtual void runtime_init();
    virtual bool runtime_launch(const int& aLaunchType);
    virtual bool runtime_relaunch(const int& aLaunchType);
    virtual bool runtime_terminate();

    // Currently only platform/runtime specific part
    virtual cmdLine_t& generateCommandLine(const int& aLaunchType,
                                           cmdLine_t& aParams) = 0;

    int sendMessageToRuntime(const int& aModuleId, CommsMessage& aMessage);

    void startTimer(const int& timeout);
    void stopTimer();

    enum PendingOperation
    {
        PENDING_OPERATION_NONE_C,
        PENDING_OPERATION_LAUNCH_NORMAL_C,
        PENDING_OPERATION_LAUNCH_PUSH_C,
        PENDING_OPERATION_LAUNCH_AUTO_INVOCATION_C,
        PENDING_OPERATION_LAUNCH_BACKGROUND_C,
        PENDING_OPERATION_LAUNCH_DEBUG_C,
        PENDING_OPERATION_TERMINATE_C
    };

    void setPendingLaunch(const int& aType);
    void setPendingTerminate();

    void handlePendingOperations();

    Uid                                 mUID;
    CoreInterface*                      mCore;
    ApplicationState                    mState;
    int                                 mRuntimeAddress;
    int                                 mPid;
    int                                 mTimerId;
    PendingOperation                    mPendingOperation;
    std::wstring                        mRuntimeArguments;
    std::wstring                        mApplicationArguments;
};

} // namespace captain
} // namespace java

#endif // RTCBASEPLUGIN_H

