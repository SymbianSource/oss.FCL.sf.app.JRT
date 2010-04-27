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



#include "servertestutils.h"
#include "comms.h"
#include "commsclientendpoint.h"
#include "commsmessage.h"

using namespace java::comms;
using namespace java::storage;
using namespace std;

ServerTestUtils::ServerTestUtils()
{
}

ServerTestUtils::~ServerTestUtils()
{
}

bool ServerTestUtils::connect()
{
    bool result = true;

    try
    {
        mCliConn = new CommsClientEndpoint();
        int resultCode = mCliConn->connect(IPC_ADDRESS_JAVA_STORAGE_C);

        if (resultCode != 0)
        {
            result = false;
            delete mCliConn;
            mCliConn = NULL;
        }
    }
    catch (...)
    {
        printf("Exp. when connecting\n");
        result = false;
    }

    return result;
}

bool ServerTestUtils::disconnect()
{
    bool result = true;
    try
    {
        int resultCode = mCliConn->disconnect();

        if (resultCode != 0)
        {
            result = false;
            printf("Comms disconnect result code: %d\n", resultCode);
        }
    }
    catch (...)
    {
        result = false;
        printf("Exp. when disconnecting\n");
    }

    delete mCliConn;
    mCliConn = NULL;

    return result;
}

bool ServerTestUtils::sendReceive(CommsMessage& aSendMsg,
                                  CommsMessage& aReceivedMsg)
{
    bool result = true;
    try
    {
        int resultCode = -1;

        aSendMsg.setReceiver(IPC_ADDRESS_JAVA_STORAGE_C);
        aSendMsg.setModuleId(PLUGIN_ID_JAVASTORAGE_CLIENT_C);

        resultCode = mCliConn->sendReceive(aSendMsg, aReceivedMsg, 10);

        if (resultCode < 0)
        {
            result = false;
            printf("Comms result code: %d\n", resultCode);
        }
    }
    catch (...)
    {
        result = false;
        printf("Exp. while sendReceive\n");
    }


    return result;
}
