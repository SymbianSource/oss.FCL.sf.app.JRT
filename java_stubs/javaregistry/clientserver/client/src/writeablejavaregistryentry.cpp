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
* Description:  writeablejavaregistryentry implementation
*              : (__JAVA_EXCLUDED VERSION)
*
*/


// INCLUDES
#include "writeablejavaregistryentry.h"

using namespace Java;
using namespace Java::Manager::Registry;

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::~CWriteableJavaRegistryEntry
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryEntry::~CWriteableJavaRegistryEntry()
{
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::Uid
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CWriteableJavaRegistryEntry::Uid() const
{
    return TUid::Null();
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::Name
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CWriteableJavaRegistryEntry::Name() const
{
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::Type
// ---------------------------------------------------------------------------
//
EXPORT_C Java::TJavaRegistryEntryType CWriteableJavaRegistryEntry::
Type() const
{
    return Java::TJavaRegistryEntryType();
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::Drive
// ---------------------------------------------------------------------------
//
EXPORT_C TDriveNumber CWriteableJavaRegistryEntry::Drive() const
{
    return EDriveC;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::NumberOfCertificateChains
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CWriteableJavaRegistryEntry::NumberOfCertificateChains() const
{
    return 0;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::CertificateChain
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CWriteableJavaRegistryEntry::
CertificateChain(TInt /* aIndex */) const
{
    return KNullDesC8;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::DrmContentId
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CWriteableJavaRegistryEntry::DrmContentId() const
{
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::SetNameL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryEntry::SetNameL(const TDesC& /* aName */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::SetTypeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryEntry::
SetTypeL(Java::TJavaRegistryEntryType /* aType */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::SetDriveL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryEntry::
SetDriveL(const TDriveNumber& /* aDrive */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::AddCertificateChainL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryEntry::
AddCertificateChainL(const TDesC8& /* aChain */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::SetDrmContentIdL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryEntry::
SetDrmContentIdL(const TDesC& /* aDrmConte ntId */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::Property
// ---------------------------------------------------------------------------
//
EXPORT_C const CJavaProperty* CWriteableJavaRegistryEntry::
Property(TInt32 /* aId */) const
{
    return NULL;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::SetPropertyL
// Takes ownership of aProperty
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryEntry::
SetPropertyL(CJavaProperty* /* aProperty */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::SetPropertyL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryEntry::
SetPropertyL(TInt32 /* aId */, const TDesC& /* aValue */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::CWriteableJavaRegistryEntry
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryEntry::
CWriteableJavaRegistryEntry(const TUid& /* aUid */)
{
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryEntry::
ConstructL(Java::TJavaRegistryEntryType /* aType */)
{
    User::Leave(KErrNotSupported);
}


// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::IntegerProperty
// ---------------------------------------------------------------------------
//
EXPORT_C TInt32 CWriteableJavaRegistryEntry::
IntegerProperty(TInt32 /* aId */, TInt32 /* aDefault */) const
{
    return 0;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::BooleanProperty
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CWriteableJavaRegistryEntry::
BooleanProperty(TInt32 /* aId */, TBool /* aDefault */) const
{
    return EFalse;
}


// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::SetBooleanPropertyL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryEntry::
SetBooleanPropertyL(TInt32 /* aId */, TBool /* aBoolValue */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::SetDescriptorPropertyL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryEntry::
SetDescriptorPropertyL(TInt32 /* aId */, const TDesC& /* aDescValue */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::SetIntegerPropertyL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryEntry::
SetIntegerPropertyL(TInt32 /* aId */, TInt32 /* aIntValue */)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::DeleteProperty
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryEntry::DeleteProperty(TInt /* aIndex */)
{
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::GetPropertyIndex
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CWriteableJavaRegistryEntry::
GetPropertyIndex(TInt32 /* aId */) const
{
    return -1;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::AttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C const MJavaAttribute* CWriteableJavaRegistryEntry::
AttributeL(const TDesC& /* aName */) const
{
    return NULL;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::AttributesL
// ---------------------------------------------------------------------------
//
EXPORT_C const RPointerArray<MJavaAttribute>&
CWriteableJavaRegistryEntry::AttributesL() const
{
    return iAttributes;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::SerializeAttributesL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryEntry::SerializeAttributesL()
{
}
