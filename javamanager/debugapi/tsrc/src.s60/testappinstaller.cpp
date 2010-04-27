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
* Description:  Tests for AppInstaller class
*
*/

#include "appinstaller.h"

#include "TestHarness.h"
#include "logger.h"
#include "commsmessage.h"


using namespace std;
using namespace java::debug;
using namespace java::comms;
using java::util::Uid;

class TestAppInstaller : public AppInstaller
{
public:
    TestAppInstaller(const std::wstring& aFilename) : AppInstaller(aFilename),
            nextOperationCount(0), handleOperationCount(0) {};
    virtual ~TestAppInstaller() {};

    virtual CommsMessage getNextOperation()
    {
        nextOperationCount++;
        return AppInstaller::getNextOperation();
    };

    virtual void handleOperationResult(CommsMessage& aMessage)
    {
        handleOperationCount++;
        AppInstaller::handleOperationResult(aMessage);
    };

    int nextOperationCount;
    int handleOperationCount;
};


TEST_GROUP(TestAppInstaller)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};


TEST(TestAppInstaller, install)
{
    TestAppInstaller installer(L"dummy.jar");

    int rc = installer.install();
    CHECK(rc != 0);
    CHECK(installer.nextOperationCount == 2);
    CHECK(installer.handleOperationCount == 2);
}

TEST(TestAppInstaller, getNextOperation)
{
    wstring dummy(L"dummy.jar");
    TestAppInstaller installer(dummy);

    CommsMessage msg = installer.getNextOperation();
    int operation = -1;
    wstring filename;

    msg >> operation >> filename;
    CHECK(operation == INSTALL_OPERATION);
    CHECK(filename.compare(dummy) == 0);

    // install is not done yet so next operation must be still INSTALL
    msg = installer.getNextOperation();
    msg >> operation >> filename;
    CHECK(operation == INSTALL_OPERATION);
    CHECK(filename.compare(dummy) == 0);

    CommsMessage result;
    Uid appUid(L"[10000001]");
    Uid suiteUid(L"[30000003]");
    result << INSTALL_OPERATION << 0 << 1 << appUid << suiteUid;
    installer.handleOperationResult(result);

    msg = installer.getNextOperation();
    msg >> operation;
    CHECK(operation == EXIT_OPERATION);
}

TEST(TestAppInstaller, handleOperationResult)
{
    CommsMessage msg;
    Uid appUid1(L"[10000001]");
    Uid appUid2(L"[20000002]");
    Uid suiteUid(L"[30000003]");

    msg << INSTALL_OPERATION << 0 << 2 << appUid1 << appUid2 << suiteUid;

    TestAppInstaller installer(L"dummy.jar");
    installer.handleOperationResult(msg);
    std::list<Uid> appUids = installer.getAppUids();

    CHECK(suiteUid == installer.getSuiteUid());
    CHECK(appUids.size() == 2);
    CHECK(appUids.front() == appUid1);
    CHECK(appUids.back() == appUid2);
}

