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


#ifndef CPIMAGNITEMADAPTER_H
#define CPIMAGNITEMADAPTER_H

// INCLUDES
#include "pimcommon.h"
#include <tz.h>
#include "functionserver.h"
#include "fs_methodcall.h"

// TYPE DEFINITIONS
enum TPIMDateFormat
{
    EPIMDateUTC, EPIMDateLocal
};

// FORWARD DECLARATIONS
class CCalenInterimUtils2;
class CCalEntry;

// CLASS DECLARATION
/**
 * Agenda Item Adapter
 * Base class for Todo item adapter and Event item adapters.
 */
NONSHARABLE_CLASS(CPIMAgnItemAdapter): public CBase
{
public:

    /**
     * Destructor
     */
    virtual ~CPIMAgnItemAdapter();

protected: // Non-public operations

    // Allow derivation with protected default constructor.
    CPIMAgnItemAdapter(java::util::FunctionServer* aFuncServer);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Checks that a requested date is in valid agenda ragne
     *
     * @param aDate Date to check
     * @return True if valid, false if not
     */
    TBool IsDateInValidAgendaRange(TPIMDate aDate) const;

    /**
     * StartOfDay
     * Converts time to start of the passed time
     * @param aTime Time which will be converted to start of date
     * @return Start of the requested date
     */
    TPIMDate StartOfDay(const TPIMDate& aTime) const;

    /**
     * ConvertTimeL
     * Converts Time to specified time format. Conversion can be made
     * from UTC to local or from local to UTC.
     * @param aTime Time which will be converted. On return this contains
     *        the converted value
     * @param aRequestedFormat The format to which the time needs to
     *        be converted
     */
    void ConvertTimeL(
        TPIMDate& aTime,
        const TPIMDateFormat aRequestedFormat);

    /**
     * Creates a new native calendar entry. The created item is left to
     * the cleanup stack and the caller takes ownership of the object
     *
     * @param aType Type of the native calendar entry
     * @return A new native calendar entry
     */
    CCalEntry* CreateCalendarEntryLC(const TInt aType);

private: // Prohibited operations

    // Prohibit copy constructor.
    CPIMAgnItemAdapter(const CPIMAgnItemAdapter&)
    {}

    // Prohibit assigment operator.
    CPIMAgnItemAdapter& operator=(const CPIMAgnItemAdapter&);

    void DoClose();

private: // Data

    // Used to generate guids. Owned
    CCalenInterimUtils2* iCalenInterimUtils;
    // Time zone API is used in time conversions. Owned
    RTz iTzServer;

protected:

    java::util::FunctionServer* iFuncServer;

};

#endif // CPIMAGNITEMADAPTER_H
// End of file
