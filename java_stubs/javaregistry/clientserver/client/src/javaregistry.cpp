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
* Description:  javaregistry implementation
*
*/


#include "javaregistry.h"

using namespace Java;
using namespace Java::Manager::Registry;

// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CJavaRegistry::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistry* CJavaRegistry::NewL()
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CJavaRegistry::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistry* CJavaRegistry::NewLC()
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CJavaRegistry::RegistryEntryExistsL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CJavaRegistry::RegistryEntryExistsL(const TUid& /*aUid*/) const
{
    User::Leave(KErrNotSupported);
    return EFalse;
}

// ---------------------------------------------------------------------------
// CJavaRegistry::GetRegistryEntryUidsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CJavaRegistry::GetRegistryEntryUidsL(RArray<TUid>& /*aUids*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CJavaRegistry::GetRegistryEntryUidsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CJavaRegistry::GetRegistryEntryUidsL
(TJavaRegistryEntryType /*aType*/, RArray<TUid>& /*aUids*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CJavaRegistry::RegistryEntryL
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistryEntry* CJavaRegistry::
RegistryEntryL(const TUid& /*aUid*/) const
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CJavaRegistry::~CJavaRegistry
// ---------------------------------------------------------------------------
//
CJavaRegistry::~CJavaRegistry()
{
}

// ---------------------------------------------------------------------------
// CJavaRegistry::CJavaRegistry
// ---------------------------------------------------------------------------
//
CJavaRegistry::CJavaRegistry()
{
}
