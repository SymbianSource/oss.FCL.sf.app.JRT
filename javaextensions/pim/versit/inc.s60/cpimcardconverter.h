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
* Description:  Converts vCard <-> PIM API
 *
*/


#ifndef CPIMCARDCONVERTER_H
#define CPIMCARDCONVERTER_H

//  INCLUDES
#include <e32base.h> // CBase
#include <vutil.h> // Versit
#include "pimcontact.h" // TPIMContactField
#include "pimcommon.h" // TPIMAttribute
#include "functionserver.h"

// FORWARD DECLARATIONS
class CPIMItem;
class RWriteStream;
class RReadStream;
class CParserVCard;
class CPIMContactValidator;
class CPIMContactItem;
class CPIMCardPropertyConverter;

// CLASS DECLARATION

/**
 *  Converts PIM Contact Items <-> vCards.
 *  vCards are stored in streams.
 */
NONSHARABLE_CLASS(CPIMCardConverter): public CBase
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMCardConverter* NewL(
        const CPIMContactValidator& aContactValidator,
        java::util::FunctionServer* aFuncServer);

    /**
     * Destructor.
     */
    virtual ~CPIMCardConverter();

public: // New functions

    /**
     * Converts Contact Items to vCards.
     * The converted vCards are stored in a stream.
     *
     * @param aStream RWriteStream to write the vCard to.
     * @param aItem CPIMItem which is to be converted.
     */
    void ItemToStreamL(RWriteStream& aStream,
                       const CPIMItem& aItem,
                       Versit::TVersitCharSet aCharset);

    /**
     * Converts vCards to contact items.
     *
     * @param aStream RReadStream to be converted.
     * @param aItemArray Array where the created items are stored
     * @par Leaving:
     * @li \c KErrCorrupt - \a No items could be created
     */
    void StreamToItemL(RReadStream& aStream,
                       RPointerArray<CPIMItem>& aItemArray,
                       Versit::TVersitCharSet aCharset);

private:

    /**
     * Converts vCards to contact items.
     *
     * @param aParser vCard to convert
     * @param aItemArray Array where the created items are stored
     * @param aParserArray Array where contained vCards are stored
     */
    void ParserToItemL(CParserVCard& aParser,
                       RPointerArray<CPIMItem>& aItemArray,
                       CArrayPtrFlat<CParserVCard>& aParserArray);

    /**
     * C++ default constructor.
     */
    CPIMCardConverter(const CPIMContactValidator& aContactValidator,
                      java::util::FunctionServer* aFuncServer);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: // Data

    // Validates contacts
    const CPIMContactValidator& iContactValidator;

    // Converts properties
    CPIMCardPropertyConverter* iPropertyConverter;

    java::util::FunctionServer* iFuncServer;

};

#endif // CPIMCARDCONVERTER_H
// End of File
