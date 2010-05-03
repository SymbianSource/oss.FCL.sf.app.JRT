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
* Description:  Implementation of STSRSASIgnature
 *
*/



#include <x509cert.h>
#include <hash.h>
#include <asymmetrickeys.h>
#include <asymmetric.h>
#include "stsconstants.h"
#include "stsrsasignature.h"

#include "javajniutils.h"
#include "logger.h"

namespace java
{
namespace satsa
{

STSRSASignature::STSRSASignature()
{
    mRSAKey = NULL;
    mHashID = NULL;
    mRSA = NULL;
    mpkey = NULL;
}



int STSRSASignature::Construct(const std::wstring aPadding)
{
    LOG(ESATSA, EInfo, "STSRSASignature::Construct+");

    if (aPadding == STSDigestSHA1)
    {
        md = EVP_sha1();
    }
    else if (aPadding == STSDigestMD2)
    {
        md = EVP_md2();
    }
    else if (aPadding == STSDigestMD5)
    {
        md = EVP_md5();
    }
    else
    {
        return KSTSErrArgument;
    }

    if (md == NULL)
    {
        // Algorithm was not found
        return KSTSErrDigest;
    }
    else
    {

        mdctx = new EVP_MD_CTX;
        if (mdctx != NULL)
        {
            EVP_MD_CTX_init(mdctx);
            return 0;
        }
        else
        {
            return KSTSErrDigest;
        }
    }

}



STSRSASignature* STSRSASignature::Create(const std::wstring aPadding,
        int* errCode)
{
    LOG(ESATSA, EInfo, "STSRSASignature::Create+");
    STSRSASignature* self = new STSRSASignature;
    if (self == NULL)
    {
        // object was not created successfully
        *errCode = KSTSErrNoMemory;
        return NULL;
    }
    else
    {
        // Object created, call the second level constructor
        *errCode = self->Construct(aPadding);
        if (*errCode == 0)
        {
            return self;
        }
        else
        {
            delete self;
            return NULL;
        }
    }
}


STSRSASignature::~STSRSASignature()
{
    if (mHashID != NULL)
        delete mHashID;

    if (mRSAKey != NULL)
        delete mRSAKey;

    if (mpkey != NULL)
        EVP_PKEY_free(mpkey);

    if (mRSA != NULL)
        mRSA = NULL;

}

int STSRSASignature::InitVerify(JNIEnv* aJni, jstring aKeyAlgorithm,
                                jstring aKeyFormat, jbyteArray aKeyEncoded)
{
    LOG(ESATSA, EInfo, "STSRSASignature::InitVerify+");
    int retVal = 0;

    // Check key validity

    // Key must be created for used algorithm
    std::wstring key_algorithm;
    try
    {
        key_algorithm = java::util::JniUtils::jstringToWstring(aJni,
                        aKeyAlgorithm);
    }
    catch (...)
    {
        ELOG(ESATSA, "InitVerify: caught exception. Return error code");
        return (KSTSErrInvalidKey);
    }
    ELOG(ESATSA, "STSRSASignature::InitVerify: Check Algorithm");
    if (key_algorithm != STSAlgorithmRSA)
    {
        return (KSTSErrInvalidKey);
    }
    // Format: Only X509 keys are supported
    std::wstring key_format;
    try
    {
        key_format = java::util::JniUtils::jstringToWstring(aJni, aKeyFormat);
    }
    catch (...)
    {
        ELOG(ESATSA, "InitVerify: caught exception. Return error code");
        return (KSTSErrInvalidKey);
    }

    LOG(ESATSA, EInfo, "STSRSASignature::InitVerify: Check key format");
    if (key_format != STSKeyFormatX509)
    {
        return (KSTSErrInvalidKey);
    }

    // Read the encoded key from the jbytearray
    LOG(ESATSA, EInfo, "STSRSASignature::InitVerify:read the encoded key");
    int key_length = aJni->GetArrayLength(aKeyEncoded);
    const unsigned char *key = new unsigned char[key_length];
    aJni->GetByteArrayRegion(aKeyEncoded, 0, key_length, (signed char*) key);

    // decode the encoded key data into a RSA data structure.
    LOG(ESATSA, EInfo,"decode the encoded key data into a RSA data");
    mRSA = d2i_RSA_PUBKEY(&mRSA, &key, (long) key_length);
    if (mRSA == NULL)
    {
        return KSTSErrInvalidKey;
    }

    LOG(ESATSA, EInfo, "STSRSASignature::InitVerify: get mpkey");
    mpkey = EVP_PKEY_new();
    EVP_PKEY_set1_RSA(mpkey, mRSA);

    LOG(ESATSA, EInfo, "STSRSASignature::InitVerify: verify init call");
    retVal = EVP_VerifyInit_ex(mdctx, md, NULL);

    LOG(ESATSA, EInfo, "STSRSASignature::InitVerify---");
    return retVal;

}

int STSRSASignature::Verify(JNIEnv* aJni, jbyteArray aSignature)
{
    LOG(ESATSA, EInfo, "STSRSASignature::Verify++");
    int result;

    if (mpkey == NULL)
    {
        ELOG(ESATSA, "STSRSASignature::Verify: mpkey is null");
        // Not initialized
        return (KSTSErrSignature);
    }
    else if (!mIsUpdated)
    {
        LOG(ESATSA, EInfo, "STSRSASignature::Verify: Not updated");
        return false;
    }
    else if (mdctx == NULL)
    {
        // If data is not updated TCK requires that exception is not thrown and
        // false is returned.
        ELOG(ESATSA, "STSRSASignature::Verify: mdctx is null");
        return false;
    }

    // Read the signature data from the jbytearray
    int siglen = aJni->GetArrayLength(aSignature);
    const unsigned char *sigbuf = new unsigned char[siglen];
    aJni->GetByteArrayRegion(aSignature, 0, siglen, (signed char*) sigbuf);

    result = EVP_VerifyFinal(mdctx, sigbuf, siglen, mpkey);
    mIsUpdated = false;

    // reset the digest
    LOG(ESATSA, EInfo, "STSRSASignature::Verify: reset the digest");
    const EVP_MD* md = NULL;
    md = EVP_MD_CTX_md(mdctx);
    EVP_MD_CTX_init(mdctx);
    EVP_DigestInit_ex(mdctx, md, NULL);

    LOG(ESATSA, EInfo, "Verify: check result and return proper value");
    if (0 == result)
    {
        return false;
    }
    else if (1 == result)
    {
        return true;
    }
    else
    {
        return KSTSErrSignature;
    }
}

int STSRSASignature::Update(JNIEnv* aJni, jbyteArray aData, jint aOffset,
                            jint aLength)
{
    LOG(ESATSA, EInfo, "STSRSASignature::Update++");
    if (mpkey == NULL)
    {
        ELOG(ESATSA, "STSRSASignature::Update: mpkey is null");
        // Not initialized
        return KSTSErrSignature;
    }

    // Read the signature data from the jbytearray
    const unsigned char *data = new unsigned char[aLength];
    aJni->GetByteArrayRegion(aData, aOffset, aLength, (signed char*) data);

    int result = EVP_VerifyUpdate(mdctx, (const void *) data,
                                  (unsigned int) aLength);

    if (!mIsUpdated)
    {
        mIsUpdated = true;
    }
    LOG(ESATSA, EInfo, "STSRSASignature::Update: return result");
    return result;
}

} // namespace satsa
} // namespace java

