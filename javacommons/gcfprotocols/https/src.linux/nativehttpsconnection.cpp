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


#include "nativehttpsconnection.h"
#include <stdio.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <string.h>

#include <logger.h>

namespace java
{
namespace https
{
int NativeHttpsConnection::createSocket(const char *aProxy,int aProxyPort, const char *aHost, int aPort)
{
    JELOG2(ESOCKET);
    PLOG4(ESOCKET, "Creating socket, proxy host = %s, proxy port = %d, host = %s, port = %d ",
          aProxy, aProxyPort, aHost, aPort);

    //Create TCP socket to proxy server

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(aProxyPort);

    if (!inet_aton(aProxy, &addr.sin_addr))
    {
        struct hostent* hp = gethostbyname(aProxy);
        addr.sin_addr.s_addr = ((struct in_addr*)(hp->h_addr))->s_addr;
    }

    int sd = socket(AF_INET, SOCK_STREAM, 0);
    LOG1(ESOCKET, EInfo, "fd = %d", sd);
    int rb = bind(sd, (struct sockaddr*) &addr, sizeof(addr));
    int rc = connect(sd, (struct sockaddr*) &addr, sizeof(addr));

    if (rc < 0)
        return -errno;  // could not connect to proxy server, return

    std::string string_host = aHost;
    LOG1(ESOCKET, EInfo, "ahost = %s", aHost);
    std::ostringstream tmpbuf;
    tmpbuf << aPort;
    std::string string_port = tmpbuf.str();

    std::string tmpst1 = "CONNECT " + string_host + ":";
    tmpst1 = tmpst1 + string_port;
    tmpst1 = tmpst1 + " HTTP/1.1\r\n\r\n";
    char *p1 = (char *) tmpst1.c_str();

    // Example p1 ="CONNECT www.google.com:443 HTTP/1.1\r\n\r\n"

    int res = send(sd, p1, strlen(p1), 0);

    if (res < 0)
        return -errno;

    LOG1(ESOCKET, EInfo, "HttpsConnection() tmp string %s", p1);
    LOG1(ESOCKET, EInfo, "send returned %d", res);
    LOG1(ESOCKET, EInfo, "NativeHttpsConnection::receiving using sd %d", sd);
    char response[1024];
    int rcnt = recv(sd, response, 1024, 0);
    LOG1(ESOCKET, EInfo, "recv returned %d", rcnt);
    response[rcnt] = 0; // null termination
    for (int i = 0; i < rcnt; i++)
        LOG1(ESOCKET, EInfo, "%c", response[i]);

    // for CONNECT request, the response line will be of the form "HTTP/1.1 200 Connection established\r\n"
    // check if the response code is 200, if it is not 200 then this website cannot be accessed via this proxy, so return error code

    std::string s3 = response;
    std::string pattern = "200";
    size_t found = s3.find(pattern);

    if (found != std::string::npos)
    {
        LOG1(ESOCKET, EInfo, "Proxy returned 200 Connection established response", 2);
        return sd;
    }
    else
        return -1; // error code


}
} // end namespace https

} // end namespace java
