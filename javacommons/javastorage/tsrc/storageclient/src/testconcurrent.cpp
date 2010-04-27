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

TEST_GROUP(TestConcurrent)
{
    JavaStorage* js;
    JavaStorage* js2;
    JavaStorage* js3;
    JavaStorage* js4;
    JavaStorageTestUtils* jtu;

    TEST_SETUP()
    {
        js = JavaStorage::createInstance();
        js2 = JavaStorage::createInstance();
        js3 = JavaStorage::createInstance();
        js4 = JavaStorage::createInstance();
        jtu = new JavaStorageTestUtils();
    }
    TEST_TEARDOWN()
    {
        try
        {
            js->rollbackTransaction();
            js->close();
            js2->rollbackTransaction();
            js2->close();
            js3->rollbackTransaction();
            js3->close();
            js4->rollbackTransaction();
            js4->close();


        }
        catch (...)
        {
            // No can do
        }

        delete js;
        js = 0;
        delete js2;
        js2 = 0;
        delete js3;
        js3 = 0;
        delete js4;
        js4 = 0;
        delete jtu;
        jtu = 0;
    }
};

/**
 * 1. Open three concurrent connections and start transactions
 * 2. Close the three connections
 */
TEST(TestConcurrent, TestOpenClose)
{
    string dbName = JAVA_DATABASE_NAME;
    // 1. Open three sessions and start transactions
    try
    {
        js->open(dbName);
        js->startTransaction();
        js2->open(dbName);
        js2->startTransaction();
        js3->open(dbName);
        js3->startTransaction();
    }
    catch (...)
    {
        FAIL("Exception while starting transaction\n");
    }

    // 2. Close all connections
    try
    {
        js->close();
        js2->close();
        js3->close();
    }
    catch (...)
    {
        FAIL("Exception in close\n");
    }
}

/**
 * 1. Open four concurrent sessions
 * 2. Populate DB with first entry
 * 3. Populate DB with another entry with second session
 * 4. Populate DB with another entry with third session
 * 5. Close one session and read entries from two others
 * 6. Close another session and read entries from open sessions
 * 7. Insert one entry to DB using the fourth session
 * 8. Read fourth entry using the third session
 * 9. Remove all entries
 * 10. Close the last sessions
 */

TEST(TestConcurrent, TestFourConcurrent)
{
    string dbName = JAVA_DATABASE_NAME;
    string tableName = APPLICATION_PACKAGE_TABLE;

    JavaStorageEntry entry;
    JavaStorageApplicationEntry_t idEntry;
    entry.setEntry(ID, L"TestConcurrent1");
    idEntry.insert(entry);

    // 1. Open four sessions
    try
    {
        js->open(dbName);
        js2->open(dbName);
        js3->open(dbName);
        js4->open(dbName);
    }
    catch (...)
    {
        FAIL("Exception while opening or starting transaction\n");
    }

    // 2. Populate DB with first entry
    try
    {
        JavaStorageApplicationEntry_t firstEntry;
        jtu->createDefaultEntry(firstEntry);
        entry.setEntry(ID, L"TestConcurrent1");
        firstEntry.insert(entry);

        // Pre-populate DB
        CHECK(jtu->populate(*js, tableName, firstEntry));
        CHECK(jtu->checkParams(*js, tableName, firstEntry, 1));
    }
    catch (...)
    {
        FAIL("Failed to open database\n");
    }

    // 3. Populate DB with another entry with second session
    try
    {
        JavaStorageApplicationEntry_t secondEntry;

        entry.setEntry(ID, L"TestConcurrent2");
        secondEntry.insert(entry);

        entry.setEntry(PACKAGE_NAME, L"BasicGame2");
        secondEntry.insert(entry);

        entry.setEntry(VENDOR, L"GreatVendor2");
        secondEntry.insert(entry);

        entry.setEntry(VERSION, L"18.01.92");
        secondEntry.insert(entry);

        // Pre-populate DB
        CHECK(jtu->populate(*js2, tableName, secondEntry));
        CHECK(jtu->checkParams(*js2, tableName, secondEntry, 1));
    }
    catch (...)
    {
        FAIL("Failed to open database\n");
    }

    // 4. Populate DB with another entry with third session
    try
    {
        JavaStorageApplicationEntry_t thirdEntry;

        entry.setEntry(ID, L"TestConcurrent3");
        thirdEntry.insert(entry);

        entry.setEntry(PACKAGE_NAME, L"BasicGame3");
        thirdEntry.insert(entry);

        entry.setEntry(VENDOR, L"GreatVendor3");
        thirdEntry.insert(entry);

        entry.setEntry(VERSION, L"18.01.93");
        thirdEntry.insert(entry);

        // Pre-populate DB
        CHECK(jtu->populate(*js3, tableName, thirdEntry));
        CHECK(jtu->checkParams(*js3, tableName, thirdEntry, 1));
    }
    catch (...)
    {
        FAIL("Failed to open database\n");
    }

    // 5. Close one session and read entries from two others

    try
    {
        js2->close();
    }
    catch (...)
    {
        FAIL("Exception in close js2\n");
    }

    try
    {
        JavaStorageApplicationEntry_t secondEntry;

        entry.setEntry(ID, L"TestConcurrent2");
        secondEntry.insert(entry);

        entry.setEntry(PACKAGE_NAME, L"BasicGame2");
        secondEntry.insert(entry);

        entry.setEntry(VENDOR, L"GreatVendor2");
        secondEntry.insert(entry);

        entry.setEntry(VERSION, L"18.01.92");
        secondEntry.insert(entry);

        CHECK(jtu->checkParams(*js3, tableName, secondEntry, 1));
    }
    catch (...)
    {
        FAIL("Failed to open database\n");
    }

    try
    {
        JavaStorageApplicationEntry_t firstEntry;

        entry.setEntry(ID, L"TestConcurrent1");
        firstEntry.insert(entry);

        entry.setEntry(PACKAGE_NAME, L"BasicGame");
        firstEntry.insert(entry);

        entry.setEntry(VENDOR, L"GreatVendor");
        firstEntry.insert(entry);

        entry.setEntry(VERSION, L"18.01.91");
        firstEntry.insert(entry);

        CHECK(jtu->checkParams(*js, tableName, firstEntry, 1));
    }
    catch (...)
    {
        FAIL("Failed to open database\n");
    }

    // 6. Close another session and read entries from open sessions

    try
    {
        js->close();
    }
    catch (...)
    {
        FAIL("Exception in close js\n");
    }

    try
    {
        JavaStorageApplicationEntry_t thirdEntry;

        entry.setEntry(ID, L"TestConcurrent3");
        thirdEntry.insert(entry);

        entry.setEntry(PACKAGE_NAME, L"BasicGame3");
        thirdEntry.insert(entry);

        entry.setEntry(VENDOR, L"GreatVendor3");
        thirdEntry.insert(entry);

        entry.setEntry(VERSION, L"18.01.93");
        thirdEntry.insert(entry);

        CHECK(jtu->checkParams(*js3, tableName, thirdEntry, 1));
    }
    catch (...)
    {
        FAIL("Failed to open database\n");
    }

    // 7. Insert one entry to DB using the fourth session
    try
    {
        JavaStorageApplicationEntry_t fourthEntry;
        entry.setEntry(ID, L"TestConcurrent4");
        fourthEntry.insert(entry);

        entry.setEntry(PACKAGE_NAME, L"BasicGame4");
        fourthEntry.insert(entry);

        entry.setEntry(VENDOR, L"GreatVendor4");
        fourthEntry.insert(entry);

        entry.setEntry(VERSION, L"18.01.94");
        fourthEntry.insert(entry);

        // Populate DB and check that entry can be found in DB
        CHECK(jtu->populate(*js4, tableName, fourthEntry));
        CHECK(jtu->checkParams(*js4, tableName, fourthEntry, 1));
    }
    catch (...)
    {
        FAIL("Failed to open database\n");
    }

    // 8. Read fourth entry using the third session
    try
    {
        JavaStorageApplicationEntry_t fourthEntry;
        entry.setEntry(ID, L"TestConcurrent4");
        fourthEntry.insert(entry);

        entry.setEntry(PACKAGE_NAME, L"BasicGame4");
        fourthEntry.insert(entry);

        entry.setEntry(VENDOR, L"GreatVendor4");
        fourthEntry.insert(entry);

        entry.setEntry(VERSION, L"18.01.94");
        fourthEntry.insert(entry);

        CHECK(jtu->checkParams(*js3, tableName, fourthEntry, 1));
    }
    catch (...)
    {
        FAIL("Failed to open database\n");
    }

    // 9. Remove all entries
    try
    {
        JavaStorageApplicationEntry_t firstEntry;
        entry.setEntry(ID, L"TestConcurrent1");
        firstEntry.insert(entry);

        entry.setEntry(PACKAGE_NAME, L"BasicGame");
        firstEntry.insert(entry);

        entry.setEntry(VENDOR, L"GreatVendor");
        firstEntry.insert(entry);

        entry.setEntry(VERSION, L"18.01.91");
        firstEntry.insert(entry);

        CHECK(jtu->remove(*js3, tableName, firstEntry));

        JavaStorageApplicationEntry_t secondEntry;
        entry.setEntry(ID, L"TestConcurrent2");
        secondEntry.insert(entry);

        entry.setEntry(PACKAGE_NAME, L"BasicGame2");
        secondEntry.insert(entry);

        entry.setEntry(VENDOR, L"GreatVendor2");
        secondEntry.insert(entry);

        entry.setEntry(VERSION, L"18.01.92");
        secondEntry.insert(entry);

        CHECK(jtu->remove(*js3, tableName, secondEntry));

        JavaStorageApplicationEntry_t thirdEntry;
        entry.setEntry(ID, L"TestConcurrent3");
        thirdEntry.insert(entry);

        entry.setEntry(PACKAGE_NAME, L"BasicGame3");
        thirdEntry.insert(entry);

        entry.setEntry(VENDOR, L"GreatVendor3");
        thirdEntry.insert(entry);

        entry.setEntry(VERSION, L"18.01.93");
        thirdEntry.insert(entry);

        CHECK(jtu->remove(*js3, tableName, thirdEntry));

        JavaStorageApplicationEntry_t fourthEntry;
        entry.setEntry(ID, L"TestConcurrent4");
        fourthEntry.insert(entry);

        entry.setEntry(PACKAGE_NAME, L"BasicGame4");
        fourthEntry.insert(entry);

        entry.setEntry(VENDOR, L"GreatVendor4");
        fourthEntry.insert(entry);

        entry.setEntry(VERSION, L"18.01.94");
        fourthEntry.insert(entry);

        CHECK(jtu->remove(*js3, tableName, fourthEntry));
    }
    catch (...)
    {
        FAIL("Failed to open database\n");
    }

    // 10. Close the last sessions
    try
    {
        js3->close();
        js4->close();
    }
    catch (...)
    {
        FAIL("Exception in close\n");
    }
}
