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
* Description:  writeablejavaregistryapplicationentry implementation
*              : (__JAVA_EXCLUDED VERSION)
*
*/


// INCLUDES
#include "writeablejavaregistryapplicationentry.h"
#include "javaattributes.h"

using namespace Java;
using namespace Java::Manager::Registry;

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::~CWriteableJavaRegistryApplicationEntry
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryApplicationEntry::~CWriteableJavaRegistryApplicationEntry()
{
}
// CWriteableJavaRegistryApplicationEntry::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryApplicationEntry*
CWriteableJavaRegistryApplicationEntry::NewL(const TUid& /* aUid */)
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryApplicationEntry*
CWriteableJavaRegistryApplicationEntry::NewLC(const TUid& /* aUid */)
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::IsStartable
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CWriteableJavaRegistryApplicationEntry::IsStartable() const
{
    return EFalse; // ETrue ? regarding to the header file
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::IsVisible
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CWriteableJavaRegistryApplicationEntry::IsVisible() const
{
    return EFalse; // ETrue ? regarding to the header file
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::IsResident
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CWriteableJavaRegistryApplicationEntry::IsResident() const
{
    return EFalse;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::GroupName
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CWriteableJavaRegistryApplicationEntry::
GroupName() const
{
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::InstallPackageUid
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CWriteableJavaRegistryApplicationEntry::InstallPackageUid() const
{
    return TUid::Null();
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::SetStartableL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryApplicationEntry::
SetStartableL(TBool /* aStartable */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::SetVisibleL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryApplicationEntry::
SetVisibleL(TBool /* aVisible */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::SetResidentL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryApplicationEntry::
SetResidentL(TBool /* aResident */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::SetGroupNameL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryApplicationEntry::
SetGroupNameL(const TDesC& /* aGroupName */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::SetInstallPackageUidL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryApplicationEntry::
SetInstallPackageUidL(const TUid& /* aInstallPackage */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::SetPropertyL
// Takes ownership of aProperty
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryApplicationEntry::
SetPropertyL(CJavaProperty* /*aProperty*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::PackageEntryL
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryPackageEntry*
CWriteableJavaRegistryApplicationEntry::PackageEntryL() const
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::
//                                     CWriteableJavaRegistryApplicationEntry
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryApplicationEntry::
CWriteableJavaRegistryApplicationEntry(const TUid& /* aUid */)
        : CWriteableJavaRegistryEntry(TUid::Null())
{

}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::AttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C const MJavaAttribute* CWriteableJavaRegistryApplicationEntry::
AttributeL(const TDesC& /* aName */) const
{
    return NULL;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryApplicationEntry::AttributesL
// ---------------------------------------------------------------------------
//
EXPORT_C const RPointerArray<MJavaAttribute>&
CWriteableJavaRegistryApplicationEntry::AttributesL() const
{
    User::Leave(KErrNotSupported);
    return iAttributes;
}

