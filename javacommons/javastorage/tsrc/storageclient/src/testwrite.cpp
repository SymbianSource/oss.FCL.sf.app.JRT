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

TEST_GROUP(TestWrite)
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
        js = 0;
        delete jtu;
        jtu = 0;
    }
};

/**
 * 1. Test empty tableName throws JSE.
 * 2. Test empty entry throws JSE.
 * 3. Test non-existent table throws JSE.
 * 4. Test write non-existent entry throws JSE.
 * 5. Test write no connection throws JSE.
 */
TEST(TestWrite, TestWriteArguments)
{
    string tableName = APPLICATION_PACKAGE_TABLE;
    string dbName = JAVA_DATABASE_NAME;
    js->open(dbName);

    JavaStorageApplicationEntry_t appEntry;
    JavaStorageEntry entry;

    entry.setEntry(JAD_PATH, L"/usr/local/bin/");
    appEntry.insert(entry);

    entry.setEntry(JAR_PATH, L"/usr/share/private/[abcs12345]/");
    appEntry.insert(entry);

    try
    {
        // 1. Test with empty tableName
        string empty = "";
        js->write(empty, appEntry);
        FAIL("No exception while calling write with empty tableName\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidArgument == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while calling write with empty tableName\n");
    }

    //2. Test empty entry throws JSE.
    try
    {
        JavaStorageApplicationEntry_t emptyEntry;
        js->write(tableName, emptyEntry);
        FAIL("No exception while calling write with empty entry\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidArgument == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while calling write with empty entry\n");
    }

    try
    {
        string tableName = "FOO_BAR";

        // 4. Test with non-existent table
        js->write(tableName, appEntry);
        FAIL("No exception while calling write with nonexistent table\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
    }
    catch (...)
    {
        FAIL("Unexpected exp while calling write with nonexistent table\n");
    }

    try
    {
        JavaStorageApplicationEntry_t appEntry2;
        entry.setEntry(L"FOO_BAR", L"/usr/local/bin/");
        appEntry2.insert(entry);

        // 5. Test with non-existing entry
        js->write(tableName, appEntry2);

        FAIL("No exception while calling write with non-existing entry\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
    }
    catch (...)
    {
        FAIL("Unexpected exp while calling write with non-existing entry\n");
    }

    js->close();

    try
    {
        // 6. Test write after connection close.
        js->write(tableName, appEntry);

        FAIL("No exception while calling write after connection close\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidConnection == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while calling write after connection close\n");
    }
}

/**
 * 1. Test data is written to database.
 * 2. Test write one entry.
 * 3. Test write whole table.
 * 4. Test write string to INT entry. JSE must be thrown.
 * 5. Test write entry with existing primaryKey. JSE must be thrown.
 * 6. Test write two same entries. JSE must be thrown.
 * 7. Test write no name. JSE must be thrown.
 * 8. Test write no value. Entries are written to storage.
 */
TEST(TestWrite, TestWrite)
{
    string tableName = APPLICATION_PACKAGE_TABLE;
    JavaStorageEntry entry;

    // 1. Test data is written to DB
    try
    {
        js->open();
        js->startTransaction();

        JavaStorageApplicationEntry_t defaultEntry;
        jtu->createDefaultEntry(defaultEntry);
        entry.setEntry(ID, L"TestWrite1");
        defaultEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, defaultEntry));
        CHECK(jtu->checkParams(*js, tableName, defaultEntry, 1));
        CHECK(jtu->remove(*js, tableName, defaultEntry));
        js->rollbackTransaction();
    }
    catch (JavaStorageException& jse)
    {
        js->rollbackTransaction();
        string printStr = "JSE while writing: " + jse.toString();
        FAIL(printStr.c_str());
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Exception while start transaction without open connection\n");
    }

    // 2. Write one column only
    try
    {
        string tableName = APPLICATION_TABLE;
        JavaStorageApplicationEntry_t appEntry;
        JavaStorageEntry entry;

        entry.setEntry(ID, L"TestWrite2");
        appEntry.insert(entry);

        entry.setEntry(NAME, L"MIDlet-1");
        appEntry.insert(entry);

        js->startTransaction();
        CHECK(jtu->populate(*js, tableName, appEntry));
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));
        CHECK(jtu->remove(*js, tableName, appEntry));
        js->rollbackTransaction();
    }
    catch (JavaStorageException& jse)
    {
        js->rollbackTransaction();
        string printStr = "JSE while writing: " + jse.toString();
        FAIL(printStr.c_str());
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Exception while start transaction without open connection\n");
    }

    // 3. Write whole table
    try
    {
        string tableName = APPLICATION_PACKAGE_TABLE;
        JavaStorageApplicationEntry_t appEntry;

        // Fill all column values.
        jtu->createAppDataEntry(appEntry);
        entry.setEntry(ID, L"TestWrite3");
        appEntry.insert(entry);

        js->startTransaction();
        CHECK(jtu->populate(*js, tableName, appEntry));
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));
        CHECK(jtu->remove(*js, tableName, appEntry));
        js->rollbackTransaction();
    }
    catch (JavaStorageException& jse)
    {
        js->rollbackTransaction();
        string printStr = "JSE while writing: " + jse.toString();
        FAIL(printStr.c_str());
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Exception while start transaction without open connection\n");
    }

    // 4. Write string to INT entry
    try
    {
        js->startTransaction();
        string tableName = APPLICATION_PACKAGE_TABLE;

        JavaStorageApplicationEntry_t appEntry;
        JavaStorageEntry entry;
        entry.setEntry(INITIAL_SIZE, L"I_am_String", JavaStorageEntry::INT);
        appEntry.insert(entry);

        entry.setEntry(ID, L"TestWrite4");
        appEntry.insert(entry);

        js->write(tableName, appEntry);
        FAIL("Storing string to INT entry do not throw JSE");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp. while storing string to INT entry\n");
    }

    // 5. Test write entry with existing primaryKey. JSE must be thrown.
    try
    {
        string tableName = APPLICATION_PACKAGE_TABLE;
        JavaStorageApplicationEntry_t appEntry;
        JavaStorageEntry entry;

        entry.setEntry(PACKAGE_NAME, L"MyName");
        appEntry.insert(entry);

        entry.setEntry(ID, L"TestWrite5");
        appEntry.insert(entry);

        js->startTransaction();
        js->write(tableName, appEntry);
        js->write(tableName, appEntry);

        FAIL("Using existing primary key did not throw exception");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp. while using same primarykey\n");
    }

    // 6. Test write two same entries. First one is considered.
    try
    {
        string tableName = APPLICATION_PACKAGE_TABLE;

        JavaStorageApplicationEntry_t appEntry;
        JavaStorageEntry entry;
        entry.setEntry(PACKAGE_NAME, L"MyName");
        appEntry.insert(entry);

        entry.setEntry(PACKAGE_NAME, L"MyName2");
        appEntry.insert(entry);

        entry.setEntry(ID, L"TestWrite6");
        appEntry.insert(entry);

        js->startTransaction();
        js->write(tableName, appEntry);

        // Check first one is considered
        JavaStorageApplicationEntry_t checkEntry;
        entry.setEntry(PACKAGE_NAME, L"MyName");
        checkEntry.insert(entry);

        CHECK(jtu->checkParams(*js, tableName, checkEntry, 1));

        // Check second one is ignored
        checkEntry.clear();
        entry.setEntry(PACKAGE_NAME, L"MyName2");
        checkEntry.insert(entry);

        CHECK(jtu->checkParams(*js, tableName, checkEntry, 0));

        // Clean
        checkEntry.clear();
        entry.setEntry(PACKAGE_NAME, L"MyName");
        checkEntry.insert(entry);
        CHECK(jtu->remove(*js, tableName, checkEntry));
        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp. while using same primarykey\n");
    }

    // 7. Test write no name. JSE must be thrown.
    try
    {
        string tableName = APPLICATION_PACKAGE_TABLE;
        JavaStorageApplicationEntry_t appEntry;
        JavaStorageEntry entry;
        entry.setEntry(PACKAGE_NAME, L"MyName");
        appEntry.insert(entry);

        entry.setEntry(L"", L"MyEmptyNameParam");
        appEntry.insert(entry);

        entry.setEntry(VENDOR, L"MyVendor3");
        appEntry.insert(entry);

        entry.setEntry(ID, L"TestWrite7");
        appEntry.insert(entry);

        js->startTransaction();
        js->write(tableName, appEntry);

        FAIL("Using empty attribute name didn't throw exception");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp. while using same primarykey\n");
    }

    // 8. Test write no value. Entries are written to storage.
    try
    {
        string tableName = APPLICATION_TABLE;
        JavaStorageApplicationEntry_t appEntry;
        JavaStorageApplicationEntry_t compareEntry;

        JavaStorageEntry entry;
        entry.setEntry(NAME, L"MyName");
        appEntry.insert(entry);
        compareEntry.insert(entry);

        entry.setEntry(MAIN_CLASS, L"");
        appEntry.insert(entry);

        entry.setEntry(PACKAGE_ID, L"MySuite");
        appEntry.insert(entry);
        compareEntry.insert(entry);

        entry.setEntry(ID, L"TestWrite8");
        appEntry.insert(entry);
        compareEntry.insert(entry);

        js->startTransaction();

        CHECK(jtu->populate(*js, tableName, appEntry));
        CHECK(jtu->checkParams(*js, tableName, compareEntry, 1));
        CHECK(jtu->remove(*js, tableName, appEntry));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp. while using empty value\n");
    }
    js->close();
}
