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

#include <stdio.h>
#include <string>

#include "comms.h"
#include "commsmessage.h"
#include "commslistener.h"
#include "commsclientendpoint.h"
#include "commsserverendpoint.h"
#include "rtcmessages.h"
#include "javauid.h"
#include "javacommonutils.h"
#include "monitor.h"


using namespace java::util;
using namespace java::comms;
using namespace java::captain;

class myCommsListener : public CommsListener
{
public:
    myCommsListener(int aInds, Monitor* aMonitor):mInds(aInds), mMonitor(aMonitor)
    {
        if (!mInds)
            mMonitor->notify();
    }
    void processMessage(java::comms::CommsMessage& message)
    {
        switch (message.getMessageId())
        {
        case RTC_MSG_ID_APPLICATION_RUNNING_IND:
            puts("RUNNIND_IND received");
            if (mInds == 1)
                mMonitor->notify();
            break;
        case RTC_MSG_ID_APPLICATION_TERMINATED_IND:
            puts("TERMINATED_IND received");
            mMonitor->notify();
            break;
        }
    }

    int         mInds;
    Monitor*    mMonitor;
};

// Usage app_launcher "uid"
int main(int ac, char** av)
{
    if (ac < 2)
    {
        printf("Usage: %s \"uid_string\" [inds] [\"runtime\"]\n", av[0]);
        puts("inds:");
        puts("1 = RUNNING_IND requested");  // RTC_LAUNCH_OPTIONS_RUNNING_IND_REQ
        puts("2 = TERMINATE_IND requested");// RTC_LAUNCH_OPTIONS_TERMINATE_IND_REQ
        puts("3 = both requested");         // both above ORed
        puts("runtime defaults to \"midp\"");
        return 0;
    }
    std::string indications("0");
    std::string runtime("midp");

    if (ac >= 3)
    {
        indications = av[2];
    }
    if (ac >= 4)
    {
        runtime = av[3];
    }

    Monitor* monitor = Monitor::createMonitor();
    int inds = java::util::JavaCommonUtils::stringToInt(indications);
    CommsClientEndpoint comms;
    myCommsListener myListener(inds, monitor);
    comms.registerDefaultListener(&myListener);
    if (!comms.connect(IPC_ADDRESS_JAVA_CAPTAIN_C))
    {
        std::wstring uid_string = java::util::JavaCommonUtils::utf8ToWstring(av[1]);

        CommsMessage message;
        message.setModuleId(PLUGIN_ID_RTC_C);
        setLaunchApplicationReqParams(message, Uid(uid_string), RTC_LAUNCH_TYPE_NORMAL_C, inds, runtime);

        int status = comms.send(message);
        printf("send() returns %d\n", status);

        monitor->wait();
//        puts("Press any key to exit");
//        if ( getchar() != 0 )
//            ;
        comms.disconnect();
    }
    else
    {
        puts("connect() failed");
    }

    return 0;
}

