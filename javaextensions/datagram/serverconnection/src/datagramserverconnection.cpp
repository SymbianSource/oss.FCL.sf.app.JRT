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

#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <wchar.h>
#include <string.h>

#include "logger.h"
#include "connectionlistener.h"
#include "javacommonutils.h"

#include "datagramserverconnection.h"
#include "datagramserverconnectionfactory.h"
#include "pushexception.h"
#include "pusherrorcodes.h"

#define INVALID_SOCKET (-1)
#define MAX_SIZE 256
#define BUFFER_SIZE 512

using namespace java::util;

DatagramServerConnection::DatagramServerConnection()
{
    LOG(ESOCKET, EInfo, "+DatagramServerConnection - default constructor");
    mListener = NULL,
                mThreadId = 0;
    mKeepRunning = false;
    mListenDatagram = INVALID_SOCKET;
    mAcceptDatagram = INVALID_SOCKET;
    mPort = 0;
    mIsNormalServerConnection = true;
    mError = 0;
    mOpenMonitor = 0;
}

OS_EXPORT DatagramServerConnection::DatagramServerConnection(
    const std::wstring& aUri, const std::wstring& aFilter) :
        mListener(0), mThreadId(0), mKeepRunning(false), mListenDatagram(
            INVALID_SOCKET), mAcceptDatagram(INVALID_SOCKET), mUri(aUri),
        mFilter(aFilter), mPort(0)
{
    JELOG2(ESOCKET);

    // sanity check for url
    ILOG1(ESOCKET,
          "++DatagramServerConnection::DatagramServerConnection() aUri = %s",
          aUri.c_str());
    mIsNormalServerConnection = false;
    mIsAppRunning = false;
    mError = 0;
    mOpenMonitor = 0;

    if (0 == mOpenMonitor)
    {
        mOpenMonitor = Monitor::createMonitor();
    }

    if (aUri.length() == 12)
        mPort = 0;
    else
    {
        std::wstring port = aUri.substr(12); // "datagram://:"
        mPort = JavaCommonUtils::wstringToInt(port);
    }
    LOG1(ESOCKET, EInfo, "created DatagramServerConnection on port %d", mPort);
}

OS_EXPORT DatagramServerConnection::~DatagramServerConnection()
{
    if (mOpenMonitor)
    {
        delete mOpenMonitor;
    }
    JELOG2(ESOCKET);
}

OS_EXPORT void DatagramServerConnection::open(ConnectionListener* aListener)
{
    JELOG2(ESOCKET);
    mListener = aListener;

    mKeepRunning = true;
    int rc = pthread_create(&mThreadId, NULL,
                            DatagramServerConnection::listenThread, this);
    mOpenMonitor->wait();

    ILOG2(ESOCKET,"after release in datagram rc = %dm mPort = %d",rc,mPort);
    if (rc < 0 || mError < 0)
    {
        ELOG1(ESOCKET,"ERROR!!! Datagram server conn already exists on Port %d : Open Failed",mPort);
        std::string errTxt("ERROR!!! Opening of Datagram Server failed ");
        throw PushException(COMMON_SRV_CONN_PLUGIN_ERROR,errTxt,__FILE__,  __FUNCTION__,__LINE__);
    }
}

void* DatagramServerConnection::listenThread(void* aParams)
{
    JELOG2(ESOCKET);
    struct sockaddr_in sender_addr;
    sender_addr.sin_family = AF_INET;
    unsigned int size = sizeof(sender_addr);
    bool match = false;

    DatagramServerConnection* pThis =
        reinterpret_cast<DatagramServerConnection*>(aParams);

    pThis->mListenDatagram = pThis->open(pThis->mPort);

    if (pThis->mListenDatagram < 0)
    {
        pThis->mError = -errno;
        (pThis->mOpenMonitor)->notify();
        pthread_exit(0);
    }
    (pThis->mOpenMonitor)->notify();


    int rt = -1;
    //char * addr = new char[MAX_SIZE];

    if (rt < 0 && pThis->mKeepRunning)
    {
        char *buf = new char[BUFFER_SIZE];
        // wchar_t * sender = new wchar_t[256];
        //char *  sender = new  char[256];
        while (match == false)
        {
            rt = recvfrom(pThis->mListenDatagram, buf, BUFFER_SIZE, 0,
                          (sockaddr*) &sender_addr, &size);
					  delete[] buf;
            //strcpy(sender,inet_ntoa(sender_addr.sin_addr));// Do we need this?
            //delete[] sender;
            ILOG2(ESOCKET, "pThis->mListenDatagram =  %d, port = %d",pThis-> mListenDatagram,pThis->mPort);

            // validate the filter
            /*if(strcmp((const char *)pThis->mFilter.c_str(),(const char *) NULL)== 0)
            {
                  LOG(ESOCKET,EInfo,"filter is NULL");
                    match = true;
            }
            else
            {
                if( strcmp((const char *)"*",(const char *)pThis->mFilter.c_str()) == 0)
                {
                    LOG(ESOCKET,EInfo,"filter = '*'");
                    match = true;
                }
                if( strcmp((const char *)"?",(const char *)pThis->mFilter.c_str()) == 0)
                {
                    LOG(ESOCKET,EInfo,"filter = '?'");
                    match = true;
                }
            }*/


            match = true;
        }
    }

    if (!(rt < 0)) // we have incoming connection
    {
        pThis->mAcceptDatagram = pThis->mListenDatagram;
        if (pThis->mIsAppRunning == true)
        {
            pThis->setActivityFlag(false);
            pthread_exit(0);
        }
        else
        {
            if (pThis->mListener)
            {
                pThis->mListener->msgArrived();
            }
            pThis->setActivityFlag(true);
        }
    }
    return 0;
}

OS_EXPORT int DatagramServerConnection::open()
{
    JELOG2(ESOCKET);

    mIsAppRunning = true;

    if (mKeepRunning) // we are listening for push connection
    {
        mKeepRunning = false;
        pthread_join(mThreadId, NULL);
    }
    else
    {
        mListenDatagram = open(mPort);
    }
    LOG2(ESOCKET,EInfo, "mListenDatagram =  %d, mPort = %d", mListenDatagram, mPort);
    setActivityFlag(false);
    return mListenDatagram;
}

int DatagramServerConnection::open(int aPort)
{
    JELOG2(ESOCKET);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons((unsigned short) aPort);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd == INVALID_SOCKET)
    {
        return -1;
    }

    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int res = bind(fd, (struct sockaddr*) &addr, sizeof(addr));

    // when server is opened on dynamic port, append the port to the url
    /*if(mPort == 0)
    {
            mPort = addr.sin_port;
            LOG1(ESOCKET, EInfo, "dynamic port : %d", mPort);
            std::wstring portString = JavaCommonUtils::intToWstring(mPort);
            mUri.append(portString);
            LOG1(ESOCKET, EInfo, "after appending mUri with dynamic port : %s", mUri);
    }*/
    LOG3(ESOCKET,EInfo, "server datgram sd =  %dm bind returns %d, mPort = %d ", fd, res, mPort);
    if (res != -1)
    {
        return fd;
    }
    LOG1(ESOCKET,EInfo, "-errno : %d" , -errno);
    close(fd);
    return -1;
}


OS_EXPORT void DatagramServerConnection::close()
{
    JELOG2(ESOCKET);



    ::close(mListenDatagram);
    ::close(mAcceptDatagram);

    if (mKeepRunning) // we are listening for push connection
    {
        mKeepRunning = false;
        pthread_join(mThreadId, NULL);
    }

    // reset members
    mListenDatagram = INVALID_SOCKET;
    mAcceptDatagram = INVALID_SOCKET;

    mListener = 0;
    mThreadId = 0;
    //mKeepRunning = false;
}

OS_EXPORT std::wstring DatagramServerConnection::getUri() const
{
    LOG(ESOCKET,EInfo, "DatagramServerConnection::getUri() ");
    return mUri;
}

OS_EXPORT std::wstring DatagramServerConnection::getFilter() const
{
    return mFilter;
}

OS_EXPORT void DatagramServerConnection::setFilter(const std::wstring& aFilter)
{
    JELOG2(ESOCKET);
    mFilter = aFilter;
}
OS_EXPORT bool DatagramServerConnection::isNormalServerConnection()
{
    return mIsNormalServerConnection;
}

OS_EXPORT void DatagramServerConnection::setNormalServerConnection()
{
    LOG(ESOCKET,EInfo,"Push is enabled");
    mIsNormalServerConnection = true;
}
int DatagramServerConnection::close(int fd)
{
    return ::close(fd);
}


/**
 * SetActivityFlag method  updates status whether
 * ServerConnection object has pending message(= message arrived but not yet read).
 * @param aFlag Updating logic of pending message flag:
 *                    - Set it 'true' when message arrives to the connection.
 *                    - Set it 'false' when ServerConnection::open() operation has been called.
 */
void DatagramServerConnection::setActivityFlag(bool aFlag)
{
    DatagramServerConnectionFactory& scf =
        DatagramServerConnectionFactory::getFactory();
    scf.setPendingMsgFlag(mUri, aFlag);
}
