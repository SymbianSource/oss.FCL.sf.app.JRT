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
* Description:  JNI Layer corresponding to MessageDigestImpl.java
*
*/


#include "javajniutils.h"
#include "com_nokia_mj_impl_security_MessageDigestImpl.h"
#include <stdio.h>
#include <openssl/evp.h>
#include "stsconstants.h"
#include <hash.h>
#include "logger.h"

using namespace java::satsa;


// JNI function.
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_security_MessageDigestImpl__1reset
(JNIEnv*, jclass, jint aHandle)
{
    const EVP_MD* md = NULL;
    int retVal;

    EVP_MD_CTX* mdctx = reinterpret_cast< EVP_MD_CTX* >(aHandle);

    md = EVP_MD_CTX_md(mdctx);
    if (md == NULL)
    {
        return KSTSErrNoMemory;
    }
    EVP_MD_CTX_init(mdctx);
    retVal = EVP_DigestInit_ex(mdctx, md, NULL);
    if (retVal == 0)
    {
        // Set error code to handle variable which is returned to Java
        return KSTSErrNoMemory;
    }

    return 0;
}


// JNI function.
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_security_MessageDigestImpl__1digest
(JNIEnv* aJni, jclass, jint aHandle,
 jbyteArray aBuf, jint aOffset, jint aLength)
{
    EVP_MD_CTX* mdctx = reinterpret_cast< EVP_MD_CTX* >(aHandle);
    const EVP_MD* md = NULL;
    unsigned int retVal;
    unsigned int err;
    unsigned char* buf = new unsigned char[EVP_MAX_MD_SIZE];
    err = EVP_DigestFinal_ex(mdctx, buf, &retVal);
    if (err == 1)
    {
        //digest successful
        if (aLength < retVal)
        {
            return KSTSErrNoMemory;
        }
        else
        {
            // set the output buffer with hashed data
            aJni->SetByteArrayRegion(aBuf,aOffset,retVal,(signed char*)buf);

            // reset the digest as per the spec
            md = EVP_MD_CTX_md(mdctx);
            if (md == NULL)
            {
                return KSTSErrNoMemory;
            }
            EVP_MD_CTX_init(mdctx);
            err = EVP_DigestInit_ex(mdctx, md, NULL);
            if (err == 0)
            {
                // Set error code to handle variable which is returned to Java
                return KSTSErrNoMemory;
            }
            else
            {
                return retVal;
            }
        }
    }
    else
    {
        return KSTSErrNoMemory;
    }
}


// JNI function.
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_security_MessageDigestImpl__1update
(JNIEnv* aJni, jclass, jint aHandle,
 jbyteArray aBuf, jint aOffset, jint aLength)
{
    int err;
    EVP_MD_CTX* mdctx = reinterpret_cast< EVP_MD_CTX* >(aHandle);
    unsigned char* buf = new unsigned char[aLength];
    aJni->GetByteArrayRegion(aBuf,aOffset,aLength,(signed char *)buf);
    err = EVP_DigestUpdate(mdctx, buf, aLength);
    if (0 == err)
    {
        err = KSTSErrNoMemory;
    }
    return err;
}


// JNI function.
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_security_MessageDigestImpl__1create
(JNIEnv* aJni, jclass,
 jstring aAlgorithm)
{
    int retVal;
    EVP_MD_CTX *mdctx = new EVP_MD_CTX;
    const EVP_MD *md;

    OpenSSL_add_all_digests();

    // Set the transformation string contents to a wstring.
    std::wstring algo;
    try
    {
        algo = java::util::JniUtils::jstringToWstring(aJni, aAlgorithm);
    }
    catch (...)
    {
        // do nothing
    }
    TInt handle = KSTSErrNoMemory;

    // Create digest object according to aAlgorithm
    if (algo == L"SHA-1")
    {
        md = EVP_sha1();
    }
    else if (algo == L"MD2")
    {
        md = EVP_md2();
    }
    else if (algo == L"MD5")
    {
        md = EVP_md5();
    }
    else
    {
        // Algorithm was not found
        handle = KSTSErrDigest;
        return handle;
    }

    EVP_MD_CTX_init(mdctx);
    retVal = EVP_DigestInit_ex(mdctx, md, NULL);
    if (0 == retVal)
    {
        // Set error code to handle variable which is returned to Java
        handle = KSTSErrNoMemory;
        return handle;
    }

    // return handle to new MessageDigest object or error code
    handle = reinterpret_cast<jint>(mdctx);
    return handle;
}


// JNI function dispose.
JNIEXPORT void
JNICALL Java_com_nokia_mj_impl_security_MessageDigestImpl__1dispose
(JNIEnv* /*aJni*/,
 jclass,
 jint aHandle)
{

    //Get the digest object from aHandle
    EVP_MD_CTX* mdctx = reinterpret_cast< EVP_MD_CTX* >(aHandle);

    if (mdctx != NULL)
    {
        // Cleanup the native handle.
        delete mdctx;
        mdctx = 0;
    }

}//end dispose

//end of file



