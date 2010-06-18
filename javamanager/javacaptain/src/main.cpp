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
* Description:  JavaCaptain Main
*
*/

#ifndef __SYMBIAN32__
#include <signal.h>
#include <pthread.h>
#include "signalhandler.h"
#endif /* __SYMBIAN32__ */

#include <cstdio>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "logger.h"
#include "monitor.h"

#include "core.h"

using namespace java::captain;
using namespace java::util;

Monitor* monitor = NULL;

#ifdef RD_JAVA_S60_RELEASE_9_2_ONWARDS
#include <dirent.h>
void makeHomeDir()
{
    // open(filename) fails if c:\private\<uid> does not exist in 9.2
    // This used to work in 5.0
    char* homeDir = "c:\\private\\200211DC";
    DIR* dir = opendir(homeDir);
    if (dir != NULL)
    {
        closedir(dir);
    }
    else
    {
        int rc = mkdir(homeDir, S_IRWXU);
        if (rc < 0)
        {
            WLOG2(EJavaCaptain, "creating %s failed, rc = %d", homeDir, rc);
        }
    }
}
#endif

// In S60 working directory should be initalized to C:\private\<UID> by OpenC
// But for some reason this does not seem to work if captain binary is in rom.
#ifdef __SYMBIAN32__
const char* const IAD_BOOT_FLAG   = "c:\\private\\200211DC\\iadboot.dat";
#else
const char* const IAD_BOOT_FLAG   = "iadboot.dat";
#endif /* __SYMBIAN32__ */


int main(int ac, char** av)
{
    JELOG2(EJavaCaptain);

#ifdef RD_JAVA_S60_RELEASE_9_2_ONWARDS
    makeHomeDir();
#endif

    // check if started by IAD installation
    if (ac == 2 && strcmp(av[1], "iad") == 0)
    {
        LOG(EJavaCaptain, EInfo, "creating iad boot event flag");
        int fd = open(IAD_BOOT_FLAG, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd < 0)
        {
            ELOG1(EJavaCaptain, "create boot event flag failed: %s", strerror(errno));
        }
        else
        {
            close(fd);
        }
    }

#ifndef __SYMBIAN32__
    sigset_t old, block;
    sigemptyset(&old);
    sigfillset(&block);

    if (!pthread_sigmask(SIG_BLOCK, &block, &old))
    {
        SignalHandler sh;
        std::puts("JavaCaptain Press Ctrl-C to exit");
#endif /* __SYMBIAN32__ */

        monitor = Monitor::createMonitor();
        Core* core = new Core(monitor);
        core->start();

#ifndef __SYMBIAN32__
        sh.start();
#endif /* __SYMBIAN32__ */

        monitor->wait();
        core->stop();

        delete core;
        delete monitor;

#ifndef __SYMBIAN32__
        sh.stop();
        pthread_sigmask(SIG_SETMASK, &old, NULL);
    }
#endif /* __SYMBIAN32__ */

    return 0;
}

