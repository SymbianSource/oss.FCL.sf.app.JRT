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
#include "cpimfield.h"

// INTERNAL INCLUDES
#include "cpimvaluebase.h"
#include "cpimintvalue.h"
#include "cpimbooleanvalue.h"
#include "cpimdatevalue.h"
#include "cpimstringvalue.h"
#include "cpimstringarrayvalue.h"
#include "cpimbinaryvalue.h"
#include "pimpanics.h"
#include "logger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPIMField::~CPIMField()
{
    JELOG2(EPim);
    iValues.ResetAndDestroy();
}

// ---------------------------------------------------------------------------
// CPIMField::NewL
// Default two-phase constructor
// ---------------------------------------------------------------------------
//
CPIMField* CPIMField::NewL(const TPIMFieldData& aFieldData)
{
    JELOG2(EPim);
    CPIMField* self = new(ELeave) CPIMField(aFieldData);
    return self;
}

// ---------------------------------------------------------------------------
// CPIMField::NewLC
// Default two-phase constructor. The newly created object is left to the
// cleanup stack
// ---------------------------------------------------------------------------
//
CPIMField* CPIMField::NewLC(const TPIMFieldData& aFieldData)
{
    JELOG2(EPim);
    CPIMField* self = CPIMField::NewL(aFieldData);
    CleanupStack::PushL(self);
    return self;
}

// ---------------------------------------------------------------------------
// CPIMField::SetValueL
// Sets new value to a specific index. This method
// leaves if there is no such field or field index.
// ---------------------------------------------------------------------------
//
void CPIMField::SetValueL(const TPIMFieldData& aFieldData)
{
    JELOG2(EPim);
    // Find that there is a value in the requested index
    __ASSERT_ALWAYS(IsInRange(aFieldData.Index()), User::Leave(KErrNotFound));

    TInt index = aFieldData.Index();

    switch (aFieldData.FieldDataType())
    {
    case EPIMFieldInt:
    {
        iValues[index]->SetIntValue(aFieldData.iIntegerBooleanValue);
        break;
    }
    case EPIMFieldBoolean:
    {
        iValues[index]->SetBooleanValue(aFieldData.iIntegerBooleanValue);
        break;
    }
    case EPIMFieldDate:
    {
        iValues[index]->SetDateValue(aFieldData.iDateValue);
        break;
    }
    case EPIMFieldString:
    {
        iValues[index]->SetStringValue(aFieldData.iStringValue);
        break;
    }
    case EPIMFieldStringArray:
    {
        iValues[index]->SetStringArrayValue(aFieldData.iStringArrayValue);
        break;
    }
    case EPIMFieldBinary:
    {
        iValues[index]->SetBinaryValue(aFieldData.iBinaryValue);
        break;
    }
    default:
    {
        __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory,
                                           EPIMPanicInvalidFieldType));
        break;
    }
    }
    iValues[index]->SetAttributes(aFieldData.iAttributes);
    // Internal attributes must not be set by this functions because
    // those may be overwriten if this is called from the Java-side
}

// ---------------------------------------------------------------------------
// CPIMField::AddValueL
// Adds new value to the value array. The index doesn't matter in this function
// because the new values is appended to the end of the value array
// ---------------------------------------------------------------------------
//
void CPIMField::AddValueL(const TPIMFieldData& aFieldData)
{
    JELOG2(EPim);
    // In here, we don't care about the index of the new value
    // thus it is added to the end of the value array
    CPIMValueBase* newValue = NULL;
    // Internal attributes must be set when item is created
    CArrayFix<TUid>* internalAttrs = new(ELeave) CArrayFixFlat<TUid> (3);
    CleanupStack::PushL(internalAttrs);

    switch (aFieldData.FieldDataType())
    {
    case EPIMFieldInt:
    {
        newValue = CPIMIntValue::NewLC(aFieldData.Attributes(),
                                       aFieldData.iIntegerBooleanValue);
        iValues.AppendL(newValue);
        break;
    }
    case EPIMFieldBoolean:
    {
        newValue = CPIMBooleanValue::NewLC(aFieldData.Attributes(),
                                           aFieldData.iIntegerBooleanValue);
        iValues.AppendL(newValue);
        break;
    }
    case EPIMFieldDate:
    {
        newValue = CPIMDateValue::NewLC(aFieldData.Attributes(),
                                        aFieldData.iDateValue);
        iValues.AppendL(newValue);
        break;
    }
    case EPIMFieldString:
    {
        newValue = CPIMStringValue::NewLC(aFieldData.Attributes(), NULL);
        iValues.AppendL(newValue);
        // Now it is safe to change the ownership of the value
        newValue->SetStringValue(aFieldData.iStringValue);
        break;
    }
    case EPIMFieldStringArray:
    {
        newValue = CPIMStringArrayValue::NewLC(aFieldData.Attributes(), NULL);
        iValues.AppendL(newValue);
        // Now it is safe to change the ownership of the value
        newValue->SetStringArrayValue(aFieldData.iStringArrayValue);
        break;
    }
    case EPIMFieldBinary:
    {
        newValue = CPIMBinaryValue::NewLC(aFieldData.Attributes(), NULL);
        iValues.AppendL(newValue);
        // Now it is safe to change the ownership of the value
        newValue->SetBinaryValue(aFieldData.iBinaryValue);
        break;
    }
    default:
    {
        __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory,
                                           EPIMPanicInvalidFieldType));
        break;
    }
    }
    // It is safe to pop newValue out of the cleanup stack
    CleanupStack::Pop(2, internalAttrs);
    // Apply internal attributes now
    newValue->SetInternalAttributes(internalAttrs);
}

// ---------------------------------------------------------------------------
// CPIMField::ValueL
// Fecthes a value from the value array. This function leaves with KErrNotFound
// if the there is no such index in the value array
// ---------------------------------------------------------------------------
//
const TPIMFieldData CPIMField::ValueL(const TInt aIndex) const
{
    JELOG2(EPim);
    // Find that there is a value in the requested index
    __ASSERT_ALWAYS(IsInRange(aIndex), User::Leave(KErrNotFound));

    TPIMFieldData fieldData;
    // Add non-value data
    fieldData.iField = iField;
    fieldData.iDataType = iFieldDataType;
    fieldData.iAttributes = iValues[aIndex]->Attributes();

    // Add specific data value to the data holder class
    switch (iFieldDataType)
    {
    case EPIMFieldInt:
    {
        fieldData.iIntegerBooleanValue = iValues[aIndex]->IntValue();
        break;
    }
    case EPIMFieldBoolean:
    {
        fieldData.iIntegerBooleanValue = iValues[aIndex]->BooleanValue();
        break;
    }
    case EPIMFieldDate:
    {
        fieldData.iDateValue = iValues[aIndex]->DateValue();
        break;
    }
    case EPIMFieldString:
    {
        fieldData.iStringValue = iValues[aIndex]->StringValue();
        break;
    }
    case EPIMFieldStringArray:
    {
        fieldData.iStringArrayValue = iValues[aIndex]->StringArrayValue();
        break;
    }
    case EPIMFieldBinary:
    {
        fieldData.iBinaryValue = iValues[aIndex]->BinaryValue();
        break;
    }
    default:
    {
        __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory,
                                           EPIMPanicInvalidFieldType));
        break;
    }
    }

    return fieldData;
}

// ---------------------------------------------------------------------------
// CPIMField::RemoveValueL
// Removes the specified value from the value array. If the requested index
// is not found, KErrNotFound is returned
// ---------------------------------------------------------------------------
//
void CPIMField::RemoveValueL(const TInt aIndex)
{
    JELOG2(EPim);
    // Find that there is a value in the requested index
    __ASSERT_ALWAYS(IsInRange(aIndex), User::Leave(KErrNotFound));
    // Delete value and remove it from the value array
    delete iValues[aIndex];
    iValues.Remove(aIndex);
}

// ---------------------------------------------------------------------------
// CPIMField::CountValues
// Returns the count of the values in this field
// ---------------------------------------------------------------------------
//
TInt CPIMField::CountValues() const
{
    JELOG2(EPim);
    return iValues.Count();
}

// ---------------------------------------------------------------------------
// CPIMField::SetLabelL
// Adds new label to this field
// ---------------------------------------------------------------------------
//
void CPIMField::SetLabelL(const TInt aIndex, const TInt aArrayIndex,
                          const TDesC& aLabel)
{
    JELOG2(EPim);
    __ASSERT_DEBUG(IsInRange(aIndex), User::Panic(KPIMPanicCategory,
                   EPIMPanicInvalidLabel));

    // Get labels
    CDesCArray* labelArray = iValues[aIndex]->Labels();
    if (!labelArray)
    {
        TInt elementCount = iValues[aIndex]->NumElements();
        labelArray = new(ELeave) CDesCArrayFlat(elementCount);
        CleanupStack::PushL(labelArray);
        // Fill array
        for (TInt i = 0; i < elementCount; i++)
        {
            labelArray->AppendL(KPIMNullArrayElement());
        }
        labelArray->Delete(aArrayIndex);
        labelArray->InsertL(aArrayIndex, aLabel);
        iValues[aIndex]->SetLabels(labelArray);
        CleanupStack::Pop(labelArray);
    }
    else
    {
        __ASSERT_DEBUG(aArrayIndex < labelArray->Count(), User::Panic(
                           KPIMPanicCategory, EPIMPanicInvalidLabel));
        labelArray->Delete(aArrayIndex);
        labelArray->InsertL(aArrayIndex, aLabel);
        // No need to do anything else because the array
        // is owned by the value container class. Bad OOP
        // practice but now it is the best workaround
    }
}

// ---------------------------------------------------------------------------
// CPIMField::LabelL
// Retrieves a label from a specific index
// ---------------------------------------------------------------------------
//
const TPtrC CPIMField::Label(const TInt aIndex, const TInt aArrayIndex) const
{
    JELOG2(EPim);
    // Panic if the value index is incorrect
    __ASSERT_DEBUG(IsInRange(aIndex), User::Panic(KPIMPanicCategory,
                   EPIMPanicInvalidLabel));
    // Get labels
    CDesCArray* labelArray = iValues[aIndex]->Labels();
    // Return null element descriptor if there is no such label
    // This indicates that the label has not been set or something
    // else is wrong.
    if (!labelArray || (aArrayIndex >= labelArray->MdcaCount()))
    {
        return KPIMNullArrayElement();
    }
    return labelArray->MdcaPoint(aArrayIndex);
}

// ---------------------------------------------------------------------------
// CPIMField::DataType
// Returns the data type of this field
// ---------------------------------------------------------------------------
//
TPIMFieldDataType CPIMField::DataType() const
{
    JELOG2(EPim);
    return iFieldDataType;
}

// ---------------------------------------------------------------------------
// CPIMField::FieldType
// Returns the type of this field
// ---------------------------------------------------------------------------
//
TPIMField CPIMField::FieldType() const
{
    JELOG2(EPim);
    return iField;
}

// ---------------------------------------------------------------------------
// CPIMField::SetAttributesL
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
void CPIMField::SetAttributesL(const TInt aIndex, TPIMAttribute aAttributes)
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(IsInRange(aIndex), User::Leave(KErrNotFound));
    iValues[aIndex]->SetAttributes(aAttributes);
}

// ---------------------------------------------------------------------------
// CPIMField::AttributesL
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
TPIMAttribute CPIMField::AttributesL(const TInt aIndex) const
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(IsInRange(aIndex), User::Leave(KErrNotFound));
    return iValues[aIndex]->Attributes();
}
// ---------------------------------------------------------------------------
// CPIMField::SetInternalAttributesL
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
void CPIMField::SetInternalAttributesL(const TInt aIndex,
                                       CArrayFix<TUid>* aInternalAttributes)
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(IsInRange(aIndex), User::Leave(KErrNotFound));
    iValues[aIndex]->SetInternalAttributes(aInternalAttributes);
}

// ---------------------------------------------------------------------------
// CPIMField::InternalAttributesL
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
const CArrayFix<TUid>& CPIMField::InternalAttributesL(const TInt aIndex) const
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(IsInRange(aIndex), User::Leave(KErrNotFound));
    return iValues[aIndex]->InternalAttributes();
}

// ---------------------------------------------------------------------------
// CPIMField::IsInRange
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
TBool CPIMField::IsInRange(const TInt aIndex) const
{
    JELOG2(EPim);
    return (aIndex >= 0) && (aIndex < iValues.Count());
}

// ---------------------------------------------------------------------------
// CPIMField::CPIMField
// Default C++ constructor
// ---------------------------------------------------------------------------
//
inline CPIMField::CPIMField(const TPIMFieldData& aFieldData) :
        iField(aFieldData.Field()), iFieldDataType(aFieldData.FieldDataType())
{
    JELOG2(EPim);
}

// End of file
