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
#include "javaosheaders.h"
#include "javasymbianoslayer.h"
#include "javacommonutils.h"
#include "s60commonutils.h"
#include "UserPreferences.h"
#include "UserPreferencesIterator.h"
#include "MIDletSuiteUserPreferences.h"
#include "MIDletSuiteCustomAttributes.h"
#include <MUserSecurityPreferencesTable.h>
#include <MSecurityPolicyV2.h>
#include "SecurityPreferences.h"
#include "javauid.h"

using namespace MIDP;
using namespace MIDP::DBv2;
using namespace java::security::legacysupport;
using namespace std;
using namespace java::util;

EXPORT_C MUserPreferences* GetUserPreferencesL(TUint32 /*aFlags*/,
        const MSecurityPolicyV2& /*aSecurityPolicy*/)
{
    return CUserPreferences::NewL();
}

CUserPreferences* CUserPreferences::NewL()
{
    CUserPreferences* self = new(ELeave) CUserPreferences();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CUserPreferences::CUserPreferences()
{
}

void CUserPreferences::ConstructL()
{
}

CUserPreferences::~CUserPreferences()
{
}

MMIDletSuiteUserPreferencesIterator* CUserPreferences::IteratorL()
{
    return CMIDletSuiteUserPreferencesIterator::NewL();
}

MMIDletSuiteCustomAttributes* CUserPreferences::CustomAttributesL(TUint32 aSuiteId)
{
    return CMIDletSuiteCustomAttributes::NewL(aSuiteId);
}

MSecurityPreferences* CUserPreferences::SecurityPreferencesL(const MMIDletSuiteInfo& /*aMIDletSuiteInfo*/)
{
    User::Leave(KErrNotSupported);
    return NULL; // this is here to make the compiler happy (it won't throw a warning)
}

CMIDletSuiteUserPreferencesIterator* CMIDletSuiteUserPreferencesIterator::NewL()
{
    CMIDletSuiteUserPreferencesIterator* self = new(ELeave) CMIDletSuiteUserPreferencesIterator();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void CMIDletSuiteUserPreferencesIterator::ConstructL()
{
    StorageUtils* storage = java::security::legacysupport::StorageUtils::NewL();
    CleanupStack::PushL(storage);
    storage->readMidletSuitesInfos(iMidletSuites);
    CleanupStack::PopAndDestroy(storage);
}

CMIDletSuiteUserPreferencesIterator::CMIDletSuiteUserPreferencesIterator()
        : iCurrentMidletSuite(0)
{
}

CMIDletSuiteUserPreferencesIterator::~CMIDletSuiteUserPreferencesIterator()
{
}

TBool CMIDletSuiteUserPreferencesIterator::HasNext()
{
    return (iCurrentMidletSuite < iMidletSuites.size());
}

MMIDletSuiteUserPreferences* CMIDletSuiteUserPreferencesIterator::NextL()
{
    if (iCurrentMidletSuite >= iMidletSuites.size())
    {
        User::Leave(KErrNotFound);
    }
    CMIDletSuiteUserPreferences* pref = CMIDletSuiteUserPreferences::NewL(
                                            *(iMidletSuites[iCurrentMidletSuite]));
    iCurrentMidletSuite++;
    return pref;
}

CMIDletSuiteUserPreferences* CMIDletSuiteUserPreferences::NewL(MidletSuiteInfo& aMidletSuiteInfo)
{
    CMIDletSuiteUserPreferences* self = new(ELeave) CMIDletSuiteUserPreferences(aMidletSuiteInfo);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void CMIDletSuiteUserPreferences::ConstructL()
{
}

CMIDletSuiteUserPreferences::CMIDletSuiteUserPreferences(MidletSuiteInfo& aMidletSuiteInfo)
        :iMidletSuiteInfo(aMidletSuiteInfo)
{
}

CMIDletSuiteUserPreferences::~CMIDletSuiteUserPreferences()
{
}

TUint32 CMIDletSuiteUserPreferences::Id() const
{
    Uid tmp(iMidletSuiteInfo.iUid);
    TUid uid;
    TInt err = uidToTUid(tmp, uid);
    if (err == KErrNone)
    {
        return uid.iUid;
    }
    else
    {
        return 0;
    }
}

TInt CMIDletSuiteUserPreferences::PropertyL(const TDesC& aName, TPtrC& aValue) const
{
    HBufC* desc = NULL;
    if (aName.Compare(KMidletSuiteName) == 0)
    {
        desc = S60CommonUtils::wstringToDes(iMidletSuiteInfo.iName.c_str());
    }
    else if (aName.Compare(KMidletSuiteVendor) == 0)
    {
        desc = S60CommonUtils::wstringToDes(iMidletSuiteInfo.iVendor.c_str());
    }
    else if (aName.Compare(KMidletSuiteVersion) == 0)
    {
        desc = S60CommonUtils::wstringToDes(iMidletSuiteInfo.iVersion.c_str());
    }
    if (desc != NULL)
    {
        aValue.Set(desc->Des());
        return KErrNone;
    }
    return KErrNotFound;
}

MSecurityPreferences& CMIDletSuiteUserPreferences::SecurityPreferencesL()
{
    User::Leave(KErrNotSupported);
    return *(reinterpret_cast<MSecurityPreferences*>(new int())); // this is here to make the compiler happy (it won't throw a warning)
}


CMIDletSuiteCustomAttributes* CMIDletSuiteCustomAttributes::NewL(TUint32 aSuiteId)
{
    CMIDletSuiteCustomAttributes* self = new(ELeave) CMIDletSuiteCustomAttributes(aSuiteId);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void CMIDletSuiteCustomAttributes::ConstructL()
{
}

CMIDletSuiteCustomAttributes::~CMIDletSuiteCustomAttributes()
{
}


HBufC* CMIDletSuiteCustomAttributes::GetL(const TDesC& aName, TUint32& aFlag)
{
    if (aName.Compare(KOnScreenKeyboardAttribute) != 0)
    {
        User::Leave(KErrNotSupported);
    }
    TUid suiteId = TUid::Uid(iSuiteId);
    Uid suiteUid;
    TUidToUid(suiteId, suiteUid);
    StorageUtils* storage = java::security::legacysupport::StorageUtils::NewL();
    CleanupStack::PushL(storage);
    wstring value = L"";
    storage->readOnScreenKeyboardAttributeL(suiteUid.toString(), value);
    CleanupStack::PopAndDestroy(storage);
    aFlag = KFlagUserModifiable;
    return mapOnScreenKeyboardToLegacyValueL(value);
}

void CMIDletSuiteCustomAttributes::AddL(const TDesC& /*aName*/,
                                        const TDesC& /*aValue*/,
                                        const TUint32 /*aFlag*/)
{
    User::Leave(KErrNotSupported);
}

void CMIDletSuiteCustomAttributes::UpdateL(const TDesC& aName,
        const TDesC& aValue,
        const TUint32 /*aFlag*/)
{
    if (aName.Compare(KOnScreenKeyboardAttribute) != 0)
    {
        User::Leave(KErrNotSupported);
    }
    wstring value = L"";
    mapOnScreenKeyboardToOmjValue(aValue, value);
    if (value.size() == 0)
    {
        return;
    }
    TUid suiteId = TUid::Uid(iSuiteId);
    Uid suiteUid;
    TUidToUid(suiteId, suiteUid);
    StorageUtils* storage = java::security::legacysupport::StorageUtils::NewL();
    CleanupStack::PushL(storage);
    storage->writeOnScreenKeyboardAttribute(suiteUid.toString(), value);
    CleanupStack::PopAndDestroy(storage);
}

void CMIDletSuiteCustomAttributes::RemoveAllL()
{
    User::Leave(KErrNotSupported);
}

CMIDletSuiteCustomAttributes::CMIDletSuiteCustomAttributes(TUint32 aSuiteId)
        : iSuiteId(aSuiteId)
{
}

HBufC* CMIDletSuiteCustomAttributes::mapOnScreenKeyboardToLegacyValueL(const std::wstring& aOmjValue)
{
    try
    {
        switch (java::util::JavaCommonUtils::wstringToInt(aOmjValue))
        {
        case 0:
            return KValueNo().Alloc();
        case 1:
            return KValueGameactions().Alloc();
        case 2:
            return KValueNavigationkeys().Alloc();
        }
    }
    catch (ExceptionBase& e)
    {
    }
    User::Leave(KErrNotFound);
    return NULL; // this is here to make the compiler happy (it won't throw a warning)
}

void CMIDletSuiteCustomAttributes::mapOnScreenKeyboardToOmjValue(const TDesC& aLegacyValue, std::wstring& aOmjValue)
{
    if (aLegacyValue.Compare(KValueNo) == 0)
    {
        aOmjValue += L"0";
    }
    else if (aLegacyValue.Compare(KValueGameactions) == 0)
    {
        aOmjValue += L"1";
    }
    else if (aLegacyValue.Compare(KValueNavigationkeys) == 0)
    {
        aOmjValue += L"2";
    }
}

CSecurityPreferences* CSecurityPreferences::NewL()
{
    CSecurityPreferences* sp = new(ELeave) CSecurityPreferences();
    CleanupStack::PushL(sp);
    sp->ConstructL();
    CleanupStack::Pop(sp);
    return sp;
}


void CSecurityPreferences::ConstructL()
{
}

CSecurityPreferences::CSecurityPreferences()
{
}

CSecurityPreferences::~CSecurityPreferences()
{
}

MPermission::TMode CSecurityPreferences::InteractionModeL(const MFunctionGroupBinding& /*aBinding*/)
{
    User::Leave(KErrNotSupported);
    return MPermission::EDenied; // this is here to make the compiler happy (it won't throw a warning)
}

void CSecurityPreferences::InteractionModeL(const MFunctionGroupBinding& /*aBinding*/,
        MPermission::TMode /*aMode*/)
{
    User::Leave(KErrNotSupported);
}

TBool CSecurityPreferences::IsFromDefaults(void) const
{
    return EFalse; // this is here to make the compiler happy (it won't throw a warning)
}

void CSecurityPreferences::SetInteractionMode(const MFunctionGroupBinding& /*aBinding*/,
        MPermission::TMode /*aMode*/)
{
}

