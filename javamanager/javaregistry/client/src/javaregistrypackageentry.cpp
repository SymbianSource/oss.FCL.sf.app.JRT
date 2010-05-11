/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  javaregistrypackageentry implementation
*
*/


#include <javaregistrypackageentry.h>
#include <javaregistryapplicationentry.h>
#include <appversion.h>

#include "writeablejavaregistrypackageentry.h"
#include "logger.h"

using namespace Java;
using namespace Java::Manager::Registry;

// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::CJavaRegistryPackageEntry
// ---------------------------------------------------------------------------
//
CJavaRegistryPackageEntry::CJavaRegistryPackageEntry
(CWriteableJavaRegistryPackageEntry* aWritableEntry) :
        CJavaRegistryEntry(aWritableEntry)
{
}

// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::Version
// ---------------------------------------------------------------------------
//
EXPORT_C TAppVersion CJavaRegistryPackageEntry::Version() const
{
    JELOG2(EJavaStorage);
    return WritableEntry()->Version();
}

// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::Vendor
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CJavaRegistryPackageEntry::Vendor() const
{
    JELOG2(EJavaStorage);
    return WritableEntry()->Vendor();
}

// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::IsUninstallable
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CJavaRegistryPackageEntry::IsUninstallable() const
{
    JELOG2(EJavaStorage);
    return WritableEntry()->IsUninstallable();
}

// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::GetEmbeddedEntries
// ---------------------------------------------------------------------------
//
EXPORT_C void CJavaRegistryPackageEntry::GetEmbeddedEntries
(RArray<TUid>& aUids) const
{
    JELOG2(EJavaStorage);
    WritableEntry()->GetEmbeddedEntries(aUids);
}

// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::NumberOfEmbeddedEntries
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CJavaRegistryPackageEntry::NumberOfEmbeddedEntries() const
{
    JELOG2(EJavaStorage);
    return WritableEntry()->NumberOfEmbeddedEntries();
}

// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::EmbeddedEntryByUidL
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistryEntry* CJavaRegistryPackageEntry::
EmbeddedEntryByUidL(const TUid& aEntryUid) const
{
    JELOG2(EJavaStorage);

    CWriteableJavaRegistryEntry* writeableEntry
    = WritableEntry()->EmbeddedEntryByUidL(aEntryUid);

    if (writeableEntry == NULL)
    {
        ILOG(EJavaStorage,
             "Can't find entry for the given uid, returning NULL.");
        return NULL;
    }
    CleanupStack::PushL(writeableEntry);

    CJavaRegistryEntry* entry = NULL;

    TJavaRegistryEntryType entryType= writeableEntry->Type();

    if (EGeneralPackage <= entryType  &&
            entryType < EGeneralApplication)
    {
        entry
        = new(ELeave) CJavaRegistryPackageEntry(
            (CWriteableJavaRegistryPackageEntry*) writeableEntry);
        ILOG(EJavaStorage, "PackageEntry created");
    }
    else if (EGeneralApplication <= entryType)
    {
        entry
        = new(ELeave) CJavaRegistryApplicationEntry
        ((CWriteableJavaRegistryApplicationEntry*) writeableEntry);
        ILOG(EJavaStorage, "ApplicationEntry created");
    }
    else
    {
        ELOG(EJavaStorage, "Invalid type, returning NULL");
        CleanupStack::PopAndDestroy(writeableEntry);
        return NULL;
    }

    CleanupStack::Pop(writeableEntry);
    return entry;
}

// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::EmbeddedEntryByNumberL
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistryEntry* CJavaRegistryPackageEntry::
EmbeddedEntryByNumberL(TInt aEntryNum) const
{
    JELOG2(EJavaStorage);
    CWriteableJavaRegistryEntry* writeableEntry
    = WritableEntry()->EmbeddedEntryByNumberL(aEntryNum);
    if (writeableEntry == NULL)
    {
        WLOG1(EJavaStorage,
              "Can't find entry for the given Entry-number: %d",
              aEntryNum);
        return NULL;
    }

    CleanupStack::PushL(writeableEntry);
    CJavaRegistryEntry* entry = NULL;
    TJavaRegistryEntryType entryType= writeableEntry->Type();

    if (EGeneralPackage <= entryType  &&
            entryType < EGeneralApplication)
    {
        entry
        = new(ELeave) CJavaRegistryPackageEntry(
            (CWriteableJavaRegistryPackageEntry*) writeableEntry);
        ILOG(EJavaStorage, "PackageEntry created");
    }
    else if (EGeneralApplication <= entryType)
    {
        entry
        = new(ELeave) CJavaRegistryApplicationEntry
        ((CWriteableJavaRegistryApplicationEntry*) writeableEntry);
        ILOG(EJavaStorage, "ApplicationEntry created");
    }
    else
    {
        ELOG(EJavaStorage, "Invalid type, returning NULL");
        CleanupStack::PopAndDestroy(writeableEntry);
        return NULL;
    }

    CleanupStack::Pop(writeableEntry);

    return entry;
}

// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::UsedUserDiskSpace
// ---------------------------------------------------------------------------
EXPORT_C TInt64 CJavaRegistryPackageEntry::UsedUserDiskSpace() const
{
    return WritableEntry()->UsedUserDiskSpace();
}

// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::UsedUserDiskSpace
// ---------------------------------------------------------------------------
EXPORT_C TBool CJavaRegistryPackageEntry::IsPreinstalled() const
{
    return WritableEntry()->IsPreinstalled();
}

// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::WritableEntry
// ---------------------------------------------------------------------------
//
CWriteableJavaRegistryPackageEntry* CJavaRegistryPackageEntry::
WritableEntry() const
{
    return (CWriteableJavaRegistryPackageEntry*)iWritableEntry;
}
