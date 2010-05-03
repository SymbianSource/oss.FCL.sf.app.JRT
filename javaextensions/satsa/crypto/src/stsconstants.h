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
* Description:  Internal constants.
 *
*/


#ifndef STSCONSTANTS_H
#define STSCONSTANTS_H



namespace java
{
namespace satsa
{


// KSTSErr<java expection name without Expection word>
// Refer to JSR177 for correct meaning for expections.

// Base value for own error code to distinguish them from native error codes.
const int KSTSErrBase = -3000;

const int KSTSErrIllegalState = KSTSErrBase - 1;
const int KSTSErrSignature = KSTSErrBase - 2;
const int KSTSErrBadPadding = KSTSErrBase - 3;
const int KSTSErrIllegalBlockSize = KSTSErrBase - 4;
const int KSTSErrNoSuchPadding = KSTSErrBase - 5;
const int KSTSErrNoSuchAlgorithm = KSTSErrBase - 6;
const int KSTSErrInvalidKey = KSTSErrBase - 7;
const int KSTSErrInvalidAlgorithmParameter = KSTSErrBase - 8;
const int KSTSErrShortBuffer = KSTSErrBase - 9;
const int KSTSErrDigest = KSTSErrBase - 10;
const int KSTSErrIllegalStateInput = KSTSErrBase - 11;

const int KSTSErrNoMemory = -4;
const int KSTSErrNotSupported = -5;
const int KSTSErrArgument = -6;

// **** CIPHER
// A transformation is of the form:
// algorithm/mode/padding
#define STSTransformationDelim L"/"

//new defines
// **** ALGORITHM
#define STSAlgorithmRSA L"RSA"
#define STSAlgorithmDSA L"DSA"

#define STSAlgorithmDES L"DES"
#define STSAlgorithmAES L"AES"
#define STSAlgorithmRC2 L"RC2"
#define STSAlgorithm3DES L"DESEDE"

// **** MODE
#define STSModeCBC L"CBC"
#define STSModeECB L"ECB"
#define STSModeNONE L"NONE"

// **** PADDING
#define STSPaddingNone L"NOPADDING"
#define STSPaddingPKCS7 L"PKCS7PADDING"
#define STSPaddingPKCS5 L"PKCS5PADDING"

// **** KEY FORMATS
#define STSKeyFormatRAW L"RAW"
#define STSKeyFormatX509 L"X.509"
#define STSKeyFormatPKCS8 L"PKCS8"

// **** MESSAGE DIGEST
#define STSDigestSHA L"SHA"

// The message digest algorithm as defined in NIST's FIPS 180-1.
// The output of this algorithm is a 160-bit digest.
#define STSDigestSHA1 L"SHA-1"

// SHA-1 message digest algorithm name to be used with signatures.
#define STSDigestSHA1Signature L"SHA1"

// The message digest algorithm as defined in RFC 1319.
// The output of this algorithm is a 128-bit (16 byte) digest.
#define STSDigestMD2 L"MD2"

// The message digest algorithm as defined in RFC 1321.
// The output of this algorithm is a 128-bit (16 byte) digest.
#define STSDigestMD5 L"MD5"

// **** SIGNATURE
// Signature algorithm is a form of <digest>with<encryption>
#define STSSignatureSeparator L"WITH"
#define STSSignatureSeparatorLength 4

} // namespace satsa
} // namespace java
#endif // STSCONSTANTS_H
// End of File
