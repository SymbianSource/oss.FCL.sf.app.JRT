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
#include <memory>
#include <sstream>
#include <stdlib.h>

#include "logger.h"
#include "javasymbianoslayer.h"

#include "ipcclient.h"
#include "rcomms.h"
#include "creceiver.h"
#include "common.h"

namespace java
{
namespace comms
{
using java::util::ScopedLock;

IpcClient::IpcClient(IpcListener* aListener) : CActive(EPriorityStandard),
        mAddress(0), mRunning(false), mReceiver(0), mListener(aListener)
{
}

IpcClient::~IpcClient()
{
}

int IpcClient::connect(int aAddr)
{
    ScopedLock lock(mMutex);

    if (mRunning) return 0;

    mAddress = aAddr;
    // create unique thread name
    std::stringstream name;
    name << "JavaCommsClient-" << mAddress << "-" << this << "-" << rand();
    std::auto_ptr<HBufC> threadName(stringToDes(name.str().c_str()));

    TInt rc = mThread.Create(
                  threadName->Des(),
                  reinterpret_cast<TThreadFunction>(messageLoop),
                  KCommsStackSize,
                  NULL,
                  this);

    if (rc == KErrNone)
    {
        // make sure that thread has been started ok before we continue.
        TRequestStatus rendezvousStatus;
        mThread.Rendezvous(rendezvousStatus);
        mThread.Resume();
        User::WaitForRequest(rendezvousStatus);
        rc = rendezvousStatus.Int();
        if (rc == KErrNone)
        {
            mRunning = true;
        }
    }

    if (rc != KErrNone)
    {
        mThread.Close();
        ELOG3(EJavaComms, "%s failed to java-comms-%d, err = %d", __PRETTY_FUNCTION__, aAddr, rc);
    }

    return rc;
}

void IpcClient::disconnect()
{
    ScopedLock lock(mMutex);

    if (!mRunning) return;

    TRequestStatus status;
    mThread.Logon(status);

    TRequestStatus* istatus = &iStatus;
    mThread.RequestComplete(istatus, KErrNone);

    User::WaitForRequest(status);
    mThread.Close();
    LOG1(EJavaComms, EInfo, "IpcClient disconnected from java-comms-%d", mAddress);
}

void IpcClient::error(TInt aError)
{
    ELOG2(EJavaComms, "%s, err = %d", __PRETTY_FUNCTION__, aError);
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, aError);
}


int IpcClient::send(ipcMessage_t* aMsg)
{
//    ScopedLock lock(mMutex);

    int rc = KErrNotReady;
    if (mRunning)
    {
        std::auto_ptr<HBufC8> msg(messageToDes(*aMsg));
        rc = mComms.Send(msg->Des());
    }

    if (rc)
    {
        ELOG2(EJavaComms, "%s failed, err = %d", __PRETTY_FUNCTION__, rc);
    }
    return rc;
}

void IpcClient::RunL()
{
//    JELOG2(EJavaComms);
    mRunning = false; // don't allow sends while exiting

    mReceiver->Cancel();
    mComms.Disconnect();
    CActiveScheduler::Stop();
}

void IpcClient::DoCancel()
{
    ELOG1(EJavaComms, "%s", __PRETTY_FUNCTION__);
}

TInt IpcClient::RunError(TInt aError)
{
    ELOG2(EJavaComms, "%s, err = %d", __PRETTY_FUNCTION__, aError);
    return KErrNone;
}

void IpcClient::messageLoop(TAny* aArgs)
{
    std::auto_ptr<CTrapCleanup> cleanup(CTrapCleanup::New());
    TInt r = KErrNoMemory;
    if (cleanup.get())
    {
        IpcClient* me = reinterpret_cast<IpcClient*>(aArgs);
        TRAP(r,me->doMainL());
        me->mListener->onExit();
        me->mRunning = false;
    }
    // disconnect blocks until this notification
    RThread::Rendezvous(r);
}

void IpcClient::doMainL()
{
    CActiveScheduler* s = new(ELeave) CActiveScheduler;
    CleanupStack::PushL(s);
    CActiveScheduler::Install(s);

    std::stringstream address;
    address << "java-comms-" << mAddress;

    std::auto_ptr<HBufC> serverName(stringToDes(address.str().c_str()));

    User::LeaveIfError(mComms.Connect(serverName->Des()));
    LOG1(EJavaComms, EInfo, "IpcClient connected to %s", address.str().c_str());

    mReceiver = CReceiver::NewLC(*this, *mListener, mComms);
    mReceiver->Receive();

    CActiveScheduler::Add(this);
    iStatus = KRequestPending;
    SetActive();

    // connect blocks until this notification
    RThread::Rendezvous(KErrNone);
    mListener->onStart();

    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy(mReceiver);
    CleanupStack::PopAndDestroy(s);
}

} // namespace comms
} // namespace java
