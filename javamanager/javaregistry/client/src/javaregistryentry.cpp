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
* Description:  javaregistryentry implementation
*
*/


#include "javaregistryentry.h"
#include "logger.h"
#include "writeablejavaregistryentry.h"

using namespace Java;
using namespace Java::Manager::Registry;

// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CJavaRegistryEntry::CJavaRegistryEntry
// ---------------------------------------------------------------------------
//
CJavaRegistryEntry::CJavaRegistryEntry
(CWriteableJavaRegistryEntry* aWritableEntry) :
        iWritableEntry(aWritableEntry)
{
}

// ---------------------------------------------------------------------------
// CJavaRegistryEntry::Uid
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CJavaRegistryEntry::Uid() const
{
    JELOG2(EJavaStorage);
    return WritableEntry()->Uid();
}

// ---------------------------------------------------------------------------
// CJavaRegistryEntry::Name
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CJavaRegistryEntry::Name() const
{
    JELOG2(EJavaStorage);
    return WritableEntry()->Name();
}

// ---------------------------------------------------------------------------
// CJavaRegistryEntry::Type
// ---------------------------------------------------------------------------
//
EXPORT_C TJavaRegistryEntryType CJavaRegistryEntry::Type() const
{
    JELOG2(EJavaStorage);
    return WritableEntry()->Type();
}


// ---------------------------------------------------------------------------
// CJavaRegistryEntry::DriveL
// ---------------------------------------------------------------------------
//
EXPORT_C TDriveNumber CJavaRegistryEntry::Drive() const
{
    JELOG2(EJavaStorage);
    return WritableEntry()->Drive();
}


// ---------------------------------------------------------------------------
// CJavaRegistryEntry::NumberOfCertificateChains
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CJavaRegistryEntry::NumberOfCertificateChains() const
{
    JELOG2(EJavaStorage);
    return WritableEntry()->NumberOfCertificateChains();
}


// ---------------------------------------------------------------------------
// CJavaRegistryEntry::CertificateChain
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CJavaRegistryEntry::CertificateChain(
    TInt aChain) const
{
    JELOG2(EJavaStorage);
    return WritableEntry()->CertificateChain(aChain);
}


// ---------------------------------------------------------------------------
// CJavaRegistryEntry::DrmContentId
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CJavaRegistryEntry::DrmContentId() const
{
    JELOG2(EJavaStorage);
    return WritableEntry()->DrmContentId();
}


// ---------------------------------------------------------------------------
// CJavaRegistryEntry::~CJavaRegistryEntry
// ---------------------------------------------------------------------------
//
EXPORT_C CJavaRegistryEntry::~CJavaRegistryEntry()
{
    if (iWritableEntry)
    {
        delete iWritableEntry;
        iWritableEntry = NULL;
    }
}

// ---------------------------------------------------------------------------
// CJavaRegistryEntry::WritableEntry
// ---------------------------------------------------------------------------
//
CWriteableJavaRegistryEntry* CJavaRegistryEntry::WritableEntry() const
{
    return iWritableEntry;
}

// ---------------------------------------------------------------------------
// CJavaRegistryEntry::AttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C const MJavaAttribute* CJavaRegistryEntry::AttributeL(
    const TDesC& aName) const
{
    JELOG2(EJavaStorage);
    return WritableEntry()->AttributeL(aName);
}

// ---------------------------------------------------------------------------
// CJavaRegistryEntry::AttributesL
// ---------------------------------------------------------------------------
//
EXPORT_C const RPointerArray<MJavaAttribute>& CJavaRegistryEntry
::AttributesL() const
{
    JELOG2(EJavaStorage);
    return WritableEntry()->AttributesL();
}
