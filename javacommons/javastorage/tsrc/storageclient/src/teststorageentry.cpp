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

TEST_GROUP(TestStorageEntry)
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
 * Test constructor with name and value.
 * 1. Test with Name and Value.
 * 2. Test empty name throws StorageException
 * 3. Test value null. If value is not set null is returned.
 * 4. Test value "".
 * 5. Test one len name and value.
 */
TEST(TestStorageEntry, TestStorageEntry)
{
    JavaStorageEntry entry;

    // 1. Test with Name and Value.
    entry.setEntry(ID, L"MyId");
    CHECK(entry.entryName() == ID);
    CHECK(entry.entryValue() == L"MyId");
    CHECK(entry.entryType() == JavaStorageEntry::NOT_DEFINED);

    // 2. Test Name "" throws StorageException
    try
    {
        entry.setEntry(L"", L"Foo Bar");
        FAIL("No exception when no entry name\n");
    }
    catch (JavaStorageException& jse)
    {
        CHECK(EInvalidArgument == jse.mStatus);
    }
    catch (...)
    {
        FAIL("Unexpected exp while createTable with no entry name\n");
    }

    // 3. Test value null. If value is not set null is returned.
    entry.setEntry(ID, L"MyId", JavaStorageEntry::NULL_TYPE);
    CHECK(entry.entryName() == ID);
    CHECK(entry.entryValue() == L"");
    CHECK(entry.entryType() == JavaStorageEntry::NULL_TYPE);

    // 4. Test value "".
    entry.setEntry(ID, L"");
    CHECK(entry.entryName() == ID);
    CHECK(entry.entryValue() == L"");
    CHECK(entry.entryType() == JavaStorageEntry::NOT_DEFINED);

    // 5. Test one len name and value.
    entry.setEntry(L"A", L"M");
    CHECK(entry.entryName() == L"A");
    CHECK(entry.entryValue() == L"M");
    CHECK(entry.entryType() == JavaStorageEntry::NOT_DEFINED);
}
