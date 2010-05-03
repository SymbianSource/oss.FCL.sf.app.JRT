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


#include "logger.h"
#include "smsconnection.h"
#include "smsserverconnectionfactory.h"
#include "cbsserverconnectionfactory.h"

using namespace java::push;
namespace java
{
namespace wma
{

SmsConnection::SmsConnection(std::wstring aUri,bool aServerConnection)
        :mError(0),
        mUri(aUri),
        mMessagesOnQueue(0),
        mServerConnection(aServerConnection),
        mReceiveOperation(aServerConnection),
        mServerConn(0),
        mMsgConn(0),
        mMessageMonitor(0),
        mServerConnectionFactory(0)
{
    JELOG2(EWMA);
}

void SmsConnection::initialize()
{
    JELOG2(EWMA);
    mMsgConn = SmsPlatformService::getNewInstance(mUri);
    if (mServerConnection)
    {
        //Get the factory instance based on the protocol
        if (mUri.find(L"cbs://:") != std::wstring::npos)
        {
            mServerConnectionFactory =
                &CbsServerConnectionFactory::getFactory();
        }
        else
        {
            mServerConnectionFactory =
                &SmsServerConnectionFactory::getFactory();
        }
        mMessageMonitor = java::util::Monitor::createMonitor();
        //Create the server connection
        mServerConn = reinterpret_cast<ServerConnectionBase*>
                      (mServerConnectionFactory->create(mUri));
        mServerConn->open(this, true);
    }
}

int SmsConnection ::numberOfDataSegments(const int aType,const int aLength,
        const int aPort,const char* aHostAddress,const char* aData)
{
    JELOG2(EWMA);
    return mMsgConn->getSmsDataSegments(aType, aLength, aPort,
                                        aHostAddress, aData);
}

int SmsConnection :: send()
{
    JELOG2(EWMA);
    return mMsgConn->send();
}

SmsConnection :: ~SmsConnection()
{
    JELOG2(EWMA);
    close();
    delete mMessageMonitor;
    delete mMsgConn;
}

void SmsConnection :: close()
{
    JELOG2(EWMA);
    if (mServerConnection)
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
}

void SmsConnection :: msgArrived(const std::wstring&)
{
    JELOG2(EWMA);
    mMessageMonitor->notify();
}

void SmsConnection :: error(const std::wstring& /*aUri*/,int aErrCode,
                            const std::string& /*aErrText*/)
{
    JELOG2(EWMA);
    ELOG1(EWMA,"WMA : Error Receiving Message : %d",aErrCode);
    mError = aErrCode;
    mMessageMonitor->notify();
}

void SmsConnection :: open(JNIEnv& aJni, jobject aPeer)
{
    JELOG2(EWMA);
    int status = 0;
    jclass sessionClass = aJni.FindClass(
                              "com/nokia/mj/impl/sms/SMSConnectionImpl");
    jmethodID MessageReceiveCallbackMethod = aJni.GetMethodID(sessionClass,
            "messageReceiveCallback", "(II)I");
    // Keeps notifying the java side about incoming messages till
    // the connection is closed
    while (mReceiveOperation && (0 == status))
    {
        //waits for the notification from server connection
        mMessageMonitor->wait();
        // check whether its a message notification or connection close
        // notification
        if (0 == mError && mReceiveOperation)
        {
            //Get no of messages available in store
            int messagesOnStore = mServerConn->getMessagesOnStore();
            //Calculate no of new messages to be notified to java
            int newMessages = messagesOnStore - mMessagesOnQueue;
            mMessagesOnQueue = messagesOnStore;
            LOG1(EWMA, EInfo , "SMS : %d New messages to be notified", newMessages);
            if (newMessages > 0)
            {
                status = aJni.CallIntMethod(aPeer,MessageReceiveCallbackMethod,
                                            newMessages,0);
            }
        }
        else if (0 != mError)
        {
            ELOG1(EWMA, "WMA : Error while receiving %d", mError);
            //If there is an error report it to java
            status = aJni.CallIntMethod(aPeer,MessageReceiveCallbackMethod,0,mError);
            mError = 0;
        }
    }
}

int SmsConnection :: getMessageType()
{
    JELOG2(EWMA);
    return mMsgConn->receivedMessageType(*mServerConn);
}

jlong SmsConnection :: getTimestamp()
{
    JELOG2(EWMA);
    return mMsgConn->getTimeStamp();
}

jstring SmsConnection :: getRemoteHostAddress(JNIEnv& aJni)
{
    JELOG2(EWMA);
    return mMsgConn->getHostAddress(aJni);
}

jobject SmsConnection :: getData(JNIEnv& aJni,const int aMsgType)
{
    JELOG2(EWMA);
    return mMsgConn->getMessageData(aJni,aMsgType);
}

} //namespace wma
} //namespace java

