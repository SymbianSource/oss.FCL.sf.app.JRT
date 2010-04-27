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
* Description:
*
*/



#include "socketserverconnection.h"
#include "socketserverconnectionfactory.h"
#include "pushexception.h"
#include "pusherrorcodes.h"

#define SOCKET_ERROR   (-1)
#define INVALID_SOCKET (-1)
#define IO_WOULDBLOCK -2

using namespace java::util;

SocketServerConnection::SocketServerConnection()
{
    LOG(ESOCKET, EInfo, "+SocketServerConnection - default constructor");
    mListener = NULL,
                mThreadId = 0;
    mKeepRunning = false;
    mListenSocket = INVALID_SOCKET;
    mAcceptSocket = INVALID_SOCKET;
    mPort = 0;
    mIsNormalServerConnection = true;
    mOpenMonitor = 0;
    mError = 0;
}
OS_EXPORT SocketServerConnection::SocketServerConnection
(const std::wstring& aUri,
 const std::wstring& aFilter)
        : mListener(0),
        mThreadId(0),
        mKeepRunning(false),
        mListenSocket(INVALID_SOCKET),
        mAcceptSocket(INVALID_SOCKET),
        mUri(aUri),
        mFilter(aFilter),
        mPort(0)

{
    JELOG2(ESOCKET);
    LOG(ESOCKET, EInfo, "+SocketServerConnection::SocketServerConnection");
    // sanity check for url
    mIsAppRunning = false;
    mIsNormalServerConnection = false;
    mOpenMonitor = 0;
    mError = 0;

    // create monitors only when connection comes via push plugin
    if (0 == mOpenMonitor)
    {
        mOpenMonitor = Monitor::createMonitor();
    }
    if (aUri.find(L"socket://:") != std::wstring::npos)
    {
        if (aUri.length() == 10)
            mPort = 0;
        else
        {
            std::wstring port = aUri.substr(10); // "socket://:"
            mPort = JavaCommonUtils::wstringToInt(port);
        }
        LOG1(ESOCKET, EInfo, "created SocketServerConnection on port %d", mPort);
    }
    else
    {
        mPort = -1;
        LOG(ESOCKET, EInfo, "SocketServerConnection::SocketServerConnection() Invalid url");
    }
}

OS_EXPORT SocketServerConnection::~SocketServerConnection()
{
    JELOG2(ESOCKET);
    if (mOpenMonitor)
    {
        delete mOpenMonitor;
    }
}


/* ----------------------------------------------------------------------

Starting point for socket server. A new thread is created which inturn initializes the server socket and waits
for an incoming connection.

pthread_create(): This OpenC call is used to create the new thread. listenThread() is passed as the start point
of the thread.

-------------------------------------------------------------------------*/
OS_EXPORT void SocketServerConnection::open(ConnectionListener* aListener)
{
    JELOG2(ESOCKET);
    mListener = aListener;

    mKeepRunning = true;

    int rc = pthread_create(&mThreadId, NULL,  SocketServerConnection::listenThread, this);
    mOpenMonitor->wait();    // wait for open to return

    ILOG2(ESOCKET,"after release rc = %dm mPort = %d",rc,mPort);
    if (rc < 0 || mError < 0)
    {
        ELOG1(ESOCKET,"Socket server conn already exists on Port %d : Open Failed",mPort);
        std::string errTxt("ERROR!!! Opening of SocketServer failed");
        throw PushException(COMMON_SRV_CONN_PLUGIN_ERROR,errTxt,__FILE__,  __FUNCTION__,__LINE__);
    }
}


// Since server socket is non-blocking, a accept() is called in a new thread

void* SocketServerConnection::listenThread(void* aParams)
{
    JELOG2(ESOCKET);

    SocketServerConnection* pThis = reinterpret_cast<SocketServerConnection*>(aParams);

    pThis->mListenSocket  = pThis->open(pThis->mPort);
    ILOG1(ESOCKET,  "+SocketServerConnection::listenThread - pThis->mListenSocket = %d",pThis->mListenSocket);

    if (pThis->mListenSocket < 0)
    {
        pThis->mError = -errno;
        (pThis->mOpenMonitor)->notify();
        pthread_exit(0);
    }
    (pThis->mOpenMonitor)->notify();


    int fd = INVALID_SOCKET;

    if (fd < 0 && pThis->mKeepRunning)
    {
        fd = pThis->accept();
    }

    if (!(fd<0))  // we have incoming connection
    {
        pThis->mAcceptSocket = fd;
        if (pThis->mIsAppRunning == true)
        {
            pThis->setActivityFlag(false);
            pthread_exit(0);
        }
        else
        {
            if (pThis->mListener)
            {
                pThis->mListener->msgArrived();
            }
            pThis->setActivityFlag(true);
        }
    }

    pthread_exit(0);
    return 0;
}


/* ----------------------------------------------------------------------
This function is used to open a server socket connection.

@param:  aPort - Port on which the server will bind and start lisetening.

OpenC apis used

htons(), htonl() : to convert values between host and network byte order

socket():  creates an endpoint for communication and returns a descriptor. We create a "STREAM" socket

bind(): We bind to address INADDR_ANY, which indicates listen happens on all networking interfaces present

listen(): listens for incoming connection.

accept(): accepts a connection on a socket.

-------------------------------------------------------------------------*/
OS_EXPORT int SocketServerConnection::open()
{
    JELOG2(ESOCKET);

    mIsAppRunning = true;

    if (mKeepRunning) // we are listening for push connection
    {
        mKeepRunning = false;
        pthread_join(mThreadId, NULL);
    }
    else
    {
        mListenSocket = open(mPort);
    }
    setActivityFlag(false);
    return mListenSocket;
}

int SocketServerConnection::open(int aPort)
{
    JELOG2(ESOCKET);
    int fd  = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == INVALID_SOCKET)
    {
        int err = -errno;
        return err;
    }

    struct sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons((unsigned short)aPort);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int res = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if (res != SOCKET_ERROR)
    {
        res = listen(fd, SOMAXCONN);
        if (res != SOCKET_ERROR)
        {
            // setNonBlocking(fd);
            return fd;
        }
    }
    int err = -errno;
    LOG(ESOCKET,EInfo,"closing ..");
    close(fd);
    return err;
}

OS_EXPORT int SocketServerConnection::accept()
{
    JELOG2(ESOCKET);
    int sock_fd = INVALID_SOCKET;

    if (mAcceptSocket != INVALID_SOCKET) // incoming push connection
    {
        sock_fd = mAcceptSocket;
        mAcceptSocket = INVALID_SOCKET;
    }
    else
    {
        sock_fd = accept(mListenSocket);
    }

    return sock_fd;
}

int SocketServerConnection::accept(int fd)
{
    // paramter fd, is the socket descriptor returned during server socket creation.
    struct sockaddr_in sa;
    socklen_t saLen = sizeof(sa);
    int res = 0;


    LOG(ESOCKET, EInfo, "before accept");
    res = ::accept(fd, (struct sockaddr *)&sa, &saLen);
    LOG1(ESOCKET, EInfo, "accept returned %d",res);
    if ((res == -1) && (errno == EWOULDBLOCK))
    {
        res = IO_WOULDBLOCK;
    }
    if (res == -1)
    {
        int err = -errno;
        return err;
    }
    return res;
}
OS_EXPORT bool SocketServerConnection::isNormalServerConnection()
{
    return mIsNormalServerConnection;
}

OS_EXPORT void SocketServerConnection::setNormalServerConnection()
{
    LOG(ESOCKET,EInfo,"Push is enabled");
    mIsNormalServerConnection = true;
}

OS_EXPORT void SocketServerConnection::close()
{
    JELOG2(ESOCKET);

    if (mListenSocket != INVALID_SOCKET)
    {
        int ret = shutdown(mListenSocket,SHUT_RDWR);
        close(mListenSocket);
        LOG1(ESOCKET, EInfo, "shutdown of mListenSocket returned %d",ret);
    }

    if (mAcceptSocket != INVALID_SOCKET)
    {
        int ret = shutdown(mAcceptSocket,SHUT_RDWR);
        close(mAcceptSocket);
        LOG1(ESOCKET, EInfo, "shutdown of mAcceptSocket returned %d",ret);
    }

    if (mKeepRunning) // we are listening for push connection
    {
        mKeepRunning = false;
        pthread_join(mThreadId, NULL);
    }
    // reset members
    mListenSocket = INVALID_SOCKET;
    mAcceptSocket = INVALID_SOCKET;

    mListener = 0;
    mThreadId = 0;
    //mKeepRunning = false;

}

OS_EXPORT std::wstring SocketServerConnection::getUri() const
{
    return mUri;
}

OS_EXPORT std::wstring SocketServerConnection::getFilter() const
{
    return mFilter;
}

OS_EXPORT void SocketServerConnection::setFilter(const std::wstring& aFilter)
{
    JELOG2(ESOCKET);
    mFilter = aFilter;
    return;
}

OS_EXPORT int SocketServerConnection::close(int fd)
{
    LOG(ESOCKET, EInfo, "+SocketServerConnection::close(int fd)");
    if (mListenSocket != INVALID_SOCKET)
    {
        int ret = shutdown(mListenSocket,SHUT_RDWR);
        LOG1(ESOCKET, EInfo, "shutdown of mListenSocket returned %d",ret);
    }

    if (mAcceptSocket != INVALID_SOCKET)
    {
        int ret = shutdown(mAcceptSocket,SHUT_RDWR);
        LOG1(ESOCKET, EInfo, "shutdown of mAcceptSocket returned %d",ret);
    }

    int ret = ::close(fd);
    // reset members
    mListenSocket = INVALID_SOCKET;
    mAcceptSocket = INVALID_SOCKET;

    LOG(ESOCKET, EInfo, "-SocketServerConnection::close(int fd)");
    return ret;
}

void SocketServerConnection::setNonBlocking(int fd)
{
    // first get the flags associated with this descriptor.
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
    {
        return; // check errno
    }

    // set the non-blocking flag to make the accept() call as non-blocking.
    flags = fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    if (flags < 0)
    {
        return; // throw exception
    }

}

void SocketServerConnection::setActivityFlag(bool aFlag)
{
    SocketServerConnectionFactory& scf =
        SocketServerConnectionFactory::getFactory();
    scf.setPendingMsgFlag(mUri, aFlag);
}

