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
* Description:
*
*/


#include "serviceclasshandler.h"
#include "applicationinfo.h"
#include "comms.h"
#include "logger.h"

using namespace java::bluetooth;
using namespace java::comms;


/*
 * If aSrvClassVal equals 0, then it is assumed that midlet gets closed.
 */

OS_EXPORT int ServiceClassHandler::setDeviceServiceClass(
    unsigned int aSrvClassVal, bool aIsServiceBitsPersistent)
{
    JELOG2(EJavaBluetooth);

    // Send message to set the service class value
    LOG1(
        EJavaBluetooth,
        EInfo,
        "  ServiceClassHandler::setDeviceServiceClass setting serviceClass:0x%X",
        aSrvClassVal);

    CommsMessage msg;
    msg.setModuleId(PLUGIN_ID_BT_DEVICE_CLASS_MANAGER_C);
    msg.setMessageId(JSR_82_MESSAGE_ID_RANGE_START_C);
    msg << (int) aIsServiceBitsPersistent;
    msg << getMidletId();
    msg << (int) aSrvClassVal;
    return sendMsg(msg);
}

std::wstring ServiceClassHandler::getMidletId()
{
    JELOG2(EJavaBluetooth);
    const java::runtime::ApplicationInfo& appInf =
        java::runtime::ApplicationInfo::getInstance();
    const java::util::Uid &uid = appInf.getUid();
    LOG1(EJavaBluetooth, EInfo,
         "- ServiceClassHandler::getMidletId MidletID:%S",
         uid.toString().c_str());
    return uid.toString();
}

int ServiceClassHandler::sendMsg(CommsMessage aMsg)
{
    JELOG2(EJavaBluetooth);

    CommsClientEndpoint comms;
    int rc = comms.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
    if (rc != 0)
    {
        ELOG1(
            EJavaBluetooth,
            "- ServiceClassHandler::sendMsg Comm connection failed! Err:%d",
            rc);
        return -1;
    }

    CommsMessage recvMsg;

    rc = comms.sendReceive(aMsg, recvMsg, WAIT_FOR_EVER);
    if (rc!=0)
    {
        ELOG1(
            EJavaBluetooth,
            "- ServiceClassHandler::sendMsg sendReceive failed! Err:%d",
            rc);
        return -1;
    }
    int retVal;
    recvMsg >> retVal;

    comms.disconnect();

    return retVal;
}
