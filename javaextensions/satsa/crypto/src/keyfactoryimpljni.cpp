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
* Description:  JNI Layer corresponding to KeyFactory.java
 *
*/


#include "com_nokia_mj_impl_security_KeyFactoryImpl.h"

#include "stssignaturefactory.h"

using namespace java::satsa;

/**
 * JNI function.
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_security_KeyFactoryImpl__1isValidAlgorithm
(JNIEnv* aJni, jclass, jstring aAlgorithm)
{

    // KeyFactory can only be used with signatures.
    return STSSignatureFactory::IsSupportedAlgorithm(aJni,
            aAlgorithm);
}

/**
 * JNI function.
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_security_KeyFactoryImpl__1isValidKey
(JNIEnv* aJni, jclass,
 jstring aKeyAlgorithm,
 jstring aKeyFormat,
 jbyteArray aKeyEncoded)
{

    jint retVal = 0;

    // Call InitVerify
    retVal = STSSignatureFactory::IsValidKey(aJni,
             aKeyAlgorithm,
             aKeyFormat,
             aKeyEncoded);

    return retVal;
}

