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
* Description:  timerserverhelpers
*
*/

#ifndef TIMERSERVERHELPERS_H
#define TIMERSERVERHELPERS_H

#include "commsclientendpoint.h"
#include "commslistener.h"
#include "commsmessage.h"
#include "monitor.h"

#include "testmessages.h"

using namespace java::comms;
using namespace java::captain;
using namespace java::util;

extern CommsClientEndpoint* commsClient;

class tsCommsListener : public CommsListener
{
public:
    tsCommsListener(Monitor* aMonitor1, Monitor* aMonitor2, int& aTimerId)
            :mMonitor1(aMonitor1), mMonitor2(aMonitor2), mTimerId(aTimerId)
    {}

    virtual void processMessage(CommsMessage& message);

private:
    Monitor*    mMonitor1;
    Monitor*    mMonitor2;
    int&        mTimerId;
};

void startSecondsTimer(int& timerId, int timeout);
void startJavaTimeTimer(int& timerId, long long timeout);

void stopTimer(int timerId);

#endif // TIMERSERVERHELPERS_H

