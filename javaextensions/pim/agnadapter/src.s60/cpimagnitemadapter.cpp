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
* Description:  Base class for the Agenda Model event and todo item adapters
 *
*/


// CLASS HEADER
#include "cpimagnitemadapter.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <caltime.h>
#include <CalenInterimUtils2.h>
#include <tzconverter.h>
#include <calentry.h>

// ============================ MEMBER FUNCTIONS ===============================

// destructor
CPIMAgnItemAdapter::~CPIMAgnItemAdapter()
{
    JELOG2(EPim);
    CallMethod(this, &CPIMAgnItemAdapter::DoClose, iFuncServer);
}

void CPIMAgnItemAdapter::DoClose()
{
    JELOG2(EPim);
    delete iCalenInterimUtils;
    iCalenInterimUtils = NULL;
    iTzServer.Close();
}

// -----------------------------------------------------------------------------
// CPIMAgnItemAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMAgnItemAdapter::ConstructL()
{
    JELOG2(EPim);
    User::LeaveIfError(iTzServer.Connect());
    iCalenInterimUtils = CCalenInterimUtils2::NewL();
}

// -----------------------------------------------------------------------------
// CPIMAgnItemAdapter::IsDateInValidAgendaRange
// Checks the date is acceptable within the agenda model's valid date range
// (from 1st January 1980 to 31st December 2100).
// -----------------------------------------------------------------------------
//
TBool CPIMAgnItemAdapter::IsDateInValidAgendaRange(TPIMDate aDate) const
{
    JELOG2(EPim);
    if (aDate < TCalTime::MinTime() || aDate > TCalTime::MaxTime())
    {
        return EFalse;
    }
    return ETrue;
}

// -----------------------------------------------------------------------------
// CPIMAgnItemAdapter::StartOfDay
// -----------------------------------------------------------------------------
//
TPIMDate CPIMAgnItemAdapter::StartOfDay(const TPIMDate& aTime) const
{
    JELOG2(EPim);
    TTime zero(0);
    return (zero + aTime.DaysFrom(zero));
}

// -----------------------------------------------------------------------------
// CPIMAgnItemAdapter::ConvertTimeL
// -----------------------------------------------------------------------------
//
void CPIMAgnItemAdapter::ConvertTimeL(TPIMDate& aTime,
                                      const TPIMDateFormat aRequestedFormat)
{
    JELOG2(EPim);
    TTime temp(aTime);
    // Create converter here because some other client  might have changed the
    // time zone during the run time of PIM API. We do not listen time changes
    CTzConverter* converter = CTzConverter::NewL(iTzServer);
    CleanupStack::PushL(converter);

    // Convert time to requested format
    if (aRequestedFormat == EPIMDateUTC)
    {
        User::LeaveIfError(converter->ConvertToUniversalTime(temp));
    }
    else if (aRequestedFormat == EPIMDateLocal)
    {
        User::LeaveIfError(converter->ConvertToLocalTime(temp));
    }
    // Do not modify if some other time format was requested...
    CleanupStack::PopAndDestroy(converter);
    aTime = temp;
}

// -----------------------------------------------------------------------------
// CPIMAgnItemAdapter::CreateCalendarEntryLC
// -----------------------------------------------------------------------------
//
CCalEntry* CPIMAgnItemAdapter::CreateCalendarEntryLC(const TInt aType)
{
    JELOG2(EPim);
    // Create global unique identifier
    HBufC8* guid = iCalenInterimUtils->GlobalUidL();
    CleanupStack::PushL(guid);

    // Create a native calendar entry. aType specifies the type
    // of the entry (meeting, memo, anniversary or todo)
    CCalEntry* entry = CCalEntry::NewL(static_cast<CCalEntry::TType>(aType),
                                       guid, CCalEntry::EMethodNone, 0); // sequence number

    CleanupStack::Pop(guid);
    CleanupStack::PushL(entry);
    return entry;
}

CPIMAgnItemAdapter::CPIMAgnItemAdapter(java::util::FunctionServer* aFuncServer) :
        iFuncServer(aFuncServer)
{
    JELOG2(EPim);
}

// End of file
