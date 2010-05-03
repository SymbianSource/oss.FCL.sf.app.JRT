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

#ifndef NATIVEDATAGRAMSERVERCONNECTION_H
#define NATIVEDATAGRAMSERVERCONNECTION_H

#include "javaosheaders.h"
#include <logger.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <wchar.h>
#include <sys/socket.h>
#include "datagramserverconnection.h"
#include "datagramserverconnectionfactory.h"

namespace java
{
class NativeDatagramServerConnection
{
public:

    //  void send_upd_packet(char *buf,int offset,int length,const char *addr);
    //void receive_upd_packet(char *buf,int *off,int *len);


private:
    int sd;

protected:

public:
    int Open(const char *aUrl, int aMode, int aPort);
    DatagramServerConnection* mDatagramServerPtr;
    int close();

    // Interface functions
    int getLocalPort();
    char * getLocalAdress();
};
}
#endif // NATIVEDATAGRAMSERVERCONNECTION_H
