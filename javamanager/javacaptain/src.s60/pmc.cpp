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


#include <sstream>
#include <memory>

#include "javasymbianoslayer.h"
#include "logger.h"

#include "pmc.h"
#include "processmanagementeventsinterface.h"
#include "coreinterface.h"

#include "midpruntimearguments.h"

using namespace java::comms;

namespace java
{
namespace captain
{

Pmc::Pmc():mCore(0), mProcessManagementEventsDispatcher(0)
{
    JELOG4(EJavaCaptain, EInfo);
}

Pmc::~Pmc()
{
    JELOG4(EJavaCaptain, EInfo);
    mCore = 0;
    mProcessManagementEventsDispatcher = 0;
}

bool Pmc::start(CoreInterface* aCore,
                ProcessManagementEventsInterface* aProcessManagementEventsDispatcher)
{
    JELOG4(EJavaCaptain, EInfo);
    mCore = aCore;
    mProcessManagementEventsDispatcher = aProcessManagementEventsDispatcher;

    return true;
}

bool Pmc::stop()
{
    JELOG4(EJavaCaptain, EInfo);

    ScopedLock lock(mProcessesMutex);
    for (processes_t::iterator iter = mProcesses.begin() ; iter != mProcesses.end() ; /*emty*/)
    {
        delete iter->second;
        mProcesses.erase(iter++);
    }
    return true;
}

int Pmc::launch(const cmdLine_t& cmdLine, const int& /*options*/)
{
    JELOG4(EJavaCaptain, EInfo);
    int cmdLineSize = cmdLine.size();
    if (cmdLineSize > 0)
    {
        std::auto_ptr<HBufC> runtimeProcess(stringToDes(cmdLine[0].c_str()));

        std::ostringstream params;
        for (unsigned int i = 1 ; i < cmdLineSize ; i++)
        {
            params << cmdLine[i] << " ";
        }
        std::auto_ptr<HBufC> runtimeArgs(stringToDes(params.str().c_str()));
        ILOG2(EJavaCaptain, "launch command line: %s : %s",
              cmdLine[0].c_str(), params.str().c_str());

        ScopedLock lock(mProcessesMutex);

        processes_t::iterator iter = mProcesses.begin();
        while (iter != mProcesses.end())
        {
            if (iter->second->isExited())
            {
                delete iter->second;
                mProcesses.erase(iter);
                iter = mProcesses.begin();
            }
            else
            {
                ++iter;
            }
        }

        std::auto_ptr<RProcess> rp(new RProcess);
        bool prewarm = false;
        if (std::find(cmdLine.begin(), cmdLine.end(), java::runtime::PREWARM_ARGUMENT) != cmdLine.end())
        {
            prewarm = true;
        }
        JavaOsLayer::startUpTrace("CAPTAIN: Creating process", -1, -1);
        if (KErrNone == rp->Create(TPtr(runtimeProcess->Des()), TPtr(runtimeArgs->Des())))
        {
            int pid = rp->Id();
            mProcesses.insert(std::make_pair(pid, new ProcessActive(rp.release(), this, prewarm)));
            return pid;
        }
    }

    return 0;
}

int Pmc::terminate(const int& pid)
{
    JELOG4(EJavaCaptain, EInfo);

    ScopedLock lock(mProcessesMutex);
    processes_t::iterator iter = mProcesses.find(pid);
    if (iter != mProcesses.end())
    {
        iter->second->Terminate(KErrNone);
    }

    return 0;
}

int Pmc::kill(const int& pid)
{
    JELOG4(EJavaCaptain, EInfo);

    ScopedLock lock(mProcessesMutex);
    processes_t::iterator iter = mProcesses.find(pid);
    if (iter != mProcesses.end())
    {
        iter->second->Kill(KErrNone);
    }

    return 0;
}

void Pmc::handleTerminated(const int& pid, const int& exitCode)
{
    JELOG4(EJavaCaptain, EInfo);
    if (mProcessManagementEventsDispatcher)
        mProcessManagementEventsDispatcher->pmcTerminated(pid, exitCode);
}

} // namespace captain
} // namespace java

