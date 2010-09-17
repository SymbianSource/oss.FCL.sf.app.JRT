/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CJavaAttribute stub.
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
NewL(const TDesC& /*aName*/, const TDesC& /*aValue*/, TBool /*aTrusted*/)
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CJavaAttribute::NewLC
// Constructs a CJavaAttribute object.
// ---------------------------------------------------------------------------
EXPORT_C CJavaAttribute* CJavaAttribute::
NewLC(const TDesC& /*aName*/, const TDesC& /*aValue*/, TBool /*aTrusted*/)
{
    User::Leave(KErrNotSupported);
    return NULL;
}

// ---------------------------------------------------------------------------
// CJavaAttribute::~MJavaAttribute
// ---------------------------------------------------------------------------
EXPORT_C CJavaAttribute::~CJavaAttribute()
{
}

// ---------------------------------------------------------------------------
// CJavaAttribute::Name
// Returns name of attribute
// ---------------------------------------------------------------------------
const TDesC& CJavaAttribute::Name() const
{
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CJavaAttribute::Value
// Returns attribute's value
// ---------------------------------------------------------------------------
const TDesC& CJavaAttribute::Value() const
{
    return KNullDesC;
}

// ---------------------------------------------------------------------------
// CJavaAttribute::Trusted
// Returns if attribute is trusted
// ---------------------------------------------------------------------------
TBool CJavaAttribute::Trusted() const
{
    return false;
}


// End of File
