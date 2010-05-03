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
* Description:  Base class for PIM Event item adapters
 *
*/


// CLASS HEADER
#include "cpimagneventadapter.h"

// INTERNAL INCLUDES
#include "pimcommon.h"
#include "agneventsupport.h"
#include "cpimrepeatrule.h"
#include "pimrepeatruleconverter.h"
#include "mpimitemdata.h"
#include "mpimeventitem.h"
#include "pimpanics.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <calentryview.h>
#include <calalarm.h>
#include <calrrule.h>


// UNNAMED LOCAL NAMESPACE
namespace
{
const TInt KPIMSecondsInMinute = 60;
// Alarm offset for TTime in seconds from the start of the date. This is
// the default alarm value which is set to the item if the user defined
// alarm value is too small (meaning that it has been passed to the next
// day when start of the event occurs
const TInt KPIMDefaultAlarmInterval = 43200;
}

// LOCAL FUNCTION PROTOTYPES
void CopyExceptionDatesToAgnL(const MPIMRepeatRuleData& aPimRepeatRuleData,
                              CCalEntry& aAgnEntry);

void CopyExceptionDatesToPimL(const CCalEntry& aAgnEntry,
                              MPIMRepeatRuleData& aPimRepeatRuleData);

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CopyExceptionDatesToAgnL
// Copies PIM API exception dates to an Agenda entry.
// Returns: void
// -----------------------------------------------------------------------------
//
void CopyExceptionDatesToAgnL(const MPIMRepeatRuleData& aPimRepeatRuleData,
                              CCalEntry& aAgnEntry)
{
    JELOG2(EPim);
    RArray<TCalTime> agnExceptionDates;
    CleanupClosePushL(agnExceptionDates);
    const CArrayFix<TPIMDate>& pimExceptionDates =
        aPimRepeatRuleData.GetExceptDatesL();
    const TInt numExceptDates = pimExceptionDates.Count();
    TCalTime agnExceptionDate;
    for (TInt i = 0; i < numExceptDates; i++)
    {
        agnExceptionDate.SetTimeUtcL(pimExceptionDates[i]);
        User::LeaveIfError(agnExceptionDates.Append(agnExceptionDate));
    }
    aAgnEntry.SetExceptionDatesL(agnExceptionDates);
    CleanupStack::PopAndDestroy(); // pimExceptionDates close
}

// -----------------------------------------------------------------------------
// CopyExceptionDatesToPimL
// Copies Agenda exception dates to a PIM API repeat rule.
// Returns: void
// -----------------------------------------------------------------------------
//
void CopyExceptionDatesToPimL(const CCalEntry& aAgnEntry,
                              MPIMRepeatRuleData& aPimRepeatRuleData)
{
    JELOG2(EPim);
    RArray<TCalTime> agnExceptionDates;
    aAgnEntry.GetExceptionDatesL(agnExceptionDates);
    CleanupClosePushL(agnExceptionDates);
    const TInt numExceptions = agnExceptionDates.Count();
    for (TInt i = 0; i < numExceptions; i++)
    {
        TCalTime agnExceptionDate = agnExceptionDates[i];
        TTime pimExceptionDate = agnExceptionDate.TimeUtcL();
        aPimRepeatRuleData.AddExceptDateL(pimExceptionDate);
    }

    CleanupStack::PopAndDestroy(); // agnExceptionDates close
}

// ============================ MEMBER FUNCTIONS ===============================

// destructor
CPIMAgnEventAdapter::~CPIMAgnEventAdapter()
{
}

// -----------------------------------------------------------------------------
// CPIMAgnEventAdapter::CPIMAgnEventAdapter
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMAgnEventAdapter::CPIMAgnEventAdapter(
    java::util::FunctionServer* aFuncServer) :
        CPIMAgnItemAdapter(aFuncServer)
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMAgnEventAdapter::ReadFieldsFromAgnL
// Reads Agenda entry's fields and converts them into PIM Item fields.
// -----------------------------------------------------------------------------
//
void CPIMAgnEventAdapter::ReadFieldsFromAgnL(MPIMEventItem& aItem,
        CCalEntry& aEntry, const TPIMField aSupportedFields[], // supported field types
        TInt aSize) // size of the array
{
    JELOG2(EPim);
    for (int i = 0; i < aSize; i++)
    {
        switch (aSupportedFields[i])
        {
        case EPIMEventSummary:
        {
            ReadSummaryFromAgnL(aItem, aEntry);
            break;
        }
        case EPIMEventLocation:
        {
            ReadLocationFromAgnL(aItem, aEntry);
            break;
        }
        case EPIMEventNote:
        {
            ReadNoteFromAgnL(aItem, aEntry);
            break;
        }
        case EPIMEventAlarm:
        {
            ReadAlarmFromAgnL(aItem, aEntry);
            break;
        }
        case EPIMEventStart:
        {
            ReadStartFromAgnL(aItem, aEntry);
            break;
        }
        case EPIMEventEnd:
        {
            ReadEndFromAgnL(aItem, aEntry);
            break;
        }
        case EPIMEventClass:
        {
            ReadClassFromAgnL(aItem, aEntry);
            break;
        }
        case EPIMEventRevision: // fallthrough
        case EPIMEventUid:
        {
            // nothing
            break;
        }
        default:
        {
            User::Leave(KErrArgument);
        }
        }
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnEventAdapter::ConvertFieldsToAgnL
// Reads PIM Item fields and converts them into Agenda entry's fields.
// -----------------------------------------------------------------------------
//
void CPIMAgnEventAdapter::ConvertFieldsToAgnL(const MPIMEventItem& aItem,
        CCalEntry& aEntry)
{
    JELOG2(EPim);
    CArrayFix<TPIMField>* fields = aItem.ItemData().FieldsLC();
    TInt amount = fields->Count();

    for (int i = 0; i < amount; i++)
    {
        TPIMEventField field = static_cast<TPIMEventField>(fields->At(i));
        switch (field)
        {
        case EPIMEventSummary: // Fallthrough
        case EPIMEventLocation: // Fallthrough
        case EPIMEventNote:
        {
            ConvertStringFieldToAgnL(aItem, aEntry, field);
            break;
        }
        case EPIMEventAlarm:
        {
            ConvertAlarmToAgnL(aItem, aEntry);
            break;
        }
        case EPIMEventStart: // Fallthrough
        case EPIMEventEnd:
        {
            ConvertDateFieldToAgnL(aItem, aEntry, field);
            break;
        }
        case EPIMEventClass:
        {
            ConvertClassToAgnL(aItem, aEntry);
            break;
        }
        case EPIMEventRevision: // fallthrough
        case EPIMEventUid:
        {
            // nothing
            break;
        }
        default:
        {
            User::Leave(KErrArgument);
        }
        }
    }
    CleanupStack::PopAndDestroy(fields);
}

// -----------------------------------------------------------------------------
// CPIMAgnEventAdapter::ClearPIMFieldsL
// Clears the fields supported by PIM API from this Agenda Model entry.
// -----------------------------------------------------------------------------
//
void CPIMAgnEventAdapter::ClearPIMFieldsL(CCalEntry& aEntry,
        const TPIMField aSupportedFields[], // Array of supported field types
        TInt aSize) // Size of the array of supported fields
{
    JELOG2(EPim);
    for (int i = 0; i < aSize; i++)
    {
        switch (aSupportedFields[i])
        {
        case EPIMEventSummary:
        {
            aEntry.SetSummaryL(KNullDesC);
            break;
        }
        case EPIMEventLocation:
        {
            aEntry.SetLocationL(KNullDesC);
            break;
        }
        case EPIMEventNote:
        {
            aEntry.SetDescriptionL(KNullDesC);
            break;
        }
        case EPIMEventAlarm:
        {
            aEntry.SetAlarmL(NULL);
            break;
        }
        case EPIMEventStart: // Fallthrough
        case EPIMEventEnd: // Fallthrough
        case EPIMEventUid: // Fallthrough
        case EPIMEventClass: // Fallthrough
        case EPIMEventRevision:
        {
            break; // For these fields there is no need to do anything
        }
        default:
        {
            User::Leave(KErrArgument);
        }
        }
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnEventAdapter::ReadSummaryFromAgnL
// Reads Agenda entry's summary field and converts it into PIM Item field.
// -----------------------------------------------------------------------------
//
void CPIMAgnEventAdapter::ReadSummaryFromAgnL(MPIMEventItem& aItem,
        CCalEntry& aEntry)
{
    JELOG2(EPim);
    const TDesC& agnSummary = aEntry.SummaryL();
    if (agnSummary != KNullDesC)
    {
        TPIMFieldData fieldData(EPIMEventSummary, KPIMAttrNone,
                                agnSummary.AllocLC());
        aItem.ItemData().AddValueL(fieldData);
        CleanupStack::Pop(); // agnSummary.AllocLC()
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnEventAdapter::ReadLocationFromAgnL
// Reads Agenda entry's location field and converts it into PIM Item field.
// -----------------------------------------------------------------------------
//
void CPIMAgnEventAdapter::ReadLocationFromAgnL(MPIMEventItem& aItem,
        CCalEntry& aEntry)
{
    JELOG2(EPim);
    const TDesC& agnLocation = aEntry.LocationL();
    if (agnLocation != KNullDesC)
    {
        TPIMFieldData fieldData(EPIMEventLocation, KPIMAttrNone,
                                agnLocation.AllocLC());
        aItem.ItemData().AddValueL(fieldData);
        CleanupStack::Pop(); // agnLocation.AllocLC()
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnEventAdapter::ReadNoteFromAgnL
// Reads Agenda entry's notes field and converts it into PIM Item field.
// -----------------------------------------------------------------------------
//
void CPIMAgnEventAdapter::ReadNoteFromAgnL(MPIMEventItem& aItem,
        CCalEntry& aEntry)
{
    JELOG2(EPim);
    const TDesC& note = aEntry.DescriptionL();
    if (note != KNullDesC)
    {
        TPIMFieldData fieldData(EPIMEventNote, KPIMAttrNone, note.AllocLC());
        aItem.ItemData().AddValueL(fieldData);
        CleanupStack::Pop(); // notes.AllocLC( )
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnEventAdapter::ConvertStringFieldToAgnL
// Makes string conversion from framework PIM item data field to Appt item field
// -----------------------------------------------------------------------------
//
void CPIMAgnEventAdapter::ConvertStringFieldToAgnL(const MPIMEventItem& aItem,
        CCalEntry& aEntry, const TPIMEventField aField) const
{
    JELOG2(EPim);
    const MPIMItemData& itemData = aItem.ItemData();
    TInt amount = itemData.CountValues(aField);
    for (TInt i = 0; i < amount; i++)
    {
        const TPIMFieldData fieldData = itemData.ValueL(aField, i);
        const TDesC& stringValue = fieldData.StringValue();
        EnsureValidStringValueL(stringValue);
        // Add correct data to the field
        switch (aField)
        {
        case EPIMEventNote:
        {
            aEntry.SetDescriptionL(stringValue);
            break;
        }
        case EPIMEventSummary:
        {
            aEntry.SetSummaryL(stringValue);
            break;
        }
        case EPIMEventLocation:
        {
            aEntry.SetLocationL(stringValue);
            break;
        }
        default:
        {
            __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory,
                                               EPIMPanicUnsupportedStringField));
            break;
        }
        }

    }
}

// -----------------------------------------------------------------------------
// CPIMAgnEventAdapter::ConvertDateFieldToAgnL
// Converts a PIM date field to native Calendar entry
// -----------------------------------------------------------------------------
//
void CPIMAgnEventAdapter::ConvertDateFieldToAgnL(const MPIMEventItem& aItem,
        CCalEntry& aEntry, const TPIMEventField aField)
{
    JELOG2(EPim);
    __ASSERT_DEBUG(aField == EPIMEventStart || aField == EPIMEventEnd,
                   User::Panic(KPIMPanicCategory, EPIMPanicUnsupportedDateField));

    TCalTime end;
    TCalTime start;

    // Get date from the Java item
    const MPIMItemData& itemData = aItem.ItemData();
    // Get date from index 0 since native entries support only one value
    const TPIMFieldData fieldData = itemData.ValueL(aField, 0);
    const TPIMDate date = fieldData.DateValue();

    // Check that the date is inside the valid range
    __ASSERT_ALWAYS(IsDateInValidAgendaRange(date), User::Leave(KErrAbort));
    // By default, set both end and start to the same time and check if there
    // is the other part of the date field present in the item
    start.SetTimeUtcL(date);
    end.SetTimeUtcL(date);

    if (aField == EPIMEventStart && itemData.CountValues(EPIMEventEnd) > 0
            && itemData.ValueL(EPIMEventEnd, 0).DateValue() >= date)
    {
        const TPIMFieldData endData = itemData.ValueL(EPIMEventEnd, 0);
        const TPIMDate endDate = endData.DateValue();
        // Check that the date is inside the valid range
        __ASSERT_ALWAYS(IsDateInValidAgendaRange(endDate), User::Leave(
                            KErrAbort));
        // Set end date to the calendar date in UTC since
        // PIM API does not support local time format
        end.SetTimeUtcL(endDate);
    }
    else if (aField == EPIMEventEnd && itemData.CountValues(EPIMEventStart) > 0
             && itemData.ValueL(EPIMEventStart, 0).DateValue() <= date)
    {
        const TPIMFieldData startData = itemData.ValueL(EPIMEventStart, 0);
        const TPIMDate startDate = startData.DateValue();
        // Check that the date is inside the valid range
        __ASSERT_ALWAYS(IsDateInValidAgendaRange(startDate), User::Leave(
                            KErrAbort));
        // Set end date to the calendar date in UTC since
        // PIM API does not support local time format
        start.SetTimeUtcL(startDate);
    }

    // Set values to the native calendar entry
    aEntry.SetStartAndEndTimeL(start, end);
}

// -----------------------------------------------------------------------------
// CPIMAgnEventAdapter::ReadAlarmFromAgnL
// Reads alarm offset from the native Calendar entry. In case of Anniversary,
// the offset is calculated from the midnight since native Calendar supports
// only dates in these types of entries
// -----------------------------------------------------------------------------
//
void CPIMAgnEventAdapter::ReadAlarmFromAgnL(MPIMEventItem& aItem,
        CCalEntry& aEntry)
{
    JELOG2(EPim);
    CCalAlarm* calAlarm = aEntry.AlarmL();
    // The previous function call returns NULL if there is no alarm
    // set in the item. The ownership is transferred to the caller
    // if the alarm values has been added to the item.
    if (calAlarm)
    {
        TTimeIntervalMinutes nativeValue = calAlarm->TimeOffset();
        // The alarm is not needed anymore so it can be deleted
        delete calAlarm;
        calAlarm = NULL;
        //            nativeValue.Int() );
        // Convert the alarm value based on the start time of the entry
        CCalEntry::TType entryType = aEntry.EntryTypeL();
        // Events (memos) and anniversaries do not have time in the native
        // side, therefore alarm field in those entries need to be calculated
        // from the end of the day
        if (entryType == CCalEntry::EAnniv)
        {
            TTime start(aEntry.StartTimeL().TimeLocalL());
            // Change the time to the end of the start date
            TTime startOfDayLocal(start);
            startOfDayLocal = StartOfDay(startOfDayLocal);
            // Calculate the difference from end of day and start time including
            // the original alarm offset which was previously read
            TTimeIntervalMinutes temp(0);
            User::LeaveIfError(startOfDayLocal.MinutesFrom(start, temp));
            // Since it is not possible to substract TTimeIntervalMinutes
            // from TTime (probably a Symbian error), the difference has
            // to be calculated using the following way...
            nativeValue = nativeValue.Int() + temp.Int();
        }
        TInt alarmValue = nativeValue.Int() * KPIMSecondsInMinute;
        //            alarmValue );
        // Add alarm value to the item
        TPIMFieldData fieldData(EPIMEventAlarm, EPIMFieldInt, KPIMAttrNone,
                                alarmValue);
        aItem.ItemData().AddValueL(fieldData);
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnEventAdapter::ConvertAlarmToAgnL
// Converts alarm from PIM item to a native entry. The alarm is calculated
// from the start date of the event and if it is not present, the alarm field
// is ignored because there is no possibility to calculate it
// -----------------------------------------------------------------------------
//
void CPIMAgnEventAdapter::ConvertAlarmToAgnL(const MPIMEventItem& aItem,
        CCalEntry& aEntry)
{
    JELOG2(EPim);
    const MPIMItemData& itemData = aItem.ItemData();

    // Note that start time must be set before alarm can be calculated
    // Add start to the item so alarm can be properly converted. The
    // native entry does not accept alarm value if start is not present

    if (itemData.CountValues(EPIMEventStart) == 0)
    {
        User::Leave(KErrArgument);
    }
    else
    {
        ConvertDateFieldToAgnL(aItem, aEntry, EPIMEventStart);
    }

    __ASSERT_DEBUG(aEntry.StartTimeL().TimeUtcL() != Time::NullTTime(),
                   User::Panic(KPIMPanicCategory, EPIMPanicInvalidState));

    // Get alarm value from the Java item. There should be only one alarm
    // value supported by the PIM API because native entries do not support
    // multiple alarm values.
    const TPIMFieldData alarmData = itemData.ValueL(EPIMEventAlarm, 0);
    TInt value = alarmData.IntegerValue();

    // Count the alarm value from the start date of the event
    TTime entryStart = aEntry.StartTimeL().TimeLocalL();
    const TPIMFieldData startData = itemData.ValueL(EPIMEventStart, 0);
    TPIMDate startTime = startData.DateValue();
    ConvertTimeL(startTime, EPIMDateLocal);
    TTimeIntervalSeconds temp(0);
    User::LeaveIfError(entryStart.SecondsFrom(startTime, temp));

    // Add difference between PIM API start and start which has been
    // converted to the item (in case if the date has been changed, it is
    // reflected here)
    value += temp.Int();

    // Check that if the alarm has passed to the following day. In this case,
    // the alarm is transferred back to 12 o'clock of the current start date
    TTime alarmTime(entryStart - TTimeIntervalSeconds(value));
    // Temporary date. This date is used when calculating if the alarm
    // value has passed to the following date.
    TTime startOfNextDay(StartOfDay(startTime + TTimeIntervalDays(1)));
    if (alarmTime >= startOfNextDay)
    {
        alarmTime = StartOfDay(entryStart);
        alarmTime += TTimeIntervalSeconds(KPIMDefaultAlarmInterval);
        User::LeaveIfError(entryStart.SecondsFrom(alarmTime, temp));
        value = temp.Int();
    }
    // Convert the alarm value to the native entry
    SetAlarmToEntryL(aEntry, value);
}

// -----------------------------------------------------------------------------
// CPIMAgnEventAdapter::ReadRepeatFromAgnL
// Reads entry's repeat details and converts them into PIM repeat rule.
// -----------------------------------------------------------------------------
//
void CPIMAgnEventAdapter::ReadRepeatFromAgnL(MPIMEventItem& aItem,
        CCalEntry& aEntry)
{
    JELOG2(EPim);
    MPIMRepeatRuleData* repeatRuleData = aItem.GetRepeat();

    TCalRRule agnRRule;
    if (aEntry.GetRRuleL(agnRRule))
    {
        repeatRuleData->clear();

        PIMRepeatRuleConverter::ConvertSupportedRepeatToPIML(*repeatRuleData,
                agnRRule);

        CopyExceptionDatesToPimL(aEntry, *repeatRuleData);

        // Anniv does not need to have an end date, so clear that field to be sure
        if (aEntry.EntryTypeL() == CCalEntry::EAnniv)
        {
            repeatRuleData->ClearFieldL(EPIMRepeatRuleEnd);
        }

        aItem.SetRepeating(ETrue);
    }
    else
    {
        repeatRuleData->clear();
        aItem.SetRepeating(EFalse);
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnEventAdapter::ConvertRepeatToAgnL
// Makes conversion from framework PIM item repeat rule to native entry repeat.
// -----------------------------------------------------------------------------
//
void CPIMAgnEventAdapter::ConvertRepeatToAgnL(
    const MPIMEventItem& aEventItemData, CCalEntry& aAgnEntry)
{
    JELOG2(EPim);
    const MPIMRepeatRuleData* repeatRuleData = aEventItemData.GetRepeat();

    TTime eventStart = aAgnEntry.StartTimeL().TimeUtcL();

    TCalRRule agnRRule = PIMRepeatRuleConverter::ConvertSupportedRepeatToAgnL(
                             *repeatRuleData, eventStart);

    if ((aAgnEntry.EntryTypeL() == CCalEntry::EAnniv) && (agnRRule.Type()
            != TCalRRule::EYearly))
    {
        User::Leave(KErrArgument);
    }

    if (agnRRule.Type() == TCalRRule::EWeekly)
    {
        __ASSERT_ALWAYS(agnRRule.Interval() <= 2, User::Leave(KErrArgument));
    }
    else // other than weekly
    {
        __ASSERT_ALWAYS(agnRRule.Interval() == 1, User::Leave(KErrArgument));
    }

    // Use local time to check the day since UTC times provide incorrect
    // results if the compared dates are near midnight
    TTime untilLocal = agnRRule.Until().TimeLocalL();
    TTime startLocal = aAgnEntry.StartTimeL().TimeLocalL();
    // Validate that repeat rule is correct. The end of the repeat rule
    // must be greater than the event start and it must not be within
    // the same day as the start of the event
    if (untilLocal != Time::NullTTime())
    {
        __ASSERT_ALWAYS(untilLocal >= startLocal, User::Leave(KErrAbort));
        TTimeIntervalDays intervalDays = untilLocal.DaysFrom(startLocal);
        // Interval smaller than one day. Check that the day is not the same
        if (intervalDays < TTimeIntervalDays(1))
        {
            __ASSERT_ALWAYS(untilLocal.DateTime().Day()
                            != startLocal.DateTime().Day(), User::Leave(KErrAbort));
        }
    }

    // Repeat Rules OK
    aAgnEntry.SetRRuleL(agnRRule);
    CopyExceptionDatesToAgnL(*repeatRuleData, aAgnEntry);
}

// -----------------------------------------------------------------------------
// CPIMAgnEventAdapter::ReadClassFromAgnL
// Reads entry's class details and converts them into PIM class.
// -----------------------------------------------------------------------------
//
void CPIMAgnEventAdapter::ReadClassFromAgnL(MPIMEventItem& aItem,
        CCalEntry& aEntry)
{
    JELOG2(EPim);
    TPIMEventClassValue eClassValue = EPIMEventClassPrivate;
    const CCalEntry::TReplicationStatus replicationStatus =
        aEntry.ReplicationStatusL();

    // Map calendar entry values to pim value
    switch (replicationStatus)
    {
    case CCalEntry::EOpen:
    {
        eClassValue = EPIMEventClassPublic;
        break;
    }
    case CCalEntry::EPrivate:
    {
        eClassValue = EPIMEventClassPrivate;
        break;
    }
    case CCalEntry::ERestricted:
    {
        eClassValue = EPIMEventClassConfidential;
        break;
    }
    default:
    {
        User::Leave(KErrArgument);
        break;
    }
    }
    TPIMFieldData fieldData(EPIMEventClass, EPIMFieldInt, KPIMAttrNone,
                            eClassValue);
    // Set value for the class item
    aItem.ItemData().AddValueL(fieldData);
}
// -----------------------------------------------------------------------------
// CPIMAgnEventAdapter::ConvertClassToAgnL
// Makes conversion from framework PIM item class to native entry class.
// -----------------------------------------------------------------------------
//
void CPIMAgnEventAdapter::ConvertClassToAgnL(const MPIMEventItem& aItem,
        CCalEntry& aEntry)
{
    JELOG2(EPim);
    CCalEntry::TReplicationStatus replicationStatus = CCalEntry::EPrivate;

    // There should and MUST be only one value in the
    // class field so get the value from the first index
    const TPIMFieldData& fieldData = aItem.ItemData().ValueL(EPIMEventClass, 0);
    TInt eClassValue = fieldData.IntegerValue();

    // Map pim class value to calendar entry value
    switch (eClassValue)
    {
    case EPIMEventClassPrivate:
    {
        replicationStatus = CCalEntry::EPrivate;
        break;
    }
    case EPIMEventClassConfidential:
    {
        replicationStatus = CCalEntry::ERestricted;
        break;
    }
    case EPIMEventClassPublic:
    {
        replicationStatus = CCalEntry::EOpen;
        break;
    }
    default:
    {
        User::Leave(KErrArgument);
        break;
    }
    }
    aEntry.SetReplicationStatusL(replicationStatus);
}

// -----------------------------------------------------------------------------
// CPIMAgnEventAdapter::SetAlarmToEntryL
// Sets the native entry alarm field.
// -----------------------------------------------------------------------------
//
void CPIMAgnEventAdapter::SetAlarmToEntryL(CCalEntry& aEntry, const TInt aValue) const
{
    JELOG2(EPim);
    TTimeIntervalMinutes offMin(aValue / KPIMSecondsInMinute);
    CCalAlarm* agnAlarm = CCalAlarm::NewL();
    agnAlarm->SetTimeOffset(offMin);

    CleanupStack::PushL(agnAlarm);
    aEntry.SetAlarmL(agnAlarm);
    CleanupStack::PopAndDestroy(agnAlarm);
}

// -----------------------------------------------------------------------------
// CPIMAgnEventAdapter::EnsureValidStringValueL
// -----------------------------------------------------------------------------
//
void CPIMAgnEventAdapter::EnsureValidStringValueL(const TDesC& aStringValue) const
{
    JELOG2(EPim);
    if (aStringValue.Length() > KPIMAgnStringFieldMaxLength)
    {
        User::Leave(KErrTooBig);
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnEventAdapter::AddDefaultValuesToEntryL
// -----------------------------------------------------------------------------
//
void CPIMAgnEventAdapter::AddDefaultValuesToEntryL(const MPIMEventItem& aItem,
        CCalEntry& aEntry) const
{
    JELOG2(EPim);
    const MPIMItemData& data = aItem.ItemData();
    // Default calendar synchronisation is private
    if (data.CountValues(EPIMEventClass) == 0)
    {
        aEntry.SetReplicationStatusL(CCalEntry::EPrivate);
    }
}

// End of File
