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
* Description:  PIM item base implementation.
 *
*/


// CLASS HEADER
#include "cpimitem.h"

// INTERNAL INCLUDES
#include "cpimitemdata.h"
#include "mpimadaptermanager.h"
#include "mpimlistadapter.h"
#include "cpimvalidator.h"
#include "pimbase64converter.h"
#include "pimpanics.h"
#include "pimjnitools.h"
#include "pimutils.h"
#include "s60commonutils.h"
#include "jstringutils.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <e32std.h>

//CONSTANTS
const TInt KByteArrayGranularity = 8;

// ---------------------------------------------------------------------------
// CPIMItem::CPIMItem
// Default C++ constructor
// ---------------------------------------------------------------------------
//
CPIMItem::CPIMItem(const CPIMValidator& aValidator) :
        iValidator(aValidator)
{
    JELOG2(EPim);
}

// ---------------------------------------------------------------------------
// CPIMItem::ConstructL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::ConstructL()
{
    JELOG2(EPim);
    // 5 for array granularity. Items is partial for default
    iInitializedFields = new(ELeave) CArrayFixFlat<TPIMField> (5);
    iItemID = KPIMNullItemID().AllocL();
    iItemData = CPIMItemData::NewL();
}

// ---------------------------------------------------------------------------
// CPIMItem::~CPIMItem
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
CPIMItem::~CPIMItem()
{
    JELOG2(EPim);
    delete iItemData;
    delete iItemID;
    delete iInitializedFields;
}

// ---------------------------------------------------------------------------
// CPIMItem::FieldDataType
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TPIMFieldDataType CPIMItem::fieldDataType(const TPIMField& aField) const
{
    JELOG2(EPim);
    return iValidator.FieldDataType(aField);
}

// ---------------------------------------------------------------------------
// CPIMItem::ValidAttributes
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TPIMAttribute CPIMItem::ValidAttributes() const
{
    JELOG2(EPim);
    return iValidator.ValidAttributes();
}

// ---------------------------------------------------------------------------
// CPIMItem::IsValidBooleanValue
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TBool CPIMItem::IsValidBooleanValue(const TPIMField& aField,
                                    const TBool& aValue) const
{
    JELOG2(EPim);
    return iValidator.IsValidBooleanValue(aField, aValue);
}

// ---------------------------------------------------------------------------
// CPIMItem::IsValidDateValue
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TBool CPIMItem::IsValidDateValue(const TPIMField& aField,
                                 const TPIMDate& aValue) const
{
    JELOG2(EPim);
    return iValidator.IsValidDateValue(aField, aValue);
}

// ---------------------------------------------------------------------------
// CPIMItem::IsValidIntegerValue
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TBool CPIMItem::IsValidIntegerValue(const TPIMField& aField, const TInt& aValue) const
{
    JELOG2(EPim);
    return iValidator.IsValidIntegerValue(aField, aValue);
}

// ---------------------------------------------------------------------------
// CPIMItem::IsValidBinaryValue
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TBool CPIMItem::IsValidBinaryValue(const TPIMField& aField,
                                   const CPIMByteArray& aValue) const
{
    JELOG2(EPim);
    return iValidator.IsValidBinaryValue(aField, aValue);
}

// ---------------------------------------------------------------------------
// CPIMItem::IsValidStringValue
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TBool CPIMItem::IsValidStringValue(const TPIMField& aField, const HBufC& aValue) const
{
    JELOG2(EPim);
    return iValidator.IsValidStringValue(aField, aValue);
}

// ---------------------------------------------------------------------------
// CPIMItem::IsValidStringArrayValue
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TBool CPIMItem::IsValidStringArrayValue(const TPIMField& aField,
                                        const CDesCArray& aValue) const
{
    JELOG2(EPim);
    return iValidator.IsValidStringArrayValue(aField, aValue);
}

// ---------------------------------------------------------------------------
// CPIMItem::SetModified
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::SetModified()
{
    JELOG2(EPim);
    SetModified(ETrue);
}

// ---------------------------------------------------------------------------
// CPIMItem::SetModified
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::SetModified(TBool aIsModified)
{
    JELOG2(EPim);
    iIsModified = aIsModified;
}

// ---------------------------------------------------------------------------
// CPIMItem::ListClosed
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::ListClosed()
{
    JELOG2(EPim);
    // The list adapter is owned by the associated list.
    iListAdapter = NULL;
}

// ---------------------------------------------------------------------------
// CPIMItem::CategoryRenamedL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::CategoryRenamedL(const TDesC& aOldName, const TDesC& aNewName)
{
    JELOG2(EPim);
    // If the new name is same as the old name, nothing needs to be done
    if (aOldName == aNewName)
    {
        return;
    }

    // Take action only if the old category is found
    TInt oldCatIndex = iItemData->FindCategory(aOldName);
    if (oldCatIndex != KErrNotFound)
    {
        // Add the new category only if it is not already present
        if (iItemData->FindCategory(aNewName) == KErrNotFound)
        {
            // Not found, add
            iItemData->AddNewCategoryL(aNewName);
        }

        // It was checked earlier that the old and new category are
        // not the same, so we'll just delete the old category
        iItemData->RemoveCategory(aOldName);
    }
}

// ---------------------------------------------------------------------------
// CPIMItem::CategoryDeleted
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TBool CPIMItem::CategoryDeleted(const TDesC& aCategory)
{
    JELOG2(EPim);
    return iItemData->RemoveCategory(aCategory);
}

// ---------------------------------------------------------------------------
// CPIMItem::RemoveAdapterAssociation
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::RemoveAdapterAssociation()
{
    JELOG2(EPim);
    iAdapterManager = NULL;
    iListAdapter = NULL;
}

// ---------------------------------------------------------------------------
// CPIMItem::LoadFullItemL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::LoadFullItemL() const
{
    JELOG2(EPim);
    // Initialized fields are NULL if there is no need to
    // load any fields and the item has been fully constructed
    if (iInitializedFields && iListAdapter && (iItemID->Compare(KPIMNullItemID)
            != 0))
    {
        // Const cast is necessary. Call specialized class to load
        // full item if it is supported. The state of the item must
        // be changed by this call, because after this it is expected
        // that the item contains full item data and does not need to load
        CPIMItem* self = const_cast<CPIMItem*>(this);
        self->DoLoadFullItemL();
    }
}

// ---------------------------------------------------------------------------
// CPIMItem::LoadFieldL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::LoadFieldL(const TPIMField aField) const
{
    JELOG2(EPim);
    // Initialized fields are NULL if there is no need to
    // load any fields and the item has been fully constructed
    if (iInitializedFields && iListAdapter && (iItemID->Compare(KPIMNullItemID)
            != 0))
    {
        // First check that the item data doesn't have this field
        if (iItemData->HasField(aField))
        {
            // Field has been loaded by the adapter
            iInitializedFields->AppendL(aField);
            return;
        }
        TInt index(0);
        TKeyArrayFix key(0, ECmpTInt);
        // The field needs to be loaded if it is not initialized
        if (iInitializedFields->Find(aField, key, index) != 0)
        {
            // Const cast is necessary. Call specialized class to load
            // full item if it is supported. The state of the item must
            // be changed by this call, because after this it is expected
            // that the item contains full item data and does not need to load
            CPIMItem* self = const_cast<CPIMItem*>(this);
            self->DoLoadFieldL(aField);
        }
    }
}

// ---------------------------------------------------------------------------
// CPIMItem::DoLoadFullItemL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::DoLoadFullItemL()
{
    JELOG2(EPim);
    // Default implementation changes partial item to full item
    delete iInitializedFields;
    iInitializedFields = NULL;
}

// ---------------------------------------------------------------------------
// CPIMItem::DoLoadFieldL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::DoLoadFieldL(const TPIMField aField)
{
    JELOG2(EPim);
    // Default implementation. Just adds the field to initialized fields
    // to mark that this field has been loaded from the database
    iInitializedFields->AppendL(aField);
}

// ---------------------------------------------------------------------------
// CPIMItem::UpdateUidFieldL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::UpdateUidFieldL(const TPIMField& aUidStringField,
                               const TPIMItemID& aItemId)
{
    JELOG2(EPim);
    // Extra checking
    if (iAdapterManager && !iAdapterManager->IsSupportedField(aUidStringField))
    {
        return;
    }

    __ASSERT_DEBUG(iItemData->CountValues(aUidStringField) == 0
                   || iItemData->CountValues(aUidStringField) == 1, User::Panic(
                       KPIMPanicCategory, EPIMPanicInvalidState));

    // Remove the value from the field, if present
    if (aItemId != KPIMNullItemID)
    {
        // Create value class for this value
        HBufC* value = HBufC::NewLC(aItemId.Length());
        TPtr valuePtr(value->Des());
        valuePtr.Copy(aItemId);

        TPIMFieldData fieldData(aUidStringField, KPIMAttrNone, value);
        // The value must be overwritten if there is an existing one
        // Else just add new value
        if (iItemData->CountValues(aUidStringField) == 0)
        {
            iItemData->AddValueL(fieldData);
        }
        else
        {
            // There was a value so set a new one
            iItemData->SetValueL(fieldData);
        }
        CleanupStack::Pop(value);
    }

    // else we just leave the field empty
}

// ---------------------------------------------------------------------------
// CPIMItem::UpdateRevisionFieldL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::UpdateRevisionFieldL(TPIMField aRevisionField,
                                    TPIMDate aLastModified)
{
    JELOG2(EPim);
    if (iAdapterManager && !iAdapterManager->IsSupportedField(aRevisionField))
    {
        return;
    }

    __ASSERT_DEBUG(iItemData->CountValues(aRevisionField) == 0
                   || iItemData->CountValues(aRevisionField) == 1, User::Panic(
                       KPIMPanicCategory, EPIMPanicInvalidState));

    // Create holder class for the value
    TPIMFieldData fieldData(aRevisionField, KPIMAttrNone, aLastModified);

    // Add new value if there is no old value. Set
    // new if there was value already
    if (iItemData->CountValues(aRevisionField) == 0)
    {
        iItemData->AddValueL(fieldData);
    }
    else
    {
        iItemData->SetValueL(fieldData);
    }
}

// ---------------------------------------------------------------------------
// CPIMItem::GetFieldsL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
CArrayFix<TPIMField>* CPIMItem::GetFieldsL() const
{
    JELOG2(EPim);
    // We need to load full item to be sure that all
    // fields are filled to this item. This is performed
    // only when the item is associated with a list
    LoadFullItemL();

    // Return value
    CArrayFix<TPIMField>* fields = iItemData->FieldsLC();

    // If this item belongs to a list, we'll sort the result according
    // to the order of the supported fields
    if (iAdapterManager)
    {
        CArrayFixFlat<TPIMField>* sortedFields = new(ELeave) CArrayFixFlat<
        TPIMField> (5);
        CleanupStack::PushL(sortedFields);

        const CArrayFix<TPIMField>& supportedFields =
            iAdapterManager->GetSupportedFieldsL();

        const TInt numSupportedFields = supportedFields.Count();

        for (TInt suppIndex = 0; suppIndex < numSupportedFields; suppIndex++)
        {
            const TPIMField supportedField = supportedFields[suppIndex];

            const TInt numPresentFields = fields->Count();
            for (TInt presIndex = 0; presIndex < numPresentFields; presIndex++)
            {
                const TPIMField presentField = (*fields)[presIndex];
                if (supportedField == presentField)
                {
                    sortedFields->AppendL(presentField);
                    fields->Delete(presIndex);
                    break;
                }
            }
        }

        // There shouldn't be any fields left BUT if there are, we'll
        // append them to the end of the result array
        const TInt numFieldsLeft = fields->Count();
        for (TInt leftIndex = 0; leftIndex < numFieldsLeft; leftIndex++)
        {
            sortedFields->AppendL((*fields)[leftIndex]);
        }

        // Replace the original result array with the sorted array
        CleanupStack::Pop(sortedFields);
        CleanupStack::PopAndDestroy(fields);
        fields = sortedFields;
    }
    else
    {
        CleanupStack::Pop(fields);
    }

    return fields;
}

//void
jintArray CPIMItem::callGetFields(JNIEnv* aJniEnv/*, jintArray aJavaFields*/)
{
    JELOG2(EPim);
    CArrayFix<TPIMField>* fields = NULL;
    TInt error = KErrNone;
    TRAP(error, fields = GetFieldsL());
    if (error != KErrNone)
    {
        // If a leave occurred the field array was never created.
        return NULL;
    }

    // We now own the field array

    jintArray javaFields = ConvertToJavaIntArray(*aJniEnv, *fields);
    delete fields;
    return javaFields;
}

// ---------------------------------------------------------------------------
// CPIMItem::IsModified
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
bool CPIMItem::isModified() const
{
    JELOG2(EPim);
    if (iIsModified)
        return true;
    return false;
}

// ---------------------------------------------------------------------------
// CPIMItem::GetBinaryL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
CPIMByteArray* CPIMItem::GetBinaryL(TPIMField aField, TInt aIndex) const
{
    JELOG2(EPim);
    const CPIMByteArray& byteArray = GetBinaryRawL(aField, aIndex);
    return PIMBase64Converter::EncodeBase64L(byteArray);
}

jbyteArray CPIMItem::callGetBinary(TPIMField aField, int aIndex,
                                   JNIEnv* aJniEnv, jintArray aError)
{
    JELOG2(EPim);
    TInt error = 0;
    CPIMByteArray* value = NULL;
    TRAP(error, value = GetBinaryL(aField, aIndex));
    SetJavaErrorCode(aJniEnv, aError, error);

    if (error != KErrNone)
    {
        // If an error occurred the value was never created.
        return NULL;
    }

    // We now own the byte array.

    const TInt numElems = value->Count();
    jbyteArray javaValue = aJniEnv->NewByteArray(numElems);

    if (!javaValue)
    {
        SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
        delete value;
        return NULL;
    }

    if (numElems > 0)
    {
        aJniEnv->SetByteArrayRegion(javaValue, 0, numElems,
                                    reinterpret_cast<jbyte*>(value->Back(0)));
    }

    delete value;
    return javaValue;
}

// ---------------------------------------------------------------------------
// CPIMItem::GetBinaryRawL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
const CPIMByteArray& CPIMItem::GetBinaryRawL(TPIMField aField, TInt aIndex) const
{
    JELOG2(EPim);
    LeaveIfInvalidOrUnsupportedFieldL(aField, EPIMFieldBinary);
    // Load field if it is not loaded yet
    LoadFieldL(aField);
    return iItemData->ValueL(aField, aIndex).BinaryValue();
}

// ---------------------------------------------------------------------------
// CPIMItem::AddBinaryL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::AddBinaryL(TPIMField aField, TPIMAttribute aAttributes,
                          CPIMByteArray* aValue)
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(aValue, User::Leave(KErrArgument));

    CPIMByteArray* decodedValue = PIMBase64Converter::DecodeBase64L(*aValue);

    CleanupStack::PushL(decodedValue);
    AddBinaryRawL(aField, aAttributes, decodedValue);
    CleanupStack::Pop(decodedValue);
    delete aValue;
}

void CPIMItem::callAddBinary(TPIMField aField, TPIMAttribute aAttributes,
                             unsigned char* aValue, int aOffset, int aLength)
{
    JELOG2(EPim);
    TInt error = 0;
    TRAP(error,
    {
        CPIMByteArray* value =
        new(ELeave) CPIMByteArray(KByteArrayGranularity);
        CleanupStack::PushL(value);
        value->AppendL(aValue + aOffset, aLength);
        AddBinaryL(aField, aAttributes, value);
        CleanupStack::Pop(value);
    });
    if (error != KErrNone)
        throw error;
}

// ---------------------------------------------------------------------------
// CPIMItem::AddBinaryRawL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::AddBinaryRawL(TPIMField aField, TPIMAttribute aAttributes,
                             CPIMByteArray* aValue)
{
    JELOG2(EPim);
    LeaveIfInvalidOrUnsupportedFieldL(aField, EPIMFieldBinary);

    // Load field if it is not loaded yet
    LoadFieldL(aField);

    // Check argument
    __ASSERT_ALWAYS(aValue, User::Leave(KErrArgument));

    if (iAdapterManager)
    {
        // Note that we must compare for equality (not equal or greater)
        // because MaxValues might return KPIMUnlimitedValues (negative).
        if (iItemData->CountValues(aField) == iAdapterManager->MaxValues(
                    aField))
        {
            User::Leave(KErrOverflow);
        }
    }

    __ASSERT_ALWAYS(IsValidBinaryValue(aField, *aValue), User::Leave(
                        KErrTooBig));
    __ASSERT_ALWAYS(!IsReadOnly(aField), User::Leave(KErrLocked));

    aAttributes = ClearUnsupportedAttributesL(aAttributes, aField);
    TPIMFieldData fieldData(aField, aAttributes, aValue);
    iItemData->AddValueL(fieldData);
    SetModified();
}

// ---------------------------------------------------------------------------
// CPIMItem::SetBinaryL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::SetBinaryL(TPIMField aField, TInt aIndex,
                          TPIMAttribute aAttributes, CPIMByteArray* aValue)
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(aValue, User::Leave(KErrArgument));

    CPIMByteArray* decodedValue =
        PIMBase64Converter::DecodeBase64L(*aValue);

    CleanupStack::PushL(decodedValue);
    SetBinaryRawL(aField, aIndex, aAttributes, decodedValue);
    CleanupStack::Pop(decodedValue);
    delete aValue;
}

void CPIMItem::callSetBinary(TPIMField aField, int aIndex,
                             TPIMAttribute aAttributes, unsigned char* aValue, int aOffset,
                             int aLength)
{
    JELOG2(EPim);
    TInt error = KErrNone;
    TRAP(error,
    {
        CPIMByteArray* value =
        new(ELeave) CPIMByteArray(KByteArrayGranularity);
        CleanupStack::PushL(value);
        value->AppendL(aValue + aOffset, aLength);
        SetBinaryL(aField, aIndex, aAttributes, value);
        CleanupStack::Pop(value);
    });
    if (error != KErrNone)
        throw error;
}

// ---------------------------------------------------------------------------
// CPIMItem::SetBinaryRawL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::SetBinaryRawL(TPIMField aField, TInt aIndex,
                             TPIMAttribute aAttributes, CPIMByteArray* aValue)
{
    JELOG2(EPim);
    LeaveIfInvalidOrUnsupportedFieldL(aField, EPIMFieldBinary);

    // Load field if it is not loaded yet
    LoadFieldL(aField);

    __ASSERT_ALWAYS(aValue, User::Leave(KErrArgument));
    __ASSERT_ALWAYS(IsValidBinaryValue(aField, *aValue), User::Leave(
                        KErrTooBig));
    __ASSERT_ALWAYS(!IsReadOnly(aField), User::Leave(KErrLocked));

    aAttributes = ClearUnsupportedAttributesL(aAttributes, aField);
    TPIMFieldData fieldData(aField, aAttributes, aValue);
    fieldData.SetIndex(aIndex);
    iItemData->SetValueL(fieldData);

    SetModified();
}

// ---------------------------------------------------------------------------
// CPIMItem::GetDateL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
const TPIMDate CPIMItem::GetDateL(TPIMField aField, TInt aIndex) const
{
    JELOG2(EPim);
    LeaveIfInvalidOrUnsupportedFieldL(aField, EPIMFieldDate);
    // Load field if it is not loaded yet
    LoadFieldL(aField);
    return iItemData->ValueL(aField, aIndex).DateValue();
}

jlong CPIMItem::callGetDate(TPIMField aField, int aIndex)
{
    JELOG2(EPim);
    TInt error = KErrNone;
    jlong retVal = 0;
    TRAP(error,
    {
        TPIMDate value = GetDateL(aField, aIndex);
        retVal = java::util::S60CommonUtils::TTimeToJavaTime(value);
    });
    if (error != KErrNone)
        throw error;
    return retVal;
}

// ---------------------------------------------------------------------------
// CPIMItem::AddDateL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::AddDateL(TPIMField aField,
                        TPIMAttribute aAttributes, TPIMDate aValue)
{
    JELOG2(EPim);
    LeaveIfInvalidOrUnsupportedFieldL(aField, EPIMFieldDate);

    // Load field if it is not loaded yet
    LoadFieldL(aField);

    if (iAdapterManager)
    {
        // Note that we must compare for equality (not equal or greater)
        // because MaxValues might return KPIMUnlimitedValues (negative).
        if (iItemData->CountValues(aField)
                == iAdapterManager->MaxValues(aField))
        {
            User::Leave(KErrOverflow);
        }
    }
    __ASSERT_ALWAYS(IsValidDateValue(aField, aValue), User::Leave(
                        KErrTooBig));
    __ASSERT_ALWAYS(!IsReadOnly(aField), User::Leave(KErrLocked));

    aAttributes = ClearUnsupportedAttributesL(aAttributes, aField);
    TPIMFieldData fieldData(aField, aAttributes, aValue);
    iItemData->AddValueL(fieldData);

    SetModified();
}

void CPIMItem::callAddDate(TPIMField aField,
                           TPIMAttribute aAttributes, jlong aValue)
{
    JELOG2(EPim);
    TPIMDate nativeValue(
        java::util::S60CommonUtils::JavaTimeToTTime(aValue));
    TInt error = KErrNone;
    TRAP(error, AddDateL(aField, aAttributes, nativeValue));
    if (error != KErrNone)
        throw error;
}

// ---------------------------------------------------------------------------
// CPIMItem::SetDateL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::SetDateL(TPIMField aField, TInt aIndex,
                        TPIMAttribute aAttributes, TPIMDate aValue)
{
    JELOG2(EPim);
    LeaveIfInvalidOrUnsupportedFieldL(aField, EPIMFieldDate);

    // Load field if it is not loaded yet
    LoadFieldL(aField);

    __ASSERT_ALWAYS(IsValidDateValue(aField, aValue), User::Leave(
                        KErrTooBig));
    __ASSERT_ALWAYS(!IsReadOnly(aField), User::Leave(KErrLocked));

    aAttributes = ClearUnsupportedAttributesL(aAttributes, aField);
    TPIMFieldData fieldData(aField, aAttributes, aValue);
    fieldData.SetIndex(aIndex);
    iItemData->SetValueL(fieldData);
    SetModified();
}

void CPIMItem::callSetDate(TPIMField aField, int aIndex,
                           TPIMAttribute aAttributes, jlong aValue)
{
    JELOG2(EPim);
    TPIMDate nativeValue(
        java::util::S60CommonUtils::JavaTimeToTTime(aValue));
    TInt error = KErrNone;
    TRAP(error, SetDateL(aField, aIndex, aAttributes, nativeValue));
    if (error != KErrNone)
        throw error;
}

// ---------------------------------------------------------------------------
// CPIMItem::GetInt
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
int CPIMItem::getInt(TPIMField aField, int aIndex) const
{
    JELOG2(EPim);
    TInt error = KErrNone;
    int retValue = 0;
    TRAP(error,
    {
        LeaveIfInvalidOrUnsupportedFieldL(aField, EPIMFieldInt);
        // Load field if it is not loaded yet
        LoadFieldL(aField);
        retValue = iItemData->ValueL(aField, aIndex).IntegerValue();
    });
    if (error != KErrNone)
        throw error;
    return retValue;
}

// ---------------------------------------------------------------------------
// CPIMItem::AddInt
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::addInt(TPIMField aField,
                      TPIMAttribute aAttributes, int aValue)
{
    JELOG2(EPim);
    TInt error = KErrNone;
    TRAP(error,
    {
        LeaveIfInvalidOrUnsupportedFieldL(aField, EPIMFieldInt);

        // Load field if it is not loaded yet
        LoadFieldL(aField);

        if (iAdapterManager)
        {
            // Note that we must compare for equality (not equal or greater)
            // because MaxValues might return KPIMUnlimitedValues (negative).
            if (iItemData->CountValues(aField) == iAdapterManager->MaxValues(
                        aField))
            {
                User::Leave(KErrOverflow);
            }
        }

        __ASSERT_ALWAYS(IsValidIntegerValue(aField, aValue),
                        User::Leave(KErrTooBig));    // codescanner::leave
        __ASSERT_ALWAYS(!IsReadOnly(aField), User::Leave(KErrLocked));

        aAttributes = ClearUnsupportedAttributesL(aAttributes, aField);   // codescanner::leave
        TPIMFieldData fieldData(aField, fieldDataType(aField),
                                aAttributes, aValue);
        iItemData->AddValueL(fieldData);   // codescanner::leave

        SetModified();
    });
    if (error != KErrNone)
        throw error;
}

// ---------------------------------------------------------------------------
// CPIMItem::SetInt
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::setInt(TPIMField aField, int aIndex,
                      TPIMAttribute aAttributes, int aValue)
{
    JELOG2(EPim);
    TInt error = KErrNone;
    TRAP(error,
    {
        LeaveIfInvalidOrUnsupportedFieldL(aField, EPIMFieldInt);

        // Load field if it is not loaded yet
        LoadFieldL(aField);

        __ASSERT_ALWAYS(IsValidIntegerValue(aField, aValue),
                        User::Leave(KErrTooBig));    // codescanner::leave
        __ASSERT_ALWAYS(!IsReadOnly(aField), User::Leave(KErrLocked));

        aAttributes = ClearUnsupportedAttributesL(aAttributes, aField);
        TPIMFieldData fieldData(aField, fieldDataType(aField),
                                aAttributes, aValue);
        fieldData.SetIndex(aIndex);
        iItemData->SetValueL(fieldData);
        SetModified();
    });
    if (error != KErrNone)
        throw error;
}

// ---------------------------------------------------------------------------
// CPIMItem::GetStringL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
const TDesC& CPIMItem::GetStringL(TPIMField aField,
                                  TInt aIndex) const
{
    JELOG2(EPim);
    LeaveIfInvalidOrUnsupportedFieldL(aField,
                                      EPIMFieldString);
    // Load field if it is not loaded yet
    LoadFieldL(aField);
    return iItemData->ValueL(aField, aIndex).StringValue();
}

jstring CPIMItem::callGetString(TPIMField aField,
                                int aIndex, JNIEnv* aJniEnv, jintArray aError)
{
    JELOG2(EPim);
    const TDesC* value = NULL;
    TInt error = KErrNone;
    TRAP(error, value = &(GetStringL(aField, aIndex)));
    SetJavaErrorCode(aJniEnv, aError, error);

    if (error != KErrNone)
    {
        return NULL;
    }

    jstring javaValue = java::util::S60CommonUtils::NativeToJavaString(
                            *aJniEnv, *value);

    if (!javaValue)
    {
        SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
        return NULL;
    }
    return javaValue;

}

// ---------------------------------------------------------------------------
// CPIMItem::AddStringL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::AddStringL(TPIMField aField,
                          TPIMAttribute aAttributes, HBufC* aValue)
{
    JELOG2(EPim);
    LeaveIfInvalidOrUnsupportedFieldL(aField, EPIMFieldString);

    // Load field if it is not loaded yet
    LoadFieldL(aField);

    __ASSERT_ALWAYS(aValue, User::Leave(KErrArgument));

    if (iAdapterManager)
    {
        // Note that we must compare for equality (not equal or greater)
        // because MaxValues might return KPIMUnlimitedValues (negative).
        if (iItemData->CountValues(aField) == iAdapterManager->MaxValues(aField))
        {
            User::Leave(KErrOverflow);
        }
    }

    __ASSERT_ALWAYS(IsValidStringValue(aField, *aValue),
                    User::Leave(KErrTooBig));
    __ASSERT_ALWAYS(!IsReadOnly(aField), User::Leave(KErrLocked));

    aAttributes = ClearUnsupportedAttributesL(aAttributes, aField);
    TPIMFieldData fieldData(aField, aAttributes, aValue);
    iItemData->AddValueL(fieldData);
    SetModified();
}

void CPIMItem::callAddString(TPIMField aField,
                             TPIMAttribute aAttributes, jstring aValue,
                             JNIEnv* aJniEnv)
{
    JELOG2(EPim);
    TInt error = KErrNone;
    TRAP(error,
    {
        __ASSERT_DEBUG(aValue,
        User::Panic(KPIMPanicCategory, EPIMPanicNullArgument));
        JStringUtils nativeValue(*aJniEnv, aValue);
        TDesC* string = static_cast< TDesC*>(&nativeValue);
        HBufC* value = string->AllocLC();
        AddStringL(aField, aAttributes, value);
        CleanupStack::Pop(value);
    }
        );
    if (error != KErrNone)
        throw error;
}

// ---------------------------------------------------------------------------
// CPIMItem::SetStringL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::SetStringL(TPIMField aField,
                          TInt aIndex, TPIMAttribute aAttributes,
                          HBufC* aValue)
{
    JELOG2(EPim);
    LeaveIfInvalidOrUnsupportedFieldL(aField, EPIMFieldString);

    // Load field if it is not loaded yet
    LoadFieldL(aField);

    __ASSERT_ALWAYS(aValue, User::Leave(KErrArgument));
    __ASSERT_ALWAYS(IsValidStringValue(aField, *aValue), User::Leave(KErrTooBig));
    __ASSERT_ALWAYS(!IsReadOnly(aField), User::Leave(KErrLocked));

    aAttributes = ClearUnsupportedAttributesL(aAttributes, aField);
    TPIMFieldData fieldData(aField, aAttributes, aValue);
    fieldData.SetIndex(aIndex);
    iItemData->SetValueL(fieldData);
    SetModified();
}

void CPIMItem::callSetString(TPIMField aField,
                             int aIndex, TPIMAttribute aAttributes,
                             jstring aValue, JNIEnv* aJniEnv)
{
    JELOG2(EPim);
    TInt error = KErrNone;
    TRAP(error,
    {
        __ASSERT_DEBUG(aValue,
        User::Panic(KPIMPanicCategory, EPIMPanicNullArgument));
        JStringUtils nativeValue(*aJniEnv, aValue);
        TDesC* string = static_cast< TDesC*>(&nativeValue);
        HBufC* value = string->AllocLC();
        SetStringL(aField, aIndex, aAttributes, value);
        CleanupStack::Pop(value);
    }
        );
    if (error != KErrNone)
        throw error;
}

// ---------------------------------------------------------------------------
// CPIMItem::GetBooleanL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TBool CPIMItem::GetBooleanL(TPIMField aField,
                            TInt aIndex) const
{
    JELOG2(EPim);
    LeaveIfInvalidOrUnsupportedFieldL(aField,
                                      EPIMFieldBoolean);
    // Load field if it is not loaded yet
    LoadFieldL(aField);
    return iItemData->ValueL(aField, aIndex).BooleanValue();
}

bool CPIMItem::callGetBoolean(
    TPIMField aField, int aIndex)
{
    JELOG2(EPim);
    TInt error = KErrNone;
    TBool retVal = EFalse;
    TRAP(error,
    {
        retVal = GetBooleanL(aField, aIndex);
    }
        );
    if (error != KErrNone)
        throw error;
    if (retVal)
        return true;
    return false;
}

// ---------------------------------------------------------------------------
// CPIMItem::AddBooleanL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::AddBooleanL(
    TPIMField aField,
    TPIMAttribute aAttributes, TBool aValue)
{
    JELOG2(EPim);
    LeaveIfInvalidOrUnsupportedFieldL(aField, EPIMFieldBoolean);

    // Load field if it is not loaded yet
    LoadFieldL(aField);

    if (iAdapterManager)
    {
        // Note that we must compare for equality (not equal or greater)
        // because MaxValues might return KPIMUnlimitedValues (negative).
        if (iItemData->CountValues(aField) == iAdapterManager->MaxValues(
                    aField))
        {
            User::Leave(KErrOverflow);
        }
    }

    __ASSERT_ALWAYS(IsValidBooleanValue(aField, aValue), User::Leave(
                        KErrTooBig));
    __ASSERT_ALWAYS(!IsReadOnly(aField), User::Leave(KErrLocked));

    aAttributes = ClearUnsupportedAttributesL(aAttributes, aField);
    TPIMFieldData fieldData(aField, fieldDataType(aField), aAttributes, aValue);
    iItemData->AddValueL(fieldData);

    SetModified();
}

void CPIMItem::callAddBoolean(
    TPIMField aField,
    TPIMAttribute aAttributes,
    jboolean aValue)
{
    JELOG2(EPim);
    TBool value =
        static_cast<TBool>(aValue);
    TInt error = KErrNone;
    TRAP(error,
    {
        AddBooleanL(aField, aAttributes, value);
    }
        );
    if (error != KErrNone)
        throw error;
}

// ---------------------------------------------------------------------------
// CPIMItem::SetBooleanL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::SetBooleanL(
    TPIMField aField, TInt aIndex,
    TPIMAttribute aAttributes,
    TBool aValue)
{
    JELOG2(EPim);
    LeaveIfInvalidOrUnsupportedFieldL(aField, EPIMFieldBoolean);

    // Load field if it is not loaded yet
    LoadFieldL(aField);

    __ASSERT_ALWAYS(IsValidBooleanValue(aField, aValue), User::Leave(
                        KErrTooBig));
    __ASSERT_ALWAYS(!IsReadOnly(aField), User::Leave(KErrLocked));

    aAttributes = ClearUnsupportedAttributesL(aAttributes, aField);
    TPIMFieldData fieldData(aField, fieldDataType(aField), aAttributes, aValue);
    fieldData.SetIndex(aIndex);
    iItemData->SetValueL(fieldData);
    SetModified();
}

void CPIMItem::callSetBoolean(
    TPIMField aField, int aIndex,
    TPIMAttribute aAttributes,
    jboolean aValue)
{
    JELOG2(EPim);
    TInt error = KErrNone;
    TRAP(error,
    {
        TBool value = static_cast< TBool>(aValue);
        SetBooleanL(aField, aIndex, aAttributes, value);
    }
        );
    if (error != KErrNone)
        throw error;
}

// ---------------------------------------------------------------------------
// CPIMItem::GetStringArrayL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
const CDesCArray& CPIMItem::GetStringArrayL(
    TPIMField aField, TInt aIndex) const
{
    JELOG2(EPim);
    LeaveIfInvalidOrUnsupportedFieldL(aField, EPIMFieldStringArray);
    // Load field if it is not loaded yet
    LoadFieldL(aField);
    return iItemData->ValueL(aField, aIndex).StringArrayValue();
}

jobjectArray CPIMItem::callGetStringArray(
    TPIMField aField, int aIndex,
    JNIEnv* aJniEnv,
    jintArray aError)
{
    JELOG2(EPim);
    const CDesCArray * value = NULL;
    TInt error = 0;

    TRAP(error, value = &(GetStringArrayL(aField, aIndex)));
    SetJavaErrorCode(aJniEnv, aError, error);

    if (error != KErrNone)
    {
        return NULL;
    }

    jobjectArray javaStringArray = CreateJavaStringArray(aJniEnv, *value,
                                   EFalse); // (DO NOT convert KPIMNullArrayElement elements to NULL)

    if (!javaStringArray)
    {
        SetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
    }
    return javaStringArray;
}

// ---------------------------------------------------------------------------
// CPIMItem::AddStringArrayL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::AddStringArrayL(
    TPIMField aField,
    TPIMAttribute aAttributes,
    CDesCArray* aValue)
{
    JELOG2(EPim);
    LeaveIfInvalidOrUnsupportedFieldL(aField, EPIMFieldStringArray);

    // Load field if it is not loaded yet
    LoadFieldL(aField);

    if (iAdapterManager)
    {
        // Note that we must compare for equality (not equal or greater)
        // because MaxValues might return KPIMUnlimitedValues (negative).
        if (iItemData->CountValues(aField) == iAdapterManager->MaxValues(
                    aField))
        {
            User::Leave(KErrOverflow);
        }
    }

    __ASSERT_ALWAYS(aValue, User::Leave(KErrArgument));
    __ASSERT_ALWAYS(NonNullElements(*aValue),
                    User::Leave(KErrArgument));
    __ASSERT_ALWAYS(IsValidStringArrayValue(aField, *aValue),
                    User::Leave(KErrTooBig));
    __ASSERT_ALWAYS(!IsReadOnly(aField), User::Leave(KErrLocked));

    aAttributes = ClearUnsupportedAttributesL(aAttributes, aField);

    CDesCArray * cleanValue = DropUnsupportedElementsL(aField, *aValue);
    CleanupStack::PushL(cleanValue);

    TPIMFieldData fieldData(aField, aAttributes, cleanValue);
    iItemData->AddValueL(fieldData);

    CleanupStack::Pop(cleanValue);
    SetModified();

    delete aValue;
}

void CPIMItem::callAddStringArray(
    JNIEnv* aJniEnv, jint aField,
    jint aAttributes,
    jobjectArray aValue)
{
    JELOG2(EPim);
    // Note that in J9, aValue must be a global reference since the VM
    // checks for invalid references if passed between two threads
    TInt error = KErrNone;
    TRAP(error,
    {
        CDesCArray* nativeValue = CreateNativeStringArrayL(aJniEnv, aValue);

        // We now own the native string array.
        CleanupStack::PushL(nativeValue);

        // Ownership of the string array is transferred.
        AddStringArrayL(aField, aAttributes, nativeValue);

        CleanupStack::Pop(nativeValue);
    }
        );
    if (error != KErrNone)
        throw error;
}

// ---------------------------------------------------------------------------
// CPIMItem::SetStringArrayL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::SetStringArrayL(
    TPIMField aField,
    TInt aIndex,
    TPIMAttribute aAttributes,
    CDesCArray* aValue)
{
    JELOG2(EPim);
    LeaveIfInvalidOrUnsupportedFieldL(aField, EPIMFieldStringArray);

    // Load field if it is not loaded yet
    LoadFieldL(aField);

    __ASSERT_ALWAYS(aValue, User::Leave(KErrArgument));
    __ASSERT_ALWAYS(NonNullElements(*aValue), User::Leave(KErrArgument));
    __ASSERT_ALWAYS(IsValidStringArrayValue(aField, *aValue),
                    User::Leave(KErrTooBig));
    __ASSERT_ALWAYS(!IsReadOnly(aField), User::Leave(KErrLocked));

    aAttributes = ClearUnsupportedAttributesL(aAttributes, aField);

    CDesCArray* cleanValue = DropUnsupportedElementsL(aField, *aValue);
    CleanupStack::PushL(cleanValue);

    TPIMFieldData fieldData(aField, aAttributes, cleanValue);
    fieldData.SetIndex(aIndex);
    iItemData->SetValueL(fieldData);

    CleanupStack::Pop(cleanValue);
    SetModified();

    delete aValue;
}

void CPIMItem::callSetStringArray(
    JNIEnv* aJniEnv,
    jint aField, jint aIndex,
    jint aAttributes,
    jobjectArray aValue)
{
    JELOG2(EPim);
    TInt error = KErrNone;
    TRAP(error,
    {
        CDesCArray* nativeValue = CreateNativeStringArrayL(aJniEnv, aValue);

        // We now own the string array.
        CleanupStack::PushL(nativeValue);

        // Ownership of the string array is transferred.
        SetStringArrayL(aField, aIndex, aAttributes, nativeValue);

        CleanupStack::Pop(nativeValue);
    }
        );
    if (error != KErrNone)
        throw error;
}

// ---------------------------------------------------------------------------
// CPIMItem::CountValuesL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TInt CPIMItem::CountValuesL(
    TPIMField aField) const
{
    JELOG2(EPim);
    LeaveIfInvalidOrUnsupportedFieldL(
        aField);
    // Load field if it is not loaded yet
    LoadFieldL(aField);
    return iItemData->CountValues(aField);
}

int CPIMItem::callCountValues(
    TPIMField aField) const
{
    JELOG2(EPim);
    TInt error = KErrNone;
    int retVal = 0;
    TRAP(error, retVal = CountValuesL(aField));
    if (error != KErrNone)
        throw error;
    return retVal;
}
// ---------------------------------------------------------------------------
// CPIMItem:: RemoveValueL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::removeValue(
    TPIMField aField,
    int aIndex)
{
    JELOG2(EPim);
    TInt error = KErrNone;
    TRAP(error,
    {
        LeaveIfInvalidOrUnsupportedFieldL(aField);

        // Load the full item if it has not been loaded yet. This has to be done
        // because it is not possible to commit partial item correctly if the client
        // has removed some values from it before getting all the fields from the item

        LoadFullItemL();

        if (aIndex < 0 || aIndex >= iItemData->CountValues(aField))
        {
            User::Leave(KErrNotFound);
        }

        // Leave if the item is read-only (i.e is associated with a list)
        __ASSERT_ALWAYS(!IsReadOnly(aField), User::Leave(KErrLocked));

        iItemData->RemoveValueL(aField, aIndex);
        SetModified();
    }
        );
    if (error != KErrNone)
        throw error;
}

// ---------------------------------------------------------------------------
// CPIMItem::GetAttributes
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TPIMAttribute CPIMItem::getAttributes(
    TPIMField aField,
    int aIndex) const
{
    JELOG2(EPim);
    TInt error = KErrNone;
    TPIMAttribute retVal = 0;
    TRAP(error,
    {
        LeaveIfInvalidOrUnsupportedFieldL(aField);
        // Load field if it is not loaded yet
        LoadFieldL(aField);
        retVal = iItemData->AttributesL(aField, aIndex);
    }
        );
    if (error != KErrNone)
        throw error;
    return retVal;
}

// ---------------------------------------------------------------------------
// CPIMItem::AddToCategoryL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::AddToCategoryL(
    const TDesC& aCategory)
{
    JELOG2(EPim);
    // The item must be fully loaded at this point because if partial item
    // is committed it may cause that the added category information will be
    // lost when the partial item is cached
    LoadFullItemL();

    if (BelongsToCategory(
                aCategory))
    {
        return; // Nothing to do
    }

    if (iListAdapter)
    {
        // Associated with an adapter
        // -> Check whether such category exists in the list
        TInt matchingIndex = 0;
        if (0 == iListAdapter->GetCategoriesL(). Find(
                    aCategory,
                    matchingIndex,
                    ECmpNormal))
        {
            // There is such category in the list
            iItemData->AddNewCategoryL(aCategory);
        }
        else
        {
            // There is no such category in the list
            User::Leave(KErrArgument);
        }
    }
    else
    {
        // Not associated with an adapter
        // -> Just add the category
        iItemData->AddNewCategoryL(aCategory);
    }

    SetModified();
}

void CPIMItem::callAddToCategory(
    jstring aCategory,
    JNIEnv* aJniEnv)
{
    const JStringUtils
    nativeCategory(*aJniEnv, aCategory);
    const TDesC * category =
        static_cast<const TDesC*>(&nativeCategory);
    TInt error = KErrNone;
    TRAP(error, AddToCategoryL(*category));
    if (error != KErrNone)
        throw error;
}

// ---------------------------------------------------------------------------
// CPIMItem::RemoveFromCategory
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::RemoveFromCategory(
    const TDesC& aCategory)
{
    JELOG2(EPim);
    // The item must be fully loaded at this point because if partial item
    // is committed it may cause that the removed category will be loaded
    // from the store and this invocation will not modify the item correctly
    TRAP_IGNORE(LoadFullItemL());

    // The error is ignored because out of memory is the only possible situation where
    // LoadFullItemL() leaves. Still, removing the category can be successfully
    // done and there is no reason to throw exception in the Java side

    if (iItemData->RemoveCategory(aCategory))
    {
        SetModified();
    }
}

void CPIMItem::callRemoveFromCategory(
    jstring aCategory,
    JNIEnv* aJniEnv)
{
    JELOG2(EPim);
    const JStringUtils nativeCategory(*aJniEnv, aCategory);
    const TDesC * category = static_cast<const TDesC*>(&nativeCategory);
    RemoveFromCategory(*category);
}
// ---------------------------------------------------------------------------
// CPIMItem::GetCategoriesL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
const CDesCArray& CPIMItem::GetCategoriesL() const
{
    JELOG2(EPim);
    return iItemData->Categories();
}

jobjectArray CPIMItem::callGetCategories(
    JNIEnv* aJniEnv)
{
    JELOG2(EPim);
    const CDesCArray * categories = NULL;
    TInt error = KErrNone;
    TRAP(error, categories = &(GetCategoriesL()));
    if (error != KErrNone)
    {
        return NULL;
    }

    jobjectArray javaCategories = CreateJavaStringArray(aJniEnv,
                                  *categories,
                                  EFalse); // (do not handle KPIMNullArrayElement elements specially)
    return javaCategories;
}

// ---------------------------------------------------------------------------
// CPIMItem::MaxCategories
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
int CPIMItem::maxCategories()
{
    JELOG2(EPim);
    if (iAdapterManager)
    {
        // Associated with an adapter
        return iAdapterManager->MaxCategories();
    }

    // Otherwise not associated with an adapter
    return KPIMUnlimitedCategories;
}

// ---------------------------------------------------------------------------
// CPIMItem::SetLastModifiedL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::SetLastModifiedL(
    TPIMDate aLastModified)
{
    JELOG2(EPim);
    iLastModified = aLastModified;
}

// ---------------------------------------------------------------------------
// CPIMItem::LastModified
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TPIMDate CPIMItem::LastModified() const
{
    JELOG2(EPim);
    return iLastModified;
}

// ---------------------------------------------------------------------------
// CPIMItem::GetId
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TPIMItemID CPIMItem::GetId() const
{
    JELOG2(EPim);
    if (!iItemID)
    {
        return KPIMNullItemID();
    }
    return iItemID->Des();
}

// ---------------------------------------------------------------------------
// CPIMItem::SetIdL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::SetIdL(
    TPIMItemID aItemID)
{
    JELOG2(EPim);
    HBufC8* itemId = aItemID.AllocL();
    delete iItemID;
    iItemID = itemId;
}

// ---------------------------------------------------------------------------
// CPIMItem::IsReadOnly
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TBool CPIMItem::IsReadOnly(
    const TPIMField& /*aField*/)
{
    JELOG2(EPim);
    return EFalse;
}

// ---------------------------------------------------------------------------
// CPIMItem::SetBaseAdapterAssociation
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::SetBaseAdapterAssociation(
    MPIMAdapterManager* aAdapterManager,
    MPIMListAdapter* aListAdapter)
{
    JELOG2(EPim);
    __ASSERT_DEBUG(aAdapterManager,
                   User::Panic(KPIMPanicCategory, EPIMPanicNullArgument));

    iAdapterManager = aAdapterManager;
    iListAdapter = aListAdapter;
}

// ---------------------------------------------------------------------------
// CPIMItem::BelongsToCategory
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TBool CPIMItem::BelongsToCategory(
    const TDesC& aCategory)
{
    JELOG2(EPim);
    if (iItemData->FindCategory(aCategory) != KErrNotFound)
    {
        return ETrue;
    }

    return EFalse;
}

// ---------------------------------------------------------------------------
// CPIMItem::LeaveIfInvalidOrUnsupportedFieldL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::LeaveIfInvalidOrUnsupportedFieldL(
    const TPIMField aField,
    const TPIMFieldDataType aDataType) const
{
    JELOG2(EPim);
    // Leave if the data type of the field is incorrect
    __ASSERT_ALWAYS(fieldDataType(aField) == aDataType,
                    User::Leave(KErrArgument));
    LeaveIfInvalidOrUnsupportedFieldL(aField);
}

// ---------------------------------------------------------------------------
// CPIMItem::LeaveIfInvalidOrUnsupportedFieldL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::LeaveIfInvalidOrUnsupportedFieldL(
    const TPIMField aField) const
{
    JELOG2(EPim);
    // Leave if this field is invalid
    __ASSERT_ALWAYS(iValidator.IsValidField(aField),
                    User::Leave(KErrArgument));

    // Leave if this field is not supported by this adapter manager
    if (iAdapterManager
            && !iAdapterManager->IsSupportedField(aField))
    {
        User::Leave(KErrNotSupported);
    }
}

// ---------------------------------------------------------------------------
// CPIMItem::DropUnsupportedElementsL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
CDesCArray* CPIMItem::DropUnsupportedElementsL(
    const TPIMField aStringArrayField,
    const CDesCArray& aStringArray)
{
    JELOG2(EPim);
    const TInt len = aStringArray.Count();
    CDesCArray * retVal = new(ELeave) CDesCArrayFlat(len);
    CleanupStack::PushL(retVal);
    for (TInt i = 0; i < len; i++)
    {
        if (iAdapterManager)
        {
            if (iAdapterManager->IsSupportedArrayElement(aStringArrayField, i))
            {
                retVal->AppendL(aStringArray[i]);
            }
            else
            {
                retVal->AppendL(KPIMNullArrayElement);
            }
        }
        else // !iAdapterManager
        {
            retVal->AppendL(aStringArray[i]);
        }
    }

    CleanupStack::Pop(retVal);
    return retVal;
}

// ---------------------------------------------------------------------------
// CPIMItem::PrepareForLoadL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMItem::PrepareForLoadL()
{
    JELOG2(EPim);
    // Reset initialized fields if there is a need for this
    if (iInitializedFields)
    {
        iInitializedFields->Reset();
    }
    else
    {
        iInitializedFields = new(ELeave) CArrayFixFlat< TPIMField> (5);
    }
    // Reset the item data to its original state
    iItemData->Reset();
}

// ---------------------------------------------------------------------------
// CPIMItem::ClearUnsupportedAttributesL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TPIMAttribute CPIMItem::ClearUnsupportedAttributesL(
    const TPIMAttribute& aAttributes,
    const TPIMField& aField)
{
    JELOG2(EPim);
    // Initialize the return set by the argument
    TPIMAttribute retAttr = aAttributes;

    // Clear off attributes that are not valid for the concrete
    // item class
    const TPIMAttribute validAttributes = ValidAttributes();
    retAttr &= validAttributes;

    // If the item is associated with a list, clear off attributes
    // that are not supported for the field by the list
    if (iAdapterManager)
    {
        const TPIMAttribute supportedAttributes =
            iAdapterManager->GetSupportedAttributesCombinedL(aField);

        retAttr &= supportedAttributes;
    }

    return retAttr;
}

// ---------------------------------------------------------------------------
// CPIMItem::NonNullElements
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TBool CPIMItem::NonNullElements(
    const CDesCArray& aStringArray) const
{
    JELOG2(EPim);
    const TInt numElements = aStringArray.Count();

    for (TInt i = 0; i < numElements; i++)
    {
        if (aStringArray[i] != KPIMNullArrayElement)
        {
            // At least one of the elements is marked non-null
            return ETrue;
        }
    }

    // All elemens were marked null
    return EFalse;
}

//  End of File´
