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
* Description:  PIM item repeat rule converter class.
 *
*/


// INCLUDE FILES
#include    "pimrepeatruleconverter.h"
#include "logger.h"

// CONSTANTS
const TInt KPIMRepeatDatesArrayGranularity = 8;

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// PIMRepeatRuleConverter::Dates
// Calculates which dates occur on given date subset.
// Returns: the dates without exception dates.
// ----------------------------------------------------------------------------
//
CArrayFixFlat<TPIMDate>* PIMRepeatRuleConverter::DatesL(
    const MPIMRepeatRuleData& aRepeatRuleData, const TPIMDate& aStartDate,
    const TPIMDate& aSubsetBeginning, const TPIMDate& aSubsetEnding,
    CArrayFixFlat<TPIMDate>& aExceptionDates,
    const TPIMDate& aRepeatRuleEndDate, const TPIMField& aRepeatRuleCount)
{
    JELOG2(EPim);
    CArrayFixFlat<TPIMDate> * repeatDates =
        new(ELeave) CArrayFixFlat<TPIMDate> (KPIMRepeatDatesArrayGranularity);
    CleanupStack::PushL(repeatDates);

    CArrayFix<TPIMField>* fields = aRepeatRuleData.GetFieldsL();
    CleanupStack::PushL(fields);

    // Make sure that only date part of the TTime is included, not the time part
    TDateTime startDateTime = aStartDate.DateTime();
    TTime newStartDate(startDateTime);

    TDateTime subsetBeginningDateTime = aSubsetBeginning.DateTime();
    TTime newSubsetBeginning(subsetBeginningDateTime);

    TDateTime subsetEndingDateTime = aSubsetEnding.DateTime();
    TTime newSubsetEnding(subsetEndingDateTime);

    TTime date = newStartDate;
    TInt countIndex = 0;
    TInt newInterval;
    if (IsRepeatFieldPresentL(EPIMRepeatRuleInterval, *fields))
    {
        newInterval = aRepeatRuleData.GetIntL(EPIMRepeatRuleInterval);
    }
    else
    {
        newInterval = 1;
    }

    TInt frequency = EPIMRepeatRuleDaily; // default if not present

    if (IsRepeatFieldPresentL(EPIMRepeatRuleFrequency, *fields))
    {
        frequency = aRepeatRuleData.GetIntL(EPIMRepeatRuleFrequency);
    }

    switch (frequency)
    {
    case(EPIMRepeatRuleDaily):
    {
        // Calculate the first repeat after the subset beginning date
        if (newSubsetBeginning > date)
        {
            while (date < newSubsetBeginning)
            {
                date = date + TTimeIntervalDays(newInterval);
                countIndex++;
            }
        }

        // First fill the array with all repeat dates
        for (; date < newSubsetEnding + TTimeIntervalDays(1); date = date
                + TTimeIntervalDays(newInterval))
        {
            if (!AddRepeatDateL(*fields, date, newSubsetBeginning,
                                newSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                &repeatDates, &countIndex))
            {
                break;
            }
        }
        break;
    }

    case(EPIMRepeatRuleWeekly):
    {
        // Calculate the first repeat after the subset beginning date
        if (newSubsetBeginning > date)
        {
            while (date < newSubsetBeginning)
            {
                date = date + TTimeIntervalDays(7 * newInterval);
                countIndex++;
            }
        }
        // First fill the array with all repeat dates
        for (; date < newSubsetEnding + TTimeIntervalDays(1); date = date
                + TTimeIntervalDays(7 * newInterval))
        {
            if (IsRepeatFieldPresentL(EPIMRepeatRuleDayInWeek, *fields))
            {
                if (!CheckDaysInWeekL(*fields, date, newSubsetBeginning,
                                      newSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                      aRepeatRuleData.GetIntL(EPIMRepeatRuleDayInWeek),
                                      &repeatDates, &countIndex))
                {
                    break;
                }
            }
            else
            {
                if (!AddRepeatDateL(*fields, date, newSubsetBeginning,
                                    newSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                    &repeatDates, &countIndex))
                {
                    break;
                }
            }
        }
        break;
    }

    case(EPIMRepeatRuleMonthly):
    {
        // If DAY_IN_MONTH is present, shift the start date into it
        if (IsRepeatFieldPresentL(EPIMRepeatRuleDayInMonth, *fields))
        {
            TInt repeatRuleDayInMonth = aRepeatRuleData.GetIntL(
                                            EPIMRepeatRuleDayInMonth);

            if (repeatRuleDayInMonth >= (newStartDate.DayNoInMonth() + 1))
            {
                // The following check puts the DAY_IN_MONTH field to
                // correct place when handling different lenght months
                if (repeatRuleDayInMonth > (newStartDate.DaysInMonth() + 1))
                {
                    date = newStartDate + TTimeIntervalDays(
                               newStartDate.DaysInMonth()
                               - (newStartDate.DayNoInMonth() + 1));
                }
                else
                {
                    date = newStartDate + TTimeIntervalDays(
                               repeatRuleDayInMonth
                               - (newStartDate.DayNoInMonth() + 1));
                }
            }
            else
            {
                date = newStartDate - TTimeIntervalDays(
                           (newStartDate.DayNoInMonth() + 1) - repeatRuleDayInMonth);
            }
        }
        // Calculate the first repeat after the subset beginning date
        if (newSubsetBeginning > date)
        {
            while (date < newSubsetBeginning)
            {
                date = date + TTimeIntervalMonths(newInterval);
                countIndex++;
            }
        }
        // First fill the array with all repeat dates
        for (; date < newSubsetEnding + TTimeIntervalDays(1); date = date
                + TTimeIntervalMonths(newInterval))
        {
            if (IsRepeatFieldPresentL(EPIMRepeatRuleWeekInMonth, *fields)
                    && IsRepeatFieldPresentL(EPIMRepeatRuleDayInWeek, *fields))
            {
                if (!CheckWeeksInMonthL(*fields, date, newSubsetBeginning,
                                        newSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                        aRepeatRuleData.GetIntL(EPIMRepeatRuleWeekInMonth),
                                        aRepeatRuleData.GetIntL(EPIMRepeatRuleDayInWeek),
                                        &repeatDates, &countIndex))
                {
                    break;
                }
            }
            else
            {
                // The following check puts the DAY_IN_MONTH field to
                // correct place when handling different lenght months
                if (IsRepeatFieldPresentL(EPIMRepeatRuleDayInMonth, *fields))
                {
                    TInt repeatRuleDayInMonth = aRepeatRuleData.GetIntL(
                                                    EPIMRepeatRuleDayInMonth);

                    if (repeatRuleDayInMonth > (date.DayNoInMonth() + 1))
                    {
                        if (repeatRuleDayInMonth < (date.DaysInMonth() + 1))
                        {
                            date = date + TTimeIntervalDays(
                                       repeatRuleDayInMonth
                                       - (date.DayNoInMonth() + 1));
                        }
                        else
                        {
                            date = date + TTimeIntervalDays(date.DaysInMonth()
                                                            - (date.DayNoInMonth() + 1));
                        }
                    }
                }
                if (!AddRepeatDateL(*fields, date, newSubsetBeginning,
                                    newSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                    &repeatDates, &countIndex))
                {
                    break;
                }
            }
        }
        break;
    }

    case(EPIMRepeatRuleYearly):
    {
        // If DAY_IN_YEAR is present, shift the start date into it
        if (IsRepeatFieldPresentL(EPIMRepeatRuleDayInYear, *fields))
        {
            TInt repeatRuleDayInYear = aRepeatRuleData.GetIntL(
                                           EPIMRepeatRuleDayInYear);

            if (repeatRuleDayInYear >= newStartDate.DayNoInYear())
            {
                date = newStartDate + TTimeIntervalDays(repeatRuleDayInYear
                                                        - newStartDate.DayNoInYear());

                // If DAY_IN_YEAR was set to 366 and this is not a leap year
                // substract one day from the calculated value
                if (repeatRuleDayInYear == 366 && !Time::IsLeapYear(
                            newStartDate.DateTime().Year()))
                {
                    date = date - TTimeIntervalDays(1);
                }
            }
            else
            {
                date = newStartDate - TTimeIntervalDays(
                           newStartDate.DayNoInYear() - repeatRuleDayInYear);
            }
        }
        // Calculate the first repeat after the subset beginning date
        if (newSubsetBeginning > date)
        {
            while (date < newSubsetBeginning)
            {
                date = date + TTimeIntervalYears(newInterval);
                countIndex++;
            }
        }
        // First fill the array with all repeat dates
        for (; date < newSubsetEnding + TTimeIntervalDays(1); date = date
                + TTimeIntervalYears(newInterval))
        {
            TInt repeatRuleWeekInMonth = 0;
            TInt repeatRuleDayInWeek = 0;
            TInt repeatRuleMonthInYear = 0;
            TInt repeatRuleDayInMonth = 0;

            if (IsRepeatFieldPresentL(EPIMRepeatRuleMonthInYear, *fields))
            {
                repeatRuleMonthInYear = aRepeatRuleData.GetIntL(
                                            EPIMRepeatRuleMonthInYear);
            }

            if (IsRepeatFieldPresentL(EPIMRepeatRuleWeekInMonth, *fields))
            {
                repeatRuleWeekInMonth = aRepeatRuleData.GetIntL(
                                            EPIMRepeatRuleWeekInMonth);
            }

            if (IsRepeatFieldPresentL(EPIMRepeatRuleDayInWeek, *fields))
            {
                repeatRuleDayInWeek = aRepeatRuleData.GetIntL(
                                          EPIMRepeatRuleDayInWeek);
            }

            if (IsRepeatFieldPresentL(EPIMRepeatRuleDayInMonth, *fields))
            {
                repeatRuleDayInMonth = aRepeatRuleData.GetIntL(
                                           EPIMRepeatRuleDayInMonth);
            }

            if (IsRepeatFieldPresentL(EPIMRepeatRuleMonthInYear, *fields))
            {
                if (!CheckMonthsInYearL(*fields, date, newSubsetBeginning,
                                        newSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                        repeatRuleWeekInMonth, repeatRuleDayInWeek,
                                        repeatRuleMonthInYear, repeatRuleDayInMonth, &repeatDates,
                                        &countIndex))
                {
                    break;
                }
            }
            else
            {
                // The following check moves the DAY_IN_YEAR field from 365
                // to 366 if DAY_IN_YEAR is set 366 and this is a leap year
                if (IsRepeatFieldPresentL(EPIMRepeatRuleDayInYear, *fields))
                {
                    if (aRepeatRuleData.GetIntL(EPIMRepeatRuleDayInYear)
                            > date.DayNoInYear())
                    {
                        if (Time::IsLeapYear(date.DateTime().Year()))
                        {
                            date = date + TTimeIntervalDays(1);
                        }
                    }
                }
                if (!AddRepeatDateL(*fields, date, newSubsetBeginning,
                                    newSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                    &repeatDates, &countIndex))
                {
                    break;
                }
            }
        }
        break;
    }

    } // switch( iRepeatRuleFrequency )

    // Finally remove any possible exception dates from the repeat dates array
    TInt exceptionDateFinder;
    TInt fetchIndex = 0;
    TKeyArrayFix key(0, ECmpTInt64);
    for (TInt index = 0; index < aExceptionDates.Count(); index++)
    {
        const TPIMDate& exceptDate = aExceptionDates.At(index);
        exceptionDateFinder = repeatDates->Find(exceptDate, key, fetchIndex);
        if (exceptionDateFinder == 0) // 0 means that the element was found
        {
            repeatDates->Delete(fetchIndex);
            // Deleting elements from the array does not cause the
            // array buffer to be automatically compressed.
            // Compress() method returns excess space to the heap.
            repeatDates->Compress();
        }
    }

    CleanupStack::PopAndDestroy(fields);
    CleanupStack::Pop(repeatDates);
    return repeatDates;
}

// -----------------------------------------------------------------------------
// PIMRepeatRuleConverter::IsValidAgendaDate
// Tells whether given date is ok for Agenda
// (between 1st January 1980 and 31st December 2100).
// -----------------------------------------------------------------------------
//
TBool PIMRepeatRuleConverter::IsValidAgendaDate(const TPIMDate& aDate)
{
    JELOG2(EPim);
    return ((aDate >= TCalTime::MinTime()) && (aDate <= TCalTime::MaxTime()));
}

// -----------------------------------------------------------------------------
// PIMRepeatRuleConverter::ConvertRepeatToAgnL
// Converts a PIM repeat rule to Agenda Model repeat definition.
// Frequency is resolved first, as it affects rule formation.
// Then frequency-dependent parts are resolved by frequencty.
// Finally common parts are resolved.
// -----------------------------------------------------------------------------
//
TCalRRule PIMRepeatRuleConverter::ConvertSupportedRepeatToAgnL(
    const MPIMRepeatRuleData& aSrcPimRRule, TPIMDate aStartDate)
{
    JELOG2(EPim);
    TCalTime calStartDate;
    calStartDate.SetTimeUtcL(aStartDate);

    TCalRRule dstAgnRRule;

    if (IsRepeatFieldPresentL(EPIMRepeatRuleFrequency, aSrcPimRRule))
    {
        const TInt pimFrequency = aSrcPimRRule.GetIntL(EPIMRepeatRuleFrequency);

        if (pimFrequency == EPIMRepeatRuleDaily)
        {
            dstAgnRRule.SetType(TCalRRule::EDaily);
        }
        else if (pimFrequency == EPIMRepeatRuleWeekly)
        {
            dstAgnRRule.SetType(TCalRRule::EWeekly);
            RArray<TDay> weekDays; // put in current weekday
            CleanupClosePushL(weekDays);
            User::LeaveIfError(weekDays.Append(aStartDate.DayNoInWeek()));
            dstAgnRRule.SetByDay(weekDays);
            CleanupStack::PopAndDestroy(&weekDays);
        }
        else if (pimFrequency == EPIMRepeatRuleMonthly)
        {
            dstAgnRRule.SetType(TCalRRule::EMonthly);
            RArray<TInt> monthDays; // put in current month day number
            CleanupClosePushL(monthDays);
            User::LeaveIfError(monthDays.Append(aStartDate.DayNoInMonth()));
            dstAgnRRule.SetByMonthDay(monthDays);
            CleanupStack::PopAndDestroy(&monthDays);
        }
        else if (pimFrequency == EPIMRepeatRuleYearly)
        {
            dstAgnRRule.SetType(TCalRRule::EYearly);
        }
        else
        {
            User::Leave(KErrCorrupt); // invalid frequency
        }
    }
    else // frequency not present
    {
        User::Leave(KErrAbort);
    }

    AddCommonRulesToAgnL(aSrcPimRRule, aStartDate, dstAgnRRule);
    return dstAgnRRule;
}

// -----------------------------------------------------------------------------
// PIMRepeatRuleConverter::ConvertRepeatToPIM
// Converts an Agenda Model repeat definition to PIM repeat rule
// -----------------------------------------------------------------------------
//
void PIMRepeatRuleConverter::ConvertSupportedRepeatToPIML(
    MPIMRepeatRuleData& aDstPimRRule, TCalRRule& aSrcAgnRRule)
{
    JELOG2(EPim);
    aDstPimRRule.clear();

    // Frequency
    if (aSrcAgnRRule.Type() == TCalRRule::EDaily)
    {
        aDstPimRRule.SetIntL(EPIMRepeatRuleFrequency, EPIMRepeatRuleDaily);
    }
    else if (aSrcAgnRRule.Type() == TCalRRule::EWeekly)
    {
        aDstPimRRule.SetIntL(EPIMRepeatRuleFrequency, EPIMRepeatRuleWeekly);
    }
    else if (aSrcAgnRRule.Type() == TCalRRule::EMonthly)
    {
        aDstPimRRule.SetIntL(EPIMRepeatRuleFrequency, EPIMRepeatRuleMonthly);
    }
    else if (aSrcAgnRRule.Type() == TCalRRule::EYearly)
    {
        aDstPimRRule.SetIntL(EPIMRepeatRuleFrequency, EPIMRepeatRuleYearly);
    }
    else
    {
        User::Leave(KErrCorrupt); // repeatrule without frequency
    }

    // Interval
    aDstPimRRule.SetIntL(EPIMRepeatRuleInterval, aSrcAgnRRule.Interval());

    // End time
    TCalTime calEndDate = aSrcAgnRRule.Until();
    TTime pimEndDate(calEndDate.TimeUtcL());
    aDstPimRRule.SetDateL(EPIMRepeatRuleEnd, pimEndDate);
}

// -----------------------------------------------------------------------------
// PIMRepeatRuleConverter::IsRepeatFieldPresentL
// Checks if a field is present in the repeat rule.
// Use the other overload instead if you are checking multiple fields.
// -----------------------------------------------------------------------------
//
TBool PIMRepeatRuleConverter::IsRepeatFieldPresentL(TPIMField aRepeatRuleField,
        const MPIMRepeatRuleData& aRepeat)
{
    JELOG2(EPim);
    CArrayFix<TPIMField>* repeatFields = aRepeat.GetFieldsL();
    CleanupStack::PushL(repeatFields);
    TBool retVal = IsRepeatFieldPresentL(aRepeatRuleField, *repeatFields);
    CleanupStack::PopAndDestroy(repeatFields);
    return retVal;
}

// -----------------------------------------------------------------------------
// PIMRepeatRuleConverter::IsRepeatFieldPresentL
// Checks if a field is present in the array of repeat fields.
// Optimizes speed if called repeatedly instead of the overload
// which takes a MPIMRepeatRuleData.
// -----------------------------------------------------------------------------
//
TBool PIMRepeatRuleConverter::IsRepeatFieldPresentL(TPIMField aRepeatRuleField,
        const CArrayFix<TPIMField>& aRepeatFields)
{
    JELOG2(EPim);
    TInt fetchIndex = 0;
    TKeyArrayFix key(0, ECmpTInt);
    TInt found = aRepeatFields.Find(aRepeatRuleField, key, fetchIndex);
    return (found == KErrNone);
}

// -----------------------------------------------------------------------------
// PIMRepeatRuleConverter::AddRepeatDateL
// Checks if a single date is qualified as a repeat date, and adds it to array.
// -----------------------------------------------------------------------------
//
TBool PIMRepeatRuleConverter::AddRepeatDateL(
    const CArrayFix<TPIMField>& /*aFields*/, TTime aDate,
    TTime aSubsetBeginning, TTime aSubsetEnding,
    const TTime& aRepeatRuleEndDate, const TInt aRepeatRuleCount,
    CArrayFixFlat<TPIMDate>** aRepeatDates, TInt* aCountIndex)
{
    JELOG2(EPim);
    if (aDate < aSubsetBeginning)
    {
        return ETrue; // Here it is possible to still continue (no date added)
    }
    if (aDate > aSubsetEnding)
    {
        return EFalse; // Not possible to continue (subset end reached)
    }
    if ((aRepeatRuleEndDate != Time::NullTTime()) && (aDate
            > aRepeatRuleEndDate))
    {
        return EFalse; // Not possible to continue (repeat rule end reached)
    }

    (*aCountIndex)++;
    if (aRepeatRuleCount > 0 && (*aCountIndex) > aRepeatRuleCount)
    {
        return EFalse; // Not possible to continue (repeat rule count full)
    }

    (*aRepeatDates)->AppendL(aDate);
    return ETrue;
}

// -----------------------------------------------------------------------------
// PIMRepeatRuleConverter::AddCommonRulesToAgnL
// Adds common parts of supported repeat rules to Agenda repeat.
// -----------------------------------------------------------------------------
//
void PIMRepeatRuleConverter::AddCommonRulesToAgnL(
    const MPIMRepeatRuleData& aSrcPimRRule, const TTime& aSrcStartDate,
    TCalRRule& aDstAgnRRule)
{
    JELOG2(EPim);
    // Interval
    TInt interval = 1;
    if (IsRepeatFieldPresentL(EPIMRepeatRuleInterval, aSrcPimRRule))
    {
        interval = aSrcPimRRule.GetIntL(EPIMRepeatRuleInterval);
    }
    aDstAgnRRule.SetInterval(interval);

    // Start date
    TCalTime calStartDate;
    calStartDate.SetTimeUtcL(aSrcStartDate);
    aDstAgnRRule.SetDtStart(calStartDate);

    // End date
    if (IsRepeatFieldPresentL(EPIMRepeatRuleEnd, aSrcPimRRule))
    {
        TTime pimEndDate = aSrcPimRRule.GetDateL(EPIMRepeatRuleEnd);
        TCalTime calEndDate;
        calEndDate.SetTimeUtcL(pimEndDate);
        aDstAgnRRule.SetUntil(calEndDate);
    }
}

// -----------------------------------------------------------------------------
// PIMRepeatRuleConverter::CheckDaysInWeekL
// Checks if any week days are qualified as repeat dates, and adds them to array
// DAY_IN_WEEK requires that one or more weekdays are set,
// otherwise cannot process
// -----------------------------------------------------------------------------
//
TBool PIMRepeatRuleConverter::CheckDaysInWeekL(
    const CArrayFix<TPIMField>& aFields, TTime aDate, TTime aSubsetBeginning,
    TTime aSubsetEnding, TTime aRepeatRuleEndDate, TInt aRepeatRuleCount,
    const TInt aRepeatRuleDayInWeek, CArrayFixFlat<TPIMDate>** aRepeatDates,
    TInt* aCountIndex)
{
    JELOG2(EPim);
    if (!IsRepeatFieldPresentL(EPIMRepeatRuleDayInWeek, aFields))
    {
        return EFalse;
    }

    TTime checkDay;
    TLocale locale;

    // Set the day to the 1st day of the week.
    if (locale.StartOfWeek() == EMonday)
    {
        checkDay = aDate - TTimeIntervalDays(aDate.DayNoInWeek());
    }
    else // In case the start of week is set to sunday, substract one day more
    {
        if (aDate.DayNoInWeek() == ESunday)
        {
            checkDay = aDate; // The day was already sunday, do not substract
        }
        else
        {
            checkDay = aDate - TTimeIntervalDays(aDate.DayNoInWeek() + 1);
        }
    }

    // If the start of the week is set to sunday, start from here
    if (locale.StartOfWeek() == ESunday)
    {
        if (aRepeatRuleDayInWeek & EPIMRepeatRuleSunday)
        {
            if (!AddRepeatDateL(aFields, checkDay, aSubsetBeginning,
                                aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                aRepeatDates, aCountIndex))
            {
                return EFalse;
            }
        }
        checkDay = checkDay + TTimeIntervalDays(1);
    }

    // If the start of the week is set to monday, start from here
    if (aRepeatRuleDayInWeek & EPIMRepeatRuleMonday)
    {
        if (!AddRepeatDateL(aFields, checkDay, aSubsetBeginning, aSubsetEnding,
                            aRepeatRuleEndDate, aRepeatRuleCount, aRepeatDates, aCountIndex))
        {
            return EFalse;
        }
    }
    checkDay = checkDay + TTimeIntervalDays(1);
    if (aRepeatRuleDayInWeek & EPIMRepeatRuleTuesday)
    {
        if (!AddRepeatDateL(aFields, checkDay, aSubsetBeginning, aSubsetEnding,
                            aRepeatRuleEndDate, aRepeatRuleCount, aRepeatDates, aCountIndex))
        {
            return EFalse;
        }
    }
    checkDay = checkDay + TTimeIntervalDays(1);
    if (aRepeatRuleDayInWeek & EPIMRepeatRuleWednesday)
    {
        if (!AddRepeatDateL(aFields, checkDay, aSubsetBeginning, aSubsetEnding,
                            aRepeatRuleEndDate, aRepeatRuleCount, aRepeatDates, aCountIndex))
        {
            return EFalse;
        }
    }
    checkDay = checkDay + TTimeIntervalDays(1);
    if (aRepeatRuleDayInWeek & EPIMRepeatRuleThursday)
    {
        if (!AddRepeatDateL(aFields, checkDay, aSubsetBeginning, aSubsetEnding,
                            aRepeatRuleEndDate, aRepeatRuleCount, aRepeatDates, aCountIndex))
        {
            return EFalse;
        }
    }
    checkDay = checkDay + TTimeIntervalDays(1);
    if (aRepeatRuleDayInWeek & EPIMRepeatRuleFriday)
    {
        if (!AddRepeatDateL(aFields, checkDay, aSubsetBeginning, aSubsetEnding,
                            aRepeatRuleEndDate, aRepeatRuleCount, aRepeatDates, aCountIndex))
        {
            return EFalse;
        }
    }
    checkDay = checkDay + TTimeIntervalDays(1);
    if (aRepeatRuleDayInWeek & EPIMRepeatRuleSaturday)
    {
        if (!AddRepeatDateL(aFields, checkDay, aSubsetBeginning, aSubsetEnding,
                            aRepeatRuleEndDate, aRepeatRuleCount, aRepeatDates, aCountIndex))
        {
            return EFalse;
        }
    }

    // If the start of the week is set to monday, sunday is the last day of week
    if (locale.StartOfWeek() == EMonday)
    {
        checkDay = checkDay + TTimeIntervalDays(1);
        if (aRepeatRuleDayInWeek & EPIMRepeatRuleSunday)
        {
            if (!AddRepeatDateL(aFields, checkDay, aSubsetBeginning,
                                aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                aRepeatDates, aCountIndex))
            {
                return EFalse;
            }
        }
    }

    return ETrue;
}

// -----------------------------------------------------------------------------
// PIMRepeatRuleConverter::CheckDaysInWeekNoMonthChangeL
// Checks if any week days are qualified as repeat dates, and adds them to array
// If the month changes to a different month during the week (the week is either
// in the beginning of the month or at the end of the month), only those days
// that belong to this month are checked and the rest are discarded.
// DAY_IN_WEEK requires that one or more weekdays are set,
// otherwise cannot process
// -----------------------------------------------------------------------------
//
TBool PIMRepeatRuleConverter::CheckDaysInWeekNoMonthChangeL(const CArrayFix<
        TPIMField>& aFields, TTime aDate, TTime aSubsetBeginning,
        TTime aSubsetEnding, TTime aRepeatRuleEndDate, TInt aRepeatRuleCount,
        const TInt aRepeatRuleDayInWeek, CArrayFixFlat<TPIMDate>** aRepeatDates,
        TInt* aCountIndex)
{
    JELOG2(EPim);
    if (!IsRepeatFieldPresentL(EPIMRepeatRuleDayInWeek, aFields))
    {
        return EFalse;
    }

    TTime checkDay = aDate;

    // First it is needed to check are there accepted dates before the given
    // date value on the same week (and on the same month)
    while (checkDay.WeekNoInYear() == aDate.WeekNoInYear()
            && checkDay.DateTime().Month() == aDate.DateTime().Month())
    {
        checkDay = checkDay - TTimeIntervalDays(1);
    }

    // In the loop the day went to previous month or previous week, add one day
    checkDay = checkDay + TTimeIntervalDays(1);

    // Store the first accepted day for further comparison.
    TTime firstAcceptedDay = checkDay;

    // Calculate how many accepted dates in this week there are in total
    TInt acceptedDaysThisWeek = 0;
    while (checkDay.WeekNoInYear() == aDate.WeekNoInYear()
            && checkDay.DateTime().Month() == aDate.DateTime().Month())
    {
        acceptedDaysThisWeek++;
        checkDay = checkDay + TTimeIntervalDays(1);
    }

    // Finally go through all the accepted days this week
    for (checkDay = firstAcceptedDay; checkDay < firstAcceptedDay
            + TTimeIntervalDays(acceptedDaysThisWeek); checkDay = checkDay
                    + TTimeIntervalDays(1))
    {
        switch (checkDay.DayNoInWeek())
        {
        case EMonday:
        {
            if (aRepeatRuleDayInWeek & EPIMRepeatRuleMonday)
            {
                if (!AddRepeatDateL(aFields, checkDay, aSubsetBeginning,
                                    aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                    aRepeatDates, aCountIndex))
                {
                    return EFalse;
                }
            }
            break;
        }
        case ETuesday:
        {
            if (aRepeatRuleDayInWeek & EPIMRepeatRuleTuesday)
            {
                if (!AddRepeatDateL(aFields, checkDay, aSubsetBeginning,
                                    aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                    aRepeatDates, aCountIndex))
                {
                    return EFalse;
                }
            }
            break;
        }
        case EWednesday:
        {
            if (aRepeatRuleDayInWeek & EPIMRepeatRuleWednesday)
            {
                if (!AddRepeatDateL(aFields, checkDay, aSubsetBeginning,
                                    aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                    aRepeatDates, aCountIndex))
                {
                    return EFalse;
                }
            }
            break;
        }
        case EThursday:
        {
            if (aRepeatRuleDayInWeek & EPIMRepeatRuleThursday)
            {
                if (!AddRepeatDateL(aFields, checkDay, aSubsetBeginning,
                                    aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                    aRepeatDates, aCountIndex))
                {
                    return EFalse;
                }
            }
            break;
        }
        case EFriday:
        {
            if (aRepeatRuleDayInWeek & EPIMRepeatRuleFriday)
            {
                if (!AddRepeatDateL(aFields, checkDay, aSubsetBeginning,
                                    aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                    aRepeatDates, aCountIndex))
                {
                    return EFalse;
                }
            }
            break;
        }
        case ESaturday:
        {
            if (aRepeatRuleDayInWeek & EPIMRepeatRuleSaturday)
            {
                if (!AddRepeatDateL(aFields, checkDay, aSubsetBeginning,
                                    aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                    aRepeatDates, aCountIndex))
                {
                    return EFalse;
                }
            }
            break;
        }
        case ESunday:
        {
            if (aRepeatRuleDayInWeek & EPIMRepeatRuleSunday)
            {
                if (!AddRepeatDateL(aFields, checkDay, aSubsetBeginning,
                                    aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                    aRepeatDates, aCountIndex))
                {
                    return EFalse;
                }
            }
            break;
        }
        default:
        {
            return EFalse;
        }
        } //switch
    } // for

    return ETrue;
}

// -----------------------------------------------------------------------------
// PIMRepeatRuleConverter::CheckWeeksInMonthL
// Checks if any weeks in month are set to contain repeat dates
// WEEK_IN_MONTH requires that DAY_IN_WEEK is set, otherwise cannot process
// -----------------------------------------------------------------------------
//
TBool PIMRepeatRuleConverter::CheckWeeksInMonthL(
    const CArrayFix<TPIMField>& aFields, TTime aDate, TTime aSubsetBeginning,
    TTime aSubsetEnding, TTime aRepeatRuleEndDate, TInt aRepeatRuleCount,
    const TInt aRepeatRuleWeekInMonth, const TInt aRepeatRuleDayInWeek,
    CArrayFixFlat<TPIMDate>** aRepeatDates, TInt* aCountIndex)
{
    JELOG2(EPim);
    TTime checkDay;

    // Make sure the date is set to the 1st day of this month.
    if (aDate.DayNoInMonth() > 0)
    {
        checkDay = aDate - TTimeIntervalDays(aDate.DayNoInMonth());
    }

    TInt firstWeekNumber = checkDay.WeekNoInYear();
    TInt lastWeekNumber = (checkDay + TTimeIntervalDays(checkDay.DaysInMonth()
                           - 1)).WeekNoInYear();
    TInt weeksInMonth = lastWeekNumber - firstWeekNumber + 1;

    // If there are only 4 weeks in month: This can happen when all weeks are
    // full weeks (7 days in each) and there are only 28 days in this month.
    if (weeksInMonth == 4)
    {
        if (aRepeatRuleWeekInMonth & EPIMRepeatRuleFirst
                || aRepeatRuleWeekInMonth & EPIMRepeatRuleFourthLast)
        {
            if (!CheckDaysInWeekL(aFields, checkDay, aSubsetBeginning,
                                  aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                  aRepeatRuleDayInWeek, aRepeatDates, aCountIndex))
            {
                return EFalse;
            }
        }
        checkDay = checkDay + TTimeIntervalDays(7); // Move one week forward

        if (aRepeatRuleWeekInMonth & EPIMRepeatRuleSecond
                || aRepeatRuleWeekInMonth & EPIMRepeatRuleThirdLast)
        {
            if (!CheckDaysInWeekL(aFields, checkDay, aSubsetBeginning,
                                  aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                  aRepeatRuleDayInWeek, aRepeatDates, aCountIndex))
            {
                return EFalse;
            }
        }
        checkDay = checkDay + TTimeIntervalDays(7); // Move one week forward

        if (aRepeatRuleWeekInMonth & EPIMRepeatRuleThird
                || aRepeatRuleWeekInMonth & EPIMRepeatRuleSecondLast)
        {
            if (!CheckDaysInWeekL(aFields, checkDay, aSubsetBeginning,
                                  aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                  aRepeatRuleDayInWeek, aRepeatDates, aCountIndex))
            {
                return EFalse;
            }
        }
        checkDay = checkDay + TTimeIntervalDays(7); // Move one week forward

        if (aRepeatRuleWeekInMonth & EPIMRepeatRuleFourth
                || aRepeatRuleWeekInMonth & EPIMRepeatRuleLast)
        {
            if (!CheckDaysInWeekL(aFields, checkDay, aSubsetBeginning,
                                  aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                  aRepeatRuleDayInWeek, aRepeatDates, aCountIndex))
            {
                return EFalse;
            }
        }
    }

    // If there are 5 weeks in month: This is the most common situation.
    // The first week and the last week can be incomplete (and so they have
    // to be handled with the different method), but the three weeks in the
    // middle are guaranteed to be full weeks with 7 days each.
    if (weeksInMonth == 5)
    {
        if (aRepeatRuleWeekInMonth & EPIMRepeatRuleFirst
                || aRepeatRuleWeekInMonth & EPIMRepeatRuleFifthLast)
        {
            if (!CheckDaysInWeekNoMonthChangeL(aFields, checkDay,
                                               aSubsetBeginning, aSubsetEnding, aRepeatRuleEndDate,
                                               aRepeatRuleCount, aRepeatRuleDayInWeek, aRepeatDates,
                                               aCountIndex))
            {
                return EFalse;
            }
        }
        checkDay = checkDay + TTimeIntervalDays(7); // Move one week forward

        if (aRepeatRuleWeekInMonth & EPIMRepeatRuleSecond
                || aRepeatRuleWeekInMonth & EPIMRepeatRuleFourthLast)
        {
            if (!CheckDaysInWeekL(aFields, checkDay, aSubsetBeginning,
                                  aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                  aRepeatRuleDayInWeek, aRepeatDates, aCountIndex))
            {
                return EFalse;
            }
        }
        checkDay = checkDay + TTimeIntervalDays(7); // Move one week forward

        if (aRepeatRuleWeekInMonth & EPIMRepeatRuleThird
                || aRepeatRuleWeekInMonth & EPIMRepeatRuleThirdLast)
        {
            if (!CheckDaysInWeekL(aFields, checkDay, aSubsetBeginning,
                                  aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                  aRepeatRuleDayInWeek, aRepeatDates, aCountIndex))
            {
                return EFalse;
            }
        }
        checkDay = checkDay + TTimeIntervalDays(7); // Move one week forward

        if (aRepeatRuleWeekInMonth & EPIMRepeatRuleFourth
                || aRepeatRuleWeekInMonth & EPIMRepeatRuleSecondLast)
        {
            if (!CheckDaysInWeekL(aFields, checkDay, aSubsetBeginning,
                                  aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                  aRepeatRuleDayInWeek, aRepeatDates, aCountIndex))
            {
                return EFalse;
            }
        }

        if (aRepeatRuleWeekInMonth & EPIMRepeatRuleFifth
                || aRepeatRuleWeekInMonth & EPIMRepeatRuleLast)
        {
            // Here we need to go exactly to the 1st day of next week
            TTime fourthWeek = checkDay;
            while (checkDay.WeekNoInYear() == fourthWeek.WeekNoInYear())
            {
                checkDay = checkDay + TTimeIntervalDays(1);
            }
            if (!CheckDaysInWeekNoMonthChangeL(aFields, checkDay,
                                               aSubsetBeginning, aSubsetEnding, aRepeatRuleEndDate,
                                               aRepeatRuleCount, aRepeatRuleDayInWeek, aRepeatDates,
                                               aCountIndex))
            {
                return EFalse;
            }
        }
    }

    // If there are 6 weeks in month: This happens only a few times a year.
    // The first week and the last week can be incomplete (and so they have
    // to be handled with the different method), but the four weeks in the
    // middle are guaranteed to be full weeks with 7 days each.
    if (weeksInMonth == 6)
    {
        if (aRepeatRuleWeekInMonth & EPIMRepeatRuleFirst)
        {
            if (!CheckDaysInWeekNoMonthChangeL(aFields, checkDay,
                                               aSubsetBeginning, aSubsetEnding, aRepeatRuleEndDate,
                                               aRepeatRuleCount, aRepeatRuleDayInWeek, aRepeatDates,
                                               aCountIndex))
            {
                return EFalse;
            }
        }
        checkDay = checkDay + TTimeIntervalDays(7); // Move one week forward

        if (aRepeatRuleWeekInMonth & EPIMRepeatRuleSecond
                || aRepeatRuleWeekInMonth & EPIMRepeatRuleFifthLast)
        {
            if (!CheckDaysInWeekL(aFields, checkDay, aSubsetBeginning,
                                  aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                  aRepeatRuleDayInWeek, aRepeatDates, aCountIndex))
            {
                return EFalse;
            }
        }
        checkDay = checkDay + TTimeIntervalDays(7); // Move one week forward

        if (aRepeatRuleWeekInMonth & EPIMRepeatRuleThird
                || aRepeatRuleWeekInMonth & EPIMRepeatRuleFourthLast)
        {
            if (!CheckDaysInWeekL(aFields, checkDay, aSubsetBeginning,
                                  aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                  aRepeatRuleDayInWeek, aRepeatDates, aCountIndex))
            {
                return EFalse;
            }
        }
        checkDay = checkDay + TTimeIntervalDays(7); // Move one week forward

        if (aRepeatRuleWeekInMonth & EPIMRepeatRuleFourth
                || aRepeatRuleWeekInMonth & EPIMRepeatRuleThirdLast)
        {
            if (!CheckDaysInWeekL(aFields, checkDay, aSubsetBeginning,
                                  aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                  aRepeatRuleDayInWeek, aRepeatDates, aCountIndex))
            {
                return EFalse;
            }
        }
        checkDay = checkDay + TTimeIntervalDays(7); // Move one week forward

        if (aRepeatRuleWeekInMonth & EPIMRepeatRuleFifth
                || aRepeatRuleWeekInMonth & EPIMRepeatRuleSecondLast)
        {
            if (!CheckDaysInWeekL(aFields, checkDay, aSubsetBeginning,
                                  aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                  aRepeatRuleDayInWeek, aRepeatDates, aCountIndex))
            {
                return EFalse;
            }
        }

        if (aRepeatRuleWeekInMonth & EPIMRepeatRuleLast)
        {
            // Here we need to go exactly to the 1st day of next week
            TTime fifthWeek = checkDay;
            while (checkDay.WeekNoInYear() == fifthWeek.WeekNoInYear())
            {
                checkDay = checkDay + TTimeIntervalDays(1);
            }
            if (!CheckDaysInWeekNoMonthChangeL(aFields, checkDay,
                                               aSubsetBeginning, aSubsetEnding, aRepeatRuleEndDate,
                                               aRepeatRuleCount, aRepeatRuleDayInWeek, aRepeatDates,
                                               aCountIndex))
            {
                return EFalse;
            }
        }
    }

    return ETrue;
}

// -----------------------------------------------------------------------------
// PIMRepeatRuleConverter::CheckMonthsInYearL
// Checks if any months in year are set to contain repeat dates
// MONTH_IN_YEAR requires that either DAY_IN_MONTH is set, or
// both WEEK_IN_MONTH and DAY_IN_WEEK are set: otherwise cannot process
// -----------------------------------------------------------------------------
//
TBool PIMRepeatRuleConverter::CheckMonthsInYearL(
    const CArrayFix<TPIMField>& aFields, TTime aDate, TTime aSubsetBeginning,
    TTime aSubsetEnding, TTime aRepeatRuleEndDate, TInt aRepeatRuleCount,
    TInt aRepeatRuleWeekInMonth, TInt aRepeatRuleDayInWeek,
    TInt aRepeatRuleMonthInYear, TInt aRepeatRuleDayInMonth, CArrayFixFlat<
    TPIMDate>** aRepeatDates, TInt* aCountIndex)
{
    JELOG2(EPim);
    // Set the day to the 1st day of the 1st month.
    TTime checkDay = aDate - TTimeIntervalDays(aDate.DayNoInYear() - 1);

    if (aRepeatRuleMonthInYear & EPIMRepeatRuleJanuary)
    {
        if (!CheckMonthL(aFields, checkDay, aSubsetBeginning, aSubsetEnding,
                         aRepeatRuleEndDate, aRepeatRuleCount, aRepeatRuleWeekInMonth,
                         aRepeatRuleDayInWeek, aRepeatRuleDayInMonth, aRepeatDates,
                         aCountIndex))
        {
            return EFalse;
        }
    }
    checkDay = checkDay + TTimeIntervalMonths(1);

    if (aRepeatRuleMonthInYear & EPIMRepeatRuleFebruary)
    {
        if (!CheckMonthL(aFields, checkDay, aSubsetBeginning, aSubsetEnding,
                         aRepeatRuleEndDate, aRepeatRuleCount, aRepeatRuleWeekInMonth,
                         aRepeatRuleDayInWeek, aRepeatRuleDayInMonth, aRepeatDates,
                         aCountIndex))
        {
            return EFalse;
        }
    }
    checkDay = checkDay + TTimeIntervalMonths(1);

    if (aRepeatRuleMonthInYear & EPIMRepeatRuleMarch)
    {
        if (!CheckMonthL(aFields, checkDay, aSubsetBeginning, aSubsetEnding,
                         aRepeatRuleEndDate, aRepeatRuleCount, aRepeatRuleWeekInMonth,
                         aRepeatRuleDayInWeek, aRepeatRuleDayInMonth, aRepeatDates,
                         aCountIndex))
        {
            return EFalse;
        }
    }
    checkDay = checkDay + TTimeIntervalMonths(1);

    if (aRepeatRuleMonthInYear & EPIMRepeatRuleApril)
    {
        if (!CheckMonthL(aFields, checkDay, aSubsetBeginning, aSubsetEnding,
                         aRepeatRuleEndDate, aRepeatRuleCount, aRepeatRuleWeekInMonth,
                         aRepeatRuleDayInWeek, aRepeatRuleDayInMonth, aRepeatDates,
                         aCountIndex))
        {
            return EFalse;
        }
    }
    checkDay = checkDay + TTimeIntervalMonths(1);

    if (aRepeatRuleMonthInYear & EPIMRepeatRuleMay)
    {
        if (!CheckMonthL(aFields, checkDay, aSubsetBeginning, aSubsetEnding,
                         aRepeatRuleEndDate, aRepeatRuleCount, aRepeatRuleWeekInMonth,
                         aRepeatRuleDayInWeek, aRepeatRuleDayInMonth, aRepeatDates,
                         aCountIndex))
        {
            return EFalse;
        }
    }
    checkDay = checkDay + TTimeIntervalMonths(1);

    if (aRepeatRuleMonthInYear & EPIMRepeatRuleJune)
    {
        if (!CheckMonthL(aFields, checkDay, aSubsetBeginning, aSubsetEnding,
                         aRepeatRuleEndDate, aRepeatRuleCount, aRepeatRuleWeekInMonth,
                         aRepeatRuleDayInWeek, aRepeatRuleDayInMonth, aRepeatDates,
                         aCountIndex))
        {
            return EFalse;
        }
    }
    checkDay = checkDay + TTimeIntervalMonths(1);

    if (aRepeatRuleMonthInYear & EPIMRepeatRuleJuly)
    {
        if (!CheckMonthL(aFields, checkDay, aSubsetBeginning, aSubsetEnding,
                         aRepeatRuleEndDate, aRepeatRuleCount, aRepeatRuleWeekInMonth,
                         aRepeatRuleDayInWeek, aRepeatRuleDayInMonth, aRepeatDates,
                         aCountIndex))
        {
            return EFalse;
        }
    }
    checkDay = checkDay + TTimeIntervalMonths(1);

    if (aRepeatRuleMonthInYear & EPIMRepeatRuleAugust)
    {
        if (!CheckMonthL(aFields, checkDay, aSubsetBeginning, aSubsetEnding,
                         aRepeatRuleEndDate, aRepeatRuleCount, aRepeatRuleWeekInMonth,
                         aRepeatRuleDayInWeek, aRepeatRuleDayInMonth, aRepeatDates,
                         aCountIndex))
        {
            return EFalse;
        }
    }
    checkDay = checkDay + TTimeIntervalMonths(1);

    if (aRepeatRuleMonthInYear & EPIMRepeatRuleSeptember)
    {
        if (!CheckMonthL(aFields, checkDay, aSubsetBeginning, aSubsetEnding,
                         aRepeatRuleEndDate, aRepeatRuleCount, aRepeatRuleWeekInMonth,
                         aRepeatRuleDayInWeek, aRepeatRuleDayInMonth, aRepeatDates,
                         aCountIndex))
        {
            return EFalse;
        }
    }
    checkDay = checkDay + TTimeIntervalMonths(1);

    if (aRepeatRuleMonthInYear & EPIMRepeatRuleOctober)
    {
        if (!CheckMonthL(aFields, checkDay, aSubsetBeginning, aSubsetEnding,
                         aRepeatRuleEndDate, aRepeatRuleCount, aRepeatRuleWeekInMonth,
                         aRepeatRuleDayInWeek, aRepeatRuleDayInMonth, aRepeatDates,
                         aCountIndex))
        {
            return EFalse;
        }
    }
    checkDay = checkDay + TTimeIntervalMonths(1);

    if (aRepeatRuleMonthInYear & EPIMRepeatRuleNovember)
    {
        if (!CheckMonthL(aFields, checkDay, aSubsetBeginning, aSubsetEnding,
                         aRepeatRuleEndDate, aRepeatRuleCount, aRepeatRuleWeekInMonth,
                         aRepeatRuleDayInWeek, aRepeatRuleDayInMonth, aRepeatDates,
                         aCountIndex))
        {
            return EFalse;
        }
    }
    checkDay = checkDay + TTimeIntervalMonths(1);

    if (aRepeatRuleMonthInYear & EPIMRepeatRuleDecember)
    {
        if (!CheckMonthL(aFields, checkDay, aSubsetBeginning, aSubsetEnding,
                         aRepeatRuleEndDate, aRepeatRuleCount, aRepeatRuleWeekInMonth,
                         aRepeatRuleDayInWeek, aRepeatRuleDayInMonth, aRepeatDates,
                         aCountIndex))
        {
            return EFalse;
        }
    }

    return ETrue;
}

// -----------------------------------------------------------------------------
// PIMRepeatRuleConverter::CheckMonthL
// Checks if this month has set to contain repeat dates
// MONTH_IN_YEAR requires that either DAY_IN_MONTH is set, or
// both WEEK_IN_MONTH and DAY_IN_WEEK are set: otherwise cannot process
// -----------------------------------------------------------------------------
//
TBool PIMRepeatRuleConverter::CheckMonthL(const CArrayFix<TPIMField>& aFields,
        TTime aDate, TTime aSubsetBeginning, TTime aSubsetEnding,
        TTime aRepeatRuleEndDate, TInt aRepeatRuleCount,
        TInt aRepeatRuleWeekInMonth, TInt aRepeatRuleDayInWeek,
        TInt aRepeatRuleDayInMonth, CArrayFixFlat<TPIMDate>** aRepeatDates,
        TInt* aCountIndex)
{
    JELOG2(EPim);
    // Make sure the date is set to the 1st day of this month.
    if (aDate.DayNoInMonth() > 0)
    {
        aDate = aDate - TTimeIntervalDays(aDate.DayNoInMonth());
    }

    if (IsRepeatFieldPresentL(EPIMRepeatRuleWeekInMonth, aFields))
    {
        if (!CheckWeeksInMonthL(aFields, aDate, aSubsetBeginning,
                                aSubsetEnding, aRepeatRuleEndDate, aRepeatRuleCount,
                                aRepeatRuleWeekInMonth, aRepeatRuleDayInWeek, aRepeatDates,
                                aCountIndex))
        {
            return EFalse;
        }
    }
    else if (IsRepeatFieldPresentL(EPIMRepeatRuleDayInMonth, aFields))
    {
        // Shift the date to the correct place. If the day is bigger than
        // there are days in this month, use the last day of the month instead.
        if (aRepeatRuleDayInMonth < (aDate.DaysInMonth() + 1))
        {
            aDate = aDate + TTimeIntervalDays(aRepeatRuleDayInMonth - 1);
        }
        else
        {
            aDate = aDate + TTimeIntervalDays(aDate.DaysInMonth() - 1);
        }

        if (!AddRepeatDateL(aFields, aDate, aSubsetBeginning, aSubsetEnding,
                            aRepeatRuleEndDate, aRepeatRuleCount, aRepeatDates, aCountIndex))
        {
            return EFalse;
        }
    }
    else
    {
        // WEEK_IN_MONTH or DAY_IN_MONTH has to be present if MONTH_IN_YEAR used
        return EFalse;
    }

    return ETrue;
}

//  End of File
