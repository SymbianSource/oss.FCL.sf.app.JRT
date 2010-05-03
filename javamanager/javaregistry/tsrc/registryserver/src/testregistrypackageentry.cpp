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

#include "javaregistry.h"
#include "javaregistryapplicationentry.h"
#include "javaregistrypackageentry.h"
#include "javastorage.h"
#include "javastorageexception.h"
#include "javastoragenames.h"
#include "javasymbianoslayer.h"
#include "javauid.h"
#include "logger.h"
#include "mjavaattribute.h"
#include "storagetestutils.h"

using namespace std;
using namespace Java;
using namespace java::storage;
using namespace java::util;

TEST_GROUP(TestRegistryPackageEntry)
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
 * Test CJavaRegistryPackageEntry::Version() method.
 * 1. Test version set.
 * 2. Test version not set.
 */
TEST(TestRegistryPackageEntry, TestVersion)
{
    LOG(EJavaStorage, EInfo, "+TestVersion");

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

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    // 2. Application suite uid.
    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    CJavaRegistryPackageEntry* entry =
        (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);

    TAppVersion appVersion = entry->Version();
    TAppVersion refVersion(0, 1, 0);

    CHECK(appVersion.iMajor == refVersion.iMajor);
    CHECK(appVersion.iMinor == refVersion.iMinor);
    CHECK(appVersion.iBuild == refVersion.iBuild);

    delete entry;
    entry = NULL;

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

    // 2. Test version not set.
    entry =
        (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);

    appVersion = entry->Version();
    TAppVersion refVersion2(0, 0, 0);

    CHECK(appVersion.iMajor == refVersion2.iMajor);
    CHECK(appVersion.iMinor == refVersion2.iMinor);
    CHECK(appVersion.iBuild == refVersion2.iBuild);

    delete entry;
    entry = NULL;

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite2));
    js->commitTransaction();
    js->close();
    LOG(EJavaStorage, EInfo, "-TestVersion");
}

/**
 * Test CJavaRegistryPackageEntry::Version() method.
 * 1. Test version set.
 * 2. Test version not set.
 */
TEST(TestRegistryPackageEntry, TestVendor)
{
    LOG(EJavaStorage, EInfo, "+TestVendor");
    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e22ab577]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e22ab588]");

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

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    // 2. Application suite uid.
    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    CJavaRegistryPackageEntry* entry =
        (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);

    _LIT(KSuiteVendor, "SuiteVendor");
    CHECK(entry->Vendor() == KSuiteVendor);

    delete entry;
    entry = NULL;

    Uid suite2Uid(L"[e22bb577]");

    JavaStorageApplicationEntry_t suite2;
    attr.setEntry(PACKAGE_NAME, L"MySuite2");
    suite2.insert(attr);

    attr.setEntry(ID, suite2Uid.toString());
    suite2.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite2.insert(attr);

    JavaStorageApplicationEntry_t midlet2;

    attr.setEntry(PACKAGE_ID, suite2Uid.toString());
    midlet2.insert(attr);

    Uid midlet2Uid(L"[e22bb588]");

    attr.setEntry(ID, midlet2Uid.toString());
    midlet2.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite2));
    js->commitTransaction();

    uidToTUid(suite2Uid, suiteUid);

    // 2. Test version not set.
    entry =
        (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);

    CHECK(entry->Vendor() == KNullDesC);

    delete entry;
    entry = NULL;

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite2));
    js->commitTransaction();
    js->close();

    LOG(EJavaStorage, EInfo, "-TestVendor");
}

// Currently not supported
/*
TEST(TestRegistryPackageEntry, TestIsUninstallable)
{
    FAIL("NOT IMPLEMENTED!");
}*/

/**
 * Test CJavaRegistryPackageEntry::GetEmbeddedEntries() method.
 * 1. Test suite contains one application.
 * 2. Test suite contains three applications.
 * 3. Test two suites are not interfering each others.
 */
TEST(TestRegistryPackageEntry, TestGetEmbeddedEntries)
{
    LOG(EJavaStorage, EInfo, "+TestGetEmbeddedEntries");

    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e22ba577]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e22ba588]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();

    // 2. Test suite contains one application.
    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());
    CJavaRegistryPackageEntry* entry =
        (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);

    RArray<TUid> embeddedUids;
    entry->GetEmbeddedEntries(embeddedUids);
    CHECK(embeddedUids.Count() == 1);

    delete entry;
    entry = NULL;
    embeddedUids.Reset();

    Uid suite3Uid(L"[e22baac7]");
    JavaStorageApplicationEntry_t suite3;
    attr.setEntry(PACKAGE_NAME, L"MySuite3");
    suite3.insert(attr);

    attr.setEntry(ID, suite3Uid.toString());
    suite3.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite3.insert(attr);

    JavaStorageApplicationEntry_t midlet2;

    attr.setEntry(PACKAGE_ID, suite3Uid.toString());
    midlet2.insert(attr);

    Uid midlet2Uid(L"[e22bacc8]");

    attr.setEntry(ID, midlet2Uid.toString());
    midlet2.insert(attr);

    JavaStorageApplicationEntry_t midlet3;

    attr.setEntry(PACKAGE_ID, suite3Uid.toString());
    midlet3.insert(attr);

    Uid midlet3Uid(L"[e22baa88]");

    attr.setEntry(ID, midlet3Uid.toString());
    midlet3.insert(attr);

    JavaStorageApplicationEntry_t midlet4;

    attr.setEntry(PACKAGE_ID, suite3Uid.toString());
    midlet4.insert(attr);

    Uid midlet4Uid(L"[e22baaa8]");

    attr.setEntry(ID, midlet4Uid.toString());
    midlet4.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet3));
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet4));
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite3));
    js->commitTransaction();

    uidToTUid(suite3Uid, suiteUid);

    // 3. Test suite contains three applications.
    entry =
        (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);

    entry->GetEmbeddedEntries(embeddedUids);
    CHECK(embeddedUids.Count() == 3);

    delete entry;
    entry = NULL;
    embeddedUids.Reset();

    // 4. Test two suites are not interfering each others.
    // Add midlet to suite1
    JavaStorageApplicationEntry_t midlet5;

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet5.insert(attr);

    Uid midlet5Uid(L"[e22bac88]");

    attr.setEntry(ID, midlet5Uid.toString());
    midlet5.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet5));
    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    entry =
        (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);

    entry->GetEmbeddedEntries(embeddedUids);
    CHECK(embeddedUids.Count() == 3);

    delete entry;
    entry = NULL;
    embeddedUids.Reset();

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet3));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet4));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet5));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite3));
    js->commitTransaction();
    js->close();
    LOG(EJavaStorage, EInfo, "-TestGetEmbeddedEntries");
}
/**
 * Test CJavaRegistryPackageEntry::NumberOfEmbeddedEntries() method.
 * 1. Test suite contains one application.
 * 2. Test suite contains three applications.
 * 3. Test two suites are not interfering each others.
 */
TEST(TestRegistryPackageEntry, TestNumberOfEmbeddedEntries)
{
    LOG(EJavaStorage, EInfo, "+TestNumberOfEmbeddedEntries");
    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e22ba577]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e22ba588]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();

    // 1. Test suite contains one application.
    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());
    CJavaRegistryPackageEntry* entry =
        (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);

    CHECK(entry != NULL);
    CHECK(entry->NumberOfEmbeddedEntries() == 1);

    delete entry;
    entry = NULL;

    Uid suite3Uid(L"[e22baac7]");
    JavaStorageApplicationEntry_t suite3;
    attr.setEntry(PACKAGE_NAME, L"MySuite3");
    suite3.insert(attr);

    attr.setEntry(ID, suite3Uid.toString());
    suite3.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite3.insert(attr);

    JavaStorageApplicationEntry_t midlet2;

    attr.setEntry(PACKAGE_ID, suite3Uid.toString());
    midlet2.insert(attr);

    Uid midlet2Uid(L"[e22bacc8]");

    attr.setEntry(ID, midlet2Uid.toString());
    midlet2.insert(attr);

    JavaStorageApplicationEntry_t midlet3;

    attr.setEntry(PACKAGE_ID, suite3Uid.toString());
    midlet3.insert(attr);

    Uid midlet3Uid(L"[e22baa88]");

    attr.setEntry(ID, midlet3Uid.toString());
    midlet3.insert(attr);

    JavaStorageApplicationEntry_t midlet4;

    attr.setEntry(PACKAGE_ID, suite3Uid.toString());
    midlet4.insert(attr);

    Uid midlet4Uid(L"[e22baaa8]");

    attr.setEntry(ID, midlet4Uid.toString());
    midlet4.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet3));
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet4));
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite3));
    js->commitTransaction();

    uidToTUid(suite3Uid, suiteUid);

    // 2. Test suite contains three applications.
    entry =
        (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);

    CHECK(entry != NULL);
    CHECK(entry->NumberOfEmbeddedEntries() == 3);

    delete entry;
    entry = NULL;

    // 3. Test two suites are not interfering each others.
    // Add midlet to suite1
    JavaStorageApplicationEntry_t midlet5;

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet5.insert(attr);

    Uid midlet5Uid(L"[e22bac88]");

    attr.setEntry(ID, midlet5Uid.toString());
    midlet5.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet5));
    js->commitTransaction();

    entry =
        (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);

    CHECK(entry != NULL);
    CHECK(entry->NumberOfEmbeddedEntries() == 3);

    delete entry;
    entry = NULL;

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet3));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet4));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet5));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite3));
    js->commitTransaction();
    js->close();

    LOG(EJavaStorage, EInfo, "-TestNumberOfEmbeddedEntries");
}

/**
 * Test CJavaRegistryPackageEntry::EmbeddedEntryByUidL() method.
 * 1. Test suite contains one application.
 * 2. Test suite contains three applications.
 *    2.1 Test get first entry.
 *    2.2 Test get second entry.
 *    2.3 Test get thrird entry.
 * 3. Test get non existing Uid.
 */
TEST(TestRegistryPackageEntry, TestEmbeddedEntryByUidL)
{
    LOG(EJavaStorage, EInfo, "+TestEmbeddedEntryByUidL");
    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e22bac77]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e22bac88]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();

    // 1. Test suite contains one application.
    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());
    CJavaRegistryPackageEntry* entry =
        (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);

    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    CJavaRegistryApplicationEntry* appEntry = (CJavaRegistryApplicationEntry*)
            entry->EmbeddedEntryByUidL(midletUid);
    CHECK(appEntry != NULL);

    // Test correct one returned.
    CHECK(appEntry->Uid() == midletUid);

    delete appEntry;
    appEntry = NULL;

    delete entry;
    entry = NULL;

    Uid suite3Uid(L"[e22baad7]");
    JavaStorageApplicationEntry_t suite3;
    attr.setEntry(PACKAGE_NAME, L"MySuite3");
    suite3.insert(attr);

    attr.setEntry(ID, suite3Uid.toString());
    suite3.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite3.insert(attr);

    JavaStorageApplicationEntry_t midlet2;

    attr.setEntry(PACKAGE_ID, suite3Uid.toString());
    midlet2.insert(attr);

    Uid midlet2Uid(L"[e22bacd8]");

    attr.setEntry(ID, midlet2Uid.toString());
    midlet2.insert(attr);

    JavaStorageApplicationEntry_t midlet3;

    attr.setEntry(PACKAGE_ID, suite3Uid.toString());
    midlet3.insert(attr);

    Uid midlet3Uid(L"[e22baad8]");

    attr.setEntry(ID, midlet3Uid.toString());
    midlet3.insert(attr);

    JavaStorageApplicationEntry_t midlet4;

    attr.setEntry(PACKAGE_ID, suite3Uid.toString());
    midlet4.insert(attr);

    Uid midlet4Uid(L"[e22cccd8]");

    attr.setEntry(ID, midlet4Uid.toString());
    midlet4.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet3));
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet4));
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite3));
    js->commitTransaction();

    uidToTUid(suite3Uid, suiteUid);

    // 2. Test suite contains three applications.
    entry =
        (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);

    // 2.1. Test get first entry.
    uidToTUid(midlet2Uid, midletUid);

    appEntry = (CJavaRegistryApplicationEntry*)
               entry->EmbeddedEntryByUidL(midletUid);
    CHECK(appEntry != NULL);

    // Test correct one returned.
    CHECK(appEntry->Uid() == midletUid);

    delete appEntry;
    appEntry = NULL;

    // 2.2. Test get second entry.
    uidToTUid(midlet3Uid, midletUid);

    appEntry = (CJavaRegistryApplicationEntry*)
               entry->EmbeddedEntryByUidL(midletUid);
    CHECK(appEntry != NULL);

    // Test correct one returned.
    CHECK(appEntry->Uid() == midletUid);

    delete appEntry;
    appEntry = NULL;

    // 2.3. Test get third entry.
    uidToTUid(midlet4Uid, midletUid);

    appEntry = (CJavaRegistryApplicationEntry*)
               entry->EmbeddedEntryByUidL(midletUid);
    CHECK(appEntry != NULL);

    // Test correct one returned.
    CHECK(appEntry->Uid() == midletUid);

    delete appEntry;
    appEntry = NULL;

    // 4. Test get non existing Uid.
    Uid nonExistingUid(L"[e22baddd]");
    uidToTUid(nonExistingUid, midletUid);

    appEntry = (CJavaRegistryApplicationEntry*)
               entry->EmbeddedEntryByUidL(midletUid);
    CHECK(appEntry == NULL);

    delete entry;
    entry = NULL;

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet3));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet4));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite3));
    js->commitTransaction();
    js->close();

    LOG(EJavaStorage, EInfo, "-TestEmbeddedEntryByUidL");
}

/**
 * Test CJavaRegistryPackageEntry::EmbeddedEntryByNumberL() method.
 * 1. Test suite contains one application.
 * 2. Test suite contains three applications.
 *    2.1 Test get first entry.
 *    2.2 Test get second entry.
 *    2.3 Test get thrird entry.
 * 3. Test negative index.
 * 4. Test index greater than embedded entries.
 *
 * NOTE: Order is not quaranteed so it might be that test case must be
 * changed to check only entry is received but not to check its parameters.
 */
TEST(TestRegistryPackageEntry, TestEmbeddedEntryByNumberL)
{
    LOG(EJavaStorage, EInfo, "+TestEmbeddedEntryByNumberL");
    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[ed2bac77]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[ed2bac88]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();

    // 1. Test suite contains one application.
    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());
    CJavaRegistryPackageEntry* entry =
        (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);

    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    CJavaRegistryApplicationEntry* appEntry = (CJavaRegistryApplicationEntry*)
            entry->EmbeddedEntryByNumberL(0);
    CHECK(appEntry != NULL);

    // Test correct one returned.
    CHECK(appEntry->Uid() == midletUid);

    delete appEntry;
    appEntry = NULL;

    delete entry;
    entry = NULL;

    Uid suite3Uid(L"[ed2baad7]");
    JavaStorageApplicationEntry_t suite3;
    attr.setEntry(PACKAGE_NAME, L"MySuite3");
    suite3.insert(attr);

    attr.setEntry(ID, suite3Uid.toString());
    suite3.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite3.insert(attr);

    JavaStorageApplicationEntry_t midlet2;

    attr.setEntry(PACKAGE_ID, suite3Uid.toString());
    midlet2.insert(attr);

    Uid midlet2Uid(L"[ed2bacd8]");

    attr.setEntry(ID, midlet2Uid.toString());
    midlet2.insert(attr);

    JavaStorageApplicationEntry_t midlet3;

    attr.setEntry(PACKAGE_ID, suite3Uid.toString());
    midlet3.insert(attr);

    Uid midlet3Uid(L"[ed2baad8]");

    attr.setEntry(ID, midlet3Uid.toString());
    midlet3.insert(attr);

    JavaStorageApplicationEntry_t midlet4;

    attr.setEntry(PACKAGE_ID, suite3Uid.toString());
    midlet4.insert(attr);

    Uid midlet4Uid(L"[ed2cccd8]");

    attr.setEntry(ID, midlet4Uid.toString());
    midlet4.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet3));
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet4));
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite3));
    js->commitTransaction();

    uidToTUid(suite3Uid, suiteUid);

    // 2. Test suite contains three applications.
    entry =
        (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);

    // 2.1. Test get first entry.
    uidToTUid(midlet2Uid, midletUid);

    appEntry = (CJavaRegistryApplicationEntry*)
               entry->EmbeddedEntryByNumberL(0);
    CHECK(appEntry != NULL);

    // Test correct one returned.
    CHECK(appEntry->Uid() == midletUid);

    delete appEntry;
    appEntry = NULL;

    // 2.2. Test get second entry.
    uidToTUid(midlet3Uid, midletUid);

    appEntry = (CJavaRegistryApplicationEntry*)
               entry->EmbeddedEntryByNumberL(1);
    CHECK(appEntry != NULL);

    // Test correct one returned.
    CHECK(appEntry->Uid() == midletUid);

    delete appEntry;
    appEntry = NULL;

    // 2.3. Test get third entry.
    uidToTUid(midlet4Uid, midletUid);

    appEntry = (CJavaRegistryApplicationEntry*)
               entry->EmbeddedEntryByNumberL(2);
    CHECK(appEntry != NULL);

    // Test correct one returned.
    CHECK(appEntry->Uid() == midletUid);

    delete appEntry;
    appEntry = NULL;

    // 3. Test negative index.
    TRAPD(err, appEntry = (CJavaRegistryApplicationEntry*)
                          entry->EmbeddedEntryByNumberL(-1));
    CHECK(NULL == appEntry);

    // 4. Test index greater than embedded entries.
    appEntry = (CJavaRegistryApplicationEntry*)
               entry->EmbeddedEntryByNumberL(3);
    CHECK(appEntry == NULL);

    delete entry;
    entry = NULL;

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet3));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet4));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite3));
    js->commitTransaction();
    js->close();

    LOG(EJavaStorage, EInfo, "-TestEmbeddedEntryByNumberL");
}
