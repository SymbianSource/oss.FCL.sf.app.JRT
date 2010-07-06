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
* Description:
 *
*/


#include "com_nokia_mj_impl_datagram_UDPDatagramConnectionImpl.h"
#include "stdlib.h"
#include "string.h"
#include "nativedatagramconnection.h"
#include "logger.h"
#include "pushexception.h"
#include "pusherrorcodes.h"
#include "socketlocalhostinfo.h"

using namespace java;

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_datagram_UDPDatagramConnectionImpl__1createNativePeer
(JNIEnv* aJni, jobject /*peer*/, jstring aName, jint aMode, jstring aHost, jint aPort)
{
    const char* name = aJni->GetStringUTFChars(aName, 0);
    const char* host = aJni->GetStringUTFChars(aHost, 0);
    NativeDatagramConnection* datagramConn = new NativeDatagramConnection(name, aMode, host, aPort);

    // Delete the memory
    (aJni)->ReleaseStringUTFChars(aName, name);
    (aJni)->ReleaseStringUTFChars(aHost, host);
    return reinterpret_cast<jint>(datagramConn);

}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_datagram_UDPDatagramConnectionImpl__1openConnection
(JNIEnv* aJni, jobject /*peer*/, jint nativePeerHandle, jint aType, jint aApn, jintArray  aReturnValue)
{
    int err  = 0;
    NativeDatagramConnection* datagramConn = reinterpret_cast<NativeDatagramConnection*>(nativePeerHandle);
    try
    {
        int retVal = datagramConn->openConnection(aType, aApn, &err);
        LOG1(ESOCKET,EInfo,"DatagramConnectionJni : openconnection return value : %d",retVal);
        aJni->SetIntArrayRegion(aReturnValue,0,1,&err);
        return retVal;
    }
    catch (PushException& ex)
    {
        ELOG(ESOCKET,"ERROR!!! CreateConnection() PushException was caught");
        if (datagramConn!=NULL)
        {
            datagramConn->datagramClose();
            delete datagramConn;
            datagramConn = NULL;
        }
        return ex.mErrCode;
    }
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_datagram_UDPDatagramConnectionImpl__1send
(JNIEnv *aJni, jobject, jint nativePeerHandle, jbyteArray aJavaBuffer, jint aOffset, jint aLength, jstring aDestAddr, jint aDestPort)
{
    const char* destaddr = aJni->GetStringUTFChars(aDestAddr, 0);
    NativeDatagramConnection* datagramConn = reinterpret_cast<NativeDatagramConnection*>(nativePeerHandle);
    int retVal = datagramConn->sendDatagram(*aJni,aJavaBuffer,aOffset,aLength,destaddr,aDestPort);
    aJni->ReleaseStringUTFChars(aDestAddr,destaddr);
    return retVal;
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_datagram_UDPDatagramConnectionImpl__1receive
(JNIEnv *aJni, jobject, jint nativePeerHandle, jbyteArray aJavaBuffer, jint aOffset, jint aLength, jobjectArray aSenderAddr, jintArray aSenderPort)
{
    int port;
    jstring jnistring;
    char * addr = new char[256];
    LOG1(ESOCKET,EInfo,"Datagram JNI, receive called len = %d", aLength);
    NativeDatagramConnection* datagramConn = reinterpret_cast<NativeDatagramConnection*>(nativePeerHandle);
    int retVal = datagramConn->receiveDatagram(*aJni,aJavaBuffer,aOffset,aLength,addr,&port);
    LOG1(ESOCKET,EInfo,"addr = %s",addr);

    jnistring = aJni->NewStringUTF(addr);
    aJni->SetObjectArrayElement(aSenderAddr,0,jnistring);

    aJni->SetIntArrayRegion(aSenderPort,0,1,&port);
    LOG1(ESOCKET,EInfo,"Datagram JNI, receive called ", 2);
    LOG1(ESOCKET,EInfo,"Datagram JNI, receive called , offset %d", aOffset);
    LOG1(ESOCKET,EInfo,"Datagram JNI, receive called , len %d", aLength);

    LOG1(ESOCKET,EInfo,"Datagram JNI, receive, returning ", 2);
    return retVal;
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_datagram_UDPDatagramConnectionImpl__1getLocalAddress
(JNIEnv *aJni, jobject, jint /* nativePeerHandle */, jobjectArray aAddress, jint aMidletIapId, jint aApType)
{
    char * addr = new char[256];

    int err = SocketLocalHostInfo::getLocalAddress(0,addr,aMidletIapId,aApType);
    PLOG2(ESOCKET, "getLocalAddress, ret = %d, addr = %s" , err, addr);
    jstring jnistring = aJni->NewStringUTF(addr);
    aJni->SetObjectArrayElement(aAddress, 0, jnistring);
    aJni->DeleteLocalRef(jnistring);
    delete[] addr;

    return err;
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_datagram_UDPDatagramConnectionImpl__1getLocalPort
(JNIEnv* /*aJni*/, jobject, jint nativePeerHandle)
{
    NativeDatagramConnection * datagramConn = reinterpret_cast<NativeDatagramConnection *>(nativePeerHandle);
    return datagramConn->getLocalPort();
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_datagram_UDPDatagramConnectionImpl__1close
(JNIEnv* /*aJni*/, jobject, jint nativePeerHandle)
{
    NativeDatagramConnection * datagramConn = reinterpret_cast<NativeDatagramConnection *>(nativePeerHandle);
    return datagramConn->datagramClose();
}

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_datagram_UDPDatagramConnectionImpl__1dispose
(JNIEnv* /*aJni*/, jobject, jint nativePeerHandle)
{
    NativeDatagramConnection * datagramConn = reinterpret_cast<NativeDatagramConnection *>(nativePeerHandle);
    delete datagramConn;
}
