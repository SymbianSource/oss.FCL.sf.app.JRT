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
* Description:  Definition of internal Java Registry Suite Entry API.
*
*/


// INCLUDE FILES

#include "javaregistry.h"
#include "javaregistrymidletentry.h"
#include "javaregistrypackageentry.h"
#include "javaregistrysuiteentry.h"
#include "installedappsregistryentry.h"
#include "logger.h"
#include "javacommonutils.h"
#include "javasymbianoslayer.h"
#include "javaoslayer.h"
#include "javastoragenames.h"

using namespace java::util;
using namespace java::storage;
using namespace std;

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistrySuiteEntry* CJavaRegistrySuiteEntry::NewLC()
{
    JELOG2(EJavaStorage);
    CJavaRegistrySuiteEntry* self = new(ELeave) CJavaRegistrySuiteEntry;
    CleanupReleasePushL(*self);
    self->ConstructL();
    return self;
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::NewL
// ---------------------------------------------------------------------------
//
CJavaRegistrySuiteEntry* CJavaRegistrySuiteEntry::NewL(
    Java::CJavaRegistryPackageEntry* aEntry)
{
    CJavaRegistrySuiteEntry* self = new(ELeave) CJavaRegistrySuiteEntry;
    self->ConstructL(aEntry);
    return self;
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistrySuiteEntry* CJavaRegistrySuiteEntry::NewL()
{
    JELOG2(EJavaStorage);
    CJavaRegistrySuiteEntry* self = NewLC();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistrySuiteEntry* CJavaRegistrySuiteEntry::NewLC(
    CInstalledAppsRegistryEntry& aEntry)
{
    JELOG2(EJavaStorage);
    CJavaRegistrySuiteEntry* self = new(ELeave) CJavaRegistrySuiteEntry;
    CleanupReleasePushL(*self);
    self->ConstructL(aEntry);
    return self;
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistrySuiteEntry* CJavaRegistrySuiteEntry::NewL(
    CInstalledAppsRegistryEntry& aEntry)
{
    JELOG2(EJavaStorage);
    CJavaRegistrySuiteEntry* self = NewLC(aEntry);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::CJavaRegistrySuiteEntry
// ---------------------------------------------------------------------------
//
CJavaRegistrySuiteEntry::CJavaRegistrySuiteEntry()
{
    JELOG2(EJavaStorage);
    AddRef();
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::~CJavaRegistrySuiteEntry
// ---------------------------------------------------------------------------
//
CJavaRegistrySuiteEntry::~CJavaRegistrySuiteEntry()
{
    JELOG2(EJavaStorage);
    ASSERT(!iRefCount);

    if (iPackageEntry)
    {
        delete iPackageEntry;
        iPackageEntry = NULL;
    }

    if (iSisRegistryEntry)
    {
        iSisRegistryEntry->Release();
    }

    if (iJarPath)
    {
        delete iJarPath;
        iJarPath = NULL;
    }

    if (iJadPath)
    {
        delete iJadPath;
        iJadPath = NULL;
    }

    if (iJarUrl)
    {
        delete iJarUrl;
        iJarUrl = NULL;
    }

    if (iJadUrl)
    {
        delete iJadUrl;
        iJadUrl = NULL;
    }

    iSuiteEntry.clear();
    iEmbeddedUids.Close();
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::ConstructL
// ---------------------------------------------------------------------------
//
void CJavaRegistrySuiteEntry::ConstructL
(CInstalledAppsRegistryEntry& aEntry)
{
    JELOG2(EJavaStorage);
    // We have got a reference to the entry
    iSisRegistryEntry = &aEntry;
    iSisRegistryEntry->AddRef();
    iPackageEntry = NULL;
    iJarPath = NULL;
    iJadPath = NULL;
    iJarUrl = NULL;
    iJadUrl = NULL;
}

void CJavaRegistrySuiteEntry::ConstructL(Java::CJavaRegistryPackageEntry* aEntry)
{
    ConstructL();
    iPackageEntry = aEntry;
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::ConstructL
// ---------------------------------------------------------------------------
//
void CJavaRegistrySuiteEntry::ConstructL()
{
    JELOG2(EJavaStorage);

    iSisRegistryEntry = NULL;
    iPackageEntry = NULL;
    iJarPath = NULL;
    iJadPath = NULL;
    iJarUrl = NULL;
    iJadUrl = NULL;
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::AddRef
// ---------------------------------------------------------------------------
//
void CJavaRegistrySuiteEntry::AddRef() const
{
    JELOG2(EJavaStorage);
    ++iRefCount;
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::Release
// ---------------------------------------------------------------------------
//
void CJavaRegistrySuiteEntry::Release() const
{
    JELOG2(EJavaStorage);
    // Only delete if the reference count is 0 after the decrement
    TInt refCount = iRefCount-1;

    if (!--iRefCount)
    {
        delete this;
    }
    LOG1(EJavaStorage, EInfo, "Reference count: %d", refCount);
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::UidL
// ---------------------------------------------------------------------------
//
TUid CJavaRegistrySuiteEntry::UidL() const
{
    JELOG2(EJavaStorage);
    return iPackageEntry->Uid();
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::SuiteNameL
// ---------------------------------------------------------------------------
//
const TDesC& CJavaRegistrySuiteEntry::SuiteNameL() const
{
    JELOG2(EJavaStorage);
    return iPackageEntry->Name();
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::VendorL
// ---------------------------------------------------------------------------
//
const TDesC& CJavaRegistrySuiteEntry::VendorL() const
{
    JELOG2(EJavaStorage);
    return iPackageEntry->Vendor();
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::VersionL
// ---------------------------------------------------------------------------
//
TAppVersion CJavaRegistrySuiteEntry::VersionL() const
{
    JELOG2(EJavaStorage);
    return iPackageEntry->Version();
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::JadPathL
// ---------------------------------------------------------------------------
//
const TDesC& CJavaRegistrySuiteEntry::JadPathL() const
{
    JELOG2(EJavaStorage);

    if (!iJadPath)
    {
        wstring value = L"";
        EntryAttributeValue(JAD_PATH, value);
        iJadPath = wstringToBuf(value);
    }
    return *iJadPath;
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::JadUrlL
// ---------------------------------------------------------------------------
//
const TDesC& CJavaRegistrySuiteEntry::JadUrlL() const
{
    JELOG2(EJavaStorage);

    if (!iJadUrl)
    {
        wstring value = L"";
        EntryAttributeValue(JAD_URL, value);
        iJadUrl = wstringToBuf(value);
    }
    return *iJadUrl;
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::JarPathL
// ---------------------------------------------------------------------------
//
const TDesC& CJavaRegistrySuiteEntry::JarPathL() const
{
    JELOG2(EJavaStorage);

    if (!iJarPath)
    {
        wstring value = L"";
        EntryAttributeValue(JAR_PATH, value);

        if (0 == value.size())
        {
            ELOG(EJavaStorage, "JAR path not found");
            User::Leave(KErrNotFound);
        }
        iJarPath = wstringToBuf(value);
    }
    return *iJarPath;
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::JarUrlL
// ---------------------------------------------------------------------------
//
const TDesC& CJavaRegistrySuiteEntry::JarUrlL() const
{
    JELOG2(EJavaStorage);
    if (!iJarUrl)
    {
        wstring value = L"";
        EntryAttributeValue(JAR_URL, value);

        if (0 == value.size())
        {
            ELOG(EJavaStorage, "JAR URL not found");
            User::Leave(KErrNotFound);
        }
        iJarUrl = wstringToBuf(value);
    }
    return *iJarUrl;
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::MIDletUids
// ---------------------------------------------------------------------------
//
EXPORT_C const RArray<TUid>& CJavaRegistrySuiteEntry::MIDletUids() const
{
    // This is not supported as this is not part of MJavaRegistrySuiteEntry
    // interface.
    return iEmbeddedUids;
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::MIDletUidsL
// ---------------------------------------------------------------------------
//
void CJavaRegistrySuiteEntry::MIDletUidsL(
    RArray<TUid>& aMIDletUids) const
{
    JELOG2(EJavaStorage);
    iPackageEntry->GetEmbeddedEntries(aMIDletUids);
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::NumberOfMIDletsL
// ---------------------------------------------------------------------------
//
TInt CJavaRegistrySuiteEntry::NumberOfMIDletsL() const
{
    JELOG2(EJavaStorage);
    return iPackageEntry->NumberOfEmbeddedEntries();
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::MIDletByUidL
// ---------------------------------------------------------------------------
//
MJavaRegistryMIDletEntry* CJavaRegistrySuiteEntry::MIDletByUidL
(TUid aMIDletUid) const
{
    JELOG2(EJavaStorage);

    if (TUid::Null() == aMIDletUid)
    {
        User::Leave(KErrArgument);
    }

    RArray<TUid> ownedMIDlets;
    MIDletUidsL(ownedMIDlets);

    TInt result = ownedMIDlets.Find(aMIDletUid);

    ownedMIDlets.Close();

    if (KErrNotFound == result)
    {
        User::Leave(KErrNotFound);
    }

    JavaStorageApplicationEntry_t app;
    ApplicationEntry(aMIDletUid, app);

    wstring midletName = L"";
    EntryAttributeValue(app, NAME, midletName);
    auto_ptr<HBufC>nameBuf(wstringToBuf(midletName));

    return CJavaRegistryMIDletEntry::NewL(*this,
                                          aMIDletUid,
                                          nameBuf->Des());
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::MIDletByNumberL
// ---------------------------------------------------------------------------
//
MJavaRegistryMIDletEntry* CJavaRegistrySuiteEntry::MIDletByNumberL
(TInt aMIDletNum) const
{
    JELOG2(EJavaStorage);

    if (aMIDletNum < 1)
    {
        User::Leave(KErrArgument);
    }

    RArray<TUid> ownedMIDlets;
    MIDletUidsL(ownedMIDlets);

    if (aMIDletNum > ownedMIDlets.Count())
    {
        ownedMIDlets.Close();
        User::Leave(KErrNotFound);
    }

    // aMIDletNum is one based whereas ownedMIDlets are zero based.
    TUid midletUid = ownedMIDlets[aMIDletNum - 1];
    ownedMIDlets.Close();

    JavaStorageApplicationEntry_t app;
    ApplicationEntry(midletUid, app);

    wstring midletName = L"";
    EntryAttributeValue(app, NAME, midletName);
    auto_ptr<HBufC>nameBuf(wstringToBuf(midletName));

    return CJavaRegistryMIDletEntry::NewL(*this,
                                          midletUid,
                                          nameBuf->Des());
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::WriteableEntry
// ---------------------------------------------------------------------------
//
EXPORT_C
CInstalledAppsRegistryEntry& CJavaRegistrySuiteEntry::WriteableEntry()
{
    JELOG2(EJavaStorage);
    return *iSisRegistryEntry;
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::InstalledAppsEntryL
// ---------------------------------------------------------------------------
//
const MInstalledAppsRegistryEntry&
CJavaRegistrySuiteEntry::InstalledAppsEntryL() const
{
    JELOG2(EJavaStorage);

    if (!iSisRegistryEntry)
    {
        Java::CJavaRegistry* reg = Java::CJavaRegistry::NewL();

        if (!reg)
        {
            ELOG(EJavaStorage, "Cannot create CJavaRegistry instance");
            User::Leave(KErrGeneral);
        }

        Java::CJavaRegistryEntry* entry = reg->RegistryEntryL(UidL());

        delete reg;
        reg = NULL;

        if (!entry)
        {
            ELOG(EJavaStorage, "Registry entry not found!");
            User::Leave(KErrNotFound);
        }

        iSisRegistryEntry = CInstalledAppsRegistryEntry::NewL(entry);
    }
    return *iSisRegistryEntry;
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::EntryAttributeValue
// ---------------------------------------------------------------------------
//
void CJavaRegistrySuiteEntry::EntryAttributeValue(const wstring& aName,
        wstring& aValue /*OUT*/) const
{
    if (iSuiteEntry.size() == 0)
    {
        java::util::Uid uid;
        TUidToUid(iPackageEntry->Uid(), uid);
        try
        {
            auto_ptr<JavaStorage> js(JavaStorage::createInstance());
            js->open();
            js->read(APPLICATION_PACKAGE_TABLE, uid, iSuiteEntry);
            js->close();
            js.reset(0);
        }
        catch (JavaStorageException& jse)
        {
            ELOG1(EJavaStorage, "Read failed: %s ",
                  jse.toString().c_str());
        }
    }

    JavaStorageEntry attr;
    attr.setEntry(aName, L"");

    JavaStorageApplicationEntry_t::const_iterator finder =
        iSuiteEntry.find(attr);

    if (finder != iSuiteEntry.end())
    {
        aValue = (*finder).entryValue();
    }
    else
    {
        LOG(EJavaStorage, EInfo, "Entry attribute value not found");
        aValue = L"";
    }
}

// ---------------------------------------------------------------------------
// CJavaRegistrySuiteEntry::ApplicationEntry
// ---------------------------------------------------------------------------
//
void CJavaRegistrySuiteEntry::ApplicationEntry(
    const TUid aUid,
    JavaStorageApplicationEntry_t& aAppEntry /*OUT*/) const
{
    try
    {
        java::util::Uid entryUid;
        auto_ptr<JavaStorage> js(JavaStorage::createInstance());
        js->open();
        js->read(APPLICATION_TABLE,
                 TUidToUid(aUid, entryUid),
                 aAppEntry);
        js->close();
        js.reset(0);

    }
    catch (JavaStorageException& jse)
    {
        ELOG1(EJavaStorage, "App entry read failed: %s",
              jse.toString().c_str());
    }
}

void CJavaRegistrySuiteEntry::EntryAttributeValue(
    const JavaStorageApplicationEntry_t& aEntry,
    const wstring& aName,
    wstring& aValue) const
{
    JavaStorageEntry attr;
    attr.setEntry(aName, L"");

    JavaStorageApplicationEntry_t::iterator findIterator = aEntry.find(attr);

    if (findIterator != aEntry.end())
    {
        aValue = (*findIterator).entryValue();
    }
    else
    {
        LOG(EJavaStorage, EInfo, "Entry attribute value not found");
        aValue = L"";
    }
}
