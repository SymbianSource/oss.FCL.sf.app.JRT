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
* Description:  Specialized integer value class
 *
*/


// CLASS HEADER
#include "cpimintvalue.h"
#include "logger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPIMIntValue::NewL
// Default two-phase constructor
// ---------------------------------------------------------------------------
//
CPIMIntValue* CPIMIntValue::NewL(TPIMAttribute aAttributes, TInt aIntegerValue)
{
    JELOG2(EPim);
    CPIMIntValue* self = new(ELeave) CPIMIntValue(aAttributes, aIntegerValue);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// ---------------------------------------------------------------------------
// CPIMIntValue::NewLC
// Default two-phase constructor. The newly created object is left to the
// cleanup stack
// ---------------------------------------------------------------------------
//
CPIMIntValue* CPIMIntValue::NewLC(TPIMAttribute aAttributes, TInt aIntegerValue)
{
    JELOG2(EPim);
    CPIMIntValue* self = CPIMIntValue::NewL(aAttributes, aIntegerValue);
    CleanupStack::PushL(self);
    return self;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPIMIntValue::~CPIMIntValue()
{
    JELOG2(EPim);
}

// ---------------------------------------------------------------------------
// CPIMIntValue::SetValue
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMIntValue::SetIntValue(TInt aIntegerValue)
{
    JELOG2(EPim);
    iIntegerValue = aIntegerValue;
}

// ---------------------------------------------------------------------------
// CPIMIntValue::Value
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TInt CPIMIntValue::IntValue() const
{
    JELOG2(EPim);
    return iIntegerValue;
}

// ---------------------------------------------------------------------------
// CPIMIntValue::CPIMIntValue
// Default C++ constructor
// ---------------------------------------------------------------------------
//
CPIMIntValue::CPIMIntValue(TPIMAttribute aAttributes, TInt aIntegerValue) :
        CPIMValueBase(aAttributes), iIntegerValue(aIntegerValue)
{
    JELOG2(EPim);
}

// ---------------------------------------------------------------------------
// CPIMIntValue::ConstructL
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CPIMIntValue::ConstructL()
{
    JELOG2(EPim);
}

// End of file
