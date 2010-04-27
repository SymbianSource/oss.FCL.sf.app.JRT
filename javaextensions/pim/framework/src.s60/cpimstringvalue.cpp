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
* Description:  Specialized string value class
 *
*/


// CLASS HEADER
#include "cpimstringvalue.h"

// INTERNAL INCLUDES
#include "pimpanics.h"
#include "logger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPIMStringValue::NewL
// Default two-phase constructor
// ---------------------------------------------------------------------------
//
CPIMStringValue* CPIMStringValue::NewL(TPIMAttribute aAttributes,
                                       HBufC* aStringValue)
{
    JELOG2(EPim);
    CPIMStringValue* self =
        new(ELeave) CPIMStringValue(aAttributes, aStringValue);

    return self;
}

// ---------------------------------------------------------------------------
// CPIMStringValue::NewLC
// Default two-phase constructor. The newly created object is left to the
// cleanup stack
// ---------------------------------------------------------------------------
//
CPIMStringValue* CPIMStringValue::NewLC(TPIMAttribute aAttributes,
                                        HBufC* aStringValue)
{
    JELOG2(EPim);
    CPIMStringValue* self = CPIMStringValue::NewL(aAttributes, aStringValue);
    CleanupStack::PushL(self);
    return self;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPIMStringValue::~CPIMStringValue()
{
    JELOG2(EPim);
    delete iStringValue;
}

// ---------------------------------------------------------------------------
// CPIMStringValue::SetValue
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
void CPIMStringValue::SetStringValue(const HBufC* aStringValue)
{
    JELOG2(EPim);
    // Remove existing value if there is one
    delete iStringValue;
    iStringValue = aStringValue;
}

// ---------------------------------------------------------------------------
// CPIMStringValue::Value
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
const HBufC* CPIMStringValue::StringValue() const
{
    JELOG2(EPim);
    __ASSERT_DEBUG(iStringValue, User::Panic(KPIMPanicCategory,
                   EPIMPanicNullArgument));
    return iStringValue;
}

// ---------------------------------------------------------------------------
// CPIMStringValue::CPIMStringValue
// Default C++ constructor
// ---------------------------------------------------------------------------
//
inline CPIMStringValue::CPIMStringValue(TPIMAttribute aAttributes,
                                        HBufC* aStringValue) :
        CPIMValueBase(aAttributes), iStringValue(aStringValue)
{
    JELOG2(EPim);
}

// End of file
