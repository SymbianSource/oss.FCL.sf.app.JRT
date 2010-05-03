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

TEST_GROUP(Connect)
{
    CommsServerEndpoint comms;

    TEST_SETUP()
    {
        comms.start(IPC_ADDRESS_COMMS_MODULE_TEST);
    }

    TEST_TEARDOWN()
    {
        comms.stop();
    }
};

/**
 * Test open connection
 * 1. Normal connect, disconnect
 * 2. Multiple connect calls without disconnect
 * 3. Disconnect before connect
 * 4. Connect to non-existing host
 */

TEST(Connect, OpenConnection)
{
    CommsClientEndpoint client;

    // 1. Normal connect, disconnect
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
    CHECK(!client.disconnect());

    // 2. Multiple connect calls without disconnect
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
    CHECK(!client.disconnect());

    // 3. Disconnect before connect
    CHECK(!client.disconnect());
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
    CHECK(!client.disconnect());

    // 4. Connect to non-existing host
    CHECK(client.connect(IPC_ADDRESS_COMMS_MODULE_TEST+666));
    CHECK(!client.disconnect());
}

/**
 * Test open connection with multiple clients
 * 1. Normal connect, disconnect
 * 2. Multiple connect/disconnects
 */

TEST(Connect, MultiClient)
{
    CommsClientEndpoint a, b, c, d, e;

    // 1. Normal connect, disconnect
    CHECK(!a.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
    CHECK(!b.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
    CHECK(!c.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
    CHECK(!d.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
    CHECK(!e.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
    CHECK(!e.disconnect());
    CHECK(!d.disconnect());
    CHECK(!c.disconnect());
    CHECK(!b.disconnect());
    CHECK(!a.disconnect());

    // 2. Multiple connect/disconnects
    for (int i=0; i<10; i++)
    {
        CHECK(!a.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
        CHECK(!b.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
        CHECK(!c.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
        CHECK(!d.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
        CHECK(!e.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
        CHECK(!e.disconnect());
        CHECK(!d.disconnect());
        CHECK(!c.disconnect());
        CHECK(!b.disconnect());
        CHECK(!a.disconnect());
    }
}

/**
 * Test server cases
 * 1. Server exits while client is connected
 * 2. Address is already in use
 * 3. Start/Connect using different addresses
 */

TEST(Connect, Server)
{
    CommsServerEndpoint server;
    CommsClientEndpoint client;
    int ADDR = IPC_ADDRESS_COMMS_MODULE_TEST + 1;

    // 1. Server exits while client is connected
    CHECK(!server.start(ADDR));
    CHECK(!client.connect(ADDR));
    CHECK(!server.stop());
    CHECK(!client.disconnect());

    // 2. Address is already in use
    CHECK(server.start(IPC_ADDRESS_COMMS_MODULE_TEST)!=0);
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
    CHECK(!server.stop());
    CHECK(!client.disconnect());

    // 3. Start/Connect using different addresses
    CHECK(!server.start(ADDR));
    CHECK(!client.connect(ADDR));
    CHECK(!client.disconnect());
    CHECK(!server.stop());

    CHECK(!server.start(ADDR+1));
    CHECK(!client.connect(ADDR+1));
    CHECK(!client.disconnect());
    CHECK(!server.stop());

}

/**
 * Test connect/disconnect from separate threads
 * 1. connect/disconnect from separate threads
 * 2. start/stop from separate threads
 */

void doDisconnect(CommsClientEndpoint* aClient)
{
    CHECK(!aClient->disconnect());
}
void* disconnectThread(void* aComms)
{
    CommsClientEndpoint* client = reinterpret_cast<CommsClientEndpoint*>(aComms);
    doDisconnect(client);
    return 0;
}

void doStop(CommsServerEndpoint* aServer)
{
    CHECK(!aServer->stop());
}
void* stopThread(void* aComms)
{
    CommsServerEndpoint* server = reinterpret_cast<CommsServerEndpoint*>(aComms);
    doStop(server);
    return 0;
}

TEST(Connect, separateThread)
{
    CommsServerEndpoint server;
    CommsClientEndpoint client;
    int ADDR = IPC_ADDRESS_COMMS_MODULE_TEST + 1;

    CommsMessage msg;

    // 1. connect/disconnect from separate threads
    CHECK(!server.start(ADDR));
    CHECK(!client.connect(ADDR));

    CHECK(client.send(msg) == 0);
    pthread_t threadId = 0;
    pthread_create(&threadId, 0, disconnectThread, &client);
    pthread_join(threadId, 0);
    CHECK(client.send(msg) != 0);

    // 2. start/stop from separate threads
    pthread_create(&threadId, 0, stopThread, &server);
    pthread_join(threadId, 0);

    CommsServerEndpoint server2;
    CHECK(!server2.start(ADDR));
    CHECK(!server2.stop());
}

/**
 * Check that onStart and onExit callbacks are done
 * (ensures that ipc provider implementation is done correctly)
 * 1. Callbacks are done after connect/disconnect
 * 2. Callbacks are done after start/stop
 */

class OnStartExitClient : public CommsClientEndpoint
{
public:
    OnStartExitClient() : mOnStart(false), mOnExit(false) {}

    virtual void onStart()
    {
        mOnStart = true;
    }
    virtual void onExit()
    {
        mOnExit = true;
    }

    bool mOnStart;
    bool mOnExit;
};

class OnStartExitServer : public CommsServerEndpoint
{
public:
    OnStartExitServer() : mOnStart(false), mOnExit(false) {}

    virtual void onStart()
    {
        mOnStart = true;
    }
    virtual void onExit()
    {
        mOnExit = true;
    }

    bool mOnStart;
    bool mOnExit;
};

TEST(Connect, onStartExit)
{
    // 1. Callbacks are done after connect/disconnect
    OnStartExitClient client;
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
    CHECK(!client.disconnect());
    CHECK(client.mOnStart);
    CHECK(client.mOnExit);

    // 2. Callbacks are done after start/stop
    OnStartExitServer server;
    CHECK(!server.start(IPC_ADDRESS_COMMS_MODULE_TEST+1));
    CHECK(!server.stop());
    CHECK(server.mOnStart);
    CHECK(server.mOnExit);
}


/**
 * Test disconnect/stop while sendReceive is pending
 * 1. disconnect while sendReceive is pending in client
 * 2. stop while sendReceive is pending in client
 * 3. disconnect while sendReceive is pending in server
 * 4. stop while sendReceive is pending in server
 */

void doSendReceive(CommsEndpoint* aComms)
{
    CommsMessage msg;
    msg.setModuleId(MODULE_ID_NO_REPLY);
    CommsMessage receivedMsg;

    int rc = aComms->sendReceive(msg, receivedMsg, 3);
    CHECK(rc != 0);
    CHECK(rc != ETIMEDOUT);

    // check that next send fails as other party is disconnected
    // (and receive thread may still be running depending on timing)
    rc = aComms->send(msg);
    CHECK(rc != 0);
}

void* sendReceiveThread(void* aComms)
{
    CommsEndpoint* comms = reinterpret_cast<CommsEndpoint*>(aComms);
    doSendReceive(comms);
    return 0;
}

class Notifier : public CommsListener
{
public:
    Notifier(Monitor& aMonitor) : mMonitor(aMonitor) {}
    virtual void processMessage(CommsMessage&)
    {
        mMonitor.notify();

    }
private:
    Monitor& mMonitor;
};

struct threadData
{
    CommsEndpoint* comms;
    int clientAddress;
};

void doServer(CommsEndpoint* aComms, int aAddress)
{
    CommsMessage msg;
    msg.setReceiver(aAddress);
    CommsMessage receivedMsg;

    int rc = aComms->sendReceive(msg, receivedMsg, 3);
    CHECK(rc != 0);
    CHECK(rc != ETIMEDOUT);
}

void* serverThread(void* aData)
{
    threadData* data = reinterpret_cast<threadData*>(aData);
    doServer(data->comms, data->clientAddress);
    delete data;
    return 0;
}

class ServerListener : public CommsListener
{
public:
    ServerListener(CommsEndpoint& aComms) : mComms(aComms) {}
    virtual void processMessage(CommsMessage& aMsg)
    {
        threadData* data = new threadData;
        data->comms = &mComms;
        data->clientAddress = aMsg.getSender();
        pthread_t threadId = 0;
        pthread_create(&threadId, 0, serverThread, data);
    }
private:
    CommsEndpoint& mComms;
};

TEST(Connect, sendReceivePending)
{
    std::auto_ptr<Monitor> monitor(Monitor::createMonitor());

    Notifier listener(*monitor);
    CommsClientEndpoint client;

    // 1. disconnect while sendReceive is pending in client
    CHECK(!comms.registerDefaultListener(&listener));
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST));

    pthread_t threadId = 0;
    pthread_create(&threadId, 0, sendReceiveThread, &client);

    monitor->wait();
    CHECK(!client.disconnect());

    pthread_join(threadId, 0);
    CHECK(!comms.unregisterDefaultListener(&listener));

    // 2. stop while sendReceive is pending in client
    CommsServerEndpoint server;
    CHECK(!server.start(IPC_ADDRESS_COMMS_MODULE_TEST+1));
    CHECK(!server.registerDefaultListener(&listener));

    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST+1));

    threadId = 0;
    pthread_create(&threadId, 0, sendReceiveThread, &client);

    monitor->wait();
    CHECK(!server.stop());

    pthread_join(threadId, 0);
    CHECK(!client.disconnect());
    CHECK(!server.unregisterDefaultListener(&listener));

    // 3. disconnect while sendReceive is pending in server
    ServerListener serverListener(server);

    CHECK(!server.registerDefaultListener(&serverListener));
    CHECK(!client.registerDefaultListener(&listener));

    CHECK(!server.start(IPC_ADDRESS_COMMS_MODULE_TEST+1));
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST+1));

    CommsMessage msg;
    CHECK(!client.send(msg));
    monitor->wait();
    CHECK(!client.disconnect());

    CHECK(!server.stop());
    CHECK(!server.unregisterDefaultListener(&serverListener));
    CHECK(!client.unregisterDefaultListener(&listener));

    // 4. stop while sendReceive is pending in server
    CHECK(!server.registerDefaultListener(&serverListener));
    CHECK(!client.registerDefaultListener(&listener));

    CHECK(!server.start(IPC_ADDRESS_COMMS_MODULE_TEST+1));
    CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST+1));

    CHECK(!client.send(msg));
    monitor->wait();
    CHECK(!server.stop());

    CHECK(!client.disconnect());
    CHECK(!server.unregisterDefaultListener(&serverListener));
    CHECK(!client.unregisterDefaultListener(&listener));
}

