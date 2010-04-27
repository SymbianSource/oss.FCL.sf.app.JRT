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
* Description:  JNI Layer corresponding to CipherImpl.java
*
*/


#include "com_nokia_mj_impl_crypto_CipherImpl.h"
#include "stscipher.h"
#include "stscipherfactory.h"
#include "stsconstants.h"

using namespace java::satsa;



// JNI function update.
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_crypto_CipherImpl__1update
(JNIEnv* aJni,
 jclass,
 jint aHandle,
 jbyteArray aInput,
 jint aInputOffset,
 jint aInputLength,
 jbyteArray aOutput,
 jint aOutputOffset,
 jboolean aDoFinal)
{

    // return value that has to be passed to java
    jint retVal = 0;

    // Get the cipher object from aHandle
    STSCipher* cipher = reinterpret_cast< STSCipher* >(aHandle);

    if (aDoFinal == JNI_TRUE)
    {
        // Finalize flag is set, call the native cipher's finalize function.
        retVal = cipher->DoFinal(aJni,
                                 aInput,
                                 aInputOffset,
                                 aInputLength,
                                 aOutput,
                                 aOutputOffset);
    }
    else
    {
        // Finalize flag is not set, call the native cipher's Update function.
        retVal = cipher->Update(aJni,
                                aInput,
                                aInputOffset,
                                aInputLength,
                                aOutput,
                                aOutputOffset);
    } // end if

    // return to Java Side
    return retVal;

}// end update



// JNI function getIV.
JNIEXPORT jbyteArray JNICALL Java_com_nokia_mj_impl_crypto_CipherImpl__1getIV
(JNIEnv* aJni,
 jclass,
 jint aHandle)
{
    // iv to be returned to java
    jbyteArray ivToJava = NULL;
    const unsigned char* iv = NULL;
    int iv_length = 0;

    // Get the cipher object from aHandle
    STSCipher* cipher = reinterpret_cast< STSCipher* >(aHandle);

    // retrieve the iv from native cipher object
    iv = cipher->IV(&iv_length);

    if (iv)
    {
        // cipher had an iv, create Java byte array for it
        ivToJava = aJni->NewByteArray(iv_length);
        if (ivToJava)
        {
            // copy iv to Java byte array
            aJni->SetByteArrayRegion(ivToJava,0,iv_length,(signed char*)iv);
        }
    }
    else
    {
        // iv is not applicable
        ivToJava = NULL;
    }

    // return iv to java
    return ivToJava;
} //  end getIV



// JNI function init.
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_crypto_CipherImpl__1init
(JNIEnv* aJni,
 jclass,
 jint aHandle,
 jint aOpMode,
 jstring aKeyAlgorithm,
 jstring aKeyFormat,
 jbyteArray aKeyEncoded,
 jbyteArray aParams)
{
    // return value
    jint retVal = 0;

    // Get the cipher object from aHandle
    STSCipher* cipher = reinterpret_cast< STSCipher* >(aHandle);

    // aOpMode can only be valid value because checked in Java side
    STSCipher::TCipherMode mode = (STSCipher::TCipherMode)aOpMode;

    if (aParams)
    {
        // Params was given as a parameter in Java side
        retVal = cipher->Init(aJni, mode, aKeyAlgorithm,aKeyFormat,aKeyEncoded, aParams);
    }
    else
    {
        // There is no params for the cipher.
        retVal = cipher->Init(aJni, mode, aKeyAlgorithm,aKeyFormat,aKeyEncoded);
    } // end if

    // retrun to Java side.
    return retVal;
}//end init



// JNI function create.
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_crypto_CipherImpl__1create
(JNIEnv* aJni,
 jclass,
 jstring aTransformation)
{

    // The cipher handle to be returned to java
    jint handle = KSTSErrNoMemory;
    int errCode = 0;

    // The cipher object that gets created.
    STSCipher* aCipher = NULL;

    // Create cipher object which uses the given algorithm
    aCipher = STSCipherFactory::CreateCipher(aJni, aTransformation, &errCode);

    if (aCipher == NULL)
    {
        // CreateCipher failed.
        // Set error code to handle which is returned to Java
        handle = errCode;
    }
    else
    {
        // return handle to new cipher object or error code
        handle = reinterpret_cast<jint>(aCipher);
    }
    return handle;

}//end create



// JNI function dispose.
JNIEXPORT void
JNICALL Java_com_nokia_mj_impl_crypto_CipherImpl__1dispose
(JNIEnv* /*aJni*/,
 jclass,
 jint aHandle)
{

    //Get the cipher object from aHandle
    STSCipher* cipher = reinterpret_cast< STSCipher* >(aHandle);

    if (NULL != cipher)
    {
        // Cleanup the native handle.
        delete cipher;
        cipher = 0;
    }

}//end dispose


//end of file


