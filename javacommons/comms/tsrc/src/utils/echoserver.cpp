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


#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>

#include "logger.h"
#include "javacommonutils.h"
#include "exceptionbase.h"

#include "echoserver.h"

using namespace java::comms;
using namespace std;

EchoServer::EchoServer()
{
}

EchoServer::~EchoServer()
{
}

int EchoServer::start(int addr)
{
    mComms.registerDefaultListener(this);
    return mComms.start(addr);
}

int EchoServer::stop()
{
    mComms.unregisterDefaultListener(this);
    return mComms.stop();
}


void EchoServer::processMessage(CommsMessage& aMessage)
{
    /*
        LOG1(EJavaComms, EInfo, "processMessage, sender    = %d", aMessage.getSender());
        LOG1(EJavaComms, EInfo, "processMessage, messageId = %d", aMessage.getMessageId());
        LOG1(EJavaComms, EInfo, "processMessage, moduleId  = %d", aMessage.getModuleId());
        LOG1(EJavaComms, EInfo, "processMessage, messageRef= %d", aMessage.getMessageRef());
    */
    switch (aMessage.getModuleId())
    {
    case MODULE_ID_NO_REPLY:
        break;

    case MODULE_ID_SLEEP_1S:
    case MODULE_ID_SLEEP_2S:
    case MODULE_ID_SLEEP_5S:
        usleep(1000*aMessage.getModuleId());
        // fall through

    default:
        mReply = aMessage;
        mReply.replyTo(aMessage);
        mComms.send(mReply);
        break;
    }
}

std::string wstringToUtf8(const std::wstring& str)
{
    string result;
    try
    {
        char* utf8 = java::util::JavaCommonUtils::wstringToUtf8(str);
        result.assign(utf8);
        delete[] utf8;
    }
    catch (java::util::ExceptionBase& e)
    {
        ELOG1(EJavaComms, "wstringToUtf8 failed: %s", e.toString().c_str());
    }
    return result;
}

