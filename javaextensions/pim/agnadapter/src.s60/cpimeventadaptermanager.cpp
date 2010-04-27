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
* Description:  Base class for the Agenda Model event adapter managers
 *
*/


// CLASS HEADER
#include "cpimeventadaptermanager.h"
#include "logger.h"

// INTERNAL INCLUDES
#include "mpimitem.h"

// -----------------------------------------------------------------------------
// CPIMApptAdapterManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMEventAdapterManager::ConstructL(const TDesC& aListName)
{
    JELOG2(EPim);
    iListName = aListName.AllocL();
}

// Destructor
CPIMEventAdapterManager::~CPIMEventAdapterManager()
{
    JELOG2(EPim);
    delete iRepeatRuleFieldArray;
    delete iRepeatRuleFieldArrayEmpty;
    delete iRepeatRuleIntervalArray;
    delete iRepeatRuleIntervalArrayEmpty;
    delete iEventAttributeArray;
    delete iListName;
}

// -----------------------------------------------------------------------------
// CPIMEventAdapterManager::GetSupportedRepeatRuleFields
// Provides the supported repeat rule fields for a given frequency.
// Returns: Supported repeat rule fields for \a aFrequency. If no
// fields are supported for the frequency, an empty array is returned.
// -----------------------------------------------------------------------------
//
const CArrayFix<TPIMField>&
CPIMEventAdapterManager::GetSupportedRepeatRuleFieldsL(
    const TPIMRepeatRuleFrequency& aFrequency)
{
    JELOG2(EPim);
    const CArrayFix<TPIMField>* retVal = NULL;

    switch (aFrequency)
    {
    case EPIMRepeatRuleDaily:
    case EPIMRepeatRuleMonthly:
    case EPIMRepeatRuleYearly:
    {
        // Return a one item array with the item frequency
        if (!iRepeatRuleFieldArrayEmpty)
        {
            iRepeatRuleFieldArrayEmpty
            = new(ELeave) CArrayFixFlat<TPIMField> (1);
            iRepeatRuleFieldArrayEmpty->AppendL(EPIMRepeatRuleFrequency);
        }
        retVal = iRepeatRuleFieldArrayEmpty;
        break;
    }
    case EPIMRepeatRuleWeekly:
    {
        if (!iRepeatRuleFieldArray)
        {
            iRepeatRuleFieldArray
            = new(ELeave) CArrayFixFlat<TPIMField> (KPIMSupportedRepeatRuleFieldsCount);

            iRepeatRuleFieldArray->AppendL(KPIMSupportedRepeatRuleFields,
                                           KPIMSupportedRepeatRuleFieldsCount);
        }
        retVal = iRepeatRuleFieldArray;
        break;
    }
    default:
    {
        User::Leave(KErrArgument); // Field was not a valid field
    }
    }

    return *retVal;
}

// -----------------------------------------------------------------------------
// CPIMEventAdapterManager::GetSupportedIntervals
// Provides the supported interval values for given frequency.
// Returns: Supported interval values for given frequency. If the
// interval field is not supported for given frequency,
// an empty array is returned.
// -----------------------------------------------------------------------------
//
const CArrayFix<TInt>& CPIMEventAdapterManager::GetSupportedIntervalsL(
    const TPIMRepeatRuleFrequency& aFrequency)
{
    JELOG2(EPim);
    const CArrayFix<TInt>* retVal = NULL;

    switch (aFrequency)
    {
    case EPIMRepeatRuleDaily:
    case EPIMRepeatRuleMonthly:
    case EPIMRepeatRuleYearly:
    {
        // Return an empty array indicating no supported fields
        if (!iRepeatRuleIntervalArrayEmpty)
        {
            iRepeatRuleIntervalArrayEmpty
            = new(ELeave) CArrayFixFlat<TInt> (1);
        }
        retVal = iRepeatRuleIntervalArrayEmpty;
        break;
    }
    case EPIMRepeatRuleWeekly:
    {
        if (!iRepeatRuleIntervalArray)
        {
            iRepeatRuleIntervalArray
            = new(ELeave) CArrayFixFlat<TPIMField> (KPIMSupportedRepeatRuleIntervalsCount);

            iRepeatRuleIntervalArray->AppendL(KPIMSupportedRepeatRuleIntervals,
                                              KPIMSupportedRepeatRuleIntervalsCount);
        }
        retVal = iRepeatRuleIntervalArray;
        break;
    }
    default:
    {
        User::Leave(KErrArgument); // Field was not a valid field
    }
    }

    return *retVal;
}

const TDesC& CPIMEventAdapterManager::ListNameL()
{
    JELOG2(EPim);
    return *iListName;
}

// -----------------------------------------------------------------------------
// CPIMEventAdapterManager::MaxCategories
// Returns: The maximum number of categories that this list can have.
// -----------------------------------------------------------------------------
//

TInt CPIMEventAdapterManager::MaxCategories()
{
    JELOG2(EPim);
    return 0;
}

// -----------------------------------------------------------------------------
// CPIMEventAdapterManager::IsSupportedField
// Returns: ETrue if supported in this list, EFalse if not supported or invalid.
// -----------------------------------------------------------------------------
//

TBool CPIMEventAdapterManager::IsSupportedField(TPIMField aField) // The field to check
{
    JELOG2(EPim);
    TInt fieldFinder;
    TInt fetchIndex = 0;
    TKeyArrayFix key(0, ECmpTInt);
    fieldFinder = iSupportedFields->Find(aField, key, fetchIndex);
    if (fieldFinder != 0) // 0 means that the element was found
    {
        return EFalse;
    }
    return ETrue;
}

// -----------------------------------------------------------------------------
// CPIMEventAdapterManager::GetSupportedFieldsL
// Returns: An int array containing all fields supported by this list.
// -----------------------------------------------------------------------------
//

const CArrayFix<TPIMField>& CPIMEventAdapterManager::GetSupportedFieldsL()
{
    JELOG2(EPim);
    return *iSupportedFields;
}

// -----------------------------------------------------------------------------
// CPIMEventAdapterManager::IsSupportedAttribute
// Returns: EFalse, as there are no supported attributes in Event list.
// -----------------------------------------------------------------------------
//

TBool CPIMEventAdapterManager::IsSupportedAttribute(TPIMField /* aField */, // The field to check the supported attribute from
        TPIMAttribute /* aAttribute */) // The attribute to check
{
    JELOG2(EPim);
    return EFalse;
}

// -----------------------------------------------------------------------------
// CPIMEventAdapterManager::GetSupportedAttributesL
// Returns: An empty array meaning no supported attributes.
// -----------------------------------------------------------------------------
//

const CArrayFix<TPIMAttribute>&
CPIMEventAdapterManager::GetSupportedAttributesL(TPIMField aField) // The field to check the supported attributes from
{
    JELOG2(EPim);
    if (!IsSupportedField(aField))
    {
        User::Leave(KErrArgument); // Field was not a valid field
    }

    if (!iEventAttributeArray)
    {
        iEventAttributeArray = new(ELeave) CArrayFixFlat<TPIMAttribute> (1);
    }
    return *iEventAttributeArray;
}

// -----------------------------------------------------------------------------
// CPIMEventAdapterManager::GetSupportedAttributesCombinedL
// Returns: An integer holding the combination of all attributes
//          supported for aField (KPIMAttrNone meaning no supported attributes).
// -----------------------------------------------------------------------------

TPIMAttribute CPIMEventAdapterManager::GetSupportedAttributesCombinedL(
    TPIMField aField) // The field to check the supported attributes from
{
    JELOG2(EPim);
    if (!IsSupportedField(aField))
    {
        User::Leave(KErrArgument); // Field was not a valid field
    }

    return KPIMAttrNone;
}

TPIMAttribute CPIMEventAdapterManager::GetAllSupportedAttributesCombined()
{
    JELOG2(EPim);
    return KPIMAttrNone;
}

// -----------------------------------------------------------------------------
// CPIMEventAdapterManager::IsSupportedArrayElement
// Returns: EFalse, as there are no supported array elements in ToDo list.
// -----------------------------------------------------------------------------
//

TBool CPIMEventAdapterManager::IsSupportedArrayElement(
    TPIMField /* aStringArrayField */, // The field where array element belongs
    TPIMArrayElement /* aArrayElement */) // The array element to check
{
    JELOG2(EPim);
    return EFalse;
}

// -----------------------------------------------------------------------------
// CPIMEventAdapterManager::GetSupportedArrayElementsL
// Returns: An integer array containing all of the supported elements of a
//          string array for the given field.
// -----------------------------------------------------------------------------
//

const CArrayFix<TPIMArrayElement>&
CPIMEventAdapterManager::GetSupportedArrayElementsL(TPIMField /* aField */) // The field to check the supported array elements
{
    JELOG2(EPim);
    User::Leave(KErrArgument); // No array elements in Event list.
    // Even this method leaves always, the compiler requires it to return an
    // array.
    // The next line is only because of that, and actually it is never executed.
    const CArrayFix<TPIMArrayElement>* temp = NULL;
    return *temp;
}

// -----------------------------------------------------------------------------
// CPIMEventAdapterManager::MaxValues
// Returns: Number of values supported for the field
//          -1 : The field supports having an unlimited number of values in it.
//           0 : The field is not supported by this list.
// -----------------------------------------------------------------------------
//

TInt CPIMEventAdapterManager::MaxValues(TPIMField aField) // The field to check for multiple value support
{
    JELOG2(EPim);
    if (IsSupportedField(aField))
    {
        return 1; // Only one value is supported for any valid Event field
    }
    else
    {
        return 0;
    }
}

// -----------------------------------------------------------------------------
// CPIMEventAdapterManager::StringArraySizeL
// Returns: Int the size of the array.
// -----------------------------------------------------------------------------
//

TInt CPIMEventAdapterManager::StringArraySizeL(TPIMField /* aStringArrayField */) // The field to check the array size
{
    JELOG2(EPim);
    User::Leave(KErrArgument);
    // Even this method leaves always, the compiler requires it to return an
    // array.
    // The next line is only because of that, and actually it is never executed.
    return 0;
}

// -----------------------------------------------------------------------------
// CPIMEventAdapterManager::ItemOrder
// Returns: pointer to a function which implements an algorithm that
//            determines the order of two items.
// -----------------------------------------------------------------------------
//

TPIMItemComparisonFunc CPIMEventAdapterManager::ItemOrder()
{
    JELOG2(EPim);
    return ItemComparisonFunc;
}

// -----------------------------------------------------------------------------
// CPIMEventAdapterManager::ItemComparisonFunc
// Compares two MPIMItemData objects. The comparision is based primarily on the
// start times, and secondary on the summary strings (if start times are equal).
// Returns: < 0: aFirst < aSecond
//          0: aFirst == aSecond
//          > 0: aFirst > aSecond
// -----------------------------------------------------------------------------
//
TInt CPIMEventAdapterManager::ItemComparisonFunc(const MPIMItem& aFirst,
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
// CPIMEventAdapterManager::ItemComparisonFuncL
// Compares two MPIMItemData objects. The comparision is based primarily on the
// start times, and secondary on the summary strings (if start times are equal).
// Returns: < 0: aFirst < aSecond
//          0: aFirst == aSecond
//          > 0: aFirst > aSecond
// -----------------------------------------------------------------------------
//
TInt CPIMEventAdapterManager::ItemComparisonFuncL(const MPIMItem& aFirst,
        const MPIMItem& aSecond)
{
    JELOG2(EPim);
    TInt firstPresent = aFirst.CountValuesL(EPIMEventStart);
    TInt secondPresent = aSecond.CountValuesL(EPIMEventStart);

    if ((firstPresent == 0) ^(secondPresent == 0))
        // Only one of the fields is present
    {
        return secondPresent - firstPresent;
    }

    else if (firstPresent > 0 && secondPresent > 0)
        // Both fields are present
    {
        const TPIMDate dateFirst = aFirst.GetDateL(EPIMEventStart, 0);
        const TPIMDate dateSecond = aSecond.GetDateL(EPIMEventStart, 0);

        if (dateFirst < dateSecond)
        {
            return -1;
        }
        else if (dateFirst > dateSecond)
        {
            return 1;
        }
    }

    // In this point either none of the items have start time field, or
    // the other possibility is that the start time fields are exactly equal.
    // Continue the comparision based on the 2nd criteria, the summary field.
    firstPresent = aFirst.CountValuesL(EPIMEventSummary);
    secondPresent = aSecond.CountValuesL(EPIMEventSummary);

    if ((firstPresent == 0) ^(secondPresent == 0))  // xor
    {
        // Only one of the fields is present
        return secondPresent - firstPresent;
    }

    else if (firstPresent > 0 && secondPresent > 0)
        // Both fields are present
    {
        return aFirst.GetStringL(EPIMEventSummary, 0).CompareC(
                   aSecond.GetStringL(EPIMEventSummary, 0));
    }

    // If start time AND summary are missing in both items, they are equal.
    return 0;
}

//  End of File
