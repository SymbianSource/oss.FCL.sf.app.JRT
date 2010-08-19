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

#include "javastorage.h"
#include "javastorageexception.h"
#include "javastoragenames.h"
#include "javauid.h"
#include "javaprocessconstants.h"
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

const JavaStorageApplicationEntry_t createEntry(const wstring& aUid,
        const wstring& aName,
        const wstring& aValue);

const JavaStorageApplicationEntry_t createValidCertsEntry(
    const wstring& aUid,
    const wstring& aCerts);

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
 * 1. Test install MIDlet suite and check suite and midlet size.
 */
TEST(TestInstalledAppsRegistryEntry, TestSizeL)
{
    LOG(EJavaStorage, EInfo, "+TestSizeL");

    MInstalledAppsRegistry* appsRegistry = MInstalledAppsRegistry::CreateL();
    CleanupReleasePushL(*appsRegistry);

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

    // now wait until Java Installer exits
    User::WaitForRequest(status);

    // free resources before returning
    rJavaInstaller.Close();

    RArray<TUid> uids;
    appsRegistry->InstalledUidsL(uids);
    CHECK(uids.Count() == 2); // One app and suite installed

    // Size is fetched through Suite and application.
    MInstalledAppsRegistryEntry* entry = appsRegistry->EntryL(uids[0]);
    CleanupReleasePushL(*entry);

    CHECK(entry->SizeL() > 4000);

    CleanupStack::PopAndDestroy(entry);

    entry = appsRegistry->EntryL(uids[1]);
    CleanupReleasePushL(*entry);

    CHECK(entry->SizeL() > 4000);
    CleanupStack::PopAndDestroy(entry);

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

    // now wait until Java Installer exits
    User::WaitForRequest(status);

    LOG(EJavaStorage, EInfo, "Calling RProcess::Close");
    // free resources before returning
    rJavaInstaller.Close();

    CleanupStack::PopAndDestroy(appsRegistry);
    LOG(EJavaStorage, EInfo, "-TestSizeL");
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

    MInstalledAppsRegistryEntry* entry = appsRegistry->EntryL(midletUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);

    CHECK(entry->UidL() == midletUid);
    CleanupStack::PopAndDestroy(entry);

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

    entry = appsRegistry->EntryL(midletUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);

    appVersion = entry->VersionL();
    CHECK(appVersion.iMajor == refVersion.iMajor);
    CHECK(appVersion.iMinor == refVersion.iMinor);
    CHECK(appVersion.iBuild == refVersion.iBuild);

    CleanupStack::PopAndDestroy(entry);

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
    entry = appsRegistry->EntryL(midletUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);

    appVersion = entry->VersionL();
    CHECK(appVersion.iMajor == refVersion2.iMajor);
    CHECK(appVersion.iMinor == refVersion2.iMinor);
    CHECK(appVersion.iBuild == refVersion2.iBuild);

    CleanupStack::PopAndDestroy(entry);

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

    attr.setEntry(ROOT_PATH, L"e:\\private\\MIDlets\\[eaaa4577]");
    suite1.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();

    // 1. Application uid.
    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    MInstalledAppsRegistryEntry* entry = appsRegistry->EntryL(midletUid);
    CHECK(entry != NULL);

    CleanupReleasePushL(*entry);

    CHECK(entry->DriveL() == 'E');

    CleanupStack::PopAndDestroy(entry);

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

    attr.setEntry(JAR_PATH, L"e:\\private\\MIDlets\\[e0004577]");
    suite1.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();

    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    MInstalledAppsRegistryEntry* entry = appsRegistry->EntryL(midletUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);

    // 2. Test application not in ROM.
    CHECK(entry->IsInRomL() == EFalse);

    CleanupStack::PopAndDestroy(entry);

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

    js->startTransaction();
    CHECK(jtu->update(*js, APPLICATION_PACKAGE_TABLE, updateEntry, suite1));
    js->commitTransaction();

    entry = appsRegistry->EntryL(midletUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);

    // 4. Test application in ROM.
    CHECK(entry->IsInRomL() == true);

    CleanupStack::PopAndDestroy(entry);

    entry = appsRegistry->EntryL(suiteUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);

    // 3. Test application suite in ROM.
    CHECK(entry->IsInRomL() == true);

    CleanupStack::PopAndDestroy(entry);

    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, updateEntry));
    js->commitTransaction();
    js->close();

    CleanupStack::PopAndDestroy(appsRegistry);
    LOG(EJavaStorage, EInfo, "-TestIsInRomL");
}

/**
 * Test CInstalledAppsRegistryEntry::NumberOfCertificateChainsL() method.
 * 1. Test when suite no cert chains. Size is 0.
 * 2. Test when suite one cert chains. Size is 1.
 * 3. Test when suite three cert chains. size is 3.
 */
TEST(TestInstalledAppsRegistryEntry, TestNumberOfCertificateChainsL)
{
    LOG(EJavaStorage, EInfo, "+TestNumberOfCertificateChainsL");

    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e22bf577]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e22bf588]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"TestCertChains");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();
    js->close();

    MInstalledAppsRegistry* appsRegistry = MInstalledAppsRegistry::CreateL();
    CleanupReleasePushL(*appsRegistry);

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    // 1. Test when no cert chains. Size is 0.
    MInstalledAppsRegistryEntry* entry = appsRegistry->EntryL(suiteUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);

    CHECK(entry->NumberOfCertificateChainsL() == 0);

    CleanupStack::PopAndDestroy(entry);

    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    // 1.2 Test no certs via app.
    entry = appsRegistry->EntryL(midletUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);
    CHECK(entry->NumberOfCertificateChainsL() == 0);

    CleanupStack::PopAndDestroy(entry);

    // 2. Test when one cert chains. Size is 1.

    JavaStorageApplicationEntry_t certEntry;
    JavaStorageApplicationEntry_t validcertsEntry;

    js->open();
    validcertsEntry = createValidCertsEntry(suite1Uid.toString(), L"1");
    js->write(MIDP_PACKAGE_TABLE, validcertsEntry);

    certEntry = createEntry(suite1Uid.toString(),
                            L"MIDlet-Certificate-1-1",
                            L"abc123");

    js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);
    js->close();

    entry = appsRegistry->EntryL(suiteUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);
    CHECK(entry->NumberOfCertificateChainsL() == 1);
    CleanupStack::PopAndDestroy(entry);

    // 2.2 Test no certs via app.
    entry = appsRegistry->EntryL(midletUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);
    CHECK(entry->NumberOfCertificateChainsL() == 1);
    CleanupStack::PopAndDestroy(entry);

    js->open();
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry));
    CHECK(jtu->remove(*js, MIDP_PACKAGE_TABLE, validcertsEntry));
    js->commitTransaction();
    js->close();

    // 3. Test when three cert chains. size is 3.
    certEntry.clear();
    validcertsEntry.clear();

    validcertsEntry = createValidCertsEntry(suite1Uid.toString(), L"1,2,3");

    js->open();
    js->write(MIDP_PACKAGE_TABLE, validcertsEntry);

    certEntry = createEntry(suite1Uid.toString(),
                            L"MIDlet-Certificate-1-1",
                            L"abc123");

    js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);

    JavaStorageApplicationEntry_t certEntry2;
    certEntry2 = createEntry(suite1Uid.toString(),
                             L"MIDlet-Certificate-2-1",
                             L"def456");

    js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);

    JavaStorageApplicationEntry_t certEntry3;
    certEntry3 = createEntry(suite1Uid.toString(),
                             L"MIDlet-Certificate-3-1",
                             L"fooBar");

    js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry3);
    js->close();

    entry = appsRegistry->EntryL(suiteUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);
    CHECK(entry->NumberOfCertificateChainsL() == 3);
    CleanupStack::PopAndDestroy(entry);

    // 2.2 Test no certs via app.
    entry = appsRegistry->EntryL(midletUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);
    CHECK(entry->NumberOfCertificateChainsL() == 3);
    CleanupStack::PopAndDestroy(entry);

    js->open();
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry3));
    CHECK(jtu->remove(*js, MIDP_PACKAGE_TABLE, validcertsEntry));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();
    js->close();

    CleanupStack::PopAndDestroy(appsRegistry);

    LOG(EJavaStorage, EInfo, "-TestNumberOfCertificateChains");
}

/**
 * Test CInstalledAppsRegistryEntry::CertificateChainL() method.
 * 1. Test when suite no cert chains.
 * 2. Test when suite one cert chains.
 * 3. Test when suite three cert chains.
 * 4. Test index greater than.
 * 5. Test index less than i.e. -1.
 */
TEST(TestInstalledAppsRegistryEntry, TestCertificateChainL)
{
    LOG(EJavaStorage, EInfo, "+TestCertificateChainL");
    MInstalledAppsRegistry* appsRegistry = MInstalledAppsRegistry::CreateL();
    CleanupReleasePushL(*appsRegistry);

    JavaStorageApplicationEntry_t midlet1;
    JavaStorageEntry attr;

    Uid suite1Uid(L"[e22bf577]");

    attr.setEntry(PACKAGE_ID, suite1Uid.toString());
    midlet1.insert(attr);

    Uid midlet1Uid(L"[e22bf588]");

    attr.setEntry(ID, midlet1Uid.toString());
    midlet1.insert(attr);

    js->open();
    js->startTransaction();
    CHECK(jtu->populate(*js, APPLICATION_TABLE, midlet1));

    JavaStorageApplicationEntry_t suite1;
    attr.setEntry(PACKAGE_NAME, L"TestCertChains");
    suite1.insert(attr);

    attr.setEntry(ID, suite1Uid.toString());
    suite1.insert(attr);
    attr.setEntry(MEDIA_ID, L"-124614446");
    suite1.insert(attr);

    CHECK(jtu->populate(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();
    js->close();

    TUid suiteUid;
    uidToTUid(suite1Uid, suiteUid);

    // 1. Test when no cert chains. Size is 0.
    MInstalledAppsRegistryEntry* entry = appsRegistry->EntryL(suiteUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);
    CHECK(entry->NumberOfCertificateChainsL() == 0);
    CHECK(entry->CertificateChainL(0) == KNullDesC8);
    CleanupStack::PopAndDestroy(entry);

    TUid midletUid;
    uidToTUid(midlet1Uid, midletUid);

    // 1.2 Test no certs via app.
    entry = appsRegistry->EntryL(midletUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);
    CHECK(entry->NumberOfCertificateChainsL() == 0);
    CHECK(entry->CertificateChainL(0) == KNullDesC8);
    CleanupStack::PopAndDestroy(entry);

    // 2. Test when one cert chains. Size is 1.

    JavaStorageApplicationEntry_t certEntry;
    JavaStorageApplicationEntry_t validcertsEntry;

    js->open();
    validcertsEntry = createValidCertsEntry(suite1Uid.toString(), L"1");
    js->write(MIDP_PACKAGE_TABLE, validcertsEntry);

    certEntry = createEntry(suite1Uid.toString(),
                            L"MIDlet-Certificate-1-1",
                            L"YWJjMTIz"); // abc123 as base64 encoded.

    js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);
    js->close();

    _LIT8(KCertChain1, "abc123");

    entry = appsRegistry->EntryL(suiteUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);
    CHECK(entry->NumberOfCertificateChainsL() == 1);
    CHECK(entry->CertificateChainL(0).Compare(KCertChain1) == 0);
    CleanupStack::PopAndDestroy(entry);

    // 2.2 Test no certs via app.
    entry = appsRegistry->EntryL(midletUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);
    CHECK(entry->NumberOfCertificateChainsL() == 1);
    CHECK(entry->CertificateChainL(0).Compare(KCertChain1) == 0);
    CleanupStack::PopAndDestroy(entry);

    js->open();
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry));
    CHECK(jtu->remove(*js, MIDP_PACKAGE_TABLE, validcertsEntry));
    js->commitTransaction();
    js->close();


    // 3. Test when three cert chains. size is 3.
    certEntry.clear();
    validcertsEntry.clear();

    validcertsEntry = createValidCertsEntry(suite1Uid.toString(), L"1,2,3");

    js->open();
    js->write(MIDP_PACKAGE_TABLE, validcertsEntry);

    certEntry = createEntry(suite1Uid.toString(),
                            L"MIDlet-Certificate-1-1",
                            L"YWJjMTIz");  // abc123

    js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry);

    JavaStorageApplicationEntry_t certEntry2;
    certEntry2 = createEntry(suite1Uid.toString(),
                             L"MIDlet-Certificate-2-1",
                             L"ZGVmNDU2");  // def456 as base 64 encoded.

    js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2);

    JavaStorageApplicationEntry_t certEntry3;
    certEntry3 = createEntry(suite1Uid.toString(),
                             L"MIDlet-Certificate-3-1",
                             L"Zm9vQmFy");  // fooBar

    js->write(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry3);
    js->close();

    entry = appsRegistry->EntryL(suiteUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);
    CHECK(entry->NumberOfCertificateChainsL() == 3);
    CHECK(entry->CertificateChainL(0).Compare(KCertChain1) == 0);
    _LIT8(KCertChain2, "def456");
    CHECK(entry->CertificateChainL(1).Compare(KCertChain2) == 0);
    _LIT8(KCertChain3, "fooBar");
    CHECK(entry->CertificateChainL(2).Compare(KCertChain3) == 0);
    CleanupStack::PopAndDestroy(entry);

    // 2.2 Test no certs via app.
    entry = appsRegistry->EntryL(midletUid);
    CHECK(entry != NULL);
    CleanupReleasePushL(*entry);
    CHECK(entry->NumberOfCertificateChainsL() == 3);
    CHECK(entry->CertificateChainL(0).Compare(KCertChain1) == 0);
    CHECK(entry->CertificateChainL(1).Compare(KCertChain2) == 0);
    CHECK(entry->CertificateChainL(2).Compare(KCertChain3) == 0);

    // 4. Test index greater than.
    CHECK(entry->CertificateChainL(3) == KNullDesC8);

    // 5. Test index less than i.e. -1.
    CHECK(entry->CertificateChainL(-1) == KNullDesC8);

    CleanupStack::PopAndDestroy(entry);

    js->open();
    js->startTransaction();
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry2));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_ATTRIBUTES_TABLE, certEntry3));
    CHECK(jtu->remove(*js, MIDP_PACKAGE_TABLE, validcertsEntry));
    CHECK(jtu->remove(*js, APPLICATION_TABLE, midlet1));
    CHECK(jtu->remove(*js, APPLICATION_PACKAGE_TABLE, suite1));
    js->commitTransaction();
    js->close();

    CleanupStack::PopAndDestroy(appsRegistry);

    LOG(EJavaStorage, EInfo, "-TestCertificateChainL");
}

const JavaStorageApplicationEntry_t createEntry(const wstring& aUid,
        const wstring& aName,
        const wstring& aValue)
{
    JavaStorageApplicationEntry_t entry;
    JavaStorageEntry attr;

    attr.setEntry(ID, aUid);
    entry.insert(attr);

    attr.setEntry(NAME, aName);
    entry.insert(attr);

    attr.setEntry(VALUE, aValue);
    entry.insert(attr);

    return entry;
}

const JavaStorageApplicationEntry_t createValidCertsEntry(const wstring& aUid,
        const wstring& aCerts)
{
    JavaStorageApplicationEntry_t entry;
    JavaStorageEntry attr;

    attr.setEntry(ID, aUid);
    entry.insert(attr);

    attr.setEntry(VALID_CERTS, aCerts);
    entry.insert(attr);

    return entry;
}
