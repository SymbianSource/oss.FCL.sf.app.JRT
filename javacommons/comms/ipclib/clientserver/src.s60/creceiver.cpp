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
* Description: CReceiver enables asynchronous communication with server side
*
*/


#include "logger.h"

#include "creceiver.h"
#include "rcomms.h"
#include "ipcclient.h"
#include "common.h"

namespace java
{
namespace comms
{

const TInt DEFAULT_BUFFER_SIZE  = 2048;

CReceiver::CReceiver(IpcClient& aParent, IpcListener& aListener, RComms& aSession)
        : CActive(EPriorityStandard), mParent(aParent), mListener(aListener), mSession(aSession)
{
    CActiveScheduler::Add(this);
    mBuffer.Create(DEFAULT_BUFFER_SIZE);
}

CReceiver::~CReceiver()
{
    mBuffer.Close();
}

void CReceiver::Receive()
{
    mSession.Receive(mBuffer, mRequiredLength, iStatus);
    SetActive();
}

void CReceiver::RunL()
{
    switch (iStatus.Int())
    {
    case KErrNone:
    {
        char* buf = desToMessage(mBuffer);
        mListener.processMessage(reinterpret_cast<ipcMessage_t*>(buf));
        delete[] buf;
        Receive();
    }
    break;

    case KErrOverflow:
    {
        // buffer was not big enough for the message so reallocate buffer with correct size
        mBuffer.Close();
        mBuffer.ReAllocL(mRequiredLength());
        WLOG2(EJavaComms, "%s: increasing buffer size to %d", __PRETTY_FUNCTION__, mRequiredLength());
        Receive();
    }
    break;

    default:
        ELOG2(EJavaComms, "%s failed, err = %d", __PRETTY_FUNCTION__, iStatus.Int());
        mParent.error(iStatus.Int());
        break;
    }
}

void CReceiver::DoCancel()
{
    mSession.CancelReceive();
}

TInt CReceiver::RunError(TInt aError)
{
    ELOG2(EJavaComms, "%s failed, err = %d", __PRETTY_FUNCTION__, aError);
    return KErrNone;
}


} // namespace comms
} // namespace java
