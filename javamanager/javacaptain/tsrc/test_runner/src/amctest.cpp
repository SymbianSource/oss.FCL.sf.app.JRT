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

TEST_GROUP(Amc)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

TEST(Amc, AMC08_Update)
{
    CommsMessage msg;
    int numOfUids = 1, noUids = 0;

    // Delete
    msg.setModuleId(PLUGIN_ID_AMC_C);
    setAmcRequestParams(msg, AMC_REQUEST_OPERATION_UPDATE, AMC_REQUEST_OPTIONS_NONE);
    msg << numOfUids << uid_normal1;  // before
    msg << noUids;                      // after;
    CHECK(commsClient->send(msg) == 0);
    // Delete empty
    msg.reset();
    msg.setModuleId(PLUGIN_ID_AMC_C);
    setAmcRequestParams(msg, AMC_REQUEST_OPERATION_UPDATE, AMC_REQUEST_OPTIONS_NONE);
    msg << noUids;  // before
    msg << noUids;  // after;
    CHECK(commsClient->send(msg) == 0);

    // Add
    msg.reset();
    msg.setModuleId(PLUGIN_ID_AMC_C);
    setAmcRequestParams(msg, AMC_REQUEST_OPERATION_UPDATE, AMC_REQUEST_OPTIONS_NONE);
    msg << noUids;                      // before UIDs
    msg << numOfUids << uid_normal1;  // After UIDs
    CHECK(commsClient->send(msg) == 0);
    // Add Empty
    msg.reset();
    msg.setModuleId(PLUGIN_ID_AMC_C);
    setAmcRequestParams(msg, AMC_REQUEST_OPERATION_UPDATE, AMC_REQUEST_OPTIONS_NONE);
    msg << noUids;  // before UIDs
    msg << noUids;  // After UIDs
    CHECK(commsClient->send(msg) == 0);

    // Update
    msg.reset();
    msg.setModuleId(PLUGIN_ID_AMC_C);
    setAmcRequestParams(msg, AMC_REQUEST_OPERATION_UPDATE, AMC_REQUEST_OPTIONS_NONE);
    msg << numOfUids << uid_normal1; // before
    msg << numOfUids << uid_normal1; // after
    CHECK(commsClient->send(msg) == 0);
    // Update Empty
    msg.reset();
    msg.setModuleId(PLUGIN_ID_AMC_C);
    setAmcRequestParams(msg, AMC_REQUEST_OPERATION_UPDATE, AMC_REQUEST_OPTIONS_NONE);
    msg << noUids; // before
    msg << noUids; // after
    CHECK(commsClient->send(msg) == 0);
}

TEST(Amc, AMC02_06_StopRunning)
{
    iTerminate(uid_push1, syncOff);

    // Update (add)
    CommsMessage msg;
    msg.setModuleId(PLUGIN_ID_AMC_C);
    setAmcRequestParams(msg, AMC_REQUEST_OPERATION_UPDATE, AMC_REQUEST_OPTIONS_NONE);
    msg << 0;               // before
    msg << 1 << uid_push1;  // after
    CHECK(commsClient->send(msg) == 0);

    // Stop
    msg.reset();
    msg.setModuleId(PLUGIN_ID_AMC_C);
    setAmcRequestParams(msg, AMC_REQUEST_OPERATION_STOP, AMC_REQUEST_OPTIONS_NONE);
    msg << 1 << uid_push1;

    CommsMessage receivedMsg;
    CHECK(commsClient->sendReceive(msg, receivedMsg, sendReceiveTimeout) == 0);

    CHECK(msg.getModuleId() == receivedMsg.getModuleId());
    CHECK(receivedMsg.getMessageId() == AMC_MSG_ID_RESPONSE);
    CHECK(msg.getMessageRef() == receivedMsg.getMessageRef());

    // Update (delete)
    msg.reset();
    msg.setModuleId(PLUGIN_ID_AMC_C);
    setAmcRequestParams(msg, AMC_REQUEST_OPERATION_UPDATE, AMC_REQUEST_OPTIONS_NONE);
    msg << 1 << uid_push1; // before
    msg << 0; // after
    CHECK(commsClient->send(msg) == 0);
}

TEST(Amc, AMC02_05_StopRunning)
{
    iLaunch(uid_normal1, syncOn, "test1");

    CommsMessage msg;
    msg.setModuleId(PLUGIN_ID_AMC_C);
    setAmcRequestParams(msg, AMC_REQUEST_OPERATION_STOP, AMC_REQUEST_OPTIONS_NONE);

    int numOfUids = 1;
    msg << numOfUids << uid_normal1;

    CommsMessage receivedMsg;
    CHECK(commsClient->sendReceive(msg, receivedMsg, sendReceiveTimeout) == 0);

    CHECK(msg.getModuleId() == receivedMsg.getModuleId());
    CHECK(receivedMsg.getMessageId() == AMC_MSG_ID_RESPONSE);
    CHECK(msg.getMessageRef() == receivedMsg.getMessageRef());

    // Update
    msg.reset();
    msg.setModuleId(PLUGIN_ID_AMC_C);
    setAmcRequestParams(msg, AMC_REQUEST_OPERATION_UPDATE, AMC_REQUEST_OPTIONS_NONE);
    msg << numOfUids << uid_normal1; // before
    msg << numOfUids << uid_normal1; // after
    CHECK(commsClient->send(msg) == 0);
}

TEST(Amc, AMC02_04_StopEmpty)
{
    CommsMessage msg;
    msg.setModuleId(PLUGIN_ID_AMC_C);
    setAmcRequestParams(msg, AMC_REQUEST_OPERATION_STOP, AMC_REQUEST_OPTIONS_NONE);

    int numOfUids = 0;
    msg << numOfUids;

    CommsMessage receivedMsg;
    CHECK(commsClient->sendReceive(msg, receivedMsg, sendReceiveTimeout) == 0);

    CHECK(msg.getModuleId() == receivedMsg.getModuleId());
    CHECK(receivedMsg.getMessageId() == AMC_MSG_ID_RESPONSE);
    CHECK(msg.getMessageRef() == receivedMsg.getMessageRef());
}

TEST(Amc, AMC02_03_StopBoth)
{
    CommsMessage msg;
    msg.setModuleId(PLUGIN_ID_AMC_C);
    setAmcRequestParams(msg, AMC_REQUEST_OPERATION_STOP, AMC_REQUEST_OPTIONS_NONE);

    int numOfUids = 2;
    msg << numOfUids << uid_normal1 << uid_nonexistant;

    CommsMessage receivedMsg;
    CHECK(commsClient->sendReceive(msg, receivedMsg, sendReceiveTimeout) == 0);

    CHECK(msg.getModuleId() == receivedMsg.getModuleId());
    CHECK(receivedMsg.getMessageId() == AMC_MSG_ID_RESPONSE);
    CHECK(msg.getMessageRef() == receivedMsg.getMessageRef());
}

TEST(Amc, AMC02_02_StopNonExisting)
{
    CommsMessage msg;
    msg.setModuleId(PLUGIN_ID_AMC_C);
    setAmcRequestParams(msg, AMC_REQUEST_OPERATION_STOP, AMC_REQUEST_OPTIONS_NONE);

    int numOfUids = 1;
    msg << numOfUids << uid_nonexistant;

    CommsMessage receivedMsg;
    CHECK(commsClient->sendReceive(msg, receivedMsg, sendReceiveTimeout) == 0);

    CHECK(msg.getModuleId() == receivedMsg.getModuleId());
    CHECK(receivedMsg.getMessageId() == AMC_MSG_ID_RESPONSE);
    CHECK(msg.getMessageRef() == receivedMsg.getMessageRef());
}

TEST(Amc, AMC02_01_StopExisting)
{
    CommsMessage msg;
    msg.setModuleId(PLUGIN_ID_AMC_C);
    setAmcRequestParams(msg, AMC_REQUEST_OPERATION_STOP, AMC_REQUEST_OPTIONS_NONE);

    int numOfUids = 1;
    msg << numOfUids << uid_normal1;

    CommsMessage receivedMsg;
    CHECK(commsClient->sendReceive(msg, receivedMsg, sendReceiveTimeout) == 0);

    CHECK(msg.getModuleId() == receivedMsg.getModuleId());
    CHECK(receivedMsg.getMessageId() == AMC_MSG_ID_RESPONSE);
    CHECK(msg.getMessageRef() == receivedMsg.getMessageRef());
}

TEST(Amc, AMC01_03_StartEmpty)
{
    CommsMessage msg;
    msg.setModuleId(PLUGIN_ID_AMC_C);
    setAmcRequestParams(msg, AMC_REQUEST_OPERATION_START, AMC_REQUEST_OPTIONS_NONE);

    int numOfUids = 0;
    msg << numOfUids;

    CHECK(commsClient->send(msg) == 0);
}

TEST(Amc, AMC01_02_StartNonExisting)
{
    CommsMessage msg;
    msg.setModuleId(PLUGIN_ID_AMC_C);
    setAmcRequestParams(msg, AMC_REQUEST_OPERATION_START, AMC_REQUEST_OPTIONS_NONE);

    int numOfUids = 1;
    msg << numOfUids << uid_nonexistant;

    CHECK(commsClient->send(msg) == 0);
}

TEST(Amc, AMC01_01_StartExisting)
{
    CommsMessage msg;
    msg.setModuleId(PLUGIN_ID_AMC_C);
    setAmcRequestParams(msg, AMC_REQUEST_OPERATION_START, AMC_REQUEST_OPTIONS_NONE);

    int numOfUids = 1;
    msg << numOfUids << uid_normal1;

    CHECK(commsClient->send(msg) == 0);
}

