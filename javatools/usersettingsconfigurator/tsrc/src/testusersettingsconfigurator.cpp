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
#include <vector>
#include <e32std.h>

#include "TestHarness.h"
#include "teststoragehandler.h"
#include "j2me/midp2/security/MFunctionGroupBinding.h"
#include "j2me/midp2/security/MExtendedProtectionDomain.h"
#include "j2me/midp2/security/MFunctionGroup.h"
#include "j2me/midp2/security/MPermission.h"
#include "j2me/midp2/security/MSecurityPolicyV2.h"

using namespace MIDP;

const TInt KSecurityPolicyFlags = 0x0200;
_LIT(KSecurityPolicy, "midp2_rp");
extern MSecurityPolicyV2* GetSecurityPolicyL(TUint32 aFlags, const TDesC& aPolicyName);
const TInt BLANKET        = 1;
const TInt SESSION        = 2;
const TInt ONESHOT        = 3;
const TInt NO             = 4;
const TInt UNKNOWN        = 5;
const TInt INVALID        = 6;
const TInt EMPTY          = 7;
const TInt GAMEACTIONS    = 8;
const TInt NAVIGATIONKEYS = 9;

TEST_GROUP(TestUserSettingsConfigurator)
{
    MSecurityPolicyV2* iSecurityPolicy;
    TEST_SETUP()
    {
        iSecurityPolicy = GetSecurityPolicyL(KSecurityPolicyFlags, KSecurityPolicy);
    }

    TEST_TEARDOWN()
    {
        delete iSecurityPolicy;
    }
};

HBufC8* GetCustomAttributes(const TDesC& aValue)
{
    _LIT(KOnScreenKeyboardAttribute, "Nokia-MIDlet-On-Screen-Keypad");
    _LIT8(KSpace, " ");

    TInt countLength = 1;
    TInt spaceLength = KSpace().Length();
    TInt keyLength       = KOnScreenKeyboardAttribute().Length();
    TInt keyByteLength   = keyLength + keyLength;
    TInt valueLength     = 2 + aValue.Length();
    TInt valueByteLength = valueLength + valueLength;
    HBufC8* customAttributesValue = HBufC8::NewL(
                                        4 /* count */
                                        + 4 /* key length */
                                        + keyByteLength /* key */
                                        + 3 /* max padding */
                                        + 4 /* value length */
                                        + 4 /* flag */
                                        + valueByteLength /* value */
                                        + 3 /* max padding */);
    TPtr8 ptr = customAttributesValue->Des();
    ptr.Append(TPtrC8((const TUint8*)&countLength, 4));
    ptr.Append(TPtrC8((const TUint8*)&keyLength, 4));
    ptr.Append(TPtrC8((const TUint8*)KOnScreenKeyboardAttribute().Ptr(), keyByteLength));
    switch (keyByteLength % 4)
    {
    case 1:
        ptr.Append(KSpace);
    case 2:
        ptr.Append(KSpace);
    case 3:
        ptr.Append(KSpace);
        break;
    }
    // flag
    ptr.Append(TPtrC8((const TUint8*)&valueLength, 4));
    ptr.Append(KSpace);
    ptr.Append(KSpace);
    ptr.Append(KSpace);
    ptr.Append(KSpace);
    ptr.Append(TPtrC8((const TUint8*)aValue.Ptr(), (2 * aValue.Length())));
    switch (valueByteLength % 4)
    {
    case 1:
        ptr.Append(KSpace);
    case 2:
        ptr.Append(KSpace);
    case 3:
        ptr.Append(KSpace);
        break;
    }
    return customAttributesValue;
}

HBufC8* GetSecurityPreferences(const MSecurityPolicyV2& aSecurityPolicy, std::wstring aSecuritySettingsName, int aInteractionMode)
{
    int BITS_PER_BYTE = 8;
    int BITS_PER_GROUP = 2;
    int GROUPS_PER_BYTE = (BITS_PER_BYTE/BITS_PER_GROUP);
    RPointerArray<const MFunctionGroupBinding>  bindings;
    aSecurityPolicy.UntrustedProtectionDomainL().FunctionGroupBindingsL(bindings);
    TInt count = bindings.Count();
    std::string preferences = "";
    std::string interactionMode;
    HBufC8* prefs = HBufC8::NewMaxL((count + 3)/4);
    TPtr8 ptr = prefs->Des();
    TInt length = ptr.Length();
    for (TInt i = 0; i < length; i++)
    {
        ptr[i] = 0;
    }
    for (TInt i = 0; i < count; i++)
    {
        const MFunctionGroupBinding& binding = *(bindings[i]);
        HBufC* name = binding.FunctionGroup().Name().AllocLC();
        std::wstring securitySettingsName = std::wstring((wchar_t*) name->Ptr(), name->Length());
        CleanupStack::PopAndDestroy(name);
        TInt    byteIndex = ((i) * BITS_PER_GROUP) / BITS_PER_BYTE;
        TInt    bitIndex  = ((i) - (byteIndex * GROUPS_PER_BYTE)) * BITS_PER_GROUP;
        TUint8  byte      = ptr[byteIndex];
        if (securitySettingsName == aSecuritySettingsName)
        {
            switch (aInteractionMode)
            {
            case ONESHOT:
                ptr[byteIndex] = (TText8)((byte & ~(3 << bitIndex)) | (2 << bitIndex));
                break;
            case SESSION:
                ptr[byteIndex] = (TText8)((byte & ~(3 << bitIndex)) | (3 << bitIndex));
                break;
            case BLANKET:
                ptr[byteIndex] = (TText8)((byte & ~(3 << bitIndex)) | (1 << bitIndex));
                break;
            case NO:
                ptr[byteIndex] = (TText8)((byte & ~(3 << bitIndex)) | (0 << bitIndex));
                break;
            case UNKNOWN:
                ptr[byteIndex] = (TText8)((byte & ~(3 << bitIndex)) | (4 << bitIndex));
                break;
            }
        }
        else
        {
            // get the default one from the legacy policy
            switch (binding.DefaultInteractionMode())
            {
            case MPermission::EDenied:
                ptr[byteIndex] = (TText8)((byte & ~(3 << bitIndex)) | (0 << bitIndex));
                break;
            case MPermission::EBlanket:
                ptr[byteIndex] = (TText8)((byte & ~(3 << bitIndex)) | (1 << bitIndex));
                break;
            case MPermission::EOneshot:
                ptr[byteIndex] = (TText8)((byte & ~(3 << bitIndex)) | (2 << bitIndex));
                break;
            case MPermission::ESession:
                ptr[byteIndex] = (TText8)((byte & ~(3 << bitIndex)) | (3 << bitIndex));
                break;
            }
        }
    }
    bindings.Close();
    return prefs;
}


std::wstring GetDefaultInteractionModeFromPolicy(const MSecurityPolicyV2& aSecurityPolicy, std::wstring aSecuritySettingsName)
{
    RPointerArray<const MFunctionGroupBinding>  bindings;
    aSecurityPolicy.UntrustedProtectionDomainL().FunctionGroupBindingsL(bindings);
    TInt count = bindings.Count();
    std::wstring interactionMode = L"";
    for (TInt i = 0; i < count; i++)
    {
        const MFunctionGroupBinding& binding = *(bindings[i]);
        HBufC* name = binding.FunctionGroup().Name().AllocLC();
        std::wstring securitySettingsName = std::wstring((wchar_t*) name->Ptr(), name->Length());
        CleanupStack::PopAndDestroy(name);
        if (securitySettingsName == aSecuritySettingsName)
        {
            // get the default one from the legacy policy
            switch (binding.DefaultInteractionMode())
            {
            case MPermission::EDenied:
                interactionMode = L"4";
                break;
            case MPermission::EBlanket:
                interactionMode = L"3";
                break;
            case MPermission::EOneshot:
                interactionMode = L"1";
                break;
            case MPermission::ESession:
                interactionMode = L"2";
                break;
            }
            break;
        }
    }
    bindings.Close();
    return interactionMode;
}

void ExecuteConfigurator()
{
    _LIT(KJavaUsersettingsConfiguratorExe, "javausersettingsconfigurator.exe");
    RProcess proc;
    int rc = proc.Create(KJavaUsersettingsConfiguratorExe, KNullDesC());
    if (rc == KErrNone)
    {
        proc.Resume();
        TRequestStatus status;
        proc.Logon(status);
        User::WaitForRequest(status);
    }
    proc.Close();
}

TEST(TestUserSettingsConfigurator, TestUserSecuritySettingsL)
{
    // init
    TInt err;
    TestStorageHandler* storage = TestStorageHandler::NewL();
    CleanupStack::PushL(storage);
    std::vector<SecuritySettings*> securitySettings;
    bool success = false;
    // unknown unsigned suite
    storage->cleanup(TUint32(0), L"appUid");
    storage->populateLegacyStorageL(TUint32(0), _L("name"), _L("vendor"), _L("version"), _L8("prefs"));
    storage->populateOmjStorageL(L"appUid", L"name1", L"vendor", L"version", L"settingsName", L"1");
    ExecuteConfigurator();
    securitySettings.clear();
    storage->retrieveSecuritySettings(L"name", L"vendor", L"version", securitySettings);
    storage->cleanup(TUint32(0), L"appUid");
    success = (securitySettings.size() == 0);
    for (int i=0; i<securitySettings.size(); i++)
    {
        delete securitySettings[i];
    }
    CHECK(success);
    // OMJ "Net Access" settings: No for current interaction mode, Oneshot&Session for allowed interaction modes
    // Legacy settings: unknown value for "Net Access"
    // -> new OMJ settings: the default interaction mode taken from the legacy policy for "Net Access"
    storage->cleanup(TUint32(0), L"appUid");
    HBufC8* prefs = GetSecurityPreferences(*iSecurityPolicy, L"Local Connectivity", UNKNOWN);
    storage->populateLegacyStorageL(TUint32(0), _L("name"), _L("vendor"), _L("version"), *prefs);
    delete prefs;
    storage->populateOmjStorageL(L"appUid", L"name", L"vendor", L"version", L"Net Access", L"4" , L"12");
    ExecuteConfigurator();
    securitySettings.clear();
    storage->retrieveSecuritySettings(L"name", L"vendor", L"version", securitySettings);
    storage->cleanup(TUint32(0), L"appUid");
    success = (securitySettings.size() == 1 && securitySettings[0]->iCurrentInteractionMode == GetDefaultInteractionModeFromPolicy(*iSecurityPolicy, L"Net Access") && securitySettings[0]->iSettingsName == L"Net Access");
    for (int i=0; i<securitySettings.size(); i++)
    {
        delete securitySettings[i];
    }
    CHECK(success);
    // OMJ "Read User Data Access" settings: No for current interaction mode, Session for allowed interaction modes
    // Legacy settings: unknown value for "Read User Data Access"
    // -> new OMJ settings: unchanged settings (the default interaction mode taken from the legacy policy for Net Access is not found among the allowed interaction modes)
    storage->cleanup(TUint32(0), L"appUid");
    storage->populateLegacyStorageL(TUint32(0), _L("name"), _L("vendor"), _L("version"), _L8("prefs"));
    storage->populateOmjStorageL(L"appUid", L"name", L"vendor", L"version", L"Read User Data Access", L"4" , L"4");
    ExecuteConfigurator();
    securitySettings.clear();
    storage->retrieveSecuritySettings(L"name", L"vendor", L"version", securitySettings);
    storage->cleanup(TUint32(0), L"appUid");
    success = (securitySettings.size() == 1 && securitySettings[0]->iCurrentInteractionMode == L"4" && securitySettings[0]->iSettingsName == L"Read User Data Access");
    for (int i=0; i<securitySettings.size(); i++)
    {
        delete securitySettings[i];
    }
    CHECK(success);
    // OMJ "Write User Data Access" settings: No for current interaction mode, Oneshot&Session for allowed interaction modes
    // Legacy settings: oneshot for "Write User Data Access"
    // -> new OMJ settings: oneshot for "Write User Data Access"
    storage->cleanup(TUint32(0), L"appUid");
    prefs = GetSecurityPreferences(*iSecurityPolicy, L"Write User Data Access", ONESHOT);
    storage->populateLegacyStorageL(TUint32(0), _L("name"), _L("vendor"), _L("version"), *prefs);
    delete prefs;
    storage->populateOmjStorageL(L"appUid", L"name", L"vendor", L"version", L"Write User Data Access", L"4" , L"12");
    ExecuteConfigurator();
    securitySettings.clear();
    storage->retrieveSecuritySettings(L"name", L"vendor", L"version", securitySettings);
    storage->cleanup(TUint32(0), L"appUid");
    success = (securitySettings.size() == 1 && securitySettings[0]->iCurrentInteractionMode == L"1" && securitySettings[0]->iSettingsName == L"Write User Data Access");
    for (int i=0; i<securitySettings.size(); i++)
    {
        delete securitySettings[i];
    }
    CHECK(success);
    // OMJ "Local Connectivity" settings: Oneshot for current interaction mode, Oneshot&Session for allowed interaction modes
    // Legacy settings: session for "Local Connectivity"
    // -> new OMJ settings: session for "Local Connectivity"
    storage->cleanup(TUint32(0), L"appUid");
    prefs = GetSecurityPreferences(*iSecurityPolicy, L"Local Connectivity", SESSION);
    storage->populateLegacyStorageL(TUint32(0), _L("name"), _L("vendor"), _L("version"), *prefs);
    delete prefs;
    storage->populateOmjStorageL(L"appUid", L"name", L"vendor", L"version", L"Local Connectivity", L"1" , L"12");
    ExecuteConfigurator();
    securitySettings.clear();
    storage->retrieveSecuritySettings(L"name", L"vendor", L"version", securitySettings);
    storage->cleanup(TUint32(0), L"appUid");
    success = (securitySettings.size() == 1 && securitySettings[0]->iCurrentInteractionMode == L"2" && securitySettings[0]->iSettingsName == L"Local Connectivity");
    for (int i=0; i<securitySettings.size(); i++)
    {
        delete securitySettings[i];
    }
    CHECK(success);
    // OMJ "Messaging" settings: Oneshot for current interaction mode, Oneshot&Session&Blanket for allowed interaction modes
    // Legacy settings: Blanket for "Messaging"
    // -> new OMJ settings: Blanket for "Messaging"
    storage->cleanup(TUint32(0), L"appUid");
    prefs = GetSecurityPreferences(*iSecurityPolicy, L"Messaging", BLANKET);
    storage->populateLegacyStorageL(TUint32(0), _L("name"), _L("vendor"), _L("version"), *prefs);
    delete prefs;
    storage->populateOmjStorageL(L"appUid", L"name", L"vendor", L"version", L"Messaging", L"1" , L"14");
    ExecuteConfigurator();
    securitySettings.clear();
    storage->retrieveSecuritySettings(L"name", L"vendor", L"version", securitySettings);
    storage->cleanup(TUint32(0), L"appUid");
    success = (securitySettings.size() == 1 && securitySettings[0]->iCurrentInteractionMode == L"3" && securitySettings[0]->iSettingsName == L"Messaging");
    for (int i=0; i<securitySettings.size(); i++)
    {
        delete securitySettings[i];
    }
    CHECK(success);
    // OMJ "ApplicationAutoInvocation" settings: Oneshot for current interaction mode, Oneshot&No for allowed interaction modes
    // Legacy settings: No for "ApplicationAutoInvocation"
    // -> new OMJ settings: No for "ApplicationAutoInvocation"
    storage->cleanup(TUint32(0), L"appUid");
    prefs = GetSecurityPreferences(*iSecurityPolicy, L"Application Auto Invocation", NO);
    storage->populateLegacyStorageL(TUint32(0), _L("name"), _L("vendor"), _L("version"), *prefs);
    delete prefs;
    storage->populateOmjStorageL(L"appUid", L"name", L"vendor", L"version", L"Application Auto Invocation", L"1" , L"9");
    ExecuteConfigurator();
    securitySettings.clear();
    storage->retrieveSecuritySettings(L"name", L"vendor", L"version", securitySettings);
    storage->cleanup(TUint32(0), L"appUid");
    success = (securitySettings.size() == 1 && securitySettings[0]->iCurrentInteractionMode == L"4" && securitySettings[0]->iSettingsName == L"Application Auto Invocation");
    for (int i=0; i<securitySettings.size(); i++)
    {
        delete securitySettings[i];
    }
    CHECK(success);
    // cleanup
    CleanupStack::PopAndDestroy(storage);
}

TEST(TestUserSettingsConfigurator, TestUserCustomAttributesSettingsL)
{
    // init
    TInt err;
    TestStorageHandler* storage = TestStorageHandler::NewL();
    CleanupStack::PushL(storage);
    bool success = false;
    // "gameactions" value
    HBufC8* customAttributes = GetCustomAttributes(_L("gameactions"));
    storage->populateLegacyStorageL(TUint32(12345), *customAttributes);
    delete customAttributes;
    storage->populateOmjStorageL(L"[00003039]", L"0");
    ExecuteConfigurator();
    std::wstring onScreenKeypad;
    storage->retrieveOnScreenKeypad(L"[00003039]", onScreenKeypad);
    storage->cleanup(TUint32(0), L"[00003039]");
    success = (onScreenKeypad == L"1");
    CHECK(success);
    // "navigationkeys" value
    customAttributes = GetCustomAttributes(_L("navigationkeys"));
    storage->populateLegacyStorageL(TUint32(12345), *customAttributes);
    delete customAttributes;
    storage->populateOmjStorageL(L"[00003039]", L"0");
    ExecuteConfigurator();
    onScreenKeypad = L"";
    storage->retrieveOnScreenKeypad(L"[00003039]", onScreenKeypad);
    storage->cleanup(TUint32(0), L"[00003039]");
    success = (onScreenKeypad == L"2");
    CHECK(success);
    // "no" value
    customAttributes = GetCustomAttributes(_L("no"));
    storage->populateLegacyStorageL(TUint32(12345), *customAttributes);
    delete customAttributes;
    storage->populateOmjStorageL(L"[00003039]", L"2");
    ExecuteConfigurator();
    onScreenKeypad = L"";
    storage->retrieveOnScreenKeypad(L"[00003039]", onScreenKeypad);
    storage->cleanup(TUint32(0), L"[00003039]");
    success = (onScreenKeypad == L"0");
    CHECK(success);
    // unknown value
    customAttributes = GetCustomAttributes(_L("unknown"));
    storage->populateLegacyStorageL(TUint32(12345), *customAttributes);
    delete customAttributes;
    storage->populateOmjStorageL(L"[00003039]", L"2");
    ExecuteConfigurator();
    onScreenKeypad = L"";
    storage->retrieveOnScreenKeypad(L"[00003039]", onScreenKeypad);
    storage->cleanup(TUint32(0), L"[00003039]");
    success = (onScreenKeypad == L"2");
    CHECK(success);
    // no value at all
    storage->populateOmjStorageL(L"[00003039]", L"1");
    ExecuteConfigurator();
    onScreenKeypad = L"";
    storage->retrieveOnScreenKeypad(L"[00003039]", onScreenKeypad);
    storage->cleanup(TUint32(0), L"[00003039]");
    success = (onScreenKeypad == L"1");
    CHECK(success);
    // empty value
    customAttributes = GetCustomAttributes(_L(""));
    storage->populateLegacyStorageL(TUint32(12345), *customAttributes);
    delete customAttributes;
    storage->populateOmjStorageL(L"[00003039]", L"1");
    ExecuteConfigurator();
    onScreenKeypad = L"";
    storage->retrieveOnScreenKeypad(L"[00003039]", onScreenKeypad);
    storage->cleanup(TUint32(0), L"[00003039]");
    success = (onScreenKeypad == L"1");
    CHECK(success);
    // cleanup
    CleanupStack::PopAndDestroy(storage);
}
