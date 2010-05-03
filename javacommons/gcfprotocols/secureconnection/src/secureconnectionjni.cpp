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
* Description:  ?Description
 *
*/


#include "stdlib.h"
#include "nativesecureconnection.h"
#include "com_nokia_mj_impl_ssl_SecureConnectionImpl.h"

using namespace java;

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_ssl_SecureConnectionImpl__1createNativePeer
(JNIEnv* aJni, jobject /*peer*/, jstring aName, jint aMode, jstring aHost, jint aPort)
{
    const char* host = aJni->GetStringUTFChars(aHost, 0);
    const char* name = aJni->GetStringUTFChars(aName, 0);

    NativeSecureConnection* secSockConn = new NativeSecureConnection(name, aMode, host, aPort);

    aJni->ReleaseStringUTFChars(aHost, host);
    aJni->ReleaseStringUTFChars(aName, name);
    return reinterpret_cast<jint>(secSockConn);
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_ssl_SecureConnectionImpl__1doHandshake
(JNIEnv *aJni, jobject /*peer*/, jint aNativePeerHandle, jint aSocket, jint aType, jint aApn, jintArray  aReturnValue)
{
    int err1 = 0;
    int err2  = 0 ;
    NativeSecureConnection* secSockConn = reinterpret_cast<NativeSecureConnection*>(aNativePeerHandle);
    int ret = secSockConn->doHandshake(aSocket,aType, aApn, &err1,&err2);
    LOG1(ESOCKET, EInfo,"SecureConnectionJNI-dohandshake err= %d",err1);
    LOG1(ESOCKET, EInfo,"SecureConnectionJNI-validate err= %d",err2);
    //int *temp = &err;
    aJni->SetIntArrayRegion(aReturnValue,0,1,&err1);
    aJni->SetIntArrayRegion(aReturnValue,1,1,&err2);
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_ssl_SecureConnectionImpl__1getport
(JNIEnv* /*aJni*/, jobject /*peer*/, jint aNativePeerHandle)
{
    NativeSecureConnection* secSockConn = reinterpret_cast<NativeSecureConnection*>(aNativePeerHandle);

    // getPort method of NativeSocketConnection is called
    // As NativeSecureConnection class derives socket behaviour from NativeSocketConnection class
    return secSockConn->getPort();
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_ssl_SecureConnectionImpl__1getLocalPort
(JNIEnv* /*aJni*/, jobject /*peer*/, jint aNativePeerHandle)
{
    NativeSecureConnection* secSockConn = reinterpret_cast<NativeSecureConnection*>(aNativePeerHandle);

    // getLocalPort method of NativeSocketConnection is called
    // As NativeSecureConnection class derives socket behaviour from NativeSocketConnection class
    return secSockConn->getLocalPort();
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_ssl_SecureConnectionImpl__1getSocketOption
(JNIEnv* /*aJni*/, jobject /*peer*/, jint aNativePeerHandle, int aOption)
{

    NativeSecureConnection* secSockConn = reinterpret_cast<NativeSecureConnection*>(aNativePeerHandle);

    // getSocketOption method of NativeSocketConnection is called
    // As NativeSecureConnection class derives socket behaviour from NativeSocketConnection
    return secSockConn->getSocketOption(aOption);
}

JNIEXPORT int JNICALL
Java_com_nokia_mj_impl_ssl_SecureConnectionImpl__1setSocketOption
(JNIEnv* /*aJni*/, jobject /*peer*/, jint aNativePeerHandle, jint aOption, jint aValue)
{

    NativeSecureConnection* secSockConn = reinterpret_cast<NativeSecureConnection*>(aNativePeerHandle);

    // setSocketOption method of NativeSocketConnection is called
    // As NativeSecureConnection class derives socket behaviour from NativeSocketConnection
    return secSockConn->setSocketOption(aOption, aValue);

}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_ssl_SecureConnectionImpl__1getLocalAddress
(JNIEnv *aJni, jobject /*peer*/, jint aNativePeerHandle, jobjectArray address)
{
    NativeSecureConnection* secSockConn = reinterpret_cast<NativeSecureConnection*>(aNativePeerHandle);
    char * addr = new char[256];

    // getLocalAddress method of NativeSocketConnection is called
    // As NativeSecureConnection class derives socket behaviour from NativeSocketConnection
    jint retValue = secSockConn->getLocalAddress(addr);
    jstring jnistring = aJni->NewStringUTF(addr);
    aJni->SetObjectArrayElement(address,0,jnistring);
    aJni->DeleteLocalRef(jnistring);
    delete[] addr;
    return retValue;
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_ssl_SecureConnectionImpl__1getAddress
(JNIEnv *aJni, jobject /*peer*/, jint aNativePeerHandle, jobjectArray address)
{
    NativeSecureConnection* secSockConn = reinterpret_cast<NativeSecureConnection*>(aNativePeerHandle);
    char * addr = new char[256];

    // getAddress method of NativeSocketConnection is called
    // As NativeSecureConnection class derives socket behaviour from NativeSocketConnection
    jint retValue = secSockConn->getAddress(addr);
    jstring jnistring = aJni->NewStringUTF(addr);
    aJni->SetObjectArrayElement(address,0,jnistring);
    aJni->DeleteLocalRef(jnistring);
    delete[] addr;
    return retValue;

}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_ssl_SecureConnectionImpl__1close
(JNIEnv* /*aJni*/, jobject /*peer*/, jint aNativePeerHandle)
{
    NativeSecureConnection* secSockConn = reinterpret_cast<NativeSecureConnection*>(aNativePeerHandle);
    return secSockConn->secureSocketClose();
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_ssl_SecureConnectionImpl__1getSecurityInfo
(JNIEnv* aJni,jobject /*peer*/, jint aNativePeerHandle, jobjectArray aStringResultArray)
{
    NativeSecureConnection* secSockConn = reinterpret_cast<NativeSecureConnection*>(aNativePeerHandle);
    ILOG(ESOCKET, "++SecureConnectionJNI ");
    char **result = secSockConn->getSecurityInfo();
    jstring jniString;

    for (int i=0; i<9; i++)
    {
        ILOG1(ESOCKET, "SecureConnectionJNI result[i]= %s",result[i]);
        jniString = aJni->NewStringUTF(result[i]);
        aJni->SetObjectArrayElement(aStringResultArray, i, jniString);

    }
    aJni->DeleteLocalRef(jniString);
    return 1;
}

JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_ssl_SecureConnectionImpl__1dispose
(JNIEnv* /*aJni*/, jobject /*peer*/, jint aNativePeerHandle)
{
    NativeSocketConnection* nativeConn = reinterpret_cast<NativeSocketConnection*>(aNativePeerHandle);
    delete nativeConn;
}
