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


// INCLUDE FILES
#include  "cpimeventmatcher.h"
#include  "cpimeventitem.h"
#include  "cpimrepeatrule.h"
#include  "pimpanics.h"
#include "logger.h"

CPIMEventMatcher::CPIMEventMatcher(const TPIMEventSearchType aSearchType,
                                   const TPIMDate& aStartDate, const TPIMDate& aEndDate,
                                   TBool aInitialEventOnly) :
        iSearchType(aSearchType), iStartDate(aStartDate), iEndDate(aEndDate),
        iInitialEventOnly(aInitialEventOnly)
{
    JELOG2(EPim);
}

CPIMEventMatcher::~CPIMEventMatcher()
{
    JELOG2(EPim);
}

TBool CPIMEventMatcher::SaneOptions()
{
    JELOG2(EPim);
    if (iSearchType != EPIMEventSearchStarting && iSearchType
            != EPIMEventSearchEnding && iSearchType != EPIMEventSearchOccurring)
    {
        return EFalse;
    }

    if (iStartDate > iEndDate)
    {
        return EFalse;
    }

    return ETrue;
}

TBool CPIMEventMatcher::MatchL(const CPIMEventItem& aTestedEvent,
                               TPIMDate& aOccurrenceStart) const
{
    JELOG2(EPim);
    TBool eventMatches = EFalse;

    switch (iSearchType)
    {
    case EPIMEventSearchStarting:
    {
        eventMatches = MatchStartingL(aTestedEvent, aOccurrenceStart);
        break;
    }
    case EPIMEventSearchEnding:
    {
        eventMatches = MatchEndingL(aTestedEvent, aOccurrenceStart);
        break;
    }
    case EPIMEventSearchOccurring:
    {
        eventMatches = MatchOccurringL(aTestedEvent, aOccurrenceStart);
        break;
    }
    default:
    {
        // Error
        User::Panic(KPIMPanicCategory, EPIMPanicEventItemsSearchType);
        break;
    }
    }

    return eventMatches;
}

TBool CPIMEventMatcher::MatchStartingL(const CPIMEventItem& aTestedEvent,
                                       TPIMDate& aOccurrenceStart) const
{
    JELOG2(EPim);
    // If the Event, regardless of repeating, starts between the given
    // range (inclusive), it matches. If we didn't get direct match,
    // we'll check if any of the repeating occurrences of the item
    // starts between the given range (if repeating occurrences are
    // taken into account and the Event is repeating).

    TPIMDate eventStart(0); // For matching and occurrence start
    TPIMDate dummyEnd(0); // Not used
    TBool match = EFalse;

    if (ExtractStartAndEndDateL(aTestedEvent, eventStart, dummyEnd) == KErrNone)
    {
        if (eventStart >= iStartDate && eventStart <= iEndDate)
        {
            aOccurrenceStart = eventStart;
            match = ETrue;
        }
        else if (!iInitialEventOnly && aTestedEvent.IsRepeating())
        {
            match = MatchStartingRepeatingL(eventStart, aTestedEvent,
                                            aOccurrenceStart);
        }
    }

    return match;
}

TBool CPIMEventMatcher::MatchStartingRepeatingL(const TPIMDate& aEventStart,
        const CPIMEventItem& aTestedEvent, TPIMDate& aOccurrenceStart) const
{
    JELOG2(EPim);
    TBool match = EFalse;

    const CPIMRepeatRule* repeatRule =
        static_cast<const CPIMRepeatRule*>(aTestedEvent.GetRepeat());

    CArrayFixFlat<TPIMDate>* occurrences = repeatRule->DatesL(aEventStart,
                                           iStartDate, iEndDate);

    const TInt numOccs = occurrences->Count();
    for (TInt occIndex = 0; occIndex < numOccs; occIndex++)
    {
        // Occurrence start date
        TPIMDate occ = occurrences->At(occIndex);

        if ((occ >= iStartDate) && (occ <= iEndDate))
        {
            aOccurrenceStart = occ;
            match = ETrue;
            break;
        }
    }

    delete occurrences;
    return match;
}

TBool CPIMEventMatcher::MatchEndingL(const CPIMEventItem& aTestedEvent,
                                     TPIMDate& aOccurrenceStart) const
{
    JELOG2(EPim);
    // If the Event, regardless of repeating, ends between the given
    // range (inclusive), it matches.  If we didn't get direct match,
    // we'll check if any of the repeating occurrences of the item
    // ends between the given range (if repeating occurrences are
    // taken into account and the Event is repeating).

    TPIMDate eventStart(0); // For occurrence start
    TPIMDate eventEnd(0); // For matching
    TBool match = EFalse;

    if (ExtractStartAndEndDateL(aTestedEvent, eventStart, eventEnd) == KErrNone)
    {
        if (eventEnd >= iStartDate && eventEnd <= iEndDate)
        {
            aOccurrenceStart = eventStart;
            match = ETrue;
        }
        else if (!iInitialEventOnly && aTestedEvent.IsRepeating())
        {
            match = MatchEndingRepeatingL(eventEnd, aTestedEvent,
                                          aOccurrenceStart);
        }
    }

    return match;
}

TBool CPIMEventMatcher::MatchEndingRepeatingL(const TPIMDate& aEventEnd,
        const CPIMEventItem& aTestedEvent, TPIMDate& aOccurrenceStart) const
{
    JELOG2(EPim);
    TBool match = EFalse;

    // We need the start date for getting the repeating
    // occurrences of the event. Skip if not present.
    TPIMDate eventStart(0);
    if (aTestedEvent.CountValuesL(EPIMEventStart) != 0)
    {
        // And again we just use the first start value
        eventStart = aTestedEvent.GetDateL(EPIMEventStart, 0);

        const CPIMRepeatRule* repeatRule =
            static_cast<const CPIMRepeatRule*>(aTestedEvent.GetRepeat());

        TTimeIntervalMicroSeconds eventLen = aEventEnd.MicroSecondsFrom(
                                                 eventStart);

        // We expand the subset of retrieved occurrences a bit
        // from the start so that the first possible end date is
        // included.
        CArrayFixFlat<TPIMDate>* occurrences = repeatRule->DatesL(eventStart,
                                               iStartDate - eventLen, iEndDate);

        const TInt numOccs = occurrences->Count();
        for (TInt occIndex = 0; occIndex < numOccs; occIndex++)
        {
            // Occurrence start date
            TPIMDate occ = occurrences->At(occIndex);

            if (((occ + eventLen) >= iStartDate) && ((occ + eventLen)
                    <= iEndDate))
            {
                aOccurrenceStart = occ;
                match = ETrue;
                break;
            }
        }

        delete occurrences;
    }

    return match;
}

TBool CPIMEventMatcher::MatchOccurringL(const CPIMEventItem& aTestedEvent,
                                        TPIMDate& aOccurrenceStart) const
{
    JELOG2(EPim);
    // If the Event, regardless of repeating, either starts or ends
    // between the given range (inclusive) or the start date is before
    // range start and the end date is after range end, it matches.
    // If we didn't get direct match, we'll check the same for any
    // possible repeating occurrences of the item.

    TPIMDate eventStart(0); // For matching and occurrence start
    TPIMDate eventEnd(0); // For matching
    TBool match = EFalse;

    if (ExtractStartAndEndDateL(aTestedEvent, eventStart, eventEnd) == KErrNone)
    {
        if (eventStart <= iEndDate && eventEnd >= iStartDate)
        {
            aOccurrenceStart = eventStart;
            match = ETrue;
        }
        else if (!iInitialEventOnly && aTestedEvent.IsRepeating())
        {
            match = MatchOccurringRepeatingL(eventStart, eventEnd,
                                             aTestedEvent, aOccurrenceStart);
        }
    }

    return match;
}

TBool CPIMEventMatcher::MatchOccurringRepeatingL(const TPIMDate& aEventStart,
        const TPIMDate& aEventEnd, const CPIMEventItem& aTestedEvent,
        TPIMDate& aOccurrenceStart) const
{
    JELOG2(EPim);
    TBool match = EFalse;

    const CPIMRepeatRule* repeatRule =
        static_cast<const CPIMRepeatRule*>(aTestedEvent.GetRepeat());

    TTimeIntervalMicroSeconds eventLen =
        aEventEnd.MicroSecondsFrom(aEventStart);

    // We expand the subset of retrieved occurrences a bit from the start
    // so that the first possible end date is included and a matching
    // event may start before the given range.
    CArrayFixFlat<TPIMDate>* occurrences = repeatRule->DatesL(aEventStart,
                                           iStartDate - eventLen, iEndDate);

    const TInt numOccs = occurrences->Count();
    for (TInt occIndex = 0; occIndex < numOccs; occIndex++)
    {
        // Occurrence start date
        TPIMDate occ = occurrences->At(occIndex);

        if (occ <= iEndDate && occ + eventLen >= iStartDate)
        {
            aOccurrenceStart = occ;
            match = ETrue;
            break;
        }
    }

    delete occurrences;
    return match;
}

TInt CPIMEventMatcher::ExtractStartAndEndDateL(const CPIMEventItem& aEvent,
        TPIMDate& aEventStart, TPIMDate& aEventEnd) const
{
    JELOG2(EPim);
    TInt retVal = KErrNone;

    if (aEvent.CountValuesL(EPIMEventStart) != 0)
    {
        // Start date is present.
        aEventStart = aEvent.GetDateL(EPIMEventStart, 0);
        if (aEvent.CountValuesL(EPIMEventEnd) != 0)
        {
            // End date is also present.
            aEventEnd = aEvent.GetDateL(EPIMEventEnd, 0);
        }
        else
        {
            // End date is not present - use start date also as end date.
            aEventEnd = aEventStart;
        }
    }
    else if (aEvent.CountValuesL(EPIMEventEnd) != 0)
    {
        // Start date is not present, but end date is present.
        // Use end date also as start date.
        aEventEnd = aEvent.GetDateL(EPIMEventEnd, 0);
        aEventStart = aEventEnd;
    }
    else
    {
        // Neither start or end date is present.
        retVal = KErrNotFound;
    }

    return retVal;
}

//  End of File
