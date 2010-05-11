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
* Description:  javaregistry implementation
 *
*/


#include "javaregistry.h"
#include "javaregistryapplicationentry.h"
#include "javaregistryentry.h"
#include "javaregistrypackageentry.h"
#include "logger.h"
#include "writeablejavaregistry.h"
#include "writeablejavaregistryentry.h"

using namespace Java;
using namespace Java::Manager::Registry;
// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CJavaRegistry::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistry* CJavaRegistry::NewL()
{
    JELOG2(EJavaStorage);

    CJavaRegistry* self = CJavaRegistry::NewLC();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CJavaRegistry::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistry* CJavaRegistry::NewLC()
{
    JELOG2(EJavaStorage);

    CJavaRegistry* self = new(ELeave) CJavaRegistry();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// ---------------------------------------------------------------------------
// CJavaRegistry::RegistryEntryExistsL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CJavaRegistry::RegistryEntryExistsL(const TUid& aUid) const
{
    return iJavaReg->RegistryEntryExistsL(aUid);
}

// ---------------------------------------------------------------------------
// CJavaRegistry::GetRegistryEntryUidsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CJavaRegistry::GetRegistryEntryUidsL(RArray<TUid>& aUids) const
{
    iJavaReg->GetRegistryEntryUidsL(aUids);
}

// ---------------------------------------------------------------------------
// CJavaRegistry::GetRegistryEntryUidsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CJavaRegistry::GetRegistryEntryUidsL(
    TJavaRegistryEntryType aType, RArray<TUid>& aUids) const
{
    JELOG2(EJavaStorage);
    iJavaReg->GetRegistryEntryUidsL(aType, aUids);
}

// ---------------------------------------------------------------------------
// CJavaRegistry::RegistryEntryL
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistryEntry* CJavaRegistry::RegistryEntryL(
    const TUid& aUid) const
{
    JELOG2(EJavaStorage);
    if (0 == aUid.iUid)
    {
        ILOG(EJavaStorage,
             "Can't find entry for uid 0, returning NULL.");
        return NULL;
    }

    CWriteableJavaRegistryEntry* writableEntry
    = iJavaReg->RegistryEntryL(aUid);

    if (writableEntry == NULL)
    {
        ILOG(EJavaStorage,
             "Can't find entry for the given uid, returning NULL.");
        return NULL;
    }

    CleanupStack::PushL(writableEntry);

    CJavaRegistryEntry* regEntry = NULL;
    TJavaRegistryEntryType entryType = writableEntry->Type();

    if (EGeneralPackage <= entryType && entryType < EGeneralApplication)
    {
        CWriteableJavaRegistryPackageEntry* writablePackageEntry
        = (CWriteableJavaRegistryPackageEntry*) writableEntry;

        regEntry = new(ELeave)
        CJavaRegistryPackageEntry(writablePackageEntry);
        // pointer ownership passed over
        ILOG(EJavaStorage, "PackageEntry created");
    }
    else if (EGeneralApplication <= entryType)
    {
        CWriteableJavaRegistryApplicationEntry* writableAppEntry
        = (CWriteableJavaRegistryApplicationEntry*) writableEntry;

        regEntry = new(ELeave)
        CJavaRegistryApplicationEntry(writableAppEntry);
        // pointer ownership passed over
        ILOG(EJavaStorage, "ApplicationEntry created");
    }
    else
    {
        ELOG1(EJavaStorage, "Invalid type, returning NULL: %d", entryType);
        CleanupStack::PopAndDestroy(writableEntry);
        return NULL;
    }

    CleanupStack::Pop(writableEntry);
    return regEntry;
}

// ---------------------------------------------------------------------------
// CJavaRegistry::~CJavaRegistry
// ---------------------------------------------------------------------------
//
CJavaRegistry::~CJavaRegistry()
{
    JELOG2(EJavaStorage);
    if (iJavaReg)
    {
        delete iJavaReg;
        iJavaReg = NULL;
    }
}

// ---------------------------------------------------------------------------
// CJavaRegistry::CJavaRegistry
// ---------------------------------------------------------------------------
//
CJavaRegistry::CJavaRegistry()
{
}

// ---------------------------------------------------------------------------
// CJavaRegistry::ConstructL
// ---------------------------------------------------------------------------
//
void CJavaRegistry::ConstructL()
{
    JELOG2(EJavaStorage);
    iJavaReg = CWriteableJavaRegistry::NewL(EFalse);
}
