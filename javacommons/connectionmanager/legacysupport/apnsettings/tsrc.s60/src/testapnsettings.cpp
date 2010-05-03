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

#include <memory>

#include "TestHarness.h"

#include "../../inc.s60/mapnsettings.h"

#include "javastorage.h"
#include "javastorageexception.h"
#include "javastoragenames.h"
#include "javauid.h"
#include "javasymbianoslayer.h"
#include "logger.h"

using namespace CENREP;
using namespace java::storage;

TEST_GROUP(TestAPNSettings)
{
    MAPNSettings* iApnSettings;

    TEST_SETUP()
    {
        iApnSettings = GetAPNSettingsLC(KAPNSettingsVersion);
        CleanupStack::Pop();
    }
    TEST_TEARDOWN()
    {
        delete iApnSettings;
        iApnSettings = NULL;
    }
};

/**
 * Test Set and Get APN
 *
 * 1. Test set with non-existing UID.
 * 2. Test set with non-existing APN ID.
 * 3. Test set with non-existing UID and APN ID.
 * 4. Test set with existing UID and APN ID.
 */
TEST(TestAPNSettings, TestGetSetAPN)
{
    java::util::Uid javaUid(L"[ec0c0c0c]");
    TUid appUid;
    uidToTUid(javaUid, appUid);

    JavaStorageApplicationEntry_t packageEntry;
    JavaStorageEntry attr;

    attr.setEntry(PACKAGE_NAME, L"TestiApnSettings");
    packageEntry.insert(attr);

    attr.setEntry(ID, javaUid.toString());
    packageEntry.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
    packageEntry.insert(attr);

    std::auto_ptr<JavaStorage> js(JavaStorage::createInstance());

    try
    {
        js->open();
        js->startTransaction();
        js->write(APPLICATION_PACKAGE_TABLE, packageEntry);
        js->commitTransaction();
    }
    catch (JavaStorageException& aJse)
    {
        std::string errorStr = "Failed to write: ";
        errorStr.append(aJse.toString());
        FAIL(errorStr.c_str());
    }

    // 1. Test set with non-existing UID.
    iApnSettings->SetAPNL(0, 1);
    TUint32 apn = iApnSettings->GetAPNIdL(0);
    CHECK(apn == KMIDletSuiteAPNNotSpecified);


    // 2. Test set with non-existing APN ID.
    iApnSettings->SetAPNL(appUid.iUid, 10101);
    apn = iApnSettings->GetAPNIdL(appUid.iUid);
    CHECK(apn == 10101);

    // 3. Test set with non-existing UID and APN ID.
    iApnSettings->SetAPNL(12345, 10101);
    apn = iApnSettings->GetAPNIdL(12345);
    CHECK(apn == KMIDletSuiteAPNNotSpecified);

    // 4. Test set with existing UID and APN ID.
    iApnSettings->SetAPNL(appUid.iUid, 1);
    apn = iApnSettings->GetAPNIdL(appUid.iUid);
    CHECK(apn == 1);

    try
    {
        js->remove(APPLICATION_PACKAGE_TABLE, packageEntry);
        js->close();
    }
    catch (JavaStorageException& aJse)
    {
        std::string errorStr = "Failed to remove: ";
        errorStr.append(aJse.toString());
        FAIL(errorStr.c_str());
    }
}

/**
 * Test Remove APN.
 *
 * 1. Test remove not set.
 * 2. Test remove.
 * 3. Test remove non-existing.
 * 4. Test get default Java APN.
 */
TEST(TestAPNSettings, TestRemoveAPN)
{
    java::util::Uid javaUid(L"[ed0d0d0d]");
    TUid appUid;
    uidToTUid(javaUid, appUid);

    JavaStorageApplicationEntry_t packageEntry;
    JavaStorageEntry attr;

    attr.setEntry(PACKAGE_NAME, L"TestiApnSettings");
    packageEntry.insert(attr);

    attr.setEntry(ID, javaUid.toString());
    packageEntry.insert(attr);

    attr.setEntry(MEDIA_ID, L"-124614446");
    packageEntry.insert(attr);

    std::auto_ptr<JavaStorage> js(JavaStorage::createInstance());

    try
    {
        js->open();
        js->startTransaction();
        js->write(APPLICATION_PACKAGE_TABLE, packageEntry);
        js->commitTransaction();
    }
    catch (JavaStorageException& aJse)
    {
        std::string errorStr = "Failed to write: ";
        errorStr.append(aJse.toString());
        FAIL(errorStr.c_str());
    }

    // 1. Test set with non-existing UID.
    iApnSettings->RemoveL(appUid.iUid);
    TUint32 apn = iApnSettings->GetAPNIdL(appUid.iUid);
    CHECK(apn == KErrNotFound);

    // 2. Test remove.
    iApnSettings->SetAPNL(appUid.iUid, 1);
    apn = iApnSettings->GetAPNIdL(appUid.iUid);
    CHECK(apn == 1);

    iApnSettings->RemoveL(appUid.iUid);
    apn = iApnSettings->GetAPNIdL(appUid.iUid);
    CHECK(apn == KErrNotFound);

    // 3. Test remove non-existing.
    iApnSettings->RemoveL(1);

    // 4. Test get default Java APN.
    CHECK(iApnSettings->GetJavaDefaultAPNL() == KMIDletSuiteAPNNotSpecified);

    try
    {
        js->remove(APPLICATION_PACKAGE_TABLE, packageEntry);
        js->close();
    }
    catch (JavaStorageException& aJse)
    {
        std::string errorStr = "Failed to remove: ";
        errorStr.append(aJse.toString());
        FAIL(errorStr.c_str());
    }
}

