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
* Description:  Provides the functionality of a RSA based cryptographic
 *                cipher for encryption and decryption.
 *
*/


#ifndef STSRSACIPHER_H
#define STSRSACIPHER_H

//  INCLUDES

#include "stscipher.h"  // Base class
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/evp.h>

class CRSAPKCS1v15Decryptor;
class CRSAPKCS1v15Encryptor;
class CRSAPublicKey;
class CDecPKCS8Data;

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  Implements encryption and decryption cipher functionality for RSA algorithm.
 *
 */
class STSRSACipher: public STSCipher
{
public:

public:

    /**
     * Creates new STSRSACipher
     */
    static STSRSACipher* Create(int* errCode);

    virtual ~STSRSACipher();

public:
    // From STSCipher
    // Functions from STSCipher

    jint DoInit(JNIEnv* aJni, const TCipherMode aMode,
                const jstring aKeyAlgorithm, const jstring aKeyFormat,
                const jbyteArray aKeyEncoded, const jbyteArray aParams);

    jint DoFinal(JNIEnv* aJni, jbyteArray aInput, jint aInputOffset,
                 jint aInputLength, jbyteArray aOutput, jint aOutputOffset);

    jint Update(JNIEnv* aJni, jbyteArray aInput, jint aInputOffset,
                jint aInputLength, jbyteArray aOutput, jint aOutputOffset);

protected:

    /**
     * C++ default constructor.
     */
    STSRSACipher();
private:

    // Cipher's mode: EEncrypt or EDecrypt.
    TCipherMode mMode;

    RSA *mRSA;
    EVP_PKEY *mpkey;

};
} // namespace satsa
} // namespace java
#endif // STSRSACIPHER_H
// End of File
