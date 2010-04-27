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
* Description:  javaregproperty implementation
*
*/


#include "javaregproperty.h"

using namespace Java::Manager::Registry;

// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CJavaProperty::NewL
// Constructs a java property object.
// ---------------------------------------------------------------------------

EXPORT_C CJavaProperty* CJavaProperty::NewL(TInt32 aId, const TDesC&  aValue)
{
    CJavaProperty*  self = NewLC(aId, aValue);
    CleanupStack::Pop();
    return self;
}


// ---------------------------------------------------------------------------
// CJavaProperty::NewLC
// Constructs a java property object.
// ---------------------------------------------------------------------------
EXPORT_C CJavaProperty* CJavaProperty::NewLC(TInt32 aId, const TDesC&  aValue)
{
    CJavaProperty*  self = new(ELeave) CJavaProperty();
    CleanupStack::PushL(self);
    self->ConstructL(aId, aValue);
    return self;
}


// ---------------------------------------------------------------------------
// CJavaProperty::ConstructL
// ---------------------------------------------------------------------------
void CJavaProperty::ConstructL(TInt32 aId, const TDesC&  aValue)
{
    iId = aId;
    iValue = aValue.AllocL();
}


// ---------------------------------------------------------------------------
// CJavaProperty::Id
// Returns property ID.
// ---------------------------------------------------------------------------
EXPORT_C TInt32 CJavaProperty::Id() const
{
    return iId;
}


// ---------------------------------------------------------------------------
// CJavaProperty::Value
// Returns property value.
// ---------------------------------------------------------------------------
EXPORT_C const TDesC& CJavaProperty::Value() const
{
    return *iValue;
}


// ---------------------------------------------------------------------------
// operator==
// ---------------------------------------------------------------------------
EXPORT_C TBool CJavaProperty::operator==(const CJavaProperty& aProp)
{
    return ((iId == aProp.Id()) &&
            (iValue->Compare(aProp.Value()) == 0));
}


// ---------------------------------------------------------------------------
// CJavaProperty::~CJavaProperty
// Destructor
// ---------------------------------------------------------------------------
EXPORT_C CJavaProperty::~CJavaProperty()
{
    delete iValue;
}

