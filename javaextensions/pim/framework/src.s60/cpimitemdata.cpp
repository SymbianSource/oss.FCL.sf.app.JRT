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
* Description:  Class for holding PIM item data
 *
*/


// CLASS HEADER
#include "cpimitemdata.h"

// INTERNAL INCLUDES
#include "cpimfield.h"
#include "pimpanics.h"
#include "logger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPIMItemData::~CPIMItemData()
{
    JELOG2(EPim);
    delete iCategories;
    iFields.ResetAndDestroy();
}

// ---------------------------------------------------------------------------
// CPIMItemData::NewL
// Default two-phase constructor
// ---------------------------------------------------------------------------
//
CPIMItemData* CPIMItemData::NewL()
{
    JELOG2(EPim);
    CPIMItemData* self = CPIMItemData::NewLC();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CPIMItemData::NewLC
// Default two-phase constructor. The newly created object is left to the
// cleanup stack
// ---------------------------------------------------------------------------
//
CPIMItemData* CPIMItemData::NewLC()
{
    JELOG2(EPim);
    CPIMItemData* self = new(ELeave) CPIMItemData();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// ---------------------------------------------------------------------------
// CPIMItemData::SetValueL
// Sets new value for a specific field value in a specific index. This method
// leaves if there is no such field or field index. Also, this panics if the
// new value field type is not the same as the previous one
// ---------------------------------------------------------------------------
//
void CPIMItemData::SetValueL(const TPIMFieldData& aFieldData)
{
    JELOG2(EPim);
    // Get the data from the holder class
    TInt index = FieldIndex(aFieldData.Field());
    // Leave with KErrNotFound if there is no requested field
    __ASSERT_ALWAYS(index != KErrNotFound, User::Leave(index));

    __ASSERT_DEBUG(aFieldData.FieldDataType() == iFields[index]->DataType(),
                   User::Panic(KPIMPanicCategory, EPIMPanicInvalidFieldType));

    iFields[index]->SetValueL(aFieldData);
}

// ---------------------------------------------------------------------------
// CPIMItemData::AddValueL
// Adds new value for the field. New field is generated if there is no existing
// field (i.e this is the first value which is added to this field
// ---------------------------------------------------------------------------
//
void CPIMItemData::AddValueL(const TPIMFieldData& aFieldData)
{
    JELOG2(EPim);
    // Check that if there is already a field of this type
    TInt index = FieldIndex(aFieldData.Field());
    if (index != KErrNotFound)
    {
        __ASSERT_DEBUG(
            aFieldData.FieldDataType() == iFields[index]->DataType(),
            User::Panic(KPIMPanicCategory, EPIMPanicInvalidFieldType));
        iFields[index]->AddValueL(aFieldData);
    }
    // Generate new field
    else
    {
        CPIMField* newField = CPIMField::NewLC(aFieldData);
        iFields.AppendL(newField);
        // Add new value to item and prepare for an error
        TRAPD(err, newField->AddValueL(aFieldData));
        if (err != KErrNone)
        {
            // The array must be returned to state that it was before this
            // function call to ensure leave safe function
            TInt entryIndex = iFields.Find(newField);
            iFields.Remove(entryIndex);
            User::Leave(err);
        }
        CleanupStack::Pop(newField);
    }
}

// ---------------------------------------------------------------------------
// CPIMItemData::ValueL
// Fetches a Value a specifc index of the requested field
// ---------------------------------------------------------------------------
//
const TPIMFieldData CPIMItemData::ValueL(const TPIMField aField,
        const TInt aIndex) const
{
    JELOG2(EPim);
    const TInt index = FieldIndex(aField);
    // Leave with KErrNotFound if there is no requested field
    __ASSERT_ALWAYS(index != KErrNotFound, User::Leave(index));
    return iFields[index]->ValueL(aIndex);
}

// ---------------------------------------------------------------------------
// CPIMItemData::RemoveValueL
// Removes a specific value of a specific field.
// ---------------------------------------------------------------------------
//
void CPIMItemData::RemoveValueL(const TPIMField aField, const TInt aIndex)
{
    JELOG2(EPim);
    TInt index = FieldIndex(aField);
    // Leave with KErrNotFound if there is no requested field
    __ASSERT_ALWAYS(index != KErrNotFound, User::Leave(index));
    iFields[index]->RemoveValueL(aIndex);
    // Remove field if this was the last value
    if (iFields[index]->CountValues() == 0)
    {
        // Remove object from array and delete it
        delete iFields[index];
        iFields.Remove(index);
    }
}

// ---------------------------------------------------------------------------
// CPIMItemData::CountValues
// Counts the values in the requested field
// ---------------------------------------------------------------------------
//
TInt CPIMItemData::CountValues(const TPIMField aField) const
{
    JELOG2(EPim);
    TInt index = FieldIndex(aField);
    // Leave with KErrNotFound if there is no requested field
    if (index != KErrNotFound)
    {
        return iFields[index]->CountValues();
    }
    // Else return zero
    return 0;
}

// ---------------------------------------------------------------------------
// CPIMItemData::FieldsL
// Returns fields that currently have more than zero values. The returned
// array is left to the cleanup stack
// ---------------------------------------------------------------------------
//
CArrayFix<TPIMField>* CPIMItemData::FieldsLC() const
{
    JELOG2(EPim);
    CArrayFixFlat<TPIMField>* fields =
        new(ELeave) CArrayFixFlat<TPIMField> (5);
    CleanupStack::PushL(fields);

    TInt fieldCount = iFields.Count();
    for (TInt i = 0; i < fieldCount; i++)
    {
        fields->AppendL(iFields[i]->FieldType());
    }
    return fields;
}

// ---------------------------------------------------------------------------
// CPIMItemData::SetLabelL
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
void CPIMItemData::SetLabelL(const TPIMField aField, const TInt aIndex,
                             const TInt aArrayIndex, const TDesC& aLabel)
{
    JELOG2(EPim);
    TInt index = FieldIndex(aField);
    // Leave with KErrNotFound if there is no requested field
    __ASSERT_ALWAYS(index != KErrNotFound, User::Leave(index));
    iFields[index]->SetLabelL(aIndex, aArrayIndex, aLabel);
}

// ---------------------------------------------------------------------------
// CPIMItemData::LabelL
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
const TPtrC CPIMItemData::LabelL(const TPIMField aField, const TInt aIndex,
                                 const TInt aArrayIndex) const
{
    JELOG2(EPim);
    TInt index = FieldIndex(aField);
    // Leave with KErrNotFound if there is no requested field
    __ASSERT_ALWAYS(index != KErrNotFound, User::Leave(index));
    return iFields[index]->Label(aIndex, aArrayIndex);
}

// ---------------------------------------------------------------------------
// CPIMItemData::Reset
// Resets the item data. All fields and categories are removed
// ---------------------------------------------------------------------------
//
void CPIMItemData::Reset()
{
    JELOG2(EPim);
    iCategories->Reset();
    iFields.ResetAndDestroy();
    // All data is now deleted but the arrays are ready for reuse. see
    // documentation of RPointerArrray and CDesCArray.
}

// ---------------------------------------------------------------------------
// CPIMItemData::SetAttributesL
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
void CPIMItemData::SetAttributesL(const TPIMField aField, const TInt aIndex,
                                  TPIMAttribute aAttributes)
{
    JELOG2(EPim);
    TInt index = FieldIndex(aField);
    // Leave with KErrNotFound if there is no requested field
    __ASSERT_ALWAYS(index != KErrNotFound, User::Leave(index));
    iFields[index]->SetAttributesL(aIndex, aAttributes);
}

// ---------------------------------------------------------------------------
// CPIMItemData::LabelL
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
TInt CPIMItemData::AttributesL(const TPIMField aField, const TInt aIndex) const
{
    JELOG2(EPim);
    TInt index = FieldIndex(aField);
    // Leave with KErrNotFound if there is no requested field
    __ASSERT_ALWAYS(index != KErrNotFound, User::Leave(index));
    return iFields[index]->AttributesL(aIndex);
}

// ---------------------------------------------------------------------------
// CPIMItemData::SetInternalAttributesL
// Sets internal attributes for a specific field and index. Internal
// attributes are not visible from the Java-side so this function provides
// the only access to set internal attributes to an item
// ---------------------------------------------------------------------------
//
void CPIMItemData::SetInternalAttributesL(const TPIMField aField,
        const TInt aIndex, CArrayFix<TUid>* aInternalAttributes)
{
    JELOG2(EPim);
    TInt index = FieldIndex(aField);
    // Leave with KErrNotFound if there is no requested field
    __ASSERT_ALWAYS(index != KErrNotFound, User::Leave(index));
    iFields[index]->SetInternalAttributesL(aIndex, aInternalAttributes);
}

// ---------------------------------------------------------------------------
// CPIMItemData::InternalAttributes
// Gets internal attributes for a specific field and index. Internal
// attributes are not visible from the Java-side so this function provides
// the only access to get internal attributes to an item
// ---------------------------------------------------------------------------
//
const CArrayFix<TUid>& CPIMItemData::InternalAttributesL(
    const TPIMField aField, const TInt aIndex) const
{
    JELOG2(EPim);
    TInt index = FieldIndex(aField);
    // Leave with KErrNotFound if there is no requested field
    __ASSERT_ALWAYS(index != KErrNotFound, User::Leave(index));
    return iFields[index]->InternalAttributesL(aIndex);
}

// ---------------------------------------------------------------------------
// CPIMItemData::FindCategory
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
TInt CPIMItemData::FindCategory(const TDesC& aCategoryName) const
{
    JELOG2(EPim);
    TInt index(0);
    if (iCategories->Find(aCategoryName, index, ECmpNormal) == 0)
    {
        return index;
    }
    return KErrNotFound;
}

// ---------------------------------------------------------------------------
// CPIMItemData::AddNewCategoryL
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
void CPIMItemData::AddNewCategoryL(const TDesC& aCategoryName)
{
    JELOG2(EPim);
    iCategories->AppendL(aCategoryName);
}

// ---------------------------------------------------------------------------
// CPIMItemData::RemoveCategory
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
TBool CPIMItemData::RemoveCategory(const TDesC& aCategoryName)
{
    JELOG2(EPim);
    TInt index = FindCategory(aCategoryName);
    if (index != KErrNotFound)
    {
        iCategories->Delete(index);
        return ETrue;
    }
    return EFalse;
}

// ---------------------------------------------------------------------------
// CPIMItemData::Categories
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
const CDesCArray& CPIMItemData::Categories() const
{
    JELOG2(EPim);
    return *iCategories;
}

// ---------------------------------------------------------------------------
// CPIMItemData::HasField
// Other items were commented in the header
// ---------------------------------------------------------------------------
//
TBool CPIMItemData::HasField(const TPIMField aField) const
{
    JELOG2(EPim);
    return (FieldIndex(aField) != KErrNotFound);
}

// ---------------------------------------------------------------------------
// CPIMItemData::CPIMItemData
// Default C++ constructor
// ---------------------------------------------------------------------------
//
inline CPIMItemData::CPIMItemData()
{
    JELOG2(EPim);
    // Nothing to do at the moment...
}

// ---------------------------------------------------------------------------
// CPIMItemData::ConstructL
// Default second phase constuctor. Does the leaving operations in item
// construction procedure
// ---------------------------------------------------------------------------
//
inline void CPIMItemData::ConstructL()
{
    JELOG2(EPim);
    iCategories = new(ELeave) CDesCArrayFlat(5);  // Array granularity
}

// ---------------------------------------------------------------------------
// CPIMItemData::FindField
// Searches requested field type. If there is no such field, KErrNotFound is
// returned
// ---------------------------------------------------------------------------
//
TInt CPIMItemData::FieldIndex(const TPIMField aField) const
{
    JELOG2(EPim);
    TInt count = iFields.Count();
    for (TInt i = 0; i < count; i++)
    {
        if (iFields[i]->FieldType() == aField)
        {
            return i;
        }
    }
    return KErrNotFound;
}

// End of file
