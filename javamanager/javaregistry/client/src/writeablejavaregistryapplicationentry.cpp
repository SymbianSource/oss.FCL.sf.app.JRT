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
* Description:  writeablejavaregistryapplicationentry implementation
*
*/


// INCLUDES
#include "javastoragenames.h"
#include "javauid.h"
#include "logger.h"
#include "writeablejavaregistryapplicationentry.h"
#include "writeablejavaregistrypackageentry.h"

using namespace Java::Manager::Registry;
using namespace java::storage;

// ============================ MEMBER FUNCTIONS =============================

CWriteableJavaRegistryApplicationEntry::CWriteableJavaRegistryApplicationEntry
(JavaStorageApplicationEntry_t& aEntry, TJavaRegistryEntryType aType) :
        CWriteableJavaRegistryEntry(aEntry , aType)
{
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::
//     ~CWriteableJavaRegistryApplicationEntry
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryApplicationEntry
::~CWriteableJavaRegistryApplicationEntry()
{
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::IsStartable
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CWriteableJavaRegistryApplicationEntry::IsStartable() const
{
    return ETrue;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::IsVisible
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CWriteableJavaRegistryApplicationEntry::IsVisible() const
{
    return ETrue;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::IsResident
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CWriteableJavaRegistryApplicationEntry::IsResident() const
{
    return ETrue;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::PackageEntryL
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryPackageEntry*
CWriteableJavaRegistryApplicationEntry::PackageEntryL() const
{
    JavaStorageApplicationEntry_t suiteEntry;
    std::wstring value;

    // Get suiteID where application belongs to.
    EntryAttributeValue(
        iEntry, PACKAGE_ID, value);

    java::util::Uid suiteUid(value);

    // Get suite information.
    StorageEntry(suiteUid, APPLICATION_PACKAGE_TABLE, suiteEntry);

    if (0 == suiteEntry.size())
    {
        ELOG(EJavaStorage, "Application does not have suite");
        User::Leave(KErrNotFound);
    }

    return new CWriteableJavaRegistryPackageEntry(
               suiteEntry, EMidp2MidletSuite);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::AttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C const MJavaAttribute* CWriteableJavaRegistryApplicationEntry::
AttributeL(const TDesC& aName) const
{
    JELOG2(EJavaStorage);
    return CWriteableJavaRegistryEntry::AttributeL(aName);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::AttributesL
// ---------------------------------------------------------------------------
//
EXPORT_C const RPointerArray<MJavaAttribute>&
CWriteableJavaRegistryApplicationEntry::AttributesL() const
{
    JELOG2(EJavaStorage);
    return CWriteableJavaRegistryEntry::AttributesL();
}
