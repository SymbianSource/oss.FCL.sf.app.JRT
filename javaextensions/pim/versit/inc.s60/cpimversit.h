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
* Description:  Converts PIM Items to Versit objects and vice versa.
 *
*/


#ifndef CPIMVERSIT_H
#define CPIMVERSIT_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h> // CDesCArray
#include <vutil.h> // Versit
#include "pimcommon.h" // TPIMListType
#include "functionserver.h"

// FORWARD DECLARATIONS
class CPIMManager;
class CPIMContactValidator;
class CPIMEventValidator;
class CPIMToDoValidator;
class CPIMCardConverter;
class CPIMCalendarConverter;
class CPIMItem;

// CLASS DECLARATION

/**
 *  Converts PIM Items to Versit objects and vice versa.
 */
NONSHARABLE_CLASS(CPIMVersit): public CBase
{

public: // Constructors and destructor

    /**
     * Two-phased constructor.
     * @param aContactValidator Validator to validate created contacts
     * @param aEventValidator Validator to validate created events
     * @param aToDoValidator Validator to validate created todos
     */

    static CPIMVersit* NewL(CPIMManager* aManager);

    /**
     * Destructor.
     */
    virtual ~CPIMVersit();

public: // new methods

    /**
     * Converts a PIM Item to a Versit object.
     *
     * @param aItem Item to convert
     * @return HBufC pointer representing the Versit object.
     *         The ownership of the object is transferred to the caller.
     * @par Leaving:
     * This method may leave on error
     */
    HBufC8* ItemToStringL(const CPIMItem& aItem,
                          Versit::TVersitCharSet aCharset);

    /**
     * Converts a Versit object to PIM Items.
     *
     * @param aString Versit object in a descriptor
     *                In case of a vCard, contains one vCard.
     *                In case of a vCalendar, may contain several
     *                vTodos or vEvents
     * @return Array containing the items
     *         The ownership of the object is transferred to the caller.
     * @par Leaving:
     * @li \c KErrNotSupported - \a aString is not a vCard or vCalendar
     * @li \c KErrCorrupt - \a aString is corrupted
     * @li \c KErrEof - vCalendar does not have terminating delimiter
     * @li \c KErrTooBig - vCard has a name or address array with too
     *                       many elements
     */
    RPointerArray<CPIMItem>*
    StringToItemL(const TDesC8& aString,
                  Versit::TVersitCharSet aCharset);

    /**
     * Returns an array of supported formats for a specific pim list type.
     *
     * @param aPimListType Type of list for which the formats are queried.
     *
     * @return Array of descriptors, each descriptor containing one format
     */
    const CDesCArray& SupportedSerialFormatsL
    (TPIMListType aPimListType) const;

private:

    /**
     * C++ constructor.
     */
    CPIMVersit(CPIMManager* aManager);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Converts a vCard to contacts.
     *
     * @param aString vCard
     * @param aItemArray contacts are stored here
     */
    void StringToContactL(const TDesC8& aString,
                          RPointerArray<CPIMItem>& aItemArray,
                          Versit::TVersitCharSet aCharset);

    /**
     * Combines 3 8-bit descriptors into one.
     *
     * @param aDesC1 First descriptor
     * @param aDesC2 Second descriptor
     * @param aDesC3 Third descriptor
     *
     * @return descriptors combined into one HBufC8
     */
    HBufC8* Combine3DesC8L(const TDesC8& aDesC1,
                           const TDesC8& aDesC2,
                           const TDesC8& aDesC3);

    /**
     * Combines 3 16-bit descriptors into one.
     * The returned value is pushed into the cleanup stack
     *
     * @param aDesC1 First descriptor
     * @param aDesC2 Second descriptor
     * @param aDesC3 Third descriptor
     *
     * @return descriptors combined into one HBufC, which is pushed into
     * cleanup stack.
     */
    HBufC* Combine3DesCLC(const TDesC& aDesC1,
                          const TDesC& aDesC2,
                          const TDesC& aDesC3);

private: // Data
    // Converts Contact Item <-> vCard
    CPIMCardConverter* iCardConverter;

    // Converts PIM Item <--> vCalendar
    CPIMCalendarConverter* iCalendarConverter;

    // Validates contacts
    const CPIMContactValidator& iContactValidator;

    // Validates events
    const CPIMEventValidator& iEventValidator;

    // Validates ToDos
    const CPIMToDoValidator& iToDoValidator;

    // Supported vCard formats
    CDesC16ArrayFlat* iVCardFormats;

    // Supported vCalendar formats
    CDesC16ArrayFlat* iVCalendarFormats;

    java::util::FunctionServer* iFuncServer;

};

#endif // CPIMVERSIT_H
// End of File
