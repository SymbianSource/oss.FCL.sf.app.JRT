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
* Description: IpcServer is the server side implementation
*
*/

#ifndef IPCSERVER_H
#define IPCSERVER_H

#include "scopedlocks.h"

#include "transport.h"

namespace java
{
namespace comms
{

class CCommsServer;

NONSHARABLE_CLASS(IpcServer) : public CActive, public IpcServerConnectionInterface
{
public:
    IpcServer(IpcListener* aListener);
    ~IpcServer();

    // IpcIpcServerConnectionInterface methods
    virtual int start(int aAddr);
    virtual void stop();
    virtual int send(ipcMessage_t* aMsg);

private:
    virtual void RunL();
    virtual void DoCancel();
    virtual TInt RunError(TInt aError);

    static void messageLoop(TAny* aArgs);
    void doMainL();

    RThread         mThread;
    bool            mRunning;
    CCommsServer*   mServer;
    int             mAddress;

    java::util::ScopedMutex mMutex;
    IpcListener*            mListener;
};

} // namespace comms
} // namespace java

#endif // IPCSERVER_H
