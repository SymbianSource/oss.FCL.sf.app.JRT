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
#include "javauid.h"
#include "javasymbianoslayer.h"

#include "storagetestutils.h"
#include "javaregistry.h"
#include "javaregistryentry.h"
#include "logger.h"
#include "minstalledappsregistry.h"

using namespace std;
using namespace Java;
using namespace java::storage;
using namespace java::util;

TEST_GROUP(TestInstalledAppsRegistry)
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
            delete js;
            js = NULL;
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
 * Test CInstalledAppsRegistry::InstalledUidsL(TUid) method.
 * 1. Test no UIDs in registry.
 * 2. Test one application in registry. One suite and one app.
 * 3. Test two MIDlets within same Suite.
 * 4. Test two suites and MIDlets.
 */
TEST(TestInstalledAppsRegistry, TestInstalledUidsL)
{
    LOG(EJavaStorage, EInfo, "+TestInstalledUidsL");
    MInstalledAppsRegistry* appsRegistry = MInstalledAppsRegistry::CreateL();
    CleanupReleasePushL(*appsRegistry);

    // 1. Test no UIDs in registry.
    RArray<TUid> uids;
    TRAPD(err, appsRegistry->InstalledUidsL(uids));
    // CHECK(uids.Count() == 0);
    CHECK(KErrNotSupported == err); // This is not supported by the original API.

    // 2. Test one application in registry. One suite and one app.
    /*JavaStorageApplicationEntry_t populateEntry;
    JavaStorageEntry attr;

    Uid suiteUid1(L"[e1111111]");
    attr.setEntry(PACKAGE_ID, suiteUid1.toString());
    populateEntry.insert(attr);

    Uid midletUid1(L"[e2222222]");

    attr.setEntry(ID, midletUid1.toString());
    populateEntry.insert(attr);

    string tableName = APPLICATION_TABLE;

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, tableName, populateEntry));
    js->commitTransaction();

    appsRegistry->InstalledUidsL(uids);
    CHECK(uids.Count() == 2);

    TUid suiteUid;
    uidToTUid(suiteUid1, suiteUid);

    CHECK(uids.Find(suiteUid) != KErrNotFound);

    TUid midletUid;
    uidToTUid(midletUid1, midletUid);

    CHECK(uids.Find(midletUid) != KErrNotFound);

    uids.Reset();

    // 3. Two MIDlets within same Suite.
    JavaStorageApplicationEntry_t populateEntry2;

    attr.setEntry(PACKAGE_ID, suiteUid1.toString());
    populateEntry2.insert(attr);

    Uid midletUid2(L"[e1111112]");

    attr.setEntry(ID, midletUid2.toString());
    populateEntry2.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, tableName, populateEntry2));
    js->commitTransaction();

    appsRegistry->InstalledUidsL(uids);
    CHECK(uids.Count() == 3);

    uidToTUid(suiteUid1, suiteUid);
    CHECK(uids.Find(suiteUid) != KErrNotFound);

    uidToTUid(midletUid1, midletUid);
    CHECK(uids.Find(midletUid) != KErrNotFound);

    uidToTUid(midletUid2, midletUid);
    CHECK(uids.Find(midletUid) != KErrNotFound);

    uids.Reset();

    // 4. Two suites and MIDlets.
    JavaStorageApplicationEntry_t populateEntry3;

    Uid suiteUid2(L"[e1111113]");
    attr.setEntry(PACKAGE_ID, suiteUid2.toString());
    populateEntry3.insert(attr);

    Uid midletUid3(L"[e2222223]");

    attr.setEntry(ID, midletUid3.toString());
    populateEntry3.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, tableName, populateEntry3));
    js->commitTransaction();

    appsRegistry->InstalledUidsL(uids);
    CHECK(uids.Count() == 5);

    uidToTUid(suiteUid1, suiteUid);
    CHECK(uids.Find(suiteUid) != KErrNotFound);
    uidToTUid(suiteUid2, suiteUid);
    CHECK(uids.Find(suiteUid) != KErrNotFound);

    uidToTUid(midletUid1, midletUid);
    CHECK(uids.Find(midletUid) != KErrNotFound);
    uidToTUid(midletUid2, midletUid);
    CHECK(uids.Find(midletUid) != KErrNotFound);
    uidToTUid(midletUid3, midletUid);
    CHECK(uids.Find(midletUid) != KErrNotFound);

    uids.Reset();

    CHECK(jtu->remove(*js, tableName, populateEntry));
    CHECK(jtu->remove(*js, tableName, populateEntry2));
    CHECK(jtu->remove(*js, tableName, populateEntry3));
    js->close();*/

    CleanupStack::PopAndDestroy(appsRegistry);
    LOG(EJavaStorage, EInfo, "-TestInstalledUidsL");
}

/**
 * Test CInstalledAppsRegistry::EntryPresentL(TUid) method.
 * 1. Test no UID in registry.
 * 2. Test no matching UID on registry.
 * 3. Test suite UID found.
 * 4. Test mIDlet UID found.
 * 5. Test query twice.
 * 6. Test query with TUid::Null(). Leaves KErrArgument.
 */
TEST(TestInstalledAppsRegistry, TestEntryPresentL)
{
    LOG(EJavaStorage, EInfo, "+TestEntryPresentL");

    MInstalledAppsRegistry* appsRegistry = MInstalledAppsRegistry::CreateL();
    CleanupReleasePushL(*appsRegistry);

    TUid uid = TUid::Uid(3777185127);    // e1234567

    // 1. No UID in registry
    TBool exists = appsRegistry->EntryPresentL(uid);
    CHECK(false == exists);

    // 2. No matching UID on registry.
    JavaStorageApplicationEntry_t populateEntry;
    JavaStorageEntry attr;

    attr.setEntry(PACKAGE_ID, L"[e1234577]");
    populateEntry.insert(attr);

    Uid entryUid(L"[e1234588]");

    attr.setEntry(ID, entryUid.toString());
    populateEntry.insert(attr);

    string tableName = APPLICATION_TABLE;

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, tableName, populateEntry));

    JavaStorageApplicationEntry_t packageEntry;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    packageEntry.insert(attr);

    Uid entryUid2(L"[e1234577]");
    attr.setEntry(ID, entryUid2.toString());
    packageEntry.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    packageEntry.insert(attr);

    tableName = APPLICATION_PACKAGE_TABLE;

    CHECK(jtu->populate(*js, tableName, packageEntry));
    js->commitTransaction();

    exists = appsRegistry->EntryPresentL(uid);
    CHECK(false == exists);

    // 3. Suite UID found.
    LOG(EJavaStorage, EInfo, "TestRegistry:: 3.");

    uid = TUid::Uid(3777185143);    // E1234577 aka suiteUID

    exists = appsRegistry->EntryPresentL(uid);
    CHECK(true == exists);

    // 4. MIDlet UID found.
    uid = TUid::Uid(3777185160);    // E1234588 aka midletUID

    exists = appsRegistry->EntryPresentL(uid);
    CHECK(true == exists);

    // 5. Query twice. This is second query
    exists = appsRegistry->EntryPresentL(uid);
    CHECK(true == exists);

    // 6. Query with TUid::Null()
    uid = TUid::Null();
    TRAPD(err, exists = appsRegistry->EntryPresentL(uid));
    // CHECK(KErrArgument == err);
    CHECK(false == exists);

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, populateEntry));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, packageEntry));
    js->commitTransaction();
    js->close();

    CleanupStack::PopAndDestroy(appsRegistry);
    LOG(EJavaStorage, EInfo, "-TestEntryPresentL");
}

/**
 * Test CInstalledAppsRegistry::EntryL(TUid) method.
 * 1. Test no UID in registry.
 * 2. Test no matching UID on registry.
 * 3. Test suite UID found.
 * 4. Test MIDlet UID found.
 * 5. Test query twice.
 * 6. Test query with TUid::Null(). Leaves KErrArgument.
 */
TEST(TestInstalledAppsRegistry, TestEntryL)
{
    LOG(EJavaStorage, EInfo, "+TestEntryL");

    MInstalledAppsRegistry* appsRegistry = MInstalledAppsRegistry::CreateL();
    CleanupReleasePushL(*appsRegistry);
    TUid uid = TUid::Uid(3777185127);    // e1234567

    // 1. No UID in registry
    MInstalledAppsRegistryEntry* entry = NULL;
    TRAPD(err, appsRegistry->EntryL(uid));
    CHECK(KErrNotFound == err);

    // 2. No matching UID on registry.
    JavaStorageApplicationEntry_t populateEntry;
    JavaStorageEntry attr;

    attr.setEntry(PACKAGE_ID, L"[e1234577]");
    populateEntry.insert(attr);

    Uid entryUid(L"[e1234588]");

    attr.setEntry(ID, entryUid.toString());
    populateEntry.insert(attr);

    string tableName = APPLICATION_TABLE;

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, tableName, populateEntry));

    JavaStorageApplicationEntry_t packageEntry;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    packageEntry.insert(attr);

    Uid entryUid2(L"[e1234577]");
    attr.setEntry(ID, entryUid2.toString());
    packageEntry.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    packageEntry.insert(attr);

    tableName = APPLICATION_PACKAGE_TABLE;

    CHECK(jtu->populate(*js, tableName, packageEntry));
    js->commitTransaction();

    TRAP(err, entry = appsRegistry->EntryL(uid));
    CHECK(KErrNotFound == err);

    // 3. Suite UID found.
    LOG(EJavaStorage, EInfo, "TestRegistry:: 3.");

    uid = TUid::Uid(3777185143);    // E1234577 aka suiteUID

    entry = appsRegistry->EntryL(uid);
    CleanupReleasePushL(*entry);
    CHECK(entry != NULL);
    CleanupStack::PopAndDestroy(entry);

    // 4. MIDlet UID found.
    uid = TUid::Uid(3777185160);    // E1234588 aka midletUID

    TRAP(err, entry = appsRegistry->EntryL(uid));
    CHECK(KErrNotFound == err); // If This is not suite
    // KErrNotFound is thrown

    //CleanupReleasePushL(*entry);
    //CHECK(entry != NULL);
    // CleanupStack::PopAndDestroy(entry);

    // 5. Query twice. This is second query
    uid = TUid::Uid(3777185160);    // E1234588 aka midletUID

    TRAP(err, entry = appsRegistry->EntryL(uid));
    // CleanupReleasePushL(*entry);
    // CHECK(entry != NULL);
    // CleanupStack::PopAndDestroy(entry);
    CHECK(KErrNotFound == err); // If This is not suite
    // KErrNotFound is thrown

    // 6. Query with TUid::Null()
    uid = TUid::Null();
    TRAP(err, entry = appsRegistry->EntryL(uid));
    // CHECK(KErrArgument == err);
    CHECK(KErrNotFound == err);

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, populateEntry));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, packageEntry));
    js->commitTransaction();
    js->close();

    CleanupStack::PopAndDestroy(appsRegistry);
    LOG(EJavaStorage, EInfo, "-TestEntryL");
}

/**
 * Test CInstalledAppsRegistry::EntryLC(TUid) method.
 * 1. Test no UID in registry.
 * 2. Test no matching UID on registry.
 * 3. Test suite UID found.
 * 4. Test MIDlet UID found.
 * 5. Test query twice.
 * 6. Test query with TUid::Null(). Leaves KErrArgument.
 */
TEST(TestInstalledAppsRegistry, TestEntryLC)
{
    LOG(EJavaStorage, EInfo, "+TestEntryLC");

    MInstalledAppsRegistry* appsRegistry = MInstalledAppsRegistry::CreateL();
    CleanupReleasePushL(*appsRegistry);
    TUid uid = TUid::Uid(3777185127);    // e1234567

    // 1. No UID in registry
    MInstalledAppsRegistryEntry* entry = NULL;
    TRAPD(err, appsRegistry->EntryLC(uid));
    CHECK(KErrNotFound == err);

    // 2. No matching UID on registry.
    JavaStorageApplicationEntry_t populateEntry;
    JavaStorageEntry attr;

    attr.setEntry(PACKAGE_ID, L"[e1234577]");
    populateEntry.insert(attr);

    Uid entryUid(L"[e1234588]");

    attr.setEntry(ID, entryUid.toString());
    populateEntry.insert(attr);

    string tableName = APPLICATION_TABLE;

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, tableName, populateEntry));

    JavaStorageApplicationEntry_t packageEntry;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    packageEntry.insert(attr);

    Uid entryUid2(L"[e1234577]");
    attr.setEntry(ID, entryUid2.toString());
    packageEntry.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    packageEntry.insert(attr);

    tableName = APPLICATION_PACKAGE_TABLE;

    CHECK(jtu->populate(*js, tableName, packageEntry));
    js->commitTransaction();

    TRAP(err, entry = appsRegistry->EntryLC(uid));
    CHECK(KErrNotFound == err);

    // 3. Suite UID found.
    uid = TUid::Uid(3777185143);    // E1234577 aka suiteUID

    entry = appsRegistry->EntryLC(uid);
    CHECK(entry != NULL);

    CleanupStack::PopAndDestroy(entry);

    // 4. MIDlet UID found.
    uid = TUid::Uid(3777185160);    // E1234588 aka midletUID

    TRAP(err, entry = appsRegistry->EntryLC(uid));
    CHECK(KErrNotFound == err);
    // CHECK(entry != NULL);
    // CleanupStack::PopAndDestroy(entry);

    // 5. Query twice. This is second query
    uid = TUid::Uid(3777185160);    // E1234588 aka midletUID
    TRAP(err, entry = appsRegistry->EntryLC(uid));
    CHECK(KErrNotFound == err);
    // CHECK(entry != NULL);
    // CleanupStack::PopAndDestroy(entry);

    // 6. Query with TUid::Null()
    uid = TUid::Null();

    TRAP(err, entry = appsRegistry->EntryLC(uid));
    // CHECK(KErrArgument == err);
    CHECK(KErrNotFound == err);

    // Clean
    js->startTransaction();
    tableName = APPLICATION_TABLE;
    CHECK(jtu->remove(*js, tableName, populateEntry));

    tableName = APPLICATION_PACKAGE_TABLE;
    CHECK(jtu->remove(*js, tableName, packageEntry));

    js->commitTransaction();
    js->close();

    CleanupStack::PopAndDestroy(appsRegistry);
    LOG(EJavaStorage, EInfo, "-TestEntryLC");
}
