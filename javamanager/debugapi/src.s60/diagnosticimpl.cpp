/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: DiagnosticImpl provides implementation for JavaDiagnostic
*
*/


#include "diagnosticimpl.h"
#include "javaredirectserver.h"
using namespace java::debug;

_LIT(KJavaRedirectThread,"JavaRedirectServerThread");
const int REDIRECT_THREAD_STACK_SIZE = 0x5000;

DiagnosticImpl::DiagnosticImpl() : CActive(EPriorityStandard), mListener(0)
{
}

DiagnosticImpl::~DiagnosticImpl()
{
    stop();
}

TInt DiagnosticImpl::setDiagnosticListener(DiagnosticListener& aListener)
{
    TInt rc = KErrAlreadyExists;
    if (!mListener)
    {
        mListener = &aListener;
        rc = start();
        if (rc != KErrNone)
        {
            mListener = 0;
        }
    }
    return rc;
}

void DiagnosticImpl::removeDiagnosticListener()
{
    stop();
    mListener = 0;
}

TInt DiagnosticImpl::start()
{
    TInt rc = mThread.Create(
                  KJavaRedirectThread,
                  reinterpret_cast<TThreadFunction>(messageLoop),
                  REDIRECT_THREAD_STACK_SIZE,
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
    }
    return rc;
}

void DiagnosticImpl::stop()
{
    if (mListener)
    {
        TRequestStatus status;
        mThread.Logon(status);

        TRequestStatus* istatus = &iStatus;
        mThread.RequestComplete(istatus, KErrNone);

        User::WaitForRequest(status);
        mThread.Close();
    }
}

void DiagnosticImpl::messageLoop(TAny* aArgs)
{
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TInt r = KErrNoMemory;
    if (cleanup)
    {
        DiagnosticImpl* me = reinterpret_cast<DiagnosticImpl*>(aArgs);
        TRAP(r,me->runServerL());
    }

    RThread::Rendezvous(r);
    delete cleanup;
}

void DiagnosticImpl::runServerL()
{
    CActiveScheduler* s = new(ELeave) CActiveScheduler;
    CleanupStack::PushL(s);
    CActiveScheduler::Install(s);

    java::util::CRedirectServer* server = new(ELeave) java::util::CRedirectServer(*this);
    CleanupStack::PushL(server);

    server->StartL();

    CActiveScheduler::Add(this);
    iStatus = KRequestPending;
    SetActive();

    RThread::Rendezvous(KErrNone);
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy(server);
    CleanupStack::PopAndDestroy(s);
}

void DiagnosticImpl::RunL()
{
    CActiveScheduler::Stop();
}

void DiagnosticImpl::DoCancel()
{
}

TInt DiagnosticImpl::RunError(TInt /*aError*/)
{
    return KErrNone;
}

void DiagnosticImpl::systemOut(const TDesC8& aData)
{
    mListener->systemOut(aData);
}

void DiagnosticImpl::systemErr(const TDesC8& aData)
{
    mListener->systemErr(aData);
}

void DiagnosticImpl::log(const TDesC8& aData)
{
    mListener->log(aData);
}
