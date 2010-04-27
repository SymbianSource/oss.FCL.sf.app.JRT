/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  StorageServer
*
*/


#include <sqlite3.h>

#include "commsmessage.h"
#include "javacommonutils.h"
#include "javastorageexception.h"
#include "javastoragemessage.h"
#include "logger.h"
#include "monitor.h"
#include "storagedbhandler.h"
#include "storageserver.h"

using namespace java::comms;
using namespace java::storage;
using namespace java::util;
using namespace std;

StorageServer::StorageServer(java::util::Monitor* aMonitor)
        :iDbHandler(0), iMonitor(aMonitor)
{
}

StorageServer::~StorageServer()
{
    JELOG2(EJavaStorage);
    iComms.stop();
    if (iDbHandler != 0)
    {
        delete iDbHandler;
        iDbHandler = 0;
    }

}

void StorageServer::start()
{
    JELOG2(EJavaStorage);
    iComms.registerDefaultListener(this);
    if (0 != iComms.start(IPC_ADDRESS_JAVA_STORAGE_C) && iMonitor)
    {
        iMonitor->notify();
        return;
    }

    iDbHandler = new StorageDBHandler();
}

void StorageServer::stop()
{
    JELOG2(EJavaStorage);
    iMonitor->notify();
}

void StorageServer::processMessage(CommsMessage& aMessage)
{
    JELOG2(EJavaStorage);

    wstring headers;
    wstring sqlStatement;
    bool connectionExists = false;

    aMessage>>headers;
    aMessage>>sqlStatement;

    CommsMessage returnMessage;

    if (0 == headers.size())
    {
        returnMessage << EInvalidDataStructure;
        returnMessage << L"Invalid message structure";
        respond(aMessage, returnMessage);
        return;
    }

    wstring sessionID;
    int messageID;

    try
    {
        int messageIDSize;

        messageIDSize = JavaCommonUtils::wstringToInt(headers.substr(0, 1));
        messageID = JavaCommonUtils::wstringToInt(
                        headers.substr(1, messageIDSize));
        sessionID = headers.substr(messageIDSize + 1, headers.size());
    }
    catch (...)
    {
        returnMessage << EInvalidDataStructure;
        returnMessage << L"Invalid message identifier";
        respond(aMessage, returnMessage);
        return;
    }

    // Select appropriate database connection if one already exists
    if (iDbHandler->selectConnection(sessionID))
    {
        connectionExists = true;

        // Return the same session ID if client sends a duplicate
        // open message for an already existing connection.
        if (JavaStorageMessage::EOpen == messageID)
        {
            returnMessage<<0;
            returnMessage<<iDbHandler->returnSessionID();
            respond(aMessage, returnMessage);
            return;
        }
    }
    else if (JavaStorageMessage::EOpen != messageID
             && JavaStorageMessage::EShutdown != messageID)
    {
        returnMessage << EInvalidConnection;
        returnMessage << L"Invalid sessionID";
        respond(aMessage, returnMessage);
        return;
    }

    int returnStatus = 1;
    int dbReturnCode = SQLITE_OK;
    wstring resultSet = L"";

    switch (messageID)
    {
    case JavaStorageMessage::ESearch:
    case JavaStorageMessage::ERead:
    case JavaStorageMessage::EUpdate:
    case JavaStorageMessage::EWrite:
    case JavaStorageMessage::ERemove:
    case JavaStorageMessage::ECreateTable:
    case JavaStorageMessage::EAppendTable:
    {
        if (0 < sqlStatement.size())
        {
            returnStatus = iDbHandler->prepare(sqlStatement);
            if (returnStatus == 0)
            {
                resultSet = iDbHandler->executeStmt();
                dbReturnCode = iDbHandler->getErrorCode();
                if (dbReturnCode != SQLITE_OK && dbReturnCode != SQLITE_DONE)
                {
                    returnStatus = EGeneralFailure;
                    resultSet = JavaCommonUtils::utf8ToWstring(
                                    iDbHandler->getErrorMessage());
                }
            }
            else
            {
                returnStatus = EGeneralFailure;
                resultSet = JavaCommonUtils::utf8ToWstring(
                                iDbHandler->getErrorMessage());
            }
        }
        else
        {
            returnMessage << EInvalidDataStructure;
            returnMessage << L"Invalid message structure";
        }
        break;
    }
    case JavaStorageMessage::EOpen:
    {
        returnStatus = iDbHandler->open(sqlStatement);
        // Pass the session ID
        resultSet = iDbHandler->returnSessionID();
        break;
    }
    case JavaStorageMessage::EClose:
    {
        returnStatus = iDbHandler->close();
        break;
    }
    case JavaStorageMessage::EStartTransaction:
    {
        returnStatus = iDbHandler->startTransaction();
        break;
    }
    case JavaStorageMessage::ECommit:
    {
        returnStatus = iDbHandler->commit();
        break;
    }
    case JavaStorageMessage::ERollback:
    {
        returnStatus = iDbHandler->rollback();
        break;
    }
    case JavaStorageMessage::EShutdown:
    {
        printf("StorageServer, Shutdown message received. \n");
        stop();
        break;
    }
    default:
        ELOG1(EJavaStorage, "Unknown messageID: %d", messageID);
        returnStatus = EInvalidDataStructure;
        resultSet = L"Invalid message identifier";
    }

    returnMessage << returnStatus;
    returnMessage << resultSet;

    LOG1(EJavaStorage, EInfo, "ResultStatus: %d", returnStatus);
    LOG1WSTR(EJavaStorage, EInfo, "ResultSet: %s", resultSet);

    respond(aMessage, returnMessage);
}

void StorageServer::respond(const CommsMessage& aReceivedMessage,
                            CommsMessage& aResponseMessage)
{
    aResponseMessage.setReceiver(aReceivedMessage.getSender());
    aResponseMessage.setModuleId(aReceivedMessage.getModuleId());
    aResponseMessage.setMessageRef(aReceivedMessage.getMessageRef());

    int retValue = iComms.send(aResponseMessage);

    if (0 != retValue)
    {
        ELOG1(EJavaStorage, "Failed to respond via COMMS: %d",
              retValue);
    }
}
