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
* Description:  Contains JNI calls made by SocketConnectionImpl
*
*/


#include "com_nokia_mj_impl_socket_SocketConnectionImpl.h"
#include "nativesocketconnection.h"


using namespace java;

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_socket_SocketConnectionImpl__1createNativePeer
(JNIEnv *aJni, jobject /*peer*/, jstring aName, jint aMode, jstring aHost, jint aPort)
{
    //LOG1(ESOCKET,EInfo,"host length = %d", (aJni)->GetStringLength(aHost));
    const char* host;
    NativeSocketConnection* nativeConn;
    const char* name = (aJni)->GetStringUTFChars(aName, 0);

    if (aHost == NULL)
    {
        LOG(ESOCKET,EInfo,"host is null");
        host = "";

    }
    else
    {
        host = (aJni)->GetStringUTFChars(aHost, 0);
    }

    nativeConn = new NativeSocketConnection(name, aMode, host, aPort);

    // Delete the memory
    (aJni)->ReleaseStringUTFChars(aName, name);
    if (aHost != NULL)
    {
        (aJni)->ReleaseStringUTFChars(aHost, host);
    }

    return reinterpret_cast<jint>(nativeConn);
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_socket_SocketConnectionImpl__1openSocket
(JNIEnv *aJni, jobject /*peer*/, jint aNativePeerHandle, jint aSock,jint aType, jint aApn, jintArray  aReturnValue)
{
    int err  = 0;
    NativeSocketConnection *nativeConn = reinterpret_cast<NativeSocketConnection* >(aNativePeerHandle);
    int ret = nativeConn->socketOpen(aSock,aType,aApn,&err);
    aJni->SetIntArrayRegion(aReturnValue,0,1,&err);
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_socket_SocketConnectionImpl__1getport
(JNIEnv* /*aJni*/, jobject /*peer*/, jint aNativePeerHandle)
{
    NativeSocketConnection* nativeConn = reinterpret_cast<NativeSocketConnection* >(aNativePeerHandle);
    return nativeConn->getPort();
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_socket_SocketConnectionImpl__1getLocalPort
(JNIEnv* /*aJni*/, jobject /*peer*/, jint aNativePeerHandle)
{
    NativeSocketConnection* nativeConn = reinterpret_cast<NativeSocketConnection* >(aNativePeerHandle);
    return nativeConn->getLocalPort();
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_socket_SocketConnectionImpl__1getSocketOption
(JNIEnv* /*aJni*/, jobject /*peer*/, jint aNativePeerHandle, jint aOption)
{
    NativeSocketConnection* nativeConn = reinterpret_cast<NativeSocketConnection* >(aNativePeerHandle);
    return nativeConn->getSocketOption(aOption);
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_socket_SocketConnectionImpl__1setSocketOption
(JNIEnv* /*aJni*/, jobject /*peer*/, jint aNativePeerHandle, jint aOption, jint aValue)
{
    NativeSocketConnection* nativeConn = reinterpret_cast<NativeSocketConnection* >(aNativePeerHandle);
    return nativeConn->setSocketOption(aOption, aValue);
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_socket_SocketConnectionImpl__1getLocalAddress
(JNIEnv *aJni, jobject /*peer*/, jint aNativePeerHandle, jobjectArray address)
{
    NativeSocketConnection* nativeConn = reinterpret_cast<NativeSocketConnection* >(aNativePeerHandle);
    char * addr = new char[256];
    jint retValue = nativeConn->getLocalAddress(addr);
    jstring jnistring = aJni->NewStringUTF(addr);
    aJni->SetObjectArrayElement(address,0,jnistring);
    aJni->DeleteLocalRef(jnistring);
    delete[] addr;
    return retValue;
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_socket_SocketConnectionImpl__1getAddress
(JNIEnv *aJni, jobject /*peer*/, jint aNativePeerHandle, jobjectArray address)
{
    NativeSocketConnection* nativeConn = reinterpret_cast<NativeSocketConnection* >(aNativePeerHandle);
    char * addr = new char[256];
    jint retValue = nativeConn->getAddress(addr);
    jstring jnistring = aJni->NewStringUTF(addr);
    aJni->SetObjectArrayElement(address,0,jnistring);
    aJni->DeleteLocalRef(jnistring);
    delete[] addr;
    return retValue;
}


JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_socket_SocketConnectionImpl__1close
(JNIEnv* /*aJni*/, jobject /*peer*/, jint aNativePeerHandle)
{
    NativeSocketConnection* nativeConn = reinterpret_cast<NativeSocketConnection* >(aNativePeerHandle);
    return nativeConn->socketClose();
}

JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_socket_SocketConnectionImpl__1dispose
(JNIEnv* /*aJni*/, jobject /*peer*/, jint aNativePeerHandle)
{
    NativeSocketConnection* nativeConn = reinterpret_cast<NativeSocketConnection* >(aNativePeerHandle);
    delete nativeConn;
}
