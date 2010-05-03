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



#ifndef TSTSCHARACTERSETCONVERTER_H
#define TSTSCHARACTERSETCONVERTER_H

//  INCLUDES
#include <e32base.h>



// CLASS DECLARATION

/**
*  Provides character set conversion and escape character parsing.
*  Converts unicode strings which may contain escaped characters to printable
*  strings.
*
*  @since 3.2
*/
NONSHARABLE_CLASS(TSTSCharacterSetConverter)
{
public:  // Constructors and destructor

    /**
        * C++ default constructor.
        */
    TSTSCharacterSetConverter();


public: // New functions

    /**
        * Converts string from unicode to printable string. String may
        * contain escaped sequences as defined in RFC2253. Those
        * sequences are parsed.
        * If aValue contains characters which are not part of
        * PrintableString character set, the method leaves with
        * KSTSErrInvalidCharactersInCAName.
        * @since 3.2
        * @param aValue String to be converted
        * @return string containing only PrintableString characters.
        * Caller takes ownership.
        *
        */
    HBufC8* EscapedUnicodeToPrintableLC(const TDesC& aValue) const;


private:

    // checks if the given character is "special", i.e. one of:
    // ",", "+", """, "\", "<", ">" or ";"
    TBool IsSpecial(const TChar& aChar) const;

    // checks if the given character is printable, i.e. one of:
    // A-Z, a-z, 0-9, space, "'", "(", ")", "+", ",", "-",".","/",":","=","?"
    TBool IsPrintable(const TChar& aChar) const;

    // Prohibit copy constructor if not deriving from CBase.
    TSTSCharacterSetConverter(const TSTSCharacterSetConverter&);
    // Prohibit assigment operator if not deriving from CBase.
    TSTSCharacterSetConverter& operator=(const TSTSCharacterSetConverter&);

};



#endif // TSTSCHARACTERSETCONVERTER_H

// End of File
