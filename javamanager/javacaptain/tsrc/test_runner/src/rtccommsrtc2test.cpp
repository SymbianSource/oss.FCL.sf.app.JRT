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
* Description:  RtcCommsRtc2
*
*/

#include "TestHarness.h"

#include <unistd.h>

#include "testuids.h"

#include "rtccommshelpers.h"

TEST_GROUP(RtcCommsRtc2)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

TEST(RtcCommsRtc2, RTCD08_LaunchSyncTerminateSyncPrematureExit3)
{
    launch(uid_premature_exit3, syncOn, "test2");
    terminate(uid_premature_exit3, syncOn);
}

TEST(RtcCommsRtc2, RTCD07_LaunchSyncTerminateSyncPrematureExit2)
{
    launch(uid_premature_exit2, syncOn, "test2");
    terminate(uid_premature_exit2, syncOn);
}

TEST(RtcCommsRtc2, RTCD06_LaunchSyncTerminateSyncPrematureExit1)
{
    launch(uid_premature_exit1, syncOn, "test2", RTC_LAUNCH_TYPE_NORMAL_C, true);
    terminate(uid_premature_exit1, syncOn);
}

TEST(RtcCommsRtc2, RTCD05_LaunchSyncTerminateSyncWontTerminate2)
{
    launch(uid_wont_terminate2, syncOn, "test2");
    terminate(uid_wont_terminate2, syncOn);
}

TEST(RtcCommsRtc2, RTCD04_05LaunchASyncSleep1sTerminateASyncSleep1sWontTerminate1)
{
    launch(uid_wont_terminate1, syncOff, "test2");
    sleep(1);
    terminate(uid_wont_terminate1, syncOff);
    sleep(1);
}

TEST(RtcCommsRtc2, RTCD04_04LaunchASyncSleep2sTerminateSyncWontTerminate1)
{
    launch(uid_wont_terminate1, syncOff, "test2");
    sleep(2);
    terminate(uid_wont_terminate1, syncOn);
}

TEST(RtcCommsRtc2, RTCD04_03LaunchASyncSleep1sTerminateSyncWontTerminate1)
{
    launch(uid_wont_terminate1, syncOff, "test2");
    sleep(1);
    terminate(uid_wont_terminate1, syncOn);
}

TEST(RtcCommsRtc2, RTCD04_02LaunchASyncTerminateSyncWontTerminate1)
{
    launch(uid_wont_terminate1, syncOff, "test2");
    terminate(uid_wont_terminate1, syncOn);
}

TEST(RtcCommsRtc2, RTCD04_01LaunchSyncTerminateSyncWontTerminate1)
{
    launch(uid_wont_terminate1, syncOn, "test2");
    terminate(uid_wont_terminate1, syncOn);
}

TEST(RtcCommsRtc2, RTCD03_LaunchSyncTerminateSyncNormal3)
{
    launch(uid_normal3, syncOn, "test2");
    terminate(uid_normal3, syncOn);
}

TEST(RtcCommsRtc2, RTCD02_LaunchSyncTerminateSyncNormal2)
{
    launch(uid_normal2, syncOn, "test2");
    terminate(uid_normal2, syncOn);
}

// RTC01 launching normal1 which is an ideal runtime

TEST(RtcCommsRtc2, RTCD01_05LaunchASyncSleep1sTerminateASyncSleep1sNormal1)
{
    launch(uid_normal1, syncOff, "test2");
    sleep(1);
    terminate(uid_normal1, syncOff);
    sleep(1);
}

TEST(RtcCommsRtc2, RTCD01_04LaunchASyncSleep2sTerminateSyncNormal1)
{
    launch(uid_normal1, syncOff, "test2");
    sleep(2);
    terminate(uid_normal1, syncOn);
}

TEST(RtcCommsRtc2, RTCD01_03LaunchASyncSleep1sTerminateSyncNormal1)
{
    launch(uid_normal1, syncOff, "test2");
    sleep(1);
    terminate(uid_normal1, syncOn);
}

TEST(RtcCommsRtc2, RTCD01_02LaunchASyncTerminateSyncNormal1)
{
    launch(uid_normal1, syncOff, "test2");
    terminate(uid_normal1, syncOn);
}

TEST(RtcCommsRtc2, RTCD01_01LaunchSyncTerminateSyncNormal1)
{
    launch(uid_normal1, syncOn, "test2");
    terminate(uid_normal1, syncOn);
}

