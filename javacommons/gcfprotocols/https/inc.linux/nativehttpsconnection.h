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

#ifndef NATIVEHTTPSCONNECTION_H
#define NATIVEHTTPSCONNECTION_H

#include<sys/types.h>
#include<sys/stat.h>
#include <wchar.h>
#include <sys/socket.h>
#include "javaosheaders.h"
#include <logger.h>

namespace java
{
namespace https
{

/**
 * This class is used to write the "CONNECT" request to the proxy server.
 * It will be used only when there is a proxy server present. HttpsConnectionImpl from java will call the function of this
 * class to create a normal tcp socket to the proxy. This socket will then be made a secure socket.
*/

class NativeHttpsConnection
{
public :

    /**
    * createSocket is called by HttpsConnectionImpl java class to create a tcp socket and issue a CONNECT request.
    *
    * @param aProxy: Proxy server address
    * @param aProxyPort: Proxy server port
    * @param aHost: Destination host address, to which https connection is opened
    * @param aPort: Destination port number
    * @return the tcp socket descriptor
    */
    int createSocket(const char *aProxy, int aProxyPort, const char *aHost, int aPort);

};
} // end namespace https
}  // end namespace java
#endif // NATIVEHTTPSCONNECTION_H
