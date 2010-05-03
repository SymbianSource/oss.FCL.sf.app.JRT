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

TEST_GROUP(TestInstalledAppsRegistryEntry)
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
 * Test CInstalledAppsRegistryEntry::SizeL() method.
 * 1. Test no application suite size set. 0 as size.
 * 2. Test no application suite size set via application entry. 0 as size.
 * 3. Test size set to 0.
 * 4. Test size set to 12345.
 * 5. Test size set to 12345.
 */
TEST(TestInstalledAppsRegistryEntry, TestSizeL)
{
    // This will always fail as size is not asked from server but it is
    // calculated from file system which is not populated.

    /*LOG(EJavaStorage, EInfo, "+TestSizeL");

    MInstalledAppsRegistry* appsRegistry = MInstalledAppsRegistry::CreateL();
    CleanupReleasePushL(*appsRegistry);

    JavaStorageApplicationEntry_t suite1;
    JavaStorageEntry attr;
    attr.setEntry(PACKAGE_NAME, L"TestMIDletByUidL");
    suite1.insert(attr);

    string tableName = APPLICATION_PACKAGE_TABLE;
    Uid suite1Uid(L"[e1230043]");
    attr.setEntry(ID, suite1Uid.toString());
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

    // 1. No suite size set.
    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);
    MInstalledAppsRegistryEntry* entry = appsRegistry->EntryL(suiteUid);
    CleanupReleasePushL(*entry);

    CHECK(entry->SizeL() == 0);

    CleanupStack::PopAndDestroy(entry);

    // 1. No suite size set via application.
    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    entry = appsRegistry->EntryL(midletUid);
    CleanupReleasePushL(*entry);

    CHECK(entry->SizeL() == 0);
    CleanupStack::PopAndDestroy(entry);

    JavaStorageApplicationEntry_t updateEntry;
    attr.setEntry(INITIAL_SIZE, L"0");
    updateEntry.insert(attr);
    js->startTransaction();
    CHECK(jtu->update(*js, tableName, updateEntry, suite1));
    js->commitTransaction();

    entry = appsRegistry->EntryL(suiteUid);
    CleanupReleasePushL(*entry);

    CHECK(entry->SizeL() == 0);
    CleanupStack::PopAndDestroy(entry);

    // 3. Test size set to 0.
    uidToTUid(midlet1Uid, midletUid);

    entry = appsRegistry->EntryL(midletUid);
    CleanupReleasePushL(*entry);

    CHECK(entry->SizeL() == 0);
    CleanupStack::PopAndDestroy(entry);

    attr.setEntry(INITIAL_SIZE, L"12345");
    updateEntry.insert(attr);
    js->startTransaction();
    CHECK(jtu->update(*js, tableName, updateEntry, suite1));
    js->commitTransaction();

    // 4. Test size set to 12345.
    entry = appsRegistry->EntryL(suiteUid);
    CleanupReleasePushL(*entry);

    CHECK(entry->SizeL() == 12345);
    CleanupStack::PopAndDestroy(entry);

    // 5. Test size set to 12345 via application.
    uidToTUid(midlet1Uid, midletUid);

    entry = appsRegistry->EntryL(midletUid);
    CleanupReleasePushL(*entry);

    CHECK(entry->SizeL() == 12345);
    CleanupStack::PopAndDestroy(entry);

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, tableName, suite1));
    CHECK(jtu->remove(*js, appTableName, midlet1));
    js->commitTransaction();
    js->close();

    CleanupStack::PopAndDestroy(appsRegistry);
    LOG(EJavaStorage, EInfo, "-TestSizeL");*/
}

/**
 * Test CInstalledAppsRegistryEntry::UidL() method.
 * 1. Application uid.
 * 2. Application suite uid.
 * NOTE: Invalid Uid cannot be tested as it cannot be queried.
 */
TEST(TestInstalledAppsRegistryEntry, TestUidL)
{
    LOG(EJavaStorage, EInfo, "+TestUidL");

    MInstalledAppsRegistry* appsRegistry = MInstalledAppsRegistry::CreateL();
    CleanupReleasePushL(*appsRegistry);
    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e22a4577]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e22a4588]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();

    // 1. Application uid.
    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    MInstalledAppsRegistryEntry* entry = NULL;
    TRAPD(err, entry = appsRegistry->EntryL(midletUid));
    // CHECK(entry != NULL);
    CHECK(KErrNotFound == err);
    // CleanupReleasePushL(*entry);
    // CHECK(entry->UidL() == midletUid);
    // CleanupStack::PopAndDestroy(entry);

    // 2. Application suite uid.
    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    entry = appsRegistry->EntryL(suiteUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);

    CHECK(entry->UidL() == suiteUid);
    CleanupStack::PopAndDestroy(entry);

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();
    js->close();

    CleanupStack::PopAndDestroy(appsRegistry);
    LOG(EJavaStorage, EInfo, "-TestUidL");
}

/**
 * Test CInstalledAppsRegistryEntry::VersionL() method.
 * 1. Test version set via suite.
 * 2. Test version via application.
 * 3. Test version not set via suite. Version is 0.0.0.
 * 4. Test version not set via application. Version is 0.0.0.
 */
TEST(TestInstalledAppsRegistryEntry, TestVersionL)
{
    LOG(EJavaStorage, EInfo, "+TestVersionL");

    MInstalledAppsRegistry* appsRegistry = MInstalledAppsRegistry::CreateL();
    CleanupReleasePushL(*appsRegistry);

    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e22a4577]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e22a4588]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    suite1.insert(attr);

    wstring vendor = L"SuiteVendor";
    attr.setEntry(VENDOR, vendor);
    suite1.insert(attr);

    attr.setEntry(VERSION, L"0.1");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);
    MInstalledAppsRegistryEntry* entry = appsRegistry->EntryL(suiteUid);
    CHECK(entry != NULL);

    CleanupReleasePushL(*entry);

    TAppVersion appVersion = entry->VersionL();

    TAppVersion refVersion(0, 1, 0);

    CHECK(appVersion.iMajor == refVersion.iMajor);
    CHECK(appVersion.iMinor == refVersion.iMinor);
    CHECK(appVersion.iBuild == refVersion.iBuild);

    CleanupStack::PopAndDestroy(entry);


    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);
    TRAPD(err, entry = appsRegistry->EntryL(midletUid));
    CHECK(KErrNotFound == err);
    // CHECK(entry != NULL);
    // CleanupReleasePushL(*entry);

    // appVersion = entry->VersionL();
    // CHECK(appVersion.iMajor == refVersion.iMajor);
    // CHECK(appVersion.iMinor == refVersion.iMinor);
    // CHECK(appVersion.iBuild == refVersion.iBuild);

    // CleanupStack::PopAndDestroy(entry);

    Uid suite2Uid(L"[e22b4577]");

    JavaStorageApplicationEntry_t suite2;
    attr.setEntry(PACKAGE_NAME, L"MySuite2");
    suite2.insert(attr);

    attr.setEntry(VENDOR, L"Inc. inc.");
    suite2.insert(attr);

    attr.setEntry(ID, suite2Uid.toString());
    suite2.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite2.insert(attr);

    JavaStorageApplicationEntry_t midlet2;

    attr.setEntry(PACKAGE_ID, suite2Uid.toString());
    midlet2.insert(attr);

    Uid midlet2Uid(L"[e22b4588]");

    attr.setEntry(ID, midlet2Uid.toString());
    midlet2.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite2));
    js->commitTransaction();

    uidToTUid(suite2Uid, suiteUid);

    TAppVersion refVersion2(0, 0, 0);

    entry = appsRegistry->EntryL(suiteUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);

    appVersion = entry->VersionL();
    CHECK(appVersion.iMajor == refVersion2.iMajor);
    CHECK(appVersion.iMinor == refVersion2.iMinor);
    CHECK(appVersion.iBuild == refVersion2.iBuild);

    CleanupStack::PopAndDestroy(entry);

    uidToTUid(midlet2Uid, midletUid);
    TRAP(err, entry = appsRegistry->EntryL(midletUid));
    CHECK(KErrNotFound == err); // Only suite can be get via EntryL
    // CHECK(entry != NULL);
    // CleanupReleasePushL(*entry);

    // appVersion = entry->VersionL();
    // CHECK(appVersion.iMajor == refVersion2.iMajor);
    // CHECK(appVersion.iMinor == refVersion2.iMinor);
    // CHECK(appVersion.iBuild == refVersion2.iBuild);

    // CleanupStack::PopAndDestroy(entry);

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite2));
    js->commitTransaction();
    js->close();

    CleanupStack::PopAndDestroy(appsRegistry);
    LOG(EJavaStorage, EInfo, "-TestVersionL");
}

/**
 * Test CInstalledAppsRegistryEntry::DriveL() method.
 * 1. Test application suite drive.
 * 2. Test application drive.
 */
TEST(TestInstalledAppsRegistryEntry, TestDriveL)
{
    LOG(EJavaStorage, EInfo, "+TestDriveL");

    MInstalledAppsRegistry* appsRegistry = MInstalledAppsRegistry::CreateL();
    CleanupReleasePushL(*appsRegistry);
    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[eaaa4577]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[eaaa4588]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    attr.setEntry(ROOT_PATH, L"e:\\private\\MIDlets\\[eaaa4577]");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();

    // 1. Application uid.
    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    MInstalledAppsRegistryEntry* entry = NULL;
    TRAPD(err, entry = appsRegistry->EntryL(midletUid));
    // CHECK(entry != NULL);
    CHECK(KErrNotFound == err); // Only suite can be get.

    // CleanupReleasePushL(*entry);
    // CHECK(entry->DriveL() == 'E');
    // CleanupStack::PopAndDestroy(entry);

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    entry = appsRegistry->EntryL(suiteUid);
    CHECK(entry != NULL);

    CleanupReleasePushL(*entry);

    CHECK(entry->DriveL() == 'E');

    CleanupStack::PopAndDestroy(entry);

    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();
    js->close();

    CleanupStack::PopAndDestroy(appsRegistry);
    LOG(EJavaStorage, EInfo, "-TestDriveL");
}

/**
 * Test CInstalledAppsRegistryEntry::IsInRomL() method.
 * 1. Test application suite not in ROM.
 * 2. Test application not in ROM.
 * 3. Test application suite in ROM.
 * 4. Test application in ROM.
 */
TEST(TestInstalledAppsRegistryEntry, TestIsInRomL)
{
    LOG(EJavaStorage, EInfo, "+TestIsInRomL");

    MInstalledAppsRegistry* appsRegistry = MInstalledAppsRegistry::CreateL();
    CleanupReleasePushL(*appsRegistry);

    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e0004577]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e0004588]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    attr.setEntry(JAR_PATH, L"e:\\private\\MIDlets\\[e0004577]");
    suite1.insert(attr);

    attr.setEntry(ROOT_PATH, L"e:\\private\\MIDlets\\[e0004577]");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();

    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);
    MInstalledAppsRegistryEntry* entry = NULL;
    TRAPD(err, entry = appsRegistry->EntryL(midletUid));

    // CHECK(KErrNotFound == err);
    // CHECK(entry != NULL);
    // CleanupReleasePushL(*entry);

    // 2. Test application not in ROM.
    // CHECK(entry->IsInRomL() == EFalse);

    // CleanupStack::PopAndDestroy(entry);

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    // 1. Test application suite not in ROM.
    entry = appsRegistry->EntryL(suiteUid);
    CHECK(entry != NULL);

    CleanupReleasePushL(*entry);
    CHECK(entry->IsInRomL() == EFalse);

    CleanupStack::PopAndDestroy(entry);

    // Move suite to ROM.
    JavaStorageApplicationEntry_t updateEntry;
    attr.setEntry(JAR_PATH, L"z:\\private\\MIDlets\\[e0004577]");
    updateEntry.insert(attr);

    attr.setEntry(ROOT_PATH, L"z:\\private\\MIDlets\\[e0004577]");
    suite1.insert(attr);

    js->startTransaction();
    CHECK(jtu->update(*js, APPLICATION_PACKAGE_TABLE, updateEntry, suite1));
    js->commitTransaction();

    // TRAP(err, entry = appsRegistry->EntryL(midletUid));
    //CHECK(KErrNotFound == err);
    // CHECK(entry != NULL);
    // CleanupReleasePushL(*entry);

    // 4. Test application in ROM.
    // CHECK(entry->IsInRomL() == true);
    // CleanupStack::PopAndDestroy(entry);

    entry = appsRegistry->EntryL(suiteUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);

    // 3. Test application suite in ROM.
    // CHECK(entry->IsInRomL() == true);

    // According to current implementation when application is installed
    // to system using S60 JavaIntaller (as always) isInRom is always set
    // to EFalse thus it cannot be set ETrue after that.
    CHECK(entry->IsInRomL() == false);

    CleanupStack::PopAndDestroy(entry);

    suite1.clear();
    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);



    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();
    js->close();

    CleanupStack::PopAndDestroy(appsRegistry);
    LOG(EJavaStorage, EInfo, "-TestIsInRomL");
}
