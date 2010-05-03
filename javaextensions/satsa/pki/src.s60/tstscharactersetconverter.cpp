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
#include    "tstscharactersetconverter.h"
#include    "stspkiconstants.h"







// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TSTSCharacterSetConverter::TSTSCharacterSetConverter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TSTSCharacterSetConverter::TSTSCharacterSetConverter()
{
}


// -----------------------------------------------------------------------------
// TSTSCharacterSetConverter::EscapedUnicodeToPrintableL
// Converts string from escaped unicode to printable string
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* TSTSCharacterSetConverter::EscapedUnicodeToPrintableLC(
    const TDesC& aValue) const
{
    TInt valueLength = aValue.Length();
    // returned value can never be longer than the original (it can be shorter)
    HBufC8* value = HBufC8::NewL(valueLength);
    CleanupStack::PushL(value);
    TPtr8 valueDes = value->Des();

    TLex lexer(aValue);

    while (!lexer.Eos())
    {
        TChar currentChar = lexer.Get();
        if (currentChar == '\\')
        {
            // we are escaping
            if (lexer.Eos())
            {
                User::Leave(KSTSErrInvalidCAName);
            }
            lexer.Mark();
            currentChar = lexer.Get();
            if (!IsSpecial(currentChar))
            {
                if (lexer.Eos())
                {
                    User::Leave(KSTSErrInvalidCAName);
                }
                lexer.Get();
                TLex hexPair(lexer.MarkedToken());
                TUint hexChar;
                User::LeaveIfError(hexPair.Val(hexChar, EHex));
                currentChar = hexChar;
            }
        }
        if (IsPrintable(currentChar))
        {
            valueDes.Append(currentChar);
        }
        else
        {
            User::Leave(KSTSErrInvalidCharactersInCAName);
        }
    }

    return value;
}

// -----------------------------------------------------------------------------
// TSTSCharacterSetConverter::IsSpecial
// checks if the given character is "special"
// -----------------------------------------------------------------------------
//
TBool TSTSCharacterSetConverter::IsSpecial(
    const TChar& aValue) const
{
    switch (aValue)
    {
    case(','):
    case('+'):
    case('\"'):
    case('\\'):
    case('<'):
    case('>'):
    case(';'):
    {
        return ETrue;
    }
    default:
    {
        return EFalse;
    }
    }
}

// -----------------------------------------------------------------------------
// TSTSCharacterSetConverter::IsPrintable
// checks if the given character is allowed in PrintableString
// -----------------------------------------------------------------------------
//
TBool TSTSCharacterSetConverter::IsPrintable(
    const TChar& aValue) const
{
    if (aValue > 128)  // non-ascii characters are not allowed
    {
        return EFalse;
    }
    if (aValue.IsAlphaDigit())  // A-Z,a-z,0-9 are allowed
    {
        return ETrue;
    }
    switch (aValue)  // some other characters are also allowed
    {
    case(' '):
    case('\''):
    case('('):
    case(')'):
    case('+'):
    case(','):
    case('-'):
    case('.'):
    case('/'):
    case(':'):
    case('='):
    case('?'):
    {
        return ETrue;
    }
    default:
    {
        return EFalse;
    }
    }
}




//  End of File
