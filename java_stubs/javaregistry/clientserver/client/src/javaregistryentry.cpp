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
* Description:  javaregistryentry implementation
*
*/


#include "javaregistryentry.h"
#include "writeablejavaregistryentry.h"
#include "javaattributes.h"

using namespace Java;
using namespace Java::Manager::Registry;

// ============================ MEMBER FUNCTIONS ==============================
// ---------------------------------------------------------------------------
// CJavaRegistryEntry::CJavaRegistryEntry
// ---------------------------------------------------------------------------
//
CJavaRegistryEntry::CJavaRegistryEntry
(CWriteableJavaRegistryEntry* /* aWritableEntry */)
{
}

// ---------------------------------------------------------------------------
// CJavaRegistryEntry::Uid
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CJavaRegistryEntry::Uid() const
{
    return TUid::Null();
}

// ---------------------------------------------------------------------------
// CJavaRegistryEntry::Name
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CJavaRegistryEntry::Name() const
{
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CJavaRegistryEntry::Type
// ---------------------------------------------------------------------------
//
EXPORT_C TJavaRegistryEntryType CJavaRegistryEntry::Type() const
{
    return EGeneralApplication;
}


// ---------------------------------------------------------------------------
// CJavaRegistryEntry::DriveL
// ---------------------------------------------------------------------------
//
EXPORT_C TDriveNumber CJavaRegistryEntry::Drive() const
{
    return EDriveC;
}


// ---------------------------------------------------------------------------
// CJavaRegistryEntry::NumberOfCertificateChains
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CJavaRegistryEntry::NumberOfCertificateChains() const
{
    return 0;
}


// ---------------------------------------------------------------------------
// CJavaRegistryEntry::CertificateChain
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CJavaRegistryEntry::
CertificateChain(TInt /* aChain */) const
{
    return KNullDesC8;
}


// ---------------------------------------------------------------------------
// CJavaRegistryEntry::DrmContentId
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CJavaRegistryEntry::DrmContentId() const
{
    return KNullDesC;
}


// ---------------------------------------------------------------------------
// CJavaRegistryEntry::~CJavaRegistryEntry
// ---------------------------------------------------------------------------
//
CJavaRegistryEntry::~CJavaRegistryEntry()
{
}
// ---------------------------------------------------------------------------
// CJavaRegistryEntry::WritableEntry
// ---------------------------------------------------------------------------
//
CWriteableJavaRegistryEntry* CJavaRegistryEntry::WritableEntry() const
{
    return NULL;
}

// ---------------------------------------------------------------------------
// CJavaRegistryEntry::AttributeL
// ---------------------------------------------------------------------------
//

EXPORT_C const MJavaAttribute* CJavaRegistryEntry::AttributeL(const TDesC& /*aName*/) const
{
    return NULL;
}

// ---------------------------------------------------------------------------
// CJavaRegistryEntry::AttributesL
// ---------------------------------------------------------------------------
//
EXPORT_C const RPointerArray<MJavaAttribute>& CJavaRegistryEntry::AttributesL() const
{
    return iAttributes;
}

