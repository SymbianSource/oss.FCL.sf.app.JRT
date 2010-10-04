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

#include <stdlib.h>
#include "logger.h"
#include "javajniutils.h"
#include "exceptionbase.h"
#include "pushexception.h"
#include "pusherrorcodes.h"
#include "javacommonutils.h"
#include "smsconnection.h"
#include "com_nokia_mj_impl_sms_SMSConnectionImpl.h"

using namespace java::wma;
using namespace java::util;
/*
 * Class:     com_nokia_mj_impl_sms_SMSConnectionImpl
 * Method:    _createConnection
 * Signature: (Ljava/lang/String;Z)J
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_sms_SMSConnectionImpl__1createConnection
(JNIEnv* aJni, jobject ,jstring aUri,jboolean aServerConnection)
{
    JELOG2(EWMA);
    std::wstring Uri = java::util::JniUtils::jstringToWstring(aJni,aUri);

    SmsConnection* smsConn = 0;
    try
    {
        smsConn = new SmsConnection(Uri, aServerConnection);
        smsConn->initialize();
        return reinterpret_cast<jint>(smsConn);
    }
    catch (java::push::PushException& ex)
    {
        ELOG(EWMA,"WMA ERROR!!! CreateConnection() PushException was caught");
        if (java::push::PUSH_CONNECTION_ALREADY_EXISTS == ex.mErrCode)
        {
            ELOG(EWMA,"WMA : PushConnection already Exists");
            JniUtils::throwNewException(aJni, "java/io/IOException" ,
                                        "Server Connection already exists");
        }
        if (java::push::COMMON_SRV_CONN_PLUGIN_ERROR == ex.mErrCode)
        {
            ELOG(EWMA,"WMA : Common pushConnection error");
            JniUtils::throwNewException(aJni, "java/io/IOException" ,
                                        "Opening Server Connection failed: "
                                        + JavaCommonUtils::intToString(ex.mErrCode));
        }
    }
    catch (std::bad_alloc &ex)
    {
        ELOG(EWMA,"WMA : Caught bad alloc exception");
        JniUtils::throwNewException(aJni, "java/lang/OutOfMemoryError" ,
                                    "Out of memory");
    }
    catch (ExceptionBase &ex)
    {
        ELOG(EWMA,"WMA : Caught an ExceptionBase exception");
        JniUtils::throwNewException(aJni, "java/io/IOException" ,
                                    "Opening connection failed");
    }
    catch (int error)
    {
        ELOG1(EWMA,"+ WMA : Caught an Exception --Value is:%d",error);
        JniUtils::throwNewException(aJni, "java/io/IOException" ,
                                    "Opening connection failed: " + JavaCommonUtils::intToString(error));
    }
    if (0 != smsConn)
    {
        delete smsConn;
    }
    return 0;
}

/*
 * Class:     com_nokia_mj_impl_sms_SMSConnectionImpl
 * Method:    _openConnection
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_sms_SMSConnectionImpl__1openConnection
(JNIEnv *aJni, jobject aPeer,jint aHandle)
{
    JELOG2(EWMA);
    SmsConnection* msgConn = reinterpret_cast<SmsConnection*>(aHandle);
    return msgConn->open(*aJni,aPeer);
}

/*
 * Class:     com_nokia_mj_impl_sms_SMSConnectionImpl
 * Method:    _smsTextDataSegments
 * Signature: (JILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_sms_SMSConnectionImpl__1smsTextDataSegments
(JNIEnv* aJni, jobject , jint aHandle, jint aType,jstring aHostAddress,
 jstring aHostPort, jstring aData)
{
    JELOG2(EWMA);
    jint numberOfSegments = 0;
    int hostPort = 0;
    const char* hostaddress = aJni->GetStringUTFChars(aHostAddress,0);
    if (aHostPort)
    {
        const char* port = aJni->GetStringUTFChars(aHostPort,0);
        hostPort = atoi(port);
        aJni->ReleaseStringUTFChars(aHostPort,port);
    }
    char* messageData = 0;
    jint len = 0;
    if (aData != NULL)
    {
        const jchar* data = aJni->GetStringChars(aData,0);
        len = aJni->GetStringLength(aData);
        messageData = new char[len*2 + 1];
        memset(messageData,0,len*2 + 1);
        memcpy((void*)messageData,(void*)data,len*2);
        aJni->ReleaseStringChars(aData,data);
    }
    SmsConnection* msgConn = reinterpret_cast<SmsConnection*>(aHandle);
    try
    {
        numberOfSegments = msgConn->numberOfDataSegments(aType,len,hostPort,
                           hostaddress,messageData);
    }
    catch (ExceptionBase &ex)
    {
        JniUtils::throwNewException(aJni, "java/lang/RuntimeException" ,
                                    "Calculating number of segments failed");
    }
    if (numberOfSegments < 0)
    {
        JniUtils::throwNewException(aJni, "java/lang/RuntimeException" ,
                                    "Calculating number of message segments failed: " +
                                    JavaCommonUtils::intToString(numberOfSegments));
    }
    aJni->ReleaseStringUTFChars(aHostAddress,hostaddress);
    delete[] messageData;
    return numberOfSegments;
}

/*
 * Class:     com_nokia_mj_impl_sms_SMSConnectionImpl
 * Method:    _smsBinaryDataSegments
 * Signature: (JILjava/lang/String;Ljava/lang/String;[B)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_sms_SMSConnectionImpl__1smsBinaryDataSegments
(JNIEnv* aJni, jobject , jint aHandle, jint aType,jstring aHostAddress,
 jstring aHostPort, jbyteArray aData)
{
    JELOG2(EWMA);
    jint numberOfSegments = 0;
    int hostPort = 0;
    char* nativeString = 0;
    int length = 0;
    const char* hostaddress = aJni->GetStringUTFChars(aHostAddress,0);
    if (aHostPort)
    {
        const char* port = aJni->GetStringUTFChars(aHostPort,0);
        hostPort = atoi(port);
        aJni->ReleaseStringUTFChars(aHostPort,port);
    }
    if (aData != NULL)
    {
        length = aJni->GetArrayLength(aData);
        nativeString = new char[length + 1];
        memset(nativeString, 0, length + 1);
        aJni->GetByteArrayRegion(aData,0,length,(jbyte *)nativeString);
        aJni->DeleteLocalRef(aData);
    }
    SmsConnection* msgConn = reinterpret_cast<SmsConnection*>(aHandle);
    try
    {
        numberOfSegments = msgConn->numberOfDataSegments(aType,length,hostPort,
                           hostaddress,(const char*)nativeString);
    }
    catch (ExceptionBase &ex)
    {
        JniUtils::throwNewException(aJni, "java/lang/RuntimeException" ,
                                    "Calculating number of segments failed");
    }
    if (numberOfSegments < 0)
    {
        JniUtils::throwNewException(aJni, "java/lang/RuntimeException" ,
                                    "Calculating number of message segments failed: " +
                                    JavaCommonUtils::intToString(numberOfSegments));

    }
    aJni->ReleaseStringUTFChars(aHostAddress,hostaddress);
    delete[] nativeString;
    return numberOfSegments;

}

/*
 * Class:     com_nokia_mj_impl_sms_SMSConnectionImpl
 * Method:    _sendMessage
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_sms_SMSConnectionImpl__1sendMessage
(JNIEnv* , jobject , jint aHandle)
{
    JELOG2(EWMA);
    SmsConnection* msgConn = reinterpret_cast<SmsConnection*>(aHandle);
    return msgConn->send();
}

/*
 * Class:     com_nokia_mj_impl_sms_SMSConnectionImpl
 * Method:    _getMessageType
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_sms_SMSConnectionImpl__1getMessageType
(JNIEnv *, jobject,jint aHandle)
{
    JELOG2(EWMA);
    SmsConnection* msgConn = reinterpret_cast<SmsConnection*>(aHandle);
    return msgConn->getMessageType();
}

/*
 * Class:     com_nokia_mj_impl_sms_SMSConnectionImpl
 * Method:    _getHostAddress
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_sms_SMSConnectionImpl__1getHostAddress
(JNIEnv *aJni, jobject,jint aHandle)
{
    JELOG2(EWMA);
    SmsConnection* msgConn = reinterpret_cast<SmsConnection*>(aHandle);
    return msgConn->getRemoteHostAddress(*aJni);
}

/*
 * Class:     com_nokia_mj_impl_sms_SMSConnectionImpl
 * Method:    _getTimestamp
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_nokia_mj_impl_sms_SMSConnectionImpl__1getTimestamp
(JNIEnv *, jobject,jint aHandle)
{
    JELOG2(EWMA);
    SmsConnection* msgConn = reinterpret_cast<SmsConnection*>(aHandle);
    return msgConn->getTimestamp();
}

/*
 * Class:     com_nokia_mj_impl_sms_SMSConnectionImpl
 * Method:    _getReceivedMessageData
 * Signature: (JI)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_nokia_mj_impl_sms_SMSConnectionImpl__1getReceivedMessageData
(JNIEnv *aJni, jobject,jint aHandle,jint aMessageType)
{
    JELOG2(EWMA);
    SmsConnection* msgConn = reinterpret_cast<SmsConnection*>(aHandle);
    return msgConn->getData(*aJni,aMessageType);
}

/*
 * Class:     com_nokia_mj_impl_sms_SMSConnectionImpl
 * Method:    _closeConnection
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_sms_SMSConnectionImpl__1closeConnection
(JNIEnv *, jobject,jint aHandle)
{
    JELOG2(EWMA);
    SmsConnection* msgConn = reinterpret_cast<SmsConnection*>(aHandle);
    msgConn->close();
}

/*
 * Class:     com_nokia_mj_impl_sms_SMSConnectionImpl
 * Method:    _dispose
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_sms_SMSConnectionImpl__1dispose
(JNIEnv *, jobject,jint aHandle)
{
    JELOG2(EWMA);
    SmsConnection* msgConn = reinterpret_cast<SmsConnection*>(aHandle);
    delete msgConn;
}

/*
 * Class:     com_nokia_mj_impl_sms_SMSConnectionImpl
 * Method:    _checkError
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_sms_SMSConnectionImpl__1checkError
(JNIEnv *aJni, jobject , jint aError , jboolean aIsSendOperation)
{
    JELOG2(EWMA);
#ifdef __SYMBIAN32__
    if (aError == KErrTimedOut)
    {
        JniUtils::throwNewException(aJni, "java/io/InterruptedIOException" ,
                                    "Timeout occurred while sending message" +
                                    JavaCommonUtils::intToString(aError));
    }
    else
    {
        if (aIsSendOperation)
        {
            JniUtils::throwNewException(aJni, "java/io/IOException" ,
                                        "Sending message failed: "
                                        + JavaCommonUtils::intToString(aError));
        }
        else
        {
            JniUtils::throwNewException(aJni, "java/io/IOException" ,
                                        "Receiving message failed: "
                                        + JavaCommonUtils::intToString(aError));
        }
    }
#endif // __SYMBIAN32__
}
