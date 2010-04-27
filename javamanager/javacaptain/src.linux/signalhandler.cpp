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
* Description:  SignalHandler
*
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <signal.h>

#include "logger.h"
#include "comms.h"
#include "commsclientendpoint.h"

#include "signalhandler.h"
#include "signalhandlermessages.h"

namespace java
{
namespace captain
{


SignalHandler::SignalHandler():mThreadId(0)
{
}

SignalHandler::~SignalHandler()
{
}

void SignalHandler::start()
{
    if (pthread_create(&mThreadId, 0, SignalHandler::signalHandlerThread, NULL))
    {
        ELOG(EJavaCaptain, "pthread_create failed for signal handler thread");
    }
}

void SignalHandler::stop()
{
    kill(getpid(), SIGINT);
    pthread_join(mThreadId, NULL);
}

void* SignalHandler::signalHandlerThread(void* /*aParams*/)
{

    CommsClientEndpoint comms;
    if (comms.connect(IPC_ADDRESS_JAVA_CAPTAIN_C))
    {
        ELOG(EJavaCaptain, "Connect to main message loop failed!");
        return NULL;
    }

    sigset_t    ss;
    sigemptyset(&ss);
    sigaddset(&ss, SIGCHLD);
    sigaddset(&ss, SIGALRM);
    sigaddset(&ss, SIGINT);

    int signal = 0;
    bool keepWaiting = true;
    while (keepWaiting && !sigwait(&ss, &signal))
    {
        CommsMessage msg;
        switch (signal)
        {
        case SIGCHLD:
            LOG(EJavaCaptain, EInfo, "SIGCHLD received");
            {
                int status = 0;
                pid_t pidi = 0;
                while ((pidi=waitpid(-1, &status, WNOHANG))  > 0)
                {
                    LOG2(EJavaCaptain, EInfo, "SIGCHLD received - waitpid returns pid=%d status=%d", (int)pidi, status);

                    LOG1(EJavaCaptain, EInfo, "WIFEXITED(status) returns %d", WIFEXITED(status));
                    if (WIFEXITED(status))
                    {
                        LOG1(EJavaCaptain, EInfo, "WEXITSTATUS(status) returns %d", WEXITSTATUS(status));
                    }

                    LOG1(EJavaCaptain, EInfo, "WIFSIGNALED(status) returns %d", WIFSIGNALED(status));
                    if (WIFSIGNALED(status))
                    {
                        LOG1(EJavaCaptain, EInfo, "WTERMSIG(status) returns %d", WTERMSIG(status));
                    }

                    LOG1(EJavaCaptain, EInfo, "WIFSTOPPED(status) returns %d", WIFSTOPPED(status));
                    if (WIFSTOPPED(status))
                    {
                        LOG1(EJavaCaptain, EInfo, "WSTOPSIG(status) returns %d", WSTOPSIG(status));
                    }

                    LOG2(EJavaCaptain, EInfo, "initiating pmcTerminated event (pid=%d, status=%d)", (int)pidi, status);
                    setSignalChildMessage(msg, pidi, status);
                    comms.send(msg);
                    LOG2(EJavaCaptain, EInfo, "initiated pmcTerminated event (pid=%d, status=%d)", (int)pidi, status);
                }
            }
            break;

        case SIGALRM:
            LOG(EJavaCaptain, EInfo, "SIGALRM received");
            setSignalAlarmMessage(msg);
            comms.send(msg);
            break;

        case SIGINT:
            LOG(EJavaCaptain, EInfo, "SIGINT received");
            setSignalInterruptMessage(msg);
            comms.send(msg);
            keepWaiting = false;
            break;

        default:
            ELOG1(EJavaCaptain, "Unknown signal %d sigwaited!", signal);
        }
    }
    return NULL;
}

} // namespace captain
} // namespace java

