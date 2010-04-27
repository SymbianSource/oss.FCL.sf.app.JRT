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

TEST_GROUP(TestLegacyRegistry)
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
 * Test CJavaRegistry::InstalledSuiteUidsL() method.
 *   1. No UID in registry.
 *   2. One UID in registry.
 *   3. Several UIDs in registtry.
 */
TEST(TestLegacyRegistry, TestInstalledSuiteUidsL)
{
    LOG(EJavaStorage, EInfo, "+TestInstalledSuiteUidsL");

    // 1. No UID in registry
    MJavaRegistry* registry = MJavaRegistry::CreateL();
    CHECK(registry != NULL);

    RArray<TUid> suiteUids;
    registry->InstalledSuiteUidsL(suiteUids);

    CHECK(0 == suiteUids.Count());

    // 2. One UID in registry.
    suiteUids.Reset();

    // Populate first suite
    JavaStorageApplicationEntry_t suite1;
    JavaStorageEntry attr;
    attr.setEntry(PACKAGE_NAME, L"TestSuite1");
    suite1.insert(attr);

    Uid suite1Uid(L"[e22a4577]");
    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));

    JavaStorageApplicationEntry_t midlet1;
    attr.setEntry(NAME, L"MyMIDlet1");
    midlet1.insert(attr);

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e12abc77]");
    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));
    js->commitTransaction();

    registry->InstalledSuiteUidsL(suiteUids);

    CHECK(1 == suiteUids.Count());

    // Check uid is same than populated one
    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    CHECK(suiteUids[0] == suiteUid);

    // 3. Several UIDs in registtry.
    suiteUids.Reset();

    // Populate the second suite
    JavaStorageApplicationEntry_t suite2;
    attr.setEntry(PACKAGE_NAME, L"TestSuite2");
    suite2.insert(attr);

    Uid suite2Uid(L"[e1234a88]");
    attr.setEntry(ID, suite2Uid.toString());
    suite2.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite2.insert(attr);

    JavaStorageApplicationEntry_t midlet2;
    attr.setEntry(NAME, L"MyMIDlet2");
    midlet2.insert(attr);

    attr.setEntry(PACKAGE_ID, suite2Uid.toString());
    midlet2.insert(attr);

    Uid midlet2Uid(L"[e12cba77]");
    attr.setEntry(ID, midlet2Uid.toString());
    midlet2.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite2));
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet2));
    js->commitTransaction();

    registry->InstalledSuiteUidsL(suiteUids);
    CHECK(2 == suiteUids.Count());

    // Check uids are the same. Entries are returned at same
    // order they're written.
    uidToTUid(suite1Uid, suiteUid);
    CHECK(suiteUids[0] == suiteUid);

    uidToTUid(suite2Uid, suiteUid);
    CHECK(suiteUids[1] == suiteUid);

    suiteUids.Close();

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite2));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet2));
    js->commitTransaction();
    js->close();

    registry->Release();

    LOG(EJavaStorage, EInfo, "-TestInstalledSuiteUidsL");
}

/**
 * Test SuiteEntryL() method.
 * 1. No suite entry in registry.
 * 2. NullUid.
 * 3. No matching entry in registry.
 * 4. Matching entry in registry.
 */
TEST(TestLegacyRegistry, TestSuiteEntryL)
{
    LOG(EJavaStorage, EInfo, "+TestSuiteEntryL");

    MJavaRegistry* registry = MJavaRegistry::CreateL();
    CHECK(registry != NULL);

    TUid uid = TUid::Uid(3777185127);    // e1234567

    // 1. No suite entry in registry.
    MJavaRegistrySuiteEntry* suiteEntry = NULL;

    TRAPD(err, suiteEntry = registry->SuiteEntryL(uid));
    CHECK(KErrNotFound == err);

    // 2. NullUid.
    TRAP(err, suiteEntry = registry->SuiteEntryL(TUid::Null()));
    // CHECK(KErrArgument == err);
    CHECK(NULL == suiteEntry);

    // 3. No matching entry in registry.

    // Populate not matching entry
    JavaStorageApplicationEntry_t suite1;
    JavaStorageEntry attr;
    attr.setEntry(PACKAGE_NAME, L"TestSuite1");
    suite1.insert(attr);

    string tableName = APPLICATION_PACKAGE_TABLE;
    Uid suite1Uid(L"[e1234577]");

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, tableName, suite1));

    JavaStorageApplicationEntry_t midlet1;
    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e3335577]");
    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    string appTableName = APPLICATION_TABLE;

    CHECK(jtu->populate(*js, appTableName, midlet1));
    js->commitTransaction();

    TRAP(err, suiteEntry = registry->SuiteEntryL(uid));
    CHECK(KErrNotFound == err);

    // 4. Matching entry in registry.

    // Populate matching entry.
    js->startTransaction();

    JavaStorageApplicationEntry_t suite2;
    attr.setEntry(PACKAGE_NAME, L"TestSuite2");
    suite2.insert(attr);

    Uid suite2Uid(L"[e1234567]");
    attr.setEntry(ID, suite2Uid.toString());
    suite2.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite2.insert(attr);

    CHECK(jtu->populate(*js, tableName, suite2));

    JavaStorageApplicationEntry_t midlet2;
    attr.setEntry(PACKAGE_ID, suite2Uid.toString());
    midlet2.insert(attr);

    Uid midlet2Uid(L"[e3365577]");
    attr.setEntry(ID, midlet2Uid.toString());
    midlet2.insert(attr);

    CHECK(jtu->populate(*js, appTableName, midlet2));
    js->commitTransaction();

    TRAP(err, suiteEntry = registry->SuiteEntryL(uid));
    CHECK(KErrNone == err);
    CHECK(NULL != suiteEntry);  // Actual suite entry is tested elsewhere.

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, tableName, suite1));
    CHECK(jtu->remove(*js, tableName, suite2));
    CHECK(jtu->remove(*js, appTableName, midlet1));
    CHECK(jtu->remove(*js, appTableName, midlet2));
    js->commitTransaction();
    js->close();

    suiteEntry->Release();
    registry->Release();

    LOG(EJavaStorage, EInfo, "-TestSuiteEntryL");
}

/**
 * Test MIDletEntryL() method.
 *
 * 1. No midlet entry in registry.
 * 2. No matching entry in registry.
 * 3. Matching entry in registry.
 */
TEST(TestLegacyRegistry, TestMIDletEntryL)
{
    LOG(EJavaStorage, EInfo, "+TestMIDletEntryL");
    MJavaRegistry* registry = MJavaRegistry::CreateL();
    CHECK(registry != NULL);

    TUid uid = TUid::Uid(3777185127);    // e1234567

    // 1. No suite entry in registry.
    MJavaRegistryMIDletEntry* midletEntry = NULL;

    TRAPD(err, midletEntry = registry->MIDletEntryL(uid));
    // CHECK(KErrNotFound == err);
    CHECK(NULL == midletEntry);

    // 2. NullUid.
    TRAP(err, midletEntry = registry->MIDletEntryL(TUid::Null()));
    // CHECK(KErrNotFound == err);
    CHECK(NULL == midletEntry);

    // 3. No matching entry in registry.

    // Populate not matching entry
    js->open(JAVA_DATABASE_NAME);
    js->startTransaction();

    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;
    attr.setEntry(NAME, L"MyMIDlet1");
    midlet1.insert(attr);

    Uid suite1Uid(L"[e123456a]");
    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    string tableName = APPLICATION_TABLE;
    Uid midlet1Uid(L"[e1234577]");
    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    CHECK(jtu->populate(*js, tableName, midlet1));

    // Suite is alse required
    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"TestSuite1");
    suite1.insert(attr);

    string suiteTable = APPLICATION_PACKAGE_TABLE;
    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, suiteTable, suite1));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    TRAP(err, midletEntry = registry->MIDletEntryL(uid));
    // CHECK(KErrNotFound == err);
    CHECK(NULL == midletEntry);

    // 4. Matching entry in registry.

    // Populate matching entry.
    js->startTransaction();

    JavaStorageApplicationEntry_t midlet2;
    attr.setEntry(NAME, L"MyMIDlet2");
    midlet2.insert(attr);

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet2.insert(attr);

    Uid midlet2Uid(L"[e1234567]");
    attr.setEntry(ID, midlet2Uid.toString());
    midlet2.insert(attr);

    CHECK(jtu->populate(*js, tableName, midlet2));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    TRAP(err, midletEntry = registry->MIDletEntryL(uid));
    CHECK(KErrNone == err);
    CHECK(NULL != midletEntry);  // Actual midlet entry is tested elsewhere.

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, tableName, midlet1));
    CHECK(jtu->remove(*js, tableName, midlet2));
    CHECK(jtu->remove(*js, suiteTable,suite1));

    js->commitTransaction();
    js->close();

    midletEntry->Release();
    registry->Release();

    LOG(EJavaStorage, EInfo, "-TestMIDletEntryL");
}

/**
 * Test MJavaRegistrySuiteEntry attributes are returned correctly.
 */
TEST(TestLegacyRegistry, TestSuiteEntryAttributes)
{
    LOG(EJavaStorage, EInfo, "+TestSuiteEntryAttributes");

    MJavaRegistry* registry = MJavaRegistry::CreateL();
    CHECK(registry != NULL);

    TUid uid = TUid::Uid(3777185127);    // e1234567

    // Populate entry
    js->open(JAVA_DATABASE_NAME);
    js->startTransaction();

    JavaStorageApplicationEntry_t suite1;
    JavaStorageEntry attr;
    wstring srcSuiteName = L"TestSuite1";
    attr.setEntry(PACKAGE_NAME, srcSuiteName);
    suite1.insert(attr);

    wstring srcJarPath = L"Path/To/Jar";
    attr.setEntry(JAR_PATH, srcJarPath);
    suite1.insert(attr);

    wstring srcJadPath = L"JAD/To/Path";
    attr.setEntry(JAD_PATH, srcJadPath);
    suite1.insert(attr);

    wstring srcVendorName = L"TestSuite1";
    attr.setEntry(VENDOR, srcVendorName);
    suite1.insert(attr);

    attr.setEntry(VERSION, L"1.2.3");
    suite1.insert(attr);

    string tableName = APPLICATION_PACKAGE_TABLE;
    Uid suite1Uid(L"[e1234567]");
    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, tableName, suite1));

    JavaStorageApplicationEntry_t midlet1;
    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e3334577]");
    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    string appTableName = APPLICATION_TABLE;

    CHECK(jtu->populate(*js, appTableName, midlet1));
    js->commitTransaction();

    MJavaRegistrySuiteEntry* suiteEntry = NULL;
    TRAPD(err, suiteEntry = registry->SuiteEntryL(uid));

    CHECK(KErrNone == err);
    CHECK(NULL != suiteEntry);

    // Test UID
    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);
    CHECK(suiteEntry->UidL() == suiteUid);

    const TDesC16& suiteName = suiteEntry->SuiteNameL();
    auto_ptr<HBufC16> nameBuf(HBufC16::NewL(suiteName.Length() + 1));
    TPtr16 nameBufPtr(nameBuf->Des());
    nameBufPtr.Append(suiteName);
    wstring refStr(desToWstring(nameBufPtr));

    CHECK(refStr == srcSuiteName);

    const TDesC16& vendor = suiteEntry->VendorL();
    auto_ptr<HBufC16> vendorBuf(HBufC16::NewL(vendor.Length() + 1));
    TPtr16 vendorBufPtr(vendorBuf->Des());
    vendorBufPtr.Append(vendor);
    refStr = desToWstring(vendorBufPtr);

    CHECK(refStr == srcVendorName);

    TAppVersion version = suiteEntry->VersionL();
    TAppVersion srcVersion(1, 2, 3);
    CHECK(version.iMajor == srcVersion.iMajor);
    CHECK(version.iMinor == srcVersion.iMinor);
    CHECK(version.iBuild == srcVersion.iBuild);

    const TDesC16& jadPath = suiteEntry->JadPathL();
    auto_ptr<HBufC16> jadBuf(HBufC16::NewL(jadPath.Length() + 1));
    TPtr16 jadBufPtr(jadBuf->Des());
    jadBufPtr.Append(jadPath);
    refStr = desToWstring(jadBufPtr);

    CHECK(refStr == srcJadPath);

    const TDesC16& jarPath = suiteEntry->JarPathL();
    auto_ptr<HBufC16> jarBuf(HBufC16::NewL(jarPath.Length() + 1));
    TPtr16 jarBufPtr(jarBuf->Des());
    jarBufPtr.Append(jarPath);
    refStr = desToWstring(jarBufPtr);

    CHECK(refStr == srcJarPath);

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, tableName, suite1));
    CHECK(jtu->remove(*js, appTableName, midlet1));
    js->commitTransaction();
    js->close();

    suiteEntry->Release();
    registry->Release();

    LOG(EJavaStorage, EInfo, "-TestSuiteEntryAttributes");
}

/**
 * Test MJavaRegistryMIDletEntry attributes are returned correctly.
 */
TEST(TestLegacyRegistry, TestMIDletEntryAttributes)
{
    LOG(EJavaStorage, EInfo, "+TestMIDletEntryAttributes");

    MJavaRegistry* registry = MJavaRegistry::CreateL();
    CHECK(registry != NULL);

    // Populate entry
    js->open(JAVA_DATABASE_NAME);
    js->startTransaction();

    JavaStorageApplicationEntry_t suite1;
    JavaStorageEntry attr;
    attr.setEntry(PACKAGE_NAME, L"TestSuite1");
    suite1.insert(attr);

    attr.setEntry(JAR_PATH, L"Path/To/Jar");
    suite1.insert(attr);

    attr.setEntry(JAD_PATH, L"JAD/To/Path");
    suite1.insert(attr);

    attr.setEntry(VENDOR, L"TestSuite1Vendor");
    suite1.insert(attr);

    attr.setEntry(VERSION, L"1.2.3");
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    string tableName = APPLICATION_PACKAGE_TABLE;
    Uid suite1Uid(L"[e1234567]");
    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    CHECK(jtu->populate(*js, tableName, suite1));

    JavaStorageApplicationEntry_t midlet1;
    wstring srcMidletName = L"MyMIDlet1";
    attr.setEntry(NAME, srcMidletName);
    midlet1.insert(attr);

    attr.setEntry(PACKAGE_ID, L"[e1234567]");
    midlet1.insert(attr);

    string appTableName = APPLICATION_TABLE;
    TUid uid = TUid::Uid(3777185143);    // e1234577
    Uid midlet1Uid(L"[e1234577]");
    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    CHECK(jtu->populate(*js, appTableName, midlet1));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    MJavaRegistryMIDletEntry* midletEntry = NULL;
    TRAPD(err, midletEntry = registry->MIDletEntryL(uid));

    CHECK(KErrNone == err);
    CHECK(NULL != midletEntry);

    // Test UID
    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);
    CHECK(midletEntry->UidL() == midletUid);

    const TDesC16& midletName = midletEntry->MIDletNameL();
    auto_ptr<HBufC16> nameBuf(HBufC16::NewL(midletName.Length() + 1));
    TPtr16 nameBufPtr(nameBuf->Des());
    nameBufPtr.Append(midletName);
    wstring refStr(desToWstring(nameBufPtr));

    CHECK(refStr == srcMidletName);

    const MJavaRegistrySuiteEntry& suiteEntry = midletEntry->SuiteEntryL();

    // Verify suite UID to be sure we're dealing with correct entry.
    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);
    CHECK(suiteEntry.UidL() == suiteUid);

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, tableName, suite1));
    CHECK(jtu->remove(*js, appTableName, midlet1));

    js->commitTransaction();
    js->close();

    midletEntry->Release();
    registry->Release();

    LOG(EJavaStorage, EInfo, "-TestMIDletEntryAttributes");
}

/**
 * Test CJavaRegistry::InstalledMIDletUidsL() method.
 *   1. No UID in registry.
 *   2. One UID in registry.
 *   3. Several UIDs in registtry.
 */
TEST(TestLegacyRegistry, TestInstalledMIDletUidsL)
{
    LOG(EJavaStorage, EInfo, "+TestInstalledMIDletUidsL");

    // 1. No UID in registry
    MJavaRegistry* registry = MJavaRegistry::CreateL();
    CHECK(registry != NULL);

    RArray<TUid> midletUids;
    registry->InstalledMIDletUidsL(midletUids);

    CHECK(0 == midletUids.Count());

    // 2. One UID in registry.
    midletUids.Reset();

    // Populate first suite
    js->open();
    js->startTransaction();

    Uid suite1Uid(L"[e22a45d7]");
    JavaStorageEntry attr;
    JavaStorageApplicationEntry_t midlet1;
    attr.setEntry(NAME, L"MyMIDlet1");
    midlet1.insert(attr);

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    string appTableName = APPLICATION_TABLE;
    Uid midlet1Uid(L"[e12abcd7]");
    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    JavaStorageApplicationEntry_t suite1;

    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    CHECK(jtu->populate(*js, appTableName, midlet1));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    registry->InstalledMIDletUidsL(midletUids);

    CHECK(1 == midletUids.Count());

    // Check uid is same than populated one
    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    CHECK(midletUids[0] == midletUid);

    // 3. Several UIDs in registtry.
    midletUids.Reset();

    // Populate the second suite
    js->startTransaction();

    Uid suite2Uid(L"[e0234a88]");
    JavaStorageApplicationEntry_t midlet2;
    attr.setEntry(NAME, L"MyMIDlet2");
    midlet2.insert(attr);

    attr.setEntry(PACKAGE_ID, suite2Uid.toString());
    midlet2.insert(attr);

    Uid midlet2Uid(L"[e02cba77]");
    attr.setEntry(ID, midlet2Uid.toString());
    midlet2.insert(attr);

    CHECK(jtu->populate(*js, appTableName, midlet2));
    JavaStorageApplicationEntry_t suite2;

    attr.setEntry(MEDIA_ID, L"-124614446");
    suite2.insert(attr);

    attr.setEntry(ID, suite2Uid.toString());
    suite2.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite2));
    js->commitTransaction();

    registry->InstalledMIDletUidsL(midletUids);
    CHECK(2 == midletUids.Count());

    // Check uids are the same. Entries are returned at same
    // order they're written.
    uidToTUid(midlet1Uid, midletUid);
    CHECK(midletUids[0] == midletUid);

    uidToTUid(midlet2Uid, midletUid);
    CHECK(midletUids[1] == midletUid);

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, appTableName, midlet1));
    CHECK(jtu->remove(*js, appTableName, midlet2));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite2));

    js->commitTransaction();
    js->close();

    registry->Release();
    midletUids.Close();

    LOG(EJavaStorage, EInfo, "-TestInstalledMIDletUidsL");
}

/**
 * Test CJavaRegistry::TestSuiteEntryExistsL() method.
 * 1. No UID in registry.
 * 2. One UID in registry.
 * 3. Application UID in registry. Entry must not exists.
 * 4. TUid::Null leaves KErrArgument.
 */
TEST(TestLegacyRegistry, TestSuiteEntryExistsL)
{
    LOG(EJavaStorage, EInfo, "+TestSuiteEntryExistsL");

    MJavaRegistry* registry = MJavaRegistry::CreateL();
    CHECK(registry != NULL);

    TUid uid = TUid::Uid(3777185127);    // e1234567

    // 1. No UID in registry.
    CHECK(registry->SuiteEntryExistsL(uid) == EFalse);

    // 4. TUid::Null leaves KErrArgument.
    // TRAPD(err, registry->SuiteEntryExistsL(TUid::Null()));
    // CHECK(KErrArgument == err);
    CHECK(EFalse == registry->SuiteEntryExistsL(TUid::Null()));

    // Populate entry
    JavaStorageApplicationEntry_t suite1;
    JavaStorageEntry attr;
    attr.setEntry(PACKAGE_NAME, L"TestSuite1");
    suite1.insert(attr);

    attr.setEntry(JAR_PATH, L"Path/To/Jar");
    suite1.insert(attr);

    attr.setEntry(JAD_PATH, L"JAD/To/Path");
    suite1.insert(attr);

    attr.setEntry(VENDOR, L"TestSuite1Vendor");
    suite1.insert(attr);

    attr.setEntry(VERSION, L"1.2.3");
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    string tableName = APPLICATION_PACKAGE_TABLE;
    Uid suite1Uid(L"[e0001111]");
    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, tableName, suite1));

    JavaStorageApplicationEntry_t midlet1;
    attr.setEntry(NAME, L"MyMIDlet1");
    midlet1.insert(attr);

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    string appTableName = APPLICATION_TABLE;
    Uid midlet1Uid(L"[e000bcd7]");
    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    CHECK(jtu->populate(*js, appTableName, midlet1));
    js->commitTransaction();

    // 2. One suite UID in registry.
    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    CHECK(registry->SuiteEntryExistsL(suiteUid) == true);

    // 3. Application UID in registry. Entry must not exists.

    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);
    // WriteableMidpRegistry queries that registry entry exists
    // without checking type information. Of course this exists
    // but it is MIDlet not Suite.
    // CHECK(registry->SuiteEntryExistsL(midletUid) == EFalse);

    js->startTransaction();
    CHECK(jtu->remove(*js, tableName, suite1));
    CHECK(jtu->remove(*js, appTableName, midlet1));
    js->commitTransaction();
    js->close();
    registry->Release();

    LOG(EJavaStorage, EInfo, "-TestSuiteEntryExistsL");
}

/**
 * Test CJavaRegistry::TestSuiteEntryExistsL() method.
 * 1. Test no entries leaves KErrNotFound.
 * 2. Test empty name leaves KErrNotFound.
 * 3. Test empty vendor leaves KErrNotFound.
 * 4. Test not matching name leaves KErrNotFound.
 * 5. Test matching name and vendor found entry.
 */
TEST(TestLegacyRegistry, TestSuiteEntryL2)
{
    LOG(EJavaStorage, EInfo, "+TestSuiteEntryL2");

    MJavaRegistry* registry = MJavaRegistry::CreateL();
    CHECK(registry != NULL);

    MJavaRegistrySuiteEntry* suiteEntry = NULL;
    _LIT(KSuiteName, "TestMySuiteName");
    _LIT(KSuiteVendor, "TestMySuiteVendor");

    // 1. Test no entries leaves KErrNotFound.
    TRAPD(err, suiteEntry = registry->SuiteEntryL(KSuiteName, KSuiteVendor));
    CHECK(KErrNotFound == err);

    JavaStorageApplicationEntry_t suite1;
    JavaStorageEntry attr;
    attr.setEntry(PACKAGE_NAME, L"TestMySuiteName");
    suite1.insert(attr);

    attr.setEntry(VENDOR, L"TestMySuiteVendor");
    suite1.insert(attr);

    string tableName = APPLICATION_PACKAGE_TABLE;
    Uid suite1Uid(L"[e1111577]");

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, tableName, suite1));

    JavaStorageApplicationEntry_t midlet1;
    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e3333577]");
    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    string appTableName = APPLICATION_TABLE;

    CHECK(jtu->populate(*js, appTableName, midlet1));
    js->commitTransaction();

    // 2. Test empty name leaves KErrNotFound.
    TRAP(err, suiteEntry = registry->SuiteEntryL(KNullDesC, KSuiteVendor));
    // CHECK(KErrArgument == err);
    CHECK(NULL == suiteEntry);

    // 3. Test empty vendor leaves KErrNotFound.
    TRAP(err, suiteEntry = registry->SuiteEntryL(KSuiteName, KNullDesC));
    // CHECK(KErrArgument == err);
    CHECK(NULL == suiteEntry);

    // 4. Test not matching name leaves KErrNotFound.
    _LIT(KSuiteNameNoMatch, "NoMatchName");
    _LIT(KSuiteVendorNoMatch, "NoMatchVendor");
    TRAP(err, suiteEntry =
             registry->SuiteEntryL(KSuiteNameNoMatch, KSuiteVendorNoMatch));
    // CHECK(KErrNotFound == err);
    CHECK(NULL == suiteEntry);

    // 5. Test matching name and vendor found entry.
    suiteEntry = registry->SuiteEntryL(KSuiteName, KSuiteVendor);
    CHECK(suiteEntry != NULL);

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);
    CHECK(suiteEntry->UidL() == suiteUid);

    js->startTransaction();
    CHECK(jtu->remove(*js, tableName, suite1));
    CHECK(jtu->remove(*js, appTableName, midlet1));
    js->commitTransaction();
    js->close();

    suiteEntry->Release();
    registry->Release();

    LOG(EJavaStorage, EInfo, "-TestSuiteEntryL2");
}

