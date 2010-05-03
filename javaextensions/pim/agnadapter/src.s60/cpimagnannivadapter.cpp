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
* Description:  Converts PIM Event items and native calendar anniversaries
 *
*/


// CLASS HEADER
#include "cpimagnannivadapter.h"

// INTERNAL INCLUDES
#include "agneventsupport.h"
#include "mpimrepeatruledata.h"
#include "mpimeventitem.h"
#include "mpimitemdata.h"
#include "pimpanics.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <calentry.h>
#include <calrrule.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMAgnAnnivAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMAgnAnnivAdapter* CPIMAgnAnnivAdapter::NewL(
    java::util::FunctionServer* aFuncServer)
{
    JELOG2(EPim);
    CPIMAgnAnnivAdapter* self = new(ELeave) CPIMAgnAnnivAdapter(aFuncServer);
    CleanupStack::PushL(self);
    CallMethodL(self, &CPIMAgnAnnivAdapter::ConstructL, self->iFuncServer);
    CleanupStack::Pop(self);
    return self;
}

// destructor
CPIMAgnAnnivAdapter::~CPIMAgnAnnivAdapter()
{
}

// -----------------------------------------------------------------------------
// CPIMAgnAnnivAdapter::CreateItemToAgnL
// Creates a new Agenda Model Anniv item from a PIM item data.
// Returns: A Anniv item based on PIM item data.
//          The ownership of the Anniv item is transferred to the caller.
// -----------------------------------------------------------------------------
//
CCalEntry* CPIMAgnAnnivAdapter::CreateItemToAgnL(const MPIMEventItem& aItem)
{
    JELOG2(EPim);
    // Create new calendar entry. Native type is anniversary
    CCalEntry* entry = CreateCalendarEntryLC(CCalEntry::EAnniv);
    // Add default values to the item. The default values are used
    // by the native calendar application and therefore needed to be added
    AddDefaultValuesToEntryL(aItem, *entry);
    // Convert fields from Java side to the native side
    ConvertFieldsToAgnL(aItem, *entry);
    // Convert repeat rule to the item if the item supports recurrence orders
    if (aItem.IsRepeating())
    {
        ConvertRepeatToAgnL(aItem, *entry);
    }

    CleanupStack::Pop(entry);
    return entry;
}

// -----------------------------------------------------------------------------
// CPIMAgnAnnivAdapter::ReadAgnToItemL
// Reads an Agenda Model Anniv item and converts it to a framework PIM item.
// -----------------------------------------------------------------------------
//
void CPIMAgnAnnivAdapter::ReadAgnToItemL(MPIMEventItem& aItem,
        CCalEntry& aEntry)
{
    JELOG2(EPim);
    ReadFieldsFromAgnL(aItem, aEntry, KPIMSupportedAnnivFields,
                       KPIMSupportedAnnivFieldsCount);

    TCalRRule tempRule;
    if (aEntry.GetRRuleL(tempRule))
    {
        ReadRepeatFromAgnL(aItem, aEntry);
        aItem.GetRepeat()->BackupRepeatL();
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnAnnivAdapter::UpdateItemToAgnL
// Reads an Agenda Model Anniv item and updates it based on the PIM item data.
// -----------------------------------------------------------------------------
//
void CPIMAgnAnnivAdapter::UpdateItemToAgnL(const MPIMEventItem& aItem,
        CCalEntry& aEntry)
{
    JELOG2(EPim);
    // Before updating values, empty the fields or set default values for the
    // fields that PIM api supports
    ClearPIMFieldsL(aEntry, KPIMSupportedAnnivFields,
                    KPIMSupportedAnnivFieldsCount);

    // Convert fields from Java side to the native side
    ConvertFieldsToAgnL(aItem, aEntry);

    // Convert repeat rule to the native entry if the item is repeating
    // and the repeating rules have been changed in the Java item
    if (aItem.IsRepeating())
    {
        if (aItem.GetRepeat()->IsModified())
        {
            ConvertRepeatToAgnL(aItem, aEntry);
        }
    }
    else
    {
        aEntry.ClearRepeatingPropertiesL();
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnAnnivAdapter::CPIMAgnAnnivAdapter
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMAgnAnnivAdapter::CPIMAgnAnnivAdapter(
    java::util::FunctionServer* aFuncServer) :
        CPIMAgnEventAdapter(aFuncServer)
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMAgnAnnivAdapter::ReadStartFromAgnL
// Reads Agenda entry's start field and converts it into PIM Item field.
// -----------------------------------------------------------------------------
//
void CPIMAgnAnnivAdapter::ReadStartFromAgnL(MPIMEventItem& aItem,
        CCalEntry& aEntry)
{
    JELOG2(EPim);
    // Get time as local time since it needs to be modified
    TTime startTime = aEntry.StartTimeL().TimeLocalL();
    if (startTime != Time::NullTTime())
    {
        // Convert start field to start of the date
        TTime startOfDay(StartOfDay(startTime));
        // Convert date to UTC time because PIM API handles internal times in UTC
        ConvertTimeL(startOfDay, EPIMDateUTC);
        TPIMFieldData fieldData(EPIMEventStart, KPIMAttrNone, startOfDay);
        aItem.ItemData().AddValueL(fieldData);
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnAnnivAdapter::ConvertDateFieldToAgnL
// Converts Date fields from PIM event item to native calendar anniversary entry
// -----------------------------------------------------------------------------
//
void CPIMAgnAnnivAdapter::ConvertDateFieldToAgnL(const MPIMEventItem& aItem,
        CCalEntry& aEntry, const TPIMEventField aField)
{
    JELOG2(EPim);
    __ASSERT_DEBUG(aField == EPIMEventStart, User::Panic(KPIMPanicCategory,
                   EPIMPanicUnsupportedDateField));
    // Get value from the item. There should be only one start date
    // since multiple start dates are not supported by native entries
    const MPIMItemData& itemData = aItem.ItemData();
    const TPIMFieldData fieldData = itemData.ValueL(aField, 0);
    const TPIMDate& date = fieldData.DateValue();

    // Check that the date is inside the valid range
    __ASSERT_ALWAYS(IsDateInValidAgendaRange(date), User::Leave(KErrAbort));

    // By default, times are in UTC time format in PIM API and therefore the
    // time must be converted to local time when changing time to the start of day
    TTime timeLocal(date);
    ConvertTimeL(timeLocal, EPIMDateLocal);
    // Set converted time to the native entry
    TCalTime calTime;
    calTime.SetTimeLocalL(StartOfDay(timeLocal));
    aEntry.SetStartAndEndTimeL(calTime, calTime);
}

// -----------------------------------------------------------------------------
// CPIMAgnAnnivAdapter::ReadEndFromAgnL
// Reads Agenda entry's end field and converts it into PIM Item field.
// -----------------------------------------------------------------------------
//
void CPIMAgnAnnivAdapter::ReadEndFromAgnL(MPIMEventItem&, CCalEntry&)
{
    JELOG2(EPim);
    // Empty implementation, as no END field is supported for Anniversary.
}

// -----------------------------------------------------------------------------
// CPIMAgnAnnivAdapter::AddDefaultValuesToEntryL
// -----------------------------------------------------------------------------
//
void CPIMAgnAnnivAdapter::AddDefaultValuesToEntryL(const MPIMEventItem& aItem,
        CCalEntry& aEntry) const
{
    JELOG2(EPim);
    // Add common event default values to the native entry
    CPIMAgnEventAdapter::AddDefaultValuesToEntryL(aItem, aEntry);
    const MPIMItemData& data = aItem.ItemData();

    if (data.CountValues(EPIMEventStart) == 0)
    {
        TTime local;
        local.HomeTime();
        // Convert start field to start of the date. Time component is ignored
        // Set time as local time since acquired above as local time
        TCalTime calTime;
        calTime.SetTimeLocalL(StartOfDay(local));
        aEntry.SetStartAndEndTimeL(calTime, calTime);
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnAnnivAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMAgnAnnivAdapter::ConstructL()
{
    JELOG2(EPim);
    CPIMAgnEventAdapter::ConstructL();
}

// End of File
