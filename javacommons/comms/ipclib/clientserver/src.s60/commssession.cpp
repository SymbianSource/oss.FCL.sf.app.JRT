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

#include <memory>

#include "logger.h"

#include "common.h"
#include "commssession.h"
#include "ipcserver.h"

namespace java
{
namespace comms
{
using java::util::ScopedLock;

CCommsSession::CCommsSession(IpcListener& aListener, int aPermissions)
        : mListener(aListener), mClientPermissions(aPermissions)
{
}

CCommsSession::~CCommsSession()
{
    while (!mMessageQueue.empty())
    {
        std::auto_ptr<HBufC8> tmp(mMessageQueue.front());
        mMessageQueue.pop();
    }
}

void CCommsSession::ServiceL(const RMessage2& aMessage)
{
    TInt function = aMessage.Function();
    switch (function)
    {
    case ESend:
        handleSendL(aMessage);
        break;

    case EReceive:
        handleReceiveL(aMessage);
        break;

    case ECancelReceive:
        handleCancelReceive(aMessage);
        break;

    default:
        aMessage.Complete(KErrNotSupported);
        break;
    }
}

int CCommsSession::send(ipcMessage_t* aMsg)
{
    ScopedLock lock(mMutex);

    // messages from server are considered to be always trusted
    aMsg->ipcHeader.permissions = -1;
    HBufC8* msg = messageToDes(*aMsg);
    mMessageQueue.push(msg);

    if (isReceivePending())
    {
        HBufC8* msg = mMessageQueue.front();

        TInt rc = doSend(mReceiveMessage, *msg);
        if (rc == KErrNone)
        {
            mMessageQueue.pop();
            delete msg;
        }
        else
        {
            if (rc != KErrOverflow)
            {
                WLOG2(EJavaComms, "%s failed, err = %d", __PRETTY_FUNCTION__, rc);
            }
        }
    }
    return 0;
}

void CCommsSession::handleSendL(const RMessage2& aMessage)
{
    TInt len = aMessage.GetDesLengthL(0);
    int rc = KErrNone;
    if (len < sizeof(ipcHeader_t))
    {
        WLOG2(EJavaComms, "%s invalid message size, len = %d", __PRETTY_FUNCTION__, len);
        rc = KErrArgument;
    }
    else
    {
        RBuf8 buffer;

        buffer.CleanupClosePushL();
        buffer.CreateL(len);

        aMessage.ReadL(0,buffer);

        char* buf = desToMessage(buffer);
        ipcMessage_t* msg = reinterpret_cast<ipcMessage_t*>(buf);
        if (msg->ipcHeader.length == len)
        {
            msg->ipcHeader.sender = getId();
            msg->ipcHeader.permissions = mClientPermissions;
            mListener.processMessage(msg);
        }
        else
        {
            WLOG3(EJavaComms, "%s message size does not match to header, len = %d header = %d",
                  __PRETTY_FUNCTION__, len, msg->ipcHeader.length);
            rc = KErrArgument;
        }

        delete[] buf;
        CleanupStack::PopAndDestroy();
    }
    aMessage.Complete(rc);
}

void CCommsSession::handleReceiveL(const RMessage2& aMessage)
{
    ScopedLock lock(mMutex);

    if (mMessageQueue.empty())
    {
        mReceiveMessage = aMessage;
    }
    else
    {
        HBufC8* msg = mMessageQueue.front();
        TInt rc = doSend(aMessage, *msg);
        if (rc == KErrNone)
        {
            mMessageQueue.pop();
            delete msg;
        }
        else
        {
            if (rc != KErrOverflow)
            {
                WLOG2(EJavaComms, "%s failed, err = %d", __PRETTY_FUNCTION__, rc);
            }
        }
    }
}

void CCommsSession::handleCancelReceive(const RMessage2& aMessage)
{
    ScopedLock lock(mMutex);

    if (isReceivePending())
    {
        mReceiveMessage.Complete(KErrCancel);
    }
    aMessage.Complete(KErrNone);
}

bool CCommsSession::isReceivePending() const
{
    return !mReceiveMessage.IsNull();
}

int CCommsSession::getId() const
{
    // to be used as unique id to identify session - not as pointer
    return (int)this;
}

TInt CCommsSession::doSend(const RMessagePtr2& aMessage, HBufC8& aData)
{
    TInt allocatedLength = aMessage.GetDesMaxLength(0);
    TInt neededLength = aData.Length();

    TPckgBuf<TInt> lengthPckg(neededLength);
    TInt rc = aMessage.Write(1, lengthPckg);

    if (rc == KErrNone && allocatedLength >= neededLength)
    {
        rc = aMessage.Write(0, aData.Des());
    }
    else
    {
        rc = KErrOverflow;
        LOG3(EJavaComms, EInfo, "%s: allocated length for message is too small (required size=%d, allocated size=%d)",
              __PRETTY_FUNCTION__, neededLength, allocatedLength);
    }

    aMessage.Complete(rc);
    return rc;
}

} // namespace comms
} // namespace java
