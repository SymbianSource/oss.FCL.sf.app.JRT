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

#include <string.h>
#include <signal.h>

#include "logger.h"

#include "socketconnection.h"

const unsigned int INVALID_THREAD_ID = 0;

namespace java
{
namespace comms
{
using java::util::ScopedLock;

SocketConnection::SocketConnection(IpcListener* aListener)
        : mThreadId(INVALID_THREAD_ID), mListener(aListener), mPort(0)
{
    init();
}

SocketConnection::SocketConnection(IpcListener* aListener, int aSocketFd)
        : mThreadId(INVALID_THREAD_ID), mSocket(aSocketFd), mListener(aListener), mPort(0)
{
    init();
}

void SocketConnection::init()
{
//    JELOG2(EJavaComms);
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

SocketConnection::~SocketConnection()
{
}

int SocketConnection::connect(int aPort)
{
//    JELOG2(EJavaComms);

    if (mThreadId != INVALID_THREAD_ID) return 0;
    mPort = aPort;
    int rc = mSocket.open(aPort);

    if (!rc)
    {
        rc = pthread_create(&mThreadId, 0, SocketConnection::messageLoop, this);
        if (rc!=0)
        {
            mThreadId = INVALID_THREAD_ID;
            ELOG1(EJavaComms, "SocketConnection::start(): pthread_create failed, errno = %d", rc);
        }
    }
    else
    {
        ELOG1(EJavaComms, "SocketConnection::connect(): failed, rc = %d", rc);
    }

    return rc;
}

void* SocketConnection::messageLoop(void* params)
{
    SocketConnection* me = reinterpret_cast<SocketConnection*>(params);

    me->mListener->onStart();
    LOG2(EJavaComms, EInfo, "Client %d connected to server %d", me->getSocket(), me->mPort);

    int rc = 0;
    while (!rc)
    {
        rc = me->receive();
    }

    me->mListener->onExit();
    LOG2(EJavaComms, EInfo, "Client %d disconnected from server %d", me->getSocket(), me->mPort);
    me->mSocket.close();
    return 0;
}

void SocketConnection::disconnect()
{
    close();
    if (mThreadId != INVALID_THREAD_ID)
    {
//        mSocket.shutdown(SHUT_WR); // sends EOF
        int rc = pthread_join(mThreadId, 0);
        if (rc)
        {
            ELOG2(EJavaComms,
                  "SocketConnection::disconnect(): pthread_join failed %d - %s", rc, strerror(rc));
        }
    }

    mSocket.close();
    mThreadId = INVALID_THREAD_ID;
}

void SocketConnection::close()
{
    // If a process forks to children processes some things can prevent
    // EOF from being seen by a client even though a parent process calls
    // close() on a client socket if children processes do not close their
    // handle

    // let's use additional 'do exit' message to signal other party that
    // connection should be closed so that we don't rely on EOF only
    ipcMessage_t msg;
    msg.ipcHeader.length = 0;
    send(&msg);

    mSocket.shutdown(SHUT_WR); // sends EOF
}


// returns: 0   in ok
//          < 0 in error
int SocketConnection::receive()
{
    unsigned int len = 0;
    int rc = mSocket.read((char*)&len, sizeof(len));

    if (!rc)
    {
        if (len < sizeof(ipcHeader_t))
        {
            return -1;
        }

        char* buf = new char[len];
        rc = mSocket.read(buf, len);

        if (!rc)
        {
            ipcMessage_t* msg = reinterpret_cast<ipcMessage_t*>(buf);
            msg->ipcHeader.sender = mSocket.getSocket();
            msg->ipcHeader.receiver = mSocket.getSocket();
            // messages are considered to be always trusted
            msg->ipcHeader.permissions = -1;
            mListener->processMessage(msg);
        }

        delete [] buf;
    }

    return rc;
}

// returns: 0   in ok
//          < 0 in error
int SocketConnection::send(ipcMessage_t* aMsg)
{
    ScopedLock lock(mSendMutex);
    int len = aMsg->ipcHeader.length;
    int rc = mSocket.write((char*)&len, sizeof(len));

    if (!rc)
    {
        rc = mSocket.write((char*)aMsg, len);
    }
    return rc;
}

int SocketConnection::getSocket() const
{
    return mSocket.getSocket();
}

} // namespace comms
} // namespace java

