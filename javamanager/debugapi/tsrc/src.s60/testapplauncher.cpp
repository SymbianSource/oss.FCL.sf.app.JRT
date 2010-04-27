/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Tests for AppLauncher class
*
*/

#include "applauncher.h"

#include "TestHarness.h"
#include "logger.h"
#include "commsmessage.h"
#include "rtcmessages.h"

using namespace std;
using namespace java::debug;
using namespace java::comms;
using namespace java::captain;
using java::util::Uid;

class TestAppLauncher : public AppLauncher
{
public:
    TestAppLauncher(const Uid& aAppUid) : AppLauncher(aAppUid),
            fail(false), createStartCalled(false), createStopCalled(false), sendToCalled(false) {};
    virtual ~TestAppLauncher() {};

    virtual CommsMessage createStartAppMessage(const std::wstring& aJvmArgs)
    {
        createStartCalled = true;
        return AppLauncher::createStartAppMessage(aJvmArgs);
    };
    virtual CommsMessage createStopAppMessage()
    {
        createStopCalled = true;
        return AppLauncher::createStopAppMessage();
    };
    virtual int sendToJavaCaptain(CommsMessage& aMessage)
    {
        sendToCalled = true;
        if (fail) return -666;
        return AppLauncher::sendToJavaCaptain(aMessage);
    };

    bool fail;
    bool createStartCalled;
    bool createStopCalled;
    bool sendToCalled;
};


TEST_GROUP(TestAppLauncher)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};


TEST(TestAppLauncher, startApp)
{
    Uid appUid(L"[12345678]");
    wstring args = L"hello world";

    TestAppLauncher launcher(appUid);
    int rc = launcher.startApp(args);

    CHECK(launcher.createStartCalled);
    CHECK(launcher.sendToCalled);
}

TEST(TestAppLauncher, startAppFail)
{
    Uid appUid(L"[12345678]");
    wstring args = L"hello world";

    TestAppLauncher launcher(appUid);
    launcher.fail = true;
    int rc = launcher.startApp(args);

    CHECK(rc != 0);
    CHECK(launcher.createStartCalled);
    CHECK(launcher.sendToCalled);
}

TEST(TestAppLauncher, stopApp)
{
    Uid appUid(L"[12345678]");

    TestAppLauncher launcher(appUid);
    int rc = launcher.stopApp();

    CHECK(rc == 0);
    CHECK(launcher.createStopCalled);
    CHECK(launcher.sendToCalled);
}

TEST(TestAppLauncher, stopAppFail)
{
    Uid appUid(L"[12345678]");

    TestAppLauncher launcher(appUid);
    launcher.fail = true;
    int rc = launcher.stopApp();

    CHECK(rc != 0);
    CHECK(launcher.createStopCalled);
    CHECK(launcher.sendToCalled);
}


TEST(TestAppLauncher, createStartAppMessage)
{
    Uid appUid(L"[12345678]");
    wstring args = L"hello world";

    TestAppLauncher launcher(appUid);
    CommsMessage msg = launcher.createStartAppMessage(args);

    CHECK(msg.getMessageId() == RTC_MSG_ID_LAUNCH_APPLICATION_REQ);
    CHECK(msg.getModuleId() == PLUGIN_ID_RTC_C);

    Uid uid;
    int launchType = 0;
    int options = 0;
    std::string rtc;
    std::wstring appArgs;
    std::wstring jvmArgs;

    getLaunchApplicationReqParams(msg, uid, launchType, options, rtc, appArgs, jvmArgs);

    CHECK(uid == appUid);
    CHECK(launchType == RTC_LAUNCH_TYPE_DEBUG_C);
    CHECK(options == RTC_LAUNCH_OPTIONS_RUNNING_IND_REQ_C);
    CHECK(rtc.compare(RTC_LAUNCH_RUNTIME_MIDP_C) == 0);
    CHECK(appArgs.compare(L"") == 0);
    CHECK(jvmArgs.compare(args) == 0);
}

TEST(TestAppLauncher, createStopAppMessage)
{
    Uid appUid(L"[12345678]");

    TestAppLauncher launcher(appUid);
    CommsMessage msg = launcher.createStopAppMessage();

    CHECK(msg.getMessageId() == RTC_MSG_ID_TERMINATE_APPLICATION_REQ);
    CHECK(msg.getModuleId() == PLUGIN_ID_RTC_C);

    Uid uid;
    int options = 0;

    getTerminateApplicationReqParams(msg, uid, options);
    CHECK(uid == appUid);
    CHECK(options == RTC_LAUNCH_OPTIONS_TERMINATE_IND_REQ_C);
}

TEST(TestAppLauncher, sendToJavaCaptain)
{
    Uid appUid(L"[12345678]");

    TestAppLauncher launcher(appUid);
    CommsMessage msg = launcher.createStopAppMessage();

    int rc = launcher.sendToJavaCaptain(msg);
    CHECK(rc == 0);

    launcher.fail = true;
    rc = launcher.sendToJavaCaptain(msg);
    CHECK(rc != 0);
}
