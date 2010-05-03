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
* Description:  JNI Layer corresponding to Signature.java
 *
*/


#include "com_nokia_mj_impl_security_SignatureImpl.h"

#include "stssignature.h"
#include "stssignaturefactory.h"
#include "stsconstants.h"

using namespace java::satsa;


// JNI function.
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_security_SignatureImpl__1verify
(JNIEnv* aJni, jclass, jint aHandle,
 jbyteArray aSignature)
{
    int retVal = 0;
    STSSignature* signature = reinterpret_cast< STSSignature* >(aHandle);

    // Call Verify.
    retVal = signature->Verify(aJni,aSignature);

    return retVal;
}

// JNI function.
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_security_SignatureImpl__1update
(JNIEnv* aJni, jclass, jint aHandle, jbyteArray aData, jint aOffset,
 jint aLength)
{
    STSSignature* signature = reinterpret_cast< STSSignature* >(aHandle);
    int err = 0;

    // Call Update
    err = signature->Update(aJni,aData, aOffset,aLength);

    return err;
}

// JNI function.
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_security_SignatureImpl__1initVerify
(JNIEnv* aJni, jclass, jint aHandle, jstring aKeyAlgorithm, jstring aKeyFormat,
 jbyteArray aKeyEncoded)
{
    STSSignature* signature = reinterpret_cast< STSSignature* >(aHandle);
    int err = 0;

    // Call InitVerify
    err = signature->InitVerify(aJni,
                                aKeyAlgorithm,
                                aKeyFormat,
                                aKeyEncoded);

    return err;
}

// JNI function.
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_security_SignatureImpl__1create
(JNIEnv* aJni,
 jclass,
 jstring aAlgorithm)
{

    int handle = KSTSErrNoMemory;
    int errCode = 0;

    STSSignature* aSignature = NULL;

    // Call CreateSignature.
    aSignature = STSSignatureFactory::CreateSignature(aJni, aAlgorithm, &errCode);

    if (aSignature == NULL)
    {
        // CreateSignature  returned error.
        // Set error code to handle variable which is returned to Java
        handle = errCode;
    }
    else
    {
        // signature created successfully, return handle to new signature object
        handle = reinterpret_cast<jint>(aSignature);
    }

    return handle;
}

// JNI function dispose.
JNIEXPORT void
JNICALL Java_com_nokia_mj_impl_security_SignatureImpl__1dispose
(JNIEnv* /*aJni*/,
 jclass,
 jint aHandle)
{

    //Get the signature object from aHandle
    STSSignature* signature = reinterpret_cast< STSSignature* >(aHandle);

    if (signature != NULL)
    {
        // Cleanup the native handle.
        delete signature;
        signature = 0;
    }

}//end dispose



