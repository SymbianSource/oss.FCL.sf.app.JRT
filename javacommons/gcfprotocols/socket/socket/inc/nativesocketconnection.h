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
* Description:  nativesocketconnection.h
 *
*/


#ifndef NATIVESOCKETCONNECTION_H
#define NATIVESOCKETCONNECTION_H

#include <sys/types.h>
#include <sys/stat.h>
#include <wchar.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <wchar.h>
#include <dirent.h>

#include "nativestreambase.h"
#include "logger.h"
#include "javaosheaders.h"

namespace java
{

/**
 * This is the base class for Native Peer of sockets.
 * SocketConnectionImpl in java will make calls to this class to create a carry
 * out the all socket operations.
 * StreamConnectionBase in java will also call the readBytes() and writeBytes()
 * methods of this class to get/send data from the native socket buffer.
 * NativeStreamBase will have stored a handle to the Object and uses it to call
 * these methods.
 */
class NativeSocketConnection: public NativeStreamBase
{

public:

    OS_IMPORT NativeSocketConnection(const char* aName, int aMode,
                                     const char *aHost, int aPort);

    // Methods from NativeStreamBase

    /**
     * ReadBytes is called by StreamConnectionBase when the buffer at
     * input stream buffer runs out of data.
     * @param[in]  aJavaBuffer: Byte array to which the data read is copied into
     * @return the number of bytes read. If error, then the
     *         value of errno should be negated and returned. (return -errno)
     */
    OS_IMPORT int readBytes(JNIEnv& aJni, jbyteArray aJavaBuffer);

    /**
     * writeBytes is called by StreamConnectionBase when the output stream
     * buffer in java overflows
     * It is also called if flush() is called at the Java side.
     * @param[in]  aJavaBuffer: A byte array of length aLength
     * @return the number of bytes written. If error, then the
     *         value of errno should be negated and returned. (return -errno)
     */
    OS_IMPORT int writeBytes(JNIEnv& aJni, jbyteArray aJavaBuffer, int aOffset,
                             int aLength);

    /**
     * StopReading is called by StreamConnectionBase once InputStream is closed
     * from the Application.
     */
    OS_IMPORT void stopReading();

    /**
     * StopWriting is called by StreamConnectionBase OutputStream is closed
     * from the Application.
     */
    OS_IMPORT void stopWriting();

    /**
     * socketWrite is internal function called by writeBytes() method.
     * @param[in]  aWriteBuf: A character array of length len holding the data
     *             to be written.
     * @return the number of bytes written. If error, then the
     *         value of errno should be negated and returned. (return -errno)
     */
    OS_IMPORT int socketWrite(char * aWriteBuf, int aLen);

    /*
     * socketOpen calls the socket open Native API
     * returns the socket descriptor on successful completion.
     * If error, then the value os errno should be negated and returned
     * (return -errno)
     */
    OS_IMPORT int socketOpen(int aSockDesc, int aType, int aApn, int * err);

    /**
     * socketRead is internal function called by readBytes() method.
     * @param[in]  p: A character array of length len.
     * @return the number of bytes read. If error, then the
     *         value of errno should be negated and returned. (return -errno)
     */
    OS_IMPORT int socketRead(char * aReadBuf, int aLen);

    /**
     * socketClose is called by SocketConnectionImpl to close socket connection.
     * @return 0 on successful completion. If error, then the
     *         value of errno should be negated and returned. (return -errno)
     */
    OS_IMPORT int socketClose();

    // Interface functions

    /**
     * getLocalPort is called by SocketConnectionImpl to get the local port
     * number to which this socket is connected.
     * During the initial socket connection establishment, a local port is
     * assigned to the client side dynamically. This function is used to retrieve
     * this value.
     * @return the local port number. If error, then the
     *         value of errno should be negated and returned. (return -errno)
     */
    OS_IMPORT int getPort();

    /**
     * getLocalPort is called by SocketConnectionImpl to get the local port
     * number to which this socket is connected.
     * During the initial socket connection establishment, a local port is
     * assigned to the client side dynamically. This function is used to
     * retrieve this value.
     * @return the local port number. If error, then the
     *         value of errno should be negated and returned. (return -errno)
     */
    OS_IMPORT int getLocalPort();

    /**
     * getLocalAddress is called by SocketConnectionImpl to get the local
     * address to which this socket is bound.
     * When some data transmission occurs with this socket, the underlying OS
     * will use any of the network interface to transmit the data.
     * One system can have more then one network interface configured to it.
     * This function returns the network interface address which was used for
     * data transfer.
     * @return the local address. If an error occurs, then the
     *         string value "error" is returned.
     */
    OS_IMPORT int getLocalAddress(char * aAddress);

    /**
     * getAddress is called by SocketConnectionImpl to get the remote address to
     * which this socket is connected.
     * This is the address which was used during the initial socket connection
     * establishment.
     * @parmam[out] aAddress: the localaddress is copied to aAddress.
     * @return 0 on successful completion. If error, then the
     *         value of errno should be negated and returned. (return -errno)
     */
    OS_IMPORT int getAddress(char * aAddress);

    /**
     * getSocketOption is called by SocketConnectionImpl to retrieve socket
     * options
     * @param[in]  option: socket option identifier (KEEPALIVE, LINGER, SNDBUF,
     *             RCVBUF, or DELAY
     * @return the numeric value for specified option. If error, then the
     *         value of errno should be negated and returned. (return -errno)
     */
    OS_IMPORT int getSocketOption(int aOption);

    /**
     * setSocketOption is called by SocketConnectionImpl to set the socket
     * options at the native layer
     * @param[in]  flag: socket option identifier (KEEPALIVE, LINGER, SNDBUF,
     *             RCVBUF, or DELAY
     * @param[in]  optval:  the value for the specified input socket option
     * @return 0 on successful completion. If error, then the
     *         value of errno should be negated and returned. (return -errno)
     */
    OS_IMPORT int setSocketOption(int aOption, int aOptval);

    /*
     * Class Destructor - deletes the memory allocated if any
     */
    OS_IMPORT ~NativeSocketConnection();

protected:
    int mSockDesc;

private:
    int mBytesRead;      // Used to store the actual number of bytes read
    int mBytesWritten;
    int mPort;           // The remort port to which the socket connection was made.
    int mMode;           // The mode of socket connection.
    char *mName;
    char *mHost;         // The remort host to which the socket connection was made.
    char *mSocketBuffer; // The internal buffer which is used to store the data
    // during read operation.
    int mLingerValue;    // The SO_LINGER socket option value.
};

} // java namespace

#endif // NATIVESOCKETCONNECTION_H
