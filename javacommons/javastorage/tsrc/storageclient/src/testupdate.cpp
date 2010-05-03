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

TEST_GROUP(TestUpdate)
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
 * 2. Test empty updateEntry throws JSE.
 * 3. Test empty matchEntry throws JSE.
 * 4. Test non-existent tableName throws JSE.
 * 5. Test update non-existent updateEntry name throws JSE.
 * 6. Test update non-existent matchEntry name throws JSE.
 * 7. Test update when connection already closed throws JSE.
 */
TEST(TestUpdate, TestUpdateArguments)
{
    JavaStorageApplicationEntry_t updateEntry;
    JavaStorageApplicationEntry_t matchEntry;
    JavaStorageApplicationEntry_t emptyEntry;

    jtu->createDefaultEntry(updateEntry);
    jtu->createDefaultEntry(matchEntry);

    // 1. Test empty tableName throws JSE.
    string dbName = JAVA_DATABASE_NAME;
    js->open(dbName);

    string empty = "";
    string tableName = APPLICATION_PACKAGE_TABLE;

    try
    {
        js->update(empty, updateEntry, matchEntry);
        FAIL("No exception while update with empty tableName\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidArgument == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while update with empty tableName\n");
    }

    // 2. Test empty updateEntry throws JSE.
    try
    {
        js->update(empty, emptyEntry, matchEntry);
        FAIL("No exception while update with empty updateEntry\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidArgument == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while update with empty primaryKey\n");
    }

    // 3. Test empty match entry throws JSE.
    try
    {
        js->update(tableName, updateEntry, emptyEntry);
        FAIL("No exception while update with empty updateEntry\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidArgument == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while update with empty entry\n");
    }

    // 4. Test non-existent tableName throws JSE.
    try
    {
        string invalidTableName = "FOO_BAAR_TAABLE";
        js->update(invalidTableName, updateEntry, matchEntry);
        FAIL("No exception while update with non-existing tableName\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASS
    }
    catch (...)
    {
        FAIL("Unexpected exp while update with non-existing tableName\n");
    }

    // 5. Test update non-existent updateEntry throws JSE.
    try
    {
        JavaStorageApplicationEntry_t nonExistentEntry;
        JavaStorageEntry entry;
        entry.setEntry(L"FOO_BAAR", L"BAAR_FOO_VALUE");
        nonExistentEntry.insert(entry);

        js->update(tableName, nonExistentEntry, matchEntry);
        FAIL("No exception while update non-existent updateEntry name\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASS
    }
    catch (...)
    {
        FAIL("Unexp exp while update with non-existent updateEntry name\n");
    }

    // 6. Test update non-existent matchEntry throws JSE.
    try
    {
        JavaStorageApplicationEntry_t nonExistentEntry;
        JavaStorageEntry entry;
        entry.setEntry(L"FOO_BAAR", L"BAAR_FOO_VALUE");
        nonExistentEntry.insert(entry);

        js->update(tableName, updateEntry, nonExistentEntry);
        FAIL("No exception while update non-existent matchEntry name\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASS
    }
    catch (...)
    {
        FAIL("Unexpected exp while update with non-existent matchentry name\n");
    }

    // 7. Test update when connection already closed throws JSE.
    js->close();
    try
    {
        js->update(tableName, updateEntry, matchEntry);
        FAIL("No exception while update with closed connection\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidConnection == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while update with closed connection\n");
    }
    updateEntry.clear();
    matchEntry.clear();
}

/**
 * 1. Test update.
 * 2. Test update one entry.
 * 3. Test update string to int entry. JSE is thrown.
 * 4. Test update same key twice in same statemenet. Second attribute is updated
 *    to storage.
 * 5. Test update no name. JSE is thrown.
 * 6. Test update no value. Value is cleared.
 * 7. Test update no existing value. Data is written to storage. Storage must
 *    contain some data where matchEntry matches but actual entry in the
 *    statement is not written to storage.
 * 8. Test update no existing entries by primary key. No entries updated.
 * 9. Test update whole table.
 * 10. Test Update primary key.
 * 11. Test update to NULL.
 */
TEST(TestUpdate, TestUpdate)
{
    string tableName = APPLICATION_PACKAGE_TABLE;
    JavaStorageApplicationEntry_t appEntry;
    JavaStorageApplicationEntry_t idEntry;

    JavaStorageEntry entry;
    entry.setEntry(JAD_PATH, L"/usr/local/bin/myApps/");
    appEntry.insert(entry);

    entry.setEntry(JAR_PATH, L"/usr/share/private/[abcs12345]/myApps/");
    appEntry.insert(entry);

    entry.setEntry(INITIAL_SIZE, L"16543", JavaStorageEntry::INT);
    appEntry.insert(entry);

    JavaStorageApplicationEntry_t defaultEntry;
    jtu->createDefaultEntry(defaultEntry);
    entry.setEntry(ID, L"TestUpdate1");
    defaultEntry.insert(entry);
    idEntry.insert(entry);

    try
    {
        js->open();
        js->startTransaction();

        CHECK(jtu->populate(*js, tableName, defaultEntry));
        CHECK(jtu->checkParams(*js, tableName, defaultEntry, 1));

        js->update(tableName, appEntry, defaultEntry);
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));

        // This comes from default params. This ensures that both values are
        // written under same primary key.
        entry.setEntry(PACKAGE_NAME, L"BasicGame");
        appEntry.insert(entry);

        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));
    }
    catch (...)
    {
        FAIL("Unexpected exp while update\n");
    }

    // 2. Test update one entry.
    try
    {
        appEntry.clear();
        entry.setEntry(PACKAGE_NAME, L"NewName");
        appEntry.insert(entry);

        js->update(tableName, appEntry, defaultEntry);
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));
    }
    catch (...)
    {
        FAIL("Unexpected exp while update one entry\n");
    }

    // 3. Test update string to int entry. JSE is thrown.
    try
    {
        appEntry.clear();
        entry.setEntry(INITIAL_SIZE, L"BIG_SIZE", JavaStorageEntry::INT);
        appEntry.insert(entry);

        js->update(tableName, appEntry, defaultEntry);
        FAIL("No exp. while updating string to int column\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
    }
    catch (...)
    {
        FAIL("Unexpected exp while update string to int column\n");
    }

    // 4. Test update same key twice in same statemenet. Second attribute
    // is considered. Logically it is updated latests.
    try
    {
        appEntry.clear();
        entry.setEntry(JAD_PATH, L"First path");
        appEntry.insert(entry);

        entry.setEntry(JAD_PATH, L"Second path");
        appEntry.insert(entry);

        // Use idEntry from now on as defaultEntry is modified in the storage.
        js->update(tableName, appEntry, idEntry);

        JavaStorageApplicationEntry_t checkEntry;
        entry.setEntry(JAD_PATH, L"Second path");
        checkEntry.insert(entry);

        CHECK(jtu->checkParams(*js, tableName, checkEntry, 1));
    }
    catch (...)
    {
        FAIL("Unexpected exp while update same entry twice\n");
    }

    // 5. Test update no name. JSE is thrown.
    try
    {
        appEntry.clear();
        entry.setEntry(L"", L"12345");
        appEntry.insert(entry);

        js->update(tableName, appEntry, idEntry);
        FAIL("No exp. while updating no name\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
    }
    catch (...)
    {
        FAIL("Unexpected exp while update no name\n");
    }

    // 6. Test update no value. Value is cleared.
    try
    {
        appEntry.clear();
        entry.setEntry(PACKAGE_NAME, L"");
        appEntry.insert(entry);

        js->update(tableName, appEntry, idEntry);
        CHECK(jtu->checkParams(*js, tableName, idEntry, 1));
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));
    }
    catch (...)
    {
        FAIL("Unexp exp while update no value\n");
    }

    // 7. Test update no existing value.
    try
    {
        appEntry.clear();
        entry.setEntry(ROOT_PATH, L"NonExistingValue");
        appEntry.insert(entry);

        js->update(tableName, appEntry, idEntry);
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));
    }
    catch (...)
    {
        FAIL("Unexpected exp while update no value\n");
    }

    // 8. Test update no matching entries
    try
    {
        appEntry.clear();
        entry.setEntry(ROOT_PATH, L"UNIQUE_ID_X");
        appEntry.insert(entry);

        js->update(tableName, defaultEntry, appEntry);

        // Verify not updated.
        CHECK(jtu->checkParams(*js, tableName, defaultEntry, 0));
    }
    catch (...)
    {
        FAIL("Unexpected exp while update with non-existent primaryKey\n");
    }

    // 9. Test update whole table.
    try
    {
        appEntry.clear();
        // Add also ID createAppDataEntry does not create it
        appEntry = idEntry;
        jtu->createAppDataEntry(appEntry);

        js->update(tableName, appEntry, idEntry);
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));
    }
    catch (...)
    {
        FAIL("Unexpected exp while update whole table\n");
    }

    // 10. Test Update primary key.
    try
    {
        appEntry.clear();
        // ID act as primary key on this table.
        entry.setEntry(ID, L"TestUpdate2");
        appEntry.insert(entry);

        js->update(tableName, appEntry, idEntry);
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));

        // Check it was really update that old ID is not found.
        CHECK(jtu->checkParams(*js, tableName, idEntry, 0));
    }
    catch (...)
    {
        FAIL("Unexpected exp while update primary key\n");
    }

    // 11. Test update to NULL.
    try
    {

        appEntry.clear();
        idEntry.clear();

        entry.setEntry(ID, L"TestUpdate2");
        idEntry.insert(entry);

        entry.setEntry(PACKAGE_NAME, L"", JavaStorageEntry::NULL_TYPE);
        appEntry.insert(entry);

        entry.setEntry(JAD_URL, L"FindMe!");
        appEntry.insert(entry);

        js->update(tableName, appEntry, idEntry);
        CHECK(jtu->checkParams(*js, tableName, idEntry, 1));
        // Set to NULL.
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1) == false);

        /*appEntry.clear();
        // ID act as primary key on this table.
        entry.setEntry(ID, L"TestUpdate2");
        appEntry.insert(entry);

        js->update(tableName, appEntry, idEntry);
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));

        // Check it was really update that old ID is not found.
        CHECK(jtu->checkParams(*js, tableName, idEntry, 0));*/
    }
    catch (...)
    {
        FAIL("Unexpected exp while update primary key\n");
    }


    appEntry.clear();
    idEntry.clear();
    defaultEntry.clear();
    js->rollbackTransaction();
    js->close();
}
