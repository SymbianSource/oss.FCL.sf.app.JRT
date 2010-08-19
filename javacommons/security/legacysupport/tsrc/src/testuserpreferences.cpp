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

#include "teststoragehandler.h"
#include "javastoragenames.h"
#include "javauid.h"
#include "javasymbianoslayer.h"

#include "j2me/midp2/data/muserpreferences.h"
#include "j2me/midp2/data/mmidletsuiteuserpreferencesiterator.h"
#include "j2me/midp2/data/mmidletsuiteuserpreferences.h"
#include "j2me/midp2/data/mmidletsuitecustomattributes.h"
#include "j2me/midp2/security/msecuritypolicyv2.h"

using namespace MIDP;
using namespace java::storage;
using namespace java::util;

_LIT(KMidletSuiteName, "MIDletSuite.Name");
_LIT(KMidletSuiteVendor, "MIDletSuite.Vendor");
_LIT(KMidletSuiteVersion, "MIDletSuite.Version");
_LIT(KAppName1, "appName1");
_LIT(KAppVendor1, "appVendor1");
_LIT(KAppVersion1, "1.1.0");
_LIT(KAppName2, "appName2");
_LIT(KAppVendor2, "appVendor2");
_LIT(KAppVersion2, "2.0.1");

IMPORT_C MSecurityPolicyV2* GetSecurityPolicyL(TUint32 aFlags, const TDesC& aPolicyName);
IMPORT_C MUserPreferences* GetUserPreferencesL(TUint32 aFlags, const MSecurityPolicyV2& aSecurityPolicy);


TEST_GROUP(TestUserPreferences)
{
    MSecurityPolicyV2* iSecurityPolicy;
    MUserPreferences* iUserPreferences;
    MMIDletSuiteUserPreferencesIterator* iIterator;
    TestStorageHandler* iStorageHandler;
    Uid iAppUid1;
    Uid iAppUid2;
    TUint32 iAppId1;
    TUint32 iAppId2;
    MMIDletSuiteUserPreferences* iSuitePref1;
    MMIDletSuiteUserPreferences* iSuitePref2;
    MMIDletSuiteCustomAttributes* iCustomAttr;

    TEST_SETUP()
    {
        iAppUid1 = Uid(L"12345678");
        iAppUid2 = Uid(L"87654321");
        TUid uid1;
        uidToTUid(iAppUid1, uid1);
        iAppId1 = uid1.iUid;
        TUid uid2;
        uidToTUid(iAppUid2, uid2);
        iAppId2 = uid2.iUid;
        TUidToUid(uid1, iAppUid1);
        TUidToUid(uid2, iAppUid2);
        iSecurityPolicy = GetSecurityPolicyL(0 /* security policy flags */, KNullDesC());
        iUserPreferences = GetUserPreferencesL(0 /* user preferences flags */, *iSecurityPolicy);
        iStorageHandler = new TestStorageHandler();
    }

    TEST_TEARDOWN()
    {
        if (iIterator)
        {
            delete iIterator;
            iIterator = NULL;
        }
        delete iSecurityPolicy;
        iSecurityPolicy = NULL;
        if (iUserPreferences)
        {
            delete iUserPreferences;
            iUserPreferences = NULL;
        }
        if (iStorageHandler)
        {
            iStorageHandler->cleanStorage(APPLICATION_PACKAGE_TABLE, iAppUid1);
            iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid1);
            iStorageHandler->cleanStorage(APPLICATION_PACKAGE_TABLE, iAppUid2);
            iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid2);
            delete iStorageHandler;
            iStorageHandler = NULL;
        }
        if (iSuitePref1)
        {
            delete iSuitePref1;
            iSuitePref1 = NULL;
        }
        if (iSuitePref2)
        {
            delete iSuitePref2;
            iSuitePref2 = NULL;
        }
        if (iCustomAttr)
        {
            delete iCustomAttr;
            iCustomAttr = NULL;
        }
    }
};

TEST(TestUserPreferences, TestUserPreferencesL)
{
    // no "known" suites
    CHECK(iUserPreferences != NULL);
    iIterator = iUserPreferences->IteratorL();
    CHECK(iIterator != NULL);
    while (iIterator->HasNext())
    {
        MMIDletSuiteUserPreferences* suitePref = iIterator->NextL();
        CHECK(suitePref != NULL);
        if (suitePref->Id() == iAppId1 || suitePref->Id() == iAppId2)
        {
            delete suitePref;
            suitePref = NULL;
            CHECK(false);
        }
        delete suitePref;
        suitePref = NULL;
    }
    // call NextL when the Iterator has reached the end
    int error;
    TRAP(error, iIterator->NextL());
    CHECK(error == KErrNotFound);
    // two "known" midlet suites
    delete iIterator;
    iIterator = NULL;
    iStorageHandler->cleanStorage(APPLICATION_PACKAGE_TABLE, iAppUid1);
    iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid1);
    iStorageHandler->populateApplicationPackageTable(iAppUid1, L"appName1", L"appVendor1", L"1.1.0");
    iStorageHandler->populateMIDPPackageTable(iAppUid1, L"UTPD");
    iStorageHandler->cleanStorage(APPLICATION_PACKAGE_TABLE, iAppUid2);
    iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid2);
    iStorageHandler->populateApplicationPackageTable(iAppUid2, L"appName2", L"appVendor2", L"2.0.1");
    iStorageHandler->populateMIDPPackageTable(iAppUid2, L"OPD");
    iIterator = iUserPreferences->IteratorL();
    CHECK(iIterator != NULL);
    bool app1Found = false;
    bool app2Found = false;
    while (iIterator->HasNext())
    {
        MMIDletSuiteUserPreferences* suitePref = iIterator->NextL();
        CHECK(suitePref != NULL);
        TUint32 id = suitePref->Id();
        if (id == iAppId1)
        {
            app1Found = true;
            iSuitePref1 = suitePref;
            continue;
        }
        else if (id == iAppId2)
        {
            app2Found = true;
            iSuitePref2 = suitePref;
            continue;
        }
        delete suitePref;
        suitePref = NULL;
    }
    CHECK((app1Found == true) && (app2Found == true));
    int error1, error2;
    // check the unsupported methods
    TRAP(error1, iSuitePref1->SecurityPreferencesL());
    TRAP(error2, iSuitePref2->SecurityPreferencesL());
    CHECK((error1 == KErrNotSupported) && (error2 == KErrNotSupported));
    // null/unknown property
    _LIT(KDummy, "dummy");
    TPtrC value;
    iSuitePref1->PropertyL(KNullDesC(), value);
    CHECK(value.Length() == 0);
    iSuitePref1->PropertyL(KDummy, value);
    CHECK(value.Length() == 0);
    // check the known properties
    iSuitePref1->PropertyL(KMidletSuiteName, value);
    CHECK(value.Compare(KAppName1) == 0);
    iSuitePref1->PropertyL(KMidletSuiteVendor, value);
    CHECK(value.Compare(KAppVendor1) == 0);
    iSuitePref1->PropertyL(KMidletSuiteVersion, value);
    CHECK(value.Compare(KAppVersion1) == 0);
    iSuitePref2->PropertyL(KMidletSuiteName, value);
    CHECK(value.Compare(KAppName2) == 0);
    iSuitePref2->PropertyL(KMidletSuiteVendor, value);
    CHECK(value.Compare(KAppVendor2) == 0);
    iSuitePref2->PropertyL(KMidletSuiteVersion, value);
    CHECK(value.Compare(KAppVersion2) == 0);
}

TEST(TestUserPreferences, TestCustomAttributesL)
{
    CHECK(iUserPreferences != NULL);
    // two "known" suites
    iStorageHandler->cleanStorage(APPLICATION_PACKAGE_TABLE, iAppUid1);
    iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid1);
    iStorageHandler->populateApplicationPackageTable(iAppUid1, L"appName1", L"appVendor1", L"1.1.0");
    iStorageHandler->populateMIDPPackageTable(iAppUid1, L"UTPD");
    iStorageHandler->cleanStorage(APPLICATION_PACKAGE_TABLE, iAppUid2);
    iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid2);
    iStorageHandler->populateApplicationPackageTable(iAppUid2, L"appName2", L"appVendor2", L"2.0.1");
    iStorageHandler->populateMIDPPackageTable(iAppUid2, L"OPD");
    iCustomAttr = iUserPreferences->CustomAttributesL(iAppId1);
    CHECK(iCustomAttr != NULL);
    // unsupported methods
    int error;
    TRAP(error, iCustomAttr->AddL(KNullDesC(), KNullDesC(), 0));
    CHECK(error == KErrNotSupported);
    TRAP(error, iCustomAttr->RemoveAllL());
    CHECK(error == KErrNotSupported);
    // unknown attributes
    _LIT(KDummy, "dummy");
    TUint32 flags;
    TRAP(error, iCustomAttr->GetL(KDummy(), flags));
    CHECK(error == KErrNotSupported);
    TRAP(error, iCustomAttr->UpdateL(KDummy(), KDummy(), flags));
    CHECK(error == KErrNotSupported);
    // retrieval of values for OnScreenKeyboardAttribute (unknown, "no", "gameactions", "navigationkeys")
    bool success;
    iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid1);
    iStorageHandler->populateMIDPPackageTable(iAppUid1, L"UTPD", L"unknown");
    TRAP(error, iCustomAttr->GetL(KOnScreenKeyboardAttribute, flags));
    CHECK(error == KErrNotFound);
    iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid1);
    iStorageHandler->populateMIDPPackageTable(iAppUid1, L"UTPD", L"3");
    TRAP(error, iCustomAttr->GetL(KOnScreenKeyboardAttribute, flags));
    CHECK(error == KErrNotFound);
    iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid1);
    iStorageHandler->populateMIDPPackageTable(iAppUid1, L"UTPD", L"0");
    HBufC* attrValue;
    attrValue = iCustomAttr->GetL(KOnScreenKeyboardAttribute, flags);
    success = (attrValue->Compare(KValueNo()) == 0);
    delete attrValue;
    attrValue = NULL;
    CHECK(success);
    iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid1);
    iStorageHandler->populateMIDPPackageTable(iAppUid1, L"UTPD", L"1");
    attrValue = iCustomAttr->GetL(KOnScreenKeyboardAttribute, flags);
    success = (attrValue->Compare(KValueGameactions()) == 0);
    delete attrValue;
    attrValue = NULL;
    CHECK(success);
    iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid1);
    iStorageHandler->populateMIDPPackageTable(iAppUid1, L"UTPD", L"2");
    attrValue = iCustomAttr->GetL(KOnScreenKeyboardAttribute, flags);
    success = (attrValue->Compare(KValueNavigationkeys()) == 0);
    delete attrValue;
    attrValue = NULL;
    CHECK(success);
    // updating with different values (unknown, "no", "gameactions", "navigationkeys")
    iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid1);
    iStorageHandler->populateMIDPPackageTable(iAppUid1, L"UTPD");
    iCustomAttr->UpdateL(KOnScreenKeyboardAttribute, KDummy, flags);
    std::wstring value = L"";
    iStorageHandler->readAttribute(MIDP_PACKAGE_TABLE, iAppUid1, ON_SCREEN_KEYPAD, value);
    CHECK(value.size() == 0);
    iCustomAttr->UpdateL(KOnScreenKeyboardAttribute, KValueNo, flags);
    iStorageHandler->readAttribute(MIDP_PACKAGE_TABLE, iAppUid1, ON_SCREEN_KEYPAD, value);
    CHECK(value.compare(L"0") == 0);
    iCustomAttr->UpdateL(KOnScreenKeyboardAttribute, KValueGameactions, flags);
    iStorageHandler->readAttribute(MIDP_PACKAGE_TABLE, iAppUid1, ON_SCREEN_KEYPAD, value);
    CHECK(value.compare(L"1") == 0);
    iCustomAttr->UpdateL(KOnScreenKeyboardAttribute, KValueNavigationkeys, flags);
    iStorageHandler->readAttribute(MIDP_PACKAGE_TABLE, iAppUid1, ON_SCREEN_KEYPAD, value);
    CHECK(value.compare(L"2") == 0);
    // OnScreenKeyboardAttribute not found
    iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid1);
    iStorageHandler->populateMIDPPackageTable(iAppUid1, L"UTPD");
    TRAP(error, iCustomAttr->GetL(KOnScreenKeyboardAttribute, flags));
    CHECK(error == KErrNotFound);
}