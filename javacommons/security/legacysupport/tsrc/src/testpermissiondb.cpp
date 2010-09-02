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

#include "j2me/midp2/security/midp2permissiondb.h"

using namespace MIDP;
using namespace java::storage;
using namespace java::util;

_LIT(KAppName, "appName");
_LIT(KAppVendor, "appVendor");
_LIT(KAppVersion, "1.0.1");

TEST_GROUP(TestPermissionDb)
{

    Midp2PermissionDB* iPermissionDb;
    TestStorageHandler* iStorageHandler;
    Uid iAppUid;

    TEST_SETUP()
    {
        iAppUid = Uid(L"appUid");
        iStorageHandler = new TestStorageHandler();
        iStorageHandler->cleanStorage(APPLICATION_PACKAGE_TABLE, iAppUid);
        iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid);
        iStorageHandler->populateApplicationPackageTable(iAppUid, L"appName", L"appVendor", L"1.0.1");
    }

    TEST_TEARDOWN()
    {
        if (iPermissionDb)
        {
            delete iPermissionDb;
            iPermissionDb = NULL;
        }
        if (iStorageHandler)
        {
            iStorageHandler->cleanStorage(APPLICATION_PACKAGE_TABLE, iAppUid);
            iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid);
            iStorageHandler->cleanStorage(MIDP_FUNC_GRP_SETTINGS_TABLE, iAppUid);
            delete iStorageHandler;
            iStorageHandler = NULL;
        }
    }
};

/**
 */
TEST(TestPermissionDb, TestNewL)
{
    int error;
    _LIT(KDummy, "dummy");
    _LIT(KAppNameLowCase, "appname");
    _LIT(KAppVendorLowCase, "appvendor");
    // null values
    TRAP(error, iPermissionDb = Midp2PermissionDB::NewL(KAppName, KAppVendor, KNullDesC()));
    CHECK(error == KErrNotFound);
    TRAP(error, iPermissionDb = Midp2PermissionDB::NewL(KAppName, KNullDesC(), KNullDesC()));
    CHECK(error == KErrNotFound);
    TRAP(error, iPermissionDb = Midp2PermissionDB::NewL(KNullDesC(), KNullDesC(), KNullDesC()));
    CHECK(error == KErrNotFound);
    // dummy values
    TRAP(error, iPermissionDb = Midp2PermissionDB::NewL(KDummy, KDummy, KDummy));
    CHECK(error == KErrNotFound);
    TRAP(error, iPermissionDb = Midp2PermissionDB::NewL(KAppName, KDummy, KDummy));
    CHECK(error == KErrNotFound);
    TRAP(error, iPermissionDb = Midp2PermissionDB::NewL(KAppName, KAppVendor, KDummy));
    CHECK(error == KErrNotFound);
    // case-sensitivity
    TRAP(error, iPermissionDb = Midp2PermissionDB::NewL(KAppName, KAppVendorLowCase, KAppVersion));
    CHECK(error == KErrNotFound);
    TRAP(error, iPermissionDb = Midp2PermissionDB::NewL(KAppNameLowCase, KAppVendor, KAppVersion));
    CHECK(error == KErrNotFound);
}

TEST(TestPermissionDb, TestGetDomainL)
{
    int error;
    // domain not found
    TMidp2Domain domain;
    TRAP(error, iPermissionDb = Midp2PermissionDB::NewL(KAppName, KAppVendor, KAppVersion));
    CHECK(error == KErrNotFound);
    // unknown domain
    iStorageHandler->populateMIDPPackageTable(iAppUid, L"unknown");
    iPermissionDb = Midp2PermissionDB::NewL(KAppName, KAppVendor, KAppVersion);
    CHECK(iPermissionDb != NULL && iPermissionDb->GetDomain(domain) == KErrNotFound);
    // unidentified third party domain
    iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid);
    iStorageHandler->populateMIDPPackageTable(iAppUid, L"UTPD");
    delete iPermissionDb;
    iPermissionDb = NULL;
    iPermissionDb = Midp2PermissionDB::NewL(KAppName, KAppVendor, KAppVersion);
    CHECK(iPermissionDb != NULL && iPermissionDb->GetDomain(domain) == KErrNone && domain == EDomainUntrusted);
    // manufacturer domain
    iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid);
    iStorageHandler->populateMIDPPackageTable(iAppUid, L"MFD");
    delete iPermissionDb;
    iPermissionDb = NULL;
    iPermissionDb = Midp2PermissionDB::NewL(KAppName, KAppVendor, KAppVersion);
    CHECK(iPermissionDb != NULL && iPermissionDb->GetDomain(domain) == KErrNone && domain == EDomainManufacturer);
    // operator domain
    iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid);
    iStorageHandler->populateMIDPPackageTable(iAppUid, L"OPD");
    delete iPermissionDb;
    iPermissionDb = NULL;
    iPermissionDb = Midp2PermissionDB::NewL(KAppName, KAppVendor, KAppVersion);
    CHECK(iPermissionDb != NULL && iPermissionDb->GetDomain(domain) == KErrNone && domain == EDomainOperator);
    // identified third party domain
    iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid);
    iStorageHandler->populateMIDPPackageTable(iAppUid, L"ITPD");
    delete iPermissionDb;
    iPermissionDb = NULL;
    iPermissionDb = Midp2PermissionDB::NewL(KAppName, KAppVendor, KAppVersion);
    CHECK(iPermissionDb != NULL && iPermissionDb->GetDomain(domain) == KErrNone && domain == EDomainTTP);
}

TEST(TestPermissionDb, TestNotSupported)
{
    TMidp2Domain domain;
    TMidp2FunctionGroup fg;
    TMidp2Permission perm;
    iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid);
    iStorageHandler->populateMIDPPackageTable(iAppUid, L"UTPD");
    iPermissionDb = Midp2PermissionDB::NewL(KAppName, KAppVendor, KAppVersion);
    CHECK(iPermissionDb->GetDomainFromName(KNullDesC(), domain) == KErrNotSupported);
    CHECK(iPermissionDb->GetFunctionGroupFromName(KNullDesC(), fg) == KErrNotSupported);
    CHECK(iPermissionDb->GetPermissionFromName(KNullDesC(), perm) == KErrNotSupported);
}

TEST(TestPermissionDb, TestGetFunctionGroupPermission)
{
    TMidp2PermissionType permissionType;
    TMidp2PermissionMode permissionMode;
    TUint8 allowedModesMask;
    iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid);
    iStorageHandler->populateMIDPPackageTable(iAppUid, L"UTPD");
    iPermissionDb = Midp2PermissionDB::NewL(KAppName, KAppVendor, KAppVersion);
    // function group not found
    iStorageHandler->cleanStorage(MIDP_FUNC_GRP_SETTINGS_TABLE, iAppUid);
    iStorageHandler->populateSecuritySettingsTable(iAppUid, L"Net Access");
    CHECK(iPermissionDb->GetFunctionGroupPermission(ELastFunctionGroupMarker, permissionType, permissionMode, allowedModesMask) == KErrNotFound);
    CHECK(iPermissionDb->GetFunctionGroupPermission(EMultimediaRecording, permissionType, permissionMode, allowedModesMask) == KErrNotFound);
    // function group allowed
    iStorageHandler->populateSecuritySettingsTable(iAppUid, L"Low Level Net Access");
    CHECK(iPermissionDb->GetFunctionGroupPermission(ELowLevelNetAccess, permissionType, permissionMode, allowedModesMask) == KErrNone && permissionType == EAllowed);
    // both current interaction mode and allowed interaction modes must be set
    iStorageHandler->cleanStorage(MIDP_FUNC_GRP_SETTINGS_TABLE, iAppUid);
    iStorageHandler->populateSecuritySettingsTable(iAppUid, L"Call Control", L"1");
    CHECK(iPermissionDb->GetFunctionGroupPermission(ECallControl, permissionType, permissionMode, allowedModesMask) == KErrNone && permissionType == EAllowed);
    iStorageHandler->cleanStorage(MIDP_FUNC_GRP_SETTINGS_TABLE, iAppUid);
    iStorageHandler->populateSecuritySettingsTable(iAppUid, L"Messaging", L"", L"1");
    CHECK(iPermissionDb->GetFunctionGroupPermission(EMessaging, permissionType, permissionMode, allowedModesMask) == KErrNone && permissionType == EAllowed);
    // invalid settings
    iStorageHandler->cleanStorage(MIDP_FUNC_GRP_SETTINGS_TABLE, iAppUid);
    iStorageHandler->populateSecuritySettingsTable(iAppUid, L"Restricted Messaging", L"5", L"4");
    CHECK(iPermissionDb->GetFunctionGroupPermission(ERestrictedMessaging, permissionType, permissionMode, allowedModesMask) == KErrNone && permissionType == EAllowed);
    iStorageHandler->cleanStorage(MIDP_FUNC_GRP_SETTINGS_TABLE, iAppUid);
    iStorageHandler->populateSecuritySettingsTable(iAppUid, L"Application Auto Invocation", L"1", L"16");
    CHECK(iPermissionDb->GetFunctionGroupPermission(EAppAutoInvocation, permissionType, permissionMode, allowedModesMask) == KErrNone && permissionType == EAllowed);
    // current interaction mode = oneshot, allowed interaction modes = denied
    iStorageHandler->cleanStorage(MIDP_FUNC_GRP_SETTINGS_TABLE, iAppUid);
    iStorageHandler->populateSecuritySettingsTable(iAppUid, L"Local Connectivity", L"1", L"1");
    allowedModesMask = 0;
    CHECK(iPermissionDb->GetFunctionGroupPermission(ELocalConnectivity, permissionType, permissionMode, allowedModesMask) == KErrNone && permissionType == EUser && permissionMode == EOneShot && allowedModesMask == EDenied);
    // current interaction mode = session, allowed interaction modes = denied + blanket
    iStorageHandler->cleanStorage(MIDP_FUNC_GRP_SETTINGS_TABLE, iAppUid);
    iStorageHandler->populateSecuritySettingsTable(iAppUid, L"Multimedia Recording", L"2", L"3");
    allowedModesMask = 0;
    CHECK(iPermissionDb->GetFunctionGroupPermission(EMultimediaRecording, permissionType, permissionMode, allowedModesMask) == KErrNone && permissionType == EUser && permissionMode == ESession && allowedModesMask == (EDenied + EBlanket));
    // current interaction mode = blanket, allowed interaction modes = denied + blanket + session
    iStorageHandler->cleanStorage(MIDP_FUNC_GRP_SETTINGS_TABLE, iAppUid);
    iStorageHandler->populateSecuritySettingsTable(iAppUid, L"Read User Data Access", L"3", L"7");
    allowedModesMask = 0;
    CHECK(iPermissionDb->GetFunctionGroupPermission(EReadUserDataAccess, permissionType, permissionMode, allowedModesMask) == KErrNone && permissionType == EUser && permissionMode == EBlanket && allowedModesMask == (EDenied + EBlanket + ESession));
    // current interaction mode = denied, allowed interaction modes = denied + blanket + session + oneshot
    iStorageHandler->cleanStorage(MIDP_FUNC_GRP_SETTINGS_TABLE, iAppUid);
    iStorageHandler->populateSecuritySettingsTable(iAppUid, L"Write User Data Access", L"4", L"15");
    allowedModesMask = 0;
    CHECK(iPermissionDb->GetFunctionGroupPermission(EWriteUserDataAccess, permissionType, permissionMode, allowedModesMask) == KErrNone && permissionType == EUser && permissionMode == EDenied && allowedModesMask == (EDenied + EBlanket + ESession + EOneShot));
}

TEST(TestPermissionDb, TestSetFunctionGroupPermission)
{
    TMidp2PermissionType permissionType;
    TMidp2PermissionMode permissionMode;
    iStorageHandler->cleanStorage(MIDP_PACKAGE_TABLE, iAppUid);
    iStorageHandler->populateMIDPPackageTable(iAppUid, L"UTPD");
    iPermissionDb = Midp2PermissionDB::NewL(KAppName, KAppVendor, KAppVersion);
    // function group not found
    iStorageHandler->cleanStorage(MIDP_FUNC_GRP_SETTINGS_TABLE, iAppUid);
    CHECK(iPermissionDb->SetFunctionGroupPermission(ELastFunctionGroupMarker, permissionType, permissionMode) == KErrNotFound);
    // oneshot
    iStorageHandler->cleanStorage(MIDP_FUNC_GRP_SETTINGS_TABLE, iAppUid);
    iStorageHandler->populateSecuritySettingsTable(iAppUid, L"Location", L"1", L"4");
    CHECK(iPermissionDb->SetFunctionGroupPermission(ELocation, permissionType, EOneShot) == KErrNone);
    // session
    iStorageHandler->cleanStorage(MIDP_FUNC_GRP_SETTINGS_TABLE, iAppUid);
    iStorageHandler->populateSecuritySettingsTable(iAppUid, L"Landmark", L"1", L"4");
    CHECK(iPermissionDb->SetFunctionGroupPermission(ELandmark, permissionType, ESession) == KErrNone);
    // blanket
    iStorageHandler->cleanStorage(MIDP_FUNC_GRP_SETTINGS_TABLE, iAppUid);
    iStorageHandler->populateSecuritySettingsTable(iAppUid, L"Local Connectivity", L"1", L"4");
    CHECK(iPermissionDb->SetFunctionGroupPermission(ELocalConnectivity, permissionType, EBlanket) == KErrNone);
    std::wstring blanketPrompt;
    iStorageHandler->readBlanketPrompt(iAppUid, L"Local Connectivity", blanketPrompt);
    CHECK(blanketPrompt == L"1");
}