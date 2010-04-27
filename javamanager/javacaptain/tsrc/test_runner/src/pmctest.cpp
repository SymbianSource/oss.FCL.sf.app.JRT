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
* Description:  Pmctest
*
*/

#include "TestHarness.h"

#include "commsclientendpoint.h"
#include "comms.h"

#include "testmessages.h"

using namespace java::comms;
using namespace java::captain;

extern java::comms::CommsClientEndpoint* commsClient;

static const int sendReceiveTimeout = 10;

TEST_GROUP(Pmc)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

TEST(Pmc, PMC04_Kill)
{
    CommsMessage msg;
    setProcessKillParameters(msg, 100);
    CHECK(commsClient->send(msg) == 0);
}

TEST(Pmc, PMC03_Terminate)
{
    CommsMessage msg;
    setProcessTerminateParameters(msg, 100);
    CHECK(commsClient->send(msg) == 0);
}

TEST(Pmc, PMC02_LaunchNonExisting)
{
    CommsMessage msg;
    std::string exe("javadestroyer"), params("GiveMeAll");

    msg.setModuleId(PLUGIN_ID_JAVACAPTAIN_TESTER_C);
    setProcessStartParameters(msg, exe, params);

    CommsMessage receivedMsg;
    CHECK(commsClient->sendReceive(msg, receivedMsg, sendReceiveTimeout) == 0);

    CHECK(receivedMsg.getMessageId() == IPC_MESSAGE_TEST_ACK);
    CHECK(msg.getMessageRef() == receivedMsg.getMessageRef());

    int status;
    getAckParameters(receivedMsg, status);
#ifdef __SYMBIAN32__
    CHECK(status < 1); // pid < 1 == launch failed
#else
    CHECK(status > 0); // Linux: fork() always succeeds
#endif
}

TEST(Pmc, PMC01_LaunchExisting)
{
    CommsMessage msg;
    std::string exe("javainstaller"), params("list");

    msg.setModuleId(PLUGIN_ID_JAVACAPTAIN_TESTER_C);
    setProcessStartParameters(msg, exe, params);

    CommsMessage receivedMsg;
    CHECK(commsClient->sendReceive(msg, receivedMsg, sendReceiveTimeout) == 0);

    CHECK(receivedMsg.getMessageId() == IPC_MESSAGE_TEST_ACK);
    CHECK(msg.getMessageRef() == receivedMsg.getMessageRef());

    int status;
    getAckParameters(receivedMsg, status);
    CHECK(status > 0); // in Linux fork() always succeeds
}

