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
* Description:  rtccommshelpers
*
*/

#ifndef RTCCOMMSHELPERS_H
#define RTCCOMMSHELPERS_H

#include "commsclientendpoint.h"
#include "javauid.h"

#include "rtcmessages.h"

using java::util::Uid;
using namespace java::captain;

extern java::comms::CommsClientEndpoint* commsClient;

const int sendReceiveTimeout = 10;
const bool syncOn = true;
const bool syncOff = false;

void launch(const Uid& aUid, const bool& sync, std::string rtc = "midp", const int& type = RTC_LAUNCH_TYPE_NORMAL_C, const bool& allowFail = false);
//void launch(const Uid& aUid, const bool& sync, std::string rtc = "midp", const int& type, const bool& allowFail)

void terminate(const Uid& aUid, const bool& sync);
void disable(const Uid& aUid);
void enable(const Uid& aUid);

#endif // RTCCOMMSHELPERS_H

