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
* Description:  Description
 *
*/


#ifndef PIMBASE64CONVERTER_H
#define PIMBASE64CONVERTER_H

//  INCLUDES
#include "pimcommon.h"

// CLASS DECLARATION

/**
 *  Base64 coder and decoder.
 *
 *  Base64 encoding is defined in IETF RFC 2045.
 *
 *  Base64 encoding is identical to the "B" binary encoding, defined in
 *  IETF RFC 2047 (which merely refers to RFC 2045).
 */
NONSHARABLE_CLASS(PIMBase64Converter)
{
public: // New functions

    /**
     * Encodes given byte array to base64 representation.
     *
     * @param aSource Source bytes.
     *
     * @return Target bytes.
     *         \b Ownership of the return value is transferred to caller.
     */
    static CPIMByteArray* EncodeBase64L(const CPIMByteArray& aSource);

    /**
     * Decodes given byte array from base64 representation.
     *
     * @param aSource Source bytes.
     *
     * @return Target bytes.
     *         \b Ownership of the return value is transferred to caller.
     */
    static CPIMByteArray* DecodeBase64L(const CPIMByteArray& aSource);

private: // New functions

    /**
     * Base64 encodes \a aQuantum and appends resulting bytes to
     * \a aToArray.
     *
     * @param aNumSourceBytes Number of (8 bit) source bytes, 0 to 3.
     *        If the number is 3, full quantum is encoded. If the number is
     *        1 or 2, partial quantum is encoded and missing bytes are
     *        fillied with pad byte. If the number is 0, nothing is done.
     *
     * @par Leaving:
     * @li \c KErrArgument - \aNumSourceBytes is invalid. On debug builds
     *     a panic is raised instead.
     */
    static void EncodeQuantumL(
        const TUint32& aQuantum,
        CPIMByteArray& aTargetArray,
        const TInt& aNumSourceBytes);

    /**
     * Base64 decodes \a aQuantum and appends resulting bytes to
     * \a aToArray. The quantum must contain 24 bits (four 6 bit source
     * bytes).
     *
     * @par Leaving:
     * @li \c KErrArgument - \aNumSourceBytes is invalid. On debug builds
     *     a panic is raised instead.
     */
    static void DecodeQuantumL(const TUint32& aQuantum,
                               CPIMByteArray& aTargetArray, const TInt& aNumSourceBytes);

    /**
     * Maps \a aByte to a character in the base64 alphabet.
     *
     * @return \a aByte mapped to a character in the base64 alphabet.
     */
    static TUint8 ByteToBase64Letter(const TUint8& aByte);

    /**
     * Maps \a aLetter (in the base64 alphabet) to a byte. Does not match
     * the pad byte.
     *
     * @return \a aLetter (in the base64 alphabet) mapped to a byte or
     *         KNonBase64Letter if the letter does not belong to the base64
     *         alphabet.
     */
    static TUint8 Base64LetterToByte(const TUint8& aLetter);

private:

    /**
     * C++ default constructor.
     * The class is non-instantiable.
     */
    PIMBase64Converter();

    /**
     * Destructor.
     */
    ~PIMBase64Converter();

};

#endif // PIMBASE64CONVERTER_H
// End of File
