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

#include "TestHarness.h"

#include "commsserverendpoint.h"
#include "commsclientendpoint.h"
#include "commslistener.h"
#include "comms.h"
#include "echoserver.h"

using namespace java::comms;

class DummyListener : public CommsListener
{
    virtual void processMessage(CommsMessage& /*message*/) {}
};

TEST_GROUP(RegisterListener)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

/**
 * Test register listener for client endpoint
 * 1. register listener
 * 2. register default listener
 * 3. register multiple listeners
 * 4. listener already exists
 * 5. listener doesn't exists
 */

TEST(RegisterListener, client)
{
    CommsClientEndpoint client;
    DummyListener listener;

    // 1. register listener
    CHECK(!client.registerListener(1, &listener));
    CHECK(!client.unregisterListener(1, &listener));

    // 2. register default listener
    CHECK(!client.registerDefaultListener(&listener));
    CHECK(!client.unregisterDefaultListener(&listener));

    // 3. register multiple listeners
    int i = 0;
    for (; i < 200; i++)
    {
        CHECK(!client.registerListener(i, &listener));
    }

    for (i--; i > -1; i--)
    {
        CHECK(!client.unregisterListener(i, &listener));
    }

    // 4. listener already exists
    CHECK(!client.registerListener(1, &listener));
    CHECK(client.registerListener(1, &listener) != 0);
    CHECK(!client.unregisterListener(1, &listener));

    CHECK(!client.registerDefaultListener(&listener));
    CHECK(client.registerDefaultListener(&listener) != 0);
    CHECK(!client.unregisterDefaultListener(&listener));

    //  5. listener doesn't exists
    CHECK(client.unregisterListener(1, &listener) != 0);
    CHECK(client.unregisterDefaultListener(&listener) != 0);
}

/**
 * Test register listener for server endpoint
 * 1. register listener
 * 2. register default listener
 * 3. register multiple listeners
 * 4. listener already exists
 */
TEST(RegisterListener, server)
{
    CommsServerEndpoint server;
    DummyListener listener;

    // 1. register listener
    CHECK(!server.registerListener(1, &listener));
    CHECK(!server.unregisterListener(1, &listener));

    // 2. register default listener
    CHECK(!server.registerDefaultListener(&listener));
    CHECK(!server.unregisterDefaultListener(&listener));

    // 3. register multiple listeners
    int i = 0;
    for (; i < 200; i++)
    {
        CHECK(!server.registerListener(i, &listener));
    }

    for (i--; i > -1; i--)
    {
        CHECK(!server.unregisterListener(i, &listener));
    }

    // 4. listener already exists
    CHECK(!server.registerListener(1, &listener));
    CHECK(server.registerListener(1, &listener) != 0);
    CHECK(!server.unregisterListener(1, &listener));

    CHECK(!server.registerDefaultListener(&listener));
    CHECK(server.registerDefaultListener(&listener) != 0);
    CHECK(!server.unregisterDefaultListener(&listener));

    //  5. listener doesn't exists
    CHECK(server.unregisterListener(1, &listener) != 0);
    CHECK(server.unregisterDefaultListener(&listener) != 0);
}


/**
 * Test not unregistering listeners
 * 1. register listeners
 */
TEST(RegisterListener, noUnregister)
{
    DummyListener listener;

    CommsServerEndpoint server;
    CommsClientEndpoint client;

    CHECK(!server.start(IPC_ADDRESS_COMMS_MODULE_TEST));
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST));

    // 1. register listener
    CHECK(!server.registerDefaultListener(&listener));
    CHECK(!client.registerDefaultListener(&listener));
    for (int i=0; i < 200; i++)
    {
        CHECK(!server.registerListener(i, &listener));
        CHECK(!client.registerListener(i, &listener));
    }

    CHECK(!client.disconnect());
    CHECK(!server.stop());
}
