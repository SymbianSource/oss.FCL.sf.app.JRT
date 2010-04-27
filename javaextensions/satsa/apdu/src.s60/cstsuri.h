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


#ifndef CSTSURI_H
#define CSTSURI_H

//  INCLUDES
#include <e32base.h>

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  Class for parsing BNF syntax URI.
 *
 *  URI can be for example: "apdu:0;target=A0.0.0.67.4.7.1F.3.2C.3"
 *  or                      "apdu:0;target=SAT"
 *
 *  <APDU_connection_string>::= "apdu:"<targetAddress>
 *  <targetAddress>         ::= [slot];target
 *  <slot>                  ::= smart card slot number. (optional. Hexadecimal
 *                              number identifying the smart card slot. Default
 *                              slot assumed if left empty)
 *  <target>                ::= "target="<AID>|"SAT"
 *  <AID>                   ::= < 5 - 16 bytes >
 *                              An AID (Application Identifier) uniquely
 *                              identifies a smart card application. It is
 *                              represented by 5 to 16 hexadecimal bytes where
 *                              each byte value is seperated by a ".".
 *
 */
NONSHARABLE_CLASS(CSTSURI): public CBase
{
public: // Type definition
    enum TURIType
    {
        EAID,
        ESAT
    };

private: // Type definition
    enum TAidParserState
    {
        EFirstDigit,
        ESecondDigit,
        EDot,
        EERROR
    };

public: // Constructors and destructor

    static CSTSURI* NewLC(const TDesC& aURIString);

    virtual ~CSTSURI();

public:

    /**
     * Getter for AID. Returns AID value, if URI type was AID, otherwice
     * returns KNullDesC8 reference.
     * @return AID in TDesC reference
     */
    const TDesC8& AID() const;

    /**
     * Getter for slot number
     * @return slot number
     */
    TUint8 Slot() const;

    /**
     * Getter for URI type
     * @return type of URI
     */
    TURIType Type() const;

protected: // New functions

    // Protected construction to allow derivation
    void ConstructL(const TDesC& aURIString);

    /**
     * C++ default constructor.
     */
    CSTSURI();

    /**
     * Cheks is URI string in correct form.
     */
    TBool CheckURIL(const TDesC& aURI);

    /**
     * Returns integer value of the character, or 255 if
     * the character is not a hexadecimal digit.
     * @param aDigit hexadecimal character (0-9, a-f)
     * @return integer value of the character of 255.
     */
    TUint8 GetHexValue(const TChar& aDigit);

    /**
     * parses AID from aURI. Note that this method may append
     * characters to aAID even when the parsing fails.
     * @param aURI uri which the AID is parsed from
     * @param aPos position to AID in aURI.
     * @param aAID AID is appended to this descriptor.
     * @return ETrue if AID was parsed successfully. EFalse if the
     * parsing fails, in which case the contents of aAID is undefined.
     */
    TBool ParseAID(const TDesC& aURI, TInt aPos, TDes8& aAID);

    /**
     * Implements the state machine of parsing the AID.
     * @param aDigit current character to parse
     * @param aState state of the state machine
     * @param aAIDDigit currently parsed digit
     */
    void ParseAIDCharacter(const TChar& aDigit,
                           TAidParserState& aState,
                           TUint8& aAIDDigit);

protected: // Data

    // slot number
    TUint8 iSlot;
    // AID, owned
    HBufC8* iAID;

    // Type of URI
    TURIType iURIType;

};

} // namespace satsa
} // namespace java
#endif // CSTSURI_H
// End of File
