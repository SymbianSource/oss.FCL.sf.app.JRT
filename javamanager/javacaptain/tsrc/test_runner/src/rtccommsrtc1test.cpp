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
* Description:  RtcCommsRtc1
*
*/

#include "TestHarness.h"

#include <unistd.h>

#include "testuids.h"

#include "rtccommshelpers.h"

TEST_GROUP(RtcCommsRtc1)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

TEST(RtcCommsRtc1, RTCS14_LaunchSyncTerminateSyncPrematureExit3)
{
    launch(uid_premature_exit3, syncOn, "test1");
    terminate(uid_premature_exit3, syncOn);
}

TEST(RtcCommsRtc1, RTCS13_LaunchSyncTerminateSyncPrematureExit2)
{
    launch(uid_premature_exit2, syncOn, "test1");
    terminate(uid_premature_exit2, syncOn);
}

TEST(RtcCommsRtc1, RTCS12_LaunchSyncTerminateSyncPrematureExit1)
{
    launch(uid_premature_exit1, syncOn, "test1", RTC_LAUNCH_TYPE_NORMAL_C, true);
    terminate(uid_premature_exit1, syncOn);
}

TEST(RtcCommsRtc1, RTCS11_LaunchSyncTerminateSyncWontTerminate2)
{
    launch(uid_wont_terminate2, syncOn, "test1");
    terminate(uid_wont_terminate2, syncOn);
}

TEST(RtcCommsRtc1, RTCS10_05LaunchASyncSleep1sTerminateASyncSleep1sWontTerminate1)
{
    launch(uid_wont_terminate1, syncOff, "test1");
    sleep(1);
    terminate(uid_wont_terminate1, syncOff);
    sleep(1);
}

TEST(RtcCommsRtc1, RTCS10_04LaunchASyncSleep2sTerminateSyncWontTerminate1)
{
    launch(uid_wont_terminate1, syncOff, "test1");
    sleep(2);
    terminate(uid_wont_terminate1, syncOn);
}

TEST(RtcCommsRtc1, RTCS10_03LaunchASyncSleep1sTerminateSyncWontTerminate1)
{
    launch(uid_wont_terminate1, syncOff, "test1");
    sleep(1);
    terminate(uid_wont_terminate1, syncOn);
}

TEST(RtcCommsRtc1, RTCS10_02LaunchASyncTerminateSyncWontTerminate1)
{
    launch(uid_wont_terminate1, syncOff, "test1");
    terminate(uid_wont_terminate1, syncOn);
}

TEST(RtcCommsRtc1, RTCS10_01LaunchSyncTerminateSyncWontTerminate1)
{
    launch(uid_wont_terminate1, syncOn, "test1");
    terminate(uid_wont_terminate1, syncOn);
}

TEST(RtcCommsRtc1, RTCS04_07LaunchASyncTerminateSyncNormal4)
{
    launch(uid_normal1, syncOff, "test1");
    terminate(uid_normal1, syncOn);
}

TEST(RtcCommsRtc1, RTCS04_06_LaunchPushSyncx2TerminateSyncNormal4)
{
    launch(uid_normal4, syncOn, "test1", RTC_LAUNCH_TYPE_PUSH_C);
    launch(uid_normal4, syncOn, "test1", RTC_LAUNCH_TYPE_PUSH_C);
    terminate(uid_normal4, syncOn);
}

TEST(RtcCommsRtc1, RTCS04_05_LaunchPushSyncx2TerminateSyncNormal1)
{
    launch(uid_normal1, syncOn, "test1", RTC_LAUNCH_TYPE_PUSH_C);
    launch(uid_normal1, syncOn, "test1", RTC_LAUNCH_TYPE_PUSH_C);
    terminate(uid_normal1, syncOn);
}

TEST(RtcCommsRtc1, RTCS04_04_LaunchPushAsyncx2TerminateSyncNormal4)
{
    launch(uid_normal4, syncOff, "test1", RTC_LAUNCH_TYPE_PUSH_C);
    launch(uid_normal4, syncOff, "test1", RTC_LAUNCH_TYPE_PUSH_C);
    terminate(uid_normal4, syncOn);
}

TEST(RtcCommsRtc1, RTCS04_03_LaunchPushAsyncx2TerminateSyncNormal1)
{
    launch(uid_normal1, syncOff, "test1", RTC_LAUNCH_TYPE_PUSH_C);
    launch(uid_normal1, syncOff, "test1", RTC_LAUNCH_TYPE_PUSH_C);
    terminate(uid_normal1, syncOn);
}

TEST(RtcCommsRtc1, RTCS04_02_LaunchPushAsyncLaunchSyncTerminateSyncNormal4)
{
    launch(uid_normal4, syncOff, "test1", RTC_LAUNCH_TYPE_PUSH_C);
    launch(uid_normal4, syncOn, "test1");
    terminate(uid_normal4, syncOn);
}

TEST(RtcCommsRtc1, RTCS04_01_LaunchPushAsyncLaunchSyncTerminateSyncNormal1)
{
    launch(uid_normal1, syncOff, "test1", RTC_LAUNCH_TYPE_PUSH_C);
    launch(uid_normal1, syncOn, "test1");
    terminate(uid_normal1, syncOn);
}

TEST(RtcCommsRtc1, RTCS03_LaunchSyncTerminateSyncNormal3)
{
    launch(uid_normal3, syncOn, "test1");
    terminate(uid_normal3, syncOn);
}

TEST(RtcCommsRtc1, RTCS02_07LaunchAsyncSyncTerminateAsyncSyncNormal2)
{
    launch(uid_normal2, syncOff, "test1");
    launch(uid_normal2, syncOn, "test1");
    terminate(uid_normal2, syncOff);
    terminate(uid_normal2, syncOn);
}

TEST(RtcCommsRtc1, RTCS02_06LaunchSyncx2TerminateSyncx2Normal2)
{
    launch(uid_normal2, syncOn, "test1");
    launch(uid_normal2, syncOn, "test1");
    terminate(uid_normal2, syncOn);
    terminate(uid_normal2, syncOn);
}

TEST(RtcCommsRtc1, RTCS02_05LaunchASyncSleep1sTerminateASyncSleep1sNormal2)
{
    launch(uid_normal2, syncOff, "test1");
    sleep(1);
    terminate(uid_normal2, syncOff);
    sleep(1);
}

TEST(RtcCommsRtc1, RTCS02_04LaunchASyncSleep2sTerminateSyncNormal2)
{
    launch(uid_normal2, syncOff, "test1");
    sleep(2);
    terminate(uid_normal2, syncOn);
}

TEST(RtcCommsRtc1, RTCS02_03LaunchASyncSleep1sTerminateSyncNormal2)
{
    launch(uid_normal2, syncOff, "test1");
    sleep(1);
    terminate(uid_normal2, syncOn);
}

TEST(RtcCommsRtc1, RTCS02_02LaunchASyncTerminateSyncNormal2)
{
    launch(uid_normal2, syncOff, "test1");
    terminate(uid_normal2, syncOn);
}

TEST(RtcCommsRtc1, RTCS02_01LaunchSyncTerminateSyncNormal2)
{
    launch(uid_normal2, syncOn, "test1");
    terminate(uid_normal2, syncOn);
}

// RTC01 launching normal1 which is an ideal runtime
TEST(RtcCommsRtc1, RTCS01_07LaunchAsyncSyncTerminateAsyncSyncNormal1)
{
    launch(uid_normal1, syncOff, "test1");
    launch(uid_normal1, syncOn, "test1");
    terminate(uid_normal1, syncOff);
    terminate(uid_normal1, syncOn);
}

TEST(RtcCommsRtc1, RTCS01_06LaunchSyncx2TerminateSyncx2Normal1)
{
    launch(uid_normal1, syncOn, "test1");
    launch(uid_normal1, syncOn, "test1");
    terminate(uid_normal1, syncOn);
    terminate(uid_normal1, syncOn);
}

TEST(RtcCommsRtc1, RTCS01_05LaunchASyncSleep1sTerminateASyncSleep1sNormal1)
{
    launch(uid_normal1, syncOff, "test1");
    sleep(1);
    terminate(uid_normal1, syncOff);
    sleep(1);
}

TEST(RtcCommsRtc1, RTCS01_04LaunchASyncSleep2sTerminateSyncNormal1)
{
    launch(uid_normal1, syncOff, "test1");
    sleep(2);
    terminate(uid_normal1, syncOn);
}

TEST(RtcCommsRtc1, RTCS01_03LaunchASyncSleep1sTerminateSyncNormal1)
{
    launch(uid_normal1, syncOff, "test1");
    sleep(1);
    terminate(uid_normal1, syncOn);
}

TEST(RtcCommsRtc1, RTCS01_02LaunchASyncTerminateSyncNormal1)
{
    launch(uid_normal1, syncOff, "test1");
    terminate(uid_normal1, syncOn);
}

TEST(RtcCommsRtc1, RTCS01_01LaunchSyncTerminateSyncNormal1)
{
    launch(uid_normal1, syncOn, "test1");
    terminate(uid_normal1, syncOn);
}
