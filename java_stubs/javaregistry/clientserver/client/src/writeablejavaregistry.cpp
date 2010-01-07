/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  writeablejavaregistry implementation
*              : (__JAVA_EXCLUDED VERSION)
*
*/


// INCLUDES
#include "writeablejavaregistry.h"

using namespace Java::Manager::Registry;

// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistry* CWriteableJavaRegistry::
NewL(TBool /* aUseIntegrity */)
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistry* CWriteableJavaRegistry::
NewLC(TBool /* aUseIntegrity */)
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistry* CWriteableJavaRegistry::
NewL(TInt64 /* aIntegritySessionId */)
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistry* CWriteableJavaRegistry::
NewLC(TInt64 /* aIntegritySessionId */)
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::~CWriteableJavaRegistry
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistry::~CWriteableJavaRegistry()
{
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::GenerateUidL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistry::GenerateUidsL(
    RArray<TUid>& /* aUids */,
    TInt /* aNumber */) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::RegistryEntryExistsL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CWriteableJavaRegistry::
RegistryEntryExistsL(const TUid& /* aUid */) const
{
    User::Leave(KErrNotSupported);
    return EFalse;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::GetRegistryEntryUidsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistry::
GetRegistryEntryUidsL(RArray<TUid>& /* aUids */) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::GetRegistryEntryUidsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistry::
GetRegistryEntryUidsL(TJavaRegistryEntryType /* aType */,
                      RArray<TUid>& /* aUids */,
                      TBool /*aAllEntries*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::GetRegistryEntryUidsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistry::GetRegistryEntryUidsL(
    const RPointerArray<CJavaProperty>& /* aProperties */,
    RArray<TUid>& /* aUids */, TBool /*aAllEntries*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::RegistryEntryL
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryEntry* CWriteableJavaRegistry::
RegistryEntryL(const TUid& /* aUid */,
               TBool /*aAllEntries*/) const
{
    User::Leave(KErrNotSupported);
    return NULL;
}


// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::SetRegistryEntryL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistry::
SetRegistryEntryL(CWriteableJavaRegistryEntry& /* aEntry */) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::RemoveRegistryEntryL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistry::
RemoveRegistryEntryL(const TUid& /* aUid */,
                     TBool /* aEmbeddedEntries */,
                     TBool /* aAllEntries */) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::CWriteableJavaRegistry
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistry::CWriteableJavaRegistry()
{
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistry::
ConstructL(TBool /* aUseIntegrity */,
           TBool /* aLegacy */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistry::
ConstructL(TInt64 /* aIntegritySessionId */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::DecideEntryTypeAndCreateL
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryEntry*
CWriteableJavaRegistry::DecideEntryTypeAndCreateL
(const TUid& /* aUid */,
 CJavaPropertyArray* /* aPropArray */) const
{
    User::Leave(KErrNotSupported);
    return NULL;
}
