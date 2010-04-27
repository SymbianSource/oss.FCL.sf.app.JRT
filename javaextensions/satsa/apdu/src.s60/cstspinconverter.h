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


#ifndef CSTSPINCONVERTER_H
#define CSTSPINCONVERTER_H

//  INCLUDES
#include <e32std.h>

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  Static class for converting PIN values. Conversions made according to
 *  PKCS#15 standard.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSPinConverter)
{

public: // New functions

    /**
     * Convert PIN value to UTF8 format.
     * @since 3.0
     * @param aPinValue PIN value to be converted
     * @param aConvertedPIN Descriptor where converted PIN will be put
     * @param aUpperCase Is there need to convert value to uppercase
     * @return void
     */
    static void ConvertToUTF8L(const TDesC& aPinValue,
                               TPtr8& aConvertedPIN,
                               TBool aUpperCase);

    /**
     * Convert PIN value to BCD format.
     * @since 3.0
     * @param aPinValue PIN value to be converted
     * @param aConvertedPIN Descriptor where converted PIN will be put
     * @param aHalfBCD If will be converted to half BCD format
     * @return void
     */
    static void ConvertToBCDL(const TDesC& aPinValue, TPtr8& aConvertedPIN,
                              TBool aHalfBCD);

    /**
     * Convert PIN value to ASCII format.
     * @since 3.0
     * @param aPinValue PIN value to be converted
     * @param aConvertedPIN Descriptor where converted PIN will be put
     * @return void
     */
    static void ConvertToASCIIL(const TDesC& aPinValue, TPtr8& aConvertedPIN);

    /**
     * Checks is all values digits or not
     * @since 3.0
     * @param aPinValue PIN value to checked
     * @return ETrue if all was digits otherwice EFalse
     */
    static TBool IsAllDigits(const TDesC& aPinValue);

    /**
     * Padds gived pin value up to length with padding character
     * @since 3.0
     * @param aPinValue PIN value to be converted
     * @param aConvertedPIN Pointer to buffer where result is put
     * @param aLength Up to this length is padded
     * @param aPadChar Padding character
     * @return void
     */
    static TInt DoBadding(const TDesC& aPinValue, HBufC8*& aConvertedPIN,
                          TInt aLength, TUint8 aPadChar);

}
;

} //namespace satsa
} //namespace java
#endif // CSTSPINCONVERTER_H
// End of File
