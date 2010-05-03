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


#ifndef SMSCONNECTION_H
#define SMSCONNECTION_H

#include <jni.h>
#include <errno.h>
#include "monitor.h"
#include "connectionlistener.h"
#include "serverconnectionbase.h"
#include "smsplatformservice.h"
#include "serverconnectionfactory.h"

namespace java
{
namespace wma
{

/**
 *  CJavaSmsConnection implements  ConnectionListener.
 *  It offers methods for sending , to get
 *  number of data segments to send the message and to
 *  receive the message.
 *  @lib
 *  @
 */

class SmsConnection: public java::push::ConnectionListener
{
public:
    //Constructor
    SmsConnection(std::wstring aUri, bool aServerConnection);

    void initialize();
    /**
     * Method to send the message.
     * @return int The success or Failure value.
     */

    int send();
    /**
     * Method to close the connection.
     *
     */

    void close();
    /**
     * from ConnectionListener.
     * This function is called when a Message arrives.
     * This function notify's the other thread which inturn notifies the
     * java side.
     */

    void msgArrived(const std::wstring&);
    /**
     * from ConnectionListener.
     * This function is called when a error occurred while receiving
     * the message.This function notify's the other thread which inturn
     * notifies the java side, about the error.
     * @param[in] aUri - Uri of the opened connection.
     * @param[in] aErrCode - error code.
     * @param[in] aErrText - error text.
     */
    void error(const std::wstring& aUri, int aErrCode,
               const std::string& aErrText);
    /**
     * Gets the Message type of the received Message
     * @return int The type of the Message either Binary/Text Message.
     */

    int getMessageType();
    /**
     * Gets the Time stamp of the received message.
     * @return jlong The time value as jlong.
     */

    jlong getTimestamp();

    //Destructor
    ~SmsConnection();
    /**
     * checks for any missed messages while opening a connection.
     * waits for incoming message & notifies the java side.
     * @param[in] aJni Jni interface pointer
     * @param[in] apeer jobject
     */

    void open(JNIEnv& aJni, jobject aPeer);
    /**
     * gets the received message as jobject to java side.
     * @param[in] aJni  Jni interface pointer.
     * @param[in] aMsgType Message type of the received message.
     * @return jobject The received message as jobject.
     */

    jobject getData(JNIEnv& aJni, const int aMsgType);
    /**
     * Gets the Remote Host Address.
     * @param[in] aJni  Jni interface pointer.
     * @return jstring The RemoteHostAddress as jstring.
     */

    jstring getRemoteHostAddress(JNIEnv& aJni);

    /**
     * function that calculates and returns numberofDataSegments of the Message.
     * @param[in]  atype  MessageType
     * @param[in] aLength length of the message
     * @param[in] aport port number
     * @param[in] ahostaddress Destination address
     * @param[in] adata Message data
     * @return int number of data segments.
     */

    int numberOfDataSegments(const int atype, const int aLenght, const int aport,
                             const char* ahostaddress, const char* adata);

private:
    //Any error while receiving
    int mError;
    //Uri of the opening connection.
    std::wstring mUri;
    //Number of Messages on queue
    int mMessagesOnQueue;
    //flag to identify server/client connection
    bool mServerConnection;
    //flag to notify notifier thread whether to exit or to listen
    bool mReceiveOperation;
    //own server connection
    ServerConnectionBase* mServerConn;
    //own os agnostic CSmsPlatformService
    SmsPlatformService* mMsgConn;
    // own monitor object
    java::util::Monitor* mMessageMonitor;
    //own server connection factory object
    java::push::ServerConnectionFactory* mServerConnectionFactory;
};

} //namespace wma
} //namespace java
#endif // SMSCONNECTION_H
