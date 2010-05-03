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
* Description:
*
*/

#include <e32cmn.h>
#include <stdio.h>
#include <string>

#include "TestHarness.h"
#include <javaenvinfo.h>
#include "logger.h"

using namespace Java;

TEST_GROUP(TestJavaEnvInfoAPI)
{

    TEST_SETUP()
    {
    }
    TEST_TEARDOWN()
    {
    }
};

/**
 * Test JavaEnvInfo::Version() method.
 *   1. Test version can be read.
 */
TEST(TestJavaEnvInfoAPI, TestVersion)
{
    LOG(EJavaStorage, EInfo, "+TestVersion");
    TVersion version = JavaEnvInfo::Version();

    // Version changes across platform release hence cannot exactly checked.
    CHECK(version.iBuild >= 0);
    CHECK(version.iMajor >= 0);
    CHECK(version.iMinor >= 0);
    LOG(EJavaStorage, EInfo, "-TestVersion");
}

/**
 * Test JavaEnvInfo::GetUserAgentHeaderL() method.
 *   1. Test headers can be read.
 */
TEST(TestJavaEnvInfoAPI, TestGetUserAgentHeaderL)
{
    LOG(EJavaStorage, EInfo, "+TestGetUserAgentHeaderL");
    std::auto_ptr<HBufC> headerBuf(JavaEnvInfo::GetUserAgentHeaderL());

    TPtrC headerPtr(headerBuf->Des());

    // Headers changes across platform release hence cannot exactly checked.
    CHECK(headerBuf.get() != 0);
    CHECK(headerPtr.Size() > 0)

    LOG(EJavaStorage, EInfo, "-TestGetUserAgentHeaderL");
}

/**
 * Test JavaEnvInfo::GetPlatformInfoL() method.
 *   1. Test platform info can be read.
 */
TEST(TestJavaEnvInfoAPI, TestGetPlatformInfoL)
{
    LOG(EJavaStorage, EInfo, "+TestGetPlatformInfoL");
    std::auto_ptr<HBufC> headerBuf(JavaEnvInfo::GetPlatformInfoL());

    TPtrC headerPtr(headerBuf->Des());

    // Info varies across platform releases hence cannot be exactly checked.
    CHECK(headerBuf.get() != 0);
    CHECK(headerPtr.Size() > 0)

    LOG(EJavaStorage, EInfo, "-TestGetPlatformInfoL");
}

