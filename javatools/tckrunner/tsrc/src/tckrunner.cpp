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

#include <unistd.h>
#include "TestHarness.h"
#include "commsmessage.h"
#include "commsclientendpoint.h"
#include "logger.h"

#include "testserver.h"

using namespace java::comms;
using namespace std;
using java::util::Uid;

const int TIMEOUT = 10;

TEST_GROUP(TckRunner)
{
    TestServer server;
    TEST_SETUP()
    {
        server.start();
    }

    TEST_TEARDOWN()
    {
        server.stop();
    }
};

/**
 * Test normal TCK run
 * 1: install operation
 * 2: install result
 * 3: uninstall operation
 * 4: launch request check
 * 5: uninstall result
 */
TEST(TckRunner, ok_case)
{
    // 1: install operation
    {
        CommsMessage msg;
        msg.setMessageId(MSG_ID_GET_NEXT_OPERATION);

        CommsMessage message;
        int rc = server.getJavaInstaller().sendReceive(msg, message, TIMEOUT);

        int oper;
        string url;
        message >> oper >> url;
        CHECK(rc == 0);
        CHECK(oper == OPERATION_INSTALL);
        CHECK(url.compare("unittest:\\\\tckrunner_tester") == 0);
    }
    // 2: install result
    {
        CommsMessage msg;
        msg.setMessageId(MSG_ID_OPERATION_RESULT);
        msg << OPERATION_INSTALL << 0 << 3 << "uid1" << "uid2" << "uid3";
        server.getJavaInstaller().send(msg);
    }
    // 3: uninstall operation
    {
        CommsMessage msg;
        msg.setMessageId(MSG_ID_GET_NEXT_OPERATION);

        CommsMessage message;
        int rc = server.getJavaInstaller().sendReceive(msg, message, TIMEOUT);

        int oper;
        string uid;
        message >> oper >> uid;
        CHECK(rc == 0);
        CHECK(oper == OPERATION_UNINSTALL);
        CHECK(uid.compare("uid1") == 0);
    }
    // 4: launch request check
    {
        CHECK(server.mLaunchReqs.size() == 3);
        CHECK(server.mLaunchReqs.front().compare("uid1") == 0);
        server.mLaunchReqs.pop();
        CHECK(server.mLaunchReqs.front().compare("uid2") == 0);
        server.mLaunchReqs.pop();
        CHECK(server.mLaunchReqs.front().compare("uid3") == 0);
        server.mLaunchReqs.pop();
    }
    // 5: uninstall result
    {
        CommsMessage msg;
        msg.setMessageId(MSG_ID_OPERATION_RESULT);
        msg << OPERATION_UNINSTALL << 0;
        server.getJavaInstaller().send(msg);
    }
}

/**
 * Installation fails
 * 1: install operation
 * 2: install result nok
 * 3: check no launch requests
 */
TEST(TckRunner, installfail)
{
    int i = 0;
    do
    {
        // 1: install operation
        {
            CommsMessage msg;
            msg.setMessageId(MSG_ID_GET_NEXT_OPERATION);

            CommsMessage message;
            int rc = server.getJavaInstaller().sendReceive(msg, message, TIMEOUT);

            int oper;
            string url;
            message >> oper >> url;
            CHECK(rc == 0);
            CHECK(oper == OPERATION_INSTALL);
            CHECK(url.compare("unittest:\\\\tckrunner_tester") == 0);
        }
        // 2: install result
        {
            CommsMessage msg;
            msg.setMessageId(MSG_ID_OPERATION_RESULT);
            msg << OPERATION_INSTALL << -1 << 3 << "uid1" << "uid2" << "uid3";
            server.getJavaInstaller().send(msg);
        }
        sleep(1);
        // 3: check no launch requests
        {
            CHECK(server.mLaunchReqs.size() == 0);
        }
    }
    while (i++ < 3);
}


/**
 * Application launch fails
 * 1: install operation
 * 2: install result
 * 3: uninstall operation
 * 4: launch request check
 * 5: uninstall result
 */
TEST(TckRunner, launchfail)
{
    // 1: install operation
    {
        CommsMessage msg;
        msg.setMessageId(MSG_ID_GET_NEXT_OPERATION);

        CommsMessage message;
        int rc = server.getJavaInstaller().sendReceive(msg, message, TIMEOUT);

        int oper;
        string url;
        message >> oper >> url;
        CHECK(rc == 0);
        CHECK(oper == OPERATION_INSTALL);
        CHECK(url.compare("unittest:\\\\tckrunner_tester") == 0);
    }
    // 2: install result
    {
        CommsMessage msg;
        msg.setMessageId(MSG_ID_OPERATION_RESULT);
        msg << OPERATION_INSTALL << 0 << 3 << "fail" << "fail" << "fail";
        server.getJavaInstaller().send(msg);
    }
    // 3: uninstall operation
    {
        CommsMessage msg;
        msg.setMessageId(MSG_ID_GET_NEXT_OPERATION);

        CommsMessage message;
        int rc = server.getJavaInstaller().sendReceive(msg, message, TIMEOUT);

        int oper;
        string uid;
        message >> oper >> uid;
        CHECK(rc == 0);
        CHECK(oper == OPERATION_UNINSTALL);
        CHECK(uid.compare("fail") == 0);
    }
    // 4: launch request check
    {
        CHECK(server.mLaunchReqs.size() == 3);
        CHECK(server.mLaunchReqs.front().compare("fail") == 0);
        server.mLaunchReqs.pop();
        CHECK(server.mLaunchReqs.front().compare("fail") == 0);
        server.mLaunchReqs.pop();
        CHECK(server.mLaunchReqs.front().compare("fail") == 0);
        server.mLaunchReqs.pop();
    }
    // 5: uninstall result
    {
        CommsMessage msg;
        msg.setMessageId(MSG_ID_OPERATION_RESULT);
        msg << OPERATION_UNINSTALL << 0;
        server.getJavaInstaller().send(msg);
    }
}

/**
 * Uninstallation fails
 * 1: install operation
 * 2: install result
 * 3: uninstall operation
 * 4: launch request check
 * 5: uninstall result nok
 */
TEST(TckRunner, uninstallfail)
{
    // 1: install operation
    {
        CommsMessage msg;
        msg.setMessageId(MSG_ID_GET_NEXT_OPERATION);

        CommsMessage message;
        int rc = server.getJavaInstaller().sendReceive(msg, message, TIMEOUT);

        int oper;
        string url;
        message >> oper >> url;
        CHECK(rc == 0);
        CHECK(oper == OPERATION_INSTALL);
        CHECK(url.compare("unittest:\\\\tckrunner_tester") == 0);
    }
    // 2: install result
    {
        CommsMessage msg;
        msg.setMessageId(MSG_ID_OPERATION_RESULT);
        msg << OPERATION_INSTALL << 0 << 1 << "uid1";
        server.getJavaInstaller().send(msg);
    }
    // 3: uninstall operation
    {
        CommsMessage msg;
        msg.setMessageId(MSG_ID_GET_NEXT_OPERATION);

        CommsMessage message;
        int rc = server.getJavaInstaller().sendReceive(msg, message, TIMEOUT);

        int oper;
        string uid;
        message >> oper >> uid;
        CHECK(rc == 0);
        CHECK(oper == OPERATION_UNINSTALL);
        CHECK(uid.compare("uid1") == 0);
    }
    // 4: launch request check
    {
        CHECK(server.mLaunchReqs.size() == 1);
        CHECK(server.mLaunchReqs.front().compare("uid1") == 0);
        server.mLaunchReqs.pop();
    }
    // 5: uninstall result
    {
        CommsMessage msg;
        msg.setMessageId(MSG_ID_OPERATION_RESULT);
        msg << OPERATION_UNINSTALL << -1;
        server.getJavaInstaller().send(msg);
    }
}


/**
 * Retry count exceeded
 * 1: install operation
 * 2: install result nok
 */
TEST(TckRunner, retrycount)
{
    int rc = 0;
    int i = 0;

    int MAX_RETRY_COUNT = 100; //needs to be bigger than in actual limit
    do
    {
        // 1: install operation
        CommsMessage msg;
        msg.setMessageId(MSG_ID_GET_NEXT_OPERATION);

        CommsMessage message;
        rc = server.getJavaInstaller().sendReceive(msg, message, TIMEOUT);

        if (rc == 0)
        {
            int oper;
            string url;
            message >> oper >> url;
            CHECK(oper == OPERATION_INSTALL || oper == OPERATION_EXIT);
            // 2: install result
            msg.reset();
            msg.setMessageId(MSG_ID_OPERATION_RESULT);
            msg << OPERATION_INSTALL << -1 << 0;
            server.getJavaInstaller().send(msg);
        }
    }
    while (rc == 0 && i++ < MAX_RETRY_COUNT);

    CHECK(i < MAX_RETRY_COUNT);
}


/**
 * Stability test
 * 1: install operation
 * 2: install result
 * 3: uninstall operation
 * 4: uninstall result
 * 5: launch request check
 */
TEST(TckRunner, stabilitytest)
{
    int TCK_RUN_LIMIT = 1000;
    int i = 0;
    do
    {

        // 1: install operation
        {
            CommsMessage msg;
            msg.setMessageId(MSG_ID_GET_NEXT_OPERATION);

            CommsMessage message;
            int rc = server.getJavaInstaller().sendReceive(msg, message, TIMEOUT);

            int oper;
            string url;
            message >> oper >> url;
            CHECK(rc == 0);
            CHECK(oper == OPERATION_INSTALL);
            CHECK(url.compare("unittest:\\\\tckrunner_tester") == 0);
        }
        // 2: install result
        {
            CommsMessage msg;
            msg.setMessageId(MSG_ID_OPERATION_RESULT);
            msg << OPERATION_INSTALL << 0 << 1 << "uid1";
            server.getJavaInstaller().send(msg);
        }
        // 3: uninstall operation
        {
            CommsMessage msg;
            msg.setMessageId(MSG_ID_GET_NEXT_OPERATION);

            CommsMessage message;
            int rc2 = server.getJavaInstaller().sendReceive(msg, message, TIMEOUT);

            int oper;
            string uid;
            message >> oper >> uid;
            CHECK(rc2 == 0);
            CHECK(oper == OPERATION_UNINSTALL);
            CHECK(uid.compare("uid1") == 0);
        }

        // 4: uninstall result
        {
            CommsMessage msg;
            msg.setMessageId(MSG_ID_OPERATION_RESULT);
            msg << OPERATION_UNINSTALL << 0;
            server.getJavaInstaller().send(msg);
        }
    }
    while (++i < TCK_RUN_LIMIT);

    // 5: launch request check
    {
        ELOG1(ETckRunner, "server.mLaunchReqs.size()=%d", server.mLaunchReqs.size());
        CHECK(server.mLaunchReqs.size() == TCK_RUN_LIMIT);
        while (!server.mLaunchReqs.empty())
        {
            server.mLaunchReqs.pop();
        }
    }
}




