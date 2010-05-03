/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Comms server,
*    part of Java platform 2.0 javarestoreconverter process
*
*/


#include <iostream>
#include <unistd.h>

#include "comms.h"
#include "javasymbianoslayer.h"
#include "logger.h"
#include "restoreserver.h"

using namespace java::comms;
using namespace std;

RestoreServer::RestoreServer()
{
}

RestoreServer::~RestoreServer()
{
    iInstallFiles.clear();
    iUninstallUids.clear();
}

void RestoreServer::setOperations(
    RPointerArray<HBufC> &aInstallFiles,
    std::vector<std::wstring> &aUninstallUids)
{
    // file index will contain the number of valid install file urls
    iFileIndex = 0;
    // clear the old install file paths if any
    iInstallFiles.clear();

    int nPointers = aInstallFiles.Count();
    iInstallFiles.reserve(nPointers);

    for (int i = 0; i < nPointers; i++)
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

    LOG1(EJavaConverters, EInfo, "RestoreServer: number of install files = %d", iFileIndex);
    iFilesTotal = iFileIndex;
    iFileIndex = 0;

    iUidIndex = 0;
    iUninstallUids = aUninstallUids;
    iUidsTotal = iUninstallUids.size();
    LOG1(EJavaConverters, EInfo, "RestoreServer: uninstall %d suites", iUidsTotal);
}

int RestoreServer::start()
{
    LOG(EJavaConverters, EInfo, "RestoreServer:start called");

    iRunning = 1;
    iComms.registerDefaultListener(this);
    // Using the same Comms endpoint as javaappconverter on purpose
    return iComms.start(IPC_ADDRESS_JAVA_APPCONVERTER_C);
}

int RestoreServer::stop()
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
void RestoreServer::processMessage(CommsMessage& aMessage)
{
    LOG1(EJavaConverters, EInfo, "RestoreServer::processMessage, sender    = %d", aMessage.getSender());
    LOG1(EJavaConverters, EInfo, "RestoreServer::processMessage, messageId = %d", aMessage.getMessageId());

    switch (aMessage.getMessageId())
    {
    case GET_NEXT_OPERATION_MESSAGE_ID:
    {
        // Java Installer asks for the next install operation.
        // Reply with 'Operation' message.

        CommsMessage reply;
        reply.replyTo(aMessage);

        reply.setMessageId(OPERATION_MESSAGE_ID);

        if (iUidIndex < iUidsTotal)
        {
            // ask Java Installer to uninstall next
            reply << UNINSTALL_OPERATION << iUninstallUids[iUidIndex];
            iUidIndex++;
        }
        else if (iFileIndex >= iFilesTotal)
        {
            // all install files have been handled, ask Java Installer to exit
            reply << EXIT_OPERATION;

            // Java Installer sends reply to 'exit' message and then
            // it exits which wakes up javaappconverter main thread
        }
        else
        {
            // ask Java Installer to install next
            reply << INSTALL_OPERATION << iInstallFiles[iFileIndex] << iFileIndex << iFilesTotal;
            iFileIndex++;
        }

        int err = iComms.send(reply);
        if (err != 0)
        {
            ELOG1(EJavaConverters,
                  "RestoreServer: Sending reply to Java Installer failed, err %d", err);
            // Propably Java Installer has crashed and the execution of
            // CRestoreConvertMIDlet is already proceeding. This server will
            // be stopped when the execution reaches EExit state.
        }
    }
    break;

    case OPERATION_RESULT_MESSAGE_ID:
    {
        // Log the result of the operation. Javarestoreconverter does not need
        // the Uids of the installed Java Applications so they are not parsed.
        int operation;
        int result;

        aMessage >> operation >> result;

        LOG2(EJavaConverters, EInfo,
             "RestoreServer: operation = %d, result = %d", operation, result);

        if (result < 0)
        {
            // Conversion failed.
            ELOG2(EJavaConverters, "RestoreServer: Converting (installing) "
                  "application number %d failed with code %d",
                  iFileIndex, result);
        }
    }
    break;

    default:
    {
        // Unknown message. Ignore it.
        WLOG1(EJavaConverters,
              "RestoreServer: Unknown message received. Msg Id = %d",
              aMessage.getMessageId());
    }
    break;
    }
}
