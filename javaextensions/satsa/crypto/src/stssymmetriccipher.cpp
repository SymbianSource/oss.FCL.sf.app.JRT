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
* Description:  Implementation of STSSymmetricCipher
 *
*/



#include "stssymmetriccipher.h"
#include <openssl/rand.h>
#include <msymmetriccipher.h>
#include <bufferedtransformation.h>
#include <cbcmode.h>
#include <padding.h>
#include <random.h>
#include "javajniutils.h"
#include "stsconstants.h"
#include "logger.h"

namespace java
{
namespace satsa
{


STSSymmetricCipher::STSSymmetricCipher()
{
    mCipher = NULL;
    mCipherCtx = NULL;
    mCipherType = NULL;

}


STSSymmetricCipher::~STSSymmetricCipher()
{
    // cleanup the cipher context
    if (mCipherCtx != NULL)
    {
        EVP_CIPHER_CTX_cleanup(mCipherCtx);
        mCipherCtx = NULL;
    }
    else
    {
        // do nothing
    }
}

STSSymmetricCipher* STSSymmetricCipher::Create(
    STSTransformation* aTransformation, int* errCode)
{
    LOG(ESATSA, EInfo, "STSSymmetricCipher::Create+");
    STSSymmetricCipher* self = new STSSymmetricCipher;
    if (self == NULL)
    {
        *errCode = KSTSErrNoMemory;
        return NULL;
    }
    int retVal = self->Construct(aTransformation);
    if (0 == retVal)
    {
        *errCode = retVal;
        return self;
    }
    else
    {
        ELOG(ESATSA, "STSSymmetricCipher::Create failed");
        *errCode = retVal;
        delete self;
        self = 0;
        return NULL;
    }

}

int STSSymmetricCipher::Construct(STSTransformation* aTransformation)
{
    LOG(ESATSA, EInfo, "STSSymmetricCipher::Construct+");
    // load ciphers that are needed for lookup by openc apis
    OpenSSL_add_all_ciphers();

    // Check transformation validity.

    // Check mode
    if (aTransformation->Mode() != STSModeCBC && aTransformation->Mode()
            != STSModeECB && aTransformation->Mode() != STSModeNONE)
    {
        ELOG(ESATSA, "STSSymmetricCipher::Construct: mode is not supported");
        // Mode is not supported
        return (KSTSErrNoSuchAlgorithm);

    }

    // Check padding
    if (aTransformation->Padding() != STSPaddingNone
            && aTransformation->Padding() != STSPaddingPKCS7
            && aTransformation->Padding() != STSPaddingPKCS5)
    {
        ELOG(ESATSA, "Construct: padding not supported");
        // Padding is not supported
        return (KSTSErrNoSuchPadding);

    }

    // No errors, object constructed successfully
    LOG(ESATSA, EInfo, "STSSymmetricCipher::Construct--");
    return 0;

}

jint STSSymmetricCipher::DoInit(JNIEnv* aJni, const TCipherMode aMode,
                                const jstring aKeyAlgorithm, const jstring aKeyFormat,
                                const jbyteArray aKeyEncoded, const jbyteArray aParams)
{
    LOG(ESATSA, EInfo, "STSSymmetricCipher::DoInit+");
    //Function return value
    jint retVal = 0;

    // clear the old iv as the cipher is now being initialized/re-initialized
    if (mIV != NULL)
    {
        delete mIV;
        mIV = NULL;
    }
    miv_length = 0;

    //clear the old mKey
    if (mKey != NULL)
    {
        delete mKey;
        mKey = NULL;
    }
    mKey_length = 0;

    // Format: Only raw keys are supported
    std::wstring key_format;
    try
    {
        key_format = java::util::JniUtils::jstringToWstring(aJni, aKeyFormat);
    }
    catch (...)
    {
        ELOG(ESATSA, "DoInit: caught exception. Return error code");
        return (KSTSErrInvalidKey);
    }
    if (key_format != STSKeyFormatRAW)
    {
        ELOG(ESATSA, "DoInit:Not raw keys");
        return (KSTSErrInvalidKey);
    }

    // Key must be created for used algorithm
    std::wstring key_algorithm;
    try
    {
        key_algorithm = java::util::JniUtils::jstringToWstring(aJni,
                        aKeyAlgorithm);
    }
    catch (...)
    {
        ELOG(ESATSA, "DoInit: caught exception. Return error code");
        return (KSTSErrInvalidKey);
    }
    if (mTransformation->Algorithm() != key_algorithm)
    {
        ELOG(ESATSA, "DoInit:Not proper algorithm");
        return (KSTSErrInvalidKey);
    }

    // read the Key
    //Create the native array from jbytearray
    mKey_length = aJni->GetArrayLength(aKeyEncoded);
    mKey = new unsigned char[mKey_length];
    aJni->GetByteArrayRegion(aKeyEncoded, 0, mKey_length, (signed char*) mKey);

    // Set the cipher type and Mode
    if (mTransformation->Algorithm() == STSAlgorithmDES)
    {
        ELOG(ESATSA, "DoInit: DES algorithm");
        if (mTransformation->Mode() == STSModeCBC)
        {
            mCipherType = EVP_des_cbc();
        }
        else if (mTransformation->Mode() == STSModeECB)
        {
            mCipherType = EVP_des_ecb();
        }
        else if (mTransformation->Mode() == STSModeNONE)
        {
            mCipherType = EVP_des_cbc();
        }

    }
    else if (mTransformation->Algorithm() == STSAlgorithm3DES)
    {
        LOG(ESATSA, EInfo, "STSSymmetricCipher::DoInit: 3DES algorithm");
        if (mTransformation->Mode() == STSModeCBC)
        {
            mCipherType = EVP_des_ede3_cbc();
        }
        else if (mTransformation->Mode() == STSModeECB)
        {
            mCipherType = EVP_des_ede3_ecb();
        }
        else if (mTransformation->Mode() == STSModeNONE)
        {
            mCipherType = EVP_des_ede3_cbc();
        }

    }
    else if (mTransformation->Algorithm() == STSAlgorithmRC2)
    {
        LOG(ESATSA, EInfo, "STSSymmetricCipher::DoInit: RC2 algorothm");
        if (mTransformation->Mode() == STSModeCBC)
        {
            mCipherType = EVP_rc2_cbc();
        }
        else if (mTransformation->Mode() == STSModeECB)
        {
            mCipherType = EVP_rc2_ecb();
        }
        else if (mTransformation->Mode() == STSModeNONE)
        {
            mCipherType = EVP_rc2_cbc();
        }

    }
    else if (mTransformation->Algorithm() == STSAlgorithmAES)
    {
        LOG(ESATSA, EInfo, "STSSymmetricCipher::DoInit: AES algorithm");
        // AES algorithms based on key size
        switch (mKey_length)
        {
        case 16:
            if (mTransformation->Mode() == STSModeCBC)
            {
                mCipherType = EVP_aes_128_cbc();
            }
            else if (mTransformation->Mode() == STSModeECB)
            {
                mCipherType = EVP_aes_128_ecb();
            }
            else if (mTransformation->Mode() == STSModeNONE)
            {
                mCipherType = EVP_aes_128_cbc();
            }
            break;
        case 24:
            if (mTransformation->Mode() == STSModeCBC)
            {
                mCipherType = EVP_aes_192_cbc();
            }
            else if (mTransformation->Mode() == STSModeECB)
            {
                mCipherType = EVP_aes_192_ecb();
            }
            else if (mTransformation->Mode() == STSModeNONE)
            {
                mCipherType = EVP_aes_192_cbc();
            }
            break;
        case 32:
            if (mTransformation->Mode() == STSModeCBC)
            {
                mCipherType = EVP_aes_256_cbc();
            }
            else if (mTransformation->Mode() == STSModeECB)
            {
                mCipherType = EVP_aes_256_ecb();
            }
            else if (mTransformation->Mode() == STSModeNONE)
            {
                mCipherType = EVP_aes_256_cbc();
            }
            break;
        default:
            // Invalid Key Size
            ELOG(ESATSA, "DoInit:Invalid Key Size");
            return (KSTSErrInvalidKey);

        };
    }

    // cipher type could not be determined
    if (mCipherType == NULL)
    {
        ELOG(ESATSA, "DoInit:cipher type could not be determined");
        return (KSTSErrNoSuchAlgorithm);
    }
    // determined cipher type,  determine and validate the parameters
    // Block size is needed for padding and iv
    int blocksize = EVP_CIPHER_block_size(mCipherType);

    // Check parameters, create IV for algorithm.
    if (mTransformation->Mode() == STSModeECB)
    {
        // ECB mode may not have IV
        if (aParams)
        {
            ELOG(ESATSA, "DoInit:ECB mode may not have IV");
            return (KSTSErrArgument);
        }
    }
    else
    {
        // CBC mode must have IV
        if (aParams)
        {
            // IV was given as a parameter
            // Create the native array from jbytearray
            miv_length = aJni->GetArrayLength(aParams);
            mIV = new unsigned char[miv_length];
            aJni->GetByteArrayRegion(aParams, 0, miv_length, (signed char*) mIV);

            // IV's size must be same as block size.
            if (blocksize != miv_length)
            {
                ELOG(ESATSA, "DoInit:blocksize mismatch");
                return (KSTSErrArgument);
            }

        }
        else
        {
            if (aMode == EDecryptMode)
            {
                ELOG(ESATSA, "DoInit: In decrypt mode IV is needed.");
                // In encrypt mode IV is needed.
                return (KSTSErrArgument);
            }

            // IV was not given, generate new random iv same as blocksize
            miv_length = blocksize;
            mIV = new unsigned char[miv_length];
            if (mIV)
            {
                RAND_pseudo_bytes(mIV, miv_length);
            }
            else
            {
                mIV = NULL;
                miv_length = 0;
                ELOG(ESATSA, "DoInit:IV is not proper");
                return KSTSErrNoMemory;
            }

        }

    }

    // Initialize the cipher context
    mCipherCtx = new EVP_CIPHER_CTX;
    if (mCipherCtx == NULL)
    {
        ELOG(ESATSA, "DoInit:cipher context is null");
        // cipher context could not be allocated
        return (KSTSErrNoMemory);
    }
    // Initialize the cipher context
    EVP_CIPHER_CTX_init(mCipherCtx);

    // Create decryptor or encryptor according to mode
    mMode = aMode;
    int ret;
    if (mMode == EEncryptMode)
    {
        ret = EVP_CipherInit_ex(mCipherCtx, mCipherType, NULL, mKey, mIV, 1);
        if (0 == ret)
        {
            ELOG(ESATSA, "DoInit:cipher:encrypt:cipher could not be initialized");
            retVal = KSTSErrNoMemory;
        }
    }
    else
    {
        // Otherwise EDecryptMode
        ret = EVP_CipherInit_ex(mCipherCtx, mCipherType, NULL, mKey, mIV, 0);
        if (0 == ret)
        {
            ELOG(ESATSA, "DoInit:cipher:decrypt:cipher could not be initialized");
            retVal = KSTSErrNoMemory;
        }
    }

    // Set the padding
    if (mTransformation->Padding() == STSPaddingPKCS7
            || mTransformation->Padding() == STSPaddingPKCS5)
    {
        LOG(ESATSA, EInfo, "Padding for pkcs7 or pkcs5");
        EVP_CIPHER_CTX_set_padding(mCipherCtx, 1); // always returns 1 (Success)
    }
    else
    {
        EVP_CIPHER_CTX_set_padding(mCipherCtx, 0); // always returns 1 (Success)
    }

    // reset state.
    mBytesProcessed = 0;
    //return
    LOG(ESATSA, EInfo, "STSSymmetricCipher::DoInit--");
    return retVal;
}

jint STSSymmetricCipher::DoFinal(JNIEnv* aJni, jbyteArray aInput,
                                 jint aInputOffset, jint aInputLength, jbyteArray aOutput,
                                 jint aOutputOffset)
{
    LOG(ESATSA, EInfo, "STSSymmetricCipher::DoFinal+");
    // Variable to hold the error code
    int retVal = 0;

    if (mCipherCtx == NULL || mCipherType == NULL)
    {
        ELOG(ESATSA, "Cipher context/type not ready");
        // Init was not called successfully.
        return (KSTSErrIllegalState);
    }

    int blocksize = EVP_CIPHER_block_size(mCipherType);
    int bytesProcessed = aInputLength + mBytesProcessed;
    // reset state.
    mBytesProcessed = 0;
    if ((bytesProcessed % blocksize != 0) && ((mMode == EDecryptMode)
            || (mTransformation->Padding() == STSPaddingNone)))
    {
        ELOG(ESATSA, "Processing failed");
        return (KSTSErrIllegalBlockSize);
    }

    // read the input data
    unsigned char* inputBuf = new unsigned char[aInputLength];
    aJni->GetByteArrayRegion(aInput, aInputOffset, aInputLength,
                             (signed char *) inputBuf);

    // create the output native array to hold the output
    int max_output_length = aInputLength + blocksize - 1;
    unsigned char* outputBuf = new unsigned char[max_output_length];
    int outputLength = 0;

    // if inputlength is  > 0 then there is some input data to update
    if (aInputLength > 0)
    {
        // call update
        retVal = EVP_CipherUpdate(mCipherCtx, outputBuf, &outputLength,
                                  inputBuf, aInputLength);
        if (0 == retVal)
        {
            ELOG(ESATSA, "Update OpenC API failed");
            // openc api returned failure
            return KSTSErrBadPadding;
        }
    }

    // finalize the output
    int final_length = 0;
    retVal = EVP_CipherFinal_ex(mCipherCtx, outputBuf + outputLength,
                                &final_length);
    if (0 == retVal)
    {
        ELOG(ESATSA, "Final OpenC API failed");
        // openc api returned failure
        return KSTSErrBadPadding;
    }

    // Re-initialize the cipher back to its init state
    int ret = EVP_CIPHER_CTX_cleanup(mCipherCtx);
    if (0 == ret)
    {
        ELOG(ESATSA, "Cleanup OpenC API failed");
        retVal = KSTSErrIllegalState;
    }
    // Initialize the cipher context
    EVP_CIPHER_CTX_init(mCipherCtx);

    // Create decryptor or encryptor according to mode
    if (mMode == EEncryptMode)
    {
        int ret = EVP_CipherInit_ex(mCipherCtx, mCipherType, NULL, mKey, mIV, 1);
        if (0 == ret)
        {
            ELOG(ESATSA, "encrypt mode:cipher init failed");
            retVal = KSTSErrNoMemory;
        }
    }
    else
    {
        // Otherwise EDecryptMode
        int ret = EVP_CipherInit_ex(mCipherCtx, mCipherType, NULL, mKey, mIV, 0);
        if (0 == ret)
        {
            retVal = KSTSErrNoMemory;
        }
    }

    // Set the padding
    if (mTransformation->Padding() == STSPaddingPKCS7
            || mTransformation->Padding() == STSPaddingPKCS5)
    {
        EVP_CIPHER_CTX_set_padding(mCipherCtx, 1); // always returns 1(Success)
    }
    else
    {
        EVP_CIPHER_CTX_set_padding(mCipherCtx, 0); // always returns 1(Success)
    }

    if ((outputLength + final_length) > (aJni->GetArrayLength(aOutput)
                                         - aOutputOffset))
    {
        // Output buffer cannot hold the output of the cipher
        return (KSTSErrShortBuffer);
    }

    aJni->SetByteArrayRegion(aOutput, aOutputOffset, (outputLength
                             + final_length), (signed char *) outputBuf);
    retVal = outputLength + final_length;

    LOG(ESATSA, EInfo, "STSSymmetricCipher::DoFinal--");
    return retVal;

}

jint STSSymmetricCipher::Update(JNIEnv* aJni, jbyteArray aInput,
                                jint aInputOffset, jint aInputLength, jbyteArray aOutput,
                                jint aOutputOffset)
{
    LOG(ESATSA, EInfo, "STSSymmetricCipher::Update+");
    // Variable to hold the error code
    int retVal = 0;

    if (mCipherCtx == NULL || mCipherType == NULL)
    {
        ELOG(ESATSA, "Init was not called successfully");
        // Init was not called successfully.
        return (KSTSErrIllegalState);
    }
    else if (0 == aInputLength)
    {
        // if inputlength is 0 then there is nothing to update
        ELOG(ESATSA, "Inputlength 0, nothing to update");
        // return 0
        return retVal;
    }

    // read the input data
    unsigned char* inputBuf = new unsigned char[aInputLength];
    aJni->GetByteArrayRegion(aInput, aInputOffset, aInputLength,
                             (signed char *) inputBuf);

    // create the output native array to hold the output
    int blocksize = EVP_CIPHER_block_size(mCipherType);
    int max_output_length = aInputLength + blocksize - 1;
    unsigned char* outputBuf = new unsigned char[max_output_length];
    int outputLength = 0;

    // call the update function
    retVal = EVP_CipherUpdate(mCipherCtx, outputBuf, &outputLength, inputBuf,
                              aInputLength);
    if (0 == retVal)
    {
        ELOG(ESATSA, "Update OpenC API failed");
        // openc api returned failure
        return KSTSErrBadPadding;
    }
    else if ((outputLength) > (aJni->GetArrayLength(aOutput) - aOutputOffset))
    {
        ELOG(ESATSA, "Update: Short buffer");
        return (KSTSErrShortBuffer);
    }

    aJni->SetByteArrayRegion(aOutput, aOutputOffset, outputLength,
                             (signed char *) outputBuf);

    mBytesProcessed += aInputLength;

    // return the number of bytes processed.
    retVal = outputLength;
    LOG(ESATSA, EInfo, "STSSymmetricCipher::Update--");
    return retVal;

}
} // namespace satsa
} // namespace java


