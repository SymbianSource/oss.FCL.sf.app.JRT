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
* Description:  Converts event properties PIM API <-> versit
 *
*/


// CLASS HEADER
#include "cpimeventpropertyconverter.h"

// INTERNAL INCLUDES
#include "pimcommon.h" // tpimfield
#include "pimpanics.h"
#include "pimversit.h"
#include "cpimparserproperty.h"
#include "cpimitem.h"
#include "cpimeventitem.h"
#include "mpimrepeatruledata.h"
#include "javasymbianoslayer.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <vcal.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMEventPropertyConverter* CPIMEventPropertyConverter::NewL()
{
    JELOG2(EPim);
    CPIMEventPropertyConverter* self =
        new(ELeave) CPIMEventPropertyConverter();
    return self;
}

// Destructor
CPIMEventPropertyConverter::~CPIMEventPropertyConverter()
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMCardConverter::ConvertPropertyL
// Inserts a proprety from a vEvent to a PIM Event Item as a field
// -----------------------------------------------------------------------------
//
void CPIMEventPropertyConverter::ConvertPropertyL(
    const CParserProperty& aProperty, // property to convert
    CPIMEventItem& aItem, // item to insert the field to
    TPIMDate& aAlarm) // DALARM is stored here
{
    JELOG2(EPim);
    TUid valueTypeUid = aProperty.Uid();
    TInt valueType = valueTypeUid.iUid;

    // The following, rather ugly, cast makes it possible for us to access
    // the protected iArrayOfParams field.
    const CPIMParserProperty& property =
        static_cast<const CPIMParserProperty&>(aProperty);

    if (aProperty.Name().CompareF(KVersitTokenCLASS) == 0)
    {
        ConvertClassPropertyL(property, aItem);
    }
    else
    {
        switch (valueType)
        {
        case KVersitPropertyDateTimeUid:
        {
            // Start, End, DALARM
            ConvertDatePropertyL(property, aItem);
            break;
        }
        case KVersitPropertyHBufCUid:
        {
            // summary, location
            ConvertStringPropertyL(property, aItem);
            break;
        }
        case KVersitPropertyMultiDateTimeUid:
        {
            // exdate
            ConvertExDatePropertyL(property, aItem);
            break;
        }
        case KVCalPropertyRecurrenceUid:
        {
            // repeat rule
            ConvertRepeatRulePropertyL(property, aItem);
            break;
        }
        case KVCalPropertyAlarmUid:
        {
            // DALARM
            ConvertAlarmPropertyL(property, aAlarm);
            break;
        }
        default:
        {
            // don't support, don't care
        }
        }
    }
}

// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::ConvertEventFieldL
// Inserts a Field from a PIM Event Item to CParserVCard as a property.
// -----------------------------------------------------------------------------
//
void CPIMEventPropertyConverter::ConvertEventFieldL(const CPIMItem& aItem, // item to read the field from
        CParserVCalEntity& aParser, // parser to insert the property to.
        TPIMEventField aField) // field to convert
{
    JELOG2(EPim);
    TInt valueCount = aItem.CountValuesL(aField);
    for (TInt i = 0; i < valueCount; i++)
    {
        switch (aField)
        {
        case EPIMEventAlarm:
        {
            ConvertAlarmFieldL(aItem, aParser, aField, i);
            break;
        }
        case EPIMEventStart:
        case EPIMEventEnd:
        case EPIMEventRevision:
        {
            ConvertDateFieldL(aItem, aParser, aField, i);
            break;
        }
        case EPIMEventSummary:
        case EPIMEventLocation:
        case EPIMEventNote:
            // UID is required by VFX. Do not fix even though this is wrong
        case EPIMEventUid:
        {
            ConvertStringFieldL(aItem, aParser, aField, i);
            break;
        }
        case EPIMEventClass:
        {
            ConvertClassFieldL(aItem, aParser, aField, i);
            break;
        }
        default:
        {
            __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory,
                                               EPIMPanicUnsupportedField));
            return;
        }
        }
    }
}

// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::ConvertRepeatRuleL
// Inserts a repeat rule to a parser.
// -----------------------------------------------------------------------------
//
void CPIMEventPropertyConverter::ConvertRepeatRuleL(
    const MPIMRepeatRuleData* aRepeat, CParserVCalEntity& aParser)
{
    JELOG2(EPim);
    // First exception dates
    const CArrayFix<TPIMDate>& exceptionDates = aRepeat->GetExceptDatesL();
    ConvertExceptionDatesL(exceptionDates, aParser);

    // The interval is always set to 1 - only exception is every 2 weeks repeat
    TInt interval = 1;
    TInt frequency = 0;
    TVersitDateTime* versitEndDate = NULL;

    GetRepeatRuleFieldsL(aRepeat, &interval, &frequency, &versitEndDate);
    if (versitEndDate)
    {
        CleanupDeletePushL(versitEndDate);
    }

    CVersitRecurrence* recurrence = CreateRecurrenceL(frequency, interval,
                                    versitEndDate);

    if (versitEndDate)
    {
        CleanupStack::Pop(versitEndDate); // versitEndDate is owned by recurrence now
    }
    CleanupStack::PushL(recurrence);

    CParserPropertyValue* propertyValue =
        new(ELeave) CParserPropertyValueRecurrence(recurrence);
    // recurrence is now owned by propertyValue
    CleanupStack::Pop(recurrence);
    AddPropertyToParserL(propertyValue, KVersitTokenRRULE(), aParser);
    // Needed cleanup stack items are popped out in the AddPropretyToParserL
}

// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::CPIMEventPropertyConverter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMEventPropertyConverter::CPIMEventPropertyConverter()
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::ConvertDatePropertyL
// Converts a date field from a vCard to a PIM Event Item
// -----------------------------------------------------------------------------
//
void CPIMEventPropertyConverter::ConvertDatePropertyL(
    const CPIMParserProperty& aProperty, // property to convert
    CPIMEventItem& aItem)
{
    JELOG2(EPim);
    TPIMField field = aProperty.MatchEventField();
    // Do not match unrecognized fields
    if (field == -1)
    {
        return;
    }

    CParserPropertyValueDate* propertyValue =
        static_cast<CParserPropertyValueDate*>(aProperty.Value());
    const TVersitDateTime* value = propertyValue->Value();

    TPIMDate date(value->iDateTime);
    aItem.AddDateL(field, KPIMAttrNone, date);
}

// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::ConvertAlarmPropertyL
// Converts an alarm field from a vCard to a PIM Event Item
// -----------------------------------------------------------------------------
//
void CPIMEventPropertyConverter::ConvertAlarmPropertyL(
    const CPIMParserProperty& aProperty, // property to convert
    TPIMDate& aAlarm) // item to insert the field to
{
    JELOG2(EPim);
    if (aProperty.MatchEventField() != EPIMEventAlarm)
    {
        return;
    }

    CParserPropertyValueAlarm* propertyValue =
        static_cast<CParserPropertyValueAlarm*>(aProperty.Value());
    const CVersitAlarm* value = propertyValue->Value();

    const TVersitDateTime* versitDT = value->iRunTime;
    TPIMDate date(versitDT->iDateTime);

    aAlarm = date;
}

// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::ConvertStringPropertyL
// Converts a string property from a vCalendar to a PIM Event Item
// -----------------------------------------------------------------------------
//
void CPIMEventPropertyConverter::ConvertStringPropertyL(
    const CPIMParserProperty& aProperty, // property to convert
    CPIMEventItem& aItem) // item to insert the field to
{
    JELOG2(EPim);
    TPIMField field = aProperty.MatchEventField();
    // Do not convert unrecognized fields
    if (field == -1)
    {
        return;
    }

    CParserPropertyValueHBufC* propertyValue =
        static_cast<CParserPropertyValueHBufC*>(aProperty.Value());
    HBufC* value = propertyValue->TakeValueOwnership();
    CleanupStack::PushL(value);
    aItem.AddStringL(field, KPIMAttrNone, value);
    CleanupStack::Pop(value); // owned by aItem
}

// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::ConvertRepeatRulePropertyL
// Inserts RRULE proprety from a vEvent to a PIM Event Item as a repeat rule
// -----------------------------------------------------------------------------
//
void CPIMEventPropertyConverter::ConvertRepeatRulePropertyL(
    const CPIMParserProperty& aProperty, // property to convert
    CPIMEventItem& aItem) // item to insert the field to
{
    JELOG2(EPim);
    if (aProperty.Name().CompareF(KVersitTokenRRULE) != 0)
    {
        // Some unsupported recurrence data (i.e. EXRULE)
        return;
    }
    MPIMRepeatRuleData* repeatRule = aItem.GetRepeat();

    CParserPropertyValueRecurrence* propertyValue =
        static_cast<CParserPropertyValueRecurrence*>(aProperty.Value());
    CVersitRecurrence* value = propertyValue->Value();
    switch (value->iRepeatType)
    {
    case CVersitRecurrence::EDaily:
    {
        repeatRule->SetIntL(EPIMRepeatRuleFrequency, EPIMRepeatRuleDaily);
        break;
    }
    case CVersitRecurrence::EWeekly:
    {
        repeatRule->SetIntL(EPIMRepeatRuleFrequency, EPIMRepeatRuleWeekly);
        repeatRule->SetIntL(EPIMRepeatRuleInterval, value->iInterval);
        break;
    }
    case CVersitRecurrence::EMonthlyByPos:
    case CVersitRecurrence::EMonthlyByDay:
    {
        repeatRule->SetIntL(EPIMRepeatRuleFrequency, EPIMRepeatRuleMonthly);
        break;
    }
    case CVersitRecurrence::EYearlyByMonth:
    case CVersitRecurrence::EYearlyByDay:
    {
        repeatRule->SetIntL(EPIMRepeatRuleFrequency, EPIMRepeatRuleYearly);
        break;
    }
    default:
    {
        // unknown frequency
        return;
    }
    }

    TVersitDateTime* versitEndDate = value->iEndDate;
    if (versitEndDate)
    {
        TPIMDate endDate(versitEndDate->iDateTime);
        repeatRule->SetDateL(EPIMRepeatRuleEnd, endDate);
    }

    aItem.SetRepeating(ETrue);
}
// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::ConvertExDatePropertyL
// Inserts EXDATE proprety from a vEvent to a PIM Event Item as a repeat rule
// field
// -----------------------------------------------------------------------------
//
void CPIMEventPropertyConverter::ConvertExDatePropertyL(
    const CPIMParserProperty& aProperty, // property to convert
    CPIMEventItem& aItem) // item to insert the field to
{
    JELOG2(EPim);
    if (aProperty.Name().CompareF(KVersitTokenEXDATE) != 0)
    {
        // some unsupported multidate
        return;
    }
    MPIMRepeatRuleData* repeatRule = aItem.GetRepeat();

    CParserPropertyValueMultiDateTime* propertyValue =
        static_cast<CParserPropertyValueMultiDateTime*>(aProperty.Value());
    CArrayPtr<TVersitDateTime>* value = propertyValue->Value();
    TInt valueCount = value->Count();
    for (TInt i = 0; i < valueCount; i++)
    {
        TPIMDate date(value->At(i)->iDateTime);
        repeatRule->AddExceptDateL(date);
    }
}

// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::ConvertClassPropertyL
// Converts CLASS property to a PIM Event item.
// -----------------------------------------------------------------------------
//
void CPIMEventPropertyConverter::ConvertClassPropertyL(
    const CPIMParserProperty& aProperty, CPIMEventItem& aItem)
{
    JELOG2(EPim);
    CParserPropertyValueHBufC* propertyValue =
        static_cast<CParserPropertyValueHBufC*>(aProperty.Value());

    const TPtrC classStringPtr = propertyValue->Value();
    TInt classInt = KErrCorrupt;

    if (classStringPtr.CompareF(KPIMClassStringPublic) == 0)
    {
        classInt = EPIMEventClassPublic;
    }
    else if (classStringPtr.CompareF(KPIMClassStringPrivate) == 0)
    {
        classInt = EPIMEventClassPrivate;
    }
    else if (classStringPtr.CompareF(KPIMClassStringConfidential) == 0)
    {
        classInt = EPIMEventClassConfidential;
    }
    // else the class value in the originating vCalendar is flawed - ignore

    if (classInt != KErrCorrupt)
    {
        aItem.addInt(EPIMEventClass, KPIMAttrNone, classInt);
    }
}

// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::ConvertExceptionDatesL
// Inserts exceptiondates to a parser.
// -----------------------------------------------------------------------------
//
void CPIMEventPropertyConverter::ConvertExceptionDatesL(const CArrayFix<
        TPIMDate>& aDates, CParserVCalEntity& aParser)
{
    JELOG2(EPim);
    TInt exDateCount = aDates.Count();
    if (exDateCount > 0)
    {
        CArrayPtrFlat<TVersitDateTime>* versitExDates =
            new(ELeave) CArrayPtrFlat<TVersitDateTime> (exDateCount);
        CleanupStack::PushL(versitExDates);
        CleanupResetAndDestroyPushL(*versitExDates);
        for (TInt i = 0; i < exDateCount; i++)
        {
            TVersitDateTime
            * versitDateTime =
                new(ELeave) TVersitDateTime(aDates.At(i).DateTime(), TVersitDateTime::EIsUTC);
            CleanupDeletePushL(versitDateTime);
            versitExDates->AppendL(versitDateTime);
            CleanupStack::Pop(versitDateTime); // versitDateTime
        }
        CParserPropertyValue* propertyValue =
            new(ELeave) CParserPropertyValueMultiDateTime(versitExDates);
        CleanupStack::Pop(2); // versitExDates is now owned by propertyValue
        AddPropertyToParserL(propertyValue, KVersitTokenEXDATE(), aParser);
        // Needed cleanup stack items are popped out in the AddPropretyToParserL
    }
}
// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::ConvertDateFieldL
// Converts a date field from a PIM Event Item to a CParserVCard.
// -----------------------------------------------------------------------------
//
void CPIMEventPropertyConverter::ConvertDateFieldL(const CPIMItem& aItem, // item to convert from
        CParserVCalEntity& aParser, // parser to insert the property to.
        TPIMEventField aField, // field to convert
        TInt aIndex) // index to the field
{
    JELOG2(EPim);
    const TPIMDate date = aItem.GetDateL(aField, aIndex);
    TDateTime dateTime = date.DateTime();
    TVersitDateTime* versitDateTime =
        new(ELeave) TVersitDateTime(dateTime, TVersitDateTime::EIsUTC);
    CleanupDeletePushL(versitDateTime);

    CParserPropertyValue* propertyValue =
        new(ELeave) CParserPropertyValueDateTime(versitDateTime);
    // versitDateTime is now owned by propertyValue
    CleanupStack::Pop(versitDateTime);

    switch (aField)
    {
    case EPIMEventStart:
    {
        AddPropertyToParserL(propertyValue, KVersitTokenDTSTART(), aParser);
        // Needed cleanup stack items are popped
        // out in the AddPropretyToParserL
        break;
    }
    case EPIMEventEnd:
    {
        AddPropertyToParserL(propertyValue, KVersitTokenDTEND(), aParser);
        // Needed cleanup stack items are popped
        // out in the AddPropretyToParserL
        break;
    }
    case EPIMEventRevision:
    {
        AddPropertyToParserL(propertyValue, KVersitTokenLASTMODIFIED(), aParser);
        // Needed cleanup stack items are popped
        // out in the AddPropretyToParserL
        break;
    }
    default:
    {
        __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory,
                                           EPIMPanicUnsupportedDateField));
    }
    }
}
// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::ConvertAlarmFieldL
// Converts an alarm field from a PIM Event Item to a CParserVCard.
// -----------------------------------------------------------------------------
//
void CPIMEventPropertyConverter::ConvertAlarmFieldL(const CPIMItem& aItem, // item to convert from
        CParserVCalEntity& aParser, // parser to insert the property to.
        TPIMEventField aField, // field to convert
        TInt aIndex) // index to the field
{
    JELOG2(EPim);
    const TInt KAlarmInterval = aItem.getInt(aField, aIndex);
    const TTimeIntervalSeconds KTimeInterval(KAlarmInterval);

    // Check that there exists start date. If item is not written to
    // database there might not be start date present
    if (aItem.CountValuesL(EPIMEventStart) == 0)
    {
        return;
    }

    // Start date present, so conversion can be made
    const TPIMDate KStartDate = aItem.GetDateL(EPIMEventStart, 0);
    TTime alarmTime(KStartDate);
    alarmTime -= KTimeInterval;
    TDateTime dateTime = alarmTime.DateTime();
    TVersitDateTime* versitDateTime =
        new(ELeave) TVersitDateTime(dateTime, TVersitDateTime::EIsUTC);
    CleanupDeletePushL(versitDateTime);

    CVersitAlarm* versitAlarm = CVersitAlarm::NewL(versitDateTime, NULL, 0,
                                KNullDesC, KNullDesC);
    // versitDateTime is now owned by versitAlarm
    CleanupStack::Pop(versitDateTime);
    CleanupStack::PushL(versitAlarm);
    CParserPropertyValue* propertyValue =
        new(ELeave) CParserPropertyValueAlarm(versitAlarm);
    // versitAlarm is now owned by propertyValue
    CleanupStack::Pop(versitAlarm);
    AddPropertyToParserL(propertyValue, KVersitTokenDALARM(), aParser);
    // Needed cleanup stack items are popped out in the AddPropretyToParserL
}
// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::ConvertStringFieldL
// Converts a string field from a PIM Event Item to vCard
// -----------------------------------------------------------------------------
//
void CPIMEventPropertyConverter::ConvertStringFieldL(const CPIMItem& aItem, // item to convert from
        CParserVCalEntity& aParser, // parser to write to
        TPIMEventField aField, // field to convert
        TInt aIndex) // index to the field
{
    JELOG2(EPim);
    const TDesC& string = aItem.GetStringL(aField, aIndex);
    CParserPropertyValue* propertyValue = CParserPropertyValueHBufC::NewL(
                                              string);

    switch (aField)
    {
    case EPIMEventSummary:
    {
        AddPropertyToParserL(propertyValue, KVersitTokenSUMMARY(), aParser);
        // Needed cleanup stack items are popped
        // out in the AddPropretyToParserL
        break;
    }
    case EPIMEventLocation:
    {
        AddPropertyToParserL(propertyValue, KVersitTokenLOCATION(), aParser);
        // Needed cleanup stack items are popped
        // out in the AddPropretyToParserL
        break;
    }
    case EPIMEventNote:
    {
        AddPropertyToParserL(propertyValue, KVersitTokenDESCRIPTION(), aParser);
        // Needed cleanup stack items are popped
        // out in the AddPropretyToParserL
        break;
    }
    case EPIMEventUid:
    {
        AddPropertyToParserL(propertyValue, KVersitTokenUID(), aParser);
        // Needed cleanup stack items are popped
        // out in the AddPropretyToParserL
        break;
    }
    default:
    {
        __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory,
                                           EPIMPanicUnsupportedStringField));
    }
    }
}

// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::ConvertClassFieldL
// Converts Event.CLASS field to a vCalendar.
// -----------------------------------------------------------------------------
//
void CPIMEventPropertyConverter::ConvertClassFieldL(const CPIMItem& aItem,
        CParserVCalEntity& aParser, TPIMEventField aField, TInt aIndex)
{
    JELOG2(EPim);
    const TInt classVal = aItem.getInt(aField, aIndex);
    TPtrC classString(KPIMClassStringConfidential);

    switch (classVal)
    {
    case EPIMEventClassConfidential:
    {
        classString.Set(KPIMClassStringConfidential);
        break;
    }
    case EPIMEventClassPrivate:
    {
        classString.Set(KPIMClassStringPrivate);
        break;
    }
    case EPIMEventClassPublic:
    {
        classString.Set(KPIMClassStringPublic);
        break;
    }
    default:
    {
        __ASSERT_DEBUG(EFalse, User::Invariant());
        break;
    }
    }

    CParserPropertyValue* propertyValue = CParserPropertyValueHBufC::NewL(
                                              classString);
    AddPropertyToParserL(propertyValue, KVersitTokenCLASS(), aParser);
    // Needed cleanup stack items are popped out in the AddPropretyToParserL
}

// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::GetRepeatRuleFieldsL
// Gets the fields from a repeat rule
// -----------------------------------------------------------------------------
//
void CPIMEventPropertyConverter::GetRepeatRuleFieldsL(
    const MPIMRepeatRuleData* aRepeat, // Fields are read from this
    TInt* aInterval, // interval is stored here
    TInt* aFrequency, // frequency is stored here
    TVersitDateTime** aVersitEndDate) // end date is stored here
{
    JELOG2(EPim);
    TBool endExists = EFalse;
    CArrayFix<TPIMField>* repeatFields = aRepeat->GetFieldsL();
    CleanupStack::PushL(repeatFields);
    TInt fieldCount = repeatFields->Count();
    for (TInt i = 0; i < fieldCount; i++)
    {
        switch (repeatFields->At(i))
        {
        case EPIMRepeatRuleFrequency:
        {
            *aFrequency = aRepeat->GetIntL(EPIMRepeatRuleFrequency);
            break;
        }
        case EPIMRepeatRuleInterval:
        {
            *aInterval = aRepeat->GetIntL(EPIMRepeatRuleInterval);
            break;
        }
        case EPIMRepeatRuleEnd:
        {
            endExists = ETrue;
            break;
        }
        default:
        {
            // We ignore other fields
        }
        }
    }
    CleanupStack::PopAndDestroy(repeatFields);
    if (endExists)
    {
        TPIMDate endDate = aRepeat->GetDateL(EPIMRepeatRuleEnd);
        *aVersitEndDate
        = new(ELeave) TVersitDateTime(endDate.DateTime(), TVersitDateTime::EIsUTC);
    }
}
// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::CreateRecurrenceL
// Makes a CVersitRecurrence according to parameters
// -----------------------------------------------------------------------------
//
CVersitRecurrence* CPIMEventPropertyConverter::CreateRecurrenceL(
    TInt aFrequency, TInt aInterval, TVersitDateTime* aVersitEndDate)
{
    JELOG2(EPim);
    CVersitRecurrence* recurrence = NULL;
    switch (aFrequency)
    {
    case EPIMRepeatRuleDaily:
    {
        recurrence
        = new(ELeave) CVersitRecurrenceDaily(aInterval, 0, aVersitEndDate);
        break;
    }
    case EPIMRepeatRuleWeekly:
    {
        recurrence
        = new(ELeave) CVersitRecurrenceWeekly(aInterval, 0, aVersitEndDate, NULL);
        break;
    }
    case EPIMRepeatRuleMonthly:
    {
        recurrence
        = new(ELeave) CVersitRecurrenceMonthlyByDay(
            aInterval, 0, aVersitEndDate, NULL, NULL, NULL);
        break;
    }
    case EPIMRepeatRuleYearly:
    {
        recurrence
        = new(ELeave) CVersitRecurrenceYearlyByDay(aInterval, 0, aVersitEndDate, NULL);
        break;
    }
    default:
    {
        User::Leave(KErrNotSupported);
    }
    }
    return recurrence;
}

// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::AddPropertyToParserL
// adds a new property value to the parser. NOTE that the property value can be
// of any type and it is taken to ownership within this function.
// -----------------------------------------------------------------------------
//
void CPIMEventPropertyConverter::AddPropertyToParserL(
    CParserPropertyValue* aPropertyValue, const TDesC8& aPropertyName,
    CParserVCalEntity& aParser)
{
    JELOG2(EPim);
    // Add property value to the cleanup stack. It should not be added
    // before this call. This function is an exception and reduces much code
    CleanupStack::PushL(aPropertyValue);
    // Create a new parser property from the property value, its name and
    // array of parameters. If there are no parameters, the aArrayOfParams
    // should be NULL and the parser property will be generated without params
    CParserProperty* property = CParserProperty::NewL(*aPropertyValue,
                                aPropertyName, NULL);
    // property takes ownership of the property value
    CleanupStack::Pop(aPropertyValue);
    // NOTE: property MUST not be pushed to the cleanup stack since the
    // AddPropertyL pushes it right away to the cleanup stack. So, we must not
    // push it here to avoid duplicate stack pointers
    aParser.AddPropertyL(property);
    // Property is now owned by the parser but we do not have to pop it
}

// End of file
