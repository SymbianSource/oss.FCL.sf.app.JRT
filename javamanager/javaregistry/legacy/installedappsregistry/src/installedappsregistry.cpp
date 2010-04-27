/*
* Copyright (c) 1997-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CInstalledAppsRegistry class - as we apply
*                Bridge patten, almost each method in this class delegates
*                the call to the implementation class.
*
*/


// INCLUDE FILES

#include <javaregistryentrytype.h>

#include "installedappsregistry.h"
#include "installedappsregistryentry.h"
#include "javaregistryentrytype.h"
#include "javaregistrysuiteentry.h"
#include "javasymbianoslayer.h"
#include "javastorage.h"
#include "javastoragenames.h"
#include "logger.h"
#include "mjavaregistry.h"

using namespace java::storage;
using namespace java::util;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// MJavaRegistry::CreateL
// This function is defined in MInstalledAppsRegistry. Whoever links to this
// file will create a read-only registry.
// ---------------------------------------------------------------------------
//
EXPORT_C MInstalledAppsRegistry* MInstalledAppsRegistry::CreateL()
{
    JELOG2(EJavaStorage);
    return CInstalledAppsRegistry::NewL();
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::NewL
// We make use of TLS.
// ---------------------------------------------------------------------------
//
EXPORT_C CInstalledAppsRegistry* CInstalledAppsRegistry::NewL()
{
    JELOG2(EJavaStorage);

    CInstalledAppsRegistry* instance =
        REINTERPRET_CAST(CInstalledAppsRegistry*, Dll::Tls());

    if (!instance)
    {
        instance = new(ELeave) CInstalledAppsRegistry();
        CleanupStack::PushL(instance);
        instance->ConstructL();
        CleanupStack::Pop(instance);

        // Set the thread local storage pointer for this DLL to be our
        // singleton instance
        Dll::SetTls(instance);
    }

    // Increment the reference count
    ++instance->iRefCount;
    LOG1(EJavaStorage, EInfo, "Reference count: %d", instance->iRefCount);
    return instance;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::Release
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistry::Release()
{
    JELOG2(EJavaStorage);

    // Only delete if the reference count has decremented to 0
    TInt refCount = iRefCount-1;

    LOG1(EJavaStorage, EInfo, "Reference count: %d", iRefCount);

    if (!--iRefCount)
    {
        delete this;
        Dll::SetTls(NULL);
    }
    LOG1(EJavaStorage, EInfo, "Reference count: %d", refCount);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::~CInstalledAppsRegistry
// ---------------------------------------------------------------------------
//
CInstalledAppsRegistry::~CInstalledAppsRegistry()
{
    if (iRegistry)
    {
        delete iRegistry;
        iRegistry = NULL;
    }
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::CInstalledAppsRegistry
// ---------------------------------------------------------------------------
//
CInstalledAppsRegistry::CInstalledAppsRegistry()
{
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::ConstructL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistry::ConstructL()
{
    iRegistry = Java::CJavaRegistry::NewL();
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::AddEntryL
// ---------------------------------------------------------------------------
//
EXPORT_C void CInstalledAppsRegistry::AddEntryL
(const CInstalledAppsRegistryEntry& /*aEntry*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::DeleteEntry
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CInstalledAppsRegistry::DeleteEntry(const TUid& /*aUid*/)
{
    return KErrNotSupported;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::InstalledUidsL
// ---------------------------------------------------------------------------
//
void CInstalledAppsRegistry::InstalledUidsL
(RArray<TUid>& aUidArray) const
{
    JELOG2(EJavaStorage);

    // New registry filters out non-present entries.
    iRegistry->GetRegistryEntryUidsL(aUidArray);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::EntryPresentL
// ---------------------------------------------------------------------------
//
TBool CInstalledAppsRegistry::EntryPresentL(const TUid& aUid) const
{
    JELOG2(EJavaStorage);
    // New registry filters out non-present entries.
    return iRegistry->RegistryEntryExistsL(aUid);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::WriteableEntryL
// ---------------------------------------------------------------------------
//
EXPORT_C CInstalledAppsRegistryEntry* CInstalledAppsRegistry::WriteableEntryL
(const TUid& /*aUid*/) const
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::WriteableEntryL
// ---------------------------------------------------------------------------
//
EXPORT_C CInstalledAppsRegistryEntry* CInstalledAppsRegistry::WriteableEntryL
(TLanguage /*aLanguage*/,
 const TDesC& /*aPackageName*/,
 const TDesC& /*aVendor*/) const
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::WriteableEntryLC
// ---------------------------------------------------------------------------
//
EXPORT_C CInstalledAppsRegistryEntry* CInstalledAppsRegistry::WriteableEntryLC
(const TUid& /*aUid*/) const
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::WriteableEntryLC
// ---------------------------------------------------------------------------
//
EXPORT_C CInstalledAppsRegistryEntry* CInstalledAppsRegistry::WriteableEntryLC
(TLanguage /*aLang*/,
 const TDesC& /*aPackage*/,
 const TDesC& /*aVendor*/) const
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::EntryL
// ---------------------------------------------------------------------------
//
MInstalledAppsRegistryEntry* CInstalledAppsRegistry::EntryL
(const TUid& aUid) const
{
    JELOG2(EJavaStorage);

    if (TUid::Null() == aUid)
    {
        User::Leave(KErrArgument);
    }

    Java::CJavaRegistryEntry* entry = iRegistry->RegistryEntryL(aUid);

    if (!entry)
    {
        User::Leave(KErrNotFound);
    }

    return CInstalledAppsRegistryEntry::NewL(entry);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::EntryL
// ---------------------------------------------------------------------------
//
MInstalledAppsRegistryEntry* CInstalledAppsRegistry::EntryL
(TLanguage /*aLang*/,
 const TDesC& /*aPackage*/,
 const TDesC& /*aVendor*/) const
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::EntryLC
// ---------------------------------------------------------------------------
//
MInstalledAppsRegistryEntry* CInstalledAppsRegistry::EntryLC
(const TUid& aUid) const
{
    JELOG2(EJavaStorage);
    if (TUid::Null() == aUid)
    {
        User::Leave(KErrArgument);
    }
    MInstalledAppsRegistryEntry* entry = EntryL(aUid);
    CleanupReleasePushL(*entry);
    return entry;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::EntryLC
// ---------------------------------------------------------------------------
//
MInstalledAppsRegistryEntry* CInstalledAppsRegistry::EntryLC
(TLanguage /*aLang*/,
 const TDesC& /*aPackage*/,
 const TDesC& /*aVendor*/) const
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::GetSystemPropertyL
// ---------------------------------------------------------------------------
//
HBufC8* CInstalledAppsRegistry::GetSystemPropertyL
(TUid /*aPropertyId*/) const
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::AppsUsingFileL
// ---------------------------------------------------------------------------
//
EXPORT_C void CInstalledAppsRegistry::AppsUsingFileL
(const TDesC& /*aFileName*/,
 RArray<TUid>& /*aUids*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::AppsUsingFilesL
// ---------------------------------------------------------------------------
//
EXPORT_C void CInstalledAppsRegistry::AppsUsingFilesL
(const RPointerArray<HBufC>& /*aFiles*/,
 RPointerArray<RArray<TUid> >& /*aUids*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::CorruptUidsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CInstalledAppsRegistry::CorruptUidsL
(RArray<TUid>& /*aUidArray*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CInstalledAppsRegistry::DependentsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CInstalledAppsRegistry::DependentsL(TUid /*aUid*/,
        RArray<TUid>& /*aDependents*/) const
{
    JELOG2(EJavaStorage);
    // This is not part of MIntalledAppsRegistry interface.
    User::Leave(KErrNotSupported);
}
