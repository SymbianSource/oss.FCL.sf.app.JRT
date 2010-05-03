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
* Description:  MessageDispatcher
*
*/


#include "commsmessage.h"
#include "javacommonutils.h"
#include "javadataaccess.h"
#include "javastorageexception.h"
#include "logger.h"
#include "messagedispatcher.h"
#include "statementutils.h"

using namespace java::comms;
using namespace java::storage;
using namespace java::util;
using namespace std;

MessageDispatcher::MessageDispatcher()
{
    JELOG2(EJavaStorage);
    mDataAccess.reset(JavaDataAccess::createInstance());
    mUtils.reset(new StatementUtils());
}

MessageDispatcher::~MessageDispatcher()
{
}

std::string MessageDispatcher::createAndSendMessage(
    JavaStorageMessage::MessageIdentifier aMsgId,
    const string& aHeaders,
    const string& aDatabaseName)
{
    JELOG2(EJavaStorage);

    string sessionID = "";
    CommsMessage receivedMessage;
    int statusCode = -1;

    string headers = aHeaders;
    createHeader(aMsgId, headers);

    switch (aMsgId)
    {
    case JavaStorageMessage::EOpen:
    {
        mDataAccess->open(headers, aDatabaseName, receivedMessage);

        receivedMessage>>statusCode;

        if (statusCode >= 0)
        {
            receivedMessage>>sessionID;
            LOG1(EJavaStorage, EInfo, "Received sessionID: %s",
                 sessionID.c_str());
        }
        break;
    }
    case JavaStorageMessage::EClose:
    {
        mDataAccess->close(headers, receivedMessage);
        receivedMessage>>statusCode;
        break;
    }
    case JavaStorageMessage::EStartTransaction:
    {
        mDataAccess->execute(headers, L"BEGIN;", receivedMessage);
        receivedMessage>>statusCode;
        break;
    }
    case JavaStorageMessage::ECommit:
    {
        mDataAccess->execute(headers, L"COMMIT;", receivedMessage);
        receivedMessage>>statusCode;
        break;
    }
    case JavaStorageMessage::ERollback:
    {
        mDataAccess->execute(headers, L"ROLLBACK;", receivedMessage);
        receivedMessage>>statusCode;
        break;
    }
    default:
    {
        ELOG1(EJavaStorage, "Unknown command: %d", aMsgId);
        throw JavaStorageException(aMsgId, "Unknown message",
                                   __FILE__, __FUNCTION__, __LINE__);
    }
    }

    if (statusCode < 0)
    {
        string errorMessage = "";
        readErrorMessage(receivedMessage, errorMessage);
        throw JavaStorageException(statusCode, errorMessage.c_str(),
                                   __FILE__, __FUNCTION__, __LINE__);
    }

    return sessionID;
}

int MessageDispatcher::createAndSendMessage(
    JavaStorageMessage::MessageIdentifier aMsgId,
    const string& aHeaders,
    const string& identifier,
    const JavaStorageApplicationEntry_t& aEntry)
{
    JELOG2(EJavaStorage);

    CommsMessage receivedMessage;
    int statusCode = -1;
    int retVal = -1;

    string headers = aHeaders;
    createHeader(aMsgId, headers);

    wstring insertion(L", ");

    switch (aMsgId)
    {
    case JavaStorageMessage::EWrite:
    {
        wstring sqlStatement = L"";
        mUtils->createWriteStatement(
            aEntry, identifier, insertion, sqlStatement);
        mDataAccess->execute(headers, sqlStatement, receivedMessage);

        receivedMessage>>statusCode;
        break;
    }
    case JavaStorageMessage::ERead:
    {
        // Contains only one entry. It is already checked.
        JavaStorageApplicationEntry_t::const_iterator iter = aEntry.begin();

        wstring sqlStatement = L"SELECT * FROM ";
        sqlStatement.append(
            JavaCommonUtils::utf8ToWstring(identifier.c_str()));
        sqlStatement.append(L" WHERE ID = '")
        .append((*iter).entryValue()).append(L"';");

        mDataAccess->execute(headers, sqlStatement, receivedMessage);

        receivedMessage>>statusCode;

        if (statusCode >= 0)
        {
            // Const cast required. This prevent need to create yet another
            // createAndSend subroutine.
            JavaStorageApplicationEntry_t& temp =
                const_cast<JavaStorageApplicationEntry_t&>(aEntry);

            // Value was added by the API so remove that it does not exists
            // in the response twice.
            temp.clear();
            populateEntry(receivedMessage, temp);
        }
        break;
    }
    case JavaStorageMessage::ERemove:
    {
        wstring sqlStatement = L"DELETE FROM ";

        sqlStatement.append(
            JavaCommonUtils::utf8ToWstring(identifier.c_str()));

        if (aEntry.size() > 0)
        {
            mUtils->createWhereStatement(aEntry, sqlStatement);
        }
        else    // Delete whole table contents.
        {
            sqlStatement.append(L";");
        }
        mDataAccess->execute(headers, sqlStatement, receivedMessage);

        receivedMessage>>statusCode;

        if (statusCode >= 0)
        {
            wstring temp = L"";
            receivedMessage>>temp;
            // How many entries removed i.e. rows removed.
            retVal = JavaCommonUtils::wstringToInt(temp);
        }
        break;
    }
    case JavaStorageMessage::ECreateTable:
    {
        wstring sqlStatement = L"";
        mUtils->createTableStatement(
            aEntry, identifier, insertion, sqlStatement);
        mDataAccess->execute(headers, sqlStatement, receivedMessage);
        receivedMessage>>statusCode;
        break;
    }
    case JavaStorageMessage::EAppendTable:
    {
        // NOTE: SQLite supports adding only one column at the time.
        // For this reason several calls need to be executed.

        wstring prefix = L"ALTER TABLE ";

        prefix.append(
            JavaCommonUtils::utf8ToWstring(identifier.c_str()));

        // Define added columns
        // ID will always be primary key
        prefix.append(L" ADD ");

        JavaStorageApplicationEntry_t::const_iterator colIter;

        for (colIter = aEntry.begin(); colIter != aEntry.end(); colIter++)
        {
            if ((*colIter).entryName() == L"")
            {
                throw JavaStorageException(EInvalidDataStructure,
                                           "Column name not defined",
                                           __FILE__, __FUNCTION__, __LINE__);
            }

            wstring sqlStatement = prefix;
            sqlStatement.append((*colIter).entryName());

            if (JavaStorageEntry::STRING == (*colIter).entryType())
            {
                sqlStatement.append(L" varchar");
            }
            else if (JavaStorageEntry::INT == (*colIter).entryType())
            {
                sqlStatement.append(L" int");
            }
            else
            {
                ELOG(EJavaStorage, "Unknown column type");
                throw JavaStorageException(EInvalidDataStructure,
                                           "Unknown column type",
                                           __FILE__, __FUNCTION__, __LINE__);
            }
            sqlStatement.append(L";");

            mDataAccess->execute(headers, sqlStatement, receivedMessage);

            receivedMessage>>statusCode;

            if (statusCode < 0)
            {
                break;  // stop execution on first error.
            }
            else
            {
                // Reset for next round
                receivedMessage.reset();
            }
        }
        break;
    }
    default:
    {
        ELOG1(EJavaStorage, "Unknown command: %d", aMsgId);
        throw JavaStorageException(aMsgId, "Unknown message",
                                   __FILE__, __FUNCTION__, __LINE__);
    }
    }

    if (statusCode < 0)
    {
        string errorMessage = "";
        readErrorMessage(receivedMessage, errorMessage);
        throw JavaStorageException(statusCode, errorMessage.c_str(),
                                   __FILE__, __FUNCTION__, __LINE__);
    }
    return retVal;
}

void MessageDispatcher::handleSearch(
    JavaStorageMessage::MessageIdentifier aMsgId,
    const string& aHeaders,
    const string& aTableName,
    const JavaStorageApplicationEntry_t& aEntry,
    JavaStorageApplicationList_t& aAppList
)
{
    int statusCode = -1;
    CommsMessage receivedMessage;

    string headers = aHeaders;
    createHeader(aMsgId, headers);

    wstring sqlStatement = L"";
    mUtils->createSearchStatement(aEntry, aTableName, sqlStatement);
    mDataAccess->execute(headers, sqlStatement, receivedMessage);

    receivedMessage>>statusCode;

    if (statusCode >= 0)
    {
        populateEntry(receivedMessage, aAppList);
    }
    else
    {
        string errorMessage = "";
        readErrorMessage(receivedMessage, errorMessage);
        throw JavaStorageException(statusCode, errorMessage.c_str(),
                                   __FILE__, __FUNCTION__, __LINE__);
    }
}

void MessageDispatcher::handleUpdate(
    JavaStorageMessage::MessageIdentifier aMsgId,
    const string& aHeaders,
    const string& identifier,
    const JavaStorageApplicationEntry_t& aUpdateEntry,
    const JavaStorageApplicationEntry_t& aMatchEntry)
{
    int statusCode = -1;
    CommsMessage receivedMessage;
    string headers = aHeaders;
    createHeader(aMsgId, headers);

    wstring sqlStatement = L"";
    mUtils->createUpdateStatement(
        aUpdateEntry, aMatchEntry, identifier, sqlStatement);

    mDataAccess->execute(headers, sqlStatement, receivedMessage);
    receivedMessage>>statusCode;

    if (statusCode < 0)
    {
        string errorMessage = "";
        readErrorMessage(receivedMessage, errorMessage);
        throw JavaStorageException(statusCode, errorMessage.c_str(),
                                   __FILE__, __FUNCTION__, __LINE__);
    }
}

void MessageDispatcher::populateEntry(CommsMessage& aReceivedMessage,
                                      JavaStorageApplicationEntry_t& aEntry)
{
    JELOG2(EJavaStorage);

    wstring data;
    aReceivedMessage>>data;

    wstring::size_type startIdx = 0;
    wstring::size_type endIdx = 0;
    wstring appDelim = L";#\n;";
    wstring attrDelim = L";\n;";

    // Skipp app delimiter
    if (data.find(appDelim) == 0)
    {
        startIdx = appDelim.size();
        endIdx = startIdx;
    }

    // This method reads only one storage row. If multiple ones are given
    // they are ignored.
    if ((endIdx = data.find(appDelim, startIdx)) != string::npos)
    {
        WLOG(EJavaStorage, "Removing multiple app occurrences");
        data = data.substr(0, endIdx);
    }

    // Reset counter
    endIdx = startIdx;

    wstring nameDelim = L"=";

    while (string::npos != startIdx || string::npos != endIdx)
    {
        // Read name
        endIdx = data.find(nameDelim, startIdx);
        if (endIdx == string::npos) break;    // Not found

        wstring name = data.substr(startIdx, (endIdx - startIdx));

        // Read value. Skip name and delimiter
        startIdx = endIdx + 1;
        endIdx = data.find(attrDelim, startIdx);
        if (endIdx == string::npos) break;    // Not found

        wstring value = L"";

        if (endIdx != startIdx)
        {
            value = data.substr(startIdx, (endIdx - startIdx));
        }
        // else empty value.

        JavaStorageEntry entry;
        entry.setEntry(name, value);
        aEntry.insert(entry);

        // Skip value and delimiter
        startIdx = endIdx + attrDelim.size();
        if (startIdx == string::npos) break;    // Not found
    }
}

void MessageDispatcher::populateEntry(CommsMessage& aReceivedMessage,
                                      JavaStorageApplicationList_t& aAppList)
{
    JELOG2(EJavaStorage);

    wstring data;
    aReceivedMessage>>data;

    // ################# TEMP #################################################
    // LOG1WSTR(EJavaStorage, EInfo, "Response: %s", data);
    // ################# END OF TEMP ##########################################

    wstring appDelim = L";#\n;";

    if (data.size() <= appDelim.size())
    {
        LOG(EJavaStorage, EInfo, "No entries to populate");
        return;
    }

    wstring::size_type startIdx = appDelim.size();
    wstring::size_type endIdx = startIdx;

    // This does two if else less within while loop as its known string
    // terminates always despite of column amount.
    data.append(appDelim);

    JavaStorageApplicationEntry_t appEntry;

    do
    {
        endIdx = data.find(appDelim, startIdx);

        if (endIdx == string::npos) break;    // No token found

        wstring appString = data.substr(startIdx, (endIdx - startIdx));

        CommsMessage oneApp;
        oneApp<<appString;
        populateEntry(oneApp, appEntry);

        aAppList.push_back(appEntry);
        appEntry.clear();

        // Skip app delimiter
        startIdx = endIdx + appDelim.size();
    }
    while (string::npos != startIdx && string::npos != endIdx);
}

void MessageDispatcher::createHeader(int aMsgId, string& aHeaders)
{
    // Add command ID that server can identify open action
    string msgID = JavaCommonUtils::intToString(aMsgId);
    string msgIDSize = JavaCommonUtils::intToString(msgID.size());
    // Insert size which must be fit to one char.
    aHeaders.insert(0, msgIDSize);
    // Insert msgID
    aHeaders.insert(1, msgID);
}

void MessageDispatcher::readErrorMessage(CommsMessage& aReceivedMessage,
        string& aErrMsg)
{
    wstring wideErrorMsg;
    aReceivedMessage>>wideErrorMsg;

    char* errorMsg = JavaCommonUtils::wstringToUtf8(wideErrorMsg);
    string errorMessage(errorMsg);
    delete[] errorMsg;
    aErrMsg = errorMessage;
    ELOG1(EJavaStorage, "Error message %s", aErrMsg.c_str());
}
