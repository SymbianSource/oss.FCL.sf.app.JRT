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
* Description:  An event occurrence matcher.
 *
*/


#ifndef CPIMEVENTMATCHER_H
#define CPIMEVENTMATCHER_H

//  INCLUDES
#include <e32base.h>
#include "pimcommon.h"
#include "pimevent.h"

// FORWARD DECLARATIONS
class CPIMEventItem;
class MPIMAdapterManager;
class CPIMValidator;

// CLASS DECLARATION
/**
 * Event occurrence matcher.
 * Used for finding events that take place on given range, possibly
 * checking repeating instances as well.
 */
NONSHARABLE_CLASS(CPIMEventMatcher): public CBase
{
public: // Constructors and destructor

    /**
     * C++ constructor.
     *
     * @param aSearchType Search type for matching.
     * @param aStartDate Start date for matching.
     * @param aEndDate End date for matching.
     * @param aInitialEventOnly Determines whether repeating
     *        occurrences are taken into account.
     *
     * @par Notes:
     * @li The validity of given options can be checked with
     *     SaneOptions() method. The matching can be carried out
     *     with invalid options as well, but results will be
     *     crap.
     */
    CPIMEventMatcher(
        const TPIMEventSearchType aSearchType,
        const TPIMDate& aStartDate,
        const TPIMDate& aEndDate,
        TBool aInitialEventOnly);

    /**
     * Destructor.
     */
    virtual ~CPIMEventMatcher();

public: // New functions

    /**
     * Checks whether the options given for matching are sane.
     * @li Search type is valid.
     * @li Start date is not greater than end date.
     *
     * This is not done during construction (e.g. within a NewL
     * function) because such failures are typically interpreted
     * as lack of resources etc.
     *
     * @return \c ETrue if the searching if valid options were
     *         given in construction; \c EFalse otherwise.
     */
    TBool SaneOptions();

    /**
     * Matches given item against the ranges and options set during
     * construction. According to them, the matching may apply to
     * either starting, ending or occurring events and either to
     * the single event instance or repeating occurrences of the
     * event.
     *
     * See constructor for search options.
     *
     * @param aTestedItem The item to test for matching.
     * @param aOccurrenceStart The start date stamp will be
     *        put here if the item matches. If repeating
     *        occurrences are taken into account, the start date
     *        of the repeating occurrence.
     *
     * @return \c ETrue if \a aTestedItem takes place in the set
     *         range according to the set options.
     */
    TBool MatchL(
        const CPIMEventItem& aTestedItem,
        TPIMDate& aOccurrenceStart) const;

private: // New functions

    /**
     * Matches events starting between set range. Examines
     * repeating occurrences if indicated.
     */
    TBool MatchStartingL(
        const CPIMEventItem& aTestedEvent,
        TPIMDate& aOccurrenceStart) const;

    /**
     * Helper function to MatchStartingL().
     */
    TBool MatchStartingRepeatingL(
        const TPIMDate& aEventStart,
        const CPIMEventItem& aTestedEvent,
        TPIMDate& aOccurrenceStart) const;

    /**
     * Matches events ending between set range. Examines
     * repeating occurrences if indicated.
     */
    TBool MatchEndingL(
        const CPIMEventItem& aTestedEvent,
        TPIMDate& aOccurrenceStart) const;

    /**
     * Helper function to MatchEndingL().
     */
    TBool MatchEndingRepeatingL(
        const TPIMDate& aEventEnd,
        const CPIMEventItem& aTestedEvent,
        TPIMDate& aOccurrenceStart) const;

    /**
     * Matches events occurring (also starting and ending)
     * between set range. Examines repeating occurrences if
     * indicated.
     */
    TBool MatchOccurringL(
        const CPIMEventItem& aTestedEvent,
        TPIMDate& aOccurrenceStart) const;

    /**
     * Helper function to MatchOccurringL.
     */
    TBool MatchOccurringRepeatingL(
        const TPIMDate& aEventStart,
        const TPIMDate& aEventEnd,
        const CPIMEventItem& aTestedEvent,
        TPIMDate& aOccurrenceStart) const;

    /**
     * Helper function to MatchEndingL and MatchOccurringL.
     * Deduces the start and end date of given Event. If either
     * start or end date is missing, the other value replaces
     * the missing one. If neither of them are present, the
     * operation fails.
     *
     * @param aEvent The Event from which to extract the dates.
     *
     * @param aEventStart Event start is put here. If Event start
     *        is not present, Event end is put instead.
     *
     * @param aEventEnd Event end is put here. If Event end is not
     *        present, Event start is put instead.
     *
     * @return KErrNotFound if neither start nor end date is present;
     *         KErrNone otherwise.
     */
    TInt ExtractStartAndEndDateL(
        const CPIMEventItem& aEvent,
        TPIMDate& aEventStart,
        TPIMDate& aEventEnd) const;

private: // Data

    const TPIMEventSearchType iSearchType;
    const TPIMDate iStartDate;
    const TPIMDate iEndDate;
    const TBool iInitialEventOnly;

};

#endif // CPIMEVENTMATCHER_H
// End of File
