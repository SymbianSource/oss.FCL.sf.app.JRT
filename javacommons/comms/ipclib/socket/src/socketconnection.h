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

#ifndef SOCKETCONNECTION_H
#define SOCKETCONNECTION_H

#include <pthread.h>

#include "javaosheaders.h"
#include "scopedlocks.h"

#include "socket.h"
#include "transport.h"

namespace java
{
namespace comms
{

OS_NONSHARABLE_CLASS(SocketConnection) : public IpcConnectionInterface
{
public:
    SocketConnection(IpcListener* aListener);
    SocketConnection(IpcListener* aListener, int aSocketFd);
    virtual ~SocketConnection();

    // IpcConnectionInterface methods
    virtual int connect(int aPort);
    virtual void disconnect();
    virtual int send(ipcMessage_t* aMsg);

    int receive();
    int getSocket() const;

private:
    static void* messageLoop(void* aThis);
    void init();
    void close();

    pthread_t mThreadId;
    java::util::ScopedMutex mSendMutex;

    Socket mSocket;
    IpcListener* mListener;
    int mPort;
};

} // namespace comms
} // namespace java

#endif // SOCKETCONNECTION_H
