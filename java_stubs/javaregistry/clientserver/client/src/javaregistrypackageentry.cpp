/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include "javaregistrypackageentry.h"
#include "javaregistryapplicationentry.h"
#include "writeablejavaregistrypackageentry.h"
#include <appversion.h>


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
    return TAppVersion(0, 0, 0);
}


// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::Vendor
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CJavaRegistryPackageEntry::Vendor() const
{
    return KNullDesC;
}



// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::IsUninstallable
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CJavaRegistryPackageEntry::IsUninstallable() const
{
    return ETrue;
}


// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::GetEmbeddedEntries
// ---------------------------------------------------------------------------
//
EXPORT_C void CJavaRegistryPackageEntry::GetEmbeddedEntries
(RArray<TUid>& /*aUids*/) const
{
}


// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::NumberOfEmbeddedEntries
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CJavaRegistryPackageEntry::NumberOfEmbeddedEntries() const
{
    return 0;
}


// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::EmbeddedEntryByUidL
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistryEntry* CJavaRegistryPackageEntry::
EmbeddedEntryByUidL(const TUid& /*aEntryUid*/) const
{
    User::Leave(KErrNotSupported);
    return NULL;
}


// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::EmbeddedEntryByNumberL
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistryEntry* CJavaRegistryPackageEntry::
EmbeddedEntryByNumberL(TInt /*aEntryNum*/) const
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::UsedUserDiskSpace
// ---------------------------------------------------------------------------
EXPORT_C TInt64 CJavaRegistryPackageEntry::UsedUserDiskSpace() const
{
    return 0;
}

// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::IsPreinstalled
// ---------------------------------------------------------------------------
EXPORT_C TBool CJavaRegistryPackageEntry::IsPreinstalled() const
{
    return EFalse;
}
