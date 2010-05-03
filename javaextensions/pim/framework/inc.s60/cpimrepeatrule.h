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
* Description:  PIM item repeat rule implementation.
 *
*/


#ifndef CPIMREPEATRULE_H
#define CPIMREPEATRULE_H

// INTERNAL INCLUDES
#include "mpimrepeatruledata.h"
#include "pimbaserepeatrule.h"

// EXTERNAL INCLUDES
#include <e32base.h>

/**
 *  Event RepeatRule implementation. The class is a container for repeat rule
 *  data.
 */
NONSHARABLE_CLASS(CPIMRepeatRule): public CBase,
        public pimbaserepeatrule,
        public MPIMRepeatRuleData
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMRepeatRule* NewL();

    /**
     * Destructor.
     */
    virtual ~CPIMRepeatRule();

public: // New functions

    /**
     * Provides dates on which this repeat rule will occur. Exception
     * dates will be noted.
     * Note that the given dates have to be in the agenda model's valid
     * date range (from 1st January 1980 to 31st December 2100).
     * The ownership of the array is transferred to the caller.
     *
     * @param aStartDate Anchor point for calculating occurrences.
     * @param aSubsetBeginning Beginning of a subset of occurrences.
     * @param aSubsetEnding Ending of a subset of occurrences.
     *
     * @return Array of dates.
     */
    CArrayFixFlat< TPIMDate>* DatesL(
        const TPIMDate& aStartDate,
        const TPIMDate& aSubsetBeginning,
        const TPIMDate& aSubsetEnding) const;

    jintArray dates(
        jlong aStartDate,
        jlong aSubsetBeginning,
        jlong aSubsetEnding,
        JNIEnv* aJniEnv);

public: // Functions from MPIMRepeatRuleData

    void AddExceptDateL(TPIMDate aDate);

    void addExceptDate(jlong aValue);

    void RemoveExceptDate(TPIMDate aDate);

    void removeExceptDate(jlong aValue);

    const CArrayFix< TPIMDate>& GetExceptDatesL() const;

    jintArray getExceptDates(JNIEnv* aJniEnv);

    TInt GetIntL(TPIMField aField) const;

    int getRepeatInt(TPIMField aField);

    void SetIntL(TPIMField aField, TInt aValue);

    void setRepeatInt(TPIMField aField, int aValue);

    TPIMDate GetDateL(TPIMField aField) const;

    jlong getRepeatDate(TPIMField aField);

    void SetDateL(TPIMField aField, TPIMDate aDate);

    void setDate(TPIMField aField, jlong aValue);

    CArrayFix< TPIMField>* GetFieldsL() const;

    jintArray getFields(JNIEnv* aJniEnv);

    void ClearFieldL(TPIMField aField);

    void clear();

    void BackupRepeatL();

    TBool IsModified() const;

private:

    /**
     * C++ default constructor.
     */
    CPIMRepeatRule();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: // Data

    CArrayFixFlat< TPIMDate>* iExceptDates; // Array of exception dates,
    // Owned by RepeatRule
    // The repeat rule fields supported by Nokia
    TPIMField iRepeatRuleFrequency;
    TPIMField iRepeatRuleInterval;
    TPIMDate iRepeatRuleEndDate;

    // The rest of the repeat rule fields
    TPIMField iRepeatRuleDayInMonth;
    TPIMField iRepeatRuleDayInWeek;
    TPIMField iRepeatRuleDayInYear;
    TPIMField iRepeatRuleMonthInYear;
    TPIMField iRepeatRuleWeekInMonth;
    TPIMField iRepeatRuleCount;

    // The following fields are used for storing the original Agenda values
    CArrayFixFlat< TPIMDate>* iOriginalExceptDates; // Owned by RepeatRule
    TPIMField iOriginalRepeatRuleFrequency;
    TPIMField iOriginalRepeatRuleInterval;
    TPIMDate iOriginalRepeatRuleEndDate;
    TPIMField iOriginalRepeatRuleDayInMonth;
    TPIMField iOriginalRepeatRuleDayInWeek;
    TPIMField iOriginalRepeatRuleDayInYear;
    TPIMField iOriginalRepeatRuleMonthInYear;
    TPIMField iOriginalRepeatRuleWeekInMonth;
    TPIMField iOriginalRepeatRuleCount;
};

#endif // CPIMREPEATRULE_H
// End of File
