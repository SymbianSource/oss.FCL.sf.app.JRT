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
* Description:  ?Description
*
*/

#include <errno.h>
#include "monitor.h"
#include "TestHarness.h"
#include "logger.h"

#include "commsserverendpoint.h"
#include "commsclientendpoint.h"
#include "commsmessage.h"
#include "echoserver.h"

using namespace java::comms;
using java::util::Monitor;

TEST_GROUP(Find)
{
    TEST_SETUP()
    {
        {
            CommsClientEndpoint client;
        }
    }

    TEST_TEARDOWN()
    {
    }
};

/**
 * Test find client endpoint
 * 1. Named endpoint exists
 * 2. Named endpoint does not exist
 * 3. Multiple named endpoints
 */

TEST(Find, FindClient)
{
    EchoServer server;
    CHECK(!server.start(IPC_ADDRESS_COMMS_MODULE_TEST));

    CommsMessage msg;
    CommsMessage receivedMsg;

    // 1. Named endpoint exists
    CommsClientEndpoint client(L"CommsModuleTest");
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST));

    CommsClientEndpoint* pClient = CommsClientEndpoint::find(L"CommsModuleTest");
    CHECK(pClient);

    CHECK(!client.send(msg));
    CHECK(!pClient->send(msg));

    CHECK(!pClient->disconnect());
    CHECK(!pClient->connect(IPC_ADDRESS_COMMS_MODULE_TEST));

    CHECK(!client.sendReceive(msg, receivedMsg, WAIT_FOR_EVER));
    CHECK(!pClient->sendReceive(msg, receivedMsg, WAIT_FOR_EVER));

    CHECK(!client.disconnect());

    // 2. Named endpoint does not exist
    pClient = CommsClientEndpoint::find(L"commsmoduletest");
    CHECK(!pClient);

    pClient = CommsClientEndpoint::find(L"");
    CHECK(!pClient);

    pClient = CommsClientEndpoint::find(L"CommsModuleTest ");
    CHECK(!pClient);

    {
        CommsClientEndpoint cli(L"clientendpoint");
        pClient = CommsClientEndpoint::find(L"clientendpoint");
        CHECK(pClient);
    }
    pClient = CommsClientEndpoint::find(L"clientendpoint");
    CHECK(!pClient);

    // 3. Multiple named endpoints
    CommsClientEndpoint client1(L"client1");
    CommsClientEndpoint client2(L"client2");
    CommsClientEndpoint client3(L"client3");

    pClient = CommsClientEndpoint::find(L"client1");
    CHECK(pClient);
    pClient = CommsClientEndpoint::find(L"client2");
    CHECK(pClient);
    pClient = CommsClientEndpoint::find(L"client3");
    CHECK(pClient);

    CHECK(!server.stop());
}


/**
 * Test find server endpoint
 * 1. Named endpoint exists
 * 2. Named endpoint does not exist
 * 3. Multiple named endpoints
 */

TEST(Find, FindServer)
{
    CommsMessage msg;
    CommsMessage receivedMsg;

    CommsClientEndpoint client;

    // 1. Named endpoint exists
    CommsServerEndpoint server(L"CommsModuleTest");
    CHECK(!server.start(IPC_ADDRESS_COMMS_MODULE_TEST));
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST));

    CommsServerEndpoint* pServer = CommsServerEndpoint::find(L"CommsModuleTest");
    CHECK(pServer);

    CHECK(!client.send(msg));
    CHECK(!pServer->stop());

    CHECK(client.send(msg) != 0);
    CHECK(!client.disconnect());

    CHECK(!pServer->start(IPC_ADDRESS_COMMS_MODULE_TEST));
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST));

    CHECK(!client.send(msg));
    CHECK(!server.stop());

    CHECK(client.send(msg) != 0);
    CHECK(!client.disconnect());

    // 2. Named endpoint does not exist
    pServer = CommsServerEndpoint::find(L"commsmoduletest");
    CHECK(!pServer);

    pServer = CommsServerEndpoint::find(L"");
    CHECK(!pServer);

    pServer = CommsServerEndpoint::find(L"CommsModuleTest ");
    CHECK(!pServer);

    {
        CommsServerEndpoint cli(L"serverendpoint");
        pServer = CommsServerEndpoint::find(L"serverendpoint");
        CHECK(pServer);
    }
    pServer = CommsServerEndpoint::find(L"serverendpoint");
    CHECK(!pServer);

    // 3. Multiple named endpoints
    CommsServerEndpoint server1(L"server1");
    CommsServerEndpoint server2(L"server2");
    CommsServerEndpoint server3(L"server3");

    pServer = CommsServerEndpoint::find(L"server1");
    CHECK(pServer);
    pServer = CommsServerEndpoint::find(L"server2");
    CHECK(pServer);
    pServer = CommsServerEndpoint::find(L"server3");
    CHECK(pServer);
}


/**
 * Server and client endpoint with same name
 * 1. Server and client endpoint with same name
 */

TEST(Find, FindSameName)
{
#ifdef __SYMBIAN32__
    EXPECT_N_LEAKS(2);
#else
    EXPECT_N_LEAKS(1);
#endif
    // 1. Server and client endpoint with same name
    CommsServerEndpoint server(L"hello world");
    CommsClientEndpoint client(L"hello world");

    CommsServerEndpoint* pServer = CommsServerEndpoint::find(L"hello world");
    CHECK(pServer);
    CommsClientEndpoint* pClient = CommsClientEndpoint::find(L"hello world");
    CHECK(pClient);
}
