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


#include <memory>
#include <sstream>
#include <stdlib.h>

#include "logger.h"
#include "javasymbianoslayer.h"

#include "ipcserver.h"
#include "commsserver.h"
#include "commssession.h"
#include "common.h"

namespace java
{
namespace comms
{
using java::util::ScopedLock;

IpcServer::IpcServer(IpcListener* aListener) : CActive(EPriorityStandard),
        mRunning(false), mAddress(0), mListener(aListener)
{
}

IpcServer::~IpcServer()
{

}

int IpcServer::start(int aAddr)
{
    ScopedLock lock(mMutex);

    if (mRunning) return 0;

    mAddress = aAddr;

    // create unique thread name
    std::stringstream name;
    name << "JavaCommsServer-" << mAddress << "-" << this << "-" << rand();
    std::auto_ptr<HBufC> threadName(stringToDes(name.str().c_str()));

    TInt rc = mThread.Create(
                  threadName->Des(),
                  reinterpret_cast<TThreadFunction>(messageLoop),
                  KCommsStackSize,
                  NULL,
                  this);

    if (rc == KErrNone)
    {
        // block until thread has been started ok
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
        ELOG3(EJavaComms, "%s failed on java-comms-%d, err = %d", __PRETTY_FUNCTION__, aAddr, rc);
    }
    return rc;
}

void IpcServer::stop()
{
//    JELOG2(EJavaComms);
    ScopedLock lock(mMutex);

    if (!mRunning) return;

    TRequestStatus status;
    mThread.Logon(status);

    TRequestStatus* istatus = &iStatus;
    mThread.RequestComplete(istatus, KErrNone);

    User::WaitForRequest(status);
    mThread.Close();

    LOG1(EJavaComms, EInfo, "IpcServer stopped on java-comms-%d", mAddress);
}

int IpcServer::send(ipcMessage_t* aMsg)
{
    ScopedLock lock(mMutex);

    int rc = KErrNotFound;
    if (mRunning)
    {
        mServer->iSessionIter.SetToFirst();
        CCommsSession* s = 0;
        while ((s = static_cast<CCommsSession*>(mServer->iSessionIter++)) !=0)
        {
            if (aMsg->ipcHeader.receiver == s->getId())
            {
                rc = s->send(aMsg);
            }
        }
    }

    if (rc)
    {
        ELOG2(EJavaComms, "%s failed, err = %d", __PRETTY_FUNCTION__, rc);
    }
    return rc;
}

void IpcServer::RunL()
{
    CActiveScheduler::Stop();
}

void IpcServer::DoCancel()
{
    ELOG1(EJavaComms, "%s", __PRETTY_FUNCTION__);
}

TInt IpcServer::RunError(TInt aError)
{
    ELOG2(EJavaComms, "%s, err = %d", __PRETTY_FUNCTION__, aError);
    return KErrNone;
}

void IpcServer::messageLoop(TAny* aArgs)
{
    std::auto_ptr<CTrapCleanup> cleanup(CTrapCleanup::New());
    TInt r = KErrNoMemory;
    if (cleanup.get())
    {
        IpcServer* me = reinterpret_cast<IpcServer*>(aArgs);
        TRAP(r,me->doMainL());
        me->mListener->onExit();
        me->mRunning = false;
    }
    // stop blocks until this notification
    RThread::Rendezvous(r);
}

void IpcServer::doMainL()
{
    CActiveScheduler* s = new(ELeave) CActiveScheduler;
    CleanupStack::PushL(s);
    CActiveScheduler::Install(s);

    mServer = new(ELeave)CCommsServer(*mListener);
    CleanupStack::PushL(mServer);

    std::stringstream address;
    address << "java-comms-" << mAddress;
    std::auto_ptr<HBufC> serverName(stringToDes(address.str().c_str()));
    mServer->StartL(serverName->Des());
    LOG1(EJavaComms, EInfo, "IpcServer started on %s", address.str().c_str());

    CActiveScheduler::Add(this);
    iStatus = KRequestPending;
    SetActive();

    // start blocks until this notification
    RThread::Rendezvous(KErrNone);
    mListener->onStart();

    CActiveScheduler::Start();
    CleanupStack::PopAndDestroy(mServer);
    CleanupStack::PopAndDestroy(s);
}


} // namespace comms
} // namespace java
