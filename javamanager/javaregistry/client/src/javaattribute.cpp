/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CJavaAttribute implementation
*
*/


#include "javaattribute.h"

using namespace Java;

// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CJavaAttribute::NewL
// Constructs a CJavaAttribute object.
// ---------------------------------------------------------------------------
EXPORT_C CJavaAttribute* CJavaAttribute::
NewL(const TDesC& aName, const TDesC& aValue, TBool aTrusted)
{
    CJavaAttribute* self = CJavaAttribute::NewLC(aName, aValue, aTrusted);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CJavaAttribute::NewLC
// Constructs a CJavaAttribute object.
// ---------------------------------------------------------------------------
EXPORT_C CJavaAttribute* CJavaAttribute::
NewLC(const TDesC& aName, const TDesC& aValue, TBool aTrusted)
{
    if (aName == KNullDesC || aValue == KNullDesC)
    {
        User::Leave(KErrArgument);
    }
    CJavaAttribute* self = new(ELeave) CJavaAttribute;
    CleanupStack::PushL(self);
    self->ConstructL(aName, aValue, aTrusted);
    return self;
}

// ---------------------------------------------------------------------------
// CJavaAttribute::ConstructL
// ---------------------------------------------------------------------------
void CJavaAttribute::
ConstructL(const TDesC& aName, const TDesC& aValue, TBool aTrusted)
{
    iName = aName.AllocL();
    iValue = aValue.AllocL();
    iTrusted = aTrusted;
}

// ---------------------------------------------------------------------------
// CJavaAttribute::Name
// Returns name of attribute
// ---------------------------------------------------------------------------
const TDesC& CJavaAttribute::Name() const
{
    return *iName;
}

// ---------------------------------------------------------------------------
// CJavaAttribute::Value
// Returns attribute's value
// ---------------------------------------------------------------------------
const TDesC& CJavaAttribute::Value() const
{
    return *iValue;
}

// ---------------------------------------------------------------------------
// CJavaAttribute::Trusted
// Returns if attribute is trusted
// ---------------------------------------------------------------------------
TBool CJavaAttribute::Trusted() const
{
    return iTrusted;
}

// ---------------------------------------------------------------------------
// CJavaAttribute::~MJavaAttribute
// ---------------------------------------------------------------------------
CJavaAttribute::~CJavaAttribute()
{
    delete iName;
    delete iValue;
}



// End of File
