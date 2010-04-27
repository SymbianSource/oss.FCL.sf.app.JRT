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
#include <memory>

#include "TestHarness.h"

#include "javastorage.h"
#include "javastorageexception.h"
#include "javastoragenames.h"
#include "javauid.h"
#include "logger.h"
#include "storagetestutils.h"

using namespace std;
using namespace java::storage;
using namespace java::util;

TEST_GROUP(TestConnection)
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

            // Temporary until multiconnection implemented
            js->open();
            JavaStorageApplicationEntry_t defaultEntry;
            jtu->createDefaultEntry(defaultEntry);
            jtu->remove(*js, APPLICATION_PACKAGE_TABLE, defaultEntry);
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
 * Test connection open
 * 1. Test open.
 * 2. Test open with empty name. JavaStorageException is thrown.
 * 3. Test two consecutive opens. Second call is silenty ignored.
 * 4. Test re-open.
 * 5. Test two connections to same target can be opened.
 * 6. Test two connections to separate targets can be opened.
 * 7. Test open to non-existing database goes ok and DB file is created.
 */
TEST(TestConnection, OpenConnection)
{
    string dbName = JAVA_DATABASE_NAME;
    string tableName = APPLICATION_PACKAGE_TABLE;
    JavaStorageEntry entry;
    JavaStorageApplicationEntry_t defaultEntry;

    // 1. Test Open
    try
    {
        js->open(dbName);

        jtu->createDefaultEntry(defaultEntry);
        entry.setEntry(ID, L"testConnection1");
        defaultEntry.insert(entry);

        jtu->remove(*js, tableName, defaultEntry);

        // Write and read to verify connection
        CHECK(jtu->populate(*js, tableName, defaultEntry));
        CHECK(jtu->checkParams(*js, tableName, defaultEntry, 1));
        js->close();
    }
    catch (...)
    {
        FAIL("Failed to open database\n");
    }

    // 2. Test open with empty name
    string empty = "";
    try
    {
        js->open(empty);
        FAIL("Calling with empty DB name did not thrown exception\n");
    }
    catch (JavaStorageException)
    {
    }
    catch (...)
    {
        FAIL("Wrong exception thrown when opening DB with empty name\n");
    }

    // 3. Test two consecutive opens
    js->open(dbName);

    // This is silently ignored
    try
    {
        js->open(dbName);
        // But data can be read
        CHECK(jtu->checkParams(*js, tableName, defaultEntry, 1));
    }
    catch (...)
    {
        FAIL("Second open was not silently ignored\n");
    }

    js->close();

    // 4. Test re-open
    js->open(dbName);
    // Read some data to verify re-open
    CHECK(jtu->checkParams(*js, tableName, defaultEntry, 1));

    // 5. Two connections to same target.
    try
    {
        auto_ptr<JavaStorage> js2(JavaStorage::createInstance());

        js2->open(dbName);
        CHECK(jtu->checkParams(*js, tableName, defaultEntry, 1));
        CHECK(jtu->checkParams(*js2, tableName, defaultEntry, 1));

        js2->close();
    }
    catch (...)
    {
        FAIL("Opening second connection to same target failed\n");
    }

    // 6. and 7. Two connections to different target.
    try
    {
        // File must be created to public area that test case hase rights
        // to remove it.
        string dbName2 = "SecondTarget";
        string pathPrefix;
        jtu->databasePath(pathPrefix);
        dbName2.insert(0, pathPrefix);

        WLOG1(EJavaStorage, "TC: DB Path: %s", dbName2.c_str());

        auto_ptr<JavaStorage> js2(JavaStorage::createInstance());
        js2->open(dbName2);

        string tableName = "MY_FIRST_TABLE";
        JavaStorageApplicationEntry_t appEntry;
        wstring name = L"LOCATION";
        wstring nameValue = L"/usr/local/bin/";
        JavaStorageEntry entry;

        entry.setEntry(name, nameValue, JavaStorageEntry::STRING);
        appEntry.insert(entry);

        name = L"PATH";
        nameValue = L"/usr/share/private/[abcs12345]/";

        entry.setEntry(name, nameValue, JavaStorageEntry::STRING);
        appEntry.insert(entry);

        // Create table as new DB.
        js2->createTable(tableName, appEntry);

        CHECK(jtu->populate(*js2, tableName, appEntry));
        CHECK(jtu->checkParams(*js2, tableName, appEntry, 1));
        CHECK(jtu->remove(*js2, tableName, appEntry));

        js2->close();

        // If removal works data file existed.
        WLOG1(EJavaStorage, "TC: Removing second storage: %s",
              dbName2.c_str());
        CHECK(remove(dbName2.c_str()) == 0);
    }
    catch (...)
    {
        FAIL("Opening second connection to different target failed\n");
    }

    CHECK(jtu->remove(*js, tableName, defaultEntry));
    js->close();
}

/**
 * Test connection close.
 * 1. Test close not open connection. Call is silently ignored.
 * 2. Close connection. JavaStorage cannot be read after that i.e. call is
 *    silently ignored.
 * 3. Test two consecutive close.
 */
TEST(TestConnection, TestClose)
{
    string dbName = JAVA_DATABASE_NAME;
    string tableName = APPLICATION_PACKAGE_TABLE;
    JavaStorageEntry entry;
    JavaStorageApplicationEntry_t defaultEntry;

    try
    {
        // 1. Test close not opened
        js->close();

        // 2. Close connection
        js->open(dbName);

        jtu->createDefaultEntry(defaultEntry);
        entry.setEntry(ID, L"testConnection2");
        defaultEntry.insert(entry);

        // Write and read to verify connection
        CHECK(jtu->populate(*js, tableName, defaultEntry));
        CHECK(jtu->checkParams(*js, tableName, defaultEntry, 1));


        js->close();
        // Data cannot be read after close.
        CHECK_EQUAL(jtu->checkParams(*js, tableName, defaultEntry, 1), false);

        // 3. Two concecutive close
        js->open(dbName);
        CHECK(jtu->remove(*js, tableName, defaultEntry));

        js->close();
        js->close();
    }
    catch (...)
    {
        FAIL("Unexpected exception\n");
    }
}
