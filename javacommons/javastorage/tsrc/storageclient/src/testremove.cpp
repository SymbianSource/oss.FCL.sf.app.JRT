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

TEST_GROUP(TestRemove)
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
 * 1. Test empty tableName throws JSE.
 * 2. Test non-existent table throws JSE.
 * 3. Test remove non-existent entry name throws JSE.
 * 4. Test remove non-existent entry value. Call is success.
 * 5. Test remove no values in strorage. Call is success.
 * 6. Test remove connection closed throws JSE.
 */
TEST(TestRemove, TestRemoveArguments)
{
    string empty = "";
    js->open();

    JavaStorageApplicationEntry_t appEntry;
    JavaStorageEntry entry;
    entry.setEntry(JAD_PATH, L"/usr/local/bin/");
    appEntry.insert(entry);

    entry.setEntry(JAR_PATH, L"/usr/share/private/[abcs12345]/");
    appEntry.insert(entry);

    entry.setEntry(ID, L"TestRemoveArguments");
    appEntry.insert(entry);

    // 1. Test with empty tableName
    try
    {
        js->remove(empty, appEntry);

        FAIL("No exception while calling remove with empty tableName\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidArgument == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while calling remove with empty tableName\n");
    }

    // 2. Test with non-existent table
    try
    {
        string tableName = "FOO_BAR";
        js->remove(tableName, appEntry);

        FAIL("No exception while calling remove with nonexistent table\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
    }
    catch (...)
    {
        FAIL("Unexpected exp remove calling write with nonexistent table\n");
    }

    // 3. Test with non-existing entry name
    try
    {
        string tableName = APPLICATION_PACKAGE_TABLE;
        JavaStorageApplicationEntry_t appEntry2;

        entry.setEntry(L"FOO_BAR", L"/usr/local/bin/");
        appEntry2.insert(entry);

        entry.setEntry(ID, L"TestRemoveArguments2");
        appEntry2.insert(entry);

        js->remove(tableName, appEntry2);

        FAIL("No exception while calling remove with non-existing entry\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
    }
    catch (...)
    {
        FAIL("Unexpected exp while calling remove with non-existing entry\n");
    }

    // 4. Test remove non-existent entry value. Call is success.
    // 5. Test remove no values in strorage. Call is success.
    try
    {
        string tableName = APPLICATION_PACKAGE_TABLE;
        JavaStorageApplicationEntry_t appEntry2;

        entry.setEntry(ID, L"FOO BAR");
        appEntry2.insert(entry);

        js->remove(tableName, appEntry2);
    }
    catch (...)
    {
        FAIL("Unexpected exp while calling remove with non-existing entry\n");
    }

    js->close();

    // 6. Test remove connection closed throws JSE.
    try
    {
        js->remove(APPLICATION_PACKAGE_TABLE, appEntry);

        FAIL("No exception while calling remove no connection\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidConnection == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while calling remove no connection\n");
    }
}

/**
 * 1. Test remove pattern. Only matching entries are removed. Test that other
 *    ones remains.
 * 2. Test remove pattern that consist of one invalid entry name.
 * 3. Test remove by primary key.
 */
TEST(TestRemove, TestRemove)
{
    string tableName = APPLICATION_PACKAGE_TABLE;
    js->open();

    JavaStorageEntry entry;
    try
    {
        JavaStorageApplicationEntry_t defaultEntry;
        JavaStorageApplicationEntry_t matchEntry;

        // Without ID parameter
        jtu->createDefaultEntry(matchEntry);

        jtu->createDefaultEntry(defaultEntry);
        entry.setEntry(ID, L"TestRemove1");
        defaultEntry.insert(entry);

        js->startTransaction();

        CHECK(jtu->populate(*js, tableName, defaultEntry));
        CHECK(jtu->checkParams(*js, tableName, matchEntry, 1));

        defaultEntry.clear();
        jtu->createDefaultEntry(defaultEntry);
        entry.setEntry(ID, L"TestRemove2");
        defaultEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, defaultEntry));

        // This does search and checks the response.
        CHECK(jtu->checkParams(*js, tableName, matchEntry, 2));

        defaultEntry.clear();
        jtu->createDefaultEntry(defaultEntry);
        entry.setEntry(ID, L"TestRemove3");
        defaultEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, defaultEntry));

        // This does search and checks the response.
        CHECK(jtu->checkParams(*js, tableName, matchEntry, 3));

        // Create non-matching search pattern.
        JavaStorageApplicationEntry_t nonMatchPattern;
        entry.setEntry(JAD_PATH, L"/different/path");
        nonMatchPattern.insert(entry);

        entry.setEntry(ID, L"NonMatching");
        nonMatchPattern.insert(entry);

        CHECK(jtu->populate(*js, tableName, nonMatchPattern));

        // Check all inserted entries are found
        CHECK(jtu->checkParams(*js, tableName, matchEntry, 3));
        CHECK(jtu->checkParams(*js, tableName, nonMatchPattern, 1));

        // clean
        CHECK(jtu->remove(*js, tableName, matchEntry));
        // Check no matching entry still exists.
        CHECK(jtu->checkParams(*js, tableName, nonMatchPattern, 1));

        // clean
        CHECK(jtu->remove(*js, tableName, nonMatchPattern));
        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while remove match\n");
    }

    // 2. Test remove pattern that consist of one invalid entry name.
    try
    {
        JavaStorageApplicationEntry_t brokenPattern;
        jtu->createDefaultEntry(brokenPattern);

        entry.setEntry(L"NOT_EXISTS", L"/different/path");
        brokenPattern.insert(entry);

        entry.setEntry(ID, L"TestRemove4");
        brokenPattern.insert(entry);

        js->remove(tableName, brokenPattern);

        FAIL("No exception while calling remove with nonexistent name\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
    }
    catch (...)
    {
        FAIL("Unexpected exp remove calling write with nonexistent name\n");
    }

    // 3. Test remove by primary key.
    try
    {
        JavaStorageApplicationEntry_t matchEntry;

        // Without ID parameter
        jtu->createDefaultEntry(matchEntry);

        JavaStorageApplicationEntry_t defaultEntry;
        jtu->createDefaultEntry(defaultEntry);

        entry.setEntry(ID, L"TestRemove5");
        defaultEntry.insert(entry);

        js->startTransaction();
        CHECK(jtu->populate(*js, tableName, defaultEntry));

        // This does search and checks the response.
        CHECK(jtu->checkParams(*js, tableName, defaultEntry, 1));

        defaultEntry.clear();
        jtu->createDefaultEntry(defaultEntry);

        entry.setEntry(ID, L"TestRemove6");
        defaultEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, defaultEntry));

        // This does search and checks the response.
        CHECK(jtu->checkParams(*js, tableName, matchEntry, 2));

        defaultEntry.clear();
        jtu->createDefaultEntry(defaultEntry);

        entry.setEntry(ID, L"TestRemove7");
        defaultEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, defaultEntry));

        // This does search and checks the response.
        CHECK(jtu->checkParams(*js, tableName, matchEntry, 3));

        // Remove by primaryKey
        JavaStorageApplicationEntry_t idEntry;

        entry.setEntry(ID, L"TestRemove5");
        idEntry.insert(entry);

        CHECK(jtu->remove(*js, tableName, idEntry));

        CHECK(jtu->checkParams(*js, tableName, matchEntry, 2));

        idEntry.clear();
        entry.setEntry(ID, L"TestRemove6");
        idEntry.insert(entry);

        CHECK(jtu->remove(*js, tableName, idEntry));

        // One entry already removed
        CHECK(jtu->checkParams(*js, tableName, matchEntry, 1));

        idEntry.clear();
        entry.setEntry(ID, L"TestRemove7");
        idEntry.insert(entry);

        CHECK(jtu->remove(*js, tableName, idEntry));

        // No entries to be found
        CHECK(jtu->checkParams(*js, tableName, matchEntry, 0));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while remove match\n");
    }
}

/**
 * 1. Test remove by int.
 * 2. Test remove by int and string.
 * 3. Test remove by int no type defined.
 */
TEST(TestRemove, TestRemoveByInt)
{
    string tableName = APPLICATION_PACKAGE_TABLE;
    js->open();

    JavaStorageApplicationEntry_t appEntry;
    JavaStorageApplicationEntry_t intEntry;
    JavaStorageApplicationEntry_t intEntryNoType;

    JavaStorageEntry entry;
    entry.setEntry(JAD_PATH, L"/testHome/RemoveByInt");
    appEntry.insert(entry);

    entry.setEntry(INITIAL_SIZE, L"2019", JavaStorageEntry::INT);
    appEntry.insert(entry);
    intEntry.insert(entry);

    entry.setEntry(INITIAL_SIZE, L"2019");
    intEntryNoType.insert(entry);

    js->startTransaction();

    // 1. Test remove by int.
    try
    {
        JavaStorageApplicationEntry_t tempAppEntry = appEntry;
        entry.setEntry(ID, L"TestRemoveByInt1");
        tempAppEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, tempAppEntry));

        // This does search and checks the response.
        CHECK(jtu->checkParams(*js, tableName, intEntry, 1));

        // This does also search to verify entry is removed
        CHECK(jtu->remove(*js, tableName, intEntry));
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while remove by int\n");
    }

    // 2. Test remove by int and string.
    try
    {
        JavaStorageApplicationEntry_t tempAppEntry = appEntry;
        entry.setEntry(ID, L"TestRemoveByInt2");
        tempAppEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, tempAppEntry));

        // This does search and checks the response.
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));
        // This does also search to very it is removed
        CHECK(jtu->remove(*js, tableName, appEntry));
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while remove by int and string\n");
    }

    // 3. Test remove by int no type defined.
    try
    {
        JavaStorageApplicationEntry_t tempAppEntry = appEntry;
        entry.setEntry(ID, L"TestRemoveByInt3");
        tempAppEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, tempAppEntry));

        // This does search and checks the response.
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));

        // This does also search to very it is removed
        CHECK(jtu->remove(*js, tableName, intEntryNoType));
        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while remove by int no type defined\n");
    }
}

/**
 * 1. Test remove returns 0 when no rows removed. No rows populated.
 * 2. Test remove returns 0 when no rows removed. Row populated
 * 3. Test remove returns 1 when one row removed.
 * 4. Test remove returns 3 when three rows removed.
 */
TEST(TestRemove, TestRemoveAmount)
{
    string tableName = APPLICATION_PACKAGE_TABLE;

    JavaStorageApplicationEntry_t removeQuery;
    JavaStorageEntry attr;
    attr.setEntry(PACKAGE_NAME, L"BasicGame");  // Same as default entry
    removeQuery.insert(attr);

    js->open();
    js->startTransaction();

    // 1. Test remove returns 0 when no rows removed. No rows populated.
    try
    {
        int result = js->remove(tableName, removeQuery);
        CHECK(0 == result);
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp1. Remove count no entries.");
    }

    // 2. Test remove returns 0 when no rows removed. Row populated
    try
    {
        JavaStorageApplicationEntry_t entry;
        entry = removeQuery;
        attr.setEntry(ID, L"TestRemoveCount1");
        entry.insert(attr);

        // Populate entry
        CHECK(jtu->populate(*js, tableName, entry));

        JavaStorageApplicationEntry_t noMatchQuery;
        attr.setEntry(PACKAGE_NAME, L"NoGame");  // Same as default entry
        noMatchQuery.insert(attr);

        int result = js->remove(tableName, noMatchQuery);
        CHECK(0 == result);
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp2. Remove count one entry no match.");
    }

    // 3. Test remove returns 1 when one row removed.
    try
    {
        // Previous step populated one entry that was not removed.
        int result = js->remove(tableName, removeQuery);
        CHECK(1 == result);
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp3. Remove count one entry.");
    }

    // 4. Test remove returns 3 when three rows removed.
    try
    {
        JavaStorageApplicationEntry_t entry;
        entry = removeQuery;
        attr.setEntry(ID, L"TestRemoveCount1");
        entry.insert(attr);

        CHECK(jtu->populate(*js, tableName, entry));

        entry.clear();
        entry = removeQuery;
        attr.setEntry(ID, L"TestRemoveCount2");
        entry.insert(attr);

        CHECK(jtu->populate(*js, tableName, entry));

        entry.clear();
        entry = removeQuery;
        attr.setEntry(ID, L"TestRemoveCount3");
        entry.insert(attr);

        CHECK(jtu->populate(*js, tableName, entry));


        int result = js->remove(tableName, removeQuery);
        CHECK(3 == result);
        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp4. Remove count three entries.");
    }
}

/**
 * 1. Test remove all.
 */
TEST(TestRemove, TestRemoveAll)
{
    string tableName = APPLICATION_PACKAGE_TABLE;

    JavaStorageApplicationEntry_t removeQuery;
    JavaStorageEntry attr;
    attr.setEntry(PACKAGE_NAME, L"BasicGame");  // Same as default entry
    removeQuery.insert(attr);

    // 1. Test remove all returns 3 when two rows removed.
    try
    {
        JavaStorageApplicationEntry_t entry;
        entry = removeQuery;
        attr.setEntry(ID, L"TestRemoveAllCount1");
        entry.insert(attr);

        js->open();
        js->startTransaction();
        CHECK(jtu->populate(*js, tableName, entry));

        entry.clear();
        entry = removeQuery;
        attr.setEntry(ID, L"TestRemoveCount2");
        entry.insert(attr);

        CHECK(jtu->populate(*js, tableName, entry));

        entry.clear();
        entry = removeQuery;
        attr.setEntry(ID, L"TestRemoveCount3");
        entry.insert(attr);

        CHECK(jtu->populate(*js, tableName, entry));

        JavaStorageApplicationEntry_t empty;
        int result = js->remove(tableName, empty);

        /* Result checking is not working in a case deleting all!
         * SQLite does optimizated delete and it is not working row
         * based mode that time --> no affected rows returned.
         * This is fixed SQLite >=3.6.5 versions.
         * CHECK(3 == result);
         */

        // Check not found.
        CHECK(jtu->checkParams(*js, tableName, entry, 0));

        entry.clear();
        entry = removeQuery;
        attr.setEntry(ID, L"TestRemoveAllCount1");
        entry.insert(attr);
        CHECK(jtu->checkParams(*js, tableName, entry, 0));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Exp: Remove all.");
    }
    js->close();
}
