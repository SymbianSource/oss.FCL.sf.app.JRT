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

/**
 * Test JavaRegistryApplicationEntry.
 */
TEST_GROUP(TestRegistryApplicationEntry)
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
 * Test CJavaRegistryApplicationEntry::IsStartable() method.
 * 1. Test return default value ETrue.
 */
TEST(TestRegistryApplicationEntry, TestIsStartable)
{
    LOG(EJavaStorage, EInfo, "+TestIsStartable");

    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e22a1111]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e22a1111]");

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

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    CJavaRegistryApplicationEntry* entry =
        (CJavaRegistryApplicationEntry*) registry->RegistryEntryL(midletUid);

    CHECK(entry != NULL);
    CHECK(entry->IsStartable() == true);

    delete entry;
    entry = NULL;

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();
    js->close();

    LOG(EJavaStorage, EInfo, "-TestIsStartable");
}

/**
 * Test CJavaRegistryApplicationEntry::IsVisible() method.
 * 1. Test return default value ETrue.
 */
TEST(TestRegistryApplicationEntry, TestIsVisible)
{
    LOG(EJavaStorage, EInfo, "+TestIsVisible");
    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e22a1111]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e22a1111]");

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
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    attr.setEntry(VERSION, L"0.1");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    CJavaRegistryApplicationEntry* entry =
        (CJavaRegistryApplicationEntry*) registry->RegistryEntryL(midletUid);

    CHECK(entry != NULL);
    CHECK(entry->IsVisible() == true);

    delete entry;
    entry = NULL;

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();
    js->close();
    LOG(EJavaStorage, EInfo, "-TestIsVisible");
}

/**
 * Test CJavaRegistryApplicationEntry::IsResident() method.
 * 1. Test return default value ETrue.
 */
TEST(TestRegistryApplicationEntry, TestIsResident)
{
    LOG(EJavaStorage, EInfo, "+TestIsResident");
    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e22a1111]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e22a1111]");

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

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    CJavaRegistryApplicationEntry* entry =
        (CJavaRegistryApplicationEntry*) registry->RegistryEntryL(midletUid);

    CHECK(entry != NULL);
    CHECK(entry->IsResident() == true);

    delete entry;
    entry = NULL;

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();
    js->close();
    LOG(EJavaStorage, EInfo, "-TestIsResident");
}

/**
 * Test CJavaRegistryApplicationEntry::GroupName() method.
 * 1. Test return default value KNullDesC.
 * 2. Test IsStartable return ETrue.
 * 3. Test IsVisible return ETrue.
 * 4. Test IsResident return ETrue.
 */
TEST(TestRegistryApplicationEntry, TestGroupName)
{
    LOG(EJavaStorage, EInfo, "+TestGroupName");
    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e22a1111]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e22a1111]");

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

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    CJavaRegistryApplicationEntry* entry =
        (CJavaRegistryApplicationEntry*) registry->RegistryEntryL(midletUid);

    CHECK(entry->IsStartable() == true);
    CHECK(entry->IsVisible() == true);
    CHECK(entry->IsResident() == true);

    CHECK(entry != NULL);
    CHECK(entry->GroupName() == KNullDesC);

    delete entry;
    entry = NULL;

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();
    js->close();
    LOG(EJavaStorage, EInfo, "-TestGroupName");
}

/**
 * Test CJavaRegistryApplicationEntry::PackageEntryL() method.
 * 1. Test return package entry.
 */
TEST(TestRegistryApplicationEntry, TestPackageEntryL)
{
    LOG(EJavaStorage, EInfo, "+TestPackageEntryL");
    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e22a2111]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e22a2211]");

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

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));

    // Session must be committed before next use of Registry otherwise
    // it is locked.
    js->commitTransaction();

    auto_ptr<CJavaRegistry> registry(CJavaRegistry::NewL());

    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    CJavaRegistryApplicationEntry* entry =
        (CJavaRegistryApplicationEntry*) registry->RegistryEntryL(midletUid);
    CHECK(entry != NULL);

    CJavaRegistryPackageEntry* packageEntry = entry->PackageEntryL();
    CHECK(packageEntry != NULL);

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    CHECK(packageEntry->Uid() == suiteUid);

    delete packageEntry;
    packageEntry = NULL;

    delete entry;
    entry = NULL;

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();
    js->close();
    registry.reset(0);
    LOG(EJavaStorage, EInfo, "-TestPackageEntryL");
}
