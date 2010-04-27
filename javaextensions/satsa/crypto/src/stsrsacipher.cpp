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
* Description:  Implementation of STSRSACipher
 *
*/



#include "stsrsacipher.h"
#include "stsconstants.h"
#include  <asymmetric.h>

#include <x509cert.h>
#include <hash.h>
#include <asymmetrickeys.h>
#include <asymmetric.h>
#include <asnpkcs.h>
#include <asn1dec.h>
#include "logger.h"

namespace java
{
namespace satsa
{


STSRSACipher::STSRSACipher()
{
    mRSA = NULL;
    mpkey = NULL;
}

STSRSACipher* STSRSACipher::Create(int* errCode)
{
    STSRSACipher* self = new STSRSACipher;
    if (self == NULL)
    {
        *errCode = KSTSErrNoMemory;
        return NULL;
    }
    else
    {
        *errCode = 0;
        return self;
    }

}


STSRSACipher::~STSRSACipher()
{

}

jint STSRSACipher::DoInit(JNIEnv* aJni, const TCipherMode aMode,
                          const jstring /*aKeyAlgorithm*/, const jstring /*aKeyFormat*/,
                          const jbyteArray aKeyEncoded, const jbyteArray aParams)
{
    LOG(ESATSA, EInfo, "STSRSACipher::DoInit+");
    if (aParams)
    {
        ELOG(ESATSA, "STSRSACipher::DoInit:RSA algorithm cannot have parameters");
        // RSA algorithm cannot have parameters
        return KSTSErrArgument;
    }

    // Read the encoded key from the jbytearray
    int key_length = aJni->GetArrayLength(aKeyEncoded);
    const unsigned char *key = new unsigned char[key_length];
    aJni->GetByteArrayRegion(aKeyEncoded, 0, key_length, (signed char*) key);

    if (aMode == EEncryptMode)
    {
        LOG(ESATSA, EInfo, "STSRSACipher::DoInit:encryption mode");
        // encryption mode: read the public key from subject public key info

        // decode the encoded key data into a RSA data structure.
        mRSA = d2i_RSA_PUBKEY(&mRSA, &key, (long) key_length);
        if (mRSA == NULL)
        {
            ELOG(ESATSA, "STSRSACipher::DoInit:Failed to get proper data");
            return KSTSErrInvalidKey;
        }

    }
    else
    {
        LOG(ESATSA, EInfo, "STSRSACipher::DoInit:decryption mode");
        // decryption mode: decode the private key in PKCS8 format encoded in
        // ASN1 DER format
        // load the algorithm lookup tables
        OpenSSL_add_all_algorithms();

        // create the BIO pointer
        BIO *bp;
        bp = BIO_new_mem_buf((void *) key, key_length);

        // Decode the PKCS8 key from the der encoded key data
        mpkey = EVP_PKEY_new();
        if (NULL == mpkey)
        {
            ELOG(ESATSA, "STSRSACipher::DoInit:Key is null");
            return KSTSErrNoMemory;
        }
        mpkey = d2i_PKCS8PrivateKey_bio(bp, &mpkey, NULL, NULL);
        if (mpkey == NULL)
        {
            ELOG(ESATSA, "STSRSACipher::DoInit:key is improper");
            return KSTSErrInvalidKey;
        }

        // Create the RSA data structure to be used for decryption
        mRSA = EVP_PKEY_get1_RSA(mpkey);
        if (mRSA == NULL)
        {
            ELOG(ESATSA, "STSRSACipher::DoInit:failed to create rsa data");
            return KSTSErrInvalidKey;
        }

    }
    // Init success and mode can be changed
    mMode = aMode;
    LOG(ESATSA, EInfo, "STSRSACipher::DoInit--");
    return 0;

}

jint STSRSACipher::DoFinal(JNIEnv* aJni, jbyteArray aInput, jint aInputOffset,
                           jint aInputLength, jbyteArray aOutput, jint aOutputOffset)
{
    LOG(ESATSA, EInfo, "STSRSACipher::DoFinal+");
    int output_length = 0;
    if (mRSA == NULL)
    {
        // Init was not called successfully.
        ELOG(ESATSA, "STSRSACipher::DoFinal:Init was not called successfully.");
        return (KSTSErrIllegalState);
    }

    //read the input data to be encrypted/decrypted
    unsigned char* inputBuf = new unsigned char[aInputLength];
    aJni->GetByteArrayRegion(aInput, aInputOffset, aInputLength,
                             (signed char *) inputBuf);

    int max_output_length = RSA_size(mRSA);
    unsigned char* outputBuf = new unsigned char[max_output_length];

    if (mMode == EEncryptMode)
    {
        if (aInputLength > (max_output_length - 11))
        {
            ELOG(ESATSA, "STSRSACipher::DoFinal:Wrong Input length");
            return (KSTSErrIllegalState);
        }

        // encrypt the data
        output_length = RSA_public_encrypt(aInputLength, inputBuf, outputBuf,
                                           mRSA, RSA_PKCS1_PADDING);

        if (output_length > aJni->GetArrayLength(aOutput))
        {
            ELOG(ESATSA, "STSRSACipher::DoFinal:short output buffer");
            return (KSTSErrShortBuffer);
        }

        aJni->SetByteArrayRegion(aOutput, aOutputOffset, output_length,
                                 (signed char *) outputBuf);

    }
    else // Decrypt mode
    {
        // decrypt the data
        output_length = RSA_private_decrypt(aInputLength, inputBuf, outputBuf,
                                            mRSA, RSA_PKCS1_PADDING);

        if (output_length > aJni->GetArrayLength(aOutput))
        {
            ELOG(ESATSA, "STSRSACipher::DoFinal:wrong output length");
            return (KSTSErrShortBuffer);
        }

        // encryption/decryption successfully completed
        aJni->SetByteArrayRegion(aOutput, aOutputOffset, output_length,
                                 (signed char *) outputBuf);

    }
    LOG(ESATSA, EInfo, "STSRSACipher::DoFinal:--");
    return output_length;

}

jint STSRSACipher::Update(JNIEnv* aJni, jbyteArray aInput, jint aInputOffset,
                          jint aInputLength, jbyteArray aOutput, jint aOutputOffset)
{
    LOG(ESATSA, EInfo, "STSRSACipher::Update+");
    int output_length = 0;
    if (mRSA == NULL)
    {
        ELOG(ESATSA, "STSRSACipher::Update:Init was not called successfully.");
        // Init was not called successfully.
        return (KSTSErrIllegalState);
    }

    // read the input data to be encrypted/decrypted
    unsigned char* inputBuf = new unsigned char[aInputLength];
    aJni->GetByteArrayRegion(aInput, aInputOffset, aInputLength,
                             (signed char *) inputBuf);

    int max_output_length = RSA_size(mRSA);
    unsigned char* outputBuf = new unsigned char[max_output_length];

    if (mMode == EEncryptMode)
    {
        if (aInputLength > (max_output_length - 11))
        {
            ELOG(ESATSA, "STSRSACipher::Update:wrong input length.");
            return (KSTSErrIllegalStateInput);
        }

        // encrypt the data
        output_length = RSA_public_encrypt(aInputLength, inputBuf, outputBuf,
                                           mRSA, RSA_PKCS1_PADDING);

        if (output_length > aJni->GetArrayLength(aOutput))
        {
            ELOG(ESATSA, "STSRSACipher::Update:wrong output length");
            return (KSTSErrShortBuffer);
        }

        aJni->SetByteArrayRegion(aOutput, aOutputOffset, output_length,
                                 (signed char *) outputBuf);

    }
    else // Decrypt mode
    {
        // decrypt the data
        output_length = RSA_private_decrypt(aInputLength, inputBuf, outputBuf,
                                            mRSA, RSA_PKCS1_PADDING);

        if (output_length > aJni->GetArrayLength(aOutput))
        {
            return (KSTSErrShortBuffer);
        }

        // encryption/decryption successfully completed
        aJni->SetByteArrayRegion(aOutput, aOutputOffset, output_length,
                                 (signed char *) outputBuf);

    }

    LOG(ESATSA, EInfo, "STSRSACipher::Update--");
    return output_length;

}

} // namespace satsa
} // namespace java


