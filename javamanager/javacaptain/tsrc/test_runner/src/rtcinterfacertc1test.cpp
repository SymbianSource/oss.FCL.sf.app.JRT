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
* Description:  RtcInterfaceRtc1
*
*/

#include "TestHarness.h"

#include <unistd.h>

#include "testuids.h"

#include "rtcinterfacehelpers.h"

using namespace java::captain;

TEST_GROUP(RtcInterfaceRtc1)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

TEST(RtcInterfaceRtc1, RTCIS01_02LaunchBackgroundTerminateNormal1)
{
    iLaunch(uid_normal1, syncOn, "test1", RTC_LAUNCH_TYPE_BACKGROUND_C);
    iTerminate(uid_normal1, syncOn);
}

TEST(RtcInterfaceRtc1, RTCIS01_03LaunchAutoInvocationTerminateNormal1)
{
    iLaunch(uid_normal1, syncOn, "test1", RTC_LAUNCH_TYPE_AUTO_INVOCATION_C);
    iTerminate(uid_normal1, syncOn);
}

TEST(RtcInterfaceRtc1, RTCIS01_02LaunchPushTerminateNormal1)
{
    iLaunch(uid_normal1, syncOn, "test1", RTC_LAUNCH_TYPE_PUSH_C);
    iTerminate(uid_normal1, syncOn);
}

TEST(RtcInterfaceRtc1, RTCIS01_01LaunchNormalTerminateNormal1)
{
    iLaunch(uid_normal1, syncOn, "test1", RTC_LAUNCH_TYPE_NORMAL_C);
    iTerminate(uid_normal1, syncOn);
}

