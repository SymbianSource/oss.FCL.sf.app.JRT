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
* Description:  ScopedLocks
*
*/

#ifndef SCOPEDLOCKS_H
#define SCOPEDLOCKS_H

#include <pthread.h>

#include "logger.h"

namespace java
{
namespace util
{

class ScopedMutex
{
public:
    ScopedMutex()
    {
        pthread_mutexattr_t mutex_attr;
        pthread_mutexattr_init(&mutex_attr);
        pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&mMutex, &mutex_attr);
        pthread_mutexattr_destroy(&mutex_attr);
    }
    ~ScopedMutex()
    {
        pthread_mutex_destroy(&mMutex);
    }
    int lock()
    {
        return pthread_mutex_lock(&mMutex);
    }
    int unlock()
    {
        return pthread_mutex_unlock(&mMutex);
    }

private:
    pthread_mutex_t mMutex;
};

class ScopedLock
{
public:
    ScopedLock(ScopedMutex& aMutex):mMutex(aMutex)
    {
        int status = mMutex.lock();
        if (0 != status)
        {
            ELOG1(EUtils, "ScopedLock().mutex.lock() failed with %d", status);
        }
    }
    ~ScopedLock()
    {
        int status = mMutex.unlock();
        if (0 != status)
        {
            ELOG1(EUtils, "ScopedLock().mutex.unlock() failed with %d", status);
        }
    }
private:
    ScopedMutex& mMutex;
};

} //end namespace util
} //end namespace java

#endif // SCOPEDLOCKS_H

