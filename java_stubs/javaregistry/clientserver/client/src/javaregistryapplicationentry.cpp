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
* Description:  javaregistryapplicationentry implementation
*
*/


#include "javaregistrypackageentry.h"
#include "javaregistryapplicationentry.h"
#include "writeablejavaregistryapplicationentry.h"

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
    return ETrue;
}


// ---------------------------------------------------------------------------
// CJavaRegistryApplicationEntry::IsVisible
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CJavaRegistryApplicationEntry::IsVisible() const
{
    return ETrue;
}


// ---------------------------------------------------------------------------
// CJavaRegistryApplicationEntry::IsResident
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CJavaRegistryApplicationEntry::IsResident() const
{
    return EFalse;
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
    User::Leave(KErrNotSupported);
    return NULL;
}
