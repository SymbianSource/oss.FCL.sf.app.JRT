/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "commsmsgsender.h"
#include "comms.h"
#include "certstoretestextensionplugin.h"
#include "testfailedexception.h"

using namespace java::comms;
using namespace java::security;

CommsMsgSender::CommsMsgSender()
{
    mComms.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
}

CommsMsgSender::~CommsMsgSender()
{
    mComms.disconnect();
}

void CommsMsgSender::SendStartJavaCertStorePlugin()
{
    SendMsg(IPC_START_JAVA_CERT_STORE_EXT_PLUGIN);
}

void CommsMsgSender::SendStopJavaCertStorePlugin()
{
    SendMsg(IPC_STOP_JAVA_CERT_STORE_EXT_PLUGIN);
}

void CommsMsgSender::SendMsg(int aMsgId)
{
    CommsMessage msg;
    msg.setReceiver(IPC_ADDRESS_JAVA_CAPTAIN_C);
    msg.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
    msg.setModuleId(PLUGIN_ID_CERT_STORE_PLUGIN_TESTER_C);
    msg.setMessageId(aMsgId);
    int err = mComms.send(msg);
    if (0 != err)
    {
        throw TestFailedException("sending comms msg failed",
                                  __FILE__,__FUNCTION__,__LINE__);
    }
}

