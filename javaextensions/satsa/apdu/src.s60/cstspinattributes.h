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
* Description:
 *
*/


#ifndef CSTSPINATTRIBUTES_H
#define CSTSPINATTRIBUTES_H

//  INCLUDES
#include <e32base.h>

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  PinAttributes type module
 *
 *  PinAttributes ::= SEQUENCE {
 *      pinFlags PinFlags,
 *      pinType PinType,
 *      minLength INTEGER (pkcs15-lb-minPinLength..pkcs15-ub-minPinLength),
 *      storedLength INTEGER (0..pkcs15-ub-storedPinLength),
 *      maxLength INTEGER OPTIONAL,
 *      pinReference [0] Reference DEFAULT 0,
 *      padChar OCTET STRING (SIZE(1)) OPTIONAL,
 *      lastPinChange GeneralizedTime OPTIONAL,
 *      path Path OPTIONAL,
 *      ... -- For future extensions
 *  }
 *  PinFlags ::= BIT STRING {
 *      case-sensitive (0),
 *      local (1),
 *      change-disabled (2),
 *      unblock-disabled (3),
 *      initialized (4),
 *      needs-padding (5),
 *      unblockingPin (6),
 *      soPin (7),
 *      disable-allowed (8),
 *      integrity-protected (9),
 *      confidentiality-protected (10),
 *      exchangeRefData (11)
 *  } (CONSTRAINED BY { -- ‘unblockingPin’ and ‘soPIN’ cannot both be set -- })
 *  PinType ::=
 *      ENUMERATED {bcd, ascii-numeric, utf8, ..., half-nibble-bcd, iso9564-1}
 *  Reference ::= INTEGER (0..pkcs15-ub-reference)
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSPinAttributes): public CBase
{
public: // pin types
    enum TPinType
    {
        EBCD = 0,
        EAsciiNumeric = 1,
        EUTF8 = 2,
        EHalfNibbleBCD = 3,
        EISO9564_1 = 4,
        ENotInitialized = 99
    };

public: // pin flags
    enum TFlagType
    {
        ECaseSensitive = 0x80, //1000 0000 -->First bit from left
        ENeedsPadding = 0x04, //0000 0100 -->6. bit from left
        EChangeDisabled = 0x20, //0010 0000 -->3. bit from left
        EUnblockDisabled = 0x10,//0001 0000 -->4. bit from left
        EDisableAllowed = 0xFF //special case, see constant KSTSDisableAllowed
    };

public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CSTSPinAttributes* NewL();

    static CSTSPinAttributes* NewLC(TInt aPinType,
                                    TInt aMinLength,
                                    TInt aStoredLength,
                                    TInt aMaxLength,
                                    TInt aPinReference,
                                    TInt aPadChar,
                                    TInt aFlags);

    /**
     * Destructor.
     */
    virtual ~CSTSPinAttributes();

public: // New functions

    /**
     * Getter for PIN type
     * @since 3.0
     * @return PIN type
     */
    TPinType PinType() const;

    /**
     * Getter for PIN minimum length
     * @since 3.0
     * @return PIN minimum length as Integer
     */
    TInt MinLength() const;

    /**
     * Getter for PIN stored length
     * @since 3.0
     * @return PIN stored length as Integer
     */
    TInt StoredLength() const;

    /**
     * Getter for PIN maximum length
     * @since 3.0
     * @return PIN maximum length as Integer
     */
    TInt MaxLength() const;

    /**
     * Getter for PIN reference
     * @since 3.0
     * @return PIN reference in hexadecimal format
     */
    TUint8 PinReference() const;

    /**
     * Getter for padding character
     * @since 3.0
     * @return padding character in TDesC reference
     */
    const TDesC8& PadChar() const;

    /**
     * Corresponds to assigment operator.
     * @since 3.0
     * @param aPinAttributes reference which data will be used
     */
    void CopyL(const CSTSPinAttributes& aPinAttributes);

    /**
     * Returns true if gived PIN Flag is set
     * @since 3.0
     * @param aFlagType Type of PIN Flag
     */
    TBool IsPinFlagSet(TFlagType aFlagType) const;

protected: // New functions

    /**
     * Protected construction to allow derivation
     */
    void ConstructL();

    /**
     * Protected construction to allow derivation
     */
    void ConstructL(TInt aPadChar,
                    TInt aFlags);

    /**
     * C++ default constructor.
     */
    CSTSPinAttributes();

    /**
     * C++ default constructor.
     */
    CSTSPinAttributes(TInt aPinType,
                      TInt aMinLength,
                      TInt aStoredLength,
                      TInt aMaxLength,
                      TInt aPinReference);

protected: // Data
    // pinFlags BIT STRING, owned
    HBufC8* iPinFlags;

    // Pin type
    TPinType iPinType;

    TInt iMinLength;
    TInt iStoredLength;
    TInt iMaxLength;

    //reference
    TUint8 iPinReference;

    // padding character, owned
    HBufC8* iPadChar;

};

} // namespace satsa
} // namespace java
#endif // CSTSPINATTRIBUTES_H
// End of File

