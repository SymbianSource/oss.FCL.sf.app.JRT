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
* Description:  Main
*
*/


#include <stdio.h>
#include <string>
#include <unistd.h>

#include "javacommonutils.h"
#include "comms.h"
#include "commsclientendpoint.h"
#include "commslistener.h"
#include "monitor.h"

#include "rtcmessages.h"

#include "testuids.h"

using namespace java::util;
using namespace java::comms;
using namespace java::captain;

CommsClientEndpoint comms;
Monitor*            monitor;
Uid                 uid;

class rtCommsListener : public CommsListener
{
public:
    void processMessage(CommsMessage& message)
    {
        switch (message.getMessageId())
        {
        case RTC_MSG_ID_TERMINATE_APPLICATION_REQ:
            printf("TERMINATE_REQ received, triggering monitor\n");
            monitor->notify();
            break;
        }
    }
};

static void sendRunningInd();
static void sendTerminatedInd();
static void sendGetRuntimeAttributes();

void runtime(const int& uSecDelayBeforeRunningInd,
             const int& numOfRunningInds,
             const bool& getRuntimeAttributes,
             const bool& immediateExit,
             const bool& obeyTerminateReqs,
             const int& numOfTerminateInds,
             const bool& stayRunning)
{
    if (uSecDelayBeforeRunningInd > 0)
    {
        usleep(uSecDelayBeforeRunningInd);
    }

    for (int runInds = 0 ; runInds < numOfRunningInds ; runInds++)
    {
        sendRunningInd();
    }

    if (getRuntimeAttributes)
    {
        sendGetRuntimeAttributes();
    }

    if (!immediateExit)
    {
        do
        {
            monitor->wait();
        }
        while (!obeyTerminateReqs);
    }

    for (int termInds = 0; termInds < numOfTerminateInds ; termInds++)
    {
        sendTerminatedInd();
    }

    if (stayRunning)
    {
        while (true)
        {
            monitor->wait();
        }
    }
}

int main(int ac, char** av)
{
    monitor = Monitor::createMonitor();

    rtCommsListener listener;
    comms.registerDefaultListener(&listener);
    comms.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);

    printf("we have liftoff\n");

    for (int i = 0 ; i<ac ; ++i)
    {
        printf("%d:%s ", i, av[i]);
    }
    printf("\n");

    uid = Uid(java::util::JavaCommonUtils::utf8ToWstring(av[3]));
    printf("%s used for launching\n", av[3]);

    bool getRuntimeAttributes = false;
    if (ac > 4 && (std::string(av[4]) == "-debug"))
    {
        printf("getRuntimeAttributes -> true\n");
        getRuntimeAttributes = true;
    }

// startup sequence : running_ind 1x
// runtime behaviour: stays running
// exit sequence    : sends terminate_ind 1x -> exits
//const Uid uid_normal1(L"normal1");
    if (uid == uid_normal1 ||
            uid == uid_push1)
    {
        runtime(500000, // uSecDelayBeforeRunningInd
                1,      // numOfRunningInds
                getRuntimeAttributes,
                false,  // immediateExit
                true,   // obeyTerminateReqs
                1,      // numOfTerminateInds
                false); // stayRunning
    }

// startup sequence : running_ind 2x (no delay in between)
// runtime behaviour: stays running
// exit sequence    : sends terminate_ind 1x -> exits
//const Uid uid_normal2(L"normal2");
    else if (uid == uid_normal2)
    {
        runtime(500000, // uSecDelayBeforeRunningInd
                2,      // numOfRunningInds
                getRuntimeAttributes,
                false,  // immediateExit
                true,   // obeyTerminateReqs
                1,      // numOfTerminateInds
                false); // stayRunning
    }

// startup sequence : running_ind 2x (no delay)
// runtime behaviour: stays running
// exit sequence    : sends terminate_ind 2x (no delay) -> exits
//const Uid uid_normal3(L"normal3");
    else if (uid == uid_normal3)
    {
        runtime(500000, //uSecDelayBeforeRunningInd
                2,      // numOfRunningInds
                getRuntimeAttributes,
                false,  // immediateExit
                true,   // obeyTerminateReqs
                2,      // numOfTerminateInds
                false); // stayRunning
    }
// startup sequence : running_ind 1x no delay
// runtime behaviour: stays running
// exit sequence    : sends terminate_ind 1x -> exits
//const Uid uid_normal1(L"normal4");
    else if (uid == uid_normal4)
    {
        runtime(0,      // uSecDelayBeforeRunningInd
                1,      // numOfRunningInds
                getRuntimeAttributes,
                false,  // immediateExit
                true,   // obeyTerminateReqs
                1,      // numOfTerminateInds
                false); // stayRunning
    }

// startup sequence : running_ind 1x
// runtime behaviour: stays running
// exit sequence    : sends nothing -> stays running
    else if (uid == uid_wont_terminate1)
    {
        runtime(500000, // uSecDelayBeforeRunningInd
                1,      // numOfRunningInds
                getRuntimeAttributes,
                false,  // immediateExit
                false,  // obeyTerminateReqs
                0,      // numOfTerminateInds
                true);  // stayRunning
    }

// startup sequence : running_ind 1x
// runtime behaviour: stays running
// exit sequence    : sends terminate_ind but stays running
    else if (uid == uid_wont_terminate2)
    {
        runtime(500000, // uSecDelayBeforeRunningInd
                1,      // numOfRunningInds
                getRuntimeAttributes,
                false,  // immediateExit
                false,  // obeyTerminateReqs
                1,      // numOfTerminateInds
                true);  // stayRunning
    }

// startup sequence : running_ind 0x
// runtime behaviour: exit immediately
// exit sequence    : sends terminate_ind 1x -> exits
//const Uid uid_premature_exit1(L"premature_exit1");
    else if (uid == uid_premature_exit1)
    {
        runtime(500000, //uSecDelayBeforeRunningInd
                0,      // numOfRunningInds
                getRuntimeAttributes,
                true,  // immediateExit
                true,  // obeyTerminateReqs
                1,      // numOfTerminateInds
                false);  // stayRunning
    }

// startup sequence : running_ind 1x
// runtime behaviour: exit immediately
// exit sequence    : sends terminate_ind 1x -> exits
//const Uid uid_premature_exit2(L"premature_exit2");
    else if (uid == uid_premature_exit2)
    {
        runtime(500000, //uSecDelayBeforeRunningInd
                1,      // numOfRunningInds
                getRuntimeAttributes,
                true,  // immediateExit
                true,  // obeyTerminateReqs
                1,      // numOfTerminateInds
                false);  // stayRunning
    }

// startup sequence : running_ind 1x
// runtime behaviour: exit immediately
// exit sequence    : sends nothing -> exits
//const Uid uid_premature_exit3(L"premature_exit3");
    else if (uid == uid_premature_exit3)
    {
        runtime(500000, //uSecDelayBeforeRunningInd
                1,      // numOfRunningInds
                getRuntimeAttributes,
                true,  // immediateExit
                true,  // obeyTerminateReqs
                0,      // numOfTerminateInds
                false);  // stayRunning
    }

    comms.disconnect();

    delete monitor;

    puts("Exiting");

    return 0;
}

void sendRunningInd()
{
    CommsMessage msg;
    setApplicationRunningIndParams(msg, uid, 0);
    comms.send(msg);
}

void sendTerminatedInd()
{
    CommsMessage msg;
    setApplicationTerminatedIndParams(msg, uid, 0);
    comms.send(msg);
}

void sendGetRuntimeAttributes()
{
    CommsMessage msg;
    setGetExtraArguments(msg);
    comms.send(msg);
}
