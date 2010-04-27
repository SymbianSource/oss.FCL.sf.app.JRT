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
* Description:  Serializer JNI wrapper.
 *
*/


//  INCLUDE FILES

#include "com_nokia_mj_impl_pim_Serializer.h"
#include <badesca.h>
#include "pimcommon.h"
#include "pimbasemanager.h"
#include "pimbaseserializer.h"
#include "pimbaseitem.h"
#include "pimutils.h"
#include "logger.h"

JNIEXPORT jint
JNICALL Java_com_nokia_mj_impl_pim_Serializer__1createSerializer(
    JNIEnv* aJniEnv, jobject /*aPeer*/, jint aManagerHandle,
    jintArray aError)
{
    JELOG2(EPim);
    pimbasemanager* manager =
        reinterpret_cast< pimbasemanager*>(aManagerHandle);
    pimbaseserializer* serializer = NULL;
    int error = 0;
    try
    {
        serializer = pimbaseserializer::getInstance(manager);
    }
    catch (int aError)
    {
        error = aError;
    }
    SetJavaErrorCode(aJniEnv, aError, error);

    return reinterpret_cast<int>(serializer);
}

JNIEXPORT void
JNICALL Java_com_nokia_mj_impl_pim_Serializer__1dispose(
    JNIEnv* /*aJniEnv*/, jobject /*aPeer*/, jint aSerializerHandle)
{
    JELOG2(EPim);
    pimbaseserializer* serializer =
        reinterpret_cast< pimbaseserializer *>(aSerializerHandle);

    delete serializer;
}

JNIEXPORT jintArray
JNICALL Java_com_nokia_mj_impl_pim_Serializer__1fromSerialFormat(
    JNIEnv* aJniEnv, jobject /*aPeer*/, jint aSerializerHandle,
    jbyteArray aBytes, jint aByteLength, jint aEncoding,
    jintArray aError)
{
    JELOG2(EPim);
    pimbaseserializer* serializer =
        reinterpret_cast< pimbaseserializer *>(aSerializerHandle);

    jintArray itemArray = serializer->fromSerialFormat(
                              aJniEnv, aBytes, aByteLength, aEncoding, aError);

    // The ownership of these items is transferred to the Java side,
    // to the caller of the method
    return itemArray;
}

JNIEXPORT jbyteArray
JNICALL Java_com_nokia_mj_impl_pim_Serializer__1toSerialFormat(
    JNIEnv* aJniEnv, jobject /*aPeer*/, jint aSerializerHandle,
    jint aItemHandle, jint aEncoding, jintArray aError)
{
    JELOG2(EPim);
    pimbaseserializer* serializer =
        reinterpret_cast< pimbaseserializer *>(aSerializerHandle);

    pimbaseitem* item =
        reinterpret_cast< pimbaseitem *>(aItemHandle);

    jbyteArray retVal = serializer->toSerialFormat(item,
                        aEncoding, aJniEnv, aError);

    // the byte array is owned on the java side
    return retVal;
}

JNIEXPORT jobjectArray
JNICALL Java_com_nokia_mj_impl_pim_Serializer__1supportedSerialFormats(
    JNIEnv* aJniEnv, jobject /*aPeer*/, jint aSerializerHandle,
    jint aPimListType, jintArray aError)
{
    JELOG2(EPim);
    pimbaseserializer* serializer =
        reinterpret_cast< pimbaseserializer *>(aSerializerHandle);

    TPIMListType listType = static_cast<TPIMListType>(aPimListType);

    jobjectArray retVal = serializer->supportedSerialFormats(
                              listType, aJniEnv, aError);

    return retVal;
}
