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
* Description: InstallOperation provides a way to launch and communicate
*              with Java Installer process

*
*/


#include "installoperation.h"

#include "logger.h"
#include "comms.h"
#include "commsmessage.h"
#include "javaprocessconstants.h"
#include "javasymbianoslayer.h"

using namespace java::debug;
using namespace java::util;
using namespace java::comms;

InstallOperation::InstallOperation()
{
    mInstaller.registerDefaultListener(this);
}

InstallOperation::~InstallOperation()
{
}

int InstallOperation::runJavaInstaller()
{
    JELOG2(EDebugApi);

    int rc = mInstaller.start(IPC_ADDRESS_JAVA_DEBUGAPI_C);
    if (!rc)
    {
        int waitCount = 5;
        do
        {
            // If install operation is done immediately after previous one then
            // it's possible that previous Java Installer process instance has not
            // exited yet.
            rc = startJavaInstaller();
            if (rc == KErrAlreadyExists)
            {
                LOG(EDebugApi, EInfo, "waiting java installer to exit");
                User::After(500000); // 0.5s
            }
            else
            {
                waitCount = 0;
            }
        }
        while (--waitCount > 0);
    }
    mInstaller.stop();

    if (rc)
    {
        ELOG1(EDebugApi, "runJavaInstaller() failed: rc=%d", rc);
    }
    return rc;
}

int InstallOperation::startJavaInstaller()
{
    JELOG2(EDebugApi);

    std::string args = java::runtime::JAVA_INSTALLER_STARTER_DLL;
    args += " poll -address=debugapi";

    std::auto_ptr<HBufC> cmdLine(stringToDes(args.c_str()));
    std::auto_ptr<HBufC> exe(stringToDes(java::runtime::JAVA_PROCESS));

    RProcess process;
    int rc = process.Create(exe->Des(), cmdLine->Des());

    if (KErrNone == rc)
    {
        // wait for JavaInstaller exit
        TRequestStatus status;
        process.Logon(status);
        process.Resume();
        User::WaitForRequest(status);
        rc = status.Int();
        process.Close();
    }

    LOG1(EDebugApi, EInfo,"javainstaller exit code: %d", rc);
    return rc;
}

/**
 * Communication with Java Installer is done asynchronously.
 * Following messages are used:
 *
 * Message Name        Id      Contents
 *
 * Get Next Operation  500     None
 *
 * Operation           501     int operation   (install = 0, uninstall = 1, exit = 2)
 *                             String url      (present if operation = install)
 *                             String uid      (present if operation = uninstall)
 *
 * Operation Result    502     int operation   (install = 0, uninstall = 1, exit = 2)
 *                             int result      (ok = 0, error codes are negative)
 *                             int uidCount    (present if operation = install, value = uids.length)
 *                             String[] uids   (present if operation = install)
 *                             String suiteUid (present if operation = install)
 */
void InstallOperation::processMessage(CommsMessage& aMessage)
{
    JELOG2(EDebugApi);

    switch (aMessage.getMessageId())
    {
    case GET_NEXT_OPERATION_MESSAGE_ID:
    {
        LOG(EDebugApi, EInfo, "GET_NEXT_OPERATION received");
        CommsMessage reply = getNextOperation();
        reply.replyTo(aMessage);
        reply.setMessageId(OPERATION_MESSAGE_ID);
        sendToJavaInstaller(reply);
    }
    break;

    case OPERATION_RESULT_MESSAGE_ID:
        LOG(EDebugApi, EInfo,"OPERATION_RESULT received");
        handleOperationResult(aMessage);
        break;

    default:
        WLOG1(EDebugApi, "Unknown message received: msgId = %d", aMessage.getMessageId());
        break;
    }
}

CommsMessage InstallOperation::getNextOperation()
{
    CommsMessage msg;
    msg << EXIT_OPERATION;
    LOG(EDebugApi, EInfo, "next operation is EXIT");
    return msg;
}

void InstallOperation::sendToJavaInstaller(CommsMessage& aMessage)
{
    int rc = mInstaller.send(aMessage);
    if (rc)
    {
        WLOG1(EDebugApi, "sendToJavaInstaller() failed: rc = %d", rc);
    }
}

void InstallOperation::handleOperationResult(CommsMessage& /*aMessage*/)
{
}
