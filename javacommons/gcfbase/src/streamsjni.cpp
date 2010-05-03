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
* Description:  Contains all JNI calls made by StreamConnectionBase
*
*/


#include "nativestreambase.h"
#include "com_nokia_mj_impl_gcf_utils_StreamConnectionBase.h"


using namespace java;

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_gcf_utils_StreamConnectionBase__1readbytes
(JNIEnv *aJni, jobject /*peer*/, jint nativePeer, jbyteArray aBytes)
{
    NativeStreamBase* nativeConn =
        reinterpret_cast<NativeStreamBase* >(nativePeer);

    return nativeConn->readBytes(*aJni, aBytes);
}


JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_gcf_utils_StreamConnectionBase__1writebytes
(JNIEnv *aJni, jobject /*peer*/, jint nativePeer, jbyteArray aBytes,
 int aOffset, int aLength)
{
    NativeStreamBase* nativeConn =
        reinterpret_cast<NativeStreamBase* >(nativePeer);

    return nativeConn->writeBytes(*aJni, aBytes, aOffset , aLength);
}


JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_gcf_utils_StreamConnectionBase__1stopreading
(JNIEnv* /*aJni*/, jobject /*peer*/, jint nativePeer)
{
    NativeStreamBase* nativeConn =
        reinterpret_cast<NativeStreamBase* >(nativePeer);

    nativeConn->stopReading();
}

JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_gcf_utils_StreamConnectionBase__1stopwriting
(JNIEnv* /*aJni*/, jobject /*peer*/, jint nativePeer)
{
    NativeStreamBase* nativeConn =
        reinterpret_cast<NativeStreamBase* >(nativePeer);

    nativeConn->stopWriting();
}

JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_gcf_utils_StreamConnectionBase__1setNativeBufferSize
(JNIEnv* /*aJni*/, jobject /*peer*/, jint nativePeer, jint size)
{
    NativeStreamBase* nativeConn =
        reinterpret_cast<NativeStreamBase* >(nativePeer);

    nativeConn->setBufferSize(size);
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_gcf_utils_StreamConnectionBase__1available
(JNIEnv* /*aJni*/, jobject /*peer*/, jint nativePeer)
{
    NativeStreamBase* nativeConn =
        reinterpret_cast<NativeStreamBase* >(nativePeer);

    return nativeConn->available();
}

