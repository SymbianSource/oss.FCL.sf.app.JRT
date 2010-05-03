/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CEntryLangDetails class.
*
*/


#include "entrylangdetails.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CEntryLangDetails::NewLC
// ---------------------------------------------------------------------------
//
CEntryLangDetails* CEntryLangDetails::NewLC(TLanguage aLang,
        const TDesC& aPackage,
        const TDesC& aVendor)
{
    CEntryLangDetails* self = new(ELeave) CEntryLangDetails(aLang);
    CleanupStack::PushL(self);
    self->ConstructL(aPackage, aVendor);
    return self;
}

// ---------------------------------------------------------------------------
// CEntryLangDetails::ConstructL
// ---------------------------------------------------------------------------
//
void CEntryLangDetails::ConstructL(const TDesC& /*aPackage*/,
                                   const TDesC& /*aVendor*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CEntryLangDetails::NewLC
// ---------------------------------------------------------------------------
//
CEntryLangDetails* CEntryLangDetails::NewLC(RReadStream& aStream)
{
    CEntryLangDetails* self = new(ELeave) CEntryLangDetails;
    CleanupStack::PushL(self);
    self->ConstructL(aStream);
    return self;
}

// ---------------------------------------------------------------------------
// CEntryLangDetails::ConstructL
// ---------------------------------------------------------------------------
//
void CEntryLangDetails::ConstructL(RReadStream& /*aStream*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CEntryLangDetails::NewLC
// ---------------------------------------------------------------------------
//
CEntryLangDetails* CEntryLangDetails::NewLC(const CEntryLangDetails& aOther)
{
    CEntryLangDetails* self =
        new(ELeave) CEntryLangDetails(aOther.iLanguage);
    CleanupStack::PushL(self);
    self->ConstructL(*aOther.iPackageName, *aOther.iVendorName);
    return self;
}

// ---------------------------------------------------------------------------
// CEntryLangDetails::CEntryLangDetails
// ---------------------------------------------------------------------------
//
CEntryLangDetails::CEntryLangDetails(TLanguage aLang)
        : iLanguage(aLang)
{
}

// ---------------------------------------------------------------------------
// CEntryLangDetails::~CEntryLangDetails
// ---------------------------------------------------------------------------
//
CEntryLangDetails::~CEntryLangDetails()
{
}

// ---------------------------------------------------------------------------
// CEntryLangDetails::Language
// ---------------------------------------------------------------------------
//
TLanguage CEntryLangDetails::Language() const
{
    return iLanguage;
}

// ---------------------------------------------------------------------------
// CEntryLangDetails::PackageName
// ---------------------------------------------------------------------------
//
const TDesC& CEntryLangDetails::PackageName() const
{
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CEntryLangDetails::VendorName
// ---------------------------------------------------------------------------
//
const TDesC& CEntryLangDetails::VendorName() const
{
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CEntryLangDetails::ExternalizeL
// ---------------------------------------------------------------------------
//
void CEntryLangDetails::ExternalizeL(RWriteStream& /*aStream*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CEntryLangDetails::SizeForStreamingL
// ---------------------------------------------------------------------------
//
TInt CEntryLangDetails::SizeForStreamingL() const
{
    User::Leave(KErrNotSupported);
    return 0;
}
