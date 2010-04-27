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

TEST_GROUP(TestSearch)
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
 * 2. Test empty search pattern throws JSE.
 * 3. Test search to non-existing table throws JSE.
 * 4. Test search no connection throws JSE.
 */
TEST(TestSearch, TestSearchArguments)
{
    string tableName = APPLICATION_PACKAGE_TABLE;
    JavaStorageApplicationEntry_t appEntry;
    JavaStorageApplicationList_t foundApps;
    JavaStorageEntry entry;

    entry.setEntry(L"LOCATION", L"/usr/local/bin/");
    appEntry.insert(entry);

    entry.setEntry(L"PATH", L"/usr/share/private/[abcs12345]/");
    appEntry.insert(entry);

    // 1. Test empty tableName throws JSE with EInvalidArgument.
    try
    {
        string empty = "";
        js->open();
        js->search(empty, appEntry, foundApps);
        FAIL("No exception while calling search with empty tableName\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidArgument == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while calling search with empty tableName\n");
    }

    // 2. Test Search * from table; enabled.
    try
    {
        JavaStorageApplicationEntry_t emptyEntry;
        js->search(tableName, emptyEntry, foundApps);
    }
    catch (...)
    {
        FAIL("Unexpected exp while calling search with empty entry\n");
    }

    // 3. Test search to non-existing table throws JSE.
    try
    {
        string nonExistingtableName = "NONEXISTING";
        js->search(nonExistingtableName, appEntry, foundApps);
        FAIL("No exception while calling search with nonexisting tableName\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
    }
    catch (...)
    {
        FAIL("Unexpected exp while calling search with nonexisting tableName\n");
    }

    js->close();

    try
    {
        js->search(tableName, appEntry, foundApps);
        FAIL("No exception while calling search after connection close\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidConnection == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while calling search after connection close\n");
    }
}

/**
 * 1. Test search.
 * 2. Test no matches. Empty applist received.
 * 3. Test multiple matches. All found. Storage data contain also items not
 *    matching the pattern.
 * 4. Test Search one match which has only one entry within.
 * 5. Test Search multiple matches which have only one entry within.
 */
TEST(TestSearch, TestSearch)
{
    string tableName = APPLICATION_PACKAGE_TABLE;

    JavaStorageApplicationEntry_t appEntry;
    JavaStorageApplicationList_t foundApps;
    JavaStorageEntry entry;

    entry.setEntry(JAR_PATH, L"/usr/local/bin/");
    appEntry.insert(entry);

    entry.setEntry(JAD_PATH, L"/usr/share/private/[abcs12345]/");
    appEntry.insert(entry);

    // 1. Test search.
    try
    {
        js->open();
        js->startTransaction();

        JavaStorageApplicationEntry_t defaultEntry = appEntry;
        entry.setEntry(ID, L"TestSearch1");
        defaultEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, defaultEntry));
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));
        CHECK(jtu->remove(*js, tableName, appEntry));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while search\n");
    }

    // 2. Test no matches.
    try
    {
        js->startTransaction();

        JavaStorageApplicationEntry_t defaultEntry = appEntry;
        entry.setEntry(ID, L"TestSearch2");
        defaultEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, defaultEntry));

        // Create non-matching search pattern.
        JavaStorageApplicationEntry_t searchPattern;
        entry.setEntry(JAR_PATH, L"/different/path");
        searchPattern.insert(entry);

        CHECK(jtu->checkParams(*js, tableName, searchPattern, 1) == false);
        CHECK(jtu->remove(*js, tableName, appEntry));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while search\n");
    }

    // 3. Test multiple matches
    try
    {
        JavaStorageApplicationEntry_t appEntry;
        JavaStorageEntry entry;

        entry.setEntry(JAD_PATH, L"multiSearch");
        appEntry.insert(entry);

        entry.setEntry(JAR_PATH, L"multisearchPath");
        appEntry.insert(entry);

        JavaStorageApplicationEntry_t writeEntry = appEntry;
        entry.setEntry(ID, L"TestSearch3");
        writeEntry.insert(entry);

        js->startTransaction();

        CHECK(jtu->populate(*js, tableName, writeEntry));
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));

        writeEntry.clear();
        writeEntry = appEntry;
        entry.setEntry(ID, L"TestSearch4");
        writeEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, writeEntry));
        CHECK(jtu->checkParams(*js, tableName, appEntry, 2));

        writeEntry.clear();
        writeEntry = appEntry;
        entry.setEntry(ID, L"TestSearch5");
        writeEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, writeEntry));
        CHECK(jtu->checkParams(*js, tableName, appEntry, 3));

        // Create non-matching search pattern.
        JavaStorageApplicationEntry_t nonMatchPattern;
        entry.setEntry(JAR_PATH, L"/different/path");
        nonMatchPattern.insert(entry);

        entry.setEntry(ID, L"TestSearch6");
        nonMatchPattern.insert(entry);
        CHECK(jtu->populate(*js, tableName, nonMatchPattern));

        // Check that nonMatchPattern is not found.
        CHECK(jtu->checkParams(*js, tableName, appEntry, 3));
        CHECK(jtu->remove(*js, tableName, appEntry));
        CHECK(jtu->remove(*js, tableName, nonMatchPattern));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while multi match\n");
    }

    // 4. Test Search one match which has only one entry within.
    try
    {
        JavaStorageApplicationEntry_t appEntry;

        entry.setEntry(ROOT_PATH, L"OneMatch");
        appEntry.insert(entry);

        JavaStorageApplicationEntry_t writeEntry = appEntry;
        entry.setEntry(ID, L"TestSearch7");
        writeEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, writeEntry));
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));

        // Create non-matching search pattern.
        JavaStorageApplicationEntry_t nonMatchPattern;
        entry.setEntry(ROOT_PATH, L"OneNotMatch");
        nonMatchPattern.insert(entry);

        entry.setEntry(ID, L"TestSearch8");
        nonMatchPattern.insert(entry);

        CHECK(jtu->populate(*js, tableName, nonMatchPattern));

        // Check that nonMatchPattern is not found.
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));
        CHECK(jtu->remove(*js, tableName, appEntry));
        CHECK(jtu->remove(*js, tableName, nonMatchPattern));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while multi match\n");
    }

    // 5. Test Search multiple matches which have only one entry within.
    try
    {
        JavaStorageApplicationEntry_t appEntry;
        JavaStorageEntry entry;

        entry.setEntry(ROOT_PATH, L"multiSearchOneEntry");
        appEntry.insert(entry);

        JavaStorageApplicationEntry_t writeEntry = appEntry;
        entry.setEntry(ID, L"TestSearch9");
        writeEntry.insert(entry);

        js->startTransaction();

        CHECK(jtu->populate(*js, tableName, writeEntry));
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));

        writeEntry.clear();
        writeEntry = appEntry;
        entry.setEntry(ID, L"TestSearch10");
        writeEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, writeEntry));
        CHECK(jtu->checkParams(*js, tableName, appEntry, 2));

        writeEntry.clear();
        writeEntry = appEntry;
        entry.setEntry(ID, L"TestSearch11");
        writeEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, writeEntry));
        CHECK(jtu->checkParams(*js, tableName, appEntry, 3));

        // Create non-matching search pattern.
        JavaStorageApplicationEntry_t nonMatchPattern;
        entry.setEntry(ROOT_PATH, L"/different/path");
        nonMatchPattern.insert(entry);

        entry.setEntry(ID, L"TestSearch12");
        nonMatchPattern.insert(entry);

        CHECK(jtu->populate(*js, tableName, nonMatchPattern));

        // Check that nonMatchPattern is not found.
        CHECK(jtu->checkParams(*js, tableName, appEntry, 3));
        CHECK(jtu->remove(*js, tableName, appEntry));
        CHECK(jtu->remove(*js, tableName, nonMatchPattern));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while multi match\n");
    }
    js->close();
}

/**
 * 1. Test search by INT.
 * 2. Test search by INT and STRING
 * 3. Test search by INT no type set. Entry found.
 */
TEST(TestSearch, TestSearchByInt)
{
    string tableName = APPLICATION_PACKAGE_TABLE;
    JavaStorageApplicationEntry_t appEntry;
    JavaStorageApplicationList_t foundApps;
    JavaStorageApplicationEntry_t intEntry;
    JavaStorageApplicationEntry_t intEntryNoType;

    JavaStorageEntry entry;
    entry.setEntry(JAD_PATH, L"/testHome/TestSearchByInt/");
    appEntry.insert(entry);

    entry.setEntry(INITIAL_SIZE, L"18", JavaStorageEntry::INT);
    appEntry.insert(entry);
    intEntry.insert(entry);

    entry.setEntry(INITIAL_SIZE, L"18");
    intEntryNoType.insert(entry);

    js->open();
    js->startTransaction();

    // 1. Test search by INT.
    try
    {
        JavaStorageApplicationEntry_t writeEntry = appEntry;
        entry.setEntry(ID, L"TestSearcyByInt1");
        writeEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, writeEntry));
        CHECK(jtu->checkParams(*js, tableName, intEntry, 1));
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while search\n");
    }

    // 2. Test search by INT and STRING
    try
    {
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while search\n");
    }

    // 3. Test search by INT no type set.
    try
    {
        // No type set. This param is then assumed to be STRING but still it is
        // found.
        CHECK(jtu->checkParams(*js, tableName, intEntryNoType, 1));
        CHECK(jtu->remove(*js, tableName, appEntry));
        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while search\n");
    }
}

/**
 * 1. Test SELECT * search one matching application.
 * 2. Test SELECT * search two matching applications.
 * 3. Test SELECT * from table.
 */
TEST(TestSearch, TestSearchAll)
{
    string tableName = APPLICATION_PACKAGE_TABLE;
    JavaStorageApplicationEntry_t appEntry;
    JavaStorageApplicationEntry_t matchEntry;
    JavaStorageApplicationList_t foundApps;

    JavaStorageEntry entry;
    entry.setEntry(JAD_PATH, L"/JAD1");
    appEntry.insert(entry);

    entry.setEntry(JAR_PATH, L"/JAR1");
    appEntry.insert(entry);

    entry.setEntry(ID, L"TestSearchAll1");
    appEntry.insert(entry);
    matchEntry.insert(entry);

    js->open();
    js->startTransaction();

    // 1. Test SELECT * search one matching application.
    try
    {
        CHECK(jtu->populate(*js, tableName, appEntry));
        js->search(tableName, matchEntry, foundApps);

        CHECK(foundApps.size() == 1);

        JavaStorageApplicationEntry_t foundEntry = foundApps.front();

        // Check entry contains all attributes.
        CHECK(foundEntry.size() == 3);

        CHECK(jtu->compareEntries(appEntry, foundEntry));
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while search\n");
    }

    appEntry.clear();
    matchEntry.clear();
    foundApps.clear();

    entry.setEntry(JAD_PATH, L"/JAD2");
    appEntry.insert(entry);

    entry.setEntry(JAR_PATH, L"/JAR1");
    appEntry.insert(entry);
    matchEntry.insert(entry);

    entry.setEntry(ID, L"TestSearchAll2");
    appEntry.insert(entry);

    // 2. Test SELECT * search two matching applications.
    try
    {
        CHECK(jtu->populate(*js, tableName, appEntry));
        js->search(tableName, matchEntry, foundApps);

        CHECK(foundApps.size() == 2);

        JavaStorageApplicationEntry_t foundEntry = foundApps.front();
        // Check entry contains all attributes.
        CHECK(foundEntry.size() == 3);

        // Check just in case that entry contains at least JAR_PATH.
        CHECK(jtu->compareEntries(matchEntry, foundEntry));
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while search\n");
    }

    // 3. Test SELECT * from table.
    try
    {
        matchEntry.clear();
        foundApps.clear();
        js->search(tableName, matchEntry, foundApps);

        CHECK(foundApps.size() == 2);

        JavaStorageApplicationEntry_t foundEntry = foundApps.front();
        // Check entry contains all attributes.
        CHECK(foundEntry.size() == 3);

        // Check just in case that entry contains at least JAR_PATH.
        CHECK(jtu->compareEntries(matchEntry, foundEntry));
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Test3: Unexpected exp while search all\n");
    }

    js->rollbackTransaction();
    js->close();
}

