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

#include <sys/socket.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#include "logger.h"

#include "serversocketconnection.h"
#include "socketconnection.h"

const unsigned int INVALID_THREAD_ID = 0;

namespace java
{
namespace comms
{
using java::util::ScopedLock;

ServerSocketConnection::ServerSocketConnection(IpcListener* aListener)
        : mThreadId(INVALID_THREAD_ID), mListener(aListener), mKeepRunning(0), mPort(0)
{
    /*
     * The SIGPIPE signal will be received if the peer has gone away
     * and an attempt is made to write data to the peer. Ignoring this
     * signal causes the write operation to receive an EPIPE error.
     * Thus, the user is informed about what happened.
    */
#ifndef __SYMBIAN32__
    signal(SIGPIPE, SIG_IGN);
#endif
}

ServerSocketConnection::~ServerSocketConnection()
{
}

int ServerSocketConnection::start(int aPort)
{
    if (mThreadId != INVALID_THREAD_ID) return 0;
    mPort = aPort;
    int rc = mListenSocket.open(aPort);

    if (!rc)
    {
        mKeepRunning = 1;
        rc = pthread_create(&mThreadId, 0, ServerSocketConnection::messageLoop, this);
        if (rc!=0)
        {
            mThreadId = INVALID_THREAD_ID;
            ELOG1(EJavaComms, "ServerSocketConnection::start(): pthread_create failed, errno = %d", rc);
        }
    }

    return rc;
}

void* ServerSocketConnection::messageLoop(void* params)
{
    ServerSocketConnection* me = reinterpret_cast<ServerSocketConnection*>(params);
    me->mListener->onStart();
    LOG1(EJavaComms, EInfo, "Server started on %d", me->mPort);
    me->select();
    me->removeConnections();
    LOG1(EJavaComms, EInfo, "Server stopped on %d", me->mPort);
    me->mListener->onExit();
    return 0;
}


void ServerSocketConnection::stop()
{
    mKeepRunning = 0;

    if (mThreadId != INVALID_THREAD_ID)
    {
        // kick select out from wait
        Socket s;
        s.open(mPort);
        s.close();

        int rc = pthread_join(mThreadId, 0);
        if (rc)
        {
            ELOG1(EJavaComms, "ServerSocketConnection::stop(): pthread_join failed, errno = %d", rc);
        }
    }

    mListenSocket.close();
    removeConnections();
    mThreadId = INVALID_THREAD_ID;
    mPort = 0;
}

int ServerSocketConnection::select()
{
    while (mKeepRunning)
    {
        fd_set socks;
        int maxfd = 0;

        createSelectSet(socks, maxfd);

        timeval tv;
        tv.tv_sec  = 600;
        tv.tv_usec = 0;

        int rc = ::select(maxfd + 1, &socks, 0, 0, &tv);

        if (rc == 0)
        {
//            LOG(EJavaComms, EInfo, "select() timed out!");
            continue;
        }
        else if (rc < 0 && errno != EINTR)
        {
            ELOG1(EJavaComms, "Error in select(): %s", strerror(errno));
            break;
        }
        else if (rc > 0)
        {
            rc = handleNewConnection(socks);
            if (rc<0) break; // bail out if accept fails
            handleRead(socks);
        }
    }
    return 0;
}

int ServerSocketConnection::handleNewConnection(fd_set& aSet)
{
    int peersock = 0;
    if (FD_ISSET(mListenSocket.getSocket(), &aSet))
    {
        peersock = mListenSocket.accept();
        if (peersock < 0)
        {
            ELOG1(EJavaComms, "Error in accept(): %s", strerror(errno));
        }
        else
        {
            ScopedLock lock(mSocketsMutex);
            SocketConnection* con = new SocketConnection(mListener, peersock);
            mSockets.push_back(con);
            LOG3(EJavaComms, EInfo, "Server %d - client %d connected - clients %d",
                 mPort, con->getSocket(), mSockets.size());
        }
    }
    return peersock;
}


void ServerSocketConnection::handleRead(fd_set& aSet)
{
    ScopedLock lock(mSocketsMutex);

    for (connections_t::iterator it = mSockets.begin(); it != mSockets.end();)
    {
        if (FD_ISSET((*it)->getSocket(), &aSet))
        {
            int rc = (*it)->receive();

            if (!rc)
            {
                ++it;
            }
            else
            {
                LOG3(EJavaComms, EInfo, "Server %d - client %d disconnected - clients %d",
                     mPort, (*it)->getSocket(), mSockets.size()-1);
                (*it)->disconnect();
                delete(*it);
                it = mSockets.erase(it);
            }
        }
        else
        {
            ++it;
        }
    }
}

void ServerSocketConnection::createSelectSet(fd_set& aSet, int& aMaxFd)
{
    ScopedLock lock(mSocketsMutex);

    FD_ZERO(&aSet);
    aMaxFd = mListenSocket.getSocket();

    // add listen socket
    FD_SET(mListenSocket.getSocket() ,&aSet);

    // add read sockets
    for (connections_t::iterator it = mSockets.begin(); it != mSockets.end(); ++it)
    {
        int sock_fd = (*it)->getSocket();
        FD_SET(sock_fd, &aSet);

        aMaxFd = (aMaxFd < sock_fd)?sock_fd:aMaxFd;
    }
}

int ServerSocketConnection::send(ipcMessage_t* aMsg)
{
    ScopedLock lock(mSocketsMutex);

    int rc = EINVAL;
    for (connections_t::iterator it = mSockets.begin(); it != mSockets.end(); ++it)
    {
        if (aMsg->ipcHeader.receiver == (*it)->getSocket())
        {
            rc = (*it)->send(aMsg);
            break;
        }
    }
    return rc;
}

void ServerSocketConnection::removeConnections()
{
    ScopedLock lock(mSocketsMutex);

    for (connections_t::iterator it = mSockets.begin(); it != mSockets.end();)
    {
        (*it)->disconnect();
        LOG3(EJavaComms, EInfo, "Server %d stopping - client %d disconnected - clients %d",
             mPort, (*it)->getSocket(), mSockets.size()-1);
        delete(*it);
        it = mSockets.erase(it);
    }
}

} // namespace comms
} // namespace java

