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
* Description:  RepeatRule types and constants
 *
*/


#ifndef PIMREPEATRULE_H
#define PIMREPEATRULE_H

/** @file
 * This file contains the PIM API constants and definitions for RepeatRules.
 */

/**
 *
 */
enum TPIMRepeatRuleField
{
    EPIMRepeatRuleDayInMonth = 1,
    /**< \e Integer [1,31]. */

    EPIMRepeatRuleDayInWeek = 2,
    /**< \e Integer from \ref TPIMRepeatRuleDayInWeek. */

    EPIMRepeatRuleDayInYear = 4,
    /**< \e Integer [1,366]. */

    EPIMRepeatRuleMonthInYear = 8,
    /**< \e Integer from \ref TPIMRepeatRuleMonthInYear. */

    EPIMRepeatRuleWeekInMonth = 16,
    /**< \e Integer from \ref TPIMRepeatRuleWeekInMonth. */

    EPIMRepeatRuleCount = 32,
    /**< \e Integer. */

    EPIMRepeatRuleEnd = 64,
    /**< \e Date. */

    EPIMRepeatRuleFrequency = 0,
    /**< \e Integer from \ref TPIMRepeatRuleFrequency. */

    EPIMRepeatRuleInterval = 128
    /**< \e Integer. */
};

/**
 * Day in week constants. The possible values of \ref EPIMRepeatRuleDayInWeek field.
 */
enum TPIMRepeatRuleDayInWeek
{
    EPIMRepeatRuleSunday = 65536,
    EPIMRepeatRuleMonday = 32768,
    EPIMRepeatRuleTuesday = 16384,
    EPIMRepeatRuleWednesday = 8192,
    EPIMRepeatRuleThursday = 4096,
    EPIMRepeatRuleFriday = 2048,
    EPIMRepeatRuleSaturday = 1024
};

/**
 * Month in year constants. The possible values of \ref EPIMRepeatRuleMonthInYear field.
 */
enum TPIMRepeatRuleMonthInYear
{
    EPIMRepeatRuleJanuary = 131072,
    EPIMRepeatRuleFebruary = 262144,
    EPIMRepeatRuleMarch = 524288,
    EPIMRepeatRuleApril = 1048576,
    EPIMRepeatRuleMay = 2097152,
    EPIMRepeatRuleJune = 4194304,
    EPIMRepeatRuleJuly = 8388608,
    EPIMRepeatRuleAugust = 16777216,
    EPIMRepeatRuleSeptember = 33554432,
    EPIMRepeatRuleOctober = 67108864,
    EPIMRepeatRuleNovember = 134217728,
    EPIMRepeatRuleDecember = 268435456
};

/**
 * Week in month constants. The possbile values of \ref EPIMRepeatRuleWeekInMonth.
 */
enum TPIMRepeatRuleWeekInMonth
{
    EPIMRepeatRuleFirst = 1,
    EPIMRepeatRuleSecond = 2,
    EPIMRepeatRuleThird = 4,
    EPIMRepeatRuleFourth = 8,
    EPIMRepeatRuleFifth = 16,
    EPIMRepeatRuleLast = 32,
    EPIMRepeatRuleSecondLast = 64,
    EPIMRepeatRuleThirdLast = 128,
    EPIMRepeatRuleFourthLast = 256,
    EPIMRepeatRuleFifthLast = 512
};

/**
 * Frequency constants. The possible values of \ref EPIMRepeatRuleFrequency.
 */
enum TPIMRepeatRuleFrequency
{
    EPIMRepeatRuleDaily = 16,
    EPIMRepeatRuleWeekly = 17,
    EPIMRepeatRuleMonthly = 18,
    EPIMRepeatRuleYearly = 19
};

#endif // PIMREPEATRULE_H
// End of File
