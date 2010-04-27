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
* Description: IpcClient is the client side implementation
*
*/

#ifndef IPCCLIENT_H
#define IPCCLIENT_H
#include "scopedlocks.h"

#include "transport.h"
#include "rcomms.h"

namespace java
{
namespace comms
{
class CReceiver;

NONSHARABLE_CLASS(IpcClient) : public CActive, public IpcConnectionInterface
{
public:
    IpcClient(IpcListener* aListener);
    ~IpcClient();

    // IpcConnectionInterface methods
    virtual int connect(int aAddr);
    virtual void disconnect();
    virtual int send(ipcMessage_t* aMsg);

    virtual void error(TInt aError);

private:
    virtual void RunL();
    virtual void DoCancel();
    virtual TInt RunError(TInt aError);

    static void messageLoop(TAny* aArgs);
    void doMainL();

    int         mAddress;
    RThread     mThread;
    bool        mRunning;
    RComms      mComms;
    CReceiver*  mReceiver;

    java::util::ScopedMutex mMutex;
    IpcListener* mListener;
};

} // namespace comms
} // namespace java

#endif // IPCCLIENT_H
