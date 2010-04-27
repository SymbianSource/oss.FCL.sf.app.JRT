/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: RmsExtensionPlugin unit tests with Java Captain
*
*/

#include <string>
#include "TestHarness.h"
#include "comms.h"
#include "commsmessage.h"
#include "commsclientendpoint.h"
#include "monitor.h"

#include "rmsextensionplugin.h"

using namespace java::rms;
using namespace java::util;
using namespace java::comms;

class NotifyListener : public CommsListener
{
public:
    NotifyListener(Monitor& aMonitor, int aMsgCount) : mListenerCountMsg(0), mListenerCount(0),
            mMonitor(aMonitor), mMsgCount(aMsgCount) {};
    virtual void processMessage(CommsMessage& aMessage)
    {
        int id = aMessage.getMessageId();
        switch (id)
        {
        case MSG_ID_RECORD_CHANGE:
            if (--mMsgCount == 0)
            {
                mMonitor.notify();
            }
            break;
        case MSG_ID_LISTENER_COUNT:
        {
            mListenerCountMsg++;
            std::string name;
            aMessage >> name >> mListenerCount;
            break;
        }
        default:
            break;
        }
    }

    int mListenerCountMsg;
    int mListenerCount;
private:
    Monitor& mMonitor;
    int mMsgCount;
};


CommsMessage createAddListenerMsg()
{
    CommsMessage addMsg;
    addMsg.setModuleId(PLUGIN_ID_RMS_C);
    addMsg.setMessageId(MSG_ID_LISTENER_CHANGE);
    addMsg << "aaa" << ADD_LISTENER;
    return addMsg;
}

CommsMessage createRemoveListenerMsg()
{
    CommsMessage removeMsg;
    removeMsg.setModuleId(PLUGIN_ID_RMS_C);
    removeMsg.setMessageId(MSG_ID_LISTENER_CHANGE);
    removeMsg << "aaa" << REMOVE_LISTENER;
    return removeMsg;
}

CommsMessage createRecordChangedMsg()
{
    CommsMessage recordMsg;
    recordMsg.setModuleId(PLUGIN_ID_RMS_C);
    recordMsg.setMessageId(MSG_ID_RECORD_CHANGE);
    recordMsg << "aaa";
    return recordMsg;
}

TEST_GROUP(TestPlugin)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};


TEST(TestPlugin, oneListener)
{
    CommsClientEndpoint a;
    std::auto_ptr<Monitor> monitor(Monitor::createMonitor());
    NotifyListener listener(*monitor, 1);
    a.registerDefaultListener(&listener);

    int rc = a.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
    CHECK(rc == 0);
    CommsMessage msg = createAddListenerMsg();
    CommsMessage receivedMsg;
    rc = a.sendReceive(msg, receivedMsg, WAIT_FOR_EVER);
    CHECK(rc == 0);
    listener.processMessage(receivedMsg);
    CHECK(listener.mListenerCountMsg == 1);

    CommsClientEndpoint dummy;
    dummy.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
    msg = createRecordChangedMsg();
    rc = dummy.send(msg);
    CHECK(rc == 0);

    monitor->wait();

    msg = createRemoveListenerMsg();
    rc = a.send(msg);
    CHECK(rc == 0);
}

TEST(TestPlugin, multipleListeners)
{
    // 1st listener
    CommsClientEndpoint a;
    std::auto_ptr<Monitor> monitorA(Monitor::createMonitor());
    NotifyListener listenerA(*monitorA, 2);
    a.registerDefaultListener(&listenerA);
    int rc = a.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
    CHECK(rc == 0);

    // 2nd listener
    CommsClientEndpoint b;
    std::auto_ptr<Monitor> monitorB(Monitor::createMonitor());
    NotifyListener listenerB(*monitorB, 2);
    b.registerDefaultListener(&listenerB);
    rc = b.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
    CHECK(rc == 0);

    // 3rd listener
    CommsClientEndpoint c;
    std::auto_ptr<Monitor> monitorC(Monitor::createMonitor());
    NotifyListener listenerC(*monitorC, 2);
    c.registerDefaultListener(&listenerC);
    rc = c.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
    CHECK(rc == 0);

    // add listeners
    CommsMessage msg = createAddListenerMsg();
    rc = a.send(msg);
    CHECK(rc == 0);
    rc = b.send(msg);
    CHECK(rc == 0);
    rc = c.send(msg);
    CHECK(rc == 0);

    // send record modified
    msg = createRecordChangedMsg();
    rc = a.send(msg);
    CHECK(rc == 0);
    rc = b.send(msg);
    CHECK(rc == 0);
    rc = c.send(msg);
    CHECK(rc == 0);

    monitorA->wait();
    monitorB->wait();
    monitorC->wait();

    CHECK(listenerA.mListenerCountMsg == 3);
    CHECK(listenerB.mListenerCountMsg == 2);
    CHECK(listenerC.mListenerCountMsg == 1);

    CHECK(listenerA.mListenerCount == 3);
    CHECK(listenerB.mListenerCount == 3);
    CHECK(listenerC.mListenerCount == 3);

    // remove listeners
    msg = createRemoveListenerMsg();
    rc = a.send(msg);
    CHECK(rc == 0);
    rc = b.send(msg);
    CHECK(rc == 0);
    rc = c.send(msg);
    CHECK(rc == 0);
}
