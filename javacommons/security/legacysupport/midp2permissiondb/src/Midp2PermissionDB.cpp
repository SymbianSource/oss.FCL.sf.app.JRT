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
#include "Midp2PermissionDB.h"
#include "StorageUtils.h"
#include "javasymbianoslayer.h"
#include "javacommonutils.h"
#include "javauid.h"

using namespace MIDP;
using namespace std;
using namespace java::security::legacysupport;
using namespace java::util;

EXPORT_C Midp2PermissionDB* Midp2PermissionDB::NewL(const TDesC& aMidletSuiteName,
        const TDesC& aMidletSuiteVendor,
        const TDesC& aMidletSuiteVersion)
{
    Midp2PermissionDB* self = new(ELeave) Midp2PermissionDB();
    CleanupStack::PushL(self);
    self->ConstructL(aMidletSuiteName, aMidletSuiteVendor, aMidletSuiteVersion);
    CleanupStack::Pop(self);
    return self;
}

void Midp2PermissionDB::ConstructL(const TDesC& aMidletSuiteName,
                                   const TDesC& aMidletSuiteVendor,
                                   const TDesC& aMidletSuiteVersion)
{
    MidletSuiteInfo midletSuiteInfo;
    StorageUtils* storage = StorageUtils::NewL();
    CleanupStack::PushL(storage);
    storage->readMidletSuiteInfoL(
        wstring((wchar_t*) aMidletSuiteName.Ptr(), aMidletSuiteName.Length()),
        wstring((wchar_t*) aMidletSuiteVendor.Ptr(), aMidletSuiteVendor.Length()),
        wstring((wchar_t*) aMidletSuiteVersion.Ptr(), aMidletSuiteVersion.Length()),
        midletSuiteInfo);
    CleanupStack::PopAndDestroy(storage);
    iMidletSuiteUid = midletSuiteInfo.iUid;
    iMidletSuiteProtectionDomain = midletSuiteInfo.iProtectionDomain;
}

Midp2PermissionDB::Midp2PermissionDB()
{
}

Midp2PermissionDB::~Midp2PermissionDB()
{
}

EXPORT_C TInt Midp2PermissionDB::GetDomain(TMidp2Domain& aDomain)
{
    if (iMidletSuiteProtectionDomain.compare(L"UTPD") == 0)
    {
        aDomain = EDomainUntrusted;
    }
    else if (iMidletSuiteProtectionDomain.compare(L"ITPD") == 0)
    {
        aDomain = EDomainTTP;
    }
    else if (iMidletSuiteProtectionDomain.compare(L"MFD") == 0)
    {
        aDomain = EDomainManufacturer;
    }
    else if (iMidletSuiteProtectionDomain.compare(L"OPD") == 0)
    {
        aDomain = EDomainOperator;
    }
    else
    {
        return KErrNotFound;
    }
    return KErrNone;
}

EXPORT_C TInt Midp2PermissionDB::GetFunctionGroupPermission(
    const TMidp2FunctionGroup& aFunctionGroup,
    TMidp2PermissionType& aPermissionType,
    TMidp2PermissionMode& aPermissionMode,
    TUint8& aAllowedModesMask)
{

    if (aFunctionGroup <= EPhoneCall || aFunctionGroup >= ELastFunctionGroupMarker)
    {
        return KErrNotFound;
    }

    // The allowed modes info is stored as a 4-bit constant:
    //    X(oneshot)X(session)X(blanket)X(no)
    // e.g. 1011 (=11) means that oneshot, blanket and no are allowed.
    // The following constants are used to encode/decode the allowed modes
    // into/from a 4-bit number
    int NO_MASK = 1;
    int BLANKET_MASK = 2;
    int SESSION_MASK = 4;
    int ONESHOT_MASK = 8;

    wstring settingsName = wstring((wchar_t*) KJcfFunctionGroups[aFunctionGroup]);
    MidletSuiteSecuritySettings securitySettings;
    StorageUtils* storage = NULL;
    int result = KErrNotFound;
    TRAP_IGNORE(
        storage = StorageUtils::NewL();
        CleanupStack::PushL(storage);
        result = storage->readMidletSuiteSecuritySettings(iMidletSuiteUid, settingsName, securitySettings);
        CleanupStack::PopAndDestroy(storage);
    );
    if (result != KErrNone)
    {
        return KErrNotFound;
    }
    if (securitySettings.iCurrentInteractionMode <= 0
            || securitySettings.iCurrentInteractionMode > 4
            || securitySettings.iAllowedInteractionModes <= 0
            || securitySettings.iAllowedInteractionModes > 15)
    {
        aPermissionType = EAllowed;
        return KErrNone;
    }
    aPermissionType = EUser;
    // set current interaction mode
    switch (securitySettings.iCurrentInteractionMode)
    {
    case 1:
        aPermissionMode = EOneShot;
        break;
    case 2:
        aPermissionMode = ESession;
        break;
    case 3:
        aPermissionMode = EBlanket;
        break;
    case 4:
        aPermissionMode = EDenied;
        break;
    }
    // set the allowed interaction modes
    int tmp = securitySettings.iAllowedInteractionModes & BLANKET_MASK;
    if (tmp > 0)
    {
        aAllowedModesMask |= EBlanket;
    }
    tmp = securitySettings.iAllowedInteractionModes & SESSION_MASK;
    if (tmp > 0)
    {
        aAllowedModesMask |= ESession;
    }
    tmp = securitySettings.iAllowedInteractionModes & ONESHOT_MASK;
    if (tmp > 0)
    {
        aAllowedModesMask |= EOneShot;
    }
    tmp = securitySettings.iAllowedInteractionModes & NO_MASK;
    if (tmp > 0)
    {
        aAllowedModesMask |= EDenied;
    }
    return KErrNone;
}

EXPORT_C TInt Midp2PermissionDB::SetFunctionGroupPermission(const TMidp2FunctionGroup&  aFunctionGroup,
        const TMidp2PermissionType& /*aPermissionType*/,
        const TMidp2PermissionMode& aPermissionMode)
{

    if (aFunctionGroup <= EPhoneCall || aFunctionGroup >= ELastFunctionGroupMarker)
    {
        return KErrNotFound;
    }

    wstring settingsName = wstring((wchar_t*)  KJcfFunctionGroups[aFunctionGroup]);
    int currentInteractionMode;
    switch (aPermissionMode)
    {
    case EOneShot:
        currentInteractionMode = 1;
        break;
    case ESession:
        currentInteractionMode = 2;
        break;
    case EBlanket:
        currentInteractionMode = 3;
        break;
    case EDenied:
        currentInteractionMode = 4;
        break;
    default:
        // oneshot
        currentInteractionMode = 1;
        break;
    }
    StorageUtils* storage = NULL;
    int result = KErrNotFound;
    TRAP_IGNORE(
        storage = StorageUtils::NewL();
        CleanupStack::PushL(storage);
        result = storage->writeMidletSuiteSecuritySettings(iMidletSuiteUid, settingsName, currentInteractionMode);
        if (result == KErrNone)
{
    if (aPermissionMode == EBlanket)
        {
            // "blanket mode, if selected by the user, MUST NOT result in any
            // additional prompts" (MIDP spec) -> mark down that the prompt
            // in blanket mode was shown
            storage->writeUserSecuritySettingsPromptFlag(iMidletSuiteUid, settingsName, true /* prompt is blanket mode was shown */);
        }
    }
    CleanupStack::PopAndDestroy(storage);
    );
    return result;
}

EXPORT_C TInt Midp2PermissionDB::GetDomainFromName(const TDesC& /*aName*/,
        TMidp2Domain& /*aDomain*/)
{
    return KErrNotSupported;
}

EXPORT_C TInt Midp2PermissionDB::GetFunctionGroupFromName(
    const TDesC& /*aName*/,
    TMidp2FunctionGroup& /*aFunctionGroup*/)
{
    return KErrNotSupported;
}

EXPORT_C TInt Midp2PermissionDB::GetPermissionFromName(
    const TDesC& /*aName*/, TMidp2Permission& /*aPermission*/)
{
    return KErrNotSupported;
}

EXPORT_C void Midp2PermissionDB::ReadMIDP2ConfigSetting(
    const enum TMidp2ConfigKey& aKey, TInt& aKeyValue)
{
    aKeyValue = (IsFunctionGroupGranted(aKey) ? 1:0);
}

TBool Midp2PermissionDB::IsFunctionGroupGranted(TInt aFGIndex)
{
    // boundary check
    if (aFGIndex < 0 || aFGIndex >= ELastFunctionGroupMarker)
    {
        return EFalse;
    }
    StorageUtils* storage = NULL;
    std::vector<MidletSuiteSecuritySettings> securitySettings;
    int result = KErrNotFound;
    TRAP_IGNORE(
        storage = StorageUtils::NewL();
        CleanupStack::PushL(storage);
        result = storage->readMidletSuiteSecuritySettings(iMidletSuiteUid, securitySettings);
        CleanupStack::PopAndDestroy(storage);
    );
    if (result != KErrNone)
    {
        return EFalse;
    }
    wstring settingsName = wstring((wchar_t*) KJcfFunctionGroups[aFGIndex]);
    for (int i=0; i<securitySettings.size(); i++)
    {
        if (securitySettings[i].iSettingsName == settingsName)
        {
            return ETrue;
        }
    }
    return EFalse;
}
