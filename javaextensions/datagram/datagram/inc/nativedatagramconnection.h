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
* Description:  NativeDatagramConnection.h
*
*/


#ifndef NATIVEDATAGRAMCONNECTION_H
#define NATIVEDATAGRAMCONNECTION_H

#include "datagramserverconnection.h"
#include "datagramserverconnectionfactory.h"
#include "javaosheaders.h"
#include "logger.h"
#include "jniarrayutils.h"
#include <jni.h>


namespace java
{
/**
* This is the base class for Native Peer of datagrams.
* DatagramConnectionImpl in java will make calls to this class to create a carry out the all datagram operations.
*/
class NativeDatagramConnection
{
public:
    NativeDatagramConnection(const char *name,int mode, const char* host,int port);
    ~NativeDatagramConnection();
    int sendDatagram(JNIEnv &aJni,jbyteArray aJavaBuffer,int aOffset,int aLength,const char *aDestAddr,int aDestPort);
    int receiveDatagram(JNIEnv &aJni,jbyteArray aJavaBuffer,int aOffset,int aLength,char* aSenderAddr, int * aSenderPort);
    int openConnection(int aType, int aApn, int * err);

    /**
    * getLocalAddress is called by SocketConnectionImpl to get the local address to which this datagram is bound.
    * When some data transmission occurs with this datagram, the underlying OS will use any of the network interface to transmit the data.
    * One system can have more then one network interface configured to it. This function returns the network interface address which was
    * used for data transfer.
    * @return the local address. If an error occurs, then the
    *  string value "error" is returned.
    */

    int getLocalAddress(char * aAddress);

    /**
    * getLocalPort is called by DatagramConnectionImpl to get the local port number to which this datagram is connected.
    * During the initial datagram connection establishment, a local port is assigned to the client side dynamically. This
    * function is used to retrieve this value.
    * @return the local port number. If error, then the
    *         value of errno should be negated and returned. (return -errno)
    */
    int getLocalPort();

    /**
    * closes the udp socket
    */
    int datagramClose();



private:
    int mSockDesc;          // datagram descriptor
    char * mName;
    std::wstring mUrl;
    int mMode;
    char * mHost;
    int mPort;
    DatagramServerConnectionFactory* mDatagramServerFactory;
    DatagramServerConnection* mDatagramServerPtr;

};
}
#endif // NATIVEDATAGRAMCONNECTION_H
