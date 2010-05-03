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
* Description:  Specialized binary value class
 *
*/


// CLASS HEADER
#include "cpimbinaryvalue.h"
#include "logger.h"

// INTERNAL INCLUDES
#include "pimpanics.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPIMBinaryValue::NewL
// Default two-phase constructor
// ---------------------------------------------------------------------------
//
CPIMBinaryValue* CPIMBinaryValue::NewL(TPIMAttribute aAttributes,
                                       CPIMByteArray* aBinaryValue)
{
    JELOG2(EPim);
    CPIMBinaryValue* self =
        new(ELeave) CPIMBinaryValue(aAttributes, aBinaryValue);
    return self;
}

// ---------------------------------------------------------------------------
// CPIMBinaryValue::NewLC
// Default two-phase constructor. The newly created object is left to the
// cleanup stack
// ---------------------------------------------------------------------------
//
CPIMBinaryValue* CPIMBinaryValue::NewLC(TPIMAttribute aAttributes,
                                        CPIMByteArray* aBinaryValue)
{
    JELOG2(EPim);
    CPIMBinaryValue* self = CPIMBinaryValue::NewL(aAttributes, aBinaryValue);
    CleanupStack::PushL(self);
    return self;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPIMBinaryValue::~CPIMBinaryValue()
{
    JELOG2(EPim);
    delete iBinaryValue;
}

// ---------------------------------------------------------------------------
// CPIMBinaryValue::SetValue
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMBinaryValue::SetBinaryValue(const CPIMByteArray* aBinaryValue)
{
    JELOG2(EPim);
    // Remove existing value if one exists
    delete iBinaryValue;
    iBinaryValue = aBinaryValue;
}

// ---------------------------------------------------------------------------
// CPIMBinaryValue::Value
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
const CPIMByteArray* CPIMBinaryValue::BinaryValue() const
{
    JELOG2(EPim);
    __ASSERT_DEBUG(iBinaryValue, User::Panic(KPIMPanicCategory,
                   EPIMPanicNullArgument));
    return iBinaryValue;
}

// ---------------------------------------------------------------------------
// CPIMBinaryValue::CPIMBinaryValue
// Default C++ constructor
// ---------------------------------------------------------------------------
//
inline CPIMBinaryValue::CPIMBinaryValue(TPIMAttribute aAttributes,
                                        CPIMByteArray* aBinaryValue) :
        CPIMValueBase(aAttributes), iBinaryValue(aBinaryValue)
{
    JELOG2(EPim);
}

// End of file
