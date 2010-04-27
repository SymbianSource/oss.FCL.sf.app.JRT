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
* Description:  Provides static information about todo lists and creates
 *                todo list adapters.
 *
*/


// CLASS HEADER
#include "cpimtodoadaptermanager.h"

// INTERNAL INCLUDES
#include "mpimitem.h"
#include "cpimtodolistadapter.h"
#include "pimlocalizationids.h"
#include "logger.h"

// -----------------------------------------------------------------------------
// CPIMToDoAdapterManager::CPIMToDoAdapterManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMToDoAdapterManager::CPIMToDoAdapterManager()
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMToDoAdapterManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMToDoAdapterManager::ConstructL(const TDesC& aListName)
{
    JELOG2(EPim);
    iListName = aListName.AllocL();
}

// -----------------------------------------------------------------------------
// CPIMToDoAdapterManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMToDoAdapterManager* CPIMToDoAdapterManager::NewL(const TDesC& aListName)
{
    JELOG2(EPim);
    CPIMToDoAdapterManager* self = new(ELeave) CPIMToDoAdapterManager;

    CleanupStack::PushL(self);
    self->ConstructL(aListName);
    CleanupStack::Pop(self);

    return self;
}

// Destructor
CPIMToDoAdapterManager::~CPIMToDoAdapterManager()
{
    JELOG2(EPim);
    delete iSupportedToDoFields;
    delete iEmptyToDoAttributeArray;
    delete iListName;
}

// -----------------------------------------------------------------------------
// CPIMToDoAdapterManager::GetAdapterManager
// Provides access to the MPIMAdapterManager representation of this object.
// Returns: MPIMAdapterManager representation of this object.
// -----------------------------------------------------------------------------
MPIMAdapterManager* CPIMToDoAdapterManager::GetAdapterManager()
{
    JELOG2(EPim);
    return this;
}

const TDesC& CPIMToDoAdapterManager::ListNameL()
{
    JELOG2(EPim);
    return *iListName;
}

// -----------------------------------------------------------------------------
// CPIMToDoAdapterManager::MaxCategories
// Returns: The maximum number of categories that this list can have.
//          0 indicates no category support,
//         -1 indicates there is no limit on the number of categories that this
//            list can have
// -----------------------------------------------------------------------------
//

TInt CPIMToDoAdapterManager::MaxCategories()
{
    JELOG2(EPim);
    return 0;
}

// -----------------------------------------------------------------------------
// CPIMToDoAdapterManager::IsSupportedField
// Returns: ETrue if supported in this list, EFalse otherwise. Invalid fields
//          return false.
// -----------------------------------------------------------------------------
//

TBool CPIMToDoAdapterManager::IsSupportedField(TPIMField aField)
{
    JELOG2(EPim);
    for (TInt index = 0; index < KPIMSupportedToDoFieldsCount; index++)
    {
        if (aField == KPIMSupportedToDoFields[index])
        {
            return ETrue;
        }
    }
    return EFalse;
}

// -----------------------------------------------------------------------------
// CPIMToDoAdapterManager::GetSupportedFieldsL
// Returns: An int array containing all fields supported by this list.
// -----------------------------------------------------------------------------
//

const CArrayFix<TPIMField>& CPIMToDoAdapterManager::GetSupportedFieldsL()
{
    JELOG2(EPim);
    if (!iSupportedToDoFields)
    {
        iSupportedToDoFields
        = new(ELeave) CArrayFixFlat<TPIMField> (KPIMSupportedToDoFieldsCount);  // codescanner::leave

        iSupportedToDoFields->AppendL(KPIMSupportedToDoFields, // codescanner::leave
                                      KPIMSupportedToDoFieldsCount);
    }
    return *iSupportedToDoFields;
}

// -----------------------------------------------------------------------------
// CPIMToDoAdapterManager::IsSupportedAttribute
// Returns: EFalse for every other attribute than KPIMAttrNone
// -----------------------------------------------------------------------------
//

TBool CPIMToDoAdapterManager::IsSupportedAttribute(TPIMField /* aField */,
        TPIMAttribute /* aAttribute */)
{
    JELOG2(EPim);
    return EFalse; // No attributes supported
}

// -----------------------------------------------------------------------------
// CPIMToDoAdapterManager::GetSupportedAttributesL
// Returns: An empty array meaning no supported attributes.
// -----------------------------------------------------------------------------
//

const CArrayFix<TPIMAttribute>&
CPIMToDoAdapterManager::GetSupportedAttributesL(TPIMField aField)
{
    JELOG2(EPim);
    if (!IsSupportedField(aField))
    {
        User::Leave(KErrArgument);
    }

    if (!iEmptyToDoAttributeArray)
    {
        iEmptyToDoAttributeArray
        = new(ELeave) CArrayFixFlat<TPIMAttribute> (1);
    }
    return *iEmptyToDoAttributeArray;
}

// -----------------------------------------------------------------------------
// CPIMToDoAdapterManager::GetSupportedAttributesCombinedL
// Returns: KPIMAttrNone meaning no supported attributes.
// -----------------------------------------------------------------------------
//

TPIMAttribute CPIMToDoAdapterManager::GetSupportedAttributesCombinedL(
    TPIMField aField) // The field to check the supported attributes from
{
    JELOG2(EPim);
    if (!IsSupportedField(aField))
    {
        User::Leave(KErrArgument); // Field was not a valid field
    }
    return KPIMAttrNone;
}

// -----------------------------------------------------------------------------
// CPIMToDoAdapterManager::GetAllSupportedAttributesCombined
// Returns: KPIMAttrNone meaning no supported attributes.
// -----------------------------------------------------------------------------
//

TPIMAttribute CPIMToDoAdapterManager::GetAllSupportedAttributesCombined()
{
    JELOG2(EPim);
    return KPIMAttrNone;
}

// -----------------------------------------------------------------------------
// CPIMToDoAdapterManager::IsSupportedArrayElement
// Returns: EFalse, as there are no supported array elements in ToDo list.
// -----------------------------------------------------------------------------
//

TBool CPIMToDoAdapterManager::IsSupportedArrayElement(
    TPIMField /* aStringArrayField */, TPIMArrayElement /* aArrayElement */)
{
    JELOG2(EPim);
    return EFalse; // No attributes supported
}

// -----------------------------------------------------------------------------
// CPIMToDoAdapterManager::GetSupportedArrayElementsL
// Returns: An integer array containing all of the supported elements of a
//          string array for the given field.
// -----------------------------------------------------------------------------
//

const CArrayFix<TPIMArrayElement>&
CPIMToDoAdapterManager::GetSupportedArrayElementsL(TPIMField /* aField */) // The field to check for supported array elements
{
    JELOG2(EPim);
    User::Leave(KErrArgument); // No array elements in ToDo list.

    // satisfy the compiler
    const CArrayFix<TPIMArrayElement>* temp = NULL;
    return *temp;
}

// -----------------------------------------------------------------------------
// CPIMToDoAdapterManager::MaxValues
// Returns: Number of values supported for the field
//          -1 : The field supports having an unlimited number of values in it.
//           0 : The field is not supported by this list.
// -----------------------------------------------------------------------------
//

TInt CPIMToDoAdapterManager::MaxValues(TPIMField aField) // The field to check for multiple value support
{
    JELOG2(EPim);
    if (IsSupportedField(aField))
    {
        return 1; // Only one value is supported for any valid ToDo field
    }
    else
    {
        return 0;
    }
}

// -----------------------------------------------------------------------------
// CPIMToDoAdapterManager::StringArraySizeL
// Returns: Int the size of the array of a single data instance of a string
//          array field.
// -----------------------------------------------------------------------------
//

TInt CPIMToDoAdapterManager::StringArraySizeL(TPIMField /* aStringArrayField */)
{
    JELOG2(EPim);
    User::Leave(KErrArgument); // no string array fields

    // satisfy the compiler
    return 0;
}

// -----------------------------------------------------------------------------
// CPIMToDoAdapterManager::ItemOrder
// Returns: pointer to a function which implements an algorithm that
//          determines the order of two items.
// -----------------------------------------------------------------------------
//

TPIMItemComparisonFunc CPIMToDoAdapterManager::ItemOrder()
{
    JELOG2(EPim);
    return ItemComparisonFunc;
}

// -----------------------------------------------------------------------------
// CPIMToDoAdapterManager::ItemComparisonFuncL
// Compares two MPIMItemData objects. The comparision is based primarily on the
// completion of the item, secondly on the due date of the item, and thirdly
// on the priority of the item.
// Returns: < 0: aFirst < aSecond
//          0: aFirst == aSecond
//          > 0: aFirst > aSecond
// -----------------------------------------------------------------------------
//
TInt CPIMToDoAdapterManager::ItemComparisonFunc(const MPIMItem& aFirst,
        const MPIMItem& aSecond)
{
    JELOG2(EPim);
    TInt retVal = 0;
    TRAPD(err, retVal = ItemComparisonFuncL(aFirst, aSecond));
    if (err != KErrNone)
    {
        // The comparison function does not actually leave as it is used with
        // correct PIM field parameters, so this assert here is just for debug
        __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory, EPIMPanicGeneral));
    }
    return retVal;
}

// -----------------------------------------------------------------------------
// CPIMToDoAdapterManager::ItemComparisonFuncL
// Compares two MPIMItemData objects. The comparision is based primarily on the
// completion of the item, secondly on the due date of the item, and thirdly
// on the priority of the item.
// Returns: < 0: aFirst < aSecond
//          0: aFirst == aSecond
//          > 0: aFirst > aSecond
// -----------------------------------------------------------------------------
//
TInt CPIMToDoAdapterManager::ItemComparisonFuncL(const MPIMItem& aFirst,
        const MPIMItem& aSecond)
{
    JELOG2(EPim);
    TInt firstPresent = aFirst.CountValuesL(EPIMToDoCompleted);
    TInt secondPresent = aSecond.CountValuesL(EPIMToDoCompleted);

    if ((firstPresent == 0) ^(secondPresent == 0))
        // Only one of the fields is present
    {
        return firstPresent - secondPresent;
    }

    else if (firstPresent > 0 && secondPresent > 0)
        // Both fields are present
    {
        TBool completedFirst = aFirst.GetBooleanL(EPIMToDoCompleted, 0);
        TBool completedSecond = aSecond.GetBooleanL(EPIMToDoCompleted, 0);

        if (!completedFirst && completedSecond)
        {
            return 1;
        }
        else if (completedFirst && !completedSecond)
        {
            return -1;
        }
    }

    // In this point either none of the items have the completed field,
    // or both of the items have the field and also have the same value in it.
    // Continue the comparision based on the 2nd criteria, the due date field.
    firstPresent = aFirst.CountValuesL(EPIMToDoDue);
    secondPresent = aSecond.CountValuesL(EPIMToDoDue);

    if ((firstPresent == 0) ^(secondPresent == 0))  // xor
        // Only one of the fields is present
    {
        return secondPresent - firstPresent;
    }

    else if (firstPresent > 0 && secondPresent > 0)
        // Both fields are present
    {
        const TPIMDate dateFirst = aFirst.GetDateL(EPIMToDoDue, 0);
        const TPIMDate dateSecond = aSecond.GetDateL(EPIMToDoDue, 0);

        if (dateFirst < dateSecond)
        {
            return -1;
        }
        else if (dateFirst > dateSecond)
        {
            return 1;
        }
    }

    // In this point either none of the items have due date field, or
    // the other possibility is that the due date fields are exactly equal.
    // Continue the comparision based on the 3rd criteria, the priority field.
    firstPresent = aFirst.CountValuesL(EPIMToDoPriority);
    secondPresent = aSecond.CountValuesL(EPIMToDoPriority);

    if ((firstPresent == 0) ^(secondPresent == 0))  // xor
        // Only one of the fields is present
    {
        return secondPresent - firstPresent;
    }

    else if (firstPresent > 0 && secondPresent > 0)
        // Both fields are present
    {
        TInt priorityFirst = aFirst.getInt(EPIMToDoPriority, 0);
        TInt prioritySecond = aSecond.getInt(EPIMToDoPriority, 0);
        if (priorityFirst < prioritySecond)
        {
            return -1;
        }
        if (priorityFirst > prioritySecond)
        {
            return 1;
        }
        // The third option is that the priorities are equal as well : return 0
        return 0;
    }

    // If all 3 comparision fields are missing in both items, they are equal
    return 0;
}

//  End of File
