/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This application is executed by the native installer
*               after a new version of Jrt has been installed from
*               .sisx package on top of older Jrt version (e.g. when
*               updating Jrt 2.1 to Jrt 2.2 via IAD).
*               This application restarts Java Captain so that new
*               version of the Jrt dlls is taken into use.
*               All running MIDlets exit when Java Captain closes.
*/


#include <e32base.h>
#include <e32std.h>
#include <unistd.h>
#include <w32std.h>

#include "comms.h"
#include "commsclientendpoint.h"
#include "commsmessage.h"
#include "coremessages.h"
#include "logger.h"
#include "rtcmessages.h"

using namespace java::comms;
using namespace java::captain;


void stopJavaCaptain()
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
        WLOG(EJavaConverters, "javafinishjrtinstall: Waiting JavaCaptain to exit...");
        rc = comms.send(message);
    }
    comms.disconnect();
}


void startJavaCaptain()
{
    _LIT(KJavaCaptainExe, "javacaptain.exe");
    _LIT(KJavaCaptainArg, "");
    RProcess proc;
    int err = proc.Create(KJavaCaptainExe, KJavaCaptainArg);
    if (err == KErrNone)
    {
        proc.Resume();
        LOG(EJavaConverters, EInfo, 
            "javafinishjrtinstall: startJavaCaptain javacaptain.exe was started ok");
    }
    else
    {
        ELOG1(EJavaConverters, 
            "javafinishjrtinstall: startJavaCaptain start javacaptain.exe failed: %d", err);
    }
    proc.Close();
}


/**
 * Create cleanup stack and run the cleaner code inside TRAP harness
 * to log unexpected leaves.
 */
TInt E32Main()
{
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    stopJavaCaptain();
    startJavaCaptain();
    delete cleanupStack;
    return KErrNone;
}
