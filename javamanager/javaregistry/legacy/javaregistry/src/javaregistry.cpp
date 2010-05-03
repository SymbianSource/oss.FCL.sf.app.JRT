/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of internal Java Registry API class.
*
*/


// INCLUDE FILES

#include "installedappsregistry.h"
#include "javaregistrylegacy.h"
#include "javaregistry.h"
#include "javaregistrymidletentry.h"
#include "logger.h"

#include "javastorageexception.h"
#include "javastoragenames.h"
#include "javacommonutils.h"
#include "javastorage.h"
#include "javasymbianoslayer.h"

#include "javaregistryapplicationentry.h"
#include "javaregistryentry.h"
#include "javaregistrypackageentry.h"

using namespace java::storage;
using namespace java::util;
using namespace std;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// MJavaRegistry::CreateL
// ---------------------------------------------------------------------------
//
EXPORT_C MJavaRegistry* MJavaRegistry::CreateL()
{
    JELOG2(EJavaStorage);
    return CJavaRegistry::NewL();
}

// ---------------------------------------------------------------------------
// CJavaRegistry::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistry* CJavaRegistry::NewLC()
{
    JELOG2(EJavaStorage);
    LOG(EJavaStorage, EInfo, "CJavaRegistry::NewLC()");
    CJavaRegistry* self = new(ELeave) CJavaRegistry;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// ---------------------------------------------------------------------------
// CJavaRegistry::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistry* CJavaRegistry::NewL()
{
    JELOG2(EJavaStorage);
    CJavaRegistry* self = NewLC();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CJavaRegistry::Release
// ---------------------------------------------------------------------------
//
void CJavaRegistry::Release() const
{
    JELOG2(EJavaStorage);
    // Non-reference counted so just delete myself
    delete this;
}

// ---------------------------------------------------------------------------
// CJavaRegistry::InstalledSuiteUidsL
// ---------------------------------------------------------------------------
//
void CJavaRegistry::InstalledSuiteUidsL(RArray<TUid>& aSuiteUids) const
{
    JELOG2(EJavaStorage);

    // New registry filters non-present applictions.
    iRegistry->GetRegistryEntryUidsL(Java::EMidp2MidletSuite, aSuiteUids);
}

// ---------------------------------------------------------------------------
// CJavaRegistry::InstalledMIDletUidsL
// ---------------------------------------------------------------------------
//
void CJavaRegistry::InstalledMIDletUidsL(RArray<TUid>& aMIDletUids) const
{
    JELOG2(EJavaStorage);

    // New registry filters non-present applictions.
    iRegistry->GetRegistryEntryUidsL(Java::EMidp2Midlet, aMIDletUids);
}

// ---------------------------------------------------------------------------
// CJavaRegistry::SuiteEntryExistsL
// ---------------------------------------------------------------------------
//
TBool CJavaRegistry::SuiteEntryExistsL(TUid aSuiteUid) const
{
    JELOG2(EJavaStorage);

    if (aSuiteUid == TUid::Null())
    {
        User::Leave(KErrArgument);
    }

    RArray<TUid> suiteUids;

    // New registry filters non-present applictions.
    iRegistry->GetRegistryEntryUidsL(Java::EMidp2MidletSuite, suiteUids);
    int result = suiteUids.Find(aSuiteUid);

    suiteUids.Close();

    TBool exists = EFalse;

    if (KErrNotFound != result)
    {
        exists = ETrue;
    }
    return exists;
}

// ---------------------------------------------------------------------------
// CJavaRegistry::SuiteEntryL
// ---------------------------------------------------------------------------
//
MJavaRegistrySuiteEntry* CJavaRegistry::SuiteEntryL(TUid aSuiteUid) const
{
    JELOG2(EJavaStorage);

    if (TUid::Null() == aSuiteUid)
    {
        User::Leave(KErrArgument);
    }

    // New registry filters non-present applictions.
    Java::CJavaRegistryPackageEntry* entry =
        (Java::CJavaRegistryPackageEntry*)
        iRegistry->RegistryEntryL(aSuiteUid);

    if (!entry)
    {
        User::Leave(KErrNotFound);
    }

    return CJavaRegistrySuiteEntry::NewL(entry);
}

// ---------------------------------------------------------------------------
// CJavaRegistry::SuiteEntryL
// ---------------------------------------------------------------------------
//
MJavaRegistrySuiteEntry* CJavaRegistry::SuiteEntryL(
    const TDesC& aSuiteName, const TDesC& aSuiteVendor) const
{
    JELOG2(EJavaStorage);

    if (aSuiteName.Compare(KNullDesC) == 0
            || aSuiteVendor.Compare(KNullDesC) == 0)
    {
        User::Leave(KErrArgument);
    }

    // Convert to wstrings
    auto_ptr<HBufC16> nameBuf(HBufC::NewL((aSuiteName.Length() + 1)));
    TPtr16 nameBufPtr(nameBuf->Des());
    nameBufPtr.Append(aSuiteName);

    wstring suiteName(desToWstring(nameBufPtr));
    nameBuf.reset(0);

    JavaStorageApplicationEntry_t findQuery;
    JavaStorageEntry attr;
    attr.setEntry(PACKAGE_NAME, suiteName);
    findQuery.insert(attr);

    auto_ptr<HBufC16>vendorBuf(HBufC::NewL((aSuiteVendor.Length() + 1)));
    TPtr16 vendorBufPtr(vendorBuf->Des());
    vendorBufPtr.Append(aSuiteVendor);

    wstring suiteVendor(desToWstring(vendorBufPtr));
    vendorBuf.reset(0);

    attr.setEntry(VENDOR, suiteVendor);
    findQuery.insert(attr);

    JavaStorageApplicationList_t foundApps;

    try
    {
        auto_ptr<JavaStorage> js(JavaStorage::createInstance());
        js->open();
        js->search(APPLICATION_PACKAGE_TABLE, findQuery, foundApps);
        js->close();
        js.reset(0);
    }
    catch (JavaStorageException& jse)
    {
        ELOG1(EJavaStorage, "Search failed: %d", jse.mStatus);
    }

    findQuery.clear();

    if (0 == foundApps.size())
    {
        User::Leave(KErrNotFound);
    }

    JavaStorageApplicationEntry_t suiteEntry = foundApps.front();
    attr.setEntry(ID, L"");

    JavaStorageApplicationEntry_t::const_iterator finder =
        suiteEntry.find(attr);

    wstring suiteUid = L"";
    if (finder != suiteEntry.end())
    {
        suiteUid = (*finder).entryValue();
    }

    java::util::Uid tempUid(suiteUid);
    TUid uid;
    TInt err = uidToTUid(tempUid, uid);

    if (KErrNone == err)
    {
        // New registry filters non-present applictions.
        Java::CJavaRegistryPackageEntry* entry =
            (Java::CJavaRegistryPackageEntry*)
            iRegistry->RegistryEntryL(uid);
        return CJavaRegistrySuiteEntry::NewL(entry);
    }
    else
    {
        ELOG1(EJavaStorage, "Uid conversion error: %d", err);
        User::Leave(KErrNotFound);
    }
    return NULL;
}

// ---------------------------------------------------------------------------
// CJavaRegistry::MIDletEntryL
// ---------------------------------------------------------------------------
//
MJavaRegistryMIDletEntry* CJavaRegistry::MIDletEntryL(TUid aMIDletUid) const
{
    JELOG2(EJavaStorage);

    // New registry filters non-present applictions.
    auto_ptr<Java::CJavaRegistryApplicationEntry> appEntry(
        (Java::CJavaRegistryApplicationEntry*)
        iRegistry->RegistryEntryL(aMIDletUid));

    if (appEntry.get() == NULL)
    {
        User::Leave(KErrNotFound);
    }

    Java::CJavaRegistryPackageEntry* packageEntry = appEntry->PackageEntryL();

    if (!packageEntry)
    {
        ELOG(EJavaStorage, "No suite found for application.");
        User::Leave(KErrNotFound);
    }

    CJavaRegistrySuiteEntry* registrySuiteEntry
    = CJavaRegistrySuiteEntry::NewL(packageEntry);

    if (!registrySuiteEntry)
    {
        ELOG(EJavaStorage, "Cannot construct CJavaRegistrySuiteEntry");
        delete packageEntry;
        packageEntry = NULL;
        User::Leave(KErrNotFound);
    }

    return CJavaRegistryMIDletEntry::NewL(*registrySuiteEntry,
                                          appEntry->Uid(),
                                          appEntry->Name(),
                                          EFalse);
}

// ---------------------------------------------------------------------------
// CJavaRegistry::InstalledAppsRegistryL
// ---------------------------------------------------------------------------
//
const MInstalledAppsRegistry& CJavaRegistry::InstalledAppsRegistryL() const
{
    JELOG2(EJavaStorage);
    return *iSisRegistry;
}

// ---------------------------------------------------------------------------
// CJavaRegistry::ConstructL
// ---------------------------------------------------------------------------
//
void CJavaRegistry::ConstructL()
{
    JELOG2(EJavaStorage);
    iSisRegistry = CInstalledAppsRegistry::NewL();

    iRegistry = Java::CJavaRegistry::NewL();
}

// ---------------------------------------------------------------------------
// CJavaRegistry::~CJavaRegistry
// ---------------------------------------------------------------------------
//
CJavaRegistry::~CJavaRegistry()
{
    JELOG2(EJavaStorage);

    if (iSisRegistry)
    {
        iSisRegistry->Release();
        iSisRegistry = NULL;
    }

    if (iRegistry)
    {
        delete iRegistry;
        iRegistry = NULL;
    }
}
