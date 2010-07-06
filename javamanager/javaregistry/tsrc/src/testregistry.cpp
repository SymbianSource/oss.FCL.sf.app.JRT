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

#include <CppUTest/TestHarness.h>

#include "javastorage.h"
#include "javastorageexception.h"
#include "javastoragenames.h"
#include "javauid.h"
#include "javasymbianoslayer.h"

#include "storagetestutils.h"
#include "javaregistry.h"
#include "javaregistryentry.h"
#include "logger.h"

using namespace std;
using namespace Java;
using namespace java::storage;
using namespace java::util;

TEST_GROUP(TestRegistry)
{
    JavaStorage* js;
    JavaStorageTestUtils* jtu;
    CActiveScheduler* newScheduler;

    TEST_SETUP()
    {
        newScheduler = new CActiveScheduler();
        CActiveScheduler::Install(newScheduler);

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

        delete newScheduler;
        newScheduler = NULL;
    }
};

/**
 * Test CJavaRegistry::RegistryEntryL(TUid) method.
 *   1. No UID in registry.
 *   2. No matching UID on registry.
 *   3. Suite UID found.
 *   4. MIDlet UID found.
 *   5. Query twice.
 *   6. Query with TUid::Null(). Null returned.
 */
TEST(TestRegistry, TestRegistry)
{
    LOG(EJavaStorage, EInfo, "+TestRegistry");

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());
    TUid uid = TUid::Uid(3777185127);    // e1234567

    // 1. No UID in registry
    CJavaRegistryEntry* entry = registry->RegistryEntryL(uid);
    CHECK(entry == NULL);

    // 2. No matching UID on registry.
    JavaStorageApplicationEntry_t populateEntry;
    JavaStorageEntry attr;

    attr.setEntry(PACKAGE_ID, L"[e1234577]");
    populateEntry.insert(attr);

    Uid entryUid(L"[e1234588]");

    attr.setEntry(ID, entryUid.toString());
    populateEntry.insert(attr);

    js->open(JAVA_DATABASE_NAME);
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, populateEntry));

    JavaStorageApplicationEntry_t packageEntry;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    packageEntry.insert(attr);

    Uid entryUid2(L"[e1234577]");
    attr.setEntry(ID, entryUid2.toString());
    packageEntry.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
    packageEntry.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, packageEntry));
    js->commitTransaction();

    entry = registry->RegistryEntryL(uid);
    CHECK(entry == NULL);

    // 3. Suite UID found.
    TUid suiteUid;
    uidToTUid(entryUid2, suiteUid);

    entry = registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);

    TUid regsuiteUid = entry->Uid();

    // Check suite UID;
    CHECK(regsuiteUid == suiteUid);

    delete entry;
    entry = NULL;

    TUid midletUid;
    uidToTUid(entryUid, midletUid);

    // 4. MIDlet UID found.
    entry = registry->RegistryEntryL(midletUid);
    CHECK(entry != NULL);

    TUid regMidletUid = entry->Uid();

    // Check UID;
    CHECK(regMidletUid == midletUid);

    delete entry;
    entry = NULL;

    // 5. Query twice. This is second query
    entry = registry->RegistryEntryL(midletUid);
    CHECK(entry != NULL);

    regMidletUid = entry->Uid();

    // Check UID;
    CHECK(regMidletUid == midletUid);

    delete entry;
    entry = NULL;

    // 6. Query with TUid::Null()
    uid = TUid::Null();

    entry = registry->RegistryEntryL(uid);
    CHECK(entry == NULL);

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, populateEntry));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, packageEntry));

    js->commitTransaction();
    js->close();
    registry.reset(0);
    LOG(EJavaStorage, EInfo, "-TestRegistry");
}

/**
 * Test RegistryEntryL when multiple entries stored to registry.
 * 1. Several suites.
 * 2. Several MIDlets.
 */
TEST(TestRegistry, TestRegistryMultiEntry)
{
    LOG(EJavaStorage, EInfo, "+TestMultiRegistry");

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    // Populate first MIDlet
    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    attr.setEntry(PACKAGE_ID, L"[e1234577]");
    midlet1.insert(attr);

    string tableName = APPLICATION_TABLE;
    Uid appUid(L"[e1234588]");

    attr.setEntry(ID, appUid.toString());
    midlet1.insert(attr);

    js->open(JAVA_DATABASE_NAME);
    js->startTransaction();

    CHECK(jtu->populate(*js, tableName, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"MySuite1");
    suite1.insert(attr);

    Uid suiteUid1(L"[e1234588]");
    attr.setEntry(ID, suiteUid1.toString());
    suite1.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));

    // Populate second MIDlet
    JavaStorageApplicationEntry_t midlet2;

    attr.setEntry(PACKAGE_ID, L"[e1234512]");
    midlet2.insert(attr);

    Uid midletUid2(L"[e1234a12]");
    attr.setEntry(ID, midletUid2.toString());
    midlet2.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet2));

    JavaStorageApplicationEntry_t suite2;
    attr.setEntry(PACKAGE_NAME, L"MySuite2");
    suite2.insert(attr);

    Uid suiteUid2(L"[e1234512]");
    attr.setEntry(ID, suiteUid2.toString());
    suite2.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
    suite2.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite2));

    // Populate third MIDlet
    JavaStorageApplicationEntry_t midlet3;

    attr.setEntry(PACKAGE_ID, L"[e1234513]");
    midlet3.insert(attr);

    Uid midletUid3(L"[e1234a13]");
    attr.setEntry(ID, midletUid3.toString());
    midlet3.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet3));

    JavaStorageApplicationEntry_t suite3;
    attr.setEntry(PACKAGE_NAME, L"MySuite3");
    suite3.insert(attr);

    Uid suiteUid3(L"[e1234513]");
    attr.setEntry(ID, suiteUid3.toString());
    suite3.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
    suite3.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite3));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    // Suite UID.
    TUid uid = TUid::Uid(3777185043);    // e1234513 Suite uid
    CJavaRegistryEntry* entry = registry->RegistryEntryL(uid);
    CHECK(entry != NULL);

    TUid suiteUid = entry->Uid();

    // Check suite UID;
    CHECK(suiteUid.iUid == uid.iUid);

    delete entry;
    entry = NULL;

    // MIDlet UID.
    uid = TUid::Uid(3777186322);    // e1234a12 MIDlet

    entry = registry->RegistryEntryL(uid);
    CHECK(entry != NULL);

    TUid midletUid = entry->Uid();

    // Check suite UID;
    CHECK(midletUid.iUid == uid.iUid);

    delete entry;
    entry = NULL;

    // Clean
    js->startTransaction();
    tableName = APPLICATION_TABLE;
    CHECK(jtu->remove(*js, tableName, midlet1));
    CHECK(jtu->remove(*js, tableName, midlet2));
    CHECK(jtu->remove(*js, tableName, midlet3));

    tableName = APPLICATION_PACKAGE_TABLE;
    CHECK(jtu->remove(*js, tableName, suite1));
    CHECK(jtu->remove(*js, tableName, suite2));
    CHECK(jtu->remove(*js, tableName, suite3));

    js->commitTransaction();
    js->close();
    registry.reset(0);
    LOG(EJavaStorage, EInfo, "-TestMultiRegistry");
}

/**
 * Test CJavaRegistry::RegistryEntryExistsL(TUid) method.
 *   1. No UID in registry.
 *   2. No matching UID on registry.
 *   3. Suite UID found.
 *   4. MIDlet UID found.
 *   5. Query twice.
 *   6. Query with TUid::Null(). Leaves KErrArgument.
 */
TEST(TestRegistry, TestEntryExists)
{
    LOG(EJavaStorage, EInfo, "+TestEntryExists");

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());
    TUid uid = TUid::Uid(3777185127);    // e1234567

    // 1. No UID in registry
    TBool exists = registry->RegistryEntryExistsL(uid);
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

    js->open(JAVA_DATABASE_NAME);
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

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    exists = registry->RegistryEntryExistsL(uid);
    CHECK(false == exists);

    // 3. Suite UID found.
    uid = TUid::Uid(3777185143);    // E1234577 aka suiteUID

    exists = registry->RegistryEntryExistsL(uid);
    CHECK(true == exists);

    // 4. MIDlet UID found.
    uid = TUid::Uid(3777185160);    // E1234588 aka midletUID

    exists = registry->RegistryEntryExistsL(uid);
    CHECK(true == exists);

    // 5. Query twice. This is second query
    exists = registry->RegistryEntryExistsL(uid);
    CHECK(true == exists);

    // 6. Query with TUid::Null()
    uid = TUid::Null();

    TRAPD(err, exists = registry->RegistryEntryExistsL(uid));
    CHECK(KErrArgument == err);

    // Clean
    js->startTransaction();
    tableName = APPLICATION_TABLE;
    CHECK(jtu->remove(*js, tableName, populateEntry));

    tableName = APPLICATION_PACKAGE_TABLE;
    CHECK(jtu->remove(*js, tableName, packageEntry));

    js->commitTransaction();
    js->close();
    registry.reset(0);
    LOG(EJavaStorage, EInfo, "-TestEntryExists");
}

/**
 * Test CJavaRegistry::GetRegistryEntryUidsL(RArray) method.
 *   1. No UID in registry.
 *   2. Suite and MIDlet UID found.
 *   3. Two MIDlets within same Suite.
 *   4. Two suites and MIDlets.
 *   5. Query twice.
 *   6. Invalid uid is not added.
 */
TEST(TestRegistry, TestGetRegistryEntryUidsL)
{
    LOG(EJavaStorage, EInfo, "+TestGetRegistryEntryUidsL");
    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    // 1. No UID in registry
    RArray<TUid> uids;
    registry->GetRegistryEntryUidsL(uids);
    CHECK(uids.Count() == 0);

    uids.Reset();

    // 2. Suite and MIDlet UID found.
    JavaStorageApplicationEntry_t populateEntry;
    JavaStorageEntry attr;

    Uid suiteUid1(L"[e1234577]");
    attr.setEntry(PACKAGE_ID, suiteUid1.toString());
    populateEntry.insert(attr);

    Uid midletUid1(L"[e1234588]");

    attr.setEntry(ID, midletUid1.toString());
    populateEntry.insert(attr);

    string tableName = APPLICATION_TABLE;

    js->open(JAVA_DATABASE_NAME);
    js->startTransaction();
    CHECK(jtu->populate(*js, tableName, populateEntry));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    suite1.insert(attr);

    attr.setEntry(ID, suiteUid1.toString());
    suite1.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    registry->GetRegistryEntryUidsL(uids);
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

    Uid midletUid2(L"[e123D58A]");

    attr.setEntry(ID, midletUid2.toString());
    populateEntry2.insert(attr);

    js->open(JAVA_DATABASE_NAME);
    js->startTransaction();
    CHECK(jtu->populate(*js, tableName, populateEntry2));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    registry->GetRegistryEntryUidsL(uids);
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

    Uid suiteUid2(L"[e12345AA]");
    attr.setEntry(PACKAGE_ID, suiteUid2.toString());
    populateEntry3.insert(attr);

    Uid midletUid3(L"[e12345DD]");

    attr.setEntry(ID, midletUid3.toString());
    populateEntry3.insert(attr);

    js->open(JAVA_DATABASE_NAME);
    js->startTransaction();
    CHECK(jtu->populate(*js, tableName, populateEntry3));

    JavaStorageApplicationEntry_t suite2;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    suite2.insert(attr);

    attr.setEntry(ID, suiteUid2.toString());
    suite2.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
    suite2.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite2));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    registry->GetRegistryEntryUidsL(uids);
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

    // 5. Query twice.
    registry->GetRegistryEntryUidsL(uids);
    CHECK(uids.Count() == 5);
    uids.Reset();

    // 6. Invalid uid is not added.
    JavaStorageApplicationEntry_t populateEntry4;

    attr.setEntry(PACKAGE_ID, L"[123G5678]");
    populateEntry4.insert(attr);

    attr.setEntry(ID, L"[e123G5DD]");
    populateEntry4.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, tableName, populateEntry4));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    registry->GetRegistryEntryUidsL(uids);

    // Invalid is not added
    CHECK(uids.Count() == 5);

    uids.Reset();
    uids.Close();

    CHECK(jtu->remove(*js, tableName, populateEntry));
    CHECK(jtu->remove(*js, tableName, populateEntry2));
    CHECK(jtu->remove(*js, tableName, populateEntry3));
    CHECK(jtu->remove(*js, tableName, populateEntry4));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite2));
    js->close();

    registry.reset(0);
    LOG(EJavaStorage, EInfo, "-TestGetRegistryEntryUidsL");
}

/**
 * Test CJavaRegistry::GetRegistryEntryUidsL(Type, RArray) method.
 *   1. No UID in registry for application type.
 *   2. No UID in registry for suite type.
 *   3. Unknown type.
 *   4. Suite UID when MIDlet and suite populated.
 *   5. General suite UID when MIDlet and suite populated.
 *   6. MIDlet UID when MIDlet and suite populated.
 *   7. General app UID when MIDlet and suite populated.
 *   8. Two MIDlets within same Suite.
 *   9. Two suites and MIDlets.
 *   10. Invalid uid is not added.
 *   11. Unknown type when app is populated.
 */
TEST(TestRegistry, TestGetRegistryEntryUidsL2)
{
    LOG(EJavaStorage, EInfo, "+TestGetRegistryEntryUidsL2");
    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    // 1. No UID in registry for application type.
    RArray<TUid> uids;
    registry->GetRegistryEntryUidsL(EGeneralApplication, uids);
    CHECK(uids.Count() == 0);

    registry->GetRegistryEntryUidsL(EMidp2Midlet, uids);
    CHECK(uids.Count() == 0);

    // 2. No UID in registry for suite type.
    registry->GetRegistryEntryUidsL(EGeneralPackage, uids);
    CHECK(uids.Count() == 0);

    registry->GetRegistryEntryUidsL(EMidp2MidletSuite, uids);
    CHECK(uids.Count() == 0);

    // 3. Unknown type.
    registry->GetRegistryEntryUidsL(EERCP, uids);
    CHECK(uids.Count() == 0);
    uids.Reset();

    JavaStorageApplicationEntry_t populateEntry;
    JavaStorageEntry attr;

    Uid suiteUid1(L"[e1234577]");
    attr.setEntry(PACKAGE_ID, suiteUid1.toString());
    populateEntry.insert(attr);

    Uid midletUid1(L"[e1234588]");
    attr.setEntry(ID, midletUid1.toString());
    populateEntry.insert(attr);

    string tableName = APPLICATION_TABLE;

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, tableName, populateEntry));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    suite1.insert(attr);

    attr.setEntry(ID, suiteUid1.toString());
    suite1.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    // 4. Suite UID when MIDlet and suite populated.
    registry->GetRegistryEntryUidsL(EMidp2MidletSuite, uids);
    CHECK(uids.Count() == 1);
    uids.Reset();

    // 5. General suite UID when MIDlet and suite populated.
    registry->GetRegistryEntryUidsL(EGeneralPackage, uids);
    CHECK(uids.Count() == 1);
    uids.Reset();

    // 6. MIDlet UID when MIDlet and suite populated.
    registry->GetRegistryEntryUidsL(EMidp2Midlet, uids);
    CHECK(uids.Count() == 1);
    uids.Reset();

    // 7. General app UID when MIDlet and suite populated.
    registry->GetRegistryEntryUidsL(EGeneralApplication, uids);
    CHECK(uids.Count() == 1);
    uids.Reset();

    // 8. Two MIDlets within same Suite.
    JavaStorageApplicationEntry_t populateEntry2;

    attr.setEntry(PACKAGE_ID, suiteUid1.toString());
    populateEntry2.insert(attr);

    Uid midletUid2(L"[e123D58A]");

    attr.setEntry(ID, midletUid2.toString());
    populateEntry2.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, tableName, populateEntry2));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    registry->GetRegistryEntryUidsL(EMidp2MidletSuite, uids);
    CHECK(uids.Count() == 1);

    TUid suiteUid;
    uidToTUid(suiteUid1, suiteUid);
    CHECK(uids.Find(suiteUid) != KErrNotFound);
    uids.Reset();

    registry->GetRegistryEntryUidsL(EMidp2Midlet, uids);
    CHECK(uids.Count() == 2);
    uids.Reset();

    registry->GetRegistryEntryUidsL(EGeneralApplication, uids);
    CHECK(uids.Count() == 2);

    TUid midletUid;
    uidToTUid(midletUid1, midletUid);
    CHECK(uids.Find(midletUid) != KErrNotFound);

    uidToTUid(midletUid2, midletUid);
    CHECK(uids.Find(midletUid) != KErrNotFound);

    uids.Reset();

    // 9. Two suites and MIDlets.
    JavaStorageApplicationEntry_t populateEntry3;

    Uid suiteUid2(L"[e12345AA]");
    attr.setEntry(PACKAGE_ID, suiteUid2.toString());
    populateEntry3.insert(attr);

    Uid midletUid3(L"[e12345DD]");
    attr.setEntry(ID, midletUid3.toString());
    populateEntry3.insert(attr);

    // js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, tableName, populateEntry3));

    JavaStorageApplicationEntry_t suite2;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    suite2.insert(attr);

    attr.setEntry(ID, suiteUid2.toString());
    suite2.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
    suite2.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite2));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    registry->GetRegistryEntryUidsL(EMidp2MidletSuite, uids);
    CHECK(uids.Count() == 2);

    uidToTUid(suiteUid1, suiteUid);
    CHECK(uids.Find(suiteUid) != KErrNotFound);
    uidToTUid(suiteUid2, suiteUid);
    CHECK(uids.Find(suiteUid) != KErrNotFound);
    uids.Reset();

    registry->GetRegistryEntryUidsL(EMidp2Midlet, uids);
    CHECK(uids.Count() == 3);

    uidToTUid(midletUid1, midletUid);
    CHECK(uids.Find(midletUid) != KErrNotFound);
    uidToTUid(midletUid2, midletUid);
    CHECK(uids.Find(midletUid) != KErrNotFound);
    uidToTUid(midletUid3, midletUid);
    CHECK(uids.Find(midletUid) != KErrNotFound);

    uids.Reset();

    // 10. Invalid uid is not added.
    JavaStorageApplicationEntry_t populateEntry4;

    attr.setEntry(PACKAGE_ID, L"[123G5678]");
    populateEntry4.insert(attr);

    attr.setEntry(ID, L"[e123G5DD]");
    populateEntry4.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, tableName, populateEntry4));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    registry->GetRegistryEntryUidsL(EMidp2MidletSuite, uids);
    CHECK(uids.Count() == 2);
    uids.Reset();

    registry->GetRegistryEntryUidsL(EMidp2Midlet, uids);
    CHECK(uids.Count() == 3);
    uids.Reset();

    // 11. Unknown type when app is populated.
    registry->GetRegistryEntryUidsL(EERCP, uids);
    CHECK(uids.Count() == 0);
    uids.Reset();

    CHECK(jtu->remove(*js, tableName, populateEntry));
    CHECK(jtu->remove(*js, tableName, populateEntry2));
    CHECK(jtu->remove(*js, tableName, populateEntry3));
    CHECK(jtu->remove(*js, tableName, populateEntry4));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite2));
    js->close();

    registry.reset(0);
    LOG(EJavaStorage, EInfo, "-TestGetRegistryEntryUidsL2");
}
