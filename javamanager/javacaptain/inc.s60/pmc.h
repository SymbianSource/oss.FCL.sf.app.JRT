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
* Description:  Pmc
*
*/

#ifndef PMC_H
#define PMC_H

#include <e32std.h>
#include <e32base.h>

#include <map>

#include "javaoslayer.h"
#include "logger.h"
#include "scopedlocks.h"

#include "pmcinterface.h"

using namespace java::util;

namespace java
{

namespace captain
{
class CoreInterface;
class ProcessManagementEventsInterface;

class ProcessEvents
{
public:
    virtual ~ProcessEvents() {}
    virtual void handleTerminated(const int& pid, const int& exitCode) = 0;
};

class Pmc : public PmcInterface,
        public ProcessEvents
{
public:
    Pmc();
    virtual ~Pmc();

    bool start(CoreInterface* aCore,
               ProcessManagementEventsInterface* aProcessManagementEventsDispatcher);
    bool stop();

    // PmcInterface
    virtual int launch(const cmdLine_t& cmdLine, const int& options);
    virtual int terminate(const int& pid);
    virtual int kill(const int& pid);

    // ProcessEvents
    virtual void handleTerminated(const int& pid, const int& exitCode);

protected:

    class ProcessActive : public CActive
    {
        ProcessActive(); // cannot be used
    public:
        ProcessActive(RProcess* aProcess, ProcessEvents* aEvents, bool aPrewarm)
                :CActive(EPriorityStandard), mRProcess(aProcess), mEvents(aEvents), mExited(false)
        {
            JELOG4(EJavaCaptain, EInfo);
            CActiveScheduler::Add(this);
            mRProcess->Logon(iStatus);
            SetActive();
            JavaOsLayer::startUpTrace("CAPTAIN: Process resume", -1, -1);
            if (aPrewarm)
            {
                // Can't use EPriorityLow, because the compiler takes wrong
                // enum from CActive.
                mRProcess->SetPriority(/*EPriorityLow*/ (TProcessPriority)150);
            }
            mRProcess->Resume();
        }
        virtual ~ProcessActive()
        {
            JELOG4(EJavaCaptain, EInfo);
            Cancel();
            if (!mExited)
            {
                mRProcess->Close();
                delete mRProcess;
            }
        }

        virtual void RunL()
        {
            JELOG4(EJavaCaptain, EInfo);
            mEvents->handleTerminated(mRProcess->Id(), mRProcess->ExitReason());
            mRProcess->Close();
            delete mRProcess;
            mExited = true;
        }
        virtual void DoCancel()
        {
            JELOG4(EJavaCaptain, EInfo);
            mRProcess->LogonCancel(iStatus);
        }

        virtual bool isExited() const
        {
            JELOG4(EJavaCaptain, EInfo);
            return mExited;
        }
        virtual void Terminate(const int& reason)
        {
            JELOG4(EJavaCaptain, EInfo);
            if (!mExited)
            {
                mRProcess->Terminate(reason);
                showErrorNote();
            }
        }
        virtual void Kill(const int& reason)
        {
            JELOG4(EJavaCaptain, EInfo);
            if (!mExited)
            {
                mRProcess->Kill(reason);
                showErrorNote();
            }
        }

    private:
        void showErrorNote()
        {
            LOG(EJavaCaptain, EInfo, "Process failed to exit gracefully");
        }

    private:
        RProcess*       mRProcess;
        ProcessEvents*  mEvents;
        bool            mExited;
    };

    CoreInterface*                          mCore;
    ProcessManagementEventsInterface*       mProcessManagementEventsDispatcher;

    typedef std::map<int, ProcessActive*>   processes_t;
    processes_t                             mProcesses;
    java::util::ScopedMutex                 mProcessesMutex;
};

} // namespace captain
} // namespace java

#endif // PMC_H
