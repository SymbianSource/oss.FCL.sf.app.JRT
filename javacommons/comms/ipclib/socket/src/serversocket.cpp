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
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>

#include "logger.h"

#include "serversocket.h"

const int INVALID_SOCKET = 0;

namespace java
{
namespace comms
{


ServerSocket::ServerSocket() : mSocket(INVALID_SOCKET)
{
//  JELOG2(EJavaComms);
}

ServerSocket::~ServerSocket()
{
    close();
}

int ServerSocket::open(int aPort)
{
//    JELOG2(EJavaComms);

    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket == -1)
    {
        return -1;
    }

    int flag = 1;
    setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, sizeof(int));

    sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(aPort);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int rc = bind(mSocket, (sockaddr*)&addr, sizeof(addr));
    if (rc != -1)
    {
        rc = listen(mSocket, 3);
        if (rc != -1)
        {
            return 0;
        }
    }

    ELOG2(EJavaComms, "listen failed %d - %s", errno, strerror(errno));
    close();
    return -1;
}

int ServerSocket::accept()
{
    sockaddr_in sa;
    socklen_t saLen = sizeof(sa);

    int socket = ::accept(mSocket, (sockaddr*)&sa, &saLen);
    if (socket<0) ELOG2(EJavaComms, "accept failed %d - %s", errno, strerror(errno));

    return socket;
}

int ServerSocket::close()
{
    int rc = 0;
    if (mSocket !=INVALID_SOCKET)
    {
        rc = ::close(mSocket);
        if (rc<0) ELOG2(EJavaComms, "close failed %d - %s", errno, strerror(errno));

        mSocket = INVALID_SOCKET;
    }
    return rc;
}

int ServerSocket::getSocket() const
{
    return mSocket;
}

} // namespace comms
} // namespace java

