/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Comms server, part of Java platform 2.0 javappconverter process
*
*/


#include <iostream>
#include <unistd.h>

#include "comms.h"
#include "logger.h"
#include "javasymbianoslayer.h"
#include "convertserver.h"

using namespace java::comms;
using namespace std;

ConvertServer::ConvertServer()
{
}

ConvertServer::~ConvertServer()
{
    iInstallFiles.clear();
}

void ConvertServer::setInstallFiles(RPointerArray<HBufC> &aInstallFiles)
{
    JELOG2(EJavaConverters);

    // file index will contain the number of valid install file urls
    iFileIndex = 0;
    // clear the old install file paths if any
    iInstallFiles.clear();

    int nPointers = aInstallFiles.Count();
    iInstallFiles.reserve(nPointers);

    // index is decremented because iFileIndex will also be decremented when
    // installing -> total effect is that install files will be given to
    // Java Installer in the original order
    for (int i = nPointers - 1; i > -1; i--)
    {
        HBufC* pBuf = aInstallFiles[i];

        if (NULL == pBuf)
        {
            // skip this NULL pointer
            continue;
        }

        TPtr16 ptrName(pBuf->Des());
        // Convert each HBufC to std:wstring before adding to iInstallFiles
        iInstallFiles.push_back(std::wstring(desToWstring(ptrName)));
        iFileIndex++;
    }

    LOG1(EJavaConverters, EInfo, "ConvertServer: number of install files = %d", iFileIndex);
    iFilesTotal = iFileIndex;
}

int ConvertServer::start()
{
    LOG(EJavaConverters, EInfo,
        "ConvertServer:start called");

    iRunning = 1;
    iComms.registerDefaultListener(this);
    return iComms.start(IPC_ADDRESS_JAVA_APPCONVERTER_C);
}

int ConvertServer::stop()
{
    if (iRunning > 0)
    {
        iRunning = 0;
        iComms.unregisterDefaultListener(this);
        return iComms.stop();
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
 *                             int current     (1..total, present if operation = install
 *                                              and caller is appconverter)
 *                             int total       (total number of apps to convert, present
 *                                              if operation = install and caller is appconverter)
 *
 * Operation Result    502     int operation   (install = 0, uninstall = 1, exit = 2)
 *                             int result      (ok = 0, error codes are negative)
 *                             int uidCount    (present if operation = install, value = uids.length)
 *                             String[] uids   (present if operation = install)
 */
void ConvertServer::processMessage(CommsMessage& aMessage)
{
    LOG1(EJavaConverters, EInfo, "ConvertServer::processMessage, sender    = %d", aMessage.getSender());
    LOG1(EJavaConverters, EInfo, "ConvertServer::processMessage, messageId = %d", aMessage.getMessageId());
    LOG1(EJavaConverters, EInfo, "ConvertServer::processMessage, moduleId  = %d", aMessage.getModuleId());
    LOG1(EJavaConverters, EInfo, "ConvertServer::processMessage, messageRef= %d", aMessage.getMessageRef());

    switch (aMessage.getMessageId())
    {
    case GET_NEXT_OPERATION_MESSAGE_ID:
    {
        // Java Installer asks for the next install operation.
        // Reply with 'Operation' message.

        CommsMessage reply;
        reply.replyTo(aMessage);

        reply.setMessageId(OPERATION_MESSAGE_ID);

        iFileIndex--;
        if (iFileIndex <= -1)
        {
            // all install files have been handled, ask Java Installer to exit
            reply << EXIT_OPERATION;

            // Java Installer sends reply to 'exit' message and then
            // it exits which wakes up javaappconverter main thread
        }
        else
        {
            // ask Java Installer to install next
            int current = (iFilesTotal - iFileIndex);
            reply << INSTALL_OPERATION << iInstallFiles[iFileIndex] << current << iFilesTotal;
        }

        int err = iComms.send(reply);
        if (err != 0)
        {
            ELOG1(EJavaConverters,
                  "ConvertServer: Sending reply to Java Installer failed, err %d", err);
            // Propably Java Installer has crashed and the execution of
            // CSilentMIDletConvert is already proceeding. This server will
            // be stopped when the execution reaches EExit state.
        }
    }
    break;

    case OPERATION_RESULT_MESSAGE_ID:
    {
        // Log the result of the operation. Javaappconverter does not need
        // the Uids of the installed Java Applications so they are not
        // parsed.
        int operation;
        int result;

        aMessage >> operation >> result;

        LOG2(EJavaConverters, EInfo,
             "ConvertServer: operation = %d, result = %d", operation, result);

        if (result < 0)
        {
            // Conversion failed.
            ELOG2(EJavaConverters, "ConvertServer: Converting (installing) "
                  "application number %d failed with code %d",
                  (iFilesTotal - iFileIndex), result);
        }
    }
    break;

    default:
    {
        // Unknown message. Ignore it.
        WLOG1(EJavaConverters,
              "ConvertServer: Unknown message received. Msg Id = %d",
              aMessage.getMessageId());
    }
    break;
    }
}
