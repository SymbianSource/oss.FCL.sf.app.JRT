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

#ifndef SMSSERVERCONNECTION_H
#define SMSSERVERCONNECTION_H

#include <es_sock.h>
#include <gsmumsg.h>

#include "serverconnectionbase.h"

namespace java
{
namespace wma
{
const wchar_t SMS_STORE_PATH[] = L"sms\\";
/**
 *  SmsServerConnection implement  ServerConnectionBase.
 *  It is responsible to start the midlet when the message arrive in the device
 *  for a registered midlet and when the midlet is not running.It is also
 *  responsible to receive message when the midlet is running
 *  @lib smsserverconnection.dll
 *  @
 */
class SmsServerConnection: public CActive,public ServerConnectionBase
{

public:
    //Constructor
    OS_IMPORT SmsServerConnection(const std::wstring& aUri,
                                  const std::wstring& aFilter);
    OS_IMPORT virtual ~SmsServerConnection();
    /**
     * from ServerConnection
     * Opens the connection. It registers the push URI to the messaging system
     *  for receiving messages.
     * Throws COMMON_SRV_CONN_PLUGIN_ERROR exception if there is an error while
     * registering the connection.
     * @param[in] aListener: Listener to be notified when message arrives.
     */
    OS_IMPORT virtual void open(java::push::ConnectionListener* aListener);
    /**
     * from ServerConnectionBase
     * Opens the connection. It registers the server URI to the messaging system
     *  for receiving messages.
     * Throws COMMON_SRV_CONN_PLUGIN_ERROR exception if there is an error while
     * registering the connection.
     * @param[in] aListener: Listener to be notified when message arrives.
     * @param[in] aAppLaunched : bool to notify listening by midlet or push.
     */
    OS_IMPORT virtual void open(java::push::ConnectionListener* aListener,
                                bool aAppLaunched);
    /**
     *from ServerConnection
     * Closes the connection
     */
    OS_IMPORT virtual void close();
    /**
     * RetrieveMessage from ServerConnectionBase
     * retrieves the received message
     * @param[out] aSmsBuf:the message buffer into which the received message is
     * written into.
     * @return SUCCESS or FAILURE status
     */
    OS_IMPORT virtual int retrieveMessage(TJavaMessageParametersBuf& aSmsBuf);
private://From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt);
private:
    //Constructor
    SmsServerConnection();

    /**
     * getSocketServer
     * opens a session to the socket server .
     */
    void getSocketServerL();

    /**
     * ReadMessageFromStack
     * Reads the arrived message from stack and stores it if the received host
     * address matches with filter from which messages are expected.
     */
    void readMessageFromStackL();
    /**
     *
     * thread function.which listens for incoming messages and informs the
     * listener about the incoming message.
     * @param[in] aParams - server connection object.
     */
    static void* listenThread(void* aParams);
    /**
     * Function which matches the received host address with the filter
     * @returns true if they match otherwise false
     */
    TBool match();
private:
    enum TState
    {
        EReceivingMessageForNotify,
        ENotifyingReadSucceeded,
        ENotifyingReadFailed,
        EExit,
    };
    // thread running state
    TBool mIsRunning;
    // state of socket server
    TBool mSocketServerOpened;
    // handle to the file system.
    RFs mFs;
    // the ongoing operation which is executed by the server connection object.
    TState mState;
    // handle to the socket
    RSocket mSocket;
    //defines the acceptable remote host.
    HBufC* mFilterDes;
    //thread id
    pthread_t mThreadId;
    // To synchronize threads
    pthread_mutex_t  mMutex;
    // To synchronize threads
    pthread_cond_t   mCondVar;
    //Input stream interface
    std::ifstream readStream;
    TPckgBuf<TUint> mIoctlBuf;
    // handle to the socket server
    RSocketServ mSocketServer;
    //listener to be notified for incoming messages
    java::push::ConnectionListener* mListener;
    //Buffer to hold the sms message parameters
    TSMSParametersBuf mSmsParameters;
    //Buffer to hold sms message data
    TSmsDataBuf mSmsDataBuf;
    // monitor object
    java::util::Monitor* mOpenMonitor;
    //Represents a complete SMS message.
    CSmsMessage* mMessage;
};
}// end of namespace wma
}// end of namespace java
#endif // SMSSERVERCONNECTION_H
