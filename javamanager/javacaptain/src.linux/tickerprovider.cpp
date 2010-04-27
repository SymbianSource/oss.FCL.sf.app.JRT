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
* Description:  TimerServer
*
*/


#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <limits.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "logger.h"

#include "tickerprovider.h"

namespace java
{
namespace captain
{

TickerProvider::TickerProvider(TickerProviderEventsInterface* aEvents)
        :TickerProviderInterface(aEvents), mTimerId(0), mNextTickAt(0LL)
{
    JELOG2(EJavaCaptain);
    int rc = timer_create(CLOCK_REALTIME, NULL, &mTimerId);
    if (rc != 0)
    {
        ELOG2(EJavaCaptain, "tickerProvider timer_create failed(%d) errono=%d", rc, errno);
        mTimerId = 0;
    }
}

TickerProvider::~TickerProvider()
{
    JELOG2(EJavaCaptain);
    if (mTimerId)
    {
        timer_delete(mTimerId);
    }
}

void TickerProvider::nextTickAt(const long long& aJavaTime)
{
    JELOG2(EJavaCaptain);

    long long periodmS = (aJavaTime - getCurrentJavaTime());
    LOG1(EJavaCaptain, EInfo, "request %d ms from now", periodmS);

    // Already or about to expire (within next 100ms)
    if (periodmS < 100)
    {
        cancel();
        kill(getpid(), SIGALRM);
        mNextTickAt = 0LL;
    }
    else
    {
        long long periodS = periodmS/1000LL;
        //19 Jan 2038 is maximum timer value(equal with INT_MAX) which can be set to the
        //to the timer_settime. It is ok to set INT_MAX as value of timer in this situation.
        //Ultimate solution would be set first INT_MAX to the timer and after expiration of that timer
        //set rest of the timer value of the orig. timer.
        int periodnS = 0;
        if ((long long) INT_MAX < periodS)
        {
            periodS = INT_MAX;
        }
        else
        {
            periodnS = periodmS - (periodS * 1000);
            periodnS *= 1000L; // ms -> us
            periodnS *= 1000L; // us -> ns
        }

        LOG1(EJavaCaptain, EInfo, "nextTickAt (%ld seconds)", periodS);
        LOG1(EJavaCaptain, EInfo, "nextTickAt (%d nanoseconds)", periodnS);

        struct itimerspec value;
        memset(&value, 0, sizeof(value));
        int rc = timer_gettime(mTimerId, &value);

        if (!rc)
        {
            // Start timer if it wasn't already started or was started but set too late
            if ((!value.it_value.tv_sec && !value.it_value.tv_nsec) ||
                    (value.it_value.tv_sec > periodS) ||
                    (value.it_value.tv_sec == periodS && value.it_value.tv_nsec > periodnS))
            {
                memset(&value, 0, sizeof(value));
                value.it_value.tv_sec = periodS;
                value.it_value.tv_nsec = periodnS;
                rc = timer_settime(mTimerId, 0, &value, NULL);
                if (rc != 0)
                {
                    ELOG2(EJavaCaptain, "tickerProvider timer_settime failed(%d) errono=%d", rc, errno);
                }
                mNextTickAt = aJavaTime;
            }
        }
        else
        {
            ELOG2(EJavaCaptain, "tickerProvider timer_gettime failed(%d) errno=%d", rc, errno);
        }
    }
}

long long TickerProvider::getNextTickAt()
{
    JELOG2(EJavaCaptain);

    struct itimerspec value;
    memset(&value, 0, sizeof(value));
    int rc = timer_gettime(mTimerId, &value);

    if (!rc)
    {
        // Timer was not started
        if (!value.it_value.tv_sec && !value.it_value.tv_nsec)
        {
            mNextTickAt = 0LL;
        }
    }
    else
    {
        ELOG2(EJavaCaptain, "tickerProvider timer_gettime failed(%d) errno=%d", rc, errno);
    }

    return mNextTickAt;
}

void TickerProvider::cancel()
{
    JELOG2(EJavaCaptain);

    struct itimerspec value;
    memset(&value, 0, sizeof(value));
    int rc = timer_gettime(mTimerId, &value);

    if (!rc)
    {
        // Stop timer only if it was started
        if (value.it_value.tv_sec || value.it_value.tv_nsec)
        {
            memset(&value, 0, sizeof(value));
            rc = timer_settime(mTimerId, 0, &value, NULL);
            if (rc != 0)
            {
                ELOG2(EJavaCaptain, "tickerProvider timer_settime failed(%d) errono=%d", rc, errno);
            }
        }
    }
    else
    {
        ELOG2(EJavaCaptain, "tickerProvider timer_gettime failed(%d) errno=%d", rc, errno);
    }
}

} // namespace captain
} // namespace java

