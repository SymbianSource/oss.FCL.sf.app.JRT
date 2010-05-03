/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
 *
*/

#ifndef ECHOSERVER_H
#define ECHOSERVER_H

#include "commslistener.h"
#include "commsserverendpoint.h"
#include "commsmessage.h"

using namespace java::comms;

const int MODULE_ID_NO_REPLY    = 100;
const int MODULE_ID_ECHO        = 101;
const int MODULE_ID_SLEEP_1S    = 1000;
const int MODULE_ID_SLEEP_2S    = 2000;
const int MODULE_ID_SLEEP_5S    = 5000;

const int IPC_ADDRESS_COMMS_MODULE_TEST = 10321;

class EchoServer : public CommsListener
{
public:
    EchoServer();
    virtual ~EchoServer();

    // CommsListener methods
    virtual void processMessage(CommsMessage& aMessage);

    int start(int addr);
    int stop();

    CommsEndpoint* getComms()
    {
        return &mComms;
    }

private:
    CommsMessage mReply;
    CommsServerEndpoint mComms;
};


class EchoListener : public CommsListener
{
public:
    EchoListener(CommsEndpoint* comms)
    {
        mComms = comms;
    }

    virtual void processMessage(CommsMessage& message)
    {
        CommsMessage reply = message;
        reply.replyTo(message);
        mComms->send(reply);

    }
private:
    CommsEndpoint* mComms;
};

std::string wstringToUtf8(const std::wstring& str);

#endif // ECHOSERVER_H
