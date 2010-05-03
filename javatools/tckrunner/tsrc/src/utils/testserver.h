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

#ifndef TESTSERVER_H
#define TESTSERVER_H
#include <queue>

#include "commslistener.h"
#include "commsserverendpoint.h"
#include "commsclientendpoint.h"
#include "commsmessage.h"

using namespace java::comms;
using namespace std;

const int MSG_ID_GET_NEXT_OPERATION = 500;
const int MSG_ID_OPERATION          = 501;
const int MSG_ID_OPERATION_RESULT   = 502;

const int OPERATION_INSTALL   = 0;
const int OPERATION_UNINSTALL = 1;
const int OPERATION_EXIT      = 2;

class TestServer : public CommsListener
{
public:
    TestServer();
    virtual ~TestServer();

    // CommsListener methods
    virtual void processMessage(CommsMessage& aMessage);

    int start();
    int stop();

    CommsEndpoint& getJavaCaptain()
    {
        return mJavaCaptain;
    }
    CommsEndpoint& getJavaInstaller()
    {
        return mJavaInstaller;
    }

    queue<string> mLaunchReqs;

private:
    int startTckRunner();
    int stopTckRunner();
    void stopJavaCaptain();

    CommsServerEndpoint mJavaCaptain;
    CommsClientEndpoint mJavaInstaller;
};

#endif // TESTSERVER_H
