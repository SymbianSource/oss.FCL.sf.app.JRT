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

#include "javaprocessconstants.h"
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

    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
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

    js->open();
    js->startTransaction();

    JavaStorageApplicationEntry_t midlet2;

    attr.setEntry(PACKAGE_ID, suite2Uid.toString());
    midlet2.insert(attr);

    Uid midlet2Uid(L"[e22b4588]");

    attr.setEntry(ID, midlet2Uid.toString());
    midlet2.insert(attr);

    js->open();
    js->startTransaction();

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite2));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
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

    _LIT(KSuiteVendor, "SuiteVendor");
    CHECK(entry->Vendor() == KSuiteVendor);

    delete entry;
    entry = NULL;

    Uid suite2Uid(L"[e22bb577]");

    JavaStorageApplicationEntry_t suite2;
    attr.setEntry(PACKAGE_NAME, L"MySuite2");
    suite2.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite2.insert(attr);

    attr.setEntry(ID, suite2Uid.toString());
    suite2.insert(attr);

    js->open();
    js->startTransaction();

    JavaStorageApplicationEntry_t midlet2;

    attr.setEntry(PACKAGE_ID, suite2Uid.toString());
    midlet2.insert(attr);

    Uid midlet2Uid(L"[e22bb588]");

    attr.setEntry(ID, midlet2Uid.toString());
    midlet2.insert(attr);

    js->open();
    js->startTransaction();

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite2));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
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

    js->open();
    js->startTransaction();

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
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
    embeddedUids.Close();

    Uid suite3Uid(L"[e22baac7]");
    JavaStorageApplicationEntry_t suite3;
    attr.setEntry(PACKAGE_NAME, L"MySuite3");
    suite3.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite3.insert(attr);

    attr.setEntry(ID, suite3Uid.toString());
    suite3.insert(attr);

    js->open();
    js->startTransaction();

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

    js->open();
    js->startTransaction();

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet3));
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet4));
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite3));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
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
    embeddedUids.Close();

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
    embeddedUids.Close();

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

    js->open();
    js->startTransaction();

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
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
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite3.insert(attr);

    attr.setEntry(ID, suite3Uid.toString());
    suite3.insert(attr);

    js->open();
    js->startTransaction();

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

    js->open();
    js->startTransaction();

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet3));
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet4));
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite3));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
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

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet5));
    // Session must be committed before next use of Registry otherwise
    // it is locked.
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

    js->open();
    js->startTransaction();

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
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
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite3.insert(attr);

    attr.setEntry(ID, suite3Uid.toString());
    suite3.insert(attr);

    js->open();
    js->startTransaction();

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

    js->open();
    js->startTransaction();

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet3));
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet4));
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite3));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
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

    js->open();
    js->startTransaction();

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
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
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite3.insert(attr);

    attr.setEntry(ID, suite3Uid.toString());
    suite3.insert(attr);

    js->open();
    js->startTransaction();

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

    js->open();
    js->startTransaction();

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet3));
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet4));
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite3));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
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
    CHECK(KErrArgument == err);

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

/**
 * Test CJavaRegistryPackageEntry::IsUninstallable() method.
 * 1. Test Block uninstall attribute not present. ETrue returned.
 * 2. Test attribute present and suite does not belong to manufacturer domain.
 *    ETrue returned.
 * 3. Test attribute present and value is set to false
 * 4. Test attribute present, suite belongs to operator domain and
 *    attribute value is set true. EFalse returned.
 * 5. Test attribute present, suite belongs to operator domain and
 *    attribute value is set True. EFalse returned.
 */
TEST(TestRegistryPackageEntry, TestIsUninstallable)
{
    LOG(EJavaStorage, EInfo, "+TestIsUninstallable");

    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[ebbbd577]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[ebbbd588]");

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

    attr.setEntry(ROOT_PATH, L"e:\\private\\MIDlets\\[ebbbd577]");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    CJavaRegistryPackageEntry* entry =
        (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);

    // 1. Test Block uninstall attribute not present. ETrue returned.
    CHECK(entry->IsUninstallable() == true);

    delete entry;
    entry = NULL;

    // Populate attributes
    JavaStorageApplicationEntry_t suiteAttribute;
    attr.setEntry(ID, suite1Uid.toString());
    suiteAttribute.insert(attr);

    wstring blockUninstall = L"Nokia-MIDlet-Block-Uninstall";

    attr.setEntry(NAME, blockUninstall);
    suiteAttribute.insert(attr);

    attr.setEntry(VALUE, L"true");
    suiteAttribute.insert(attr);

    attr.setEntry(TRUSTED, L"0", JavaStorageEntry::INT);
    suiteAttribute.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, suiteAttribute));
    suiteAttribute.clear();
    js->commitTransaction();

    // Populate to thrird party domain
    JavaStorageApplicationEntry_t securityEntry;
    attr.setEntry(ID, suite1Uid.toString());
    securityEntry.insert(attr);

    attr.setEntry(SECURITY_DOMAIN_CATEGORY, L"ITPD");
    securityEntry.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, MIDP_PACKAGE_TABLE, securityEntry));
    js->commitTransaction();
    securityEntry.clear();

    // 2. Test attribute present and suite does not belong to manufacturer domain.
    //    ETrue returned.
    entry = (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->IsUninstallable() == true);

    delete entry;
    entry = NULL;

    // Update value to false
    JavaStorageApplicationEntry_t matchEntry;
    attr.setEntry(ID, suite1Uid.toString());
    matchEntry.insert(attr);

    attr.setEntry(NAME, blockUninstall);
    matchEntry.insert(attr);
    attr.setEntry(VALUE, L"true");
    matchEntry.insert(attr);
    attr.setEntry(TRUSTED, L"0", JavaStorageEntry::INT);
    matchEntry.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suiteAttribute.insert(attr);

    attr.setEntry(NAME, blockUninstall);
    suiteAttribute.insert(attr);
    attr.setEntry(VALUE, L"false");
    suiteAttribute.insert(attr);
    attr.setEntry(TRUSTED, L"0", JavaStorageEntry::INT);
    suiteAttribute.insert(attr);

    js->startTransaction();
    CHECK(jtu->update(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, suiteAttribute, matchEntry));
    js->commitTransaction();
    suiteAttribute.clear();
    matchEntry.clear();

    // 3. Test attribute present and value is set to false
    entry = (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->IsUninstallable() == true);

    delete entry;
    entry = NULL;

    // Update domain to operator domain
    attr.setEntry(ID, suite1Uid.toString());
    matchEntry.insert(attr);
    securityEntry.insert(attr);

    attr.setEntry(SECURITY_DOMAIN_CATEGORY, L"OPD");
    securityEntry.insert(attr);

    js->startTransaction();
    CHECK(jtu->update(*js, MIDP_PACKAGE_TABLE, securityEntry, matchEntry));
    js->commitTransaction();
    securityEntry.clear();
    matchEntry.clear();

    // Update value to true
    attr.setEntry(ID, suite1Uid.toString());
    matchEntry.insert(attr);

    attr.setEntry(NAME, blockUninstall);
    matchEntry.insert(attr);
    attr.setEntry(VALUE, L"false");
    matchEntry.insert(attr);
    attr.setEntry(TRUSTED, L"0", JavaStorageEntry::INT);
    matchEntry.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suiteAttribute.insert(attr);

    attr.setEntry(NAME, blockUninstall);
    suiteAttribute.insert(attr);
    attr.setEntry(VALUE, L"true");
    suiteAttribute.insert(attr);
    attr.setEntry(TRUSTED, L"1", JavaStorageEntry::INT);
    suiteAttribute.insert(attr);

    js->startTransaction();
    CHECK(jtu->update(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, suiteAttribute, matchEntry));
    js->commitTransaction();
    suiteAttribute.clear();
    matchEntry.clear();

    // 4. Test attribute present, suite belongs to operator domain and
    //    attribute value is set true. EFalse returned.
    entry = (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->IsUninstallable() == false);

    delete entry;
    entry = NULL;

    // Update domain to manufacturer
    attr.setEntry(ID, suite1Uid.toString());
    matchEntry.insert(attr);
    securityEntry.insert(attr);

    attr.setEntry(SECURITY_DOMAIN_CATEGORY, L"MFD");
    securityEntry.insert(attr);

    js->startTransaction();
    CHECK(jtu->update(*js, MIDP_PACKAGE_TABLE, securityEntry, matchEntry));
    js->commitTransaction();
    securityEntry.clear();
    matchEntry.clear();

    // Update value to True
    attr.setEntry(ID, suite1Uid.toString());
    matchEntry.insert(attr);

    attr.setEntry(NAME, blockUninstall);
    matchEntry.insert(attr);
    attr.setEntry(VALUE, L"true");
    matchEntry.insert(attr);
    attr.setEntry(TRUSTED, L"0", JavaStorageEntry::INT);
    matchEntry.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suiteAttribute.insert(attr);

    attr.setEntry(NAME, blockUninstall);
    suiteAttribute.insert(attr);
    attr.setEntry(VALUE, L"True");
    suiteAttribute.insert(attr);
    attr.setEntry(TRUSTED, L"1", JavaStorageEntry::INT);
    suiteAttribute.insert(attr);

    js->startTransaction();
    CHECK(jtu->update(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, suiteAttribute, matchEntry));
    js->commitTransaction();
    suiteAttribute.clear();
    matchEntry.clear();

    // 5. Test attribute present, suite belongs to operator domain and
    //    attribute value is set True. EFalse returned.
    entry = (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->IsUninstallable() == EFalse);

    delete entry;
    entry = NULL;

    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));

    suiteAttribute.clear();
    attr.setEntry(ID, suite1Uid.toString());
    suiteAttribute.insert(attr);
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, suiteAttribute));
    CHECK(jtu->remove(*js, MIDP_PACKAGE_TABLE, suiteAttribute));

    js->commitTransaction();
    js->close();
    registry.reset(0);
    LOG(EJavaStorage, EInfo, "-TestIsUninstallable");
}

/**
 * Test CJavaRegistryPackageEntry::UsedUserDiskSpace() method.
 * 1. Test non existing path returns KErrAccessDenied.
 * 2. Simulate preinstalled application size.
 * 3. Test consumed disk space when application is installed.
 *   3.1 Install application
 *   3.2 Check size
 *   3.3 Uninstall application
 */
TEST(TestRegistryPackageEntry, TestUsedUserDiskSpace)
{
    LOG(EJavaStorage, EInfo, "+TestUsedUserDiskSpace");
    // 1. Test non existing path returns KErrAccessDenied.

    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e22a4d7d]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e22a45fa]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    js->open();
    js->startTransaction();

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    wstring vendor = L"SuiteVendor";
    attr.setEntry(VENDOR, vendor);
    suite1.insert(attr);

    attr.setEntry(VERSION, L"0.1");
    suite1.insert(attr);

    attr.setEntry(ROOT_PATH, L"c:\\NoExists\\");
    suite1.insert(attr);

    attr.setEntry(JAR_PATH, L"c:\\NoExists\\NotMe.jar");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    CJavaRegistryPackageEntry* entry =
        (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);

    TInt diskSpace = entry->UsedUserDiskSpace();
    CHECK(KErrAccessDenied == diskSpace);

    delete entry;
    entry = NULL;

    // 2. Simulate preinstalled application size.
    Uid suite2Uid(L"[e22bcd77]");

    JavaStorageApplicationEntry_t suite2;
    attr.setEntry(PACKAGE_NAME, L"MySuite2");
    suite2.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite2.insert(attr);

    attr.setEntry(VENDOR, L"Inc. inc.");
    suite2.insert(attr);

    attr.setEntry(INITIAL_SIZE, L"121");
    suite2.insert(attr);

    attr.setEntry(ROOT_PATH, L"c:\\NoExists\\");
    suite2.insert(attr);

    attr.setEntry(JAR_PATH, L"c:\\Different\\NotMe.jar");
    suite2.insert(attr);

    attr.setEntry(ID, suite2Uid.toString());
    suite2.insert(attr);

    js->open();
    js->startTransaction();

    JavaStorageApplicationEntry_t midlet2;

    attr.setEntry(PACKAGE_ID, suite2Uid.toString());
    midlet2.insert(attr);

    Uid midlet2Uid(L"[e22b4cd8]");

    attr.setEntry(ID, midlet2Uid.toString());
    midlet2.insert(attr);

    js->open();
    js->startTransaction();

    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite2));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    uidToTUid(suite2Uid, suiteUid);

    entry =
        (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);

    diskSpace = entry->UsedUserDiskSpace();
    CHECK(100 == diskSpace);

    delete entry;
    entry = NULL;

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet2));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite2));
    js->commitTransaction();
    js->close();


    //3. Test consumed disk space when application is installed.
    RProcess rJavaInstaller;
    TFileName fileName;
    TBuf<400> commandLine;

    // Build command line used to pass all necessary info to Java Installer
    std::auto_ptr<HBufC> installerStarterDll(
        stringToDes(java::runtime::JAVA_INSTALLER_STARTER_DLL));
    commandLine = installerStarterDll->Des();
    commandLine.Append(_L(" install -jar=c:\\HelloWorld1.jar"));
    commandLine.Append(_L(" -silent -upgrade=yes -ocsp=no"));
    commandLine.Append(_L(" -download=no -upgrade_data=yes"));

    // start JavaInstaller
    std::auto_ptr<HBufC> installerProcess(
        stringToDes(java::runtime::JAVA_PROCESS));
    TInt err = rJavaInstaller.Create(installerProcess->Des(), commandLine);

    TRequestStatus status;

    if (KErrNone == err)
    {
        LOG(EJavaStorage, EInfo, "Calling Logon");
        // This call will wait until Java Installer exits (or panics)
        rJavaInstaller.Logon(status);

        LOG(EJavaStorage, EInfo, "Calling Resume");
        rJavaInstaller.Resume();
    }
    else
    {
        ELOG1(EJavaStorage, "Cannot start Installer, error %d", err);
        User::Leave(err);
    }

    LOG(EJavaStorage, EInfo, "Calling RProcess::Close");
    // free resources before returning
    rJavaInstaller.Close();

    // now wait until Java Installer exits
    User::WaitForRequest(status);

    registry.reset(CJavaRegistry::NewL());
    RArray<TUid> uids;
    registry->GetRegistryEntryUidsL(EGeneralPackage, uids);
    CHECK(uids.Count() == 1); // One app installed

    entry = (CJavaRegistryPackageEntry*) registry->RegistryEntryL(uids[0]);
    CHECK(entry != NULL);

    diskSpace = entry->UsedUserDiskSpace();
    // Disk space can wary some bytes.
    CHECK(diskSpace > 4000);

    delete entry;
    entry = NULL;
    registry.reset(0);

    // UNINSTALL
    // Build command line used to pass all necessary info to Java Installer
    commandLine = installerStarterDll->Des();
    commandLine.Append(_L(" uninstall -uid="));
    TUidName uidName = (uids[0]).Name();
    commandLine.Append(uidName);
    uids.Reset();
    uids.Close();
    commandLine.Append(_L(" -silent"));

    // start JavaInstaller
    err = rJavaInstaller.Create(installerProcess->Des(), commandLine);

    status = KRequestPending;

    if (KErrNone == err)
    {
        LOG(EJavaStorage, EInfo, "Calling Logon");
        // This call will wait until Java Installer exits (or panics)
        rJavaInstaller.Logon(status);

        LOG(EJavaStorage, EInfo, "Calling Resume");
        rJavaInstaller.Resume();
    }
    else
    {
        ELOG1(EJavaStorage, "Cannot start Installer, error %d", err);
        User::Leave(err);
    }

    LOG(EJavaStorage, EInfo, "Calling RProcess::Close");
    // free resources before returning
    rJavaInstaller.Close();

    // now wait until Java Installer exits
    User::WaitForRequest(status);
    LOG(EJavaStorage, EInfo, "-TestUsedUserDiskSpace");
}

/**
 * Test CJavaRegistryPackageEntry::IsPreinstalled() method.
 * 1. Test DB not populated. Return EFalse.
 * 2. Test NO_PREINSTALL. Return EFalse.
 * 3. Test PREINSTALLED. Return ETrue.
 * 4. Test INSTALLED. Return EFalse.
 * 5. Test no match in PREINSTALL_TABLE. EFalse.
 */
TEST(TestRegistryPackageEntry, TestIsPreinstalled)
{
    LOG(EJavaStorage, EInfo, "+TestIsPreinstalled");

    // 1. Test DB not populated. Return EFalse.
    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e22a6877]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e22a6888]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"MySuite");
    suite1.insert(attr);

    wstring vendor = L"SuiteVendor";
    attr.setEntry(VENDOR, vendor);
    suite1.insert(attr);

    attr.setEntry(VERSION, L"0.1");
    suite1.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    attr.setEntry(PACKAGE_NAME, L"IsPreinstall");
    suite1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    // 2. Application suite uid.
    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    CJavaRegistryPackageEntry* entry =
        (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->IsPreinstalled() == false);
    delete entry;
    entry = NULL;

    // 2. Test NO_PREINSTALL. Return EFalse.
    JavaStorageApplicationEntry_t preinstall1;
    attr.setEntry(NAME, L"IsPreinstall");
    preinstall1.insert(attr);

    attr.setEntry(VENDOR, vendor);
    preinstall1.insert(attr);

    attr.setEntry(VERSION, L"1.1.1");
    preinstall1.insert(attr);

    attr.setEntry(INSTALL_STATE, L"2");
    preinstall1.insert(attr);

    CHECK(jtu->populate(*js, PREINSTALL_TABLE, preinstall1));

    entry = (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->IsPreinstalled() == false);
    delete entry;
    entry = NULL;

    // 3. Test PREINSTALLED. Return ETrue.
    JavaStorageApplicationEntry_t matchEntry;
    JavaStorageApplicationEntry_t updateEntry;
    attr.setEntry(VENDOR, vendor);
    matchEntry.insert(attr);

    attr.setEntry(INSTALL_STATE, L"1", JavaStorageEntry::INT);
    updateEntry.insert(attr);

    CHECK(jtu->update(*js, PREINSTALL_TABLE, updateEntry, matchEntry));
    updateEntry.clear();

    entry = (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->IsPreinstalled() == true);
    delete entry;
    entry = NULL;

    // 4. Test INSTALLED. Return EFalse.
    attr.setEntry(INSTALL_STATE, L"0", JavaStorageEntry::INT);
    updateEntry.insert(attr);

    CHECK(jtu->update(*js, PREINSTALL_TABLE, updateEntry, matchEntry));
    updateEntry.clear();

    entry = (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->IsPreinstalled() == false);
    delete entry;
    entry = NULL;

    // 5. Test no match in PREINSTALL_TABLE. EFalse.
    attr.setEntry(NAME, L"DoNotMatch");
    updateEntry.insert(attr);

    CHECK(jtu->update(*js, PREINSTALL_TABLE, updateEntry, matchEntry));
    updateEntry.clear();

    entry = (CJavaRegistryPackageEntry*) registry->RegistryEntryL(suiteUid);
    CHECK(entry != NULL);
    CHECK(entry->IsPreinstalled() == false);
    delete entry;
    entry = NULL;

    // Clean
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, matchEntry));
    js->close();
    LOG(EJavaStorage, EInfo, "-TestIsPreinstalled");
}
