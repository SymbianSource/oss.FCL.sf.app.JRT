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


// INCLUDE FILES
#include "cstsuri.h"
#include "stsapduconstants.h"
#include "logger.h"

namespace java
{
namespace satsa
{

// CONSTANTS
const TInt KSTSAIDMaxLength = 16;
const TInt KSTSAIDMinLength = 5;

const TInt KSTSMinURILength = 15;//(SAT uri without slot number)
const TInt KSTSMaxURILength = 61;//(AID uri with slot and maximum AID)

const TUint8 KSTSErrNotAHexNumber = 255;

const TInt KSTSApduUriPrefixLength = 5; // "apdu:" takes 5 characters

const TUint8 KSTSApduUriSlotTargetSeparator = ';';
const TUint8 KSTSApduUriAIDSeparator = '.';
const TUint8 KSTSSmallA = 'a';
const TUint8 KSTSBigA = 'A';
const TInt KSTSValueOfHexA = 0x0A;
const TInt KSTSMaxHexValue = 0x0F;
const TInt KSTSLengthOfSingleHexDigitInBits = 4;

_LIT(KSTSUriTargetPrefix, "target=");
_LIT(KSTSUriTargetSAT, "SAT");

// ============================ MEMBER FUNCTIONS ===============================


CSTSURI::CSTSURI()
{
}

void CSTSURI::ConstructL(const TDesC& aURIString)
{
    LOG(ESATSA, EInfo, "CSTSURI::ConstructL++");
    // does URI conform to the BNF syntax
    if (!CheckURIL(aURIString))
    {
        ELOG(ESATSA, "CSTSURI::ConstructL: URI does not conform to BNF sntax");
        // IllegalArgumentException
        User::Leave(KSTSErrIllegalArgument + KSTSErrIAInvalidParam);
    }
}

CSTSURI* CSTSURI::NewLC(const TDesC& aURIString)
{
    LOG(ESATSA, EInfo, "CSTSURI::NewLC++");
    CSTSURI* self = new(ELeave) CSTSURI();
    CleanupStack::PushL(self);
    self->ConstructL(aURIString);
    LOG(ESATSA, EInfo, "CSTSURI::NewLC---");
    return self;
}

CSTSURI::~CSTSURI()
{
    delete iAID;
}

// -----------------------------------------------------------------------------
// CSTSURI::AID
// Getter for AID value
// -----------------------------------------------------------------------------
const TDesC8& CSTSURI::AID() const
{
    return *iAID;
}

// -----------------------------------------------------------------------------
// CSTSURI::Slot
// Getter for Slot
// -----------------------------------------------------------------------------
TUint8 CSTSURI::Slot() const
{
    return iSlot;
}

// -----------------------------------------------------------------------------
// CSTSURI::Type
// Getter for URI type
// -----------------------------------------------------------------------------
CSTSURI::TURIType CSTSURI::Type() const
{
    return iURIType;
}

// -----------------------------------------------------------------------------
// CSTSURI::CheckURIL
// Checks uri and saves needed information to member variables
//
/*
 URI can be for example: "apdu:0;target=A0.0.0.67.4.7.1F.3.2C.3"
 or                     "apdu:0;target=SAT"

 <APDU_connection_string>::= "apdu:"<targetAddress>
 <targetAddress>            ::= [slot];target
 <slot>                 ::= smart card slot number. (optional. Hexadecimal
 number identifying the smart card slot. Default
 slot assumed if left empty)
 <target>               ::= "target="<AID>|"SAT"
 <AID>                  ::= < 5 - 16 bytes >
 An AID (Application Identifier) uniquely
 identifies a smart card application. It is
 represented by 5 to 16 hexadecimal bytes where
 each byte value is seperated by a ".".
 */
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSTSURI::CheckURIL(const TDesC& aURI)
{
    LOG(ESATSA, EInfo, "CSTSURI::CheckURIL");
    const TInt uriLength = aURI.Length();
    LOG1(ESATSA, EInfo, "CSTSURI::CheckURIL:The lenghth of URI is %d", uriLength);
    // check length
    if ((uriLength > KSTSMaxURILength) || (uriLength < KSTSMinURILength))
    {
        ELOG(ESATSA, "CSTSURI::CheckURIL:wrong length of the URI");
        return EFalse;
    }
    // uri is now between 15 and 61 characters
    // Connector has already parsed the scheme part of the uri,
    // so we don't need to

    // "apdu:" takes 5 first characters
    TInt pos = KSTSApduUriPrefixLength;

    // parse slot number
    TChar slotChar = aURI[pos++];
    TUint8 slotInt = 0;
    //if slot number is gived
    if (slotChar != KSTSApduUriSlotTargetSeparator)
    {
        //create ptr which starts from slot number position
        TPtrC16 slotAndRest(aURI.Ptr() + KSTSApduUriPrefixLength, uriLength
                            - KSTSApduUriPrefixLength);
        TLex slotLex(slotAndRest);
        TInt slotNumber;
        //parse slot number
        TInt err = slotLex.Val(slotNumber);
        if (err != KErrNone)
        {
            return EFalse;
        }

        slotInt = static_cast<TUint8>(slotNumber);

        //calculate amount of slot number digits
        HBufC8* slotBuf = HBufC8::NewL(KSTSMaxURILength);
        slotBuf->Des().AppendNum(slotNumber);
        TInt slotNumberLength = slotBuf->Des().Length();
        delete slotBuf;

        //if digits of slotnumber are more that one
        slotChar = aURI[pos + (slotNumberLength - 1)];
        pos += slotNumberLength;
    }

    if (slotChar != KSTSApduUriSlotTargetSeparator)
    {
        return EFalse;
    }

    // next should be "target="
    if (aURI.Find(KSTSUriTargetPrefix()) != pos)
    {
        return EFalse;
    }
    pos += KSTSUriTargetPrefix().Length();

    TURIType uriType = EAID;
    TBufC8<KSTSAIDMaxLength> aid;

    if (aURI.Find(KSTSUriTargetSAT()) == pos)
    {
        if (uriLength != (pos + KSTSUriTargetSAT().Length()))
        {
            return EFalse;
        }
        uriType = ESAT;
    }
    else
    {
        TPtr8 aidDes = aid.Des();
        if (!ParseAID(aURI, pos, aidDes))
        {
            return EFalse;
        }
    }

    iSlot = slotInt;
    iURIType = uriType;

    //copies data to the member buffer
    iAID = aid.AllocL();
    LOG(ESATSA, EInfo, "CSTSURI::CheckURIL---");
    return ETrue;
}

// -----------------------------------------------------------------------------
// CSTSURI::GetHexValue
// returns an integer value of given hex character
// -----------------------------------------------------------------------------
//
TUint8 CSTSURI::GetHexValue(const TChar& aDigit)
{
    // if aDigit is 0-9, we can use TChar::GetNumericValue directly
    if (aDigit.IsDigit())
    {
        return static_cast<TUint8>(aDigit.GetNumericValue());
    }

    // if aDigit is not a hexadecimal digit, it is an error
    if (!(aDigit.IsHexDigit()))
    {
        return KSTSErrNotAHexNumber;
    }

    // otherwise, aDigit is a-f or A-F
    TChar charA = KSTSSmallA;
    if (aDigit.IsUpper())
    {
        charA = KSTSBigA;
    }

    // we can get the value by calculating the difference of the character
    // and character 'a', and adding the value of hex digit 'a'.
    TUint8 retVal = static_cast<TUint8>((aDigit - charA) + KSTSValueOfHexA);
    return retVal;
}

// -----------------------------------------------------------------------------
// CSTSURI::ParseAID
// parses AID from URI
// -----------------------------------------------------------------------------
//
TBool CSTSURI::ParseAID(const TDesC& aURI, TInt aPos, TDes8& aAID)
{
    TInt aidLength = aAID.Length();
    TInt aidMaxLength = aAID.MaxLength();
    TInt uriLength = aURI.Length();
    TInt pos = aPos;
    TAidParserState aidState = EFirstDigit;
    TUint8 aidDigit = 0;
    while (pos < uriLength)
    {
        TChar digit = aURI[pos++];
        ParseAIDCharacter(digit, aidState, aidDigit);
        switch (aidState)
        {
        case EFirstDigit:
        {
            if (++aidLength > aidMaxLength)
            {
                return EFalse;
            }
            aAID.Append(aidDigit);
            break;
        }
        case EERROR:
        {
            return EFalse;
        }
        default:
        {
            // other states don't require action
        }
        }
    }
    if (aidState == ESecondDigit)
    {
        // the uri ended after first digit, which is not yet added
        if (++aidLength > aidMaxLength)
        {
            return EFalse;
        }
        aAID.Append(aidDigit);
    }
    if (aidState == EFirstDigit)
    {
        // AID ended with a dot
        return EFalse;
    }

    if (aidState == EDot)
    {
        // Append also last digit
        aAID.Append(aidDigit);
    }

    if (aidLength < KSTSAIDMinLength)
    {
        return EFalse;
    }
    return ETrue;
}

// -----------------------------------------------------------------------------
// CSTSURI::ParseAIDCharacter
// Implements AID parsing state machine
// -----------------------------------------------------------------------------
void CSTSURI::ParseAIDCharacter(const TChar& aDigit, TAidParserState& aState,
                                TUint8& aAIDDigit)
{
    switch (aState)
    {
    case EFirstDigit:
    {
        aAIDDigit = GetHexValue(aDigit);
        if (aAIDDigit > KSTSMaxHexValue)
        {
            aState = EERROR;
        }
        else
        {
            aState = ESecondDigit;
        }
        break;
    }
    case ESecondDigit:
    {
        if (aDigit != KSTSApduUriAIDSeparator)
        {
            TUint8 secondDigit = GetHexValue(aDigit);
            if (secondDigit > KSTSMaxHexValue)
            {
                aState = EERROR;
            }
            else
            {
                aAIDDigit <<= KSTSLengthOfSingleHexDigitInBits;
                aAIDDigit |= secondDigit;
                aState = EDot;
            }
        }
        else
        {
            aState = EFirstDigit;
        }
        break;
    }
    case EDot:
    {
        if (aDigit != KSTSApduUriAIDSeparator)
        {
            aState = EERROR;
        }
        else
        {
            aState = EFirstDigit;
        }
        break;
    }
    default:
    {

    }
    }
}

} // namespace satsa
} // namespace java
//  End of File
