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
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>
#ifndef __SYMBIAN32__
#include <netinet/tcp.h>  // TCP_NODELAY
#endif
#include <string.h>

#include "logger.h"

#include "socket.h"

const int INVALID_SOCKET = 0;

namespace java
{
namespace comms
{

Socket::Socket() : mSocket(INVALID_SOCKET)
{
}

Socket::Socket(int aSocket) : mSocket(aSocket)
{
    setSockOpts();
}

Socket::~Socket()
{
    close();
}

int Socket::open(int aPort)
{
    sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(aPort);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    mSocket = socket(AF_INET, SOCK_STREAM , 0);

    setSockOpts();

    int rc = connect(mSocket, (sockaddr*)&addr, sizeof(addr));

    if (rc<0)
    {
        ELOG2(EJavaComms, "connect failed %d - %s", errno, strerror(errno));
        close();
    }
    else
    {
        rc = 0;
    }

    return rc;
}

// returns: 0   ok
//          < 0 in error
int Socket::read(char *aBuf, int len)
{
    int rc = 0;
    do
    {
        if (mSocket==INVALID_SOCKET) return -1;

        fd_set readset;
        FD_ZERO(&readset);
        FD_SET(mSocket, &readset);

        timeval tv;
        tv.tv_sec = 600;
        tv.tv_usec = 0;

        rc = select(mSocket+1, &readset, 0, 0, &tv);

        if (rc < 0 && errno != EINTR)
        {
            ELOG3(EJavaComms, "select failed %d - %s - fd=%d", errno, strerror(errno), mSocket);
            break;
        }
        else if (rc > 0 && FD_ISSET(mSocket, &readset))
        {
            rc = recv(aBuf, len);
            break;
        }
    }
    while (1);

    return rc;
}


// returns: 0   ok (all bytes read)
//          < 0 in error
int Socket::recv(char *aBuf, int len)
{
    int count = len;
    while (count > 0)
    {
        int rc = ::recv(mSocket, aBuf, count, 0);

        if (rc < 0)
        {
            if (errno == EINTR) continue;
            WLOG3(EJavaComms, "recv failed %d - %s - fd=%d", errno, strerror(errno), mSocket);
            return -1;
        }
        if (rc == 0)     // EOF
        {
//            LOG1(EJavaComms, EInfo, "EOF received - exiting fd=%d", mSocket);
            return -1;
        }
        aBuf += rc;
        count -= rc;
    }
    return 0;
}

// returns: 0   in ok
//          < 0 in error
int Socket::write(const char *aBuf, int len)
{
    int sent = 0;
    do
    {
        int rc = send(mSocket, aBuf+sent, len-sent, 0);
        if (rc > 0)
            sent += rc;
        else if (rc < 0 && errno != EINTR)
        {
            WLOG3(EJavaComms, "send failed %d - %s - fd=%d", errno, strerror(errno), mSocket);
            return -1;
        }
    }
    while (len > sent);

    return 0;
}

int Socket::shutdown(int aHow)
{
    int rc = ::shutdown(mSocket, aHow);
    if (rc<0) WLOG3(EJavaComms, "shutdown failed %d - %s - fd=%d", errno, strerror(errno), mSocket);

    return rc;
}

void Socket::close()
{
    if (mSocket !=INVALID_SOCKET)
    {
        int rc = ::close(mSocket);
        if (rc<0) WLOG3(EJavaComms, "close failed %d - %s - fd=%d", errno, strerror(errno), mSocket);

        mSocket = INVALID_SOCKET;
    }
}

int Socket::getSocket() const
{
    return mSocket;
}

void Socket::setSockOpts()
{
    int flag = 1;
    int rc = setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, sizeof(int));
    if (rc<0) ELOG3(EJavaComms, "setsockopt(SO_REUSEADDR) failed %d - %s - fd=%d", errno, strerror(errno), mSocket);

    rc = setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
    if (rc<0) ELOG3(EJavaComms, "setsockopt(TCP_NODELAY) failed %d - %s - fd=%d", errno, strerror(errno), mSocket);
}

} // namespace comms
} // namespace java
