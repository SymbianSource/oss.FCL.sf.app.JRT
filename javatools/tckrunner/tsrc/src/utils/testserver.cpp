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

#include <string>
#include <unistd.h>
#include <errno.h>

#ifdef __SYMBIAN32__
#include <spawn.h>
#else
#include <stdio.h>
#endif //__SYMBIAN32__


#include "logger.h"
#include "comms.h"
#include "rtcmessages.h"
#include "coremessages.h"

#include "testserver.h"

using namespace java::comms;
using namespace java::captain;


TestServer::TestServer()
{
}

TestServer::~TestServer()
{
}

int TestServer::start()
{
    mJavaCaptain.registerDefaultListener(this);
    mJavaInstaller.registerDefaultListener(this);
    stopJavaCaptain();
    int rc = mJavaCaptain.start(IPC_ADDRESS_JAVA_CAPTAIN_C);

    if (!rc)
    {
        startTckRunner();
        rc = mJavaInstaller.connect(IPC_ADDRESS_JAVA_TCK_RUNNER_C);

        while (rc != 0)
        {
            // tck runner is not up yet
            sleep(1);
            WLOG(ETckRunner, "waiting for tckrunner...");
            rc = mJavaInstaller.connect(IPC_ADDRESS_JAVA_TCK_RUNNER_C);
        }
    }
    else
    {
        ELOG1(ETckRunner, "Unable to create Java Captain endpoint %d. Is Java Captain running?", rc);
    }
    return rc;
}

int TestServer::stop()
{
    stopTckRunner();
    mJavaCaptain.unregisterDefaultListener(this);
    mJavaInstaller.unregisterDefaultListener(this);
    int rc1 = mJavaCaptain.stop();
    int rc2 = mJavaInstaller.disconnect();
    return rc1 + rc2;
}


void TestServer::processMessage(CommsMessage& aMessage)
{
    switch (aMessage.getMessageId())
    {
    case MSG_ID_OPERATION:
    {
        int oper;
        string urlOrUid;
        aMessage >> oper >> urlOrUid;

        CommsMessage msg;
        msg.replyTo(aMessage);
        msg.setMessageId(MSG_ID_OPERATION_RESULT);
        int status = 0;
        msg << oper << status << 1 << urlOrUid;
        mJavaInstaller.send(msg);
    }
    break;

    case RTC_MSG_ID_LAUNCH_APPLICATION_REQ:
    {
        string rtc, uid;
        int option, type;
        aMessage >> uid >> type >> option >> rtc;

        CommsMessage msg;
        msg.replyTo(aMessage);
        msg.setMessageId(RTC_MSG_ID_APPLICATION_TERMINATED_IND);
        int status = 0;
        if (uid.compare("fail") == 0)
        {
            status = -1;
        }
        msg << uid << status;
        mJavaCaptain.send(msg);

        mLaunchReqs.push(uid);
    }
    break;

    default:
        LOG1(ETckRunner, EInfo, "Unknown message= %d", aMessage.getMessageId());
        break;
    }
}

int TestServer::startTckRunner()
{
    int rc = 0;
    int pid = 0;

    char* av[4];
    int index = 0;
    av[index++] = "tckrunner";
    av[index++] = "unittest";
    av[index++] = "unittest:\\\\tckrunner_tester";
    av[index] = NULL;

#ifdef __SYMBIAN32__
    rc = posix_spawn(&pid, "tckrunner", NULL, NULL, av, NULL);
#else
    if (!(pid = fork()))
    {
        rc = execvp("tckrunner", av);
        if (rc == -1)
        {
            rc = errno;
        }
    }
#endif // __SYMBIAN32__

    if (rc)
    {
        ELOG3(ETckRunner, "%s failed, %s - errno=%d", __PRETTY_FUNCTION__, strerror(rc), rc);
    }
    return rc;
}


int TestServer::stopTckRunner()
{
    CommsMessage msg;
    msg.setMessageId(MSG_ID_OPERATION_RESULT);
    int status = 0;
    msg << OPERATION_EXIT << status;
    int rc = mJavaInstaller.send(msg);
    while (rc == 0)
    {
        sleep(1);
        WLOG(ETckRunner, "waiting tckrunner to exit...");
        rc = mJavaInstaller.send(msg);
    }
    return rc;
}

void TestServer::stopJavaCaptain()
{
    CommsMessage message;
    message.setModuleId(PLUGIN_ID_JAVACAPTAIN_CORE_C);
    message.setMessageId(CORE_MSG_ID_STOP_JAVACAPTAIN);
    CommsClientEndpoint comms;
    comms.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
    int rc = comms.send(message);
    while (rc == 0)
    {
        sleep(1);
        WLOG(ETckRunner, "waiting JavaCaptain to exit...");
        rc = comms.send(message);
    }
    comms.disconnect();
}


