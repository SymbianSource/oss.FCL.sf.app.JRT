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

bool createAndPopulate(JavaStorage& aJs,
                       JavaStorageTestUtils& aJtu,
                       string& aTableName,
                       JavaStorageApplicationEntry_t& idEntry);

TEST_GROUP(TestAppend)
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
TEST(TestAppend, TestAppendArguments)
{
    string dbName;
    jtu->databasePath(dbName);
    dbName += "TestAppendDB1";

    js->open(dbName);

    string empty = "";
    string tableName = "MY_APPEND_TABLE_1";

    // API doesn't allow table deletion so delete whole DB. Clear table
    // before execution.
    remove(dbName.c_str());

    JavaStorageApplicationEntry_t appEntry;
    wstring name = L"LOCATION";
    wstring nameValue = L"";
    JavaStorageEntry entry;

    entry.setEntry(name, nameValue, JavaStorageEntry::STRING);
    appEntry.insert(entry);

    entry.setEntry(L"PATH", nameValue, JavaStorageEntry::STRING);
    appEntry.insert(entry);

    JavaStorageApplicationEntry_t appendEntry;
    entry.setEntry(VALUE, nameValue, JavaStorageEntry::STRING);
    appendEntry.insert(entry);

    name = L"AGE";
    entry.setEntry(name, nameValue, JavaStorageEntry::INT);
    appendEntry.insert(entry);

    try
    {
        // Create table to append
        js->createTable(tableName, appEntry);
    }
    catch (...)
    {
        FAIL("Unexpected exp while createTable\n");
    }

    try
    {
        JavaStorageApplicationEntry_t emptyEntry;
        // 1. Test empty tableName throws JSE.
        js->appendTable(empty, appendEntry);

        FAIL("No exception while appendTable with empty entry\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidArgument == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while appendTable with empty entry\n");
    }

    try
    {
        JavaStorageApplicationEntry_t emptyEntry;
        // 2. Test empty schema throws JSE
        js->appendTable(tableName, emptyEntry);

        FAIL("No exception while appendTable with empty entry\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidArgument == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while appendTable with empty entry\n");
    }

    try
    {
        JavaStorageApplicationEntry_t invalidEntry;
        wstring name = L"LOCATION";
        wstring nameValue = L"";
        JavaStorageEntry entry;
        // No entry type defined
        entry.setEntry(name, nameValue);
        invalidEntry.insert(entry);

        // 3. Test no column type defined throws JSE.
        js->appendTable(tableName, invalidEntry);

        FAIL("No exception while appendTable with no entry type defined\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidDataStructure == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while appendTable with no entry type defined\n");
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
        js->appendTable(tableName, invalidEntry);

        FAIL("No exception while appendTable with no entry name\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidArgument == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while appendTable with no entry name\n");
    }

    js->close();

    try
    {
        // 5. Test create when connection already closed throws JSE.
        js->createTable(tableName, appEntry);

        FAIL("No exception while appendTable without connection\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidConnection == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while appendTable without connection\n");
    }

    // API doesn't allow table deletion so delete whole DB.
    // DB not necessary exists yet as data is not written and committed.
    printf("Removing storage: %s\n", dbName.c_str());
    remove(dbName.c_str());
}

/**
 * 1. Test append no table to append throws JSE.
 * 2. Test append existing column throws JSE.
 * 3. Test create two same columns. JSE is thrown.
 * 4. Test create two same name columns. Other column is string and other is
 *    int. JSE is thrown.
 */
TEST(TestAppend, TestInvalidAppend)
{
    JavaStorageApplicationEntry_t appEntry;
    wstring name = L"LOCATION";
    wstring nameValue = L"";
    JavaStorageEntry entry;

    entry.setEntry(name, nameValue, JavaStorageEntry::STRING);
    appEntry.insert(entry);

    entry.setEntry(L"PATH", nameValue, JavaStorageEntry::STRING);
    appEntry.insert(entry);

    JavaStorageApplicationEntry_t appendEntry;
    entry.setEntry(VALUE, nameValue, JavaStorageEntry::STRING);
    appendEntry.insert(entry);

    entry.setEntry(L"AGE", nameValue, JavaStorageEntry::INT);
    appendEntry.insert(entry);

    string dbName;
    jtu->databasePath(dbName);
    dbName += "TestAppendDB1";

    js->open(dbName);

    try
    {
        string nonExisting = "I_DO_NOT_EXISTS";
        // 1. Test append non-existing table.
        js->appendTable(nonExisting, appendEntry);

        FAIL("No exception while appendTable to non-existing tabl\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
    }
    catch (...)
    {
        FAIL("Unexpected exp while appendTable to non-existing table\n");
    }

    string tableName = "MY_APPEND_TABLE2";

    js->close();
    // API doesn't allow table deletion so delete whole DB. Clear table
    // before execution.
    remove(dbName.c_str());

    js->open(dbName);

    try
    {
        // Create table to append
        js->createTable(tableName, appEntry);
    }
    catch (...)
    {
        FAIL("Unexpected exp while createTable\n");
    }

    try
    {
        // 2. Test append existing entry throws JSE.
        js->appendTable(tableName, appEntry);

        FAIL("No exception while appendTable with existing entry\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
    }
    catch (...)
    {
        FAIL("Unexpected exp while appendTable with existing entry\n");
    }

    // 3. Test apped two same columns. JSE is thrown.
    try
    {
        JavaStorageApplicationEntry_t populateEntry;
        name = L"LOCATION_X";
        nameValue = L"";

        entry.setEntry(name, nameValue, JavaStorageEntry::STRING);
        populateEntry.insert(entry);

        name = L"LOCATION_X";
        entry.setEntry(name, nameValue, JavaStorageEntry::STRING);
        populateEntry.insert(entry);

        js->appendTable(tableName, populateEntry);
        FAIL("No exception while appending two same columns\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
    }
    catch (...)
    {
        FAIL("Unexpected exp while appending two same columns\n");
    }

    // 4. Test apped two same columns. JSE is thrown. Different data types used.
    try
    {
        JavaStorageApplicationEntry_t populateEntry;
        name = L"LOCATION_X";
        nameValue = L"";

        entry.setEntry(name, nameValue, JavaStorageEntry::STRING);
        populateEntry.insert(entry);

        name = L"LOCATION_X";
        entry.setEntry(name, nameValue, JavaStorageEntry::INT);
        populateEntry.insert(entry);

        js->appendTable(tableName, populateEntry);
        FAIL("No exception while appending two same columns dif types\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
    }
    catch (...)
    {
        FAIL("Unexpected exp while appending two same columns dif types\n");
    }

    js->close();
    // API doesn't allow table deletion so delete whole DB. Clear table
    // before execution.
    remove(dbName.c_str());
}

/**
 * 1. Test append table. Data can be written to it.
 * 2. Test append one column. Data can be written to it.
 * 3. Test append column with value. Check value is discarded.
 * 4. Test append string and int columns. Data can be written to them.
 */
TEST(TestAppend, TestAppend)
{
    string dbName;
    jtu->databasePath(dbName);
    dbName += "TestAppendDB2";

    JavaStorageEntry entry;
    JavaStorageApplicationEntry_t idEntry;
    entry.setEntry(ID, L"TestApp21", JavaStorageEntry::STRING);
    idEntry.insert(entry);

    js->open(dbName);

    string tableName = "MY_APPEND_TABLE3";

    // API doesn't allow table deletion so delete whole DB. Clear table
    // before execution.
    remove(dbName.c_str());

    CHECK(createAndPopulate(*js, *jtu, tableName, idEntry));

    JavaStorageApplicationEntry_t appendEntry;
    wstring name = L"VALUE";
    wstring nameValue = L"";

    entry.setEntry(name, nameValue, JavaStorageEntry::STRING);
    appendEntry.insert(entry);

    name = L"ADDRESS";
    entry.setEntry(name, nameValue, JavaStorageEntry::STRING);
    appendEntry.insert(entry);

    // 1. Test append table. Data can be written to it.
    try
    {
        JavaStorageApplicationEntry_t populateEntry;
        JavaStorageApplicationEntry_t appendPopulateEntry;
        JavaStorageApplicationEntry_t allEntries;

        name = L"LOCATION";
        wstring nameValue = L"MyGreatLocation";

        entry.setEntry(name, nameValue, JavaStorageEntry::STRING);
        populateEntry.insert(entry);
        allEntries.insert(entry);

        name = L"PATH";
        nameValue = L"my/path";
        entry.setEntry(name, nameValue, JavaStorageEntry::STRING);
        populateEntry.insert(entry);
        allEntries.insert(entry);

        name = L"VALUE";
        nameValue = L"vaaalue";
        entry.setEntry(name, nameValue, JavaStorageEntry::STRING);
        allEntries.insert(entry);
        appendPopulateEntry.insert(entry);

        name = L"ADDRESS";
        nameValue = L"Steet a 18";
        entry.setEntry(name, nameValue, JavaStorageEntry::STRING);
        allEntries.insert(entry);
        appendPopulateEntry.insert(entry);

        js->startTransaction();
        js->appendTable(tableName, appendEntry);

        // Check original params are working.
        CHECK(jtu->update(*js, tableName, populateEntry, idEntry));
        CHECK(jtu->checkParams(*js, tableName, populateEntry, 1));

        // Check appended columns can be written
        CHECK(jtu->update(*js, tableName, appendPopulateEntry, idEntry));
        CHECK(jtu->checkParams(*js, tableName, appendPopulateEntry, 1));

        // Check all columns can be updated
        CHECK(jtu->update(*js, tableName, allEntries, idEntry));
        CHECK(jtu->checkParams(*js, tableName, allEntries, 1));

        CHECK(jtu->remove(*js, tableName, populateEntry));
        js->commitTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while appendTable\n");
    }

    // 2. Append one column
    tableName = "MY_APPEND_TABLE4";
    idEntry.clear();
    entry.setEntry(ID, L"TestApp22", JavaStorageEntry::STRING);
    idEntry.insert(entry);
    wstring empty = L"";

    CHECK(createAndPopulate(*js, *jtu, tableName, idEntry));

    try
    {
        JavaStorageApplicationEntry_t appendEntry2;
        JavaStorageApplicationEntry_t populateEntry2;

        name = L"FUZION";
        nameValue = L"Is Full FUZZ!";

        entry.setEntry(name, empty, JavaStorageEntry::STRING);
        appendEntry2.insert(entry);

        entry.setEntry(name, nameValue);
        populateEntry2.insert(entry);

        js->startTransaction();
        js->appendTable(tableName, appendEntry2);

        // It is already tested that existing columns can be updated.

        // Check appended columns can be written
        CHECK(jtu->update(*js, tableName, populateEntry2, idEntry));
        CHECK(jtu->checkParams(*js, tableName, populateEntry2, 1));

        CHECK(jtu->remove(*js, tableName, populateEntry2));
        js->commitTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while appendTable\n");
    }

    // 3. Test append column with value. Check value is discarded.
    tableName = "MY_APPEND_TABLE5";
    idEntry.clear();
    entry.setEntry(ID, L"TestApp23", JavaStorageEntry::STRING);
    idEntry.insert(entry);

    empty = L"";

    CHECK(createAndPopulate(*js, *jtu, tableName, idEntry));

    try
    {
        JavaStorageApplicationEntry_t populateEntry2;
        name = L"FUZION";
        nameValue = L"Is Full FUZZ!";

        entry.setEntry(name, nameValue, JavaStorageEntry::STRING);
        populateEntry2.insert(entry);

        js->startTransaction();
        js->appendTable(tableName, populateEntry2);

        // Check value discarded
        CHECK(jtu->checkParams(*js, tableName, populateEntry2, 1) == false);
        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while appendTable\n");
    }

    // 4. Test append string and int columns. Data can be written to them.
    tableName = "MY_APPEND_TABLE6";
    idEntry.clear();
    entry.setEntry(ID, L"TestApp24", JavaStorageEntry::STRING);
    idEntry.insert(entry);
    empty = L"";

    CHECK(createAndPopulate(*js, *jtu, tableName, idEntry));

    try
    {
        JavaStorageApplicationEntry_t populateEntry;
        JavaStorageApplicationEntry_t appendPopulateEntry;
        JavaStorageApplicationEntry_t allEntries;

        name = L"LOCATION";
        wstring nameValue = L"Test 4 location";

        entry.setEntry(name, nameValue, JavaStorageEntry::STRING);
        populateEntry.insert(entry);
        allEntries.insert(entry);

        name = L"PATH";
        nameValue = L"Test4/path";
        entry.setEntry(name, nameValue, JavaStorageEntry::STRING);
        populateEntry.insert(entry);
        allEntries.insert(entry);

        name = L"VALUE";
        nameValue = L"Test 4 values";
        entry.setEntry(name, nameValue, JavaStorageEntry::STRING);
        allEntries.insert(entry);
        appendPopulateEntry.insert(entry);

        name = L"AGE";
        nameValue = L"49";
        entry.setEntry(name, nameValue, JavaStorageEntry::INT);
        allEntries.insert(entry);
        appendPopulateEntry.insert(entry);
        appendEntry.insert(entry);

        js->startTransaction();
        js->appendTable(tableName, appendEntry);

        // Check original params are working.
        CHECK(jtu->update(*js, tableName, populateEntry, idEntry));
        CHECK(jtu->checkParams(*js, tableName, populateEntry, 1));

        // Check appended columns can be written
        CHECK(jtu->update(*js, tableName, appendPopulateEntry, idEntry));
        CHECK(jtu->checkParams(*js, tableName, appendPopulateEntry, 1));

        // Check all columns can be updated
        CHECK(jtu->update(*js, tableName, allEntries, idEntry));
        CHECK(jtu->checkParams(*js, tableName, allEntries, 1));

        CHECK(jtu->remove(*js, tableName, populateEntry));
        js->commitTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while appendTable\n");
    }
    js->close();

    printf("Removing storage: %s\n", dbName.c_str());
    remove(dbName.c_str());
}

bool createAndPopulate(JavaStorage& aJs,
                       JavaStorageTestUtils& aJtu,
                       string& aTableName,
                       JavaStorageApplicationEntry_t& idEntry)
{
    JavaStorageApplicationEntry_t appEntry;
    JavaStorageApplicationEntry_t populateEntry;

    populateEntry = idEntry;
    appEntry = idEntry;

    wstring name = L"LOCATION";
    wstring empty = L"";
    wstring nameValue = L"createdLocation";
    JavaStorageEntry entry;

    entry.setEntry(name, empty, JavaStorageEntry::STRING);
    appEntry.insert(entry);

    entry.setEntry(name, nameValue);
    populateEntry.insert(entry);

    name = L"PATH";
    nameValue = L"createdPath";
    entry.setEntry(name, empty, JavaStorageEntry::STRING);
    appEntry.insert(entry);

    entry.setEntry(name, nameValue);
    populateEntry.insert(entry);

    try
    {
        // Create table to append
        aJs.createTable(aTableName, appEntry);
        return aJtu.populate(aJs, aTableName, populateEntry);
    }
    catch (...)
    {
        return false;
    }
}
