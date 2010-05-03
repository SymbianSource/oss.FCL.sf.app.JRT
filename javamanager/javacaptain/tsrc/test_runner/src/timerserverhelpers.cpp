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


#include "TestHarness.h"

#include "timerserverhelpers.h"

using namespace java::comms;
using namespace java::captain;
using namespace java::util;

static const int sendReceiveTimeout = 10;

void tsCommsListener::processMessage(CommsMessage& message)
{
    switch (message.getMessageId())
    {
    case IPC_MESSAGE_TEST_TIMEOUT:
        getTimeoutParameters(message, mTimerId);
        mMonitor1->notify();
        mMonitor2->wait();
    }
}


void startSecondsTimer(int& timerId, int timeout)
{
    CommsMessage msg;
    msg.setModuleId(PLUGIN_ID_JAVACAPTAIN_TESTER_C);
    setTimerStartSecondsParameters(msg, timeout);

    CommsMessage receivedMsg;
    CHECK(commsClient->sendReceive(msg, receivedMsg, sendReceiveTimeout) == 0);

    CHECK(msg.getModuleId() == receivedMsg.getModuleId());
    CHECK(receivedMsg.getMessageId() == IPC_MESSAGE_TEST_TIMER_ACK);
    CHECK(msg.getMessageRef() == receivedMsg.getMessageRef());

    getTimerAckParameters(receivedMsg, timerId);
    CHECK(timerId != 0);
}

void startJavaTimeTimer(int& timerId, long long timeout)
{
    CommsMessage msg;
    msg.setModuleId(PLUGIN_ID_JAVACAPTAIN_TESTER_C);
    setTimerStartJavaTimeParameters(msg, timeout);

    CommsMessage receivedMsg;
    CHECK(commsClient->sendReceive(msg, receivedMsg, sendReceiveTimeout) == 0);

    CHECK(msg.getModuleId() == receivedMsg.getModuleId());
    CHECK(receivedMsg.getMessageId() == IPC_MESSAGE_TEST_TIMER_ACK);
    CHECK(msg.getMessageRef() == receivedMsg.getMessageRef());

    getTimerAckParameters(receivedMsg, timerId);
    CHECK(timerId != 0);
}

void stopTimer(int aTimerId)
{
    CommsMessage msg;
    msg.setModuleId(PLUGIN_ID_JAVACAPTAIN_TESTER_C);
    setTimerStopParameters(msg, aTimerId);

    CommsMessage receivedMsg;
    CHECK(commsClient->sendReceive(msg, receivedMsg, sendReceiveTimeout) == 0);

    CHECK(msg.getModuleId() == receivedMsg.getModuleId());
    CHECK(receivedMsg.getMessageId() == IPC_MESSAGE_TEST_TIMER_ACK);
    CHECK(msg.getMessageRef() == receivedMsg.getMessageRef());

    int timerId;
    getTimerAckParameters(receivedMsg, timerId);
    CHECK(timerId == aTimerId);
    return;
}

