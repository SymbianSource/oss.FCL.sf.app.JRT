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
* Description:  Handles PIM API todo item <-> Agenda Model todo conversions
 *
*/


// CLASS HEADER
#include "cpimagntodoadapter.h"

// INTERNAL INCLUDES
#include "mpimtodoitem.h"
#include "mpimitemdata.h"
#include "pimpanics.h"
#include "pimtypes.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <e32math.h>
#include <calentry.h>
#include <calalarm.h>


// UNNAMED LOCAL NAMESPACE
namespace
{
// Seconds in a one minute
const TInt KPIMSecondsInMinute = 60;
// Maximum length of ToDo item string fields in S60
const TInt KPIMToDoStringValueMaxLength = 160;
// Alarm offset for TTime in seconds from the start of the date. This is
// the default alarm value which is set to the item if the user defined
// alarm value is too small (meaning that it has been passed to the next
// day when start of the event occurs
const TInt KPIMDefaultAlarmInterval = 43200;
}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMAgnToDoAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMAgnToDoAdapter* CPIMAgnToDoAdapter::NewL(
    java::util::FunctionServer* aFuncServer)
{
    JELOG2(EPim);
    CPIMAgnToDoAdapter* self = new(ELeave) CPIMAgnToDoAdapter(aFuncServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// destructor
CPIMAgnToDoAdapter::~CPIMAgnToDoAdapter()
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMAgnToDoAdapter::CreateItemToAgnL
// Creates a new Agenda Model To-Do item from a PIM item data.
// Returns: A To-do item based on PIM item data.
//          The ownership of the To-do item is transferred to the caller.
// -----------------------------------------------------------------------------
//
CCalEntry* CPIMAgnToDoAdapter::CreateItemToAgnL(const MPIMToDoItem& aItem)
{
    JELOG2(EPim);
    // Create new calendar entry
    CCalEntry* entry = CreateCalendarEntryLC(CCalEntry::ETodo);
    // Export entry. No need to reset since this is a new entry
    ExportItemL(aItem, *entry, EFalse);
    CleanupStack::Pop(entry);
    return entry;
}

// -----------------------------------------------------------------------------
// CPIMAgnToDoAdapter::ReadAgnToItemL
// Reads an Agenda Model To-do item and converts it to a framework PIM item.
// -----------------------------------------------------------------------------
//
void CPIMAgnToDoAdapter::ReadAgnToItemL(MPIMToDoItem& aItem, CCalEntry& aEntry)
{
    JELOG2(EPim);
    MPIMItemData& itemData = aItem.ItemData();
    // Read string fields to PIM API ToDo entry (summary and note)
    ReadStringFieldsL(itemData, aEntry);
    // Read integer fields to PIM API ToDo entry (priority and class)
    ReadIntFieldsL(itemData, aEntry);
    // Read date fields to PIM API ToDo entry (due and completion date). Alarm
    // is read here also because it is related to the due date of the item
    ReadDateFieldsL(itemData, aEntry);
}

// -----------------------------------------------------------------------------
// CPIMAgnToDoAdapter::UpdateItemToAgnL
// Reads an Agenda Model To-do item and updates it based on the PIM item data.
// -----------------------------------------------------------------------------
//
void CPIMAgnToDoAdapter::UpdateItemToAgnL(const MPIMToDoItem& aItem,
        CCalEntry& aEntry)
{
    JELOG2(EPim);
    // Export PIM item data to the native entry. Reset the entry for new data
    ExportItemL(aItem, aEntry, ETrue);
}

// -----------------------------------------------------------------------------
// CPIMAgnToDoAdapter::ExportItemL
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CPIMAgnToDoAdapter::ExportItemL(const MPIMToDoItem& aItem,
                                     CCalEntry& aEntry, TBool aResetEntry)
{
    JELOG2(EPim);
    if (aResetEntry)
    {
        // Reset native entry for exporting new data
        aEntry.SetSummaryL(KNullDesC());
        aEntry.SetDescriptionL(KNullDesC());
        aEntry.SetPriorityL(0);
        aEntry.SetCompletedL(EFalse, aEntry.CompletedTimeL());
    }

    // Export item data to the native ToDo calendar entry
    const MPIMItemData& itemData = aItem.ItemData();
    CArrayFix<TPIMField>* fields = itemData.FieldsLC();

    // Add default values to the calendar entry
    AddDefaultValuesToEntryL(itemData, aEntry);

    // Convert each field to the native ToDo calendar entry
    TInt count = fields->Count();
    for (TInt i = 0; i < count; i++)
    {
        TPIMToDoField field = static_cast<TPIMToDoField>(fields->At(i));
        ConvertToAgnL(field, aEntry, itemData);
    }
    CleanupStack::PopAndDestroy(fields);
}

// -----------------------------------------------------------------------------
// CPIMAgnToDoAdapter::AddDefaultValuesToEntryL
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CPIMAgnToDoAdapter::AddDefaultValuesToEntryL(const MPIMItemData& aData,
        CCalEntry& aEntry) const
{
    JELOG2(EPim);
    // Calendar creates medium priority ToDos by default
    if (!aData.CountValues(EPIMToDoPriority))
    {
        aEntry.SetPriorityL(EPIMToDoNativePriorityMedium);
    }
    // Calendar uses private synchronization by default
    if (!aData.CountValues(EPIMToDoClass))
    {
        aEntry.SetReplicationStatusL(CCalEntry::EPrivate);
    }
    // Calendar does not support timed ToDo so the time is set to 00:00 o'clock
    if (!aData.CountValues(EPIMToDoDue))
    {
        TTime thisTime;
        thisTime.HomeTime();
        // Set time to calendar specific due time. Currently this is the start
        // of the date. Note that No conversion needed since time is local
        TCalTime calThisTime;
        // Set time as local time since acquired above as local time
        calThisTime.SetTimeLocalL(StartOfDay(thisTime));
        aEntry.SetStartAndEndTimeL(calThisTime, calThisTime);
    }
    if (!aData.CountValues(EPIMToDoCompletionDate) && !aData.CountValues(
                EPIMToDoCompleted))
    {
        aEntry.SetCompletedL(EFalse, aEntry.CompletedTimeL());
    }

}

// -----------------------------------------------------------------------------
// CPIMAgnToDoAdapter::ReadStringFieldsL
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CPIMAgnToDoAdapter::ReadStringFieldsL(MPIMItemData& aData,
        CCalEntry& aEntry)
{
    JELOG2(EPim);
    // Summary is converted to PIM API ToDo summary field
    const TDesC& sum = aEntry.SummaryL();
    if (sum != KNullDesC)
    {
        TPIMFieldData fieldData(EPIMToDoSummary, KPIMAttrNone, sum.AllocLC());
        aData.AddValueL(fieldData);
        CleanupStack::Pop(); // agnSummary.AllocLC()
    }
    // Description is converted to PIM API ToDo note field
    const TDesC& note = aEntry.DescriptionL();
    if (note != KNullDesC)
    {
        TPIMFieldData fieldData(EPIMToDoNote, KPIMAttrNone, note.AllocLC());
        aData.AddValueL(fieldData);
        CleanupStack::Pop(); // AllocLC
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnToDoAdapter::ReadIntFieldsL
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CPIMAgnToDoAdapter::ReadIntFieldsL(MPIMItemData& aData, CCalEntry& aEntry)
{
    JELOG2(EPim);
    // Convert synchornization field to PIM API. The default value is private
    // and unrecognized priority values are mapped also to private due to
    // security reasons
    TPIMToDoClassValue value = EPIMToDoClassPrivate;
    const CCalEntry::TReplicationStatus status = aEntry.ReplicationStatusL();
    if (status == CCalEntry::EOpen)
    {
        // Open is mapped as public synhronization
        value = EPIMToDoClassPublic;
    }
    else if (status == CCalEntry::ERestricted)
    {
        // Open is mapped as user confidential synhronization
        value = EPIMToDoClassConfidential;
    }
    // Note that boolean and integer fields must be identified in the constructor
    TPIMFieldData classData(EPIMToDoClass, EPIMFieldInt, KPIMAttrNone, value);
    aData.AddValueL(classData);

    // Default value is zero acording to vCalendar specification. PIM API default
    // priority is medium which indicates normal native priority
    // See common/pimtodo.h for platform-specific native priorities.
    TUint nativePriority = aEntry.PriorityL();
    TPIMToDoPriority priority = EPIMToDoPriorityMedium;
    if (nativePriority == EPIMToDoNativePriorityHigh)
    {
        // Native priority value High is mapped to value 1
        priority = EPIMToDoPriorityHigh;
    }
    else if (nativePriority == EPIMToDoNativePriorityLow)
    {
        // Native priority value Low is mapped to value 7
        priority = EPIMToDoPriorityLow;
    }
    // Note that boolean and integer fields must be identified in the constructor
    TPIMFieldData
    prData(EPIMToDoPriority, EPIMFieldInt, KPIMAttrNone, priority);
    aData.AddValueL(prData);
}

// -----------------------------------------------------------------------------
// CPIMAgnToDoAdapter::ReadDateFieldsL
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CPIMAgnToDoAdapter::ReadDateFieldsL(MPIMItemData& aData, CCalEntry& aEntry)
{
    JELOG2(EPim);
    TTime nullTime = Time::NullTTime();
    // The Agenda todo entry end field is the due date
    TTime due(aEntry.EndTimeL().TimeLocalL());
    if (due != nullTime)
    {
        // Set due to the PIM API specific due date, in this case, the start of date
        // Note that PIM API uses times as UTC times so the due date must be in
        // correct format. Previously requested as local time -> do not change
        TPIMDate pimDueDate(StartOfDay(due));
        // Date must be converted UTC time because acquired as local above
        ConvertTimeL(pimDueDate, EPIMDateUTC);
        TPIMFieldData dueFieldData(EPIMToDoDue, KPIMAttrNone, pimDueDate);
        aData.AddValueL(dueFieldData);

        // Get alarm. Ownership is transferred to the caller. Alarm cannot be set
        // if the due date is not set because the calculation is done as an offset
        // from the ToDo due date.
        CCalAlarm* calAlarm = aEntry.AlarmL();
        if (calAlarm)
        {
            TTimeIntervalMinutes nativeValue = calAlarm->TimeOffset();
            // The alarm is not needed anymore so it can be deleted
            delete calAlarm;
            calAlarm = NULL;
            // Change the time to the start of the due date
            TTime startOfDayLocal(StartOfDay(due));
            // Calculate the difference from the start of due date and start time including
            // the original alarm offset which was previously read
            TTimeIntervalMinutes temp(0);
            User::LeaveIfError(startOfDayLocal.MinutesFrom(due, temp));
            // Since it is not possible to substract TTimeIntervalMinutes
            // from TTime (probably a Symbian error), the difference has
            // to be calculated using the following way...
            TInt alarm = (nativeValue.Int() + temp.Int()) * KPIMSecondsInMinute;
            // Add alarm value to the item
            TPIMFieldData fieldData(EPIMToDoExtAlarm, EPIMFieldInt,
                                    KPIMAttrNone, alarm);
            // Add value to the PIM item data
            aData.AddValueL(fieldData);
        }
    }

    // Completion date. If the item has a completion date, the item is then completed
    // and completed flag is set to true in PIM API. Null time if not crossed out.
    TTime completed = aEntry.CompletedTimeL().TimeUtcL();
    if (completed != nullTime)
    {
        TPIMFieldData dateData(EPIMToDoCompletionDate, KPIMAttrNone, completed);
        aData.AddValueL(dateData);
        // Note that boolean and integer fields must be identified in the constructor
        TPIMFieldData flag(EPIMToDoCompleted, EPIMFieldBoolean, KPIMAttrNone,
                           ETrue);
        aData.AddValueL(flag);
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnToDoAdapter::ConvertToAgnL
// Makes the conversion from framework PIM item data field to To-do item field.
// -----------------------------------------------------------------------------
//
void CPIMAgnToDoAdapter::ConvertToAgnL(TPIMToDoField aField, // The field to be converted
                                       CCalEntry& aEntry, const MPIMItemData& aItem)
{
    JELOG2(EPim);
    TInt amount = aItem.CountValues(aField);
    for (TInt index = 0; index < amount; index++)
    {
        switch (aField)
        {
        case EPIMToDoCompletionDate: // fallthrough
        case EPIMToDoDue:
        {
            ConvertDateToAgnL(aField, index, aEntry, aItem);
            break;
        }
        case EPIMToDoSummary: // fallthrough
        case EPIMToDoNote:
        {
            ConvertStringToAgnL(aField, index, aEntry, aItem);
            break;
        }
        case EPIMToDoClass: // fallthrough
        case EPIMToDoExtAlarm:
        case EPIMToDoPriority:
        {
            ConvertIntToAgnL(aField, index, aEntry, aItem);
            break;
        }
        case EPIMToDoCompleted:
        {
            ConvertBooleanToAgnL(aField, index, aEntry, aItem);
            break;
        }
        case EPIMToDoUid: // fallthrough
        case EPIMToDoRevision:
        {
            // nothing
            break;
        }
        default:
        {
            __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory,
                                               EPIMPanicUnsupportedField));
        }
        }
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnToDoAdapter::ConvertDateToAgnL
// Makes date conversion from framework PIM item data field to To-do item field.
// -----------------------------------------------------------------------------
//
void CPIMAgnToDoAdapter::ConvertDateToAgnL(TPIMToDoField aField, // Date field to be converted
        TInt aIndex, // Index of the date field
        CCalEntry& aEntry, const MPIMItemData& aItem) // The PIM item to read the field from
{
    JELOG2(EPim);
    const TPIMFieldData fieldData = aItem.ValueL(aField, aIndex);
    const TPIMDate& date = fieldData.DateValue();
    switch (aField)
    {
    case EPIMToDoDue:
    {
        // Because undated to-dos are possible, the due date can be set
        // to a null value.
        if (date != Time::NullTTime() && !IsDateInValidAgendaRange(date))
        {
            User::Leave(KErrAbort);
        }
        // Java dates cannot be set to native null TTime
        else
        {
            // Convert due date and time to calendar specific due time
            // Note that PIM API dates are in UTC time format -> convert
            TTime dueDate(date);
            ConvertTimeL(dueDate, EPIMDateLocal);
            // Set time to native entry. Note that the time is local
            TCalTime calDate;
            calDate.SetTimeLocalL(StartOfDay(dueDate));
            aEntry.SetStartAndEndTimeL(calDate, calDate);
        }
        break;
    }
    case EPIMToDoCompletionDate:
    {
        if (date != Time::NullTTime())
        {
            __ASSERT_ALWAYS(IsDateInValidAgendaRange(date), User::Leave(
                                KErrAbort));
            TCalTime calDate;
            calDate.SetTimeUtcL(date);
            aEntry.SetCompletedL(ETrue, calDate);
        }
        else
        {
            aEntry.SetCompletedL(EFalse, aEntry.CompletedTimeL());
        }
        break;
    }
    default:
    {
        __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory,
                                           EPIMPanicUnsupportedField));
    }
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnToDoAdapter::ConvertStringToAgnL
// Makes string conversion from framework PIM item data field to To-do item field
// -----------------------------------------------------------------------------
//
void CPIMAgnToDoAdapter::ConvertStringToAgnL(TPIMToDoField aField, // String field to be converted
        TInt aIndex, // Index of the date field
        CCalEntry& aEntry, // The Agenda Model entry
        const MPIMItemData& aItem) // The PIM item to read the field from
{
    JELOG2(EPim);
    const TPIMFieldData fieldData = aItem.ValueL(aField, aIndex);
    const TDesC& string = fieldData.StringValue();

    // Check that string is not too long
    __ASSERT_ALWAYS(string.Length() <= KPIMToDoStringValueMaxLength,
                    User::Leave(KErrTooBig));

    switch (aField)
    {
    case EPIMToDoSummary:
    {
        aEntry.SetSummaryL(string);
        break;
    }
    case EPIMToDoNote:
    {
        aEntry.SetDescriptionL(string);
        break;
    }
    default:
    {
        // Should not happen
    }
    }

}

// -----------------------------------------------------------------------------
// CPIMAgnToDoAdapter::ConvertIntToAgnL
// Makes int conversion from framework PIM item data field to To-do item field.
// -----------------------------------------------------------------------------
//
void CPIMAgnToDoAdapter::ConvertIntToAgnL(TPIMToDoField aField, // Int field to be converted
        TInt aIndex, // Index of the date field
        CCalEntry& aEntry, // The Agenda model entry typecasted to a Todo item
        const MPIMItemData& aItemData) // The PIM item to read the field from
{
    JELOG2(EPim);
    const TPIMFieldData fieldData = aItemData.ValueL(aField, aIndex);
    switch (aField)
    {
    case EPIMToDoPriority:
    {
        TInt intField = fieldData.IntegerValue();

        if ((EPIMToDoPriorityHigh <= intField) && (intField
                < EPIMToDoPriorityMedium))
        {
            aEntry.SetPriorityL(EPIMToDoNativePriorityHigh);
        }
        else if ((EPIMToDoPriorityMedium <= intField) && (intField
                 < EPIMToDoPriorityLow))
        {
            aEntry.SetPriorityL(EPIMToDoNativePriorityMedium);
        }
        else if ((EPIMToDoPriorityLow <= intField) && (intField
                 <= EPIMToDoPriorityMaxValue))
        {
            aEntry.SetPriorityL(EPIMToDoNativePriorityLow);
        }
        else
        {
            // From requirement specification: Imported to-do items with
            // priority set to zero must be mapped to the native priority
            // value Medium.
            aEntry.SetPriorityL(EPIMToDoNativePriorityMedium);
        }
        break;
    }
    case EPIMToDoClass:
    {
        CCalEntry::TReplicationStatus replicationStatus = CCalEntry::EPrivate;

        // Single value assumed
        TInt classValue = fieldData.IntegerValue();

        switch (classValue)
        {
        case EPIMToDoClassPrivate:
        {
            replicationStatus = CCalEntry::EPrivate;
            break;
        }
        case EPIMToDoClassConfidential:
        {
            replicationStatus = CCalEntry::ERestricted;
            break;
        }
        case EPIMToDoClassPublic:
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
        break;
    }
    case EPIMToDoExtAlarm:
    {
        CCalAlarm* agnAlarm = CCalAlarm::NewL();
        CleanupStack::PushL(agnAlarm);
        agnAlarm->SetTimeOffset(AlarmOffsetL(aItemData, aEntry));
        aEntry.SetAlarmL(agnAlarm);
        CleanupStack::PopAndDestroy(agnAlarm);
        break;
    }
    default:
    {
        // Should not happen
        __ASSERT_DEBUG(EFalse, User::Invariant());
    }
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnToDoAdapter::ConvertBooleanToAgnL
// Makes boolean conversion from framework PIM item data field to To-do item field
// -----------------------------------------------------------------------------
//
void CPIMAgnToDoAdapter::ConvertBooleanToAgnL(TPIMToDoField aField, // Boolean field to be converted
        TInt aIndex, // Index of the date field
        CCalEntry& aEntry, // The Agenda model entry typecasted to a Todo item
        const MPIMItemData& aItem) // The PIM item to read the field from
{
    JELOG2(EPim);
    const TPIMFieldData fieldData = aItem.ValueL(aField, aIndex);
    TBool booleanField = fieldData.BooleanValue();

    if (booleanField) // completed flag is set to value TRUE
    {
        // Check if the completed date field is present
        if (aItem.CountValues(EPIMToDoCompletionDate) == 0)
        {
            // If completed date is not present, use the current time.
            TTime currentTime;
            currentTime.HomeTime();
            TCalTime calCurrentTime;
            // Set time as local time since acquired above as local time
            calCurrentTime.SetTimeLocalL(currentTime);
            aEntry.SetCompletedL(ETrue, calCurrentTime);
        }
        else
        {
            TPIMFieldData completionData = aItem.ValueL(EPIMToDoCompletionDate,
                                           aIndex);
            const TPIMDate& date = completionData.DateValue();
            if (date != Time::NullTTime())
            {
                TCalTime calDate;
                calDate.SetTimeUtcL(date);
                aEntry.SetCompletedL(ETrue, calDate);
            }
            else
            {
                // If completed date is set to null time, use the current time.
                TTime currentTime;
                currentTime.HomeTime();
                TCalTime calCurrentTime;
                // Set time as local time since acquired above as local time
                calCurrentTime.SetTimeLocalL(currentTime);
                aEntry.SetCompletedL(ETrue, calCurrentTime);
            }
        }
    }
    else // completed flag is set to value FALSE
    {
        aEntry.SetCompletedL(EFalse, aEntry.CompletedTimeL());
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnToDoAdapter::AlarmOffsetL
// Calculates PIM alarm offset for native Calendar entry
// The offset calculation is done by calculating the difference between the
// due date and the alarm offset. After we know the alarm date and time we
// calculate difference between calender specific todo due date (which now is
// 00:00)
//
// For example:
//
// If our due time is 23:00 and the alarm offset is 900 seconds before it.
// Then the alarm should launch at 22:45 at the same day. So, the offset which
// we have to write to the native alarm is the difference between 00:00 and 22:45
// which is a negative value because the alarm rises in the future. This all is
// done because the calendar understands only the date of the due date and not the
// exact time of it. The time is ignored and set to 00:00 by default.
// -----------------------------------------------------------------------------
//
TTimeIntervalMinutes CPIMAgnToDoAdapter::AlarmOffsetL(
    const MPIMItemData& aItemData, CCalEntry& aEntry)
{
    JELOG2(EPim);
    // Note that start time must be set before alarm can be calculated
    // Add start to the item so alarm can be properly converted. The
    // native entry does not accept alarm value if start is not present
    // Due has been already set to default time (current home time) if
    // ToDo due field is not present in the field so start should be present
    if (!aItemData.CountValues(EPIMToDoDue))
    {
        User::Leave(KErrArgument);
    }
    else
    {
        ConvertDateToAgnL(EPIMToDoDue, 0, aEntry, aItemData);
    }

    __ASSERT_DEBUG(aEntry.StartTimeL().TimeUtcL() != Time::NullTTime(),
                   User::Panic(KPIMPanicCategory, EPIMPanicInvalidState));

    // Get alarm value from the Java item. There should be only one alarm
    // value supported by the PIM API because native entries do not support
    // multiple alarm values.
    const TPIMFieldData alarmData = aItemData.ValueL(EPIMToDoExtAlarm, 0);
    TInt value = alarmData.IntegerValue();

    // Count the alarm value from the start date of the event
    TTime entryStart = aEntry.StartTimeL().TimeLocalL();
    const TPIMFieldData dateData = aItemData.ValueL(EPIMToDoDue, 0);
    TPIMDate dueDate(dateData.DateValue());
    ConvertTimeL(dueDate, EPIMDateLocal);
    TTimeIntervalSeconds temp(0);
    User::LeaveIfError(entryStart.SecondsFrom(dueDate, temp));

    // Add difference between PIM API start and start which has been
    // converted to the item (in case if the date has been changed, it is
    // reflected here)
    value += temp.Int();

    // Check that if the alarm has passed to the following day. In this case,
    // the alarm is transferred back to 12 o'clock of the current start date
    TTime alarmTime(entryStart - TTimeIntervalSeconds(value));
    // Temporary date. This date is used when calculating if the alarm
    // value has passed to the following date.
    TTime startOfNextDay(StartOfDay(dueDate + TTimeIntervalDays(1)));
    if (alarmTime >= startOfNextDay)
    {
        alarmTime = StartOfDay(entryStart);
        alarmTime += TTimeIntervalSeconds(KPIMDefaultAlarmInterval);
        User::LeaveIfError(entryStart.SecondsFrom(alarmTime, temp));
        value = temp.Int();
    }
    // Convert seconds to minutes
    return TTimeIntervalMinutes(value / KPIMSecondsInMinute);
}

// -----------------------------------------------------------------------------
// CPIMAgnToDoAdapter::CPIMAgnToDoAdapter
// -----------------------------------------------------------------------------
//
CPIMAgnToDoAdapter::CPIMAgnToDoAdapter(java::util::FunctionServer* aFuncServer) :
        CPIMAgnItemAdapter(aFuncServer)
{
    JELOG2(EPim);
}

// End of File
