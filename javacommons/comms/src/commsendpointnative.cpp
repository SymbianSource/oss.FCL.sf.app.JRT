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
* Description:  Comms java to native method call implementations
*
*/

#include <sstream>
#include <errno.h>

#include "logger.h"
#include "javajniutils.h"
#include "javacommonutils.h"

#include "com_nokia_mj_impl_comms_CommsEndpoint.h"
#include "com_nokia_mj_impl_comms_CommsEndpointBase.h"
#include "com_nokia_mj_impl_comms_CommsServerEndpoint.h"
#include "commsclientendpoint.h"
#include "commsserverendpoint.h"
#include "commsmessage.h"
#include "commscontext.h"

using namespace java::comms;
using namespace java::util;

// CommsEndpointBase methods

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_comms_CommsEndpointBase__1registerlistener(
    JNIEnv* aEnv,
    jobject,
    jint aHandle,
    jint aModuleId,
    jobject aListener)
{
    JELOG2(EJavaComms);
    CommsEndpoint* endpoint = reinterpret_cast<CommsEndpoint*>(aHandle);
    return endpoint->registerJavaListener(aModuleId, aListener, aEnv);
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_comms_CommsEndpointBase__1unregisterlistener(
    JNIEnv* aEnv,
    jobject,
    jint aHandle,
    jint aModuleId)
{
    JELOG2(EJavaComms);
    CommsEndpoint* endpoint = reinterpret_cast<CommsEndpoint*>(aHandle);
    return endpoint->unregisterJavaListener(aModuleId, 0, aEnv);
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_comms_CommsEndpointBase__1send(
    JNIEnv* aEnv,
    jobject,
    jint aHandle,
    jbyteArray aArray)
{
    JELOG2(EJavaComms);
    int len = aEnv->GetArrayLength(aArray);

    char* buf = new char[len];
    aEnv->GetByteArrayRegion(aArray, 0, len, (jbyte*)buf);
    ipcMessage_t* msg = reinterpret_cast<ipcMessage_t*>(buf);
    msg->ipcHeader.length = len;
    CommsMessage message(msg);
    delete[] buf;

    CommsEndpoint* endpoint = reinterpret_cast<CommsEndpoint*>(aHandle);
    return endpoint->send(message);
}


JNIEXPORT jbyteArray JNICALL Java_com_nokia_mj_impl_comms_CommsEndpointBase__1sendreceive(
    JNIEnv* aEnv,
    jobject,
    jint aHandle,
    jbyteArray aArray,
    jint aTimeout)
{
    JELOG2(EJavaComms);
    int len = aEnv->GetArrayLength(aArray);

    char* buf = new char[len];
    aEnv->GetByteArrayRegion(aArray, 0, len, (jbyte*)buf);
    ipcMessage_t* msg = reinterpret_cast<ipcMessage_t*>(buf);
    msg->ipcHeader.length = len;
    CommsMessage message(msg);
    delete[] buf;

    CommsEndpoint* endpoint = reinterpret_cast<CommsEndpoint*>(aHandle);
    if (endpoint)
    {
        CommsMessage receivedMessage;

        int rc = endpoint->sendReceive(message, receivedMessage, aTimeout);
        if (!rc)
        {
            char* arr = receivedMessage.toByteArray();
            int len = (reinterpret_cast<ipcMessage_t*>(arr))->ipcHeader.length;
            jbyteArray bytes = aEnv->NewByteArray(len);
            aEnv->SetByteArrayRegion(bytes, 0, len, (jbyte*)arr);
            return bytes;
        }
        else
        {
            ELOG1(EJavaComms, "Java_com_nokia_mj_impl_comms_CommsEndpointBase__1sendreceive() failed (%d)", rc);
            std::stringstream msg;
            msg << "CommsEndpoint.sendReceive() failed, error code=" << rc;

            if (rc == ETIMEDOUT || rc == EINTR)
            {
                JniUtils::throwNewException(aEnv,"java/lang/InterruptedException", msg.str());
            }
            else
            {
                JniUtils::throwNewException(aEnv,"java/io/IOException", msg.str());
            }
        }
    }

    return 0;
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_comms_CommsEndpointBase__1registerdefaultlistener(
    JNIEnv* aEnv,
    jobject,
    jint aHandle,
    jobject aListener)
{
    JELOG2(EJavaComms);
    CommsEndpoint* endpoint = reinterpret_cast<CommsEndpoint*>(aHandle);
    return endpoint->registerDefaultJavaListener(aListener, aEnv);
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_comms_CommsEndpointBase__1unregisterdefaultlistener(
    JNIEnv* aEnv,
    jobject,
    jint aHandle)
{
    JELOG2(EJavaComms);
    CommsEndpoint* endpoint = reinterpret_cast<CommsEndpoint*>(aHandle);
    return endpoint->unregisterDefaultJavaListener(0, aEnv);
}

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_comms_CommsEndpointBase__1release(
    JNIEnv*,
    jobject,
    jint aHandle)
{
    JELOG2(EJavaComms);
    CommsEndpoint* endpoint = reinterpret_cast<CommsEndpoint*>(aHandle);
    delete endpoint;
}

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_comms_CommsEndpointBase__1detach(
    JNIEnv* aEnv,
    jobject,
    jint aHandle)
{
    JELOG2(EJavaComms);
    CommsEndpoint* endpoint = reinterpret_cast<CommsEndpoint*>(aHandle);
    endpoint->detachFromVm(aEnv);
}

// CommsEndpoint methods

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_comms_CommsEndpoint__1create(
    JNIEnv* aEnv,
    jobject)
{
    JELOG2(EJavaComms);
    CommsClientEndpoint* client = new CommsClientEndpoint();
    client->attachToVm(aEnv);
    return (jint)client;
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_comms_CommsEndpoint__1find(
    JNIEnv* aEnv,
    jclass,
    jstring aName)
{
    JELOG2(EJavaComms);
    std::wstring name = JniUtils::jstringToWstring(aEnv, aName);
    CommsClientEndpoint* endpoint = CommsContext::getContext().find(name);

    jint rc = 0;
    if (endpoint)
    {
        endpoint->attachToVm(aEnv);
        rc = (jint)endpoint;
    }
    else
    {
        WLOG1WSTR(EJavaComms, "Find client endpoint failed, no such endpoint=%s", name);
    }
    return rc;
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_comms_CommsEndpoint__1connect(
    JNIEnv* aEnv,
    jobject,
    jint aHandle,
    jint aAddress)
{
    JELOG2(EJavaComms);
    CommsClientEndpoint* client = reinterpret_cast<CommsClientEndpoint*>(aHandle);
    client->attachToVm(aEnv);
    jint rc =client->connect(aAddress);
    return rc;
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_comms_CommsEndpoint__1disconnect(
    JNIEnv*,
    jobject,
    jint aHandle)
{
    JELOG2(EJavaComms);
    CommsClientEndpoint* client = reinterpret_cast<CommsClientEndpoint*>(aHandle);
    jint rc = client->disconnect();
    return rc;
}

// CommsServerEndpoint methods

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_comms_CommsServerEndpoint__1create(
    JNIEnv* aEnv,
    jobject)
{
    JELOG2(EJavaComms);
    CommsServerEndpoint* server = new CommsServerEndpoint();
    server->attachToVm(aEnv);
    return (jint)server;
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_comms_CommsServerEndpoint__1find(
    JNIEnv* aEnv,
    jclass,
    jstring aName)
{
    JELOG2(EJavaComms);
    std::wstring name = JniUtils::jstringToWstring(aEnv, aName);
    CommsServerEndpoint* server = CommsContext::getContext().findServer(name);
    jint rc = 0;
    if (server)
    {
        server->attachToVm(aEnv);
        rc = (jint)server;
    }
    else
    {
        WLOG1WSTR(EJavaComms, "Find server endpoint failed, no such endpoint=%s", name);
    }
    return rc;
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_comms_CommsServerEndpoint__1start(
    JNIEnv* aEnv,
    jobject,
    jint aHandle,
    jint aAddress)
{
    JELOG2(EJavaComms);
    CommsServerEndpoint* server = reinterpret_cast<CommsServerEndpoint*>(aHandle);
    server->attachToVm(aEnv);
    jint rc = server->start(aAddress);
    return rc;
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_comms_CommsServerEndpoint__1stop(
    JNIEnv*,
    jobject,
    jint aHandle)
{
    JELOG2(EJavaComms);
    CommsServerEndpoint* server = reinterpret_cast<CommsServerEndpoint*>(aHandle);
    jint rc = server->stop();
    return rc;
}

