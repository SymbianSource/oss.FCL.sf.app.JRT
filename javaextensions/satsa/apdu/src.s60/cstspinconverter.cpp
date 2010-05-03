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
#include "cstspinconverter.h"
#include <charconv.h>
#include <f32file.h>

namespace java
{
namespace satsa
{

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSPinConverter::ConvertToUTF8L
//
// -----------------------------------------------------------------------------
void CSTSPinConverter::ConvertToUTF8L(const TDesC& aPinValue,
                                      TPtr8& aConvertedPIN, TBool aUpperCase)
{

    // RFs for CCnvCharacterSetConverter
    RFs fsSession;
    User::LeaveIfError(fsSession.Connect());
    CleanupClosePushL(fsSession);

    CCnvCharacterSetConverter* converter = CCnvCharacterSetConverter::NewLC();

    // transform pin to UTF8
    if (converter->PrepareToConvertToOrFromL(KCharacterSetIdentifierUtf8,
            fsSession) != CCnvCharacterSetConverter::EAvailable)
    {
        User::Leave(KErrNotSupported);
    }

    TInt error = converter->ConvertFromUnicode(aConvertedPIN, aPinValue);

    // check to see that the descriptor isn’t corrupt - leave if it is
    if (error == CCnvCharacterSetConverter::EErrorIllFormedInput)
    {
        User::Leave(KErrCorrupt);
    }
    if (error < 0)
    {
        User::Leave(KErrGeneral);
    }

    //convert to uppercase
    if (aUpperCase)
    {
        aConvertedPIN.UpperCase();
    }

    CleanupStack::PopAndDestroy(converter);
    CleanupStack::PopAndDestroy(&fsSession);

}

// -----------------------------------------------------------------------------
// CSTSPinConverter::ConvertToBCDL
// Converts gived value to BCD (Binary Coded Desimal) format. In normal case,
// sets each upper nibble to 0. If halfBCD is used, sets each upper nibble to F.
// Returns: aConvertedPIN: Puts converted data into this pointer
// -----------------------------------------------------------------------------
void CSTSPinConverter::ConvertToBCDL(const TDesC& aPinValue,
                                     TPtr8& aConvertedPIN, TBool aHalfBCD)
{

    //verify that each character is a digit
    if (!IsAllDigits(aPinValue))
    {
        User::Leave(KErrCorrupt);
    }
    //encode the characters as BCD (see Section 2) digits: x = BCD(PIN)
    //BCD=Binary Coded Decimal
    TUint8 mask = 0x00;
    if (aHalfBCD)
    {
        //upper nibble
        mask = 0xF0;//IIII0000
    }

    TInt length = aPinValue.Length();
    for (TInt i = 0; i < length; i++)
    {
        TUint8 currentNumber = (TUint8) aPinValue[i];

        //ignore first nibble
        TUint8 firstNibleMask = 0x0F;
        TUint8 firstNibleIgnored = (TUint8)(currentNumber & firstNibleMask);

        TUint8 result = (TUint8)(firstNibleIgnored | mask);

        aConvertedPIN.Append(result);
    }
}

// -----------------------------------------------------------------------------
// CSTSPinConverter::ConvertToASCIIL
// Just basic conversion from TDesC to TDes8 is needed
// -----------------------------------------------------------------------------
void CSTSPinConverter::ConvertToASCIIL(const TDesC& aPinValue,
                                       TPtr8& aConvertedPIN)
{

    // verify that each character is a digit in the current code-page
    if (!IsAllDigits(aPinValue))
    {
        User::Leave(KErrCorrupt);
    }
    // if needed encode the characters as ASCII (ANSI X3.4:1968)
    aConvertedPIN.Append(aPinValue);
}

// -----------------------------------------------------------------------------
// CSTSPinConverter::IsAllDigits
// Checks, are all values in gived descriptor digits or not.
// Returns: ETrue: All was digits
//          EFalse: At least one value was not digit
// -----------------------------------------------------------------------------
TBool CSTSPinConverter::IsAllDigits(const TDesC& aPinValue)
{

    //if empty
    if (aPinValue.Length() == 0)
    {
        return EFalse;
    }
    TLex lex;
    lex.Assign(aPinValue);
    while (lex.Peek() != 0)
    {
        if (!(lex.Get()).IsDigit())
        {
            //if was not digit
            return EFalse;
        }
    }
    return ETrue;
}

// -----------------------------------------------------------------------------
// CSTSPinConverter::DoBadding
// Reallocs gived buffer if needed. aConvertedPIN MUST NOT be in cleanupStack.
// Pads only if PIN value length is less than gived length.
// -----------------------------------------------------------------------------
TInt CSTSPinConverter::DoBadding(const TDesC& aPinValue,
                                 HBufC8*& aConvertedPIN, TInt aLength, TUint8 aPadChar)
{
    TInt currentLength = aPinValue.Length();
    // do padding
    if (currentLength < aLength)
    {
        // check that buffer is large enough
        if (aConvertedPIN->Des().MaxLength() < aLength)
        {
            HBufC8* tmp = aConvertedPIN->ReAlloc(aLength);
            if (!(tmp))
            {
                return KErrNoMemory;
            }
            aConvertedPIN = tmp;
        }
        // add padding characters up to stored length
        aConvertedPIN->Des().AppendFill(aPadChar, aLength - currentLength);
    }
    return KErrNone;
}

} // namespace satsa
} // namespace java
//  End of File
