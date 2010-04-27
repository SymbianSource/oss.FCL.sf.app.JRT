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
* Description:  An item matcher.
 *
*/


// INCLUDE FILES
#include  "cpimitemmatcher.h"
#include  "mpimadaptermanager.h"
#include  "cpimvalidator.h"
#include  "cpimitem.h"
#include  "pimpanics.h"
#include "logger.h"

CPIMItemMatcher::CPIMItemMatcher(const CPIMValidator& aValidator,
                                 MPIMAdapterManager& aAdapterManager, const CPIMItem& aMatchingItem) :
        iValidator(aValidator), // not owned
        iAdapterManager(aAdapterManager), // not owned
        iMatchingItem(aMatchingItem) // not owned
{
    JELOG2(EPim);
}

void CPIMItemMatcher::ConstructL()
{
    JELOG2(EPim);
    iMatchingFields = iMatchingItem.GetFieldsL();
}

CPIMItemMatcher* CPIMItemMatcher::NewLC(const CPIMValidator& aValidator,
                                        MPIMAdapterManager& aAdapterManager, const CPIMItem& aMatchingItem)
{
    JELOG2(EPim);
    CPIMItemMatcher* self =
        new(ELeave) CPIMItemMatcher(aValidator, aAdapterManager, aMatchingItem);

    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CPIMItemMatcher::~CPIMItemMatcher()
{
    JELOG2(EPim);
    delete iMatchingFields;
}

TBool CPIMItemMatcher::MatchL(const CPIMItem& aTestedItem)
{
    JELOG2(EPim);
    const TInt n = iMatchingFields->Count();
    for (TInt i = 0; i < n; i++)
    {
        const TPIMField matchingField = (*iMatchingFields)[i];

        if (!MatchAllMatchingValuesL(matchingField, aTestedItem))
        {
            return EFalse;
        }
    }

    return ETrue;
}

TBool CPIMItemMatcher::MatchAllMatchingValuesL(TPIMField aMatchingField,
        const CPIMItem& aTestedItem)
{
    JELOG2(EPim);
    const TInt n = iMatchingItem.CountValuesL(aMatchingField);
    for (TInt i = 0; i < n; i++)
    {
        if (!FindAnyTestedValueL(aMatchingField, i, // _matching_ value index
                                 aTestedItem))
        {
            return EFalse;
        }
    }

    return ETrue;
}

TBool CPIMItemMatcher::FindAnyTestedValueL(TPIMField aMatchingField,
        TInt aMatchingValueIndex, const CPIMItem& aTestedItem)
{
    JELOG2(EPim);
    const TInt n = aTestedItem.CountValuesL(aMatchingField);
    for (TInt i = 0; i < n; i++)
    {
        if (MatchValueAndAttributesL(aMatchingField, aMatchingValueIndex,
                                     aTestedItem, i))
        {
            return ETrue;
        }
    }

    return EFalse;
}

TBool CPIMItemMatcher::MatchValueAndAttributesL(TPIMField aMatchingField,
        TInt aMatchingValueIndex, const CPIMItem& aTestedItem,
        TInt aTestedValueIndex)
{
    JELOG2(EPim);
    // Check attributes first
    if (!MatchAttributesL(aMatchingField, aMatchingValueIndex, aTestedItem,
                          aTestedValueIndex))
    {
        return EFalse;
    }

    TBool valueMatches = EFalse;

    TPIMFieldDataType matchingFieldDataType = iValidator.FieldDataType(
                aMatchingField);

    switch (matchingFieldDataType)
    {
    case EPIMFieldBoolean:
    {
        valueMatches = MatchBooleanL(aMatchingField, aMatchingValueIndex,
                                     aTestedItem, aTestedValueIndex);
        break;
    }

    case EPIMFieldDate:
    {
        valueMatches = MatchDateL(aMatchingField, aMatchingValueIndex,
                                  aTestedItem, aTestedValueIndex);
        break;
    }

    case EPIMFieldInt:
    {
        valueMatches = MatchIntL(aMatchingField, aMatchingValueIndex,
                                 aTestedItem, aTestedValueIndex);
        break;
    }

    case EPIMFieldBinary:
    {
        valueMatches = MatchBinaryL(aMatchingField, aMatchingValueIndex,
                                    aTestedItem, aTestedValueIndex);
        break;
    }

    case EPIMFieldString:
    {
        valueMatches = MatchStringL(aMatchingField, aMatchingValueIndex,
                                    aTestedItem, aTestedValueIndex);
        break;
    }

    case EPIMFieldStringArray:
    {
        valueMatches = MatchStringArrayL(aMatchingField, aMatchingValueIndex,
                                         aTestedItem, aTestedValueIndex);
        break;
    }

    default:
    {
        // We should never end up here
        __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory,
                                           EPIMPanicInvalidFieldType));
    }
    }

    return valueMatches;
}

TBool CPIMItemMatcher::MatchAttributesL(TPIMField aMatchingField,
                                        TInt aMatchingValueIndex, const CPIMItem& aTestedItem,
                                        TInt aTestedValueIndex)
{
    JELOG2(EPim);
    // Get the attributes for the matching value.
    TPIMAttribute matchingAttributes = iMatchingItem.getAttributes(
                                           aMatchingField, aMatchingValueIndex);

    // Get rid of unsupported attributes.
    matchingAttributes &= iAdapterManager. GetSupportedAttributesCombinedL(
                              aMatchingField);

    const TPIMAttribute testedAttributes = aTestedItem.getAttributes(
                                               aMatchingField, aTestedValueIndex);

    if ((testedAttributes & matchingAttributes) != matchingAttributes)
    {
        // Not all matching attributes were contained.
        return EFalse;
    }

    return ETrue;
}

TBool CPIMItemMatcher::MatchBooleanL(TPIMField aMatchingField,
                                     TInt aMatchingValueIndex, const CPIMItem& aTestedItem,
                                     TInt aTestedValueIndex)
{
    JELOG2(EPim);
    return iMatchingItem.GetBooleanL(aMatchingField, aMatchingValueIndex)
           == aTestedItem.GetBooleanL(aMatchingField, aTestedValueIndex);
}

TBool CPIMItemMatcher::MatchDateL(TPIMField aMatchingField,
                                  TInt aMatchingValueIndex, const CPIMItem& aTestedItem,
                                  TInt aTestedValueIndex)
{
    JELOG2(EPim);
    return iMatchingItem.GetDateL(aMatchingField, aMatchingValueIndex)
           == aTestedItem.GetDateL(aMatchingField, aTestedValueIndex);
}

TBool CPIMItemMatcher::MatchIntL(TPIMField aMatchingField,
                                 TInt aMatchingValueIndex, const CPIMItem& aTestedItem,
                                 TInt aTestedValueIndex)
{
    JELOG2(EPim);
    return iMatchingItem.getInt(aMatchingField, aMatchingValueIndex)
           == aTestedItem.getInt(aMatchingField, aTestedValueIndex);
}

TBool CPIMItemMatcher::MatchBinaryL(TPIMField aMatchingField,
                                    TInt aMatchingValueIndex, const CPIMItem& aTestedItem,
                                    TInt aTestedValueIndex)
{
    JELOG2(EPim);
    // Loop through the byte arrays
    const CPIMByteArray& matchingByteArray = iMatchingItem.GetBinaryRawL(
                aMatchingField, aMatchingValueIndex);

    const CPIMByteArray& testedByteArray = aTestedItem.GetBinaryRawL(
                                               aMatchingField, aTestedValueIndex);

    const TInt n = matchingByteArray.Count();
    if (testedByteArray.Count() != n)
    {
        return EFalse;
    }

    for (TInt i = 0; i < n; i++)
    {
        if (matchingByteArray[i] != testedByteArray[i])
        {
            return EFalse;
        }
    }

    return ETrue;
}

TBool CPIMItemMatcher::MatchStringL(TPIMField aMatchingField,
                                    TInt aMatchingValueIndex, const CPIMItem& aTestedItem,
                                    TInt aTestedValueIndex)
{
    JELOG2(EPim);
    // Match substring, case-insensitive

    const TDesC& matchingString = iMatchingItem.GetStringL(aMatchingField,
                                  aMatchingValueIndex);

    const TDesC& testedString = aTestedItem.GetStringL(aMatchingField,
                                aTestedValueIndex);

    // Find substring, collated (case-insensitive and
    // discarding punctuation)
    if (testedString.FindC(matchingString) == KErrNotFound)
    {
        return EFalse;
    }

    return ETrue;
}

TBool CPIMItemMatcher::MatchStringArrayL(TPIMField aMatchingField,
        TInt aMatchingValueIndex, const CPIMItem& aTestedItem,
        TInt aTestedValueIndex)
{
    JELOG2(EPim);
    // Match supported, present elements as
    // string values
    const CDesCArray& matchingArray = iMatchingItem.GetStringArrayL(
                                          aMatchingField, aMatchingValueIndex);

    const CDesCArray& testedArray = aTestedItem.GetStringArrayL(aMatchingField,
                                    aTestedValueIndex);

    const TInt n = matchingArray.Count();
    for (TInt i = 0; i < n; i++)
    {
        const TPtrC& matchingElem = matchingArray[i];

        if (matchingElem == KPIMNullArrayElement)
        {
            continue; // "null" element - next element
        }

        const TPtrC& testedElem = testedArray[i];

        if (testedElem.FindC(matchingElem) == KErrNotFound)
        {
            return EFalse;
        }
    }

    return ETrue;
}

//  End of File
