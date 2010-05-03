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
* Description:  javaregistryapplicationentry implementation
*
*/


#include "javaregistrypackageentry.h"
#include "javaregistryapplicationentry.h"
#include "writeablejavaregistryapplicationentry.h"
#include "logger.h"

using namespace Java;
using namespace Java::Manager::Registry;


// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CJavaRegistryApplicationEntry::CJavaRegistryApplicationEntry
// ---------------------------------------------------------------------------
//
CJavaRegistryApplicationEntry::CJavaRegistryApplicationEntry
(CWriteableJavaRegistryApplicationEntry* aWritableEntry) :
        CJavaRegistryEntry(aWritableEntry)
{
}

// ---------------------------------------------------------------------------
// CJavaRegistryApplicationEntry::IsStartable
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CJavaRegistryApplicationEntry::IsStartable() const
{
    return WritableEntry()->IsStartable();
}

// ---------------------------------------------------------------------------
// CJavaRegistryApplicationEntry::IsVisible
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CJavaRegistryApplicationEntry::IsVisible() const
{
    return WritableEntry()->IsVisible();
}

// ---------------------------------------------------------------------------
// CJavaRegistryApplicationEntry::IsResident
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CJavaRegistryApplicationEntry::IsResident() const
{
    return WritableEntry()->IsResident();
}

// ---------------------------------------------------------------------------
// CJavaRegistryApplicationEntry::GroupName
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CJavaRegistryApplicationEntry::GroupName() const
{
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CJavaRegistryApplicationEntry::PackageEntryL
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistryPackageEntry* CJavaRegistryApplicationEntry::
PackageEntryL() const
{
    CWriteableJavaRegistryPackageEntry* writeablePackage
    = WritableEntry()->PackageEntryL();

    if (writeablePackage == NULL)
    {
        WLOG(EJavaStorage,
             "Can't find the PackageEntry, returning NULL.");
        return NULL;
    }
    CleanupStack::PushL(writeablePackage);

    CJavaRegistryPackageEntry* package
    = new(ELeave) CJavaRegistryPackageEntry(writeablePackage);

    CleanupStack::Pop(writeablePackage);
    return package;
}

// ---------------------------------------------------------------------------
// CJavaRegistryApplicationEntry::WritableEntry
// ---------------------------------------------------------------------------
//
CWriteableJavaRegistryApplicationEntry* CJavaRegistryApplicationEntry::
WritableEntry() const
{
    return (CWriteableJavaRegistryApplicationEntry*)iWritableEntry;
}
