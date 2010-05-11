/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#include "logger.h"
#include "monitor.h"

#ifndef __SYMBIAN32__
#include <sys/time.h>
#include <errno.h>
#endif
using namespace java::util;

Monitor::Monitor() : mWaiting(false), mNotifyBeforeWait(false)
{
    JELOG2(EUtils);
}

Monitor::~Monitor()
{
    JELOG2(EUtils);
    pthread_mutex_destroy(&mMutex);
    pthread_cond_destroy(&mCondVar);
}

OS_EXPORT Monitor* Monitor::createMonitor()
{
    JELOG2(EUtils);
    int result = -1;
    Monitor* monitor = new Monitor();
    if (monitor)
    {
        result = pthread_mutex_init(&monitor->mMutex, 0);
        if (result == 0)
        {
            result = pthread_cond_init(&monitor->mCondVar, 0);
            if (result != 0)
            {
                ELOG(EUtils, "Monitor::createMonitor() "
                     "pthread_cond_init() failed");
            }
        }
        else
        {
            ELOG(EUtils, "Monitor::createMonitor() "
                 "pthread_mutex_init() failed");
        }
    }
    else
    {
        ELOG(EUtils, "Monitor::createMonitor() "
             "new Monitor() failed");
    }
    if (result != 0 && monitor)
    {
        delete monitor;
        monitor = 0;
    }
    return monitor;
}

OS_EXPORT int Monitor::wait(int timeOut)
{
    JELOG4(EUtils, EEntryLog & EInfoHeavyLoad);
    pthread_mutex_lock(&mMutex);
    if (!mNotifyBeforeWait)
    {
        mWaiting = true;
        if (timeOut == 0)
        {
            while (mWaiting)
            {
                pthread_cond_wait(&mCondVar, &mMutex);
            }
        }
        else
        {
            struct timeval currentTimeVal;
            struct timespec timeOutTime;
            gettimeofday(&currentTimeVal, 0);
            timeOutTime.tv_sec = currentTimeVal.tv_sec + timeOut / 1000;
            timeOutTime.tv_nsec =
                currentTimeVal.tv_usec * 1000 + (timeOut % 1000) * 1000 * 1000;

            int err = pthread_cond_timedwait(&mCondVar, &mMutex, &timeOutTime);
            if (err != 0 && err != ETIMEDOUT)
            {
                ELOG1(EUtils, "Monitor: Timed wait failed, err = %d", err);
            }
        }
    }
    else
    {
        mNotifyBeforeWait = false;
    }
    pthread_mutex_unlock(&mMutex);
    return 0;
}

OS_EXPORT int Monitor::notify()
{
    JELOG4(EUtils, EEntryLog & EInfoHeavyLoad);
    pthread_mutex_lock(&mMutex);
    if (mWaiting)
    {
        pthread_cond_signal(&mCondVar);
    }
    else
    {
        mNotifyBeforeWait = true;
    }
    mWaiting = false;
    pthread_mutex_unlock(&mMutex);
    return 0;
}
