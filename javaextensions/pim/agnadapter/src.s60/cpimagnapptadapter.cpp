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
* Description:  Converts PIM Event items and native calendar entries
 *
*/


// CLASS HEADER
#include "cpimagnapptadapter.h"

// INTERNAL INCLUDES
#include "pimpanics.h"
#include "agneventsupport.h"
#include "mpimrepeatruledata.h"
#include "mpimitemdata.h"
#include "mpimeventitem.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <calentry.h>
#include <calrrule.h>
#include <calalarm.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMAgnApptAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMAgnApptAdapter* CPIMAgnApptAdapter::NewL(
    java::util::FunctionServer* aFuncServer)
{
    JELOG2(EPim);
    CPIMAgnApptAdapter* self = new(ELeave) CPIMAgnApptAdapter(aFuncServer);
    CleanupStack::PushL(self);
    CallMethodL(self, &CPIMAgnApptAdapter::ConstructL, self->iFuncServer);
    CleanupStack::Pop(self);
    return self;
}

// destructor
CPIMAgnApptAdapter::~CPIMAgnApptAdapter()
{
}

// -----------------------------------------------------------------------------
// CPIMAgnApptAdapter::CreateItemToAgnL
// Creates a new Agenda Model Appt item from a PIM item data.
// Returns: A Appt item based on PIM item data.
//          The ownership of the Appt item is transferred to the caller.
// -----------------------------------------------------------------------------
//
CCalEntry* CPIMAgnApptAdapter::CreateItemToAgnL(const MPIMEventItem& aItem)
{
    JELOG2(EPim);
    // Create new calendar entry
    CCalEntry* entry = CreateCalendarEntryLC(CCalEntry::EAppt);
    // Add default values to the item. The default values are used
    // by the native calendar application and therefore needed to be added
    AddDefaultValuesToEntryL(aItem, *entry);
    // Convert fields from Java side to the native side
    ConvertFieldsToAgnL(aItem, *entry);
    // Convert repeat rule since appointments can have recurrence rules
    if (aItem.IsRepeating())
    {
        ConvertRepeatToAgnL(aItem, *entry);
    }

    CleanupStack::Pop(entry);
    return entry;
}

// -----------------------------------------------------------------------------
// CPIMAgnApptAdapter::ReadAgnToItemL
// Reads an Agenda Model Appointment item and converts it to framework PIM item.
// -----------------------------------------------------------------------------
//
void CPIMAgnApptAdapter::ReadAgnToItemL(MPIMEventItem& aItem, CCalEntry& aEntry)
{
    JELOG2(EPim);
    ReadFieldsFromAgnL(aItem, aEntry, KPIMSupportedApptFields,
                       KPIMSupportedApptFieldsCount);

    TCalRRule tempRepeat;
    if (aEntry.GetRRuleL(tempRepeat))
    {
        ReadRepeatFromAgnL(aItem, aEntry);
        aItem.GetRepeat()->BackupRepeatL();
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnApptAdapter::UpdateItemToAgnL
// Reads an Agenda Model Appt item and updates it based on the PIM item data.
// -----------------------------------------------------------------------------
//
void CPIMAgnApptAdapter::UpdateItemToAgnL(const MPIMEventItem& aItem, // PIM Item Data
        CCalEntry& aEntry) // Agenda Model entry
{
    JELOG2(EPim);
    // Before updating values, empty the fields or set default values for the
    // fields that PIM api supports
    ClearPIMFieldsL(aEntry, KPIMSupportedApptFields,
                    KPIMSupportedApptFieldsCount);

    // Convert fields from Java side to the native side
    ConvertFieldsToAgnL(aItem, aEntry);
    // If there is a defined repeat rule, convert it also
    if (aItem.IsRepeating())
    {
        if (aItem.GetRepeat()->IsModified())
        {
            ConvertRepeatToAgnL(aItem, aEntry);
        }
    }
    else
    {
        // Clear the repeat, as it might have been removed.
        aEntry.ClearRepeatingPropertiesL();
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnApptAdapter::CPIMAgnApptAdapter
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMAgnApptAdapter::CPIMAgnApptAdapter(java::util::FunctionServer* aFuncServer) :
        CPIMAgnEventAdapter(aFuncServer)
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMAgnApptAdapter::ReadStartFromAgnL
// Reads Agenda entry's start field and converts it into PIM Item field.
// -----------------------------------------------------------------------------
//
void CPIMAgnApptAdapter::ReadStartFromAgnL(MPIMEventItem& aItem,
        CCalEntry& aEntry)
{
    JELOG2(EPim);
    TTime agnStartTime = aEntry.StartTimeL().TimeUtcL();
    if (agnStartTime != Time::NullTTime())
    {
        TPIMFieldData fieldData(EPIMEventStart, KPIMAttrNone, agnStartTime);
        aItem.ItemData().AddValueL(fieldData);
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnApptAdapter::ReadEndFromAgnL
// Reads Agenda entry's end field and converts it into PIM Item field.
// -----------------------------------------------------------------------------
//
void CPIMAgnApptAdapter::ReadEndFromAgnL(MPIMEventItem& aItem,
        CCalEntry& aEntry)
{
    JELOG2(EPim);
    TTime agnEndTime = aEntry.EndTimeL().TimeUtcL();
    if (agnEndTime != Time::NullTTime())
    {
        TPIMFieldData fieldData(EPIMEventEnd, KPIMAttrNone, agnEndTime);
        aItem.ItemData().AddValueL(fieldData);
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnApptAdapter::AddDefaultValuesToEntryL
// -----------------------------------------------------------------------------
//
void CPIMAgnApptAdapter::AddDefaultValuesToEntryL(const MPIMEventItem& aItem,
        CCalEntry& aEntry) const
{
    JELOG2(EPim);
    // Add common event default values to the native entry
    CPIMAgnEventAdapter::AddDefaultValuesToEntryL(aItem, aEntry);
    const MPIMItemData& data = aItem.ItemData();

    // Set default start and end time if these are not present in the Java item
    if (data.CountValues(EPIMEventStart) == 0 && data.CountValues(EPIMEventEnd)
            == 0)
    {
        TCalTime defaultTime;
        TTime currentTime;
        // Get current time in universal time
        currentTime.UniversalTime();
        defaultTime.SetTimeUtcL(currentTime);
        aEntry.SetStartAndEndTimeL(defaultTime, defaultTime);
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnApptAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMAgnApptAdapter::ConstructL()
{
    JELOG2(EPim);
    CPIMAgnEventAdapter::ConstructL();
}

// End of file
