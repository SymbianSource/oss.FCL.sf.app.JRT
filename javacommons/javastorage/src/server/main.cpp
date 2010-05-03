/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  StorageServer Main
*
*/

#include <cstdio>
#include <signal.h>
#include <memory.h>
#include <string>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include "monitor.h"
#include "storageserver.h"

using namespace java::storage;

StorageServer*          server = 0;
java::util::Monitor*    monitor = 0;

void sigint_handler(int /*sig*/)
{
    monitor->notify();
}

int main(int /*ac*/, char** /*av*/)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);

    std::puts("StorageServer started.");

    monitor = java::util::Monitor::createMonitor();
    server = new StorageServer(monitor);
    server->start();
    monitor->wait();

    if (server)
    {
        delete server;
        server = NULL;
    }
    if (monitor)
    {
        delete monitor;
        monitor = NULL;
    }

    std::puts("StorageServer stopped.");
    return 0;
}
