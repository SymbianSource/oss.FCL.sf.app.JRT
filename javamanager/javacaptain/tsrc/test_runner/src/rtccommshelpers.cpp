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
* Description:  rtccommshelpers
*
*/


#include "TestHarness.h"

#include "commsmessage.h"

#include "rtcmessages.h"

#include "testmessages.h"
#include "rtccommshelpers.h"

using java::comms::CommsMessage;
using namespace java::captain;

void launch(const Uid& aUid, const bool& sync, std::string rtc,
            const int& type, const bool& allowFail)
{
// Launch start
    CommsMessage msg;
    setLaunchApplicationReqParams(msg, aUid, type,
                                  sync ? RTC_LAUNCH_OPTIONS_RUNNING_IND_REQ_C : RTC_LAUNCH_OPTIONS_NONE_C,
                                  rtc);

    if (sync)
    {
        CommsMessage receivedMsg;
        CHECK(commsClient->sendReceive(msg, receivedMsg, sendReceiveTimeout) == 0);

        CHECK(msg.getModuleId() == receivedMsg.getModuleId());
        CHECK(receivedMsg.getMessageId() == RTC_MSG_ID_APPLICATION_RUNNING_IND); //|| receivedMsg.getMessageId() == RTC_MSG_ID_APPLICATION_TERMINATED_IND);
        CHECK(msg.getMessageRef() == receivedMsg.getMessageRef());

        Uid uid;
        int status= -1;
        if (receivedMsg.getMessageId() == RTC_MSG_ID_APPLICATION_RUNNING_IND)
        {
            getApplicationRunningIndParams(receivedMsg, uid, status);
        }
//        else if ( receivedMsg.getMessageId() == RTC_MSG_ID_APPLICATION_TERMINATED_IND )
//        {
//            getApplicationTerminatedIndParams(receivedMsg, uid, status);
//        }
        CHECK(0 == status || (allowFail && (-1==status)));   // : false)); //|| -1 == status);
        CHECK(uid == aUid);
    }
    else
    {
        CHECK(commsClient->send(msg) == 0);
    }
// Launch end
}

void terminate(const Uid& aUid, const bool& sync)
{
    // To make sure that the application is not running
    CommsMessage msg;
    setTerminateApplicationReqParams(msg, aUid,
                                     sync ? RTC_TERMINATE_OPTIONS_TERMINATE_IND_REQ_C : RTC_TERMINATE_OPTIONS_NONE_C);

    if (sync)
    {
        CommsMessage receivedMsg;
        CHECK(commsClient->sendReceive(msg, receivedMsg, sendReceiveTimeout) == 0);

        CHECK(msg.getModuleId() == receivedMsg.getModuleId());
        CHECK(receivedMsg.getMessageId() == RTC_MSG_ID_APPLICATION_TERMINATED_IND);
        CHECK(msg.getMessageRef() == receivedMsg.getMessageRef());

        Uid uid;
        int status = 0;
        getApplicationTerminatedIndParams(receivedMsg, uid, status);
        CHECK(!status);
        CHECK(uid == aUid);
    }
    else
    {
        CHECK(commsClient->send(msg) == 0);
    }
}

void disable(const Uid& aUid)
{
    CommsMessage msg;
    setDisableAppParameters(msg, aUid);
    CHECK(commsClient->send(msg) == 0);
}

void enable(const Uid& aUid)
{
    CommsMessage msg;
    setEnableAppParameters(msg, aUid);
    CHECK(commsClient->send(msg) == 0);
}

