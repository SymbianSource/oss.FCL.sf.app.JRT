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
* Description:  Implementation of STSDSASignature
 *
*/



#include <x509cert.h>
#include <asymmetric.h>

#include "stsconstants.h"
#include "stsdsasignature.h"

#include "javajniutils.h"
#include "logger.h"

namespace java
{
namespace satsa
{



STSDSASignature::STSDSASignature():mDSA(0), mpkey(0)
{

}


inline int STSDSASignature::Construct(const std::wstring aPadding)
{
    // Create digest object according to aAlgorithm
    if (aPadding == STSDigestSHA1)
    {
        md = EVP_dss1();
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
        // Algorithm was not found
        return KSTSErrDigest;
    }
    if (md == NULL)
    {
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


STSDSASignature* STSDSASignature::Create(const std::wstring aPadding,
        int* errCode)
{
    LOG(ESATSA, EInfo, "STSDSASignature::Create+");
    STSDSASignature* self = new STSDSASignature;
    if (self == NULL)
    {
        // object was not created successfully
        ELOG(ESATSA,
             "STSDSASignature::Create: object was not created successfully");
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


STSDSASignature::~STSDSASignature()
{

    if (0 != mpkey)
    {
        EVP_PKEY_free(mpkey);
    }
    if (0 != mDSA)
    {
        delete mDSA;
        mDSA = 0;
    }

}

int STSDSASignature::InitVerify(JNIEnv* aJni, jstring aKeyAlgorithm,
                                jstring aKeyFormat, jbyteArray aKeyEncoded)
{
    LOG(ESATSA, EInfo, "STSDSASignature::InitVerify+");
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
        ELOG(ESATSA, "caught exception. Return error code");
        return (KSTSErrInvalidKey);
    }
    if (key_algorithm != STSAlgorithmDSA)
    {
        ELOG(ESATSA, "Invalid Key");
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
        ELOG(ESATSA, "caught exception. Return error code");
        return (KSTSErrInvalidKey);
    }
    if (key_format != STSKeyFormatX509)
    {
        ELOG(ESATSA, "Wrong Key Format");
        return (KSTSErrInvalidKey);
    }

    // Read the encoded key from the jbytearray
    int key_length = aJni->GetArrayLength(aKeyEncoded);
    const unsigned char *key = new unsigned char[key_length];
    aJni->GetByteArrayRegion(aKeyEncoded, 0, key_length, (signed char*) key);

    mDSA = d2i_DSA_PUBKEY(&mDSA, &key, (long) key_length);
    if (mDSA == NULL)
    {
        ELOG(ESATSA, "Key is not ok");
        return KSTSErrInvalidKey;
    }

    mpkey = EVP_PKEY_new();
    EVP_PKEY_set1_DSA(mpkey, mDSA);

    retVal = EVP_VerifyInit_ex(mdctx, md, NULL);
    LOG(ESATSA, EInfo, "STSDSASignature::InitVerify--");
    return retVal;

}

int STSDSASignature::Verify(JNIEnv* aJni, jbyteArray aSignature)
{
    LOG(ESATSA, EInfo, "STSDSASignature::Verify+");
    int result;
    if (mpkey == NULL)
    {
        // Not initialized
        ELOG(ESATSA, "STSDSASignature::Verify: Key is not initialized");
        return KSTSErrSignature;
    }
    if (!mIsUpdated)
    {
        return false;
    }

    if (mdctx == NULL)
    {
        // If data is not updated TCK requires that exception is not thrown and
        // false is returned.
        ELOG(ESATSA, "STSDSASignature::Verify:Data is not upadted");
        return false;
    }

    // Read the signature data from the jbytearray
    int siglen = aJni->GetArrayLength(aSignature);
    const unsigned char *sigbuf = new unsigned char[siglen];
    aJni->GetByteArrayRegion(aSignature, 0, siglen, (signed char*) sigbuf);

    result = EVP_VerifyFinal(mdctx, sigbuf, siglen, mpkey);
    mIsUpdated = false;

    // reset the digest
    const EVP_MD* md = NULL;
    md = EVP_MD_CTX_md(mdctx);
    EVP_MD_CTX_init(mdctx);
    EVP_DigestInit_ex(mdctx, md, NULL);
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

int STSDSASignature::Update(JNIEnv* aJni, jbyteArray aData, jint aOffset,
                            jint aLength)
{
    LOG(ESATSA, EInfo, "STSDSASignature::Update+");
    if (mpkey == NULL)
    {
        ELOG(ESATSA, "STSDSASignature::Update: Key is not initialized");
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

    LOG(ESATSA, EInfo, "STSDSASignature::Update--");
    return result;

}
} // namespace satsa
} // namespace java


