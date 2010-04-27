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

#include "javacommonutils.h"
#include "javaoslayer.h"
#include "javastorage.h"
#include "javastorageexception.h"
#include "javastoragenames.h"
#include "javasymbianoslayer.h"
#include "javauid.h"
#include "logger.h"
#include "mjavaregistry.h"
#include "storagetestutils.h"

using namespace std;
using namespace java::storage;
using namespace java::util;

/**
 * NOTE: Some of MJavaRegistryMIDletEntry interface methods are already tested
 * at testlegacyregistry.cpp test case.
 */
TEST_GROUP(TestMIDletEntry)
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
 * Test CJavaRegistryMIDletEntry::SizeL() method.
 * 1. No Size set. 0 as size.
 * 2. Test size set to 0.
 * 3. Test size set to 12345.
 */
TEST(TestMIDletEntry, TestSizeL)
{
    LOG(EJavaStorage, EInfo, "+TestSizeL");

    MJavaRegistry* registry = MJavaRegistry::CreateL();
    CHECK(registry != NULL);

    JavaStorageApplicationEntry_t suite1;
    JavaStorageEntry attr;
    attr.setEntry(PACKAGE_NAME, L"TestMIDletByUidL");
    suite1.insert(attr);

    string tableName = APPLICATION_PACKAGE_TABLE;
    Uid suite1Uid(L"[e1230043]");
    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, tableName, suite1));
    js->commitTransaction();

    JavaStorageApplicationEntry_t midlet1;
    attr.setEntry(NAME, L"MyMIDlet1");
    midlet1.insert(attr);

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    string appTableName = APPLICATION_TABLE;
    Uid midlet1Uid(L"[e12a0078]");
    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, appTableName, midlet1));
    js->commitTransaction();

    // 1. No Size set. 0 as size.
    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    // 1. Test TUid::Null() leaves KErrArgument.
    MJavaRegistrySuiteEntry* suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);

    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    MJavaRegistryMIDletEntry* midletEntry
    = suiteEntry->MIDletByUidL(midletUid);
    CHECK(midletEntry != NULL);
    CHECK(midletEntry->SizeL() == 0);

    midletEntry->Release();
    suiteEntry->Release();

    JavaStorageApplicationEntry_t updateEntry;
    attr.setEntry(INITIAL_SIZE, L"0");
    updateEntry.insert(attr);
    js->startTransaction();
    CHECK(jtu->update(*js, tableName, updateEntry, suite1));
    js->commitTransaction();

    suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);

    // 2. Test size set to 0.
    midletEntry = suiteEntry->MIDletByUidL(midletUid);
    CHECK(midletEntry != NULL);
    CHECK(midletEntry->SizeL() == 0);

    midletEntry->Release();
    suiteEntry->Release();

    attr.setEntry(INITIAL_SIZE, L"12345");
    updateEntry.insert(attr);
    js->startTransaction();
    CHECK(jtu->update(*js, tableName, updateEntry, suite1));
    js->commitTransaction();

    suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);

    // 3. Test size set to 12345.
    midletEntry = suiteEntry->MIDletByUidL(midletUid);
    CHECK(midletEntry != NULL);
    // CHECK(midletEntry->SizeL() == 12345);
    CHECK(midletEntry->SizeL() == 0); // Original legacyregistry
    // does not implement size.

    midletEntry->Release();
    suiteEntry->Release();

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, tableName, suite1));
    CHECK(jtu->remove(*js, appTableName, midlet1));
    js->commitTransaction();
    js->close();

    registry->Release();

    LOG(EJavaStorage, EInfo, "-TestSizeL");
}

