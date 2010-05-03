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
* Description:  Comms server, part of OMJ S60 preinstaller process
 *
*/


#include <iostream>
#include <unistd.h>

#include "comms.h"
#include "logger.h"
#include "javasymbianoslayer.h"
#include "preinstallcommsserver.h"

using namespace java::comms;
using namespace std;

PreinstallCommsServer::PreinstallCommsServer() : mRunning(0)
{
}

PreinstallCommsServer::~PreinstallCommsServer()
{
    mJadFiles.clear();
}

void PreinstallCommsServer::setJadFiles(RPointerArray<HBufC> &aJadFiles)
{
    JELOG2(EJavaPreinstaller);

    // jad index will contain the number of valid jad file urls
    mJadIndex = 0;
    // clear the old jad file paths if any
    mJadFiles.clear();

    int nPointers = aJadFiles.Count();
    mJadFiles.reserve(nPointers);

    // index is decremented because mJadIndex will also be decremented when
    // installing -> total effect is that jad files will be given to
    // Java Installer in the original order
    for (int i = nPointers - 1; i > -1; i--)
    {
        HBufC* pBuf = aJadFiles[i];

        if (NULL == pBuf)
        {
            // skip this NULL pointer
            continue;
        }

        TPtr16 ptrName(pBuf->Des());
        // Convert each HBufC to std:wstring before adding to mJadFiles
        mJadFiles.push_back(std::wstring(desToWstring(ptrName)));
        mJadIndex++;
    }

    LOG1(EJavaPreinstaller, EInfo, "PreinstallCommsServer: number of Jad files = %d", mJadIndex);
}

int PreinstallCommsServer::start()
{
    LOG(EJavaPreinstaller, EInfo,
        "PreinstallCommsServer:start called");

    if (mRunning == 0)
    {
        mRunning = 1;
        mComms.registerDefaultListener(this);
        return mComms.start(IPC_ADDRESS_JAVA_PREINSTALLER_C);
    }
    else
    {
        WLOG(EJavaPreinstaller,
             "PreinstallCommsServer:start called but server is already running");
        return 0;
    }
}

int PreinstallCommsServer::stop()
{
    if (mRunning > 0)
    {
        mRunning = 0;
        mComms.unregisterDefaultListener(this);
        return mComms.stop();
    }
    else
    {
        return 0;
    }
}

/**
 * Communicates with Java Installer. The following messages are used.
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
 */
void PreinstallCommsServer::processMessage(CommsMessage& aMessage)
{
    LOG1(EJavaPreinstaller, EInfo, "processMessage, sender    = %d", aMessage.getSender());
    LOG1(EJavaPreinstaller, EInfo, "processMessage, messageId = %d", aMessage.getMessageId());
    LOG1(EJavaPreinstaller, EInfo, "processMessage, messageRef= %d", aMessage.getMessageRef());

    switch (aMessage.getMessageId())
    {
    case GET_NEXT_OPERATION_MESSAGE_ID:
    {
        // Java Installer asks for the next install operation.
        // Reply with 'Operation' message.

        CommsMessage reply;
        reply.replyTo(aMessage);

        reply.setMessageId(OPERATION_MESSAGE_ID);

        mJadIndex--;
        if (mJadIndex <= -1)
        {
            // all Jad files have been handled, ask Java Installer to exit
            reply << EXIT_OPERATION;

            // Java Installer sends reply to 'exit' message and then
            // it exits which causes the completion of the CSilentMIDletInstall active object
        }
        else
        {
            // ask Java Installer to install next
            reply << INSTALL_OPERATION << mJadFiles[mJadIndex];
        }

        // Ignore possible error code returned by send() because Comms
        // logs the error situations
        mComms.send(reply);
    }
    break;

    case OPERATION_RESULT_MESSAGE_ID:
    {
        // Log the result of the operation. Preinstaller does not need
        // the Uids of the installed Java Applications so they are not
        // parsed.
        int operation;
        int result;

        aMessage >> operation >> result;

        LOG2(EJavaPreinstaller, EInfo,
             "PreinstallCommsServer: operation = %d, result = %d", operation, result);
    }
    break;

    default:
    {
        // Unknown message. Ignore it.
        WLOG1(EJavaPreinstaller,
              "PreinstallCommsServer: Unknown message received. Msg Id = %d",
              aMessage.getMessageId());
    }
    break;
    }
}
