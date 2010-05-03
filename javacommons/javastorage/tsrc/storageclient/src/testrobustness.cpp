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

#include "javastorage.h"
#include "javastorageexception.h"
#include "javastoragenames.h"
#include "storagetestutils.h"

using namespace std;
using namespace java::storage;

bool executeStep(JavaStorage& aJs,
                 JavaStorageTestUtils& aJtu,
                 string& aTableName,
                 wstring& aValue);

TEST_GROUP(TestRobustness)
{
    JavaStorage* js;
    JavaStorageTestUtils* jtu;

    TEST_SETUP()
    {
        js = JavaStorage::createInstance();
        jtu = new JavaStorageTestUtils();
    }
    TEST_TEARDOWN()
    {
        try
        {
            js->rollbackTransaction();
            js->close();
        }
        catch (...)
        {
            // No can do
        }

        delete js;
        js = NULL;
        delete jtu;
        jtu = NULL;
    }
};

/**
 * Test escaped values.
 * 1. Test ' char is escaped.
 * 2. Test ' char twice at row.
 * 3. Test ' char twice.
 * 4. Test ' char only.
 * 5. Test ' char first.
 * 6. Test char last.
 * 7. Test only two escaped char.
 */
TEST(TestRobustness, TestEscapedValues)
{
    // 1. Test ' char is escaped.
    try
    {
        js->open();
        js->startTransaction();

        string tableName = APPLICATION_PACKAGE_TABLE;

        JavaStorageApplicationEntry_t appEntry;
        JavaStorageEntry entry;
        entry.setEntry(VENDOR, L"I'm under construction");
        appEntry.insert(entry);

        entry.setEntry(ID, L"Tr1");
        appEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, appEntry));
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));
        CHECK(jtu->remove(*js, tableName, appEntry));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp1\n");
    }

    // 2. Test ' char twice at row.
    try
    {
        js->open();
        js->startTransaction();

        string tableName = APPLICATION_PACKAGE_TABLE;

        JavaStorageApplicationEntry_t appEntry;
        JavaStorageEntry entry;
        entry.setEntry(VENDOR, L"H''oHoi");
        appEntry.insert(entry);

        entry.setEntry(ID, L"Tr2");
        appEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, appEntry));
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));
        CHECK(jtu->remove(*js, tableName, appEntry));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp2\n");
    }

    // 3. Test ' char twice.
    try
    {
        js->open();
        js->startTransaction();

        string tableName = APPLICATION_PACKAGE_TABLE;

        JavaStorageApplicationEntry_t appEntry;
        JavaStorageEntry entry;
        entry.setEntry(VENDOR, L"Hello, 'surrounded' by giants!");
        appEntry.insert(entry);

        entry.setEntry(ID, L"Tr3");
        appEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, appEntry));
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));
        CHECK(jtu->remove(*js, tableName, appEntry));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp3\n");
    }

    // 4. Test ' char only.
    try
    {
        js->open();
        js->startTransaction();

        string tableName = APPLICATION_PACKAGE_TABLE;

        JavaStorageApplicationEntry_t appEntry;
        JavaStorageEntry entry;
        entry.setEntry(VENDOR, L"'");
        appEntry.insert(entry);

        entry.setEntry(ID, L"Tr4");
        appEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, appEntry));
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));
        CHECK(jtu->remove(*js, tableName, appEntry));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp3\n");
    }

    // 5. Test ' char first.
    try
    {
        js->open();
        js->startTransaction();

        string tableName = APPLICATION_PACKAGE_TABLE;

        JavaStorageApplicationEntry_t appEntry;
        JavaStorageEntry entry;
        entry.setEntry(VENDOR, L"'Ello");
        appEntry.insert(entry);

        entry.setEntry(ID, L"Tr5");
        appEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, appEntry));
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));
        CHECK(jtu->remove(*js, tableName, appEntry));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp5\n");
    }

    // 6. Test char last.
    try
    {
        js->open();
        js->startTransaction();

        string tableName = APPLICATION_PACKAGE_TABLE;

        JavaStorageApplicationEntry_t appEntry;
        JavaStorageEntry entry;
        entry.setEntry(VENDOR, L"Hi'");
        appEntry.insert(entry);

        entry.setEntry(ID, L"Tr6");
        appEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, appEntry));
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));
        CHECK(jtu->remove(*js, tableName, appEntry));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp6\n");
    }

    // 7. Only two escaped char.
    try
    {
        js->open();
        js->startTransaction();

        string tableName = APPLICATION_PACKAGE_TABLE;

        JavaStorageApplicationEntry_t appEntry;
        JavaStorageEntry entry;
        entry.setEntry(VENDOR, L"''");
        appEntry.insert(entry);

        entry.setEntry(ID, L"Tr7");
        appEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, appEntry));
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));
        CHECK(jtu->remove(*js, tableName, appEntry));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp7\n");
    }
}

/**
 * Test hard characters.
 * 1. Test ! char.
 * 2. Test $ char.
 * 3. Test € char.
 */
TEST(TestRobustness, TestCharacters)
{
    string tableName = APPLICATION_PACKAGE_TABLE;

    // 1. Test ! char.
    wstring value = L"Hello!";
    CHECK(executeStep(*js, *jtu, tableName, value));

    // 2. Test $ char.
    value = L"500$";
    CHECK(executeStep(*js, *jtu, tableName, value));

    // 3. Test € char.
    value = L"800€";
    CHECK(executeStep(*js, *jtu, tableName, value));
}

bool executeStep(JavaStorage& aJs,
                 JavaStorageTestUtils& aJtu,
                 string& aTableName,
                 wstring& aValue)
{
    bool result = true;
    try
    {
        aJs.open();
        aJs.startTransaction();


        JavaStorageApplicationEntry_t appEntry;
        JavaStorageEntry entry;
        entry.setEntry(VENDOR, aValue);
        appEntry.insert(entry);

        if (!aJtu.populate(aJs, aTableName, appEntry))
        {
            printf("\nPopulate Failed\n");
            aJs.rollbackTransaction();
            return false;
        }

        if (!aJtu.checkParams(aJs, aTableName, appEntry, 1))
        {
            printf("\nCheck Failed\n");
            aJs.rollbackTransaction();
            return false;
        }

        if (!aJtu.remove(aJs, aTableName, appEntry))
        {
            printf("\nRemove Failed\n");
            aJs.rollbackTransaction();
            return false;
        }

        aJs.rollbackTransaction();
        result = true;
    }
    catch (...)
    {
        aJs.rollbackTransaction();
    }
    return result;
}
