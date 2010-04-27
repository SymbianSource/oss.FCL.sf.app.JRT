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
* Description:  Converts vCalendar <-> PIM API
 *
*/


#ifndef CPIMCALENDARCONVERTER_H
#define CPIMCALENDARCONVERTER_H

//  INCLUDES
#include <e32base.h> // CBase
#include <vutil.h> // Versit
#include "pimtodo.h" // TPIMToDoField
#include "pimevent.h" // TPIMEventField
#include "pimcommon.h" // TPIMAttribute
#include "functionserver.h"

// FORWARD DECLARATIONS
class CPIMItem;
class RWriteStream;
class RReadStream;
class CParserVCalEntity;
class CParserProperty;
class CPIMEventValidator;
class CPIMToDoValidator;
class CPIMToDoPropertyConverter;
class CPIMEventPropertyConverter;

// CLASS DECLARATION

/**
 *  Converts PIM ToDo/Event Items <-> vCalendars.
 *  vCalendars are stored in streams.
 */
NONSHARABLE_CLASS(CPIMCalendarConverter): public CBase
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMCalendarConverter* NewL(
        const CPIMEventValidator& aEventValidator,
        const CPIMToDoValidator& aToDoValidator,
        java::util::FunctionServer* aFuncServer);

    /**
     * Destructor.
     */
    virtual ~CPIMCalendarConverter();

public: // New functions

    /**
     * Converts Event/ToDo Items to vCalendars.
     * The converted vCalendars are stored in a stream.
     *
     * @param aStream RWriteStream to write the vCalendar to.
     * @param aItem CPIMItem which is to be converted.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aItem is not event or todo
     */
    void ItemToStreamL(RWriteStream& aStream,
                       const CPIMItem& aItem,
                       const Versit::TVersitCharSet aCharset);

    /**
     * Converts vCalendars to Event/ToDo items.
     *
     * @param aStream RReadStream to be converted.
     * @param aItemArray array where created items are stored.
     */
    void StreamToItemL(RReadStream& aStream,
                       RPointerArray<CPIMItem>& aItemArray,
                       const Versit::TVersitCharSet aCharset);

private:

    /**
     * C++ default constructor.
     */
    CPIMCalendarConverter(const CPIMEventValidator& aEventValidator,
                          const CPIMToDoValidator& aToDoValidator,
                          java::util::FunctionServer* aFuncServer);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Converts vEvents to Event items.
     *
     * @param aParser vEvent to convert
     * @param aItemArray array where the created item is stored.
     */
    void ParserToEventL(CParserVCalEntity& aParser,
                        RPointerArray<CPIMItem>& aItemArray);

    /**
     * Converts vToDos to ToDo items.
     *
     * @param aParser vTodo to convert
     * @param aItemArray array where the created item is stored.
     */
    void ParserToToDoL(CParserVCalEntity& aParser,
                       RPointerArray<CPIMItem>& aItemArray);

private: // Data

    // Validates events
    const CPIMEventValidator& iEventValidator;

    // Validates ToDos
    const CPIMToDoValidator& iToDoValidator;

    // Converts ToDo properties
    CPIMToDoPropertyConverter* iToDoConverter;

    // Converts Event properties
    CPIMEventPropertyConverter* iEventConverter;

    java::util::FunctionServer* iFuncServer;

};

#endif // CPIMCALENDARCONVERTER_H
// End of File
