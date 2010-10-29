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
* Description:  Contains JNI calls made by ServerSocketConnectionImpl
*
*/


#include "com_nokia_mj_impl_socket_ServerSocketConnectionImpl.h"
#include "socketserverconnectionfactory.h"
#include "socketserverconnection.h"
#include "javacommonutils.h"
#include "logger.h"
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <wchar.h>
#include <dirent.h>

#ifdef __SYMBIAN32__
#include <e32def.h>
#include <e32cmn.h>
#endif // __SYMBIAN32__

#ifndef __SYMBIAN32__
#include <netinet/tcp.h>
#endif

#include "pushexception.h"
#include "pusherrorcodes.h"
#include "socketlocalhostinfo.h"
using namespace java::util;
using namespace java;

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_socket_ServerSocketConnectionImpl__1createSocketServerNativePeer
(JNIEnv *aJni, jobject /*peer*/, jstring aUrl, jint /*aMode*/, jint /*aPort*/, jintArray aFactoryHandle)
{
    JELOG2(ESOCKET);
    LOG(ESOCKET, EInfo, "+SocketServerConnectionJNI::createSocketServerNativePeer");
    const char* name = (aJni)->GetStringUTFChars(aUrl, 0);

    SocketServerConnectionFactory * scf = NULL;
    SocketServerConnection *iSocketServerPtr;
    std::wstring Url = JavaCommonUtils::utf8ToWstring((const char *)name);
    //NativeSocketServerConnection* nativeConn = new NativeSocketServerConnection(name, aMode, aPort);
    try
    {
        scf = &SocketServerConnectionFactory::getFactory();
        LOG(ESOCKET, EInfo, "-SocketServerConnectionJNI - getfactory created");
        iSocketServerPtr = reinterpret_cast<SocketServerConnection*>(scf->create(Url));
    }
    catch (PushException& ex)
    {
        ELOG(ESOCKET,"ERROR!!! CreateConnection() PushException was caught");
        int handle = reinterpret_cast<jint>(scf);
        aJni->SetIntArrayRegion(aFactoryHandle,0,1,&handle);
        if (scf != NULL)
        {
            // scf->releaseConnection(Url);
            scf = NULL;
            iSocketServerPtr = NULL;
        }
        (aJni)->ReleaseStringUTFChars(aUrl, name);
        return 0;
    }
    LOG(ESOCKET, EInfo, "-SocketServerConnectionJNI::createSocketServerNativePeer");
    int handle = reinterpret_cast<jint>(scf);
    aJni->SetIntArrayRegion(aFactoryHandle,0,1,&handle);
    (aJni)->ReleaseStringUTFChars(aUrl, name);

    return reinterpret_cast<jint>(iSocketServerPtr);
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_socket_ServerSocketConnectionImpl__1openServerSocket
(JNIEnv *aJni, jobject /*peer*/, jint nativePeerHandle, jint /*aSocketServerFactoryHandle*/, jstring aUrl)
{
    LOG(ESOCKET, EInfo, "+SocketServerConnectionJNI::openServerSocket");
    SocketServerConnection * iSocketServerPtr = reinterpret_cast<SocketServerConnection* >(nativePeerHandle);
    int retVal = -1;
    try
    {
        if (iSocketServerPtr != NULL)
        {
            int mSD = iSocketServerPtr->open();

            if (mSD < 0)
            {
                return -1;
            }
            retVal = mSD;
        }
    }
    catch (PushException& ex)
    {
        ELOG(ESOCKET,"ERROR!!! CreateConnection() PushException was caught");
        SocketServerConnectionFactory * SocketServerFactoryPtr = reinterpret_cast<SocketServerConnectionFactory* >(nativePeerHandle);
        const char* name = (aJni)->GetStringUTFChars(aUrl, 0);
        std::wstring Url = JavaCommonUtils::utf8ToWstring((const char *)name);
        if (SocketServerFactoryPtr != NULL)
        {
            //SocketServerFactoryPtr->releaseConnection(Url);
            SocketServerFactoryPtr = NULL;
            iSocketServerPtr = NULL;
        }
        (aJni)->ReleaseStringUTFChars(aUrl, name);
        if (PUSH_CONNECTION_ALREADY_EXISTS == ex.mErrCode)
        {
            LOG1(ESOCKET,EInfo,"PushConnection already Exists : %d",ex.mErrCode);
            retVal = ex.mErrCode;
        }
        if (COMMON_SRV_CONN_PLUGIN_ERROR == ex.mErrCode)
        {
            LOG1(ESOCKET,EInfo,"Common pushConnection error : %d",ex.mErrCode);
            retVal = ex.mErrCode;
        }
        else
        {
            retVal = -1;
        }
    }
    return retVal;
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_socket_ServerSocketConnectionImpl__1getLocalAddress
(JNIEnv *aJni, jobject /*peer*/, jint /*nativePeerHandle*/, jint aSockDesc, jint aMidletIapId, jint aApType, jobjectArray address)
{
    JELOG2(ESOCKET);
    char *addr = new char[50];
    int err = SocketLocalHostInfo::getLocalAddress(aSockDesc,addr,aMidletIapId,aApType);
    PLOG2(ESOCKET, "getLocalAddress, ret = %d, addr = %s" , err, addr);
    jstring jnistring = aJni->NewStringUTF(addr);
    aJni->SetObjectArrayElement(address, 0, jnistring);
    delete[] addr;
    return err;
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_socket_ServerSocketConnectionImpl__1getLocalPort
(JNIEnv* /*aJni*/, jobject /*peer*/, jint /*nativePeerHandle*/, jint aSockDesc)
{
    JELOG2(ESOCKET);
    //NativeSocketServerConnection* nativeConn    =  reinterpret_cast<NativeSocketServerConnection* > (nativePeerHandle);

    //return nativeConn->getLocalPort();

    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    if (getsockname(aSockDesc, (struct sockaddr*)&addr, &len) == -1)
    {
        LOG1(ESOCKET, EInfo, "SocketServerConnectionJNI::getLocalPort() : getlocalport failed : %d", errno);
        return -errno;
    }

    return ntohs(addr.sin_port);
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_socket_ServerSocketConnectionImpl__1accept
(JNIEnv* /*aJni*/, jobject /*peer*/, jint nativePeerHandle)
{
    JELOG2(ESOCKET);
    LOG(ESOCKET, EInfo, "+SocketServerConnectionJNI::_accept");
    //NativeSocketServerConnection* nativeConn = reinterpret_cast<NativeSocketServerConnection* > (nativePeerHandle);
    //return nativeConn->accept();
    SocketServerConnection * iSocketServerPtr = reinterpret_cast<SocketServerConnection* >(nativePeerHandle);
    LOG(ESOCKET, EInfo, "-SocketServerConnectionJNI::_accept");
    if (iSocketServerPtr != NULL)
    {
        return iSocketServerPtr->accept();
    }
    return -1;
}

JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_socket_ServerSocketConnectionImpl__1close
(JNIEnv *aJni, jobject /*peer*/, jint nativePeerHandle, jint aSocketServerFactoryHandle, jstring aUrl, jint aSockDesc)
{
    JELOG2(ESOCKET);
    LOG(ESOCKET, EInfo, "+SocketServerConnectionJNI::_close");

    const char* name = (aJni)->GetStringUTFChars(aUrl, 0);
    std::wstring Url = JavaCommonUtils::utf8ToWstring((const char *)name);
    SocketServerConnectionFactory * SocketServerFactoryPtr = reinterpret_cast<SocketServerConnectionFactory* >(aSocketServerFactoryHandle);
    if (SocketServerFactoryPtr != NULL)
    {
        if (Url.length() != 10) // not dynamic url case
        {
            SocketServerFactoryPtr->releaseConnection(Url);
            SocketServerFactoryPtr = NULL;
        }
        else  // dynamic url case, dont release socket://: url
        {
            SocketServerConnection * iSocketServerPtr = reinterpret_cast<SocketServerConnection* >(nativePeerHandle);
            if (iSocketServerPtr != NULL)
            {
                if (iSocketServerPtr->isNormalServerConnection() == true)
                {
                    iSocketServerPtr->close(aSockDesc); // just close the socket in case of normal connection
                }
                else // in case the dynamic url is registered for push, then release the connection
                {
                    SocketServerFactoryPtr->releaseConnection(Url);
                    SocketServerFactoryPtr = NULL;
                    iSocketServerPtr = NULL;
                }
            }
        }

    }
    (aJni)->ReleaseStringUTFChars(aUrl, name);
    return 0;
}
