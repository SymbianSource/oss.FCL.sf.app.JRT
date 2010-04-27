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
* Description:  Implementation of CPropertyValuePair class.
*
*/


#include "PropertyValuePair.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPropertyValuePair::NewLC
// ---------------------------------------------------------------------------
//
CPropertyValuePair* CPropertyValuePair::NewLC(TUid aPropertyId,
        const TDesC8& aValue)
{
    CPropertyValuePair* self = new(ELeave) CPropertyValuePair(aPropertyId);
    CleanupStack::PushL(self);
    self->ConstructL(aValue);
    return self;
}

// ---------------------------------------------------------------------------
// CPropertyValuePair::ConstructL
// ---------------------------------------------------------------------------
//
void CPropertyValuePair::ConstructL(const TDesC8& /*aValue*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CPropertyValuePair::NewLC
// ---------------------------------------------------------------------------
//
CPropertyValuePair* CPropertyValuePair::NewLC(RReadStream& aStream)
{
    CPropertyValuePair* self = new(ELeave) CPropertyValuePair;
    CleanupStack::PushL(self);
    self->ConstructL(aStream);
    return self;
}

// ---------------------------------------------------------------------------
// CPropertyValuePair::ConstructL
// ---------------------------------------------------------------------------
//
void CPropertyValuePair::ConstructL(RReadStream& /*aStream*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CPropertyValuePair::NewLC
// ---------------------------------------------------------------------------
//
CPropertyValuePair* CPropertyValuePair::NewLC
(const CPropertyValuePair& aOther)
{
    CPropertyValuePair* self =
        new(ELeave) CPropertyValuePair(aOther.iPropertyId);
    CleanupStack::PushL(self);
    self->ConstructL(*aOther.iValue);
    return self;
}

// ---------------------------------------------------------------------------
// CPropertyValuePair::CPropertyValuePair
// ---------------------------------------------------------------------------
//
CPropertyValuePair::CPropertyValuePair(TUid aPropertyId)
        : iPropertyId(aPropertyId)
{
}

// ---------------------------------------------------------------------------
// CPropertyValuePair::~CPropertyValuePair
// ---------------------------------------------------------------------------
//
CPropertyValuePair::~CPropertyValuePair()
{
}

// ---------------------------------------------------------------------------
// CPropertyValuePair::PropertyId
// ---------------------------------------------------------------------------
//
TUid CPropertyValuePair::PropertyId() const
{
    return TUid::Null();
}

// ---------------------------------------------------------------------------
// CPropertyValuePair::Value
// ---------------------------------------------------------------------------
//
const TDesC8& CPropertyValuePair::Value() const
{
    return KNullDesC8;
}

// ---------------------------------------------------------------------------
// CPropertyValuePair::SetValueL
// ---------------------------------------------------------------------------
//
void CPropertyValuePair::SetValueL(const TDesC8& /*aValue*/)
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CPropertyValuePair::ExternalizeL
// ---------------------------------------------------------------------------
//
void CPropertyValuePair::ExternalizeL(RWriteStream& /*aStream*/) const
{
    User::Leave(KErrNotSupported);
}

// ---------------------------------------------------------------------------
// CPropertyValuePair::SizeForStreamingL
// ---------------------------------------------------------------------------
//
TInt CPropertyValuePair::SizeForStreamingL() const
{
    User::Leave(KErrNotSupported);
    return 0;
}
