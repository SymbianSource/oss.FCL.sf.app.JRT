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

#include <CppUTest/TestHarness.h>

#include "javacommonutils.h"
#include "javaoslayer.h"
#include "javastorage.h"
#include "javastorageexception.h"
#include "javastoragenames.h"
#include "javasymbianoslayer.h"
#include "javauid.h"
#include "logger.h"
#include "minstalledappsregistry.h"
#include "mjavaregistry.h"
#include "storagetestutils.h"

using namespace std;
using namespace java::storage;
using namespace java::util;

/**
 * NOTE: Some of MJavaRegistrySuiteEntry interface methods are already tested
 * at testlegacyregistry.cpp test case.
 */
TEST_GROUP(TestSuiteEntry)
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
 * Test CJavaRegistrySuiteEntry::JadUrlL() method.
 * 1. Test no JAD URL defined. Only Jar present. Empty URL returned.
 * 2. Test one len Url.
 * 3. Test standard Url.
 */
TEST(TestSuiteEntry, TestJadUrlL)
{
    LOG(EJavaStorage, EInfo, "+TestJadUrlL");

    MJavaRegistry* registry = MJavaRegistry::CreateL();
    CHECK(registry != NULL);

    JavaStorageApplicationEntry_t suite1;
    JavaStorageEntry attr;
    attr.setEntry(PACKAGE_NAME, L"TestJadUrlL");
    suite1.insert(attr);

    attr.setEntry(JAR_URL, L"http://www.url.to.Jar.com");
    suite1.insert(attr);

    string tableName = APPLICATION_PACKAGE_TABLE;
    Uid suite1Uid(L"[e1234321]");
    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
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
    Uid midlet1Uid(L"[e12e1077]");
    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    CHECK(jtu->populate(*js, appTableName, midlet1));
    js->commitTransaction();

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    // 1. No JAD URL defined. Only Jar present. Empty URL returned.
    MJavaRegistrySuiteEntry* suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);
    CHECK(suiteEntry->JadUrlL().Size() == 0);  // Empty if not set.

    suiteEntry->Release();

    // 2. One len Url.
    JavaStorageApplicationEntry_t updateEntry;
    attr.setEntry(JAD_URL, L"1");
    updateEntry.insert(attr);
    js->startTransaction();
    CHECK(jtu->update(*js, tableName, updateEntry, suite1));
    js->commitTransaction();

    suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);
    _LIT(KOneLenURL, "1");
    CHECK(suiteEntry->JadUrlL().Compare(KOneLenURL) == 0);

    suiteEntry->Release();

    // 3. Standard Url.
    updateEntry.clear();

    attr.setEntry(JAD_URL, L"http://www.gonna.be/standard/URL/index.html");
    updateEntry.insert(attr);
    js->startTransaction();
    CHECK(jtu->update(*js, tableName, updateEntry, suite1));
    js->commitTransaction();

    suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);
    _LIT(KStandardURL, "http://www.gonna.be/standard/URL/index.html");
    CHECK(suiteEntry->JadUrlL().Compare(KStandardURL) == 0);

    suiteEntry->Release();
    updateEntry.clear();

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, tableName, suite1));
    CHECK(jtu->remove(*js, appTableName, midlet1));
    js->commitTransaction();
    js->close();

    registry->Release();

    LOG(EJavaStorage, EInfo, "-TestJadUrlL");
}

/**
 * Test CJavaRegistrySuiteEntry::JarUrlL() method.
 * 1. Test no JAR URL defined leaves KErrNotFound.
 * 2. Test one len JAR Url.
 * 3. Test standard JAR Url.
 */
TEST(TestSuiteEntry, TestJarUrlL)
{
    LOG(EJavaStorage, EInfo, "+TestJarUrlL");

    MJavaRegistry* registry = MJavaRegistry::CreateL();
    CHECK(registry != NULL);

    JavaStorageApplicationEntry_t suite1;
    JavaStorageEntry attr;
    attr.setEntry(PACKAGE_NAME, L"TestJarUrlL");
    suite1.insert(attr);

    attr.setEntry(JAD_URL, L"http://www.url.to.Jad.com");
    suite1.insert(attr);

    string tableName = APPLICATION_PACKAGE_TABLE;
    Uid suite1Uid(L"[e1234543]");
    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
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
    Uid midlet1Uid(L"[e12e0077]");
    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    CHECK(jtu->populate(*js, appTableName, midlet1));
    js->commitTransaction();

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    // 1. No JAD URL defined. Only Jar present. Empty URL returned.
    MJavaRegistrySuiteEntry* suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);
    TRAPD(err, suiteEntry->JarUrlL());
    CHECK(KErrNotFound == err);

    suiteEntry->Release();

    // 2. One len Url.
    JavaStorageApplicationEntry_t updateEntry;
    attr.setEntry(JAR_URL, L"1");
    updateEntry.insert(attr);
    js->startTransaction();
    CHECK(jtu->update(*js, tableName, updateEntry, suite1));
    js->commitTransaction();

    suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);
    _LIT(KOneLenURL, "1");
    CHECK(suiteEntry->JarUrlL().Compare(KOneLenURL) == 0);

    suiteEntry->Release();

    // 3. Standard Url.
    updateEntry.clear();

    attr.setEntry(JAR_URL, L"http://www.gonna.be/standard/URL/index.html");
    updateEntry.insert(attr);
    js->startTransaction();
    CHECK(jtu->update(*js, tableName, updateEntry, suite1));
    js->commitTransaction();

    suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);
    _LIT(KStandardURL, "http://www.gonna.be/standard/URL/index.html");
    CHECK(suiteEntry->JarUrlL().Compare(KStandardURL) == 0);

    suiteEntry->Release();
    updateEntry.clear();

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, tableName, suite1));
    CHECK(jtu->remove(*js, appTableName, midlet1));
    js->commitTransaction();
    js->close();

    registry->Release();

    LOG(EJavaStorage, EInfo, "-TestJarUrlL");
}

/**
 * Test CJavaRegistrySuiteEntry::MIDletUidsL() method.
 * 1. Test one embedded application.
 * 2. Test three embedded applications.
 */
TEST(TestSuiteEntry, TestMIDletUids)
{
    LOG(EJavaStorage, EInfo, "+TestMIDletUidsL");

    MJavaRegistry* registry = MJavaRegistry::CreateL();
    CHECK(registry != NULL);

    JavaStorageApplicationEntry_t suite1;
    JavaStorageEntry attr;
    attr.setEntry(PACKAGE_NAME, L"TestMIdletUids");
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

    JavaStorageApplicationEntry_t midlet1;
    attr.setEntry(NAME, L"MyMIDlet1");
    midlet1.insert(attr);

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    string appTableName = APPLICATION_TABLE;
    Uid midlet1Uid(L"[e12a0077]");
    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    CHECK(jtu->populate(*js, appTableName, midlet1));
    js->commitTransaction();

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    // 1. Test one embedded application.
    MJavaRegistrySuiteEntry* suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);
    RArray<TUid> embeddedUids;
    suiteEntry->MIDletUidsL(embeddedUids);
    CHECK(embeddedUids.Count() == 1);

    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);
    CHECK(embeddedUids[0] == midletUid);

    embeddedUids.Reset();
    suiteEntry->Release();

    JavaStorageApplicationEntry_t midlet2;
    attr.setEntry(NAME, L"MyMIDlet2");
    midlet2.insert(attr);

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet2.insert(attr);

    Uid midlet2Uid(L"[e12a0007]");
    attr.setEntry(ID, midlet2Uid.toString());
    midlet2.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, appTableName, midlet2));
    js->commitTransaction();

    JavaStorageApplicationEntry_t midlet3;
    attr.setEntry(NAME, L"MyMIDlet3");
    midlet3.insert(attr);

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet3.insert(attr);

    Uid midlet3Uid(L"[e12a0117]");
    attr.setEntry(ID, midlet3Uid.toString());
    midlet3.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, appTableName, midlet3));
    js->commitTransaction();

    // 2. Test three embedded applications.
    suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);
    suiteEntry->MIDletUidsL(embeddedUids);
    CHECK(embeddedUids.Count() == 3);

    uidToTUid(midlet1Uid, midletUid);
    CHECK(embeddedUids.Find(midletUid) >= 0);

    uidToTUid(midlet2Uid, midletUid);
    CHECK(embeddedUids.Find(midletUid) >= 0);

    uidToTUid(midlet3Uid, midletUid);
    CHECK(embeddedUids.Find(midletUid) >= 0);

    embeddedUids.Reset();
    suiteEntry->Release();
    embeddedUids.Close();

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, tableName, suite1));
    CHECK(jtu->remove(*js, appTableName, midlet1));
    CHECK(jtu->remove(*js, appTableName, midlet2));
    CHECK(jtu->remove(*js, appTableName, midlet3));
    js->commitTransaction();
    js->close();

    registry->Release();

    LOG(EJavaStorage, EInfo, "-TestMIDletUidsL");
}

/**
 * Test CJavaRegistrySuiteEntry::NumberOfMIDletsL() method.
 * 1. Test one embedded application.
 * 2. Test three embedded applications.
 */
TEST(TestSuiteEntry, TestNumberOfMIDletsL)
{
    LOG(EJavaStorage, EInfo, "+TestNumberOfMIDletsL");

    MJavaRegistry* registry = MJavaRegistry::CreateL();
    CHECK(registry != NULL);

    JavaStorageApplicationEntry_t suite1;
    JavaStorageEntry attr;
    attr.setEntry(PACKAGE_NAME, L"TestNumberOfMIDletsL");
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

    JavaStorageApplicationEntry_t midlet1;
    attr.setEntry(NAME, L"MyMIDlet1");
    midlet1.insert(attr);

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    string appTableName = APPLICATION_TABLE;
    Uid midlet1Uid(L"[e12a0077]");
    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    CHECK(jtu->populate(*js, appTableName, midlet1));
    js->commitTransaction();

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    // 1. Test one embedded application.
    MJavaRegistrySuiteEntry* suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);
    CHECK(suiteEntry->NumberOfMIDletsL() == 1);

    suiteEntry->Release();

    JavaStorageApplicationEntry_t midlet2;
    attr.setEntry(NAME, L"MyMIDlet2");
    midlet2.insert(attr);

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet2.insert(attr);

    Uid midlet2Uid(L"[e12a0007]");
    attr.setEntry(ID, midlet2Uid.toString());
    midlet2.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, appTableName, midlet2));
    js->commitTransaction();

    JavaStorageApplicationEntry_t midlet3;
    attr.setEntry(NAME, L"MyMIDlet3");
    midlet3.insert(attr);

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet3.insert(attr);

    Uid midlet3Uid(L"[e12a0117]");
    attr.setEntry(ID, midlet3Uid.toString());
    midlet3.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, appTableName, midlet3));
    js->commitTransaction();

    // 2. Test three embedded applications.
    suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);
    CHECK(suiteEntry->NumberOfMIDletsL() == 3);

    suiteEntry->Release();

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, tableName, suite1));
    CHECK(jtu->remove(*js, appTableName, midlet1));
    CHECK(jtu->remove(*js, appTableName, midlet2));
    CHECK(jtu->remove(*js, appTableName, midlet3));
    js->commitTransaction();
    js->close();

    registry->Release();

    LOG(EJavaStorage, EInfo, "-TestNumberOfMIDletsL");
}

/**
 * Test CJavaRegistrySuiteEntry::MIDletByUidL() method.
 * 1. Test TUid::Null() leaves KErrArgument.
 * 2. Test TUid is not embedded leaves KErrNotFound.
 * 3. Test TUid is not embedded leaves KErrNotFound when entry contains
 *    embedded application.
 * 4. Test one embedded application.
 * 5. Test two embedded applications.
 */
TEST(TestSuiteEntry, TestMIDletByUidL)
{
    LOG(EJavaStorage, EInfo, "+TestMIDletByUidL");

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

    JavaStorageApplicationEntry_t midlet1;
    attr.setEntry(NAME, L"MyMIDlet1");
    midlet1.insert(attr);

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    string appTableName = APPLICATION_TABLE;
    Uid midlet1Uid(L"[e12a0077]");
    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    CHECK(jtu->populate(*js, appTableName, midlet1));
    js->commitTransaction();

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    // 1. Test TUid::Null() leaves KErrArgument.
    MJavaRegistrySuiteEntry* suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);
    TRAPD(err, suiteEntry->MIDletByUidL(TUid::Null()));
    CHECK(KErrArgument == err);

    // 2. Test TUid is not embedded leaves KErrNotFound.
    TUid uid = TUid::Uid(3777185127);    // e1234567
    TRAP(err, suiteEntry->MIDletByUidL(uid));
    CHECK(KErrNotFound == err);

    suiteEntry->Release();

    // 3. Test TUid is not embedded leaves KErrNotFound.
    suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);
    TRAP(err, suiteEntry->MIDletByUidL(uid));
    CHECK(KErrNotFound == err);

    // 4. Test one embedded application.
    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);
    MJavaRegistryMIDletEntry* midletEntry
    = suiteEntry->MIDletByUidL(midletUid);
    CHECK(midletEntry != NULL);
    CHECK(midletEntry->UidL() == midletUid);

    midletEntry->Release();
    suiteEntry->Release();

    JavaStorageApplicationEntry_t midlet2;
    attr.setEntry(NAME, L"MyMIDlet2");
    midlet2.insert(attr);

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet2.insert(attr);

    Uid midlet2Uid(L"[e12a0007]");
    attr.setEntry(ID, midlet2Uid.toString());
    midlet2.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, appTableName, midlet2));
    js->commitTransaction();

    // 5. Test two embedded applications.
    suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);
    uidToTUid(midlet1Uid, midletUid);
    midletEntry = suiteEntry->MIDletByUidL(midletUid);
    CHECK(midletEntry != NULL);
    CHECK(midletEntry->UidL() == midletUid);

    midletEntry->Release();

    uidToTUid(midlet2Uid, midletUid);
    midletEntry = suiteEntry->MIDletByUidL(midletUid);
    CHECK(midletEntry != NULL);
    CHECK(midletEntry->UidL() == midletUid);

    midletEntry->Release();
    suiteEntry->Release();

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, tableName, suite1));
    CHECK(jtu->remove(*js, appTableName, midlet1));
    CHECK(jtu->remove(*js, appTableName, midlet2));
    js->commitTransaction();
    js->close();

    registry->Release();

    LOG(EJavaStorage, EInfo, "-TestMIDletByUidL");
}

/**
 * Test CJavaRegistrySuiteEntry::MIDletByNumberL() method.
 * 1. Test 0 leaves KErrArgument.
 * 2. Test 2 leaves KErrNotFound after population.
 * 3. Test one embedded application.
 * 4. Test two embedded applications.
 */
TEST(TestSuiteEntry, TestMIDletByNumberL)
{
    LOG(EJavaStorage, EInfo, "+TestMIDletByNumberL");

    MJavaRegistry* registry = MJavaRegistry::CreateL();
    CHECK(registry != NULL);

    JavaStorageApplicationEntry_t suite1;
    JavaStorageEntry attr;
    attr.setEntry(PACKAGE_NAME, L"TestMIDletByNumberL");
    suite1.insert(attr);

    string tableName = APPLICATION_PACKAGE_TABLE;
    Uid suite1Uid(L"[e1230022]");
    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, tableName, suite1));
    js->commitTransaction();

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    JavaStorageApplicationEntry_t midlet1;
    attr.setEntry(NAME, L"MyMIDlet1");
    midlet1.insert(attr);

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    string appTableName = APPLICATION_TABLE;
    Uid midlet1Uid(L"[e12a0021]");
    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, appTableName, midlet1));
    js->commitTransaction();

    // 1. Test 0 leaves KErrArgument.
    MJavaRegistrySuiteEntry* suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);
    TRAPD(err, suiteEntry->MIDletByNumberL(0));
    CHECK(KErrArgument == err);

    suiteEntry->Release();

    // 2. Test 2 leaves KErrNotFound after population.
    suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);
    TRAP(err, suiteEntry->MIDletByNumberL(2));
    CHECK(KErrNotFound == err);

    // 3. Test one embedded application.
    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);
    MJavaRegistryMIDletEntry* midletEntry
    = suiteEntry->MIDletByNumberL(1);
    CHECK(midletEntry != NULL);
    CHECK(midletEntry->UidL() == midletUid);

    midletEntry->Release();
    suiteEntry->Release();

    JavaStorageApplicationEntry_t midlet2;
    attr.setEntry(NAME, L"MyMIDlet2");
    midlet2.insert(attr);

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet2.insert(attr);

    Uid midlet2Uid(L"[e12a0012]");
    attr.setEntry(ID, midlet2Uid.toString());
    midlet2.insert(attr);

    js->startTransaction();
    CHECK(jtu->populate(*js, appTableName, midlet2));
    js->commitTransaction();

    // 4. Test two embedded applications.
    suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);

    // Order is not quaranteed!
    uidToTUid(midlet1Uid, midletUid);
    midletEntry = suiteEntry->MIDletByNumberL(1);
    CHECK(midletEntry != NULL);
    CHECK(midletEntry->UidL() == midletUid);

    midletEntry->Release();

    uidToTUid(midlet2Uid, midletUid);
    midletEntry = suiteEntry->MIDletByNumberL(2);
    CHECK(midletEntry != NULL);
    CHECK(midletEntry->UidL() == midletUid);

    midletEntry->Release();
    suiteEntry->Release();

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, tableName, suite1));
    CHECK(jtu->remove(*js, appTableName, midlet1));
    CHECK(jtu->remove(*js, appTableName, midlet2));
    js->commitTransaction();
    js->close();

    registry->Release();

    LOG(EJavaStorage, EInfo, "-TestMIDletByNumberL");
}

/**
 * Test CJavaRegistrySuiteEntry::InstalledAppsEntryL() method.
 * 1. Test InstalledAppsEntry is valid object.
 */
TEST(TestSuiteEntry, TestInstalledAppsEntryL)
{
    LOG(EJavaStorage, EInfo, "+TestInstalledAppsEntryL");

    MJavaRegistry* registry = MJavaRegistry::CreateL();
    CHECK(registry != NULL);
    CleanupReleasePushL(*registry);

    JavaStorageApplicationEntry_t suite1;
    JavaStorageEntry attr;
    attr.setEntry(PACKAGE_NAME, L"TestMIDletByNumberL");
    suite1.insert(attr);

    string tableName = APPLICATION_PACKAGE_TABLE;
    Uid suite1Uid(L"[e123abcd]");
    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
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
    Uid midlet1Uid(L"[ea0000a1]");
    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    CHECK(jtu->populate(*js, appTableName, midlet1));
    js->commitTransaction();

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    // 1. Test 0 leaves KErrArgument.
    MJavaRegistrySuiteEntry* suiteEntry = registry->SuiteEntryL(suiteUid);
    CHECK(suiteEntry != NULL);
    CleanupReleasePushL(*suiteEntry);

    const MInstalledAppsRegistryEntry& appsRegEntry =
        suiteEntry->InstalledAppsEntryL();
    CHECK(appsRegEntry.UidL() == suiteUid);

    CleanupStack::PopAndDestroy(suiteEntry);

    // Clean
    js->startTransaction();
    CHECK(jtu->remove(*js, tableName, suite1));
    CHECK(jtu->remove(*js, appTableName, midlet1));
    js->commitTransaction();
    js->close();

    CleanupStack::PopAndDestroy(registry);
    LOG(EJavaStorage, EInfo, "-TestInstalledAppsEntryL");
}
