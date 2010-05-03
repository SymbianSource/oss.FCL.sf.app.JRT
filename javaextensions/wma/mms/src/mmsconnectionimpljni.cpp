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


#include "logger.h"
#include "javajniutils.h"
#include "pushexception.h"
#include "pusherrorcodes.h"
#include "javacommonutils.h"
#include "mmsconnection.h"
#include "com_nokia_mj_impl_mms_MMSConnectionImpl.h"

using namespace java::wma;

/*
 * Class:     com_nokia_mj_impl_mms_MMSConnectionImpl
 * Method:    _createPeer
 * Signature: (ZLjava/lang/String;[I)J
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_mms_MMSConnectionImpl__1createPeer
(JNIEnv* aJni, jobject aPeer, jboolean aServerConnection, jstring aUri)
{
    JELOG2(EWMA);
    MmsConnection* mmsConn = 0;
    std::wstring Uri = java::util::JniUtils::jstringToWstring(aJni,aUri);
    try
    {
        mmsConn = new MmsConnection(aServerConnection, Uri);
        mmsConn->initialize(*aJni, aPeer);
        return reinterpret_cast<jint>(mmsConn);
    }
    catch (PushException& ex)
    {
        ELOG(EWMA,"ERROR!!! CreateConnection() PushException was caught");
        if (PUSH_CONNECTION_ALREADY_EXISTS == ex.mErrCode)
        {
            ELOG(EWMA,"MMS : PushConnection already Exists");
            JniUtils::throwNewException(aJni, "java/io/IOException" ,
                                        "Server Connection already exists");
        }
        else if (COMMON_SRV_CONN_PLUGIN_ERROR == ex.mErrCode)
        {
            ELOG(EWMA,"MMS : Common pushConnection error");
            JniUtils::throwNewException(aJni, "java/io/IOException" ,
                                        "Opening server connection failed: "
                                        + JavaCommonUtils::intToString(ex.mErrCode));
        }
    }
    catch (int error)
    {
        ELOG1(EWMA,"MMS : Caught an Exception --Value is:%d",error);
        JniUtils::throwNewException(aJni, "java/io/IOException" ,
                                    "Opening connection failed: " +
                                    JavaCommonUtils::intToString(error));
    }
    catch (std::bad_alloc &memoryAllocationException)
    {
        ELOG(EWMA,"MMS : Bad allocation exception");
        JniUtils::throwNewException(aJni, "java/lang/OutOfMemoryError" ,
                                    "Out of memory");
    }
    if (mmsConn != 0)
    {
        mmsConn->close(*aJni, aPeer);
        delete mmsConn;
    }
    return 0;
}

/*
* Class:     com_nokia_mj_impl_mms_MMSConnectionImpl
* Method:    _send
* Signature: ([BIILjava/lang/String;J)I
*/
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_mms_MMSConnectionImpl__1send
(JNIEnv* aEnv, jobject , jbyteArray bMsg, jint offset, jint msgLenght,
 jstring aAddress, jint aNativeHandle)
{
    JELOG2(EWMA);
    jint status = NULL;
    MmsConnection* mmscSession =reinterpret_cast <MmsConnection*>
                                (aNativeHandle);
    status = mmscSession->sendMMS(*aEnv, bMsg, offset, msgLenght, aAddress);
    return status;
}

/*
 * Class:     com_nokia_mj_impl_mms_MMSConnectionImpl
 * Method:    _openConnection
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_mms_MMSConnectionImpl__1openConnection
(JNIEnv* aEnv, jobject aPeer, jint aNativeHandle)
{
    JELOG2(EWMA);
    MmsConnection* mmsConn = reinterpret_cast <MmsConnection *>
                             (aNativeHandle);
    mmsConn->open(*aEnv,aPeer);
}

/*
 * Class:     com_nokia_mj_impl_mms_MMSConnectionImpl
 * Method:    _retrieveMessage
 * Signature: (J)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_nokia_mj_impl_mms_MMSConnectionImpl__1retrieveMessage
(JNIEnv* aEnv, jobject , jint aNativeHandle)
{
    JELOG2(EWMA);
    MmsConnection* mmsConn = reinterpret_cast <MmsConnection *>
                             (aNativeHandle);
    return mmsConn->retrieveMMSMessage(*aEnv);
}

/*
 * Class:     com_nokia_mj_impl_mms_MMSConnectionImpl
 * Method:    _closeConnection
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_mms_MMSConnectionImpl__1closeConnection
(JNIEnv* aJni, jobject aPeer, jint aNativeHandle)
{
    JELOG2(EWMA);
    MmsConnection* mmsConn = reinterpret_cast <MmsConnection *>
                             (aNativeHandle);
    mmsConn->close(*aJni,aPeer);
}

/*
 * Class:     com_nokia_mj_impl_mms_MMSConnectionImpl
 * Method:    _dispose
 * Signature: (J)V
 */
JNIEXPORT  void JNICALL Java_com_nokia_mj_impl_mms_MMSConnectionImpl__1dispose
(JNIEnv* , jobject , jint aNativeHandle)
{
    JELOG2(EWMA);
    MmsConnection* mmsConn = reinterpret_cast <MmsConnection *>
                             (aNativeHandle);
    delete mmsConn;
}

/*
 * Class:     com_nokia_mj_impl_mms_MMSConnectionImpl
 * Method:    _checkError
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_mms_MMSConnectionImpl__1checkError
(JNIEnv *aJni, jobject , jint aError)
{
    JELOG2(EWMA);
#ifdef __SYMBIAN32__
    if (aError == KErrCancel || aError == KErrTimedOut)
    {
        JniUtils::throwNewException(aJni, "java/io/InterruptedIOException" ,
                                    "Sending message failed: " +
                                    JavaCommonUtils::intToString(aError));
    }
    else
    {
        JniUtils::throwNewException(aJni, "java/io/IOException" ,
                                    "Sending message failed: " +
                                    JavaCommonUtils::intToString(aError));
    }
#endif // __SYMBIAN32__
}

