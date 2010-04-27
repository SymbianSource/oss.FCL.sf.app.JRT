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
* Description: RmsExtensionPlugin unit tests
*
*/

#include <string>
#include "TestHarness.h"
#include "javacommonutils.h"
#include "commsmessage.h"
#include "comms.h"

#include "rmsextensionplugin.h"

using namespace java::rms;
using namespace java::util;

class TestRmsExtensionPlugin : public RmsExtensionPlugin
{
public:
    TestRmsExtensionPlugin() : mRecordChangeMsgs(0), mListenerCountMsgs(0) {};
    virtual ~TestRmsExtensionPlugin() {};

    using RmsExtensionPlugin::addListener;
    using RmsExtensionPlugin::removeListener;
    using RmsExtensionPlugin::notifyListeners;
    using RmsExtensionPlugin::sendListenerCount;
    using RmsExtensionPlugin::createListenerCountMsg;
    virtual int send(CommsMessage& aMessage)
    {
        int msgId = aMessage.getMessageId();
        int rc = 0;
        switch (msgId)
        {
        case MSG_ID_RECORD_CHANGE:
            mRecordChangeMsgs++;
            break;
        case MSG_ID_LISTENER_COUNT:
            mListenerCountMsgs++;
            break;
        default:
            rc = aMessage.getMessageId();
        }
        return rc;
    };

    using RmsExtensionPlugin::mListeners;
    using RmsExtensionPlugin::listenersIterator;
    int mRecordChangeMsgs;
    int mListenerCountMsgs;
};

TEST_GROUP(TestMethods)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

TEST(TestMethods, addRemoveListener)
{
    TestRmsExtensionPlugin rms;
    CHECK(rms.mListeners.size() == 0);

    rms.addListener("aaa", 1);
    CHECK(rms.mListeners.size() == 1);

    TestRmsExtensionPlugin::listenersIterator iter = rms.mListeners.find("aaa");
    CHECK(iter->second.size() == 1);

    rms.removeListener("aaa", 1);
    CHECK(rms.mListeners.size() == 0);
}

TEST(TestMethods, addSameListener)
{
    TestRmsExtensionPlugin rms;
    CHECK(rms.mListeners.size() == 0);

    rms.addListener("aaa", 1);
    rms.addListener("aaa", 1);
    rms.addListener("aaa", 1);
    CHECK(rms.mListeners.size() == 1);

    TestRmsExtensionPlugin::listenersIterator iter = rms.mListeners.find("aaa");
    CHECK(iter->second.size() == 3);

    rms.removeListener("aaa", 1);
    iter = rms.mListeners.find("aaa");
    CHECK(iter->second.size() == 2);

    rms.removeListener("aaa", 1);
    rms.removeListener("aaa", 1);
    CHECK(iter->second.size() == 0);
}

TEST(TestMethods, manyListenersOneName)
{
    TestRmsExtensionPlugin rms;
    CHECK(rms.mListeners.size() == 0);

    // multiple listeners for one name
    for (int i = 0; i < 10; i++)
    {
        rms.addListener("aaa", i);
    }
    CHECK(rms.mListeners.size() == 1);
    TestRmsExtensionPlugin::listenersIterator iter = rms.mListeners.find("aaa");
    CHECK(iter->second.size() == 10);

    for (int i = 0; i < 10; i++)
    {
        CHECK(iter->second.at(i) == i);
    }

    for (int i = 0; i < 10; i++)
    {
        rms.removeListener("aaa", i);
    }
    CHECK(rms.mListeners.size() == 0);
}

TEST(TestMethods, manyListenersManyNames)
{
    TestRmsExtensionPlugin rms;
    CHECK(rms.mListeners.size() == 0);

    // multiple listeners for many names
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            rms.addListener(JavaCommonUtils::intToString(i), j);
        }
    }
    CHECK(rms.mListeners.size() == 10);


    for (int i = 0; i < 10; i++)
    {
        TestRmsExtensionPlugin::listenersIterator iter = rms.mListeners.find(JavaCommonUtils::intToString(i));
        CHECK(iter->second.size() == 10);
        for (int j = 0; j < 10; j++)
        {
            CHECK(iter->second.at(j) == j);
        }
    }

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            rms.removeListener(JavaCommonUtils::intToString(i), j);
        }
    }
    CHECK(rms.mListeners.size() == 0);
}

TEST(TestMethods, removeNonExistingListener)
{
    TestRmsExtensionPlugin rms;
    CHECK(rms.mListeners.size() == 0);

    // no such name
    rms.removeListener("aaa", 1);
    CHECK(rms.mListeners.size() == 0);

    // no listener
    rms.addListener("aaa", 1);
    rms.removeListener("aaa", 2);
    CHECK(rms.mListeners.size() == 1);
    TestRmsExtensionPlugin::listenersIterator iter = rms.mListeners.find("aaa");
    CHECK(iter->second.size() == 1);
    rms.removeListener("aaa", 1);
    CHECK(rms.mListeners.size() == 0);
}

TEST(TestMethods, notifyListeners)
{
    TestRmsExtensionPlugin rms;
    CHECK(rms.mRecordChangeMsgs == 0);

    CommsMessage msg;
    msg.setSender(0);
    msg.setMessageId(MSG_ID_RECORD_CHANGE);

    rms.addListener("aaa", 1);
    rms.addListener("aaa", 2);

    rms.notifyListeners("aaa", msg);
    CHECK(rms.mRecordChangeMsgs == 2);
}

TEST(TestMethods, noListeners)
{
    TestRmsExtensionPlugin rms;

    CommsMessage msg;
    msg.setSender(0);
    msg.setMessageId(MSG_ID_RECORD_CHANGE);

    rms.notifyListeners("aaa", msg);
    CHECK(rms.mRecordChangeMsgs == 0);

    rms.addListener("aaa", 1);
    rms.addListener("aaa", 2);
    rms.addListener("bbb", 1);

    rms.notifyListeners("ccc", msg);
    CHECK(rms.mRecordChangeMsgs == 0);

    msg.setSender(1);
    rms.notifyListeners("bbb", msg);
    CHECK(rms.mRecordChangeMsgs == 0);

    TestRmsExtensionPlugin::listenersIterator iter = rms.mListeners.find("aaa");
    CHECK(iter->second.size() == 2);

    iter = rms.mListeners.find("bbb");
    CHECK(iter->second.size() == 1);
}

TEST(TestMethods, sendFails)
{
    TestRmsExtensionPlugin rms;

    CommsMessage msg;
    msg.setSender(0);
    msg.setMessageId(-1);

    rms.addListener("aaa", 1);
    rms.addListener("aaa", 2);

    TestRmsExtensionPlugin::listenersIterator iter = rms.mListeners.find("aaa");
    CHECK(iter->second.size() == 2);

    rms.notifyListeners("aaa", msg);
    CHECK(rms.mListeners.size() == 0);
}

TEST(TestMethods, processMessage)
{
    CommsMessage addMsg;
    addMsg.setSender(1);
    addMsg.setMessageId(MSG_ID_LISTENER_CHANGE);
    addMsg << "aaa" << ADD_LISTENER;

    CommsMessage removeMsg;
    removeMsg.setSender(1);
    removeMsg.setMessageId(MSG_ID_LISTENER_CHANGE);
    removeMsg << "aaa" << REMOVE_LISTENER;

    CommsMessage recordMsg;
    recordMsg.setSender(0);
    recordMsg.setMessageId(MSG_ID_RECORD_CHANGE);
    recordMsg << "aaa";

    TestRmsExtensionPlugin rms;
    // add listener
    rms.processMessage(addMsg);
    TestRmsExtensionPlugin::listenersIterator iter = rms.mListeners.find("aaa");
    CHECK(iter->second.size() == 1);
    CHECK(rms.mListenerCountMsgs == 1);

    // record modified
    rms.processMessage(recordMsg);
    CHECK(rms.mRecordChangeMsgs == 1);

    // remove listener
    rms.processMessage(removeMsg);
    CHECK(rms.mListeners.size() == 0);
}

TEST(TestMethods, unknownMessage)
{
    CommsMessage dummy;
    dummy.setMessageId(666);

    TestRmsExtensionPlugin rms;
    rms.processMessage(dummy);
    CHECK(rms.mRecordChangeMsgs == 0);
    CHECK(rms.mListeners.size() == 0);
}

TEST(TestMethods, createListenerCountMsg)
{

    TestRmsExtensionPlugin rms;
    CommsMessage msg = rms.createListenerCountMsg("aaa", 3);

    std::string name;
    int count;
    msg >> name >> count;

    CHECK(msg.getMessageId() == MSG_ID_LISTENER_COUNT);
    CHECK(msg.getModuleId() == PLUGIN_ID_RMS_C);
    CHECK(name == "aaa");
    CHECK(count == 3);
}

TEST(TestMethods, sendListenerCount)
{
    TestRmsExtensionPlugin rms;
    rms.addListener("aaa", 1);
    rms.addListener("aaa", 2);
    CHECK(rms.mListenerCountMsgs == 0);

    CommsMessage dummy;
    // invalid name
    rms.sendListenerCount("bbb", dummy);
    CHECK(rms.mListenerCountMsgs == 0);

    // valid name
    rms.sendListenerCount("aaa", dummy);
    CHECK(rms.mListenerCountMsgs == 2);
}



