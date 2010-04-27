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
* Description:  Converts PIM Event items and native calendar day notes
 *
*/


// CLASS HEADER
#include "cpimagnmemoadapter.h"

// INTERNAL INCLUDES
#include "pimpanics.h"
#include "agneventsupport.h"
#include "mpimeventitem.h"
#include "mpimitemdata.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <calentry.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMAgnMemoAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMAgnMemoAdapter* CPIMAgnMemoAdapter::NewL(
    java::util::FunctionServer* aFuncServer)
{
    JELOG2(EPim);
    CPIMAgnMemoAdapter* self = new(ELeave) CPIMAgnMemoAdapter(aFuncServer);
    CleanupStack::PushL(self);
    CallMethodL(self, &CPIMAgnMemoAdapter::ConstructL, self->iFuncServer);
    CleanupStack::Pop(self);
    return self;
}

// destructor
CPIMAgnMemoAdapter::~CPIMAgnMemoAdapter()
{
}

// -----------------------------------------------------------------------------
// CPIMAgnMemoAdapter::CreateItemToAgnL
// Creates a new Agenda Model Event item from a PIM item data.
// Returns: A Event item based on PIM item data.
//          The ownership of the Event item is transferred to the caller.
// -----------------------------------------------------------------------------
//
CCalEntry* CPIMAgnMemoAdapter::CreateItemToAgnL(const MPIMEventItem& aItem)
{
    JELOG2(EPim);
    // Create new calendar entry
    CCalEntry* entry = CreateCalendarEntryLC(CCalEntry::EEvent);
    // Add default values to the item. The default values are used
    // by the native calendar application and therefore needed to be added
    AddDefaultValuesToEntryL(aItem, *entry);
    // Convert fields from Java side to the native side
    ConvertFieldsToAgnL(aItem, *entry);
    CleanupStack::Pop(entry);
    return entry;
}

// -----------------------------------------------------------------------------
// CPIMAgnMemoAdapter::ReadAgnToItemL
// Reads an Agenda Model Event item and converts it to a framework PIM item.
// -----------------------------------------------------------------------------
//
void CPIMAgnMemoAdapter::ReadAgnToItemL(MPIMEventItem& aItem, CCalEntry& aEntry)
{
    JELOG2(EPim);
    ReadFieldsFromAgnL(aItem, aEntry, KPIMSupportedMemoFields,
                       KPIMSupportedMemoFieldsCount);
}

// -----------------------------------------------------------------------------
// CPIMAgnMemoAdapter::UpdateItemToAgnL
// Reads an Agenda Model Event item and updates it based on the PIM item data.
// -----------------------------------------------------------------------------
//
void CPIMAgnMemoAdapter::UpdateItemToAgnL(const MPIMEventItem& aItem,
        CCalEntry& aEntry)
{
    JELOG2(EPim);
    // Before updating values, empty the fields or set default values for the
    // fields that PIM api supports
    ClearPIMFieldsL(aEntry, KPIMSupportedMemoFields,
                    KPIMSupportedMemoFieldsCount);

    // Convert fields from Java side to the native side
    ConvertFieldsToAgnL(aItem, aEntry);
}

// -----------------------------------------------------------------------------
// CPIMAgnMemoAdapter::CPIMAgnMemoAdapter
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMAgnMemoAdapter::CPIMAgnMemoAdapter(java::util::FunctionServer* aFuncServer) :
        CPIMAgnEventAdapter(aFuncServer)
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMAgnMemoAdapter::ReadStartFromAgnL
// Reads Agenda entry's start field and converts it into PIM Item field.
// -----------------------------------------------------------------------------
//
void CPIMAgnMemoAdapter::ReadStartFromAgnL(MPIMEventItem& aItem,
        CCalEntry& aEntry)
{
    JELOG2(EPim);
    TTime agnStartTime(aEntry.StartTimeL().TimeUtcL());

    if (agnStartTime != Time::NullTTime())
    {
        // TTime start date, as a TTime but without a time component.
        TPIMFieldData fieldData(EPIMEventStart, KPIMAttrNone, agnStartTime);
        aItem.ItemData().AddValueL(fieldData);
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnMemoAdapter::ReadEndFromAgnL
// Reads Agenda entry's end field and converts it into PIM Item field.
// -----------------------------------------------------------------------------
//
void CPIMAgnMemoAdapter::ReadEndFromAgnL(MPIMEventItem& aItem,
        CCalEntry& aEntry)
{
    JELOG2(EPim);
    TTime agnEndDate(aEntry.EndTimeL().TimeUtcL());

    if (agnEndDate != Time::NullTTime())
    {
        // TTime end date, as a TTime but without a time component.
        TPIMFieldData fieldData(EPIMEventEnd, KPIMAttrNone, agnEndDate);
        aItem.ItemData().AddValueL(fieldData);
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnMemoAdapter::AddDefaultValuesToEntryL
// -----------------------------------------------------------------------------
//
void CPIMAgnMemoAdapter::AddDefaultValuesToEntryL(const MPIMEventItem& aItem,
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
void CPIMAgnMemoAdapter::ConstructL()
{
    JELOG2(EPim);
    CPIMAgnEventAdapter::ConstructL();
}

// End of file
