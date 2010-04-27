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
* Description:  Amctest
*
*/

#include "TestHarness.h"

#include "commsclientendpoint.h"
#include "comms.h"
#include "javauid.h"

#include "rtcinterfacehelpers.h"
#include "amcmessages.h"
#include "testuids.h"


using namespace java::comms;
using namespace java::captain;
using namespace java::util;

using java::util::Uid;

static Uid uid_nonexistant;

extern java::comms::CommsClientEndpoint* commsClient;

//static const int sendReceiveTimeout = 10;

TEST_GROUP(Custom)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

TEST(Custom, C01_StopPush)
{
    iLaunch(uid_push1, syncOff, "test1");
//    iTerminate(uid_push1, syncOn);
//    terminate(uid_push1, syncOff);
//    launch(uid_push1, syncOff, "test1");

    CommsMessage msg;
    msg.setModuleId(PLUGIN_ID_AMC_C);
    setAmcRequestParams(msg, AMC_REQUEST_OPERATION_STOP, AMC_REQUEST_OPTIONS_NONE);

    int numOfUids = 1;
    msg << numOfUids << uid_push1;

    CommsMessage receivedMsg;
    CHECK(commsClient->sendReceive(msg, receivedMsg, sendReceiveTimeout) == 0);

    CHECK(msg.getModuleId() == receivedMsg.getModuleId());
    CHECK(receivedMsg.getMessageId() == AMC_MSG_ID_RESPONSE);
    CHECK(msg.getMessageRef() == receivedMsg.getMessageRef());

}

