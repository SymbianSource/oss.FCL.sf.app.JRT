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

TEST_GROUP(Heap)
{
    TEST_SETUP()
    {
        {
        }
    }

    TEST_TEARDOWN()
    {
    }
};


#ifdef __SYMBIAN32__
class HeapListener : public CommsListener
{
    virtual void processMessage(CommsMessage& message)
    {
        std::string s;
        int i;
        message >> s >> i;
    }
};

// The purpose of this test is to ensure that clients of javacomms library
// can use __UHEAP_MARK etc macros in their unit tests without giving false alarms
TEST(Heap, HeapCheck)
{
    __UHEAP_MARK;
    {
        EchoServer server;
        CHECK(!server.start(IPC_ADDRESS_COMMS_MODULE_TEST));

        CommsClientEndpoint client;
        HeapListener listener;
        CHECK(!client.registerDefaultListener(&listener));
        CHECK(!client.connect(IPC_ADDRESS_COMMS_MODULE_TEST));

        CommsMessage msg;
        msg << "add some random stuff" << 123;

        CHECK(!client.send(msg));

        CommsMessage receivedMsg;
        CHECK(!client.sendReceive(msg, receivedMsg, 1));
    }
    __UHEAP_MARKEND;
}
#endif

