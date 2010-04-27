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


// INCLUDE FILES
#include  "cpimrepeatrule.h"
#include  "pimrepeatruleconverter.h"
#include  "pimjnitools.h"
#include  "pimutils.h"
#include  "s60commonutils.h"
#include "logger.h"

// CONSTANTS
namespace
{
const TInt KPIMRepeatRuleMaskWeekInMonth = 1023; // All week bits set
const TInt KPIMRepeatRuleMaskDayInWeek = 131071; // All day bits set
const TInt KPIMRepeatRuleMaskMonthInYear = 536870911; // All month bits set
}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMRepeatRule::CPIMRepeatRule
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMRepeatRule::CPIMRepeatRule() :
        iRepeatRuleFrequency(0), iRepeatRuleInterval(0), iRepeatRuleEndDate(
            Time::NullTTime()), iRepeatRuleDayInMonth(0), iRepeatRuleDayInWeek(0),
        iRepeatRuleDayInYear(0), iRepeatRuleMonthInYear(0),
        iRepeatRuleWeekInMonth(0), iRepeatRuleCount(0)
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMRepeatRule::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMRepeatRule::ConstructL()
{
    JELOG2(EPim);
    iExceptDates = new(ELeave) CArrayFixFlat<TPIMDate> (1);
    // Create here also the array to store the original exception dates
    iOriginalExceptDates = new(ELeave) CArrayFixFlat<TPIMDate> (1);
}

// -----------------------------------------------------------------------------
// CPIMRepeatRule::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMRepeatRule* CPIMRepeatRule::NewL()
{
    JELOG2(EPim);
    CPIMRepeatRule* self = new(ELeave) CPIMRepeatRule;

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

pimbaserepeatrule* pimbaserepeatrule::getInstance()
{
    JELOG2(EPim);
    CPIMRepeatRule* repeatRule = NULL;
    TInt error = KErrNone;
    TRAP(error, repeatRule = CPIMRepeatRule::NewL());
    if (error != KErrNone)
        throw KErrGeneral;
    return repeatRule;
}

// Destructor
CPIMRepeatRule::~CPIMRepeatRule()
{
    JELOG2(EPim);
    delete iExceptDates;
    delete iOriginalExceptDates;
}

// -----------------------------------------------------------------------------
// CPIMRepeatRule::DatesL
// Returns the repeat dates.
// Returns: An array of repeat dates without the exception dates.
//          The ownership of the array is transferred to the caller.
// -----------------------------------------------------------------------------
//
CArrayFixFlat<TPIMDate>* CPIMRepeatRule::DatesL(const TPIMDate& aStartDate,
        const TPIMDate& aSubsetBeginning, const TPIMDate& aSubsetEnding) const
{
    JELOG2(EPim);
    return PIMRepeatRuleConverter::DatesL(*this, aStartDate, aSubsetBeginning,
                                          aSubsetEnding, *iExceptDates, iRepeatRuleEndDate, iRepeatRuleCount);
}

jintArray CPIMRepeatRule::dates(jlong aStartDate, jlong aSubsetBeginning,
                                jlong aSubsetEnding, JNIEnv* aJniEnv)
{
    JELOG2(EPim);
    CArrayFix<TPIMDate>* repeatDates = NULL;

    TPIMDate nativeStartDate(java::util::S60CommonUtils::JavaTimeToTTime(
                                 aStartDate));
    TPIMDate nativeSubsetBeginning(java::util::S60CommonUtils::JavaTimeToTTime(
                                       aSubsetBeginning));
    TPIMDate nativeSubsetEnding(java::util::S60CommonUtils::JavaTimeToTTime(
                                    aSubsetEnding));
    TInt error = KErrNone;
    TRAP(error, repeatDates = DatesL(nativeStartDate, nativeSubsetBeginning,
                                     nativeSubsetEnding));

    if (error != KErrNone || repeatDates == NULL)
    {
        // major error
        return NULL;
    }

    jintArray repeatDatesIntArray = NativeDatesToJavaInts(*aJniEnv,
                                    *repeatDates);

    delete repeatDates;
    return repeatDatesIntArray;

}

// -----------------------------------------------------------------------------
// CPIMRepeatRule::AddExceptDateL
// Adds an exception date into the repeat rule.
// -----------------------------------------------------------------------------
//
void CPIMRepeatRule::AddExceptDateL(TPIMDate aDate)
{
    JELOG2(EPim);
    TInt exceptionDateFinder;
    TInt fetchIndex = 0;
    TKeyArrayFix key(0, ECmpTInt64);
    exceptionDateFinder = iExceptDates->Find(aDate, key, fetchIndex);
    if (exceptionDateFinder != 0) // 0 would mean that the element was found
    {
        iExceptDates->AppendL(aDate);
    }
}

void CPIMRepeatRule::addExceptDate(jlong aValue)
{
    JELOG2(EPim);
    TPIMDate nativeValue(java::util::S60CommonUtils::JavaTimeToTTime(aValue));
    TInt error = KErrNone;
    TRAP(error, AddExceptDateL(nativeValue));
}

// -----------------------------------------------------------------------------
// CPIMRepeatRule::RemoveExceptDate
// Removes an exception date given as a parameter.
// -----------------------------------------------------------------------------
//
void CPIMRepeatRule::RemoveExceptDate(TPIMDate aDate)
{
    JELOG2(EPim);
    TInt exceptionDateFinder;
    TInt fetchIndex = 0;
    TKeyArrayFix key(0, ECmpTInt64);
    exceptionDateFinder = iExceptDates->Find(aDate, key, fetchIndex);
    if (exceptionDateFinder == 0) // 0 means that the element was found
    {
        iExceptDates->Delete(fetchIndex);
        // Deleting elements from the array does not cause the
        // array buffer to be automatically compressed.
        // Compress() method returns excess space to the heap.
        iExceptDates->Compress();
    }
}

void CPIMRepeatRule::removeExceptDate(jlong aValue)
{
    JELOG2(EPim);
    TPIMDate nativeValue(java::util::S60CommonUtils::JavaTimeToTTime(aValue));
    RemoveExceptDate(nativeValue);
}

// -----------------------------------------------------------------------------
// CPIMRepeatRule::GetExceptDatesL
// Returns the exception dates.
// Returns: An array of exceptiond dates.
//          The ownership of the array stays with the repeat rule.
// -----------------------------------------------------------------------------
//
const CArrayFix<TPIMDate>& CPIMRepeatRule::GetExceptDatesL() const
{
    JELOG2(EPim);
    return *iExceptDates;
}

jintArray CPIMRepeatRule::getExceptDates(JNIEnv* aJniEnv)
{
    JELOG2(EPim);
    // The ownership of the except dates array stays with the repeat rule.
    const CArrayFix<TPIMDate>* exceptDates = NULL;
    TInt error = KErrNone;
    TRAP(error, exceptDates = &(GetExceptDatesL()));
    if (error != KErrNone || exceptDates == NULL)
    {
        // major error
        return NULL;
    }
    jintArray exceptDatesIntArray = NativeDatesToJavaInts(*aJniEnv,
                                    *exceptDates);
    return exceptDatesIntArray;
}

// -----------------------------------------------------------------------------
// CPIMRepeatRule::GetIntL
// Returns an integer value.
// Returns: An integer value based on the parameter field.
// -----------------------------------------------------------------------------
//
TInt CPIMRepeatRule::GetIntL(TPIMField aField) const
{
    JELOG2(EPim);
    switch (aField)
    {
        // First the two supported repeat rule int fields
    case EPIMRepeatRuleFrequency:
    {
        if (iRepeatRuleFrequency == 0)
        {
            User::Leave(KErrNotFound);
        }
        return iRepeatRuleFrequency;
    }
    case EPIMRepeatRuleInterval:
    {
        if (iRepeatRuleInterval == 0)
        {
            User::Leave(KErrNotFound);
        }
        return iRepeatRuleInterval;
    }

    // Then the rest of the repeat rule fields (not supported by Nokia.)
    case EPIMRepeatRuleDayInMonth:
    {
        if (iRepeatRuleDayInMonth == 0)
        {
            User::Leave(KErrNotFound);
        }
        return iRepeatRuleDayInMonth;
    }
    case EPIMRepeatRuleDayInWeek:
    {
        if (iRepeatRuleDayInWeek == 0)
        {
            User::Leave(KErrNotFound);
        }
        return iRepeatRuleDayInWeek;
    }
    case EPIMRepeatRuleDayInYear:
    {
        if (iRepeatRuleDayInYear == 0)
        {
            User::Leave(KErrNotFound);
        }
        return iRepeatRuleDayInYear;
    }
    case EPIMRepeatRuleMonthInYear:
    {
        if (iRepeatRuleMonthInYear == 0)
        {
            User::Leave(KErrNotFound);
        }
        return iRepeatRuleMonthInYear;
    }
    case EPIMRepeatRuleWeekInMonth:
    {
        if (iRepeatRuleWeekInMonth == 0)
        {
            User::Leave(KErrNotFound);
        }
        return iRepeatRuleWeekInMonth;
    }
    case EPIMRepeatRuleCount:
    {
        if (iRepeatRuleCount == 0)
        {
            User::Leave(KErrNotFound);
        }
        return iRepeatRuleCount;
    }
    default:
    {
        User::Leave(KErrArgument);
        // Even the method leaves here, the compiler requires it to return
        // an integer value. The next line is only because of that.
        return 0;
    }
    }
}

int CPIMRepeatRule::getRepeatInt(TPIMField aField)
{
    JELOG2(EPim);
    TInt error = KErrNone;
    int retVal = 0;
    TRAP(error, retVal = GetIntL(aField));
    if (error != KErrNone)
        throw error;
    return retVal;
}

// -----------------------------------------------------------------------------
// CPIMRepeatRule::SetIntL
// Sets an integer value for the given field.
// -----------------------------------------------------------------------------
//
void CPIMRepeatRule::SetIntL(TPIMField aField, TInt aValue)
{
    JELOG2(EPim);
    switch (aField)
    {
        // First the two supported repeat rule int fields
    case EPIMRepeatRuleFrequency:
    {
        if (aValue == EPIMRepeatRuleDaily || aValue == EPIMRepeatRuleWeekly
                || aValue == EPIMRepeatRuleMonthly || aValue
                == EPIMRepeatRuleYearly)
        {
            iRepeatRuleFrequency = aValue;
        }
        else
        {
            User::Leave(KErrTooBig);
        }
        break;
    }
    case EPIMRepeatRuleInterval:
    {
        if (aValue > 0)
        {
            iRepeatRuleInterval = aValue;
        }
        else
        {
            User::Leave(KErrTooBig);
        }
        break;
    }

    // Then the rest of the repeat rule fields (not supported by Nokia.)
    case EPIMRepeatRuleDayInMonth:
    {
        if (0 < aValue && aValue < 32) // Valid values are 1-31
        {
            iRepeatRuleDayInMonth = aValue;
        }
        else
        {
            User::Leave(KErrTooBig);
        }
        break;
    }
    case EPIMRepeatRuleDayInWeek:
    {
        if (aValue > 0 && aValue <= KPIMRepeatRuleMaskDayInWeek && (aValue
                & KPIMRepeatRuleMaskWeekInMonth) == 0)
        {
            // Value does consist of 1 or more weekdays only
            iRepeatRuleDayInWeek = aValue;
        }
        else
        {
            User::Leave(KErrTooBig);
        }
        break;
    }
    case EPIMRepeatRuleDayInYear:
    {
        if (0 < aValue && aValue < 367) // Valid values are 1-366
        {
            iRepeatRuleDayInYear = aValue;
        }
        else
        {
            User::Leave(KErrTooBig);
        }
        break;
    }
    case EPIMRepeatRuleMonthInYear:
    {
        if (aValue > 0 && aValue <= KPIMRepeatRuleMaskMonthInYear && (aValue
                & KPIMRepeatRuleMaskDayInWeek) == 0)
        {
            // Value does consist of 1 or more months only
            iRepeatRuleMonthInYear = aValue;
        }
        else
        {
            User::Leave(KErrTooBig);
        }
        break;
    }
    case EPIMRepeatRuleWeekInMonth:
    {
        if (aValue > 0 && aValue <= KPIMRepeatRuleMaskWeekInMonth)
        {
            // Value does consist of 1 or more weeks in month only
            iRepeatRuleWeekInMonth = aValue;
        }
        else
        {
            User::Leave(KErrTooBig);
        }
        break;
    }
    case EPIMRepeatRuleCount:
    {
        if (aValue > 0)
        {
            iRepeatRuleCount = aValue;
        }
        else
        {
            User::Leave(KErrTooBig);
        }
        break;
    }
    default:
    {
        User::Leave(KErrArgument);
    }
    }
}

void CPIMRepeatRule::setRepeatInt(TPIMField aField, int aValue)
{
    JELOG2(EPim);
    TInt error = KErrNone;
    TRAP(error, SetIntL(aField, aValue));
    if (error != KErrNone)
        throw error;

}

// -----------------------------------------------------------------------------
// CPIMRepeatRule::GetDateL
// Returns a date value.
// Returns: A date value based on the parameter field.
// -----------------------------------------------------------------------------
//
TPIMDate CPIMRepeatRule::GetDateL(TPIMField aField) const
{
    JELOG2(EPim);
    if (aField != EPIMRepeatRuleEnd)
    {
        User::Leave(KErrArgument);
    }

    if (iRepeatRuleEndDate == Time::NullTTime())
    {
        User::Leave(KErrNotFound);
    }

    return iRepeatRuleEndDate;
}

jlong CPIMRepeatRule::getRepeatDate(TPIMField aField)
{
    JELOG2(EPim);
    TInt error = KErrNone;
    TPIMDate value;
    TRAP(error, value = GetDateL(aField));
    if (error != KErrNone)
        throw error;
    jlong retVal = java::util::S60CommonUtils::TTimeToJavaTime(value);
    return retVal;
}

// -----------------------------------------------------------------------------
// CPIMRepeatRule::SetDateL
// Sets a date value for the given field.
// -----------------------------------------------------------------------------
//
void CPIMRepeatRule::SetDateL(TPIMField aField, TPIMDate aDate)
{
    JELOG2(EPim);
    if (aField == EPIMRepeatRuleEnd)
    {
        if (!PIMRepeatRuleConverter::IsValidAgendaDate(aDate))
        {
            User::Leave(KErrTooBig);
        }
        iRepeatRuleEndDate = aDate;
    }
    else
    {
        User::Leave(KErrArgument);
    }
}

void CPIMRepeatRule::setDate(TPIMField aField, jlong aValue)
{
    JELOG2(EPim);
    TPIMDate nativeValue(java::util::S60CommonUtils::JavaTimeToTTime(aValue));
    TInt error = KErrNone;
    TRAP(error, SetDateL(aField, nativeValue));
    if (error != KErrNone)
        throw error;
}

// -----------------------------------------------------------------------------
// CPIMRepeatRule::GetFieldsL
// Returns the fields that are in use.
// Returns: An array of repeat rule fields that are in use.
//          The ownership of the array is transferred to the caller.
// -----------------------------------------------------------------------------
//
CArrayFix<TPIMField>* CPIMRepeatRule::GetFieldsL() const
{
    JELOG2(EPim);
    CArrayFixFlat<TPIMField>* repeatRuleFields = new(ELeave) CArrayFixFlat<
    TPIMField> (1);
    CleanupStack::PushL(repeatRuleFields);

    // First the three supported repeat rule fields
    if (iRepeatRuleFrequency != 0)
    {
        repeatRuleFields->AppendL(EPIMRepeatRuleFrequency);
    }
    if (iRepeatRuleInterval != 0)
    {
        repeatRuleFields->AppendL(EPIMRepeatRuleInterval);
    }
    if (iRepeatRuleEndDate != Time::NullTTime())
    {
        repeatRuleFields->AppendL(EPIMRepeatRuleEnd);
    }

    // Then the rest of the repeat rule fields (not supported by Nokia.)
    if (iRepeatRuleDayInMonth != 0)
    {
        repeatRuleFields->AppendL(EPIMRepeatRuleDayInMonth);
    }
    if (iRepeatRuleDayInWeek != 0)
    {
        repeatRuleFields->AppendL(EPIMRepeatRuleDayInWeek);
    }
    if (iRepeatRuleDayInYear != 0)
    {
        repeatRuleFields->AppendL(EPIMRepeatRuleDayInYear);
    }
    if (iRepeatRuleMonthInYear != 0)
    {
        repeatRuleFields->AppendL(EPIMRepeatRuleMonthInYear);
    }
    if (iRepeatRuleWeekInMonth != 0)
    {
        repeatRuleFields->AppendL(EPIMRepeatRuleWeekInMonth);
    }
    if (iRepeatRuleCount != 0)
    {
        repeatRuleFields->AppendL(EPIMRepeatRuleCount);
    }

    CleanupStack::Pop(repeatRuleFields);
    return repeatRuleFields;
}

jintArray CPIMRepeatRule::getFields(JNIEnv* aJniEnv)
{
    JELOG2(EPim);
    CArrayFix<TPIMField>* fields = NULL;
    TInt error = KErrNone;
    TRAP(error, fields = GetFieldsL());
    // We now own the field array

    if (error != KErrNone || fields == NULL)
    {
        delete fields;
        return NULL;
    }

    const TInt numFields = fields->Count();

    jintArray javaFields = aJniEnv->NewIntArray(numFields);

    if (javaFields == NULL)
    {
        delete fields;
        return NULL;
    }

    if (numFields > 0)
    {
        aJniEnv->SetIntArrayRegion(javaFields, 0, numFields, fields->Back(0));
    }

    delete fields;
    return javaFields;
}

// -----------------------------------------------------------------------------
// CPIMRepeatRule::ClearFieldL
// Clears the value of the field given as a parameter.
// -----------------------------------------------------------------------------
//
void CPIMRepeatRule::ClearFieldL(TPIMField aField)
{
    JELOG2(EPim);
    switch (aField)
    {
        // First the three supported repeat rule fields
    case EPIMRepeatRuleFrequency:
    {
        iRepeatRuleFrequency = 0;
        break;
    }
    case EPIMRepeatRuleInterval:
    {
        iRepeatRuleInterval = 0;
        break;
    }
    case EPIMRepeatRuleEnd:
    {
        iRepeatRuleEndDate = Time::NullTTime();
        break;
    }

    // Then the rest of the repeat rule fields (not supported by Nokia.)
    case EPIMRepeatRuleDayInMonth:
    {
        iRepeatRuleDayInMonth = 0;
        break;
    }
    case EPIMRepeatRuleDayInWeek:
    {
        iRepeatRuleDayInWeek = 0;
        break;
    }
    case EPIMRepeatRuleDayInYear:
    {
        iRepeatRuleDayInYear = 0;
        break;
    }
    case EPIMRepeatRuleMonthInYear:
    {
        iRepeatRuleMonthInYear = 0;
        break;
    }
    case EPIMRepeatRuleWeekInMonth:
    {
        iRepeatRuleWeekInMonth = 0;
        break;
    }
    case EPIMRepeatRuleCount:
    {
        iRepeatRuleCount = 0;
        break;
    }
    default:
    {
        User::Leave(KErrArgument);
    }
    }
}

// -----------------------------------------------------------------------------
// CPIMRepeatRule::Clear
// Clears all values of the repeat rule.
// -----------------------------------------------------------------------------
//
void CPIMRepeatRule::clear()
{
    JELOG2(EPim);
    iExceptDates->Reset();
    iRepeatRuleFrequency = 0;
    iRepeatRuleInterval = 0;
    iRepeatRuleEndDate = Time::NullTTime();
    iRepeatRuleDayInMonth = 0;
    iRepeatRuleDayInWeek = 0;
    iRepeatRuleDayInYear = 0;
    iRepeatRuleMonthInYear = 0;
    iRepeatRuleWeekInMonth = 0;
    iRepeatRuleCount = 0;
}

// -----------------------------------------------------------------------------
// CPIMRepeatRule::BackupRepeatL
// Copies all the fields from the Agenda model's Repeat definition.
// -----------------------------------------------------------------------------
//
void CPIMRepeatRule::BackupRepeatL()
{
    JELOG2(EPim);
    for (TInt i = 0; i < iExceptDates->Count(); i++)
    {
        iOriginalExceptDates->AppendL(iExceptDates->At(i));
    }
    iOriginalRepeatRuleFrequency = iRepeatRuleFrequency;
    iOriginalRepeatRuleInterval = iRepeatRuleInterval;
    iOriginalRepeatRuleEndDate = iRepeatRuleEndDate;
    iOriginalRepeatRuleDayInMonth = iRepeatRuleDayInMonth;
    iOriginalRepeatRuleDayInWeek = iRepeatRuleDayInWeek;
    iOriginalRepeatRuleDayInYear = iRepeatRuleDayInYear;
    iOriginalRepeatRuleMonthInYear = iRepeatRuleMonthInYear;
    iOriginalRepeatRuleWeekInMonth = iRepeatRuleWeekInMonth;
    iOriginalRepeatRuleCount = iRepeatRuleCount;
}

// -----------------------------------------------------------------------------
// CPIMRepeatRule::IsModified
// Checks if the repeat rule has been changed or not.
// -----------------------------------------------------------------------------
//
TBool CPIMRepeatRule::IsModified() const
{
    JELOG2(EPim);
    // First check that has the exception dates amount changed
    if (iExceptDates->Count() != iOriginalExceptDates->Count())
    {
        return ETrue;
    }

    // If original repeat had same amount of exception dates, compare all dates
    if (iExceptDates->Count() > 0 && iOriginalExceptDates->Count() > 0)
    {
        TInt exceptionDateFinder;
        TInt fetchIndex = 0;
        TKeyArrayFix key(0, ECmpTInt64);
        for (TInt i = 0; i < iExceptDates->Count(); i++)
        {
            const TPIMDate& exceptDate = iExceptDates->At(i);
            exceptionDateFinder = iOriginalExceptDates->Find(exceptDate, key,
                                  fetchIndex);
            if (exceptionDateFinder != 0) // 0 would mean that date was found
            {
                return ETrue;
            }
        }
    }

    // Exception dates comparison passed, continue with the rest of the fields
    if (iOriginalRepeatRuleFrequency == iRepeatRuleFrequency
            && iOriginalRepeatRuleInterval == iRepeatRuleInterval
            && iOriginalRepeatRuleEndDate == iRepeatRuleEndDate
            && iOriginalRepeatRuleDayInMonth == iRepeatRuleDayInMonth
            && iOriginalRepeatRuleDayInWeek == iRepeatRuleDayInWeek
            && iOriginalRepeatRuleDayInYear == iRepeatRuleDayInYear
            && iOriginalRepeatRuleMonthInYear == iRepeatRuleMonthInYear
            && iOriginalRepeatRuleWeekInMonth == iRepeatRuleWeekInMonth
            && iOriginalRepeatRuleCount == iRepeatRuleCount)
    {
        // All the fields are identical, the repeat has not been changed
        return EFalse;
    }
    else
    {
        // At least one of the fields was different, the repeat has been changed
        return ETrue;
    }
}

//  End of File
