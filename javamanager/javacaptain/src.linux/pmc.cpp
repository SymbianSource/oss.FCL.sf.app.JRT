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


#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "logger.h"

#include "pmc.h"
#include "processmanagementeventsinterface.h"
#include "coreinterface.h"

namespace java
{
namespace captain
{

Pmc::Pmc()
{
    JELOG4(EJavaCaptain, EInfo);
}

Pmc::~Pmc()
{
    JELOG4(EJavaCaptain, EInfo);
}

bool Pmc::start(CoreInterface* /*aCore*/,
                ProcessManagementEventsInterface* /*aProcessManagementEventsDispatcher*/)
{
    JELOG4(EJavaCaptain, EInfo);

    return true;
}

bool Pmc::stop()
{
    JELOG4(EJavaCaptain, EInfo);

    return true;
}

int Pmc::launch(const cmdLine_t& cmdLine, const int& /*options*/)
{
    JELOG4(EJavaCaptain, EInfo);

    int status = 0, pid = 0;

    if (!(pid = fork()))
    {
        sigset_t unblock;
        sigemptyset(&unblock);
        sigaddset(&unblock, SIGINT);
        int rc = pthread_sigmask(SIG_UNBLOCK, &unblock, NULL);
        if (rc)
        {
            ELOG1(EJavaCaptain, "pthread_sigmask(UNBLOCK) for a child failed with %d", rc);
        }

        char** av = new char*[cmdLine.size() + 1];
        unsigned int index;
        for (index = 0; index < cmdLine.size() ; index++)
        {
            av[index] = const_cast<char*>(cmdLine[index].c_str());
            LOG1(EJavaCaptain, EInfo, "Launch command line is: %s", av[index]);
        }
        av[index] = NULL;

        status = execvp(av[0], av);

        if (status == -1)
        {
            LOG1(EJavaCaptain, EError, "execvp() returned %d", status);
            LOG2(EJavaCaptain, EError, "can't start %s, error message: %s", av[0], strerror(errno));
        }
        else
        {
            LOG1(EJavaCaptain, EInfo, "execvp() returned %d", status);
        }
        delete [] av;
        exit(status);
    }

    return pid;
}

int Pmc::terminate(const int& pid)
{
    JELOG4(EJavaCaptain, EInfo);
    LOG1(EJavaCaptain, EInfo, "kill(pid=%d, SIGINT)", pid);
    ::kill(pid, SIGINT);
    return 0;
}

int Pmc::kill(const int& pid)
{
    JELOG4(EJavaCaptain, EInfo);
    LOG1(EJavaCaptain, EInfo, "kill(pid=%d, SIGKILL)", pid);
    ::kill(pid, SIGKILL);
    return 0;
}

} // namespace captain
} // namespace java

