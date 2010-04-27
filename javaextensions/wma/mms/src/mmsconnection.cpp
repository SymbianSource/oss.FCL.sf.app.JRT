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


#include <errno.h>

#include "logger.h"
#include "mmsconnection.h"
#include "mmsserverconnectionfactory.h"

namespace java
{
namespace wma
{

MmsConnection::MmsConnection(bool aServerConnection,std::wstring aUri)
        :mUri(aUri),
        mIsServerConnection(aServerConnection),
        mReceiveOperation(aServerConnection),
        mServerConn(0),
        mMessageMonitor(0),
        mServerConnectionFactory(0),
        mMmsPlatformService(0)
{
    JELOG2(EWMA);
}

void MmsConnection::initialize(JNIEnv& aJni, jobject aPeer)
{
    JELOG2(EWMA);
    if (mIsServerConnection)
    {
        mServerConnectionFactory = &MmsServerConnectionFactory::getFactory();
        mServerConn = reinterpret_cast<MmsServerConnection*>
                      (mServerConnectionFactory->create(mUri));

        mServerConn->setOpen();
        mMessageMonitor = java::util::Monitor::createMonitor();
    }
    mMmsPlatformService = MMSPlatformService::getInstance(aJni, aPeer,
                          mIsServerConnection, mUri, this);

}

void MmsConnection::msgArrived(const std::wstring&)
{
    JELOG2(EWMA);
    mMessageMonitor->notify();
}

void MmsConnection::error(const std::wstring& /*aUri*/, int /*aErrCode*/,
                          const std::string& /*aErrText*/)
{
    // This is not handled
}

void MmsConnection::open(JNIEnv& aJni, jobject aPeer)
{
    JELOG2(EWMA);
    int status =0;
    int messages = 0;
    int newMessages = 0;
    int messagesNotified = 0;
    jclass sessionClass = aJni.FindClass(
                              "com/nokia/mj/impl/mms/MMSConnectionImpl");
    jmethodID receiveCallBackMethodId = aJni.GetMethodID(sessionClass,
                                        "messageReceiveCallback", "(I)I");
    // Keep notifying the java side about incoming messages till
    // the connection is closed
    while (mReceiveOperation && (0 == status))
    {
        // waits for the notification from server connection
        mMessageMonitor->wait();
        // check whether its a message notification or connection close
        // notification
        if (mReceiveOperation)
        {
            // Get no of messages available in store
            messages = mMmsPlatformService->getNumberOfMessages();
            // Calculate no of new messages to be notified to java
            newMessages = messages - messagesNotified;
            messagesNotified = messages;
            if (newMessages > 0)
            {
                status = aJni.CallIntMethod(aPeer, receiveCallBackMethodId,
                                            newMessages);
            }
            newMessages = 0;
        }
    }
}

void MmsConnection::close(JNIEnv& aJni, jobject aPeer)
{
    JELOG2(EWMA);
    if (mIsServerConnection)
    {
        mReceiveOperation = false;
        if (0 != mMessageMonitor)
        {
            mMessageMonitor->notify();
        }
        if ((0 != mServerConnectionFactory) && (0 != mServerConn))
        {
            mServerConnectionFactory->releaseConnection(mUri);
            mServerConnectionFactory = 0;
            mServerConn = 0;
        }
    }
    if (0 != mMmsPlatformService)
    {
        mMmsPlatformService->closeConnection(aJni, aPeer);
    }
}

jint MmsConnection::sendMMS(JNIEnv& aJni, jbyteArray aMsg, jint aOffset,
                            jint aMsgLenght, jstring aAddress)
{
    JELOG2(EWMA);
    return mMmsPlatformService->send(aJni, aMsg, aOffset, aMsgLenght, aAddress);
}

jbyteArray MmsConnection::retrieveMMSMessage(JNIEnv& aJni)
{
    JELOG2(EWMA);
    return mMmsPlatformService->retrieveMessage(aJni);
}

MmsConnection::~MmsConnection()
{
    JELOG2(EWMA);
    delete mMmsPlatformService;
    delete mMessageMonitor;
}

} //namespace wma
} //namespace java
