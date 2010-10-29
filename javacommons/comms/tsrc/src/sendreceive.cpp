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
* Description:  Unit tests for send and sendReceive functionality
*
*/

#include <pthread.h>
#include <unistd.h>
#include "TestHarness.h"
#include "monitor.h"
#include "logger.h"

#include "comms.h"
#include "commsclientendpoint.h"
#include "commsendpoint.h"
#include "echoserver.h"

using namespace java::comms;
using namespace std;
using java::util::Monitor;

TEST_GROUP(SendReceive)
{
    EchoServer server;
    CommsClientEndpoint client;

    TEST_SETUP()
    {
        server.start(IPC_ADDRESS_COMMS_MODULE_TEST);
        client.connect(IPC_ADDRESS_COMMS_MODULE_TEST);
    }

    TEST_TEARDOWN()
    {
        client.disconnect();
        server.stop();
    }
};

/**
 * Test message sending
 * 1. Sending empty message
 * 2. Check contents of message (empty message)
 * 3. Message payload check
 */

TEST(SendReceive, normal)
{
#ifdef __SYMBIAN32__
    EXPECT_N_LEAKS(1);
#else
    EXPECT_N_LEAKS(2);
#endif
    CommsMessage msg;
    msg.setModuleId(MODULE_ID_NO_REPLY);

    // 1. Sending empty message
    CHECK(client.send(msg) == 0);

    // 2. Check contents of message (empty message)
    CommsMessage msg2;
    msg2.setModuleId(MODULE_ID_ECHO);
    CommsMessage receivedMsg;

    CHECK(client.sendReceive(msg2, receivedMsg, 1) == 0);
    CHECK(msg2.toString().compare(receivedMsg.toString()) == 0);
    CHECK(msg2.getModuleId() == receivedMsg.getModuleId());

    // 3. Message payload check
    CommsMessage msg3;
    msg3.setModuleId(MODULE_ID_ECHO);
    CommsMessage receivedMsg2;

    int in = 0xCAFEBABE;
    int zero = 0;
    int one = 1;
    int max = 0xFFFFFFFF;
    int neg = -12345;
    string ascii = "abcdefghijklmnopqrstyvwxyzABCDEFGHIJKLMNOPQRSTYVWXYZ0123456789";
    msg3 << ascii << in << zero << one << max << neg;

    CHECK(client.sendReceive(msg3, receivedMsg2, 1) == 0);

    string s;
    int i;
    receivedMsg2 >> s;
    CHECK(s.compare(ascii) == 0);
    receivedMsg2 >> i;
    CHECK(i == in);
    receivedMsg2 >> i;
    CHECK(i == zero);
    receivedMsg2 >> i;
    CHECK(i == one);
    receivedMsg2 >> i;
    CHECK(i == max);
    receivedMsg2 >> i;
    CHECK(i == neg);

}

/**
 * Test message sending with wait for ever flag
 * 1. Check contents of message (empty message)
 * 2. Message payload check
 */

TEST(SendReceive, normalwaitforever)
{
#ifdef __SYMBIAN32__
    EXPECT_N_LEAKS(1);
#else
    EXPECT_N_LEAKS(2);
#endif
    // 1. Check contents of message (empty message)
    CommsMessage msg2;
    msg2.setModuleId(MODULE_ID_ECHO);
    CommsMessage receivedMsg;

    CHECK(client.sendReceive(msg2, receivedMsg, WAIT_FOR_EVER) == 0);
    CHECK(msg2.toString().compare(receivedMsg.toString()) == 0);
    CHECK(msg2.getModuleId() == receivedMsg.getModuleId());

    // 2. Message payload check
    CommsMessage msg3;
    msg3.setModuleId(MODULE_ID_ECHO);
    CommsMessage receivedMsg2;

    int in = 0xCAFEBABE;
    int zero = 0;
    int one = 1;
    int max = 0xFFFFFFFF;
    int neg = -12345;
    string ascii = "abcdefghijklmnopqrstyvwxyzABCDEFGHIJKLMNOPQRSTYVWXYZ0123456789";
    msg3 << ascii << in << zero << one << max << neg;

    CHECK(client.sendReceive(msg3, receivedMsg2, WAIT_FOR_EVER) == 0);

    string s;
    int i;
    receivedMsg2 >> s;
    CHECK(s.compare(ascii) == 0);
    receivedMsg2 >> i;
    CHECK(i == in);
    receivedMsg2 >> i;
    CHECK(i == zero);
    receivedMsg2 >> i;
    CHECK(i == one);
    receivedMsg2 >> i;
    CHECK(i == max);
    receivedMsg2 >> i;
    CHECK(i == neg);

}

/**
 * Test message sending with big timeout values
 * 1. OpenC timeout limit
 * 2. 1h timeout
 * 3. 24h timeout
 */

TEST(SendReceive, bigtimeout)
{
    EXPECT_N_LEAKS(1);
    CommsMessage msg;
    msg.setModuleId(MODULE_ID_ECHO);
    CommsMessage receivedMsg;

    // 1. OpenC timeout limit
    CHECK(client.sendReceive(msg, receivedMsg, 2148) == 0);
    CHECK(msg.getModuleId() == receivedMsg.getModuleId());

    // 2. 1h timeout
    CHECK(client.sendReceive(msg, receivedMsg, 3600) == 0);
    CHECK(msg.getModuleId() == receivedMsg.getModuleId());

    // 3. 24h timeout
    CHECK(client.sendReceive(msg, receivedMsg, 86400) == 0);
    CHECK(msg.getModuleId() == receivedMsg.getModuleId());
}

/**
 * Test message sending
 * 1. Sending big message
 */

TEST(SendReceive, bigmessage)
{
#ifdef __SYMBIAN32__
    EXPECT_N_LEAKS(1);
#else
    EXPECT_N_LEAKS(2);
#endif
    // 1. Sending big message
    int SIZE = 60000; // 60k
    char* array = new char[SIZE];
    for (int i = 0; i < SIZE; i++)
    {
        array[i] = i;
    }
    string input(array, SIZE);
    delete[] array;

    CommsMessage msg;
    msg.setModuleId(MODULE_ID_ECHO);
    msg << input;

    CommsMessage receivedMsg;
    CHECK(client.sendReceive(msg, receivedMsg, 1) == 0);

    string s;
    receivedMsg >> s;
    CHECK(s.compare(input) == 0);
}


/**
 * Test reply from listener
 * 1. sendReceive and reply from listener
 */

TEST(SendReceive, listener)
{
    // 1. sendReceive and reply from listener
    int MODULEID = 666;
    CommsEndpoint* comms = server.getComms();
    EchoListener listener(comms);

    CHECK(!comms->registerListener(MODULEID, &listener));

    CommsMessage msg;
    msg.setModuleId(MODULEID);
    msg << "hello world";

    CommsMessage receivedMsg;
    CHECK(client.sendReceive(msg, receivedMsg, 1) == 0);

    string s;
    receivedMsg >> s;
    CHECK(s.compare("hello world") == 0);

    CHECK(!comms->unregisterListener(MODULEID, &listener));
}


/**
 * Test sendReceive timeout
 * 1. sendReceive no timeout
 * 2. sendReceive timeout
 */

TEST(SendReceive, timeouts)
{
    EXPECT_N_LEAKS(1);
    // 1. sendReceive no timeout
    CommsMessage msg;
    msg.setModuleId(MODULE_ID_SLEEP_1S);

    CommsMessage receivedMsg;
    CHECK(client.sendReceive(msg, receivedMsg, 2) == 0);

    // 2. sendReceive timeout
    msg.setModuleId(MODULE_ID_NO_REPLY);
    CHECK(client.sendReceive(msg, receivedMsg, 1) != 0);
}


/**
 * Test send error cases
 * 1. send when not connected
 * 2. sendReceive when not connected
 * 3. send to non-existing endpoint
 * 4. invalid timeout values
 */

TEST(SendReceive, errors)
{
    EXPECT_N_LEAKS(1);
    CommsClientEndpoint con;

    // 1. send when not connected
    CommsMessage msg;
    msg.setModuleId(MODULE_ID_ECHO);
    CHECK(!con.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
    CHECK(!con.send(msg));
    CHECK(!con.disconnect());
    CHECK(con.send(msg)!=0);

    // 2. sendReceive when not connected
    CommsMessage receivedMsg;
    CHECK(!con.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
    CHECK(!con.sendReceive(msg, receivedMsg, 2));
    CHECK(!con.sendReceive(msg, receivedMsg, WAIT_FOR_EVER));
    CHECK(!con.disconnect());
    CHECK(con.sendReceive(msg, receivedMsg, 2)!=0);

    // 3. send to non-existing endpoint
    CommsMessage dummy;
    dummy.setReceiver(666);
    CHECK(server.getComms()->send(msg) != 0);

    // 4. invalid timeout values
    msg.setModuleId(MODULE_ID_ECHO);
    CHECK(!con.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
    CHECK(con.sendReceive(msg, receivedMsg, 0)!=0);
    CHECK(con.sendReceive(msg, receivedMsg, -2)!=0);
    CHECK(!con.disconnect());
}

/**
 * Test multithread send (client)
 * 1. send in multiple threads
 * 2. sendReceive in multiple threads
 */

void doSends(CommsClientEndpoint* aComms)
{
    CommsMessage msg;
    msg.setModuleId(MODULE_ID_ECHO);
    CommsMessage receivedMsg;

    for (int i = 0; i < 5; i++)
    {
        CHECK(aComms->send(msg) == 0);
        CHECK(aComms->sendReceive(msg, receivedMsg, 1) == 0);
        CHECK(aComms->sendReceive(msg, receivedMsg, WAIT_FOR_EVER) == 0);
        usleep(300000); //0.3s
    }
}

void* clientSendThread(void* aComms)
{
    CommsClientEndpoint* comms = reinterpret_cast<CommsClientEndpoint*>(aComms);
    doSends(comms);
    return 0;
}

TEST(SendReceive, multiThreadClient)
{
    EXPECT_N_LEAKS(1);
    CommsClientEndpoint con;
    CHECK(!con.connect(IPC_ADDRESS_COMMS_MODULE_TEST));

    const int NUM_THREADS = 5;
    pthread_t thread[NUM_THREADS];

    for (int i=0; i<NUM_THREADS; i++)
    {
        pthread_create(&thread[i], 0, &clientSendThread, &con);
    }

    for (int i=0; i<NUM_THREADS; i++)
    {
        pthread_join(thread[i], 0);
    }

    CHECK(!con.disconnect());
}


class ReplyListener : public CommsListener
{
public:
    ReplyListener(CommsEndpoint& aComms, Monitor& aMonitor)
            : mComms(aComms), mMonitor(aMonitor), mReplyCount(30) {}
    virtual void processMessage(CommsMessage& aMessage)
    {
        CommsMessage reply;
        reply.replyTo(aMessage);
        //ELOG1(EJavaComms, "ReplyListener count=%d", mReplyCount);
        mComms.send(reply);

        if (--mReplyCount < 0)
        {
            mMonitor.notify();
        }
        //ELOG1(EJavaComms, "ReplyListener after send count=%d", mReplyCount);
    }
private:
    CommsEndpoint& mComms;
    Monitor& mMonitor;
    int mReplyCount;
};

TEST(SendReceive, sendsInProcessMessage)
{
    EXPECT_N_LEAKS(1);
    std::auto_ptr<Monitor> monitor(Monitor::createMonitor());

    CommsClientEndpoint con;
    ReplyListener listener(con, *monitor);

    CHECK(!con.connect(IPC_ADDRESS_COMMS_MODULE_TEST));
    CHECK(!con.registerDefaultListener(&listener));

    CommsMessage msg;
    CHECK(!con.send(msg));

    monitor->wait();
    CHECK(!con.disconnect());
}


/**
 * Test sendReceive with two clients
 * 1. server replies to first message after second message is received
 *    (and two clients use sendReceive to send single messsage)
 */
class RoutingListener : public CommsListener
{
public:
    RoutingListener(CommsEndpoint& aComms) : mComms(aComms), mMsgCount(0) {}

    virtual void processMessage(CommsMessage& aMessage)
    {
        if (mMsgCount == 0)
        {
            // delay reply until second message is received
            mFirstMessage = aMessage;
        }
        else
        {
            // reply to first client
            CommsMessage reply;
            reply.replyTo(mFirstMessage);
            mComms.send(reply);

            // reply to second client
            reply.replyTo(aMessage);
            mComms.send(reply);
        }
        mMsgCount++;
    }
private:
    CommsMessage mFirstMessage;
    int mMsgCount;
    CommsEndpoint& mComms;
};

void doSendReceive()
{
    CommsClientEndpoint con;
    CHECK(!con.connect(IPC_ADDRESS_COMMS_MODULE_TEST + 1));
    CommsMessage msg;
    CommsMessage receivedMsg;
    CHECK(!con.sendReceive(msg, receivedMsg, WAIT_FOR_EVER));
    CHECK(!con.disconnect());
}

void* clientSendReceiveThread(void*)
{
    doSendReceive();
    return 0;
}

TEST(SendReceive, twoClientsAndServer)
{
    CommsServerEndpoint server;
    RoutingListener listener(server);
    CHECK(!server.registerDefaultListener(&listener));
    CHECK(!server.start(IPC_ADDRESS_COMMS_MODULE_TEST + 1));

    pthread_t thread1;
    pthread_t thread2;
    pthread_create(&thread1, 0, &clientSendReceiveThread, 0);
    pthread_create(&thread2, 0, &clientSendReceiveThread, 0);
    pthread_join(thread1, 0);
    pthread_join(thread2, 0);

    server.stop();
}
