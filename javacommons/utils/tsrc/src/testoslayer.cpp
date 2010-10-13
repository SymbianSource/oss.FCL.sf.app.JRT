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

#include <stdio.h>
#include <string>

#include "TestHarness.h"

#include "javacommonutils.h"
#include "javaoslayer.h"

using namespace std;
using namespace java::util;

TEST_GROUP(TestOsLayer)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

/**
 * Test JavaOsLayer::bootClassPath().
 *
 * 1. Test midp cp.
 * 2. Test installer cp.
 * 3. Test tckrunner cp.
 * 4. Test negative type.
 * 5. Test positive greater boyndary.
 */
TEST(TestOsLayer, bootClassPath)
{
    // 1. Test midp cp.
    list <wstring> odcs;
    list <wstring> bc;
    const wstring delim = L";";
    JavaOsLayer::bootClassPath(odcs, bc, BOOT_CLASSPATH_MIDP);
    CHECK(odcs.size() > 35);
    CHECK(bc.size() == 1);
    odcs.clear();
    bc.clear();

    // 2. Test installer cp.
    JavaOsLayer::bootClassPath(odcs, bc, BOOT_CLASSPATH_INSTALLER);
    CHECK(odcs.size() > 24);
    CHECK(bc.size() == 1);
    odcs.clear();
    bc.clear();

    // 3. Test tckrunner cp.
    JavaOsLayer::bootClassPath(odcs, bc, BOOT_CLASSPATH_TCKRUNNER);
    CHECK(bc.size() > 10);
    CHECK(bc.size() == 1);
    odcs.clear();
    bc.clear();

    // 4. Test negative type.
    JavaOsLayer::bootClassPath(odcs, bc, -1);
    CHECK(odcs.size() > 35);
    CHECK(bc.size() == 1);
    odcs.clear();
    bc.clear();

    // 5. Test positive greater boyndary.
    JavaOsLayer::bootClassPath(odcs, bc, 50);
    CHECK(odcs.size() > 4);
    CHECK(bc.size() == 1);
    odcs.clear();
    bc.clear();
}
