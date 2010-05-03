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
* Description:  Implementation of STSSignatureFactory
 *
*/



#include <x509cert.h>
#include <openssl/evp.h>
#include "javajniutils.h"
#include "stssignaturefactory.h"
#include "stsrsasignature.h"
#include "stsdsasignature.h"
#include "stsconstants.h"
#include "logger.h"

namespace java
{
namespace satsa
{

STSSignature* STSSignatureFactory::CreateSignature(JNIEnv* aJni,
        const jstring aAlgorithm, int* errCode)
{
    LOG(ESATSA, EInfo, "STSSignatureFactory::CreateSignature+");
    std::wstring padding;
    std::wstring algorithm;

    //convert the jstring to native string
    std::wstring sigstr;
    try
    {
        sigstr = java::util::JniUtils::jstringToWstring(aJni, aAlgorithm);
    }
    catch (...)
    {
        ELOG(ESATSA, "CreateSignature: caught exception. Return error code");
        *errCode = KSTSErrNoSuchAlgorithm;
        return NULL;
    }

    // Algorithm is a form of <padding>with<algorithm>
    int index = sigstr.find(STSSignatureSeparator);

    // String must contain 'with'
    if (index == wstring::npos)
    {
        ELOG(ESATSA, "CreateSignature: No Such Algorithm");
        *errCode = KSTSErrNoSuchAlgorithm;
        return NULL;
    }

    // Get padding part
    padding.assign(sigstr, 0, index);

    // Skip 'with'
    index += STSSignatureSeparatorLength;

    // Get algorithm part
    algorithm.assign(sigstr, index, wstring::npos);

    // return null if there isn't padding or algorithm
    if (padding.length() <= 0 || algorithm.length() <= 0)
    {
        ELOG(ESATSA, "CreateSignature:there isn't padding or algorithm ");
        *errCode = KSTSErrNoSuchAlgorithm;
        return NULL;
    }

    // With signatures padding name "SHA1" must be used and Message digest
    // algorithm name is "SHA-1".
    if (padding == STSDigestSHA1Signature)
    {
        padding.assign(STSDigestSHA1);
    }

    STSSignature* signature = NULL;

    OpenSSL_add_all_digests();

    if (algorithm == STSAlgorithmDSA)
    {
        signature = STSDSASignature::Create(padding, errCode);
    }
    else if (algorithm == STSAlgorithmRSA)
    {
        signature = STSRSASignature::Create(padding, errCode);
    }
    else
    {
        // Algorithm is not supported
        ELOG(ESATSA, "CreateSignature:algorithm is not supported ");
        *errCode = KSTSErrNoSuchAlgorithm;
        return NULL;
    }

    if (*errCode == 0)
    {
        //no errors, return the signature object
        return signature;
    }
    else
    {
        ELOG(ESATSA, "STSSignatureFactory::CreateSignature:Error, return NULL");
        return NULL;
    }
}

jboolean STSSignatureFactory::IsSupportedAlgorithm(JNIEnv* aJni,
        const jstring aAlgorithm)
{
    LOG(ESATSA, EInfo, "STSSignatureFactory::IsSupportedAlgorithm+");
    //convert the jstring to native string
    std::wstring algorithm;
    try
    {
        algorithm = java::util::JniUtils::jstringToWstring(aJni, aAlgorithm);
    }
    catch (...)
    {
        ELOG(ESATSA, "IsSupportedAlgorithm:caught exception. Return error code");
        return false;
    }
    // RSA and DSA algorithms are supported.
    if (algorithm == STSAlgorithmRSA || algorithm == STSAlgorithmDSA)
    {
        return true;
    }
    else
    {
        ELOG(ESATSA, "IsSupportedAlgorithm:Algorithm is not supported");
        return false;
    }
}

jint STSSignatureFactory::IsValidKey(JNIEnv* aJni, const jstring aAlgorithm,
                                     const jstring aKeyFormat, const jbyteArray aKeyEncoded)
{
    LOG(ESATSA, EInfo, "STSSignatureFactory::IsValidKey+");
    std::wstring algorithm;
    try
    {
        algorithm = java::util::JniUtils::jstringToWstring(aJni, aAlgorithm);
    }
    catch (...)
    {
        ELOG(ESATSA, "IsValidKey: caught exception. Return error code");
        return KSTSErrNotSupported;
    }
    // only X.509 keyformat is supported
    std::wstring format;
    try
    {
        format = java::util::JniUtils::jstringToWstring(aJni, aKeyFormat);
    }
    catch (...)
    {
        ELOG(ESATSA, "IsValidKey: caught exception. Return error code");
        return KSTSErrNotSupported;
    }
    if (format != STSKeyFormatX509)
    {
        ELOG(ESATSA, "IsValidKey:Key Format Not Supported");
        return KSTSErrNotSupported;
    }

    // check the algorithm from the subject public info.

    //Read the encoded key from the jbytearray
    int key_length = aJni->GetArrayLength(aKeyEncoded);
    const unsigned char *key = new unsigned char[key_length];
    aJni->GetByteArrayRegion(aKeyEncoded, 0, key_length, (signed char*) key);

    //check if RSA
    RSA* iRSA = NULL;
    iRSA = d2i_RSA_PUBKEY(&iRSA, &key, (long) key_length);
    if (iRSA == NULL)
    {
        //check for DSA
        DSA* iDSA = NULL;
        iDSA = d2i_DSA_PUBKEY(&iDSA, &key, (long) key_length);

        if (iDSA == NULL)
        {
            //Not RSA or DSA
            ELOG(ESATSA, "STSSignatureFactory::IsValidKey: Neither RSA nor DSA");
            return KSTSErrNotSupported;
        }
        else
        {
            //Check if algorithm is DSA as Key is for DSA
            if (algorithm == STSAlgorithmDSA)
            {
                return 0;
            }
            else
            {
                ELOG(ESATSA, "IsValidKey: DSA not supported");
                return KSTSErrNotSupported;
            }
        }
    }
    else
    {
        //Key is for RSA check the algorithm whether its RSA
        if (algorithm == STSAlgorithmRSA)
        {
            return 0;
        }
        else
        {
            ELOG(ESATSA, "IsValidKey: RSA not supported");
            return KSTSErrNotSupported;
        }
    }

}
} // namespace satsa
} // namespace java

