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

void removeTestJar(list <wstring>& list, const wstring& jar)
{

    std::list<wstring>::iterator iter = find(list.begin(), list.end(), jar);
    if (iter != list.end())
    {
        list.erase(iter);
    }
}

void removeTestJars(list <wstring>& list)
{
    removeTestJar(list, L"Z:\\resource\\java\\jvm\\lib\\jrt\\javacommonutilstest.jar");
    removeTestJar(list, L"Z:\\resource\\java\\jvm\\lib\\jrt\\j2meunitomj.jar");
}

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
    removeTestJars(bc);
    CHECK(bc.size() == 1);
    odcs.clear();
    bc.clear();

    // 2. Test installer cp.
    JavaOsLayer::bootClassPath(odcs, bc, BOOT_CLASSPATH_INSTALLER);
    CHECK(odcs.size() > 24);
    removeTestJars(bc);
    CHECK(bc.size() == 1);
    odcs.clear();
    bc.clear();

    // 3. Test tckrunner cp.
    JavaOsLayer::bootClassPath(odcs, bc, BOOT_CLASSPATH_TCKRUNNER);
    CHECK(odcs.size() > 10);
    removeTestJars(bc);
    CHECK(bc.size() == 1);
    odcs.clear();
    bc.clear();

    // 4. Test negative type.
    JavaOsLayer::bootClassPath(odcs, bc, -1);
    CHECK(odcs.size() > 35);
    removeTestJars(bc);
    CHECK(bc.size() == 1);
    odcs.clear();
    bc.clear();

    // 5. Test positive greater boyndary.
    JavaOsLayer::bootClassPath(odcs, bc, 50);
    CHECK(odcs.size() > 4);
    removeTestJars(bc);
    CHECK(bc.size() == 1);
    odcs.clear();
    bc.clear();
}
