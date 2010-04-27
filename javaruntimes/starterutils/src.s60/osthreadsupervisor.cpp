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
* Description:  This class is for thread death monitoring.
*
*/

#include <e32base.h>

#include "osthreadsupervisor.h"

#include "runtimeexception.h"

#include "logger.h"
#include "javacommonutils.h"

using namespace java::runtime;
using namespace java::util;


OsThreadSupervisor::OsThreadSupervisor(bool tryThreadDumping) :
        mDoExit(false), mTryThreadDump(tryThreadDumping)
{
    JELOG2(EJavaRuntime);
    startUndertakerThread();
}

OsThreadSupervisor::~OsThreadSupervisor()
{
    JELOG2(EJavaRuntime);
    // Wake the undertaker thread in order to close it.
    mDoExit = true;
    mRequestStatus = KRequestPending;
    TRequestStatus* requestStatus = &mRequestStatus;
    mSupervisorThread.RequestComplete(requestStatus, KErrNone);
    mSupervisorThread.Close();
}


void OsThreadSupervisor::startUndertakerThread()
{
    JELOG2(EJavaRuntime);
    RThread thread;

    // Create the undertaker thread.
    _LIT(KUndertakerName, "JavaMidp undertaker");
    TInt err = thread.Create(KUndertakerName,
                             OsThreadSupervisor::underTakerThreadMain,
                             0x2000, 0, this);

    if (err == KErrNone)
    {
        // Set the prioroty to high.
        thread.SetPriority(EPriorityMuchMore);
        thread.Resume();
        thread.Close();
    }
    else
    {
        thread.Close();
        std::string errorStr("Not able to create undertaker thread. Reason: ");
        errorStr.append(JavaCommonUtils::intToString(err));
        throw RuntimeException(errorStr, __FILE__, __FUNCTION__, __LINE__);
    }

}

int OsThreadSupervisor::underTakerThreadMain(void* starter)
{
    JELOG2(EJavaRuntime);
    OsThreadSupervisor* supervisor =
        reinterpret_cast<OsThreadSupervisor*>(starter);

    // Open the thread handle so that RequstComplete methods can be
    // called from another thread.
    supervisor->mSupervisorThread.Open(RThread().Id());

    RUndertaker underTaker;
    if (underTaker.Create() == KErrNone)
    {
        // Get the process id of this process.
        TProcessId processId = RProcess().Id();
        int status = KErrNone;
        LOG1(EJavaRuntime, EInfo, "Starting to monitor MIDP process %lld",
             processId.Id());
        // We will receive notification from ALL the thread deaths from all
        // processes. That is why the looping is needed.
        while (status == KErrNone)
        {
            int threadHandle;

            // Logon to monitor thread deaths.
            status = underTaker.Logon(supervisor->mRequestStatus, threadHandle);
            if (status != KErrNone)
            {
                ELOG1(EJavaRuntime, "Undertaker thread failed to do logon. "
                      " Reason: %d", status);
                break;
            }

            // Start to wait thread deaths or shutdown indication.
            User::WaitForRequest(supervisor->mRequestStatus);
            if (supervisor->mDoExit)
            {
                // We are going down, so no need to monitor thread deaths.
                // Closing the undertaker thread.
                LOG(EJavaRuntime, EInfo, "Undertaker closed succesfully");
                return 0;
            }
            RThread thread;
            thread.SetHandle(threadHandle);
            LOG2(EJavaRuntime, EInfo, "A thread %lld has died with exit type %d",
                 thread.Id().Id(), thread.ExitType());

            // We are only interested in panicing threads.
            if (thread.ExitType() == EExitPanic)
            {
                RProcess terminatedProcess;
                status = thread.Process(terminatedProcess);
                if (status == KErrNone)
                {
                    LOG2(EJavaRuntime, EInfo,
                         "Thread panic from process id: %lld. "
                         "Our id is: %lld",
                         terminatedProcess.Id().Id(), processId.Id());
                    // Check if this thread belong to our process.
                    if (terminatedProcess.Id() == processId)
                    {
                        // One of MIDP threads has paniced, so lets terminate
                        // the whole process.
                        status = KErrDied;
                    }
                    terminatedProcess.Close();
                }
                else
                {
                    ELOG1(EJavaRuntime, "Solving paniced thread's process "
                          "failed. Reason: %d", status);
                }
            }
            thread.Close();
        }
    }
    ELOG(EJavaRuntime, "Panic in one of the MIDP threads. Stopping process!");
    RProcess().Terminate(KErrAbort);
    return 0;
}

