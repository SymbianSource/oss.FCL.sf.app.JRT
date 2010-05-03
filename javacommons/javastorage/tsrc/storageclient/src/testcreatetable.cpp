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

TEST_GROUP(TestCreateTable)
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
 * 2. Test empty schema throws JSE.
 * 3. Test no column type defined throws JSE.
 * 4. Test no column name defined throws JSE.
 * 5. Test create when connection already closed throws JSE.
 */
TEST(TestCreateTable, TestCreateArguments)
{
    string dbName;
    jtu->databasePath(dbName);
    dbName += "TestCreateDB1";

    js->open(dbName);

    string empty = "";
    string tableName = "MY_FIRST_TABLE";
    JavaStorageApplicationEntry_t appEntry;
    wstring nameValue = L"/usr/local/bin/";
    JavaStorageEntry entry;

    entry.setEntry(L"LOCATION", nameValue, JavaStorageEntry::STRING);
    appEntry.insert(entry);

    nameValue = L"/usr/share/private/[abcs12345]/";
    entry.setEntry(L"PATH", nameValue, JavaStorageEntry::STRING);
    appEntry.insert(entry);

    try
    {
        // 1. Test with empty tableName
        js->createTable(empty, appEntry);

        FAIL("No exception while createTable with empty tableName\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidArgument == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while createTable with empty tableName\n");
    }

    try
    {
        JavaStorageApplicationEntry_t emptyEntry;
        // 2. Test empty schema throws JSE
        js->createTable(tableName, emptyEntry);

        FAIL("No exception while createTable with empty entry\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidArgument == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while createTable with empty entry\n");
    }

    try
    {
        JavaStorageApplicationEntry_t invalidEntry;
        JavaStorageEntry entry;

        // No entry type defined
        entry.setEntry(L"LOCATION", L"/usr/local/bin/");
        invalidEntry.insert(entry);

        // 3. Test no column type defined throws JSE.
        js->createTable(tableName, invalidEntry);

        FAIL("No exception while createTable with no entry type defined\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidDataStructure == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while createTable with no entry type defined\n");
    }

    try
    {
        JavaStorageApplicationEntry_t invalidEntry;
        wstring name = L"";
        wstring nameValue = L"/usr/local/bin/";
        JavaStorageEntry entry;
        // No entry name defined
        entry.setEntry(name, nameValue, JavaStorageEntry::STRING);
        invalidEntry.insert(entry);

        // 4. Test no column type defined throws JSE.
        js->createTable(tableName, invalidEntry);

        FAIL("No exception while createTable with no entry name\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidArgument == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while createTable with no entry name\n");
    }

    js->close();

    try
    {
        // 5. Test create when connection already closed throws JSE.
        js->createTable(tableName, appEntry);

        FAIL("No exception while createTable without connection\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidConnection == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while createTable without connection\n");
    }

    // If removal works data file existed.
    printf("Removing storage: %s\n", dbName.c_str());
    CHECK(remove(dbName.c_str()) == 0);
}

/**
 * 1. Test create table. Data can be written to it.
 * 2. Test create one column. Data can be written to it.
 * 3. Test create string and int columns. Data can be written to them.
 * 4. Test create column with value. Check value is discarded.
 * 5. Test create two same columns. JSE is thrown.
 * 6. Test create two same tables. JSE is thrown.
 * 7. Test create two same name columns. Other column is string and other is
 *    int. JSE is thrown.
 */
TEST(TestCreateTable, TestCreate)
{
    // 1. Test create table. Data can be written to it.
    string dbName;
    jtu->databasePath(dbName);
    dbName += "TestCreateDB2";

    js->open(dbName);

    string tableName = "MY_SECOND_TABLE";
    JavaStorageApplicationEntry_t appEntry;
    wstring nameValue = L"";
    JavaStorageEntry entry;

    // ID is created as PRIMARY KEY
    entry.setEntry(ID, nameValue, JavaStorageEntry::STRING);
    appEntry.insert(entry);


    entry.setEntry(L"LOCATION", nameValue, JavaStorageEntry::STRING);
    appEntry.insert(entry);

    nameValue = L"";

    entry.setEntry(L"PATH", nameValue, JavaStorageEntry::STRING);
    appEntry.insert(entry);

    try
    {
        JavaStorageApplicationEntry_t populateEntry;

        entry.setEntry(L"LOCATION", L"MyGreatLocation");
        populateEntry.insert(entry);

        entry.setEntry(L"PATH", L"my/Path", JavaStorageEntry::STRING);
        populateEntry.insert(entry);

        entry.setEntry(ID, L"TestCreateTable1");
        populateEntry.insert(entry);

        js->startTransaction();
        js->createTable(tableName, appEntry);
        populateEntry.insert(entry);
        CHECK(jtu->populate(*js, tableName, populateEntry));
        CHECK(jtu->checkParams(*js, tableName, populateEntry, 1));
        CHECK(jtu->remove(*js, tableName, populateEntry));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while createTable\n");
    }

    // 2. Test create one column. Data can be written to it.
    try
    {
        tableName = "M";
        JavaStorageApplicationEntry_t populateEntry;
        nameValue = L"";

        entry.setEntry(ID, nameValue, JavaStorageEntry::STRING);
        populateEntry.insert(entry);

        js->startTransaction();
        js->createTable(tableName, populateEntry);
        populateEntry.clear();

        // Add value
        nameValue = L"TestApp4";
        entry.setEntry(ID, nameValue, JavaStorageEntry::STRING);
        populateEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, populateEntry));
        CHECK(jtu->checkParams(*js, tableName, populateEntry, 1));
        CHECK(jtu->remove(*js, tableName, populateEntry));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while createTable with one column\n");
    }

    // 3. Test create string and int columns. Data can be written to them.
    try
    {
        tableName = "PERSONAL_INFORMATION";
        JavaStorageApplicationEntry_t populateEntry;
        nameValue = L"";

        entry.setEntry(ID, nameValue, JavaStorageEntry::STRING);
        populateEntry.insert(entry);

        entry.setEntry(NAME, nameValue, JavaStorageEntry::STRING);
        populateEntry.insert(entry);

        entry.setEntry(L"AGE", nameValue, JavaStorageEntry::INT);
        populateEntry.insert(entry);

        js->startTransaction();
        js->createTable(tableName, populateEntry);
        populateEntry.clear();

        nameValue = L"Mr. Smith";
        entry.setEntry(NAME, nameValue, JavaStorageEntry::STRING);
        populateEntry.insert(entry);

        nameValue = L"49";
        entry.setEntry(L"AGE", nameValue, JavaStorageEntry::INT);
        populateEntry.insert(entry);

        entry.setEntry(ID, L"TestApp4", JavaStorageEntry::STRING);
        populateEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, populateEntry));
        CHECK(jtu->checkParams(*js, tableName, populateEntry, 1));
        CHECK(jtu->remove(*js, tableName, populateEntry));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while createTable string and int\n");
    }

    // 4. Test create column with value. Check value is discarded.
    try
    {
        tableName = "VALUE_DISCARD";
        JavaStorageApplicationEntry_t populateEntry;

        entry.setEntry(ID, L"TestApp4", JavaStorageEntry::STRING);
        populateEntry.insert(entry);

        nameValue = L"MyGreatLocation";
        entry.setEntry(L"LOCATION", nameValue, JavaStorageEntry::STRING);
        populateEntry.insert(entry);

        nameValue = L"my/Path";
        entry.setEntry(L"PATH", nameValue, JavaStorageEntry::STRING);
        populateEntry.insert(entry);

        js->startTransaction();
        js->createTable(tableName, populateEntry);
        // Check values discarded
        CHECK(jtu->checkParams(*js, tableName, populateEntry, 1) == false);

        CHECK(jtu->populate(*js, tableName, populateEntry));
        CHECK(jtu->checkParams(*js, tableName, populateEntry, 1));
        CHECK(jtu->remove(*js, tableName, populateEntry));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while createTable\n");
    }

    // 5. Test create two same columns. JSE is thrown.
    try
    {
        tableName = "TWO_VALUES";
        JavaStorageApplicationEntry_t populateEntry;
        nameValue = L"";

        entry.setEntry(L"LOCATION", nameValue, JavaStorageEntry::STRING);
        populateEntry.insert(entry);

        entry.setEntry(L"LOCATION", nameValue, JavaStorageEntry::STRING);
        populateEntry.insert(entry);

        js->createTable(tableName, populateEntry);
        FAIL("No exception while creating two same columns\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while createTable two same columns\n");
    }

    // 6. Test create two same tables. JSE is thrown.
    try
    {
        tableName = "TWO_TABLES";
        JavaStorageApplicationEntry_t populateEntry;
        nameValue = L"";

        entry.setEntry(L"LOCATION", nameValue, JavaStorageEntry::STRING);
        populateEntry.insert(entry);

        js->createTable(tableName, populateEntry);
        js->createTable(tableName, populateEntry);
        FAIL("No exception while creating two same columns\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while createTable two same columns\n");
    }

    // 7. Test create two same name columns but different type.
    try
    {
        tableName = "TWO_VALUE_TYPES";
        JavaStorageApplicationEntry_t populateEntry;
        nameValue = L"";

        entry.setEntry(L"LOCATION", nameValue, JavaStorageEntry::STRING);
        populateEntry.insert(entry);

        entry.setEntry(L"LOCATION", nameValue, JavaStorageEntry::INT);
        populateEntry.insert(entry);

        js->createTable(tableName, populateEntry);
        FAIL("No exception while creating two same columns different type\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while createTable two same columns type\n");
    }

    js->rollbackTransaction();
    js->close();

    // If removal works data file existed.
    CHECK(remove(dbName.c_str()) == 0);
}
