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
* Description:  rtcinterfacehelpers
*
*/


#include "TestHarness.h"

#include "commsmessage.h"

#include "rtcmessages.h"

#include "testmessages.h"
#include "rtcinterfacehelpers.h"

using java::comms::CommsMessage;
using namespace java::captain;

void iLaunch(const Uid& aUid, const bool& /*sync*/, std::string rtc, const int type)
{
// Launch start
    CommsMessage msg;
    setLaunchAppParameters(msg, aUid, type, rtc);
    CHECK(commsClient->send(msg) == 0);
// Launch end
}

void iTerminate(const Uid& aUid, const bool& /*sync*/)
{
    // To make sure that the application is not running
    CommsMessage msg;
    setTerminateAppParameters(msg, aUid);
    CHECK(commsClient->send(msg) == 0);
}

void iDisable(const Uid& aUid)
{
    CommsMessage msg;
    setDisableAppParameters(msg, aUid);
    CHECK(commsClient->send(msg) == 0);
}

void iEnable(const Uid& aUid)
{
    CommsMessage msg;
    setEnableAppParameters(msg, aUid);
    CHECK(commsClient->send(msg) == 0);
}

