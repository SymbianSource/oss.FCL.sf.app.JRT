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
* Description:  RtcInterface
*
*/

#ifndef RTCINTERFACE_H
#define RTCINTERFACE_H

#include <string>

#include "javauid.h"
#include "commsmessage.h"

#include "rtcmessages.h" // RTC_LAUNCH_TYPE consts

using java::util::Uid;
using java::comms::CommsMessage;

namespace java
{
namespace captain
{

class rtcLaunchInfo
{
public:
    rtcLaunchInfo(const Uid& aUID,
                  const int& aLaunchType = RTC_LAUNCH_TYPE_NORMAL_C,
                  std::string aRtcType = "midp",
                  std::wstring aApplicationArguments = L"",
                  std::wstring aRuntimeArguments = L"")
            :mUID(aUID), mLaunchType(aLaunchType), mRtcType(aRtcType),
            mApplicationArguments(aApplicationArguments),
            mRuntimeArguments(aRuntimeArguments)
    {}

public:
    const Uid           mUID;
    const int           mLaunchType;
    const std::string   mRtcType;
    const std::wstring  mApplicationArguments;
    const std::wstring  mRuntimeArguments;
};

class rtcTerminateInfo
{
public:
    rtcTerminateInfo(const Uid& aUID):mUID(aUID)
    {}

    Uid mUID;
};

class RtcInterface
{
public:
    virtual ~RtcInterface() {}

    virtual bool launch(const rtcLaunchInfo& aLaunchInfo) = 0;
    virtual bool terminate(const rtcTerminateInfo& aTerminateInfo) = 0;

    virtual bool enable(const Uid& aUID) = 0;
    virtual bool disable(const Uid& aUID) = 0;

    virtual void setGlobalRuntimeArguments(const std::wstring& aArguments) = 0;
    virtual std::wstring getGlobalRuntimeArguments() = 0;

    virtual void setGlobalApplicationArguments(const std::wstring& aArguments) = 0;
    virtual std::wstring getGlobalApplicationArguments() = 0;
};

} // namespace captain
} // namespace java

#endif // RTCINTERFACE_H

