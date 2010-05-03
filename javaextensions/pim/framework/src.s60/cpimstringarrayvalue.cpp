/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Specialized string array value class
 *
*/


// CLASS HEADER
#include "cpimstringarrayvalue.h"

// INTERNAL INCLUDES
#include "pimpanics.h"
#include "logger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPIMStringArrayValue::NewL
// Default two-phase constructor
// ---------------------------------------------------------------------------
//
CPIMStringArrayValue* CPIMStringArrayValue::NewL(TPIMAttribute aAttributes,
        CDesCArray* aStringArrayValue)
{
    JELOG2(EPim);
    CPIMStringArrayValue* self =
        new(ELeave) CPIMStringArrayValue(aAttributes, aStringArrayValue);
    return self;
}

// ---------------------------------------------------------------------------
// CPIMStringArrayValue::NewLC
// Default two-phase constructor. The newly created object is left to the
// cleanup stack
// ---------------------------------------------------------------------------
//
CPIMStringArrayValue* CPIMStringArrayValue::NewLC(TPIMAttribute aAttributes,
        CDesCArray* aStringArrayValue)
{
    JELOG2(EPim);
    CPIMStringArrayValue* self = CPIMStringArrayValue::NewL(aAttributes,
                                 aStringArrayValue);

    CleanupStack::PushL(self);
    return self;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPIMStringArrayValue::~CPIMStringArrayValue()
{
    JELOG2(EPim);
    delete iStringArrayValue;
}

// ---------------------------------------------------------------------------
// CPIMStringArrayValue::SetValue
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMStringArrayValue::SetStringArrayValue(
    const CDesCArray* aStringArrayValue)
{
    JELOG2(EPim);
    // Remove existing value if one exists
    delete iStringArrayValue;
    iStringArrayValue = aStringArrayValue;
}

// ---------------------------------------------------------------------------
// CPIMStringArrayValue::Value
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
const CDesCArray* CPIMStringArrayValue::StringArrayValue() const
{
    JELOG2(EPim);
    __ASSERT_DEBUG(iStringArrayValue, User::Panic(KPIMPanicCategory,
                   EPIMPanicNullArgument));
    return iStringArrayValue;
}

TInt CPIMStringArrayValue::NumElements() const
{
    JELOG2(EPim);
    __ASSERT_DEBUG(iStringArrayValue, User::Panic(KPIMPanicCategory,
                   EPIMPanicNullArgument));
    return iStringArrayValue->MdcaCount();
}

// ---------------------------------------------------------------------------
// CPIMStringArrayValue::CPIMStringArrayValue
// Default C++ constructor
// ---------------------------------------------------------------------------
//
inline CPIMStringArrayValue::CPIMStringArrayValue(TPIMAttribute aAttributes,
        CDesCArray* aStringArrayValue) :
        CPIMValueBase(aAttributes), iStringArrayValue(aStringArrayValue)
{
    JELOG2(EPim);
}

// End of file
