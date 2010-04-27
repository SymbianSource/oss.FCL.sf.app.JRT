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
* Description:  Specialized boolean value class
 *
*/


// CLASS HEADER
#include "cpimbooleanvalue.h"
#include "logger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPIMBooleanValue::NewL
// Default two-phase constructor
// ---------------------------------------------------------------------------
//
CPIMBooleanValue* CPIMBooleanValue::NewL(TPIMAttribute aAttributes,
        TBool aBooleanValue)
{
    JELOG2(EPim);
    CPIMBooleanValue* self =
        new(ELeave) CPIMBooleanValue(aAttributes, aBooleanValue);
    return self;
}

// ---------------------------------------------------------------------------
// CPIMBooleanValue::NewLC
// Default two-phase constructor. The newly created object is left to the
// cleanup stack
// ---------------------------------------------------------------------------
//
CPIMBooleanValue* CPIMBooleanValue::NewLC(TPIMAttribute aAttributes,
        TBool aBooleanValue)
{
    JELOG2(EPim);
    CPIMBooleanValue* self =
        CPIMBooleanValue::NewL(aAttributes, aBooleanValue);
    CleanupStack::PushL(self);
    return self;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPIMBooleanValue::~CPIMBooleanValue()
{
    JELOG2(EPim);
}

// ---------------------------------------------------------------------------
// CPIMBooleanValue::SetValue
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMBooleanValue::SetBooleanValue(TBool aBooleanValue)
{
    JELOG2(EPim);
    iBooleanValue = aBooleanValue;
}

// ---------------------------------------------------------------------------
// CPIMBooleanValue::Value
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TBool CPIMBooleanValue::BooleanValue() const
{
    JELOG2(EPim);
    return iBooleanValue;
}

// ---------------------------------------------------------------------------
// CPIMBooleanValue::CPIMBooleanValue
// Default C++ constructor
// ---------------------------------------------------------------------------
//
inline CPIMBooleanValue::CPIMBooleanValue(TPIMAttribute aAttributes,
        TBool aBooleanValue) :
        CPIMValueBase(aAttributes), iBooleanValue(aBooleanValue)
{
    JELOG2(EPim);
}

// End of file
