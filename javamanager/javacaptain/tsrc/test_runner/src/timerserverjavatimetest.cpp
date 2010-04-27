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
* Description:  TimerServerJavaTime
*
*/

#include "TestHarness.h"

#include <sys/time.h>
#include <time.h>


#include "commsclientendpoint.h"
#include "commslistener.h"
#include "comms.h"
#include "monitor.h"

#include "testmessages.h"
#include "timerserverhelpers.h"

using namespace java::comms;
using namespace java::captain;
using namespace java::util;

extern java::comms::CommsClientEndpoint* commsClient;

static Monitor* monitor1;
static Monitor* monitor2;
static int      timerId;

class JavaTime
{
public:
    JavaTime():mMillisFromEpoch(0LL) {}
    JavaTime(const long long& aTime):mMillisFromEpoch(aTime) {}
    JavaTime(const JavaTime& aJt):mMillisFromEpoch(aJt.getTime()) {}
    JavaTime& operator= (const JavaTime& aJt)
    {
        mMillisFromEpoch = aJt.getTime();
        return *this;
    }
    const long long& operator+ (const long long& a)
    {
        return (this->mMillisFromEpoch += a);
    }
    virtual ~JavaTime() {}

    virtual long long getTime() const
    {
        return mMillisFromEpoch;
    }
    virtual void setTime(const long long& aNewTime)
    {
        mMillisFromEpoch = aNewTime;
    }

private:
    long long mMillisFromEpoch;
};

long long getCurrentJavaTime()
{
    timeval tim;
    int err = gettimeofday(&tim, NULL);
    if (-1 == err)
    {
        return -1LL;
    }

    return (tim.tv_sec * 1000LL) +
           (tim.tv_usec / 1000LL);
}


TEST_GROUP(TimerServerJavaTime)
{
    tsCommsListener* tsListener;
    TEST_SETUP()
    {
        monitor1 = Monitor::createMonitor();
        monitor2 = Monitor::createMonitor();
        tsListener = new tsCommsListener(monitor1, monitor2, timerId);
        commsClient->registerDefaultListener(tsListener);
    }

    TEST_TEARDOWN()
    {
        commsClient->unregisterDefaultListener(tsListener);
        delete tsListener;
        delete monitor2;
        delete monitor1;
    }
};

static void omaEquals(int& a, int&b)
{
    if (a != b)
        printf("Comparing 0x%x == 0x%x fails!!!\n", a, b);
    CHECK(a == b);
}
/*
TEST(TimerServerJavaTime, TS10_STOP)
{
    int timerId = 0;
    startTimer(timerId, 10);
    stopTimer(timerId);
}

TEST(TimerServerJavaTime, TS09_3_timers_a2_b2_c2_seconds_c_canceled)
{
    int tid1=0, tid2=0, tid3=0;
    startTimer(tid1, 2);
    startTimer(tid2, 2);
    startTimer(tid3, 2);
    stopTimer(tid3);
    monitor->wait();
    CHECK(receivedTimeout==tid2 || receivedTimeout==tid1);
    monitor2->notify();

    monitor->wait();
    CHECK(receivedTimeout==tid2 || receivedTimeout==tid1);
    monitor2->notify();
}

TEST(TimerServerJavaTime, TS08_3_timers_a1_b2_c3_seconds_b_canceled)
{   int tid1=0, tid2=0, tid3=0;
    startTimer(tid1, 1);
    startTimer(tid2, 2);
    stopTimer(tid2);
    startTimer(tid3, 3);
    monitor->wait();
    CHECK(receivedTimeout==tid1 || receivedTimeout==tid3);
    monitor2->notify();

    monitor->wait();
    CHECK(receivedTimeout==tid1 || receivedTimeout==tid3);
    monitor2->notify();
}

TEST(TimerServerJavaTime, TS07_3_timers_a3_b2_c1_seconds_a_canceled)
{
    int tid1=0, tid2=0, tid3=0;
    startTimer(tid1, 3);
    startTimer(tid2, 2);
    stopTimer(tid1);
    startTimer(tid3, 1);
    monitor->wait();
    CHECK(receivedTimeout==tid2 || receivedTimeout==tid3);
    monitor2->notify();

    monitor->wait();
    CHECK(receivedTimeout==tid2 || receivedTimeout==tid3);
    monitor2->notify();
}

TEST(TimerServerJavaTime, TS06_3_timers_2_1_2_seconds)
{
    int tid1=0, tid2=0, tid3=0;
    startTimer(tid1, 2);
    startTimer(tid2, 1);
    startTimer(tid3, 2);
    monitor->wait();
    omaEquals(tid2, receivedTimeout);
//    CHECK(tid2 == receivedTimeout);
    monitor2->notify();

    monitor->wait();
    CHECK(receivedTimeout==tid1 || receivedTimeout==tid3);
    monitor2->notify();

    monitor->wait();
    CHECK(receivedTimeout==tid1 || receivedTimeout==tid3);
    monitor2->notify();
}


TEST(TimerServerJavaTime, TS05_3_timers_1_2_3_seconds)
{
    int tid1=0, tid2=0, tid3=0;
    startTimer(tid1, 1);
    startTimer(tid2, 2);
    startTimer(tid3, 3);
    monitor->wait();
    omaEquals(tid1, receivedTimeout);
//    CHECK(tid1 == receivedTimeout);
    monitor2->notify();

    monitor->wait();
    omaEquals(tid2, receivedTimeout);
//    CHECK(tid2 == receivedTimeout);
    monitor2->notify();

    monitor->wait();
    omaEquals(tid3, receivedTimeout);
//    CHECK(tid3 == receivedTimeout);
    monitor2->notify();
}
*/
TEST(TimerServerJavaTime, TSJT02_01_3_timers_2_1_2_seconds)
{
    int tid1=0, tid2=0, tid3=0;
    startJavaTimeTimer(tid1, JavaTime(getCurrentJavaTime()+(2L * 1000L)).getTime());
    startJavaTimeTimer(tid2, JavaTime(getCurrentJavaTime()+(1L * 1000L)).getTime());
    startJavaTimeTimer(tid3, JavaTime(getCurrentJavaTime()+(2L * 1000L)).getTime());

    monitor1->wait();
    CHECK(tid2 == timerId);
    monitor2->notify();

    monitor1->wait();
    CHECK(tid1 == timerId || tid3 == timerId);
    monitor2->notify();

    monitor1->wait();
    CHECK(tid1 == timerId || tid3 == timerId);
    monitor2->notify();
}

TEST(TimerServerJavaTime, TSJT02_02_3_timers_1_2_3_seconds)
{
    int tid1=0, tid2=0, tid3=0;
    startJavaTimeTimer(tid1, JavaTime(getCurrentJavaTime()+(1L * 1000L)).getTime());
    startJavaTimeTimer(tid2, JavaTime(getCurrentJavaTime()+(2L * 1000L)).getTime());
    startJavaTimeTimer(tid3, JavaTime(getCurrentJavaTime()+(3L * 1000L)).getTime());

    monitor1->wait();
    CHECK(tid1 == timerId);
    monitor2->notify();

    monitor1->wait();
    CHECK(tid2 == timerId);
    monitor2->notify();

    monitor1->wait();
    CHECK(tid3 == timerId);
    monitor2->notify();
}

TEST(TimerServerJavaTime, TSJT02_01_3_timers_3_2_1_seconds)
{
    int tid1=0, tid2=0, tid3=0;
    startJavaTimeTimer(tid1, JavaTime(getCurrentJavaTime()+(3L * 1000L)).getTime());
    startJavaTimeTimer(tid2, JavaTime(getCurrentJavaTime()+(2L * 1000L)).getTime());
    startJavaTimeTimer(tid3, JavaTime(getCurrentJavaTime()+(1L * 1000L)).getTime());

    monitor1->wait();
    CHECK(tid3 == timerId);
    monitor2->notify();

    monitor1->wait();
    CHECK(tid2 == timerId);
    monitor2->notify();

    monitor1->wait();
    CHECK(tid1 == timerId);
    monitor2->notify();
}

TEST(TimerServerJavaTime, TSJT01_05_5_timers_2_2_2_2_2_seconds)
{
    int tid1=0, tid2=0, tid3=0, tid4=0, tid5=0;
    JavaTime jt(getCurrentJavaTime()+(2L * 1000L));
    startJavaTimeTimer(tid1, jt.getTime());
    startJavaTimeTimer(tid2, jt.getTime());
    startJavaTimeTimer(tid3, jt.getTime());
    startJavaTimeTimer(tid4, jt.getTime());
    startJavaTimeTimer(tid5, jt.getTime());

    monitor1->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3 || timerId==tid4 || timerId==tid5);
    monitor2->notify();
    monitor1->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3 || timerId==tid4 || timerId==tid5);
    monitor2->notify();
    monitor1->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3 || timerId==tid4 || timerId==tid5);
    monitor2->notify();
    monitor1->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3 || timerId==tid4 || timerId==tid5);
    monitor2->notify();
    monitor1->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3 || timerId==tid4 || timerId==tid5);
    monitor2->notify();
}

TEST(TimerServerJavaTime, TSJT01_04_4_timers_2_2_2_2_seconds)
{
    int tid1=0, tid2=0, tid3=0, tid4=0;
    startJavaTimeTimer(tid1, JavaTime(getCurrentJavaTime()+(2L * 1000L)).getTime());
    startJavaTimeTimer(tid2, JavaTime(getCurrentJavaTime()+(1900L)).getTime());
    startJavaTimeTimer(tid3, JavaTime(getCurrentJavaTime()+(1800L)).getTime());
    startJavaTimeTimer(tid4, JavaTime(getCurrentJavaTime()+(1700L)).getTime());

    monitor1->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3 || timerId==tid4);
    monitor2->notify();
    monitor1->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3 || timerId==tid4);
    monitor2->notify();
    monitor1->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3 || timerId==tid4);
    monitor2->notify();
    monitor1->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3 || timerId==tid4);
    monitor2->notify();
}

TEST(TimerServerJavaTime, TSJT01_03_3_timers_2_2_2_seconds)
{
    int tid1=0, tid2=0, tid3=0;
    startJavaTimeTimer(tid1, JavaTime(getCurrentJavaTime()+(2L * 1000L)).getTime());
    startJavaTimeTimer(tid2, JavaTime(getCurrentJavaTime()+(2L * 1000L)).getTime());
    startJavaTimeTimer(tid3, JavaTime(getCurrentJavaTime()+(2L * 1000L)).getTime());

    monitor1->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3);
    monitor2->notify();

    monitor1->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3);
    monitor2->notify();

    monitor1->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3);
    monitor2->notify();
}

TEST(TimerServerJavaTime, TSJT01_02_2_timers_2_2_seconds)
{
    int tid1=0, tid2=0;
    startJavaTimeTimer(tid1, JavaTime(getCurrentJavaTime()+(2L * 1000L)).getTime());
    startJavaTimeTimer(tid2, JavaTime(getCurrentJavaTime()+(2L * 1000L)).getTime());

    monitor1->wait();
    CHECK(timerId==tid1 || timerId==tid2);
    monitor2->notify();

    monitor1->wait();
    CHECK(timerId==tid1 || timerId==tid2);
    monitor2->notify();
}

TEST(TimerServerJavaTime, TSJT01_01_1_timer_2_seconds)
{
    int tid = 0;
    JavaTime jt(getCurrentJavaTime() + (2L * 1000L));
    startJavaTimeTimer(tid, jt.getTime());
    monitor1->wait();
//    CHECK(tid == receivedTimeout);
    omaEquals(tid, timerId);
    monitor2->notify();
}

