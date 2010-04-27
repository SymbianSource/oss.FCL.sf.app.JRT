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
* Description:
 *
*/


#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "logger.h"
#include "nativedatagramconnection.h"
#include "javacommonutils.h"
#include "apnsettings.h"
#include "pushexception.h"
#include "pusherrorcodes.h"


using namespace java;
using namespace java::util;

NativeDatagramConnection::NativeDatagramConnection(const char *aName,
        int aMode, const char* aHost, int aPort)
{
    ILOG1(ESOCKET,
          "NativeDatagramConnection::NativeDatagramConnection() , host = %s",
          aHost);
    mName = NULL;
    mHost = NULL;
    mDatagramServerFactory = NULL;
    mDatagramServerPtr = NULL;
    mName = new char[strlen(aName) + 1];
    strcpy(mName, aName);
    LOG1(ESOCKET, EInfo," name = %s",aName);
    mMode = aMode;
    LOG1(ESOCKET, EInfo, "before   , aHost + %s", aHost);
    mHost = new char[strlen(aHost) + 1];
    strcpy(mHost, aHost);
    LOG2(ESOCKET, EInfo, "after  mHost = %s , aHost + %s", mHost, aHost);
    mPort = aPort;
}

int NativeDatagramConnection::openConnection(int aType, int aApn, int * err)
{
    JELOG2(ESOCKET);
    int ret  =0;
    if (strcmp(mHost, "") == 0)
    {
        LOG(ESOCKET, EInfo, "Datagram server connection");
        mDatagramServerFactory =
            &DatagramServerConnectionFactory::getFactory();

        mUrl = JavaCommonUtils::utf8ToWstring((const char *) mName);
        try
        {
            mDatagramServerPtr = reinterpret_cast<DatagramServerConnection*>(mDatagramServerFactory->create(
                                     mUrl));
        }
        catch (PushException& ex)
        {
            LOG(ESOCKET, EInfo, "ERROR!!! CreateConnection() PushException was caught");
            return -1;
        }
        if (mDatagramServerPtr != NULL)
        {
            mSockDesc = mDatagramServerPtr->open();
        }
        else
        {
            LOG(ESOCKET, EInfo, "mDatagramServerPtr is null ");
            return -1;
        }
    }
    else
    {

        LOG(ESOCKET, EInfo, "Datagram client connection");
        mSockDesc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        if (aApn != -1)
        {
            // remove comments if open c patch available
            ret = ApnSettings::setDefaultApn(aType,aApn);
        }
    }
    LOG1(ESOCKET, EInfo, " NativeDatagramConnection::openConnection :: mSockDesc = ",mSockDesc);
    *err = ret;
    return mSockDesc;
}

int NativeDatagramConnection::sendDatagram(JNIEnv &aJni,
        jbyteArray aJavaBuffer, int aOffset, int aLength,
        const char *aDestAddr, int aDestPort)
{
    JELOG2(ESOCKET);
    char * writeBuffer = NULL;
    int retVal;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(aDestPort);

    int requiredNativeBufferLength = aLength - aOffset;
    // Allocate native buffer
    if (writeBuffer == NULL)
    {
        writeBuffer = new char[requiredNativeBufferLength + 1];
    }
    /* Copy the data to be written from java buffer to the native buffer.  */
    JNIArrayUtils::CopyToNative(aJni, aJavaBuffer, aOffset,
                                requiredNativeBufferLength, writeBuffer);

    if (!inet_aton(aDestAddr, &addr.sin_addr))
    {
        struct hostent* hp = gethostbyname(aDestAddr);
        addr.sin_addr.s_addr = ((struct in_addr*)(hp->h_addr))->s_addr;
    }
    LOG1(ESOCKET, EInfo, "send buffer is %s", writeBuffer);
    retVal = sendto(mSockDesc, writeBuffer, requiredNativeBufferLength, 0,
                    (struct sockaddr*) &addr, sizeof(addr));
    LOG1(ESOCKET, EInfo, "number of bytes sent is %d ", retVal);
    if (writeBuffer != NULL)
    {
        delete[] writeBuffer;
    }

    if (retVal < 0)
    {
        return -errno;
    }
    else
    {
        return retVal;
    }

}


int NativeDatagramConnection::receiveDatagram(JNIEnv &aJni,
        jbyteArray aJavaBuffer, int aOffset, int aLength, char* aSenderAddr,
        int * aSenderPort)
{
    JELOG2(ESOCKET);
    char * readBuffer = NULL;
    struct sockaddr_in sender_addr;
    sender_addr.sin_family = AF_INET;
    unsigned int size = sizeof(sender_addr);
    int retVal;
    int requiredNativeBufferLength = aLength - aOffset;
    if (readBuffer == NULL)
    {
        readBuffer = new char[requiredNativeBufferLength + 2];
    }
    int bytesReceived = recvfrom(mSockDesc, readBuffer,
                                 requiredNativeBufferLength, 0, (struct sockaddr*) &sender_addr,
                                 &size);

    if (bytesReceived < 0)
    {
        retVal = -errno;
    }
    else
    {
        retVal = bytesReceived;
        strcpy(aSenderAddr, inet_ntoa(sender_addr.sin_addr));
        *aSenderPort = ntohs(sender_addr.sin_port);

        ILOG2(ESOCKET,
              "NativeDatagramConnection::receiveDatagram() senderaddress =  %s, port= %d",
              aSenderAddr, aSenderPort);

        JNIArrayUtils::CopyToJava(aJni, readBuffer, bytesReceived, aJavaBuffer,
                                  aOffset, bytesReceived);
    }
    delete[] readBuffer;
    return retVal;

}

int NativeDatagramConnection::getLocalAddress(char * aAddress)
{
    JELOG2(ESOCKET);
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int retval;
    LOG1(ESOCKET, EInfo,
         "NativeDatagramConnection::getLocalAddress() : mSockDesc =  : %d",
         mSockDesc);
    if (getsockname(mSockDesc, (struct sockaddr*) &addr, &len) < 0)
    {
        LOG1(
            ESOCKET,
            EInfo,
            "NativeDatagramConnection::getLocalAddress() : getlocaladdress failed : %d",
            errno);
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

int NativeDatagramConnection::getLocalPort()
{
    JELOG2(ESOCKET);
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int retval;

    if ((getsockname(mSockDesc, (struct sockaddr*) &addr, &len)) < 0)
    {
        LOG1(
            ESOCKET,
            EInfo,
            "NativeDatagramConnection::getLocalPort() : getlocalport failed : %d",
            errno);
        retval = -errno;
    }
    else
    {
        retval = ntohs(addr.sin_port);
    }
    return retval;
}

// called only via midlet, when UDPDatagrmaConnection.close() is called
int NativeDatagramConnection::datagramClose()
{
    JELOG2(ESOCKET);
    int retValue;
    if (strcmp(mHost, "") == 0) // server connection
    {
        //retValue = close(mSockDesc);
        if (mUrl.length() != 12)
        {
            if (mDatagramServerFactory != NULL)
            {
                mDatagramServerFactory->releaseConnection(mUrl);
                mDatagramServerFactory = NULL;
                mDatagramServerPtr = NULL;
            }
        }
        else          // is it is normal server connection, then just close the socket
        {
            if (mDatagramServerPtr->isNormalServerConnection() == true)
            {
                retValue = ::close(mSockDesc);
                if (retValue == -1)
                {
                    LOG1(ESOCKET, EInfo,
                         "NativeDatagramConnection::datagramClose() : socket close failed : %d",
                         errno);
                    return -errno;
                }
                else
                {
                    LOG1(ESOCKET, EInfo,
                         "NativeDatagramConnection::setsocketoption  setsockopt errno %d",
                         -errno);
                    return retValue;
                }
            }
            else   // it is added to push db so call releaseconnection
            {
                mDatagramServerFactory->releaseConnection(mUrl);
                mDatagramServerFactory = NULL;
                mDatagramServerPtr = NULL;
            }
        }
        return 0;
    }
    else
    {
        retValue = close(mSockDesc);
        if (retValue == -1)
        {
            LOG1(ESOCKET,
                 EInfo,
                 "NativeDatagramConnection::socketClose() : socket close failed : %d",
                 errno);
            return -errno;
        }
        else
        {
            LOG1(ESOCKET, EInfo,
                 "NativeDatagramConnection::setsocketoption  setsockopt errno %d",
                 -errno);
            return retValue;
        }
    }
}

NativeDatagramConnection::~NativeDatagramConnection()
{
    JELOG2(ESOCKET);
    if (mName != NULL)
    {
        delete[] mName;
    }
    if (mHost != NULL)
    {
        delete[] mHost;
    }
}

