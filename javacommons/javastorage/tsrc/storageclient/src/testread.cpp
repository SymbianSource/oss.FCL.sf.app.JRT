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
#include "storagetestutils.h"

using namespace std;
using namespace java::storage;
using namespace java::util;

bool compareEntries(const JavaStorageApplicationEntry_t& aRef,
                    const JavaStorageApplicationEntry_t& aSource);

TEST_GROUP(TestRead)
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
 * 2. Test empty primaryKey throws JSE.
 * 3. Test non-existent tableName throws JSE.
 * 4. Test non-existent primaryKey don't return entries.
 * 5. Test read when connection already closed throws JSE.
 */
TEST(TestRead, TestReadArguments)
{
    // 1. Test empty tableName throws JSE.
    js->open();

    string empty = "";
    Uid appUid1(L"test");
    string tableName = APPLICATION_PACKAGE_TABLE;
    JavaStorageApplicationEntry_t readEntries;

    try
    {
        js->read(empty, appUid1, readEntries);

        FAIL("No exception while read with empty tableName\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidArgument == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while read with empty tableName\n");
    }

    // 2. Test empty primaryKey throws JSE.
    try
    {
        Uid emptyUid;
        js->read(tableName, emptyUid, readEntries);

        FAIL("No exception while read with empty primaryKey\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidArgument == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while read with empty primaryKey\n");
    }

    // 3. Test non-existent tableName throws JSE.
    try
    {
        string nonExistentTable = "FOO_BAR_TABLE";
        js->read(nonExistentTable, appUid1, readEntries);

        FAIL("No exception while read with non-existent table\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
    }
    catch (...)
    {
        FAIL("Unexpected exp while read with non-existent table\n");
    }

    // 4. Test non-existent primaryKey don't return entries.
    try
    {
        Uid nonExistingPrimaryKey(L"FOO_BAR_KEY");
        js->read(tableName, nonExistingPrimaryKey, readEntries);
        CHECK(readEntries.size() == 0);
    }
    catch (...)
    {
        FAIL("Unexpected exp while read non-existent primaryKey\n");
    }

    // 5. Test read when connection already closed throws JSE.
    js->close();
    try
    {
        js->read(tableName, appUid1, readEntries);

        FAIL("No exception while read with closed connection\n");
    }
    catch (JavaStorageException& jse)
    {
        // PASSED
        CHECK(EInvalidConnection == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while read with closed connection\n");
    }
}
/**
 * 1. Test read. All atributes matching to primary key are returned. All
 *    attributes not matching to primary key are not returned. Also int columns
 *    are read and checked.
 * 2. Test readEntry no matching entry.
 * 3. Test multple matches. This is possible when Uid is not primary key
 *    of the table.
 * 4. Test readEntry empty column between.
 */
TEST(TestRead, TestRead)
{
    Uid appUID(L"TestApp5");
    js->open();

    string tableName = APPLICATION_PACKAGE_TABLE;
    JavaStorageApplicationEntry_t appEntry;
    JavaStorageEntry entry;

    entry.setEntry(JAD_PATH, L"/usr/local/bin/");
    appEntry.insert(entry);

    entry.setEntry(JAR_PATH, L"/usr/share/private/[abcs12345]/");
    appEntry.insert(entry);

    entry.setEntry(INITIAL_SIZE, L"2833");
    appEntry.insert(entry);

    try
    {
        JavaStorageApplicationEntry_t writeEntry = appEntry;
        entry.setEntry(ID, L"TestApp5");
        writeEntry.insert(entry);

        js->startTransaction();
        CHECK(jtu->populate(*js, tableName, writeEntry));
        CHECK(jtu->checkParams(*js, tableName, appEntry, 1));

        JavaStorageApplicationEntry_t readEntry;
        js->read(tableName, appUID, readEntry);

        CHECK(compareEntries(appEntry, readEntry));
        readEntry.clear();

        JavaStorageApplicationEntry_t secondEntry;
        Uid secondUID(L"SecondUIDApp");
        entry.setEntry(PACKAGE_NAME, L"MySecondApp");
        secondEntry.insert(entry);

        entry.setEntry(ROOT_PATH, L"My.Second.App");
        secondEntry.insert(entry);

        entry.setEntry(VENDOR, L"MIDletSuiteIsPOP");
        secondEntry.insert(entry);

        entry.setEntry(ID, L"SecondUIDApp");
        secondEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, secondEntry));
        CHECK(jtu->checkParams(*js, tableName, secondEntry, 1));

        js->read(tableName, appUID, readEntry);
        CHECK(compareEntries(appEntry, readEntry));
        readEntry.clear();

        js->read(tableName, secondUID, readEntry);
        CHECK(compareEntries(secondEntry, readEntry));
        readEntry.clear();

        CHECK(jtu->remove(*js, tableName, appEntry));
        CHECK(jtu->remove(*js, tableName, secondEntry));

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexpected exp while calling read\n");
    }
    js->close();

    // 2. Test readEntry no matching entry.
    try
    {
        Uid noMatch(L"No match at all");
        JavaStorageApplicationEntry_t readEntry;

        js->open();
        js->read(tableName, noMatch, readEntry);
        CHECK(readEntry.size() == 0);
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexp exp test 2\n");
    }
    js->close();

    // 3. Test multple matches. This is possible when Uid is not primary key
    //    of the table.
    try
    {
        string pushTable = PUSH_REGISTRATIONS_TABLE;
        Uid uid3(L"ReadTest3");
        JavaStorageApplicationEntry_t popEntry1;
        JavaStorageApplicationEntry_t readEntry;

        entry.setEntry(URL, L"http://test.com");
        popEntry1.insert(entry);

        entry.setEntry(NAME, L"Test.com");
        popEntry1.insert(entry);

        entry.setEntry(FILTER, L"*");
        popEntry1.insert(entry);

        entry.setEntry(REGISTRATION_TYPE, L"1", JavaStorageEntry::INT);
        popEntry1.insert(entry);

        entry.setEntry(ID, uid3.toString());
        popEntry1.insert(entry);

        js->open();
        js->startTransaction();

        CHECK(jtu->populate(*js, pushTable, popEntry1));
        CHECK(jtu->checkParams(*js, pushTable, popEntry1, 1));

        js->read(pushTable, uid3, readEntry);
        CHECK(compareEntries(popEntry1, readEntry));
        readEntry.clear();

        JavaStorageApplicationEntry_t popEntry2;
        entry.setEntry(URL, L"http://hushus.com");
        popEntry2.insert(entry);

        entry.setEntry(NAME, L"Hus!");
        popEntry2.insert(entry);

        entry.setEntry(FILTER, L"*;Blaclist=LoudVoices");
        popEntry2.insert(entry);

        entry.setEntry(REGISTRATION_TYPE, L"2", JavaStorageEntry::INT);
        popEntry2.insert(entry);

        entry.setEntry(ID, uid3.toString());
        popEntry2.insert(entry);

        CHECK(jtu->populate(*js, pushTable, popEntry2));
        CHECK(jtu->checkParams(*js, pushTable, popEntry2, 1));

        js->read(pushTable, uid3, readEntry);

        // Received entry cannot be fully compared as it is not quaranteed
        // which one is returned. Only Uid is same.
        CHECK(readEntry.size() == popEntry1.size());

        JavaStorageApplicationEntry_t refIDEntry;
        entry.setEntry(ID, uid3.toString());
        refIDEntry.insert(entry);
        CHECK(compareEntries(refIDEntry, readEntry));

        readEntry.clear();
        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexp exp test 3\n");
    }
    js->close();

    // 4. Test readEntry empty column between.
    try
    {
        js->open();
        js->startTransaction();

        JavaStorageApplicationEntry_t writeEntry;
        JavaStorageApplicationEntry_t readEntry;
        JavaStorageApplicationEntry_t compareEntry; // exclude empty one

        Uid uid4(L"ReadTest4");
        entry.setEntry(PACKAGE_NAME, L"MySecondApp");
        writeEntry.insert(entry);
        compareEntry.insert(entry);

        entry.setEntry(ROOT_PATH, L"");
        writeEntry.insert(entry);

        entry.setEntry(VENDOR, L"MIDletSuiteIsPOP");
        writeEntry.insert(entry);
        compareEntry.insert(entry);

        entry.setEntry(ID, uid4.toString());
        writeEntry.insert(entry);
        compareEntry.insert(entry);

        CHECK(jtu->populate(*js, tableName, writeEntry));
        // Empties are not returned
        CHECK(jtu->checkParams(*js, tableName, compareEntry, 1));

        js->read(tableName, uid4, readEntry);
        CHECK(compareEntries(compareEntry, readEntry));
        readEntry.clear();

        js->rollbackTransaction();
    }
    catch (...)
    {
        js->rollbackTransaction();
        FAIL("Unexp exp test 4\n");
    }
    js->close();
}

bool compareEntries(
    const JavaStorageApplicationEntry_t& aRef,
    const JavaStorageApplicationEntry_t& aSource)
{
    auto_ptr<JavaStorageTestUtils> jtu(new JavaStorageTestUtils());

    bool equals = false;

    JavaStorageApplicationEntry_t::const_iterator refIter;

    // Go through all reference values.
    for (refIter = aRef.begin(); refIter != aRef.end(); refIter++)
    {
        JavaStorageEntry refEntry = (*refIter);

        // Check source contains same entry.
        JavaStorageApplicationEntry_t::const_iterator sourceIterator =
            aSource.find(refEntry);

        if (sourceIterator != aSource.end())
        {
            JavaStorageEntry sourceEntry = (*sourceIterator);
            // Make sure it definitely is same.
            bool result = jtu->compareEntry(refEntry, sourceEntry);
            if (!result)
            {
                printf("[TC]: Entry did not match\n");
                return equals;
            }
        }
        else
        {
            printf("[TC]: No similar source entry found\n");

            jtu->printEntry(aRef);
            jtu->printEntry(aSource);

            return equals;
        }
    }

    return true;
}
