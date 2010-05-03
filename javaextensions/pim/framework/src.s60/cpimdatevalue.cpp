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
* Description:  Specialized date value class
 *
*/


// CLASS HEADER
#include "cpimdatevalue.h"
#include "logger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPIMDateValue::NewL
// Default two-phase constructor
// ---------------------------------------------------------------------------
//
CPIMDateValue* CPIMDateValue::NewL(TPIMAttribute aAttributes,
                                   TPIMDate aDateValue)
{
    JELOG2(EPim);
    CPIMDateValue* self = new(ELeave) CPIMDateValue(aAttributes, aDateValue);

    return self;
}

// ---------------------------------------------------------------------------
// CPIMDateValue::NewLC
// Default two-phase constructor. The newly created object is left to the
// cleanup stack
// ---------------------------------------------------------------------------
//
CPIMDateValue* CPIMDateValue::NewLC(TPIMAttribute aAttributes,
                                    TPIMDate aDateValue)
{
    JELOG2(EPim);
    CPIMDateValue* self = CPIMDateValue::NewL(aAttributes, aDateValue);
    CleanupStack::PushL(self);
    return self;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPIMDateValue::~CPIMDateValue()
{
    JELOG2(EPim);
}

// ---------------------------------------------------------------------------
// CPIMDateValue::SetValue
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMDateValue::SetDateValue(TPIMDate aDateValue)
{
    JELOG2(EPim);
    iDateValue = aDateValue;
}

// ---------------------------------------------------------------------------
// CPIMDateValue::Value
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TPIMDate CPIMDateValue::DateValue() const
{
    JELOG2(EPim);
    return iDateValue;
}

// ---------------------------------------------------------------------------
// CPIMDateValue::CPIMDateValue
// Default C++ constructor
// ---------------------------------------------------------------------------
//
inline CPIMDateValue::CPIMDateValue(TPIMAttribute aAttributes,
                                    TPIMDate aDateValue) :
        CPIMValueBase(aAttributes), iDateValue(aDateValue)
{
    JELOG2(EPim);
}

// End of file
