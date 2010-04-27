/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  socketserverconnection.h
*
*/

#ifndef SOCKETSERVERCONNECTION_H
#define SOCKETSERVERCONNECTION_H
#include <string>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "serverconnection.h"
#include "javaosheaders.h"
#include "logger.h"
#include "connectionlistener.h"
#include "javacommonutils.h"
#include "monitor.h"


using namespace java::push;

/**
* This is the base class for Native Peer of server socket.
* NativeSocketServerConnection will make calls to this class to create a carry out the all server socket operations.
*/

class SocketServerConnection : public ServerConnection
{

public:
    SocketServerConnection();
    OS_IMPORT SocketServerConnection(const std::wstring& aUri,const std::wstring& aFilter);
    OS_IMPORT virtual ~SocketServerConnection();

    virtual void open(ConnectionListener* aListener);
    virtual void close();
    virtual std::wstring getUri() const;
    virtual std::wstring getFilter() const;
    OS_IMPORT virtual void setFilter(const std::wstring& aFilter);

    OS_IMPORT int open();
    OS_IMPORT int accept();
    OS_IMPORT void setNormalServerConnection();
    OS_IMPORT bool isNormalServerConnection();

    /**
    * close , to close the server connection
    * @param[in]  fd: server socket descriptor.
    * @return 0 on successful completion.. If error, then the
    *         value of errno should be negated and returned. (return -errno)
    */

    OS_IMPORT int close(int fd);

private:

    /**
    * open is called by listening thread to create a new server socket to the specified port.
    * @param[in]  aPort: port on which server listens.
    * @return 0 on successful completion.. If error, then the
    *         value of errno should be negated and returned. (return -errno)
    */

    int open(int aPort);

    /**
    * accept is called by listening thread to accept an incoming connection. This will frist use the select() api to check for any
    * incoming connection. Then it calls the accept() system call to accept the connection
    * @param[in]  fd: server socket descriptor.
    * @return 0 on successful completion.. If error, then the
    *         value of errno should be negated and returned. (return -errno)
    */

    int accept(int fd);


    /**
    * setNonBlocking , to make the server socket as non-blocking. fcntl() system call of OpenC is used to set the socket option O_NONBLOCK.
    * @param[in]  fd: server socket descriptor.
    * @return 0 on successful completion.. If error, then the
    *         value of errno should be negated and returned. (return -errno)
    */

    void setNonBlocking(int fd);

    static void* listenThread(void* params);

    /**
     * SetActivityFlag method  updates status whether
     * ServerConnection object has pending message(= message arrived but not yet read).
     * @param aFlag Updating logic of pending message flag:
     *                    - Set it 'true' when message arrives to the connection.
     *                    - Set it 'false' when ServerConnection::open() operation has been called.
     */
    void setActivityFlag(bool aFlag);

private:
    ConnectionListener* mListener;

    pthread_t mThreadId;
    bool mKeepRunning;

    int mListenSocket;
    int mAcceptSocket;
    std::wstring mUri;
    std::wstring mFilter;
    int mPort;
    bool mIsAppRunning;
    bool mIsNormalServerConnection;
    java::util::Monitor* mOpenMonitor;
    int mError;
};

#endif // SOCKETSERVERCONNECTION_H
