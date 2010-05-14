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
 * Description:  ApnSettings.app
 *
 */
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include "socketlocalhostinfo.h"
#include "logger.h"

int SocketLocalHostInfo::getLocalAddress(int aSockDesc, char *aLocalAddr, int /* aIadid */, int /* aType */)
{

    int retval;
    struct sockaddr_in saddr;
    socklen_t len = sizeof(saddr);

    if (getsockname(aSockDesc, (struct sockaddr*) &saddr, &len) == -1)
    {
        ELOG1(ESOCKET,
              "SocketServerConnectionJNI::getLocalAddress() : getlocaladdress failed : %d",
              errno);
        strcpy(aLocalAddr, "error");
        retval = -errno;
    }
    else
    {
        strcpy(aLocalAddr, inet_ntoa(saddr.sin_addr));
        retval = 0;
    }

    return 0;
}
