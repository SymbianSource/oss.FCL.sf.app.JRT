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

#ifndef DATAGRAMSERVERCONNECTION_H
#define DATAGRAMSERVERCONNECTION_H
#include <string>

#include "serverconnection.h"
#include "javaosheaders.h"
#include "monitor.h"

using namespace java::push;
/**
* This is the base class for Native Peer of datagram socket.
* NativeDatagramServerConnection will make calls to this class to create and carry out the all server datagram operations.
*/

class DatagramServerConnection : public ServerConnection
{

public:
    DatagramServerConnection();
    OS_IMPORT DatagramServerConnection(const std::wstring& aUri,const std::wstring& aFilter);
    OS_IMPORT virtual ~DatagramServerConnection();

    OS_IMPORT virtual void open(ConnectionListener* aListener);
    virtual void close();
    virtual std::wstring getUri() const;
    virtual std::wstring getFilter() const;
    OS_IMPORT virtual void setFilter(const std::wstring& aFilter);

    OS_IMPORT int open();
    OS_IMPORT void  setNormalServerConnection();
    OS_IMPORT bool isNormalServerConnection();

private:
    /**
    * open is called by listening thread to create a new datagram socket on the localhost to listen on the specified port.
    * @param[in]  aPort: port on which server listens.
    * @return 0 on successful completion.. If error, then the
    *         value of errno should be negated and returned. (return -errno)
    */
    int open(int aPort);

    /**
    * close , to close the server connection
    * @param[in]  fd: server socket descriptor.
    * @return 0 on successful completion.. If error, then the
    *         value of errno should be negated and returned. (return -errno)
    */

    int close(int fd);

    void setActivityFlag(bool aFlag);

    static void* listenThread(void* params);

private:
    ConnectionListener* mListener;

    pthread_t mThreadId;
    bool mKeepRunning;

    int mListenDatagram;
    int mAcceptDatagram;
    std::wstring mUri;
    std::wstring mFilter;
    int mPort;
    bool mIsAppRunning;
    bool mIsNormalServerConnection;
    java::util::Monitor* mOpenMonitor;
    int mError;
};

#endif // DATAGRAMSERVERCONNECTION_H
