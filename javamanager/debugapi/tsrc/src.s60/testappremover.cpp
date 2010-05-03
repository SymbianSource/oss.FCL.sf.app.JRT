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
* Description:  Tests for AppRemover class
*
*/

#include "appremover.h"

#include "TestHarness.h"
#include "logger.h"
#include "commsmessage.h"


using namespace std;
using namespace java::debug;
using namespace java::comms;
using java::util::Uid;

class TestAppRemover : public AppRemover
{
public:
    TestAppRemover(const Uid& aSuiteUid) : AppRemover(aSuiteUid),
            nextOperationCount(0), handleOperationCount(0) {};
    virtual ~TestAppRemover() {};

    virtual CommsMessage getNextOperation()
    {
        nextOperationCount++;
        return AppRemover::getNextOperation();
    };

    virtual void handleOperationResult(CommsMessage& aMessage)
    {
        handleOperationCount++;
        AppRemover::handleOperationResult(aMessage);
    };

    int nextOperationCount;
    int handleOperationCount;
};


TEST_GROUP(TestAppRemover)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};


TEST(TestAppRemover, uninstall)
{
    Uid suiteUid(L"[87654321]");
    TestAppRemover installer(suiteUid);

    int rc = installer.uninstall();
    CHECK(rc != 0);
    CHECK(installer.nextOperationCount == 2);
    CHECK(installer.handleOperationCount == 2);
}

TEST(TestAppRemover, getNextOperation)
{
    Uid suiteUid(L"[87654321]");
    TestAppRemover installer(suiteUid);

    CommsMessage msg = installer.getNextOperation();
    int operation = -1;

    msg >> operation;
    CHECK(operation == UNINSTALL_OPERATION);

    // uninstall is not done yet so next operation must be still UNINSTALL
    msg = installer.getNextOperation();
    msg >> operation;
    CHECK(operation == UNINSTALL_OPERATION);

    CommsMessage result;
    result << UNINSTALL_OPERATION << 0;
    installer.handleOperationResult(result);

    msg = installer.getNextOperation();
    msg >> operation;
    CHECK(operation == EXIT_OPERATION);
}

TEST(TestAppRemover, handleOperationResult)
{
    CommsMessage msg;
    msg << UNINSTALL_OPERATION << 0;

    Uid suiteUid(L"[87654321]");
    TestAppRemover installer(suiteUid);

    installer.handleOperationResult(msg);
    CHECK(installer.handleOperationCount == 1);
}

