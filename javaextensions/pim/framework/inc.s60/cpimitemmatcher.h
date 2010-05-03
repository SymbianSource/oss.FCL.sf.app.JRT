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


#ifndef CPIMITEMMATCHER_H
#define CPIMITEMMATCHER_H

//  INCLUDES
#include <e32base.h>
#include "pimcommon.h"

// FORWARD DECLARATIONS
class CPIMItem;
class MPIMAdapterManager;
class CPIMValidator;

// CLASS DECLARATION
/**
 * Class for matching an item against multiple items for a match
 * defined in the PIM API PIMList.items( aMatchingItem ) method.
 * See MatchL() method.
 */
NONSHARABLE_CLASS(CPIMItemMatcher): public CBase
{
public: // Constructors and destructor

    //@{
    /**
     * Constructors.
     * \c NewLC() version leaves the created object in the
     * cleanup stack.
     *
     * @param aValidator A validator by which the field types are
     *        determined and the validity of
     *        the attributes in \a aMatchingItem is checked.
     * @param aAdapterManager An adapter manager by which support
     *        for the attributes in \a aMatchingItem are checked.
     * @param aMatchingItem The item against which the matching
     *        is done in subsequent matching operations.
     *        Some of the item data may be cached to
     *        optimize execution speed.
     *
     * @return New instance of CPIMItemMatcher.
     */
    static CPIMItemMatcher* NewLC(
        const CPIMValidator& aValidator,
        MPIMAdapterManager& aAdapterManager,
        const CPIMItem& aMatchingItem);
    //@}

    /**
     * Destructor.
     */
    virtual ~CPIMItemMatcher();

public: // New functions

    /**
     * Determines whether tested item matches the matching item
     * which was during construction.
     * @li Boolean, Date, Int and Binary fields are tested for
     *     exact match.
     * @li String fields are tested for collated substring match.
     * @li String Arry elements are tested for collated
     *     substring match. Null elements are ignored.
     *
     *
     * @param aTestedItem the item which is compared against
     *        \a aMatchingItem.
     *
     * @return \c ETrue if all values (and associated attributes)
     *         of all fields of the matching item are present
     *         in the \a aTestedItem; \c EFalse otherwise.
     *
     * @par Leaving:
     * Some of the leave codes indicate that the tested item did
     * not match the matching item. However, if the matching item
     * originates from this list (as required in the PIM API),
     * we should not come to such a situation.
     * @li \c KErrArgument - A matching field is invalid for
     *     given PIM type (no match).
     * @li \c KErrNotSupported - A matching field is not supported
     *     for given PIM list (no match).
     */
    TBool MatchL(const CPIMItem& aTestedItem);

private: // New functions

    /**
     * Match all values of given field in given item.
     *
     * @return \c ETrue if all values (and their attributes) of
     *         \a aMatchingField of the matching item are present
     *         in \a aTestedItem; \c EFalse otherwise.
     */
    TBool MatchAllMatchingValuesL(
        TPIMField aMatchingField,
        const CPIMItem& aTestedItem);

    /**
     * Match a value of matching item in given item.
     *
     * @return \c ETrue if some of the values in the
     *         \a aMatchingField of \a aTestedItem matches the
     *         value of \a aMatchingField indicated by
     *         \a aMatchingValueIndex in the
     *         matching item; \c EFalse otherwise.
     */
    TBool FindAnyTestedValueL(
        TPIMField aMatchingField,
        TInt aMatchingValueIndex,
        const CPIMItem& aTestedItem);

    /**
     * Matches a value of any type and its attributes to a given
     * value and attributes of the matching item.
     *
     * @return \c ETrue if the value and attributes of
     *         \a aMatchingField indicated by
     *         \a aTestedValueIndex in \a aTestedItem matches
     *         the value and attributes indicated by
     *         \a aMatchingValueIndex of
     *         \a aMatchingField in the matching item; \c EFalse
     *         otherwise.
     *
     * @par Notes:
     * @li The attributes associated with the value in the
     *     matching item must be a subset of the attributes
     *     associated with the value in the tested item.
     * @li The requirements for matching value may vary according
     *     to the type of the field.
     */
    TBool MatchValueAndAttributesL(
        TPIMField aMatchingField,
        TInt aMatchingValueIndex,
        const CPIMItem& aTestedItem,
        TInt aTestedValueIndex);

    /**
     * Matches the attributes associated with a value in the
     * tested item to the attributes with a value in the matching
     * item.
     *
     * @return \c ETrue if the attributes associated with the
     *         value in \a aMatchingValueIndex of
     *         \a aMatchingField of the matching item are a
     *         complete subset of the attributes associated with
     *         the value in \a aTestedValueindex of
     *         \a aMatchingField of \a aTestedItem; \c EFalse
     *         otherwise.
     */
    TBool MatchAttributesL(
        TPIMField aMatchingField,
        TInt aMatchingValueIndex,
        const CPIMItem& aTestedItem,
        TInt aTestedValueIndex);

    /**
     * Matches a boolean value (without attributes) in the given
     * item against the value in the matching item. The values
     * must match exactly.
     */
    TBool MatchBooleanL(
        TPIMField aMatchingField,
        TInt aMatchingValueIndex,
        const CPIMItem& aTestedItem,
        TInt aTestedValueIndex);

    /**
     * Matches a date value (without attributes) in the given
     * item against the value in the matching item. The values
     * must match exactly.
     */
    TBool MatchDateL(
        TPIMField aMatchingField,
        TInt aMatchingValueIndex,
        const CPIMItem& aTestedItem,
        TInt aTestedValueIndex);

    /**
     * Matches an int value (without attributes) in the given
     * item against the value in the matching item. The values
     * must match exactly.
     */
    TBool MatchIntL(
        TPIMField aMatchingField,
        TInt aMatchingValueIndex,
        const CPIMItem& aTestedItem,
        TInt aTestedValueIndex);

    /**
     * Matches a binary value (without attributes) in the given
     * item against the value in the matching item. The values
     * must match exactly.
     */
    TBool MatchBinaryL(
        TPIMField aMatchingField,
        TInt aMatchingValueIndex,
        const CPIMItem& aTestedItem,
        TInt aTestedValueIndex);

    /**
     * Matches a string value (without attributes) in the given
     * item against the value in the matching item. The value
     * in the given item must be a substring of the value in the
     * matching item, case-insensitive.
     */
    TBool MatchStringL(
        TPIMField aMatchingField,
        TInt aMatchingValueIndex,
        const CPIMItem& aTestedItem,
        TInt aTestedValueIndex);

    /**
     * Matches a string array value (without attributes) in the
     * given item against the value in the matching item. The
     * elements of the array in the given item must be substrings
     * of the elements in the array in the matching item,
     * case-insensitive. Empty elements in the matching item value
     * are ignored.
     */
    TBool MatchStringArrayL(
        TPIMField aMatchingField,
        TInt aMatchingValueIndex,
        const CPIMItem& aTestedItem,
        TInt aTestedValueIndex);

private: // Constructors

    /**
     * Constructor.
     */
    CPIMItemMatcher(
        const CPIMValidator& aValidator,
        MPIMAdapterManager& aAdapterManager,
        const CPIMItem& aMatchingItem);

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

private: // Data

    const CPIMValidator& iValidator;
    MPIMAdapterManager& iAdapterManager;
    const CPIMItem& iMatchingItem;

    /** Cached matching fields, owned. */
    CArrayFix< TPIMField>* iMatchingFields;

};

#endif // CPIMITEMMATCHER_H
// End of File
