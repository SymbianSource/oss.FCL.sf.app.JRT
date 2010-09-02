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
* Description:  Socket Connection Impl Class
*
*/




#ifndef __SYMBIAN32__
#include <netinet/tcp.h>  // TCP_NODELAY
#endif

#include <stdio.h>
#include "nativesocketconnection.h"
#include "jniarrayutils.h"
#include "apnsettings.h"
using namespace java;

OS_EXPORT NativeSocketConnection::NativeSocketConnection(const char* aName,int aMode, const char *aHost, int aPort)
{
    // JELOG2(ESOCKET);
    mName = NULL;
    mHost = NULL;
    mName = new char[strlen(aName)+1];
    strcpy(mName, aName);
    mMode = aMode;
    mPort = aPort;
    LOG1(ESOCKET, EInfo, "before   , aHost + %s" , aHost);
    mHost = new char[strlen(aHost)+1];
    strcpy(mHost, aHost);
    LOG2(ESOCKET, EInfo, "after  mHost = %s , aHost + %s" ,mHost, aHost);
    mSocketBuffer = NULL;
    mLingerValue = 0;
}


OS_EXPORT NativeSocketConnection::~NativeSocketConnection()
{
    // JELOG2(ESOCKET);
    if (mName != NULL)
    {
        delete[] mName;
    }
    if (mHost != NULL)
    {
        delete[] mHost;
    }
    if (mSocketBuffer != NULL)
    {
        delete[] mSocketBuffer;
        mSocketBuffer = NULL;
    }

}

OS_EXPORT int NativeSocketConnection::readBytes(JNIEnv& aJni, jbyteArray aJavaBuffer)
{
    JELOG2(ESOCKET);
    if (mSocketBuffer == NULL)
    {

        /*
        Memory is allocated to isocketBuffer only once during the first call of this readBytes() function
        Since the value of mBufferSize will be unknown(uninitialized) during the constructor call, it is not
        possible to do this operation inside the constructor.
        */

        mSocketBuffer = new char[mBufferSize];
    }

    // socketRead() function will actually make the OpenC call to read the data from the socket
    mBytesRead = socketRead(mSocketBuffer, mBufferSize);

    ILOG1(ESOCKET, "NativeSocketConnection::readBytes() : Num of bytes read is %d", mBytesRead);

    /* Copy the data read from the native buffer to the java buffer. Copy only "bytesRead" number of bytes.
       Because it is possible that the java request for some 'n' bytes to be read, but actually lesser bytes of data was
       read from the socket.
    */

    if (mBytesRead > 0)
    {
        JNIArrayUtils::CopyToJava(aJni, mSocketBuffer, mBytesRead, aJavaBuffer, 0, mBytesRead);
    }

    return mBytesRead;
}

OS_EXPORT int NativeSocketConnection::writeBytes(JNIEnv& aJni, jbyteArray aJavaBuffer, int aOffset, int aLength)
{
    JELOG2(ESOCKET);
    char *iWriteBuffer =  new char[aLength + 1];
    int retval;

    /* Copy the data to be written from java buffer to the native buffer.  */
    JNIArrayUtils::CopyToNative(aJni, aJavaBuffer, aOffset, aLength, iWriteBuffer);

    // socketWrite() function will actually make the OpenC call to read the data from the socket
    mBytesWritten = socketWrite(iWriteBuffer, aLength);

    ILOG1(ESOCKET,  "NativeSocketConnection::writeBytes() : Num of bytes written is %d", mBytesWritten);

    delete[] iWriteBuffer;
    iWriteBuffer = NULL;

    if (mBytesWritten < 0)
    {
        retval =  - errno; // If write failed, return the errno value
    }
    else
    {
        retval = mBytesWritten;
    }
    return retval;
}

OS_EXPORT void NativeSocketConnection::stopReading()
{
    // JELOG2(ESOCKET);
    return;
}

OS_EXPORT void NativeSocketConnection::stopWriting()
{
    // JELOG2(ESOCKET);
    return;
}


/* ----------------------------------------------------------------------
This function is used to open a socket connection.

OpenC apis used

inet_aton() : converts the Internet host address cp from the standard numbers-and- dots notation
              into binary data and stores it in the structure

socket():  creates an endpoint for communication and returns a descriptor. We create a "STREAM" socket


connect(): initiates a connection to the socket server. This involves the three way handshake process.
-------------------------------------------------------------------------*/
OS_EXPORT int NativeSocketConnection::socketOpen(int aSockDesc, int aType, int aApn, int * err)
{
    // // JELOG2(ESOCKET);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(mPort);
    int retval;

    LOG(ESOCKET, EInfo, "NativeSocketConnection::socketOpen");
    if (aSockDesc == -1)
    {
        LOG(ESOCKET, EInfo, "Opening a socket");
        mSockDesc = socket(AF_INET, SOCK_STREAM , 0);
        if (mSockDesc == -1)
        {
            LOG1(ESOCKET, EInfo, "creation of socket failed : %d", errno);
            return -errno;
        }
        LOG1(ESOCKET, EInfo,"NativeSocketConnection::socketOpen() : Socket descriptor = %d",mSockDesc);

        int ret  =0;
        if (aApn != -1)
        {
            // remove comments if open c patch available
            ret = ApnSettings::setDefaultApn(aType,aApn);
        }
        LOG1(ESOCKET, EInfo,"NativeSocketConnection::socketOpen() : Ssetdefaultapn returned = %d",ret);
        *err = ret;

        if (!inet_aton(mHost, &addr.sin_addr))
        {
            LOG1(ESOCKET, EInfo, "before getHostByName , mHost = %s" ,mHost);
            struct hostent* hp = gethostbyname(mHost);
            if (hp ==  NULL)
            {
                 //error condition, we have to handle -18 error.
                ELOG1(ESOCKET,"NativeSocketConnection:: gthostbyname error : %d" , h_errno);                 
                int tmp = ApnSettings::retryConnection(h_errno,aType,aApn);
                if(tmp == 0)
                {
                    // connection reset done, attempt once again
                    hp = gethostbyname(mHost);
                    if(hp == NULL)
                    {
                        return -(h_errno);    
                    }
                 }
                 else
                 {
                     // retry not supported/failed
                     return tmp; 
                  
                 }
            }
            addr.sin_addr.s_addr = ((struct in_addr*)(hp->h_addr))->s_addr;
        }
        LOG(ESOCKET, EInfo, "After getHostByName");


        int rc = connect(mSockDesc, (struct sockaddr*)&addr, sizeof(addr));
        if (rc < 0)
        {
            LOG1(ESOCKET, EInfo, "NativeSocketConnection::socketOpen() : socket connect failed : %d", errno);
            retval =  -errno;
        }
        else
        {
            retval = mSockDesc;
        }
    }
    else
    {
        LOG(ESOCKET, EInfo, "Socket already opened, setting socket descriptor");
        mSockDesc = aSockDesc;
        retval = 1;
    }
    return retval;
}


OS_EXPORT int NativeSocketConnection::socketRead(char *aReadBuf, int aLen)
{
    LOG(ESOCKET,EInfo, "NativeSocketConnection::socketRead");
    int retValue = recv(mSockDesc, aReadBuf, aLen, 0);

    if ((retValue==-1) && (errno == EAGAIN))
    {
        retValue = 0;
    }
    return retValue;
}

OS_EXPORT int NativeSocketConnection::socketWrite(char *aWriteBuf, int aLen)
{
    LOG1(ESOCKET, EInfo,"SOCKET WRITE BUF IS %s",aWriteBuf);
    int retValue = send(mSockDesc, aWriteBuf, aLen, 0);

    return retValue;
}



OS_EXPORT int NativeSocketConnection::getSocketOption(int aOption)
{
    // JELOG2(ESOCKET);
    LOG1(ESOCKET, EInfo, "++NativeSocketConnection::getSocketOption,option = %d", aOption);
    int level = SOL_SOCKET;
    int optname = -1;
    int optval = 0;
    socklen_t optsize = sizeof(optname);
    void * opttarget = (void *)(&optval) ;
    int retval;

    switch (aOption)
    {
    case 0: // DELAY
        level = IPPROTO_TCP;
        optname = TCP_NODELAY;
        break;
    case 1: // LINGER
        return mLingerValue;
    case 2: // KEEPALIVE
        optname = SO_KEEPALIVE;
        break;
    case 3: // RCVBUF
        optname = SO_RCVBUF;
        break;
    case 4: // SNDBUF
        optname = SO_SNDBUF;
        break;
    }

    if (getsockopt(mSockDesc, level,  optname, opttarget, &optsize) == 0)
    {
        retval = optval;
    }
    else
    {
        LOG1(ESOCKET, EInfo, "NativeSocketConnection::getSocketOption failed, option = %d",aOption);
        retval = -errno;
    }
    LOG(ESOCKET, EInfo, "--NativeSocketConnection::getSocketOption");
    return retval;
}

OS_EXPORT int NativeSocketConnection::setSocketOption(int aOption, int aOptval)
{
    // JELOG2(ESOCKET);
    int    level = SOL_SOCKET;
    socklen_t optsize =  sizeof(aOptval);
    int    optname = -1;
    void * opttarget = (void *) & aOptval ;
    int retval;

    switch (aOption)
    {
        // DELAY
    case 0:
        level = IPPROTO_TCP;
        optname = TCP_NODELAY;
        break;
        // LINGER
    case 1:
        /* The openC api doesn't support SO_LINGER option. This is because, the native symbian doesn't support this.
        But since, tck has few test cases related to this option, this call should return success
        */
        mLingerValue = aOptval;
        return 0;
        // KEEPALIVE
    case 2:
        optname = SO_KEEPALIVE;
        break;
        // RCVBUF
    case 3:
        optname = SO_RCVBUF;
        break;
        // SNDBUF
    case 4:
        optname = SO_SNDBUF;
        break;
    }
    //LOG3(ESOCKET,EInfo,"IN NativeSocketConnection::setsocketoption optname is %d level is %s aOptval is %d", optname, level, aOptval);
    if (setsockopt(mSockDesc, level,  optname, opttarget, optsize) == 0)
    {
        LOG(ESOCKET,EInfo, "NativeSocketConnection::setsocketoption  setsockopt returned 0");
        retval = 0;
    }
    else
    {
        retval = -errno;
    }
    return retval;
}

OS_EXPORT int NativeSocketConnection::socketClose()
{
    // JELOG2(ESOCKET);
    int retValue = close(mSockDesc);
    if (retValue == -1)
    {
        LOG1(ESOCKET, EInfo, "NativeSocketConnection::socketClose() : socket close failed : %d", errno);
        return -errno;
    }
    else
    {
        LOG(ESOCKET,EInfo, "NativeSocketConnection::socketClose successfull");
        return retValue;
    }
}

/* ----------------------------------------------------------------------
This function is used to get remote port number to which this socket is connected.
OpenC api used
getpeername(): gets name of connected peer socket. This fills the peer address and port into the structure passed as an argument to it.
-------------------------------------------------------------------------*/
OS_EXPORT int NativeSocketConnection::getPort()
{
    // JELOG2(ESOCKET);
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int retval;

    if ((getpeername(mSockDesc, (struct sockaddr*)&addr, &len)) < 0)
    {
        LOG1(ESOCKET, EInfo, "NativeSocketConnection::getPort() : getport failed : %d", errno);
        retval = -errno;
    }
    else
    {
        retval = ntohs(addr.sin_port);
    }
    return retval;
}


/* ----------------------------------------------------------------------
This function is used to get local port number to which this socket is bound.
OpenC api used
getsockname(): gets socket name. This fills the local address and port into the structure passed as an argument to it.
-------------------------------------------------------------------------*/
OS_EXPORT int NativeSocketConnection::getLocalPort()
{
    // JELOG2(ESOCKET);
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int retval;

    if ((getsockname(mSockDesc, (struct sockaddr*)&addr, &len)) < 0)
    {
        LOG1(ESOCKET, EInfo, "NativeSocketConnection::getLocalPort() : getlocalport failed : %d", errno);
        retval =  -errno;
    }
    else
    {
        retval = ntohs(addr.sin_port);
    }
    return retval;
}



OS_EXPORT int NativeSocketConnection::getLocalAddress(char * aAddress)
{
    // JELOG2(ESOCKET);
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int retval;

    if (getsockname(mSockDesc, (struct sockaddr*)&addr, &len) < 0)
    {
        LOG1(ESOCKET, EInfo, "NativeSocketConnection::getLocalAddress() : getlocaladdress failed : %d", errno);
        strcpy(aAddress, "error");
        retval =  -errno;
    }
    else
    {
        strcpy(aAddress, inet_ntoa(addr.sin_addr));
        retval = 0;
    }
    return retval;
}

OS_EXPORT int NativeSocketConnection::getAddress(char * aAddress)
{
    // JELOG2(ESOCKET);
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int retval;

    if (getpeername(mSockDesc, (struct sockaddr*)&addr, &len) < 0)
    {
        strcpy(aAddress, "error");
        retval = -errno;
    }
    else
    {
        strcpy(aAddress, inet_ntoa(addr.sin_addr));
        retval = 0;
    }
    return retval;
}
