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
* Description: CCommsSession handles communication with client side
*
*/

#ifndef COMMSSESSION_H
#define COMMSSESSION_H

#include <queue>

#include "scopedlocks.h"

#include "transport.h"

namespace java
{
namespace comms
{
class IpcListener;

NONSHARABLE_CLASS(CCommsSession): public CSession2
{
public:
    CCommsSession(IpcListener& aListener, int aPermissions);
    virtual ~CCommsSession();

    virtual void ServiceL(const RMessage2& aMessage);

    int send(ipcMessage_t* aMsg);
    int getId() const;
private:
    void handleSendL(const RMessage2& aMessage);
    void handleReceiveL(const RMessage2& aMessage);
    void handleCancelReceive(const RMessage2& aMessage);
    bool isReceivePending() const;
    TInt doSend(const RMessagePtr2& aMessage, HBufC8& aData);

    RMessagePtr2            mReceiveMessage;
    IpcListener&            mListener;
    std::queue<HBufC8*>     mMessageQueue;
    java::util::ScopedMutex mMutex;
    const int               mClientPermissions;
};

} // namespace comms
} // namespace java

#endif // COMMSSESSION_H
