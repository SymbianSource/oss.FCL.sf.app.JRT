/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: AppLauncher can be used to start and stop Java application.
*
*/

#include "applauncher.h"

#include "logger.h"
#include "javauid.h"
#include "rtcmessages.h"
#include "comms.h"
#include "commsmessage.h"
#include "commsclientendpoint.h"

using namespace java::debug;
using namespace java::util;
using namespace java::comms;
using namespace java::captain;

AppLauncher::AppLauncher(const java::util::Uid& aAppUid): mUid(aAppUid), mPid(0)
{
}

AppLauncher::~AppLauncher()
{
}

int AppLauncher::startApp(const std::wstring& aJvmArgs)
{
    JELOG2(EDebugApi);

    CommsMessage start = createStartAppMessage(aJvmArgs);
    int rc = sendToJavaCaptain(start);

    LOG3(EDebugApi, EInfo, "startApp(): uid=%S, result=%d, pid=%d", mUid.toString().c_str(), rc, mPid);
    return rc;
}

int AppLauncher::stopApp()
{
    JELOG2(EDebugApi);

    CommsMessage stop = createStopAppMessage();
    int rc = sendToJavaCaptain(stop);

    LOG2(EDebugApi, EInfo, "stopApp(): uid=%S, result=%d", mUid.toString().c_str(), rc);
    return rc;
}

CommsMessage AppLauncher::createStartAppMessage(const std::wstring& aJvmArgs)
{
    CommsMessage msg;
    setLaunchApplicationReqParams(msg, mUid,
                                  RTC_LAUNCH_TYPE_DEBUG_C,
                                  RTC_LAUNCH_OPTIONS_RUNNING_IND_REQ_C,
                                  RTC_LAUNCH_RUNTIME_MIDP_C,
                                  L"", aJvmArgs);
    return msg;
}

CommsMessage AppLauncher::createStopAppMessage()
{
    CommsMessage msg;
    setTerminateApplicationReqParams(msg, mUid,
                                     RTC_LAUNCH_OPTIONS_TERMINATE_IND_REQ_C);
    return msg;
}

int AppLauncher::sendToJavaCaptain(CommsMessage& aMessage)
{
    CommsClientEndpoint comms;
    comms.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);

    CommsMessage reply;
    int rc = comms.sendReceive(aMessage, reply, WAIT_FOR_EVER);
    if (!rc)
    {
        Uid ignored;
        reply >> ignored >> rc >> mPid;
    }

    comms.disconnect();
    return rc;
}

pid_t AppLauncher::getPid()
{
    return mPid;
}
