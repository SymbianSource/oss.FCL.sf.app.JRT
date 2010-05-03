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

#ifndef TEST2RUNTIME_H
#define TEST2RUNTIME_H

#include "javaosheaders.h"

#include "javauid.h"

#include "rtcruntimeinterface.h"
#include "timerserverinterface.h"

namespace java
{
namespace captain
{

using java::comms::CommsListener;
using java::comms::CommsMessage;
using java::util::Uid;

class CoreInterface;
class ApplicationRuntimeEventsInterface;

OS_NONSHARABLE_CLASS(Test2Runtime) : public RtcRuntimeInterface,
        public TimerServerEventsInterface
{
public:
    Test2Runtime(const Uid& aUID, CoreInterface* aCore);
    virtual ~Test2Runtime();

    // RtcRuntimeInterface
    virtual bool launch(const rtcLaunchInfo& aLaunchInfo);
    virtual int pid() const;
    virtual Uid uid() const;
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

    // CommsListener
    virtual void processMessage(CommsMessage& aMessage);

    // TimerServerEventsInterface methods
    virtual void timerTimeout(const int& aTimerId);

private:
    void startTimer(const int& timeout);
    void stopTimer();

    Uid             mUid;
    CoreInterface*  mCore;
    int             mPid;
    bool            mRunning;
    bool            mExiting;
    int             mRuntimeAddress;
    int             mTimerId;
};

} // namespace captain
} // namespace java

#endif // TEST2RUNTIME_H

