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
* Description:  Class for holding PIM field data
 *
*/


// CLASS HEADER
#include "tpimfielddata.h"

// INTERNAL INCLUDES
#include "pimpanics.h"

// ---------------------------------------------------------------------------
// TPIMFieldData::Field
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
TPIMField TPIMFieldData::Field() const
{
    return iField;
}

// ---------------------------------------------------------------------------
// TPIMFieldData::FieldDataType
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
TPIMFieldDataType TPIMFieldData::FieldDataType() const
{
    return iDataType;
}

// ---------------------------------------------------------------------------
// TPIMFieldData::Index
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
TInt TPIMFieldData::Index() const
{
    return iIndex;
}

// ---------------------------------------------------------------------------
// TPIMFieldData::SetIndex
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
void TPIMFieldData::SetIndex(TInt aIndex)
{
    iIndex = aIndex;
}

// ---------------------------------------------------------------------------
// TPIMFieldData::Attributes
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
TPIMAttribute TPIMFieldData::Attributes() const
{
    return iAttributes;
}

// ---------------------------------------------------------------------------
// TPIMFieldData::IntegerValue
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
TInt TPIMFieldData::IntegerValue() const
{
    __ASSERT_ALWAYS(iDataType == EPIMFieldInt, User::Panic(KPIMPanicCategory,
                    EPIMPanicInvalidFieldType));

    return iIntegerBooleanValue;
}

// ---------------------------------------------------------------------------
// TPIMFieldData::BooleanValue
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
TBool TPIMFieldData::BooleanValue() const
{
    __ASSERT_ALWAYS(iDataType == EPIMFieldBoolean, User::Panic(
                        KPIMPanicCategory, EPIMPanicInvalidFieldType));

    return iIntegerBooleanValue;
}

// ---------------------------------------------------------------------------
// TPIMFieldData::DateValue
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
TPIMDate TPIMFieldData::DateValue() const
{
    __ASSERT_ALWAYS(iDataType == EPIMFieldDate, User::Panic(KPIMPanicCategory,
                    EPIMPanicInvalidFieldType));

    return iDateValue;
}

// ---------------------------------------------------------------------------
// TPIMFieldData::StringValue
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
const TDesC& TPIMFieldData::StringValue() const
{
    __ASSERT_ALWAYS(iDataType == EPIMFieldString, User::Panic(
                        KPIMPanicCategory, EPIMPanicInvalidFieldType));
    return *iStringValue;
}

// ---------------------------------------------------------------------------
// TPIMFieldData::StringArrayValue
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
const CDesCArray& TPIMFieldData::StringArrayValue() const
{
    __ASSERT_ALWAYS(iDataType == EPIMFieldStringArray, User::Panic(
                        KPIMPanicCategory, EPIMPanicInvalidFieldType));

    return *iStringArrayValue;
}

// ---------------------------------------------------------------------------
// TPIMFieldData::BinaryValue
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
const CPIMByteArray& TPIMFieldData::BinaryValue() const
{
    __ASSERT_ALWAYS(iDataType == EPIMFieldBinary, User::Panic(
                        KPIMPanicCategory, EPIMPanicInvalidFieldType));

    return *iBinaryValue;
}

// End of file
