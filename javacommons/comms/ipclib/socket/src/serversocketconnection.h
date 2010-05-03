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

#ifndef SERVERSOCKETCONNECTION_H
#define SERVERSOCKETCONNECTION_H

#include <pthread.h>
#include <list>
#include <sys/select.h>

#include "javaosheaders.h"
#include "scopedlocks.h"

#include "serversocket.h"
#include "transport.h"

namespace java
{
namespace comms
{

class SocketConnection;

OS_NONSHARABLE_CLASS(ServerSocketConnection) : public IpcServerConnectionInterface
{
public:
    ServerSocketConnection(IpcListener* aListener);
    virtual ~ServerSocketConnection();

    // IpcServerConnectionInterface methods
    virtual int start(int aPort);
    virtual void stop();
    virtual int send(ipcMessage_t* aMsg);

private:
    static void* messageLoop(void* params);
    int select();

    int handleNewConnection(fd_set& aSet);
    void handleRead(fd_set& aSet);
    void createSelectSet(fd_set& aSet, int& aMaxFd);
    void removeConnections();

    pthread_t mThreadId;
    IpcListener* mListener;

    int mKeepRunning;
    ServerSocket mListenSocket;

    typedef std::list<SocketConnection*> connections_t;
    connections_t mSockets; // access protected by mSocketsMutex
    java::util::ScopedMutex mSocketsMutex;

    int mPort;
};

} // namespace comms
} // namespace java

#endif // SERVERSOCKETCONNECTION_H
