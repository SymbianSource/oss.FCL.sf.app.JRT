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
* Description:  RtcRuntimeInterface
*
*/

#ifndef RTCRUNTIMEINTERFACE_H
#define RTCRUNTIMEINTERFACE_H

#include "rtcinterface.h"
#include "processmanagementeventsinterface.h"
#include "commslistener.h"
#include "javauid.h"

namespace java
{
namespace captain
{

using java::comms::CommsListener;
using java::comms::CommsMessage;

class RtcRuntimeInterface : public ProcessManagementEventsInterface,
        public CommsListener
{
public:
    virtual ~RtcRuntimeInterface() {}

    // returns true on success, false on launch failure
    virtual bool launch(const rtcLaunchInfo& aLaunchInfo) = 0;
    virtual Uid uid() const = 0;
    virtual int pid() const = 0;
    virtual bool isRunning() const = 0;
    virtual int runtimeAddress() const = 0;

    // returns true if already terminated, false of termination is just started
    virtual bool terminate(const rtcTerminateInfo& aTerminateInfo) = 0;

    virtual void runningInd(const int& aRuntimeAddress, const int& aStatus) = 0;
    virtual void terminatedInd(const int& aStatus) = 0;

    // ProcessManagementEventsInterface
    virtual void pmcTerminated(const int& pid, const int& exitCode) = 0;

    // CommsListener
    virtual void processMessage(CommsMessage& aMessage) = 0;
};

} // namespace captain
} // namespace java

#endif // RTCRUNTIMEINTERFACE_H

