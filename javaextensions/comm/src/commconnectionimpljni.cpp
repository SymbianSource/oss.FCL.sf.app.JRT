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

#include "com_nokia_mj_impl_comm_CommConnectionImpl.h"
#include "nativecommconnection.h"
#include "javacommconnection.h"
#include "fs_methodcall.h"

using namespace java::comm;

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_comm_CommConnectionImpl__1createNativePeer
(JNIEnv *, jobject)
{
    JavaCommConnection *nativeObj = new JavaCommConnection();
    //NativeCommConnection *obj = new NativeCommConnection();
    NativeCommConnection *obj = nativeObj->getNativeObject();
    return reinterpret_cast<jint>(obj);
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_comm_CommConnectionImpl__1openCommportConnection
(JNIEnv *aJni, jobject, jint aNativePeerHandle, jstring aPort, jint aPortNumber, jintArray aIntegerOptions)
{
    NativeCommConnection *nativeConn = reinterpret_cast<NativeCommConnection*>(aNativePeerHandle);
    std::string port = (aJni)->GetStringUTFChars(aPort, 0);
    int *optionsList = new int[3];
    jint* jNativeBufferPtr = reinterpret_cast<jint*>(optionsList);
    aJni->GetIntArrayRegion(aIntegerOptions, 0, 3, jNativeBufferPtr);
    int ret = nativeConn->openPortConnection(port,aPortNumber,optionsList);
    delete[] optionsList;
    return ret;
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_comm_CommConnectionImpl__1setBaudRate
(JNIEnv *, jobject, jint aNativePeerHandle, jint aBaudRate)
{
    NativeCommConnection *nativeConn = reinterpret_cast<NativeCommConnection*>(aNativePeerHandle);
    int ret = nativeConn->setBaudRate(aBaudRate);
    return ret;

}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_comm_CommConnectionImpl__1getBaudRate
(JNIEnv *, jobject, jint aNativePeerHandle)
{
    NativeCommConnection *nativeConn = reinterpret_cast<NativeCommConnection*>(aNativePeerHandle);
    int ret = nativeConn->getBaudRate();
    return ret;

}

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_comm_CommConnectionImpl__1close
(JNIEnv *, jobject, jint aNativePeerHandle)
{
    NativeCommConnection *nativeConn = reinterpret_cast<NativeCommConnection*>(aNativePeerHandle);
    nativeConn->close();

}

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_comm_CommConnectionImpl__1dispose
(JNIEnv *, jobject, jint aNativePeerHandle)
{
    NativeCommConnection* nativeConn = reinterpret_cast<NativeCommConnection*>(aNativePeerHandle);
    delete nativeConn;
}
