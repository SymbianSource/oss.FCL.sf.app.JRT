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
* Description:  nativesocketserverconnection.h
*
*/



#ifndef NATIVESOCKETSERVERCONNECTION_H
#define NATIVESOCKETSERVERCONNECTION_H

#include "socketserverconnectionfactory.h"
#include "socketserverconnection.h"
#include "logger.h"
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <wchar.h>
#include <dirent.h>


namespace java
{

/**
* This is the base class for Native Peer of Server Sockets.
* SocketServerConnectionImpl in java will make calls to this class to open and listen for socket connections
* After acceptandopen of socket connection, the socket descriptor is passed to NativeSocketConnection class
* for further socket read and write operations
*/
class NativeSocketServerConnection
{

public:

    SocketServerConnection* iSocketServerPtr;

    NativeSocketServerConnection(const char *aUrl , int aMode, int aPort);

    /*
    * accepts the incoming connection
    */
    int accept();

    int close();

    // Interface functions


    /**
    * getLocalPort is called by SocketConnectionImpl to get the local port number to which this socket is connected.
    * During the initial socket connection establishment, a local port is assigned to the client side dynamically. This
    * function is used to retrieve this value.
    * @return the local port number. If error, then the
    * value of errno should be negated and returned. (return -errno)
    */

    int getLocalPort();

    /**
    * getLocalAddress is called by SocketConnectionImpl to get the local address to which this socket is bound.
    * When some data transmission occurs with this socket, the underlying OS will use any of the network interface to transmit the data.
    * One system can have more then one network interface configured to it. This function returns the network interface address which was
    * used for data transfer.
    * @return the local address. If an error occurs, then the
    *  string value "error" is returned.
    */

    int getLocalAddress(char * aAddress);

private:
    int mSD;

};

}

#endif // NATIVESOCKETSERVERCONNECTION_H
