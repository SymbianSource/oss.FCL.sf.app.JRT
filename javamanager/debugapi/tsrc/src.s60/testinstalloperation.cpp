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
* Description:  Tests for InstallOperation class
*
*/

#include "installoperation.h"
#include "TestHarness.h"
using namespace java::debug;

class TestInstallOperation : public InstallOperation
{
public:
    TestInstallOperation():mError(0) {};

    using InstallOperation::runJavaInstaller;
    virtual int startJavaInstaller()
    {
        return mError;
    };

    void setError(int aError)
    {
        mError = aError;
    };
private:
    int mError;
};


TEST_GROUP(TestInstallOperation)
{
    TestInstallOperation runner;
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

TEST(TestInstallOperation, runOk)
{
    int rc = runner.runJavaInstaller();
    CHECK(rc == 0);
}

TEST(TestInstallOperation, runAlreadyExists)
{
    runner.setError(KErrAlreadyExists);
    int rc = runner.runJavaInstaller();
    CHECK(rc == KErrAlreadyExists);
}

TEST(TestInstallOperation, runError)
{
    runner.setError(KErrGeneral);
    int rc = runner.runJavaInstaller();
    CHECK(rc == KErrGeneral);
}
