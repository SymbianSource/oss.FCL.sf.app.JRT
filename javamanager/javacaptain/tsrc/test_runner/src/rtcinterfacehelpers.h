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

#ifndef RTCINTERFACEHELPERS_H
#define RTCINTERFACEHELPERS_H


#include "commsclientendpoint.h"
#include "javauid.h"

#include "rtcmessages.h"

using java::util::Uid;
using namespace java::captain;

extern java::comms::CommsClientEndpoint* commsClient;

const int sendReceiveTimeout = 10;
const bool syncOn = true;
const bool syncOff = false;

void iLaunch(const Uid& aUid, const bool& sync, std::string rtc = "midp", const int type = RTC_LAUNCH_TYPE_NORMAL_C);
void iTerminate(const Uid& aUid, const bool& sync);
void iDisable(const Uid& aUid);
void iEnable(const Uid& aUid);

#endif // RTCINTERFACEHELPERS_H

