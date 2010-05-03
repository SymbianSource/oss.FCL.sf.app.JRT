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
* Description:  PIM item repeat rule converter.
 *
*/


#ifndef PIMREPEATRULECONVERTER_H
#define PIMREPEATRULECONVERTER_H

//  INCLUDES
#include <e32base.h>
#include <calrrule.h>
#include "mpimrepeatruledata.h"

/**
 *  Event RepeatRuleConverter implementation. The class converts repeat rule
 *  between PIM framework and Agenda model.
 */
NONSHARABLE_CLASS(PIMRepeatRuleConverter)
{
public:

    static CArrayFixFlat< TPIMDate>* DatesL(
        const MPIMRepeatRuleData& aRepeatRuleData,
        const TPIMDate& aStartDate,
        const TPIMDate& aSubsetBeginning,
        const TPIMDate& aSubsetEnding,
        CArrayFixFlat< TPIMDate>& aExceptionDates,
        const TPIMDate& aRepeatRuleEndDate,
        const TPIMField& aRepeatRuleCount);

    static TBool IsValidAgendaDate(const TPIMDate& aDate);

    static TCalRRule ConvertSupportedRepeatToAgnL(
        const MPIMRepeatRuleData& aSrcPimRRule, TPIMDate aStartDate);

    static void ConvertSupportedRepeatToPIML(MPIMRepeatRuleData& aDstPimRRule,
            TCalRRule& aSrcAgnRRule);

    static TBool IsRepeatFieldPresentL(TPIMField aRepeatRuleField,
                                       const MPIMRepeatRuleData& aRepeat);

    static TBool IsRepeatFieldPresentL(TPIMField aRepeatRuleField,
                                       const CArrayFix<TPIMField>& aRepeatFields);

private:

    static TBool AddRepeatDateL(
        const CArrayFix< TPIMField>& aFields,
        TTime aDate,
        TTime aSubsetBeginning,
        TTime aSubsetEnding,
        const TTime& aRepeatRuleEndDate,
        const TInt aRepeatRuleCount,
        CArrayFixFlat< TPIMDate>** aRepeatDates,
        TInt* aCountIndex);

    static TBool CheckDaysInWeekL(const CArrayFix<TPIMField>& aFields,
                                  TTime aDate, TTime aSubsetBeginning, TTime aSubsetEnding,
                                  TTime aRepeatRuleEndDate, TInt aRepeatRuleCount,
                                  const TInt aRepeatRuleDayInWeek,
                                  CArrayFixFlat<TPIMDate>** aRepeatDates, TInt* aCount);

    static TBool CheckDaysInWeekNoMonthChangeL(
        const CArrayFix<TPIMField>& aFields, TTime aDate,
        TTime aSubsetBeginning, TTime aSubsetEnding,
        TTime aRepeatRuleEndDate, TInt aRepeatRuleCount,
        const TInt aRepeatRuleDayInWeek,
        CArrayFixFlat<TPIMDate>** aRepeatDates, TInt* aCount);

    static TBool CheckWeeksInMonthL(const CArrayFix<TPIMField>& aFields,
                                    TTime aDate, TTime aSubsetBeginning, TTime aSubsetEnding,
                                    TTime aRepeatRuleEndDate, TInt aRepeatRuleCount,
                                    const TInt aRepeatRuleWeekInMonth, const TInt aRepeatRuleDayInWeek,
                                    CArrayFixFlat<TPIMDate>** aRepeatDates, TInt* aCount);

    static TBool CheckMonthsInYearL(const CArrayFix<TPIMField>& aFields,
                                    TTime aDate, TTime aSubsetBeginning, TTime aSubsetEnding,
                                    TTime aRepeatRuleEndDate, TInt aRepeatRuleCount,
                                    TInt aRepeatRuleWeekInMonth, TInt aRepeatRuleDayInWeek,
                                    TInt aRepeatRuleMonthInYear, TInt aRepeatRuleDayInMonth,
                                    CArrayFixFlat<TPIMDate>** aRepeatDates, TInt* aCount);

    static TBool CheckMonthL(const CArrayFix<TPIMField>& aFields, TTime aDate,
                             TTime aSubsetBeginning, TTime aSubsetEnding,
                             TTime aRepeatRuleEndDate, TInt aRepeatRuleCount,
                             TInt aRepeatRuleWeekInMonth, TInt aRepeatRuleDayInWeek,
                             TInt aRepeatRuleDayInMonth, CArrayFixFlat<TPIMDate>** aRepeatDates,
                             TInt* aCount);

    static void AddCommonRulesToAgnL(const MPIMRepeatRuleData& aSrcPimRRule,
                                     const TTime& aSrcStartDate, TCalRRule& aDstAgnRRule);

private: // Hidden constructor

    PIMRepeatRuleConverter()
    {}

};

#endif // PIMREPEATRULECONVERTER_H
// End of File
