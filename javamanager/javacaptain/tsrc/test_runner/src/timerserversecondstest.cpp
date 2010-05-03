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
* Description:  TimerServerSecondsTest
*
*/

#include "TestHarness.h"

#include "commsclientendpoint.h"
#include "commslistener.h"
#include "comms.h"
#include "monitor.h"

#include "testmessages.h"
#include "timerserverhelpers.h"

using namespace java::comms;
using namespace java::captain;
using namespace java::util;

static Monitor* monitorS = 0;
static Monitor* monitorS2 = 0;
static int timerId = 0;

TEST_GROUP(TimerServerSeconds)
{
    tsCommsListener* tsListener2;

    TEST_SETUP()
    {
        monitorS = Monitor::createMonitor();
        monitorS2 = Monitor::createMonitor();
        tsListener2 = new tsCommsListener(monitorS, monitorS2, timerId);
        commsClient->registerDefaultListener(tsListener2);
    }

    TEST_TEARDOWN()
    {
        monitorS2->notify();
        commsClient->unregisterDefaultListener(tsListener2);
        delete tsListener2;
        delete monitorS;
        delete monitorS2;
    }
};


TEST(TimerServerSeconds, TSS03_03_3_timers_a2_b2_c2_seconds_c_canceled)
{
    int tid1=0, tid2=0, tid3=0;
    startSecondsTimer(tid1, 2);
    startSecondsTimer(tid2, 2);
    startSecondsTimer(tid3, 2);
    stopTimer(tid3);

    monitorS->wait();
    CHECK(tid1 == timerId || tid2 == timerId);
    monitorS2->notify();

    monitorS->wait();
    CHECK(tid1 == timerId || tid2 == timerId);
    monitorS2->notify();
}

TEST(TimerServerSeconds, TSS03_02_3_timers_a1_b2_c3_seconds_b_canceled)
{
    int tid1=0, tid2=0, tid3=0;
    startSecondsTimer(tid1, 1);
    startSecondsTimer(tid2, 2);
    stopTimer(tid2);
    startSecondsTimer(tid3, 3);

    monitorS->wait();
    CHECK(tid1 == timerId);
    monitorS2->notify();

    monitorS->wait();
    CHECK(tid3 == timerId);
    monitorS2->notify();
}

TEST(TimerServerSeconds, TSS03_01_3_timers_a3_b2_c1_seconds_a_canceled)
{
    int tid1=0, tid2=0, tid3=0;
    startSecondsTimer(tid1, 3);
    startSecondsTimer(tid2, 2);
    stopTimer(tid1);
    startSecondsTimer(tid3, 1);

    monitorS->wait();
    CHECK(tid3 == timerId);
    monitorS2->notify();

    monitorS->wait();
    CHECK(tid2 == timerId);
    monitorS2->notify();
}

TEST(TimerServerSeconds, TSS02_03_3_timers_2_1_2_seconds)
{
    int tid1=0, tid2=0, tid3=0;
    startSecondsTimer(tid1, 2);
    startSecondsTimer(tid2, 1);
    startSecondsTimer(tid3, 2);

    monitorS->wait();
    CHECK(tid2 == timerId);
    monitorS2->notify();

    monitorS->wait();
    CHECK(timerId==tid1 || timerId==tid3);
    monitorS2->notify();

    monitorS->wait();
    CHECK(timerId==tid1 || timerId==tid3);
    monitorS2->notify();
}

TEST(TimerServerSeconds, TSS02_02_3_timers_1_2_3_seconds)
{
    int tid1=0, tid2=0, tid3=0;
    startSecondsTimer(tid1, 1);
    startSecondsTimer(tid2, 2);
    startSecondsTimer(tid3, 3);

    monitorS->wait();
    CHECK(timerId==tid1);
    monitorS2->notify();

    monitorS->wait();
    CHECK(timerId==tid2);
    monitorS2->notify();

    monitorS->wait();
    CHECK(timerId==tid3);
    monitorS2->notify();
}

TEST(TimerServerSeconds, TSS02_01_3_timers_3_2_1_seconds)
{
    int tid1=0, tid2=0, tid3=0;
    startSecondsTimer(tid1, 3);
    startSecondsTimer(tid2, 2);
    startSecondsTimer(tid3, 1);

    monitorS->wait();
    CHECK(timerId==tid3);
    monitorS2->notify();

    monitorS->wait();
    CHECK(timerId==tid2);
    monitorS2->notify();

    monitorS->wait();
    CHECK(timerId==tid1);
    monitorS2->notify();
}

TEST(TimerServerSeconds, TSS01_05_5_timers_2_2_2_2_2_seconds)
{
    int tid1=0, tid2=0, tid3=0, tid4=0, tid5=0;
    startSecondsTimer(tid1, 2);
    startSecondsTimer(tid2, 2);
    startSecondsTimer(tid3, 2);
    startSecondsTimer(tid4, 2);
    startSecondsTimer(tid5, 2);

    monitorS->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3 || timerId==tid4 || timerId==tid5);
    monitorS2->notify();

    monitorS->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3 || timerId==tid4 || timerId==tid5);
    monitorS2->notify();

    monitorS->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3 || timerId==tid4 || timerId==tid5);
    monitorS2->notify();

    monitorS->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3 || timerId==tid4 || timerId==tid5);
    monitorS2->notify();

    monitorS->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3 || timerId==tid4 || timerId==tid5);
    monitorS2->notify();
}

TEST(TimerServerSeconds, TSS01_04_4_timers_2_2_2_2_seconds)
{
    int tid1=0, tid2=0, tid3=0, tid4=0;
    startSecondsTimer(tid1, 2);
    startSecondsTimer(tid2, 2);
    startSecondsTimer(tid3, 2);
    startSecondsTimer(tid4, 2);

    monitorS->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3 || timerId==tid4);
    monitorS2->notify();

    monitorS->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3 || timerId==tid4);
    monitorS2->notify();

    monitorS->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3 || timerId==tid4);
    monitorS2->notify();

    monitorS->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3 || timerId==tid4);
    monitorS2->notify();
}

TEST(TimerServerSeconds, TSS01_03_3_timers_2_2_2_seconds)
{
    int tid1=0, tid2=0, tid3=0;
    startSecondsTimer(tid1, 2);
    startSecondsTimer(tid2, 2);
    startSecondsTimer(tid3, 2);

    monitorS->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3);
    monitorS2->notify();

    monitorS->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3);
    monitorS2->notify();

    monitorS->wait();
    CHECK(timerId==tid1 || timerId==tid2 || timerId==tid3);
    monitorS2->notify();
}

TEST(TimerServerSeconds, TSS01_02_2_timers_2_2_seconds)
{
    int tid1=0, tid2=0;
    startSecondsTimer(tid1, 2);
    startSecondsTimer(tid2, 2);

    monitorS->wait();
    CHECK(timerId==tid1 || timerId==tid2);
    monitorS2->notify();

    monitorS->wait();
    CHECK(timerId==tid1 || timerId==tid2);
    monitorS2->notify();
}

TEST(TimerServerSeconds, TSS01_01_1_timer_2_seconds)
{
    int tid = 0;
    startSecondsTimer(tid, 2);

    monitorS->wait();
    CHECK(tid == timerId);
    monitorS2->notify();
}

