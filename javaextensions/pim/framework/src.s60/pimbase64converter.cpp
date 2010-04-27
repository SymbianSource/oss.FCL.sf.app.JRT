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
* Description:  Base64 coder and decoder.
 *
*/


// INCLUDE FILES
#include "pimbase64converter.h"
#include "pimcommon.h"
#include "pimpanics.h"
#include "logger.h"

// CONSTANTS

/**
 * Six least significant bits.
 */
const TUint8 KSixLSBits = 63; // 00111111

/**
 * Eight least significant bits.
 */
const TUint8 KEightLSBits = 255; // 11111111

/**
 * Pad byte.
 */
const TUint8 KPadLetter = '=';

/**
 * A non-base64 letter.
 * Can be used to denote an erroneous situation.
 */
const TUint8 KNonBase64Letter = 255;

/**
 * Byte array granularity. Byte arrays are usually quite large.
 */
const TInt KByteArrayGranularity = 24;

#if 0 // Disabled to satisfy TCK, which expects no newlines
/**
 * Recommended number of encoded characters per line.
 */
const TInt KNumEncodedCharsPerLine = 76;
#endif

/**
 * Base64 alphabet from RFC 2045.
 * 64 characters, indexed from 0 to 63. The pad byte is not included
 * in the array.
 *
 * <pre>
 *                   Table 1: The Base64 Alphabet
 *
 *    Value Encoding  Value Encoding  Value Encoding  Value Encoding
 *        0 A            17 R            34 i            51 z
 *        1 B            18 S            35 j            52 0
 *        2 C            19 T            36 k            53 1
 *        3 D            20 U            37 l            54 2
 *        4 E            21 V            38 m            55 3
 *        5 F            22 W            39 n            56 4
 *        6 G            23 X            40 o            57 5
 *        7 H            24 Y            41 p            58 6
 *        8 I            25 Z            42 q            59 7
 *        9 J            26 a            43 r            60 8
 *       10 K            27 b            44 s            61 9
 *       11 L            28 c            45 t            62 +
 *       12 M            29 d            46 u            63 /
 *       13 N            30 e            47 v
 *       14 O            31 f            48 w         (pad) =
 *       15 P            32 g            49 x
 *       16 Q            33 h            50 y
 *
 * </pre>
 */
const TUint8 KBase64Alphabet[64] =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',

    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',

    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

CPIMByteArray* PIMBase64Converter::EncodeBase64L(const CPIMByteArray& aSource)
{
    JELOG2(EPim);
    CPIMByteArray* targetBytes =
        new(ELeave) CPIMByteArray(KByteArrayGranularity);
    CleanupStack::PushL(targetBytes);

    // Loop through the source array.
    //
    // Read an 8 bit byte on each iteration. Insert the byte to current
    // quantum, shifting it's contents 8 bits left before insertion.
    // When three bytes are read, append the bytes as four base64-encoded
    // bytes to the target array. Then reset the quantum and start reading
    // another three bytes.
    //
    // If less than three bytes could be read from the source array,
    // Convert a partial quantum. If zero bytes were read, the encoding
    // stops. If one or two bytes were read, pad bytes ('=') are appended
    // after conversion to make it full four base64 bytes and encoding stops.

    TUint32 quantum = 0;
    TInt numBytesInQuantum = 0;
    const TInt numTotalSourceBytes = aSource.Count();
    TInt targetByteCount = 0;

    for (TInt i = 0; i < numTotalSourceBytes; i++)
    {
        TUint8 sourceByte = aSource[i]; // 8 raw, non-encoded bits
        quantum <<= 8;
        quantum |= sourceByte;
        numBytesInQuantum++;

        if (numBytesInQuantum == 3)
        {
            // 24 bits in the quantum
            EncodeQuantumL(quantum, *targetBytes, 3);
            quantum = 0;
            numBytesInQuantum = 0;
            targetByteCount += 4;
        }

#if 0 // Disabled to satisfy TCK, which expects no newlines
        // PIM API TCK does not accept newlines in output.
        if ((targetByteCount> 0) &&
                ((targetByteCount % KNumEncodedCharsPerLine) == 0))
        {
            targetBytes->AppendL('\n');
        }
#endif
    }

    // 0, 8 or 16 bits in the quantum
    EncodeQuantumL(quantum, *targetBytes, numBytesInQuantum);

    CleanupStack::Pop(targetBytes);
    return targetBytes;

}

CPIMByteArray* PIMBase64Converter::DecodeBase64L(const CPIMByteArray& aSource)
{
    JELOG2(EPim);
    CPIMByteArray* targetBytes =
        new(ELeave) CPIMByteArray(KByteArrayGranularity);
    CleanupStack::PushL(targetBytes);

    // Loop through the source array.
    //
    // Read an 8 bit byte on each iteration. If the byte is a pad byte, stop
    // reading. If the byte is not a base64 letter, skip it. Otherwise,
    // consider it a base64 letter and convert it to the corresponding 6 bit
    // value (in an 8 bit byte, of course). Insert the 6 bits to a quantum,
    // shifting its contents 6 bits left before insertion. When four bytes
    // are read, append the bytes as three base64-decoded bytes to the target
    // array. Then reset the quantum and start reading another four bytes.
    //
    // If a pad byte was read before full quantum was constructed, a partial
    // quantum is constructed. Quantum decoding method can also parse an empty
    // quantum, in which case it does nothing.

    TUint32 quantum = 0;
    TInt numBytesInQuantum = 0;
    const TInt numTotalSourceBytes = aSource.Count();

    for (TInt i = 0; i < numTotalSourceBytes; i++)
    {
        TUint8 sourceBase64Letter = aSource[i];

        if (sourceBase64Letter == KPadLetter)
        {
            break; // End of input
        }

        TUint8 sourceByte = Base64LetterToByte(sourceBase64Letter);

        if (sourceByte == KNonBase64Letter)
        {
            continue; // Skip
        }

        quantum <<= 6;
        quantum |= sourceByte;
        numBytesInQuantum++;

        if (numBytesInQuantum == 4)
        {
            // 24 bits in quantum
            DecodeQuantumL(quantum, *targetBytes, 4);
            quantum = 0;
            numBytesInQuantum = 0;
        }
    }

    // 0, 6, 12 or 18 bits in quantum
    DecodeQuantumL(quantum, *targetBytes, numBytesInQuantum);

    CleanupStack::Pop(targetBytes);
    return targetBytes;
}

void PIMBase64Converter::EncodeQuantumL(const TUint32& aQuantum,
                                        CPIMByteArray& aTargetArray, const TInt& aNumSourceBytes)
{
    JELOG2(EPim);
    TInt numTargetBytes = 0;
    TInt padShift = 0;

    switch (aNumSourceBytes)
    {
    case 3: // 24 bits
    {
        numTargetBytes = 4; // 6 + 6 + 6 + 6
        break;
    }
    case 2: // 16 bits
    {
        numTargetBytes = 3; // 6 + 6 + 4
        padShift = 2;
        break;
    }
    case 1: // 8 bits
    {
        numTargetBytes = 2; // 6 + 2
        padShift = 4;
        break;
    }
    case 0: // 0 bits
    {
        return; // Done.
    }
    default:
    {
        User::Leave(KErrArgument);
    }
    }

    TInt numPadBytes = 4 - numTargetBytes;

    while (numTargetBytes > 0)
    {
        numTargetBytes--;
        TUint32 tempQuantum = aQuantum;
        TInt shift = 6 * numTargetBytes;
        tempQuantum <<= padShift;
        tempQuantum >>= shift;
        tempQuantum &= KSixLSBits;

        TUint8 byte = static_cast<TUint8>(tempQuantum);
        TUint8 base64Letter = ByteToBase64Letter(byte);
        aTargetArray.AppendL(base64Letter);
    }

    while (numPadBytes > 0)
    {
        aTargetArray.AppendL(KPadLetter);
        numPadBytes--;
    }
}

void PIMBase64Converter::DecodeQuantumL(const TUint32& aQuantum,
                                        CPIMByteArray& aTargetArray, const TInt& aNumSourceBytes)
{
    JELOG2(EPim);
    TInt numTargetBytes = 0;
    TInt padShift = 0;

    switch (aNumSourceBytes)
    {
    case 4: // 24 bits
    {
        numTargetBytes = 3; // 8 + 8 + 8 bits
        break;
    }
    case 3: // 18 bits
    {
        numTargetBytes = 2; // 8 + 8 (+ 2) bits
        padShift = 2;
        break;
    }
    case 2: // 12 bits
    {
        numTargetBytes = 1; // 8 (+ 4) bits
        padShift = 4;
        break;
    }
    case 1: // 6 bits
    {
        // This should never happen
        __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory,
                                           EPIMPanicBase64Error));
        break;
    }
    case 0: // 0 bits
    {
        return; // Done.
    }
    default:
    {
        User::Leave(KErrArgument);
    }
    }

    while (numTargetBytes > 0)
    {
        numTargetBytes--;
        TUint32 tempQuantum = aQuantum;
        tempQuantum >>= (8 * numTargetBytes + padShift);
        tempQuantum &= KEightLSBits;
        TUint8 byte = static_cast<TUint8>(tempQuantum);
        aTargetArray.AppendL(byte);
    }
}

inline TUint8 PIMBase64Converter::ByteToBase64Letter(const TUint8& aByte)
{
    JELOG2(EPim);
    __ASSERT_DEBUG(aByte < 64, User::Panic(KPIMPanicCategory,
                                           EPIMPanicBase64Error));

    return KBase64Alphabet[aByte];
}

TUint8 PIMBase64Converter::Base64LetterToByte(const TUint8& aLetter)
{
    JELOG2(EPim);
    const TUint8 KUCAlphaBase = 0; // first upper case base64 letter
    const TUint8 KLCAlphaBase = 26; // first lower case base64 letter
    const TUint8 KNumBase = 52; // first numerical letter
    const TUint8 KPlus = 62; // '+' character
    const TUint8 KSlash = 63; // '/' character

    TUint8 retVal = 0;

    if ((aLetter >= 'A') && (aLetter <= 'Z'))
    {
        retVal = static_cast<TUint8>(aLetter - 'A' + KUCAlphaBase);
    }
    else if ((aLetter >= 'a') && (aLetter <= 'z'))
    {
        retVal = static_cast<TUint8>(aLetter - 'a' + KLCAlphaBase);
    }
    else if ((aLetter >= '0') && (aLetter <= '9'))
    {
        retVal = static_cast<TUint8>(aLetter - '0' + KNumBase);
    }
    else if (aLetter == '+')
    {
        retVal = KPlus;
    }
    else if (aLetter == '/')
    {
        retVal = KSlash;
    }
    else
    {
        retVal = KNonBase64Letter;
    }

    __ASSERT_DEBUG((retVal < 64) || (retVal == KNonBase64Letter), User::Panic(
                       KPIMPanicCategory, EPIMPanicBase64Error));

    return retVal;
}

//  End of File
