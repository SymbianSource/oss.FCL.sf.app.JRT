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


#ifndef MESSAGEDISPATCHER_H
#define MESSAGEDISPATCHER_H

#include <string>
#include <memory>

#include "javaosheaders.h"
#include "javastorageentry.h"
#include "javastorage.h"
#include "javastoragemessage.h"

namespace java
{
namespace comms
{
class CommsMessage;
}  // end namespace comms
namespace storage
{
class JavaDataAccess;
class StatementUtils;

/**
 * MessageDispatcher takes care of message handling. It creates messages for
 * the storage and gives them for sending and construct response object from
 * the message response if needed.
 */
OS_NONSHARABLE_CLASS(MessageDispatcher)
{
    friend class JavaStorageImpl;

public:
    /**
     * Destructor.
     */
    ~MessageDispatcher();

    /**
     * CreateAndSendMessage for short storage statements. These are
     * transaction and connection handling requests.
     *
     * @param aMsgId message identifier.
     * @param aHeaders connection headers.
     * @param aStorageName storage name to be opened. This is used only when
     *                     connection is opened.
     * @return sessionID string if connection is opened, not used otherwise.
     * @throws JavaStorageException if unknown message id or error response
     *         received from the storage.
     */
    std::string createAndSendMessage(
        JavaStorageMessage::MessageIdentifier aMsgId,
        const std::string& aHeaders,
        const std::string& aStorageName
    );

    /**
     * CreateAndSendMessage for storage statements.
     *
     * @param aMsgId message identifier.
     * @param aHeaders connection headers.
     * @param aTableName table where statement is executed.
     * @param aEntry Application entry holding statement data. It contains e.g.
     *               entries written to storage table.
     * @return amount of removed storage entries. This is used only when data
     *         is removed from the storage.
     * @throws JavaStorageException if unknown message id or error response
     *         received from the storage.
     */
    int createAndSendMessage(
        JavaStorageMessage::MessageIdentifier aMsgId,
        const std::string& aHeaders,
        const std::string& aTableName,
        const java::storage::JavaStorageApplicationEntry_t& aEntry
    );

    /**
     * Handlesearch handles search statements. It creates SQL statement
     * out of entry, handles communication and constructs application list
     * from response.
     *
     * @param aMsgId message identifier.
     * @param aHeaders connection headers.
     * @param aTableName table where statement is executed.
     * @param aEntry Application entry holding statement data. It contains
     *               search query data.
     * @param[out] aAppList response object. Each application represents one row.
     * @throws JavaStorageException if unknown message id, error response
     *         received from the storage or invalid data structure detected.
     */
    void handleSearch(
        JavaStorageMessage::MessageIdentifier aMsgId,
        const std::string& aHeaders,
        const std::string& aTableName,
        const java::storage::JavaStorageApplicationEntry_t& aEntry,
        JavaStorageApplicationList_t& aAppList
    );

    /**
     * HandleUpdate hadles update statements. It creates SQL statement
     * out of entries and send it to storage for execution.
     *
     * @param aMsgId message identifier.
     * @param aHeaders connection headers.
     * @param aTableName table where statement is executed.
     * @param aUpdateEntry Application entry holding updated data.
     * @param aMatchEntry Application entry holding data to match content
     *                    to be updated.
     * @throws JavaStorageException if unknown message id or error response
     *         received from the storage.
     */
    void handleUpdate(
        JavaStorageMessage::MessageIdentifier aMsgId,
        const std::string& aHeaders,
        const std::string& aTableName,
        const JavaStorageApplicationEntry_t& aUpdateEntry,
        const JavaStorageApplicationEntry_t& aMatchEntry);

private:
    /**
     * Private constructor. Creates data access object that is used to
     * communicate with the storage.
     */
    MessageDispatcher();

    /**
     * Populate single entry. If response contains more than one application
     * entries they are ignored. If response contains malformed entries
     * population is stopped and already populated valid entries are left
     * populated for a robustness sake.
     *
     * @param aReceivedMessage response message from the storage as wstring
     *                         representation.
     * @param[out] aEntry Application entry populated with response message
     *                    content. Entry is left empty if no entries found.
     */
    void populateEntry(comms::CommsMessage& aReceivedMessage,
                       JavaStorageApplicationEntry_t& aEntry);

    /**
     * Populate application list. If response contains malformed application
     * entries population is stopped and already populated valid entries are
     * left populated for a robustness sake.
     *
     * @param aReceivedMessage response message from the storage as wstring
     *                         representation.
     * @param[out] aAppList Application list populated with response message
     *                    content. List is left empty if no entries found.
     */
    void populateEntry(comms::CommsMessage& aReceivedMessage,
                       JavaStorageApplicationList_t& aAppList);

    /**
     * Create message header.
     *
     * @param aMsgId message identifier.
     * @param aHeaders current headers.
     */
    void createHeader(int aMgsId, std::string& aHeaders);

    /**
     * Read error message from received message.
     *
     * @param aReceivedMessage received message.
     * @param[out] aErrMsg error message.
     */
    void readErrorMessage(comms::CommsMessage& aReceivedMessage,
                          std::string& aErrMsg);

private:
    std::auto_ptr<JavaDataAccess> mDataAccess;
    std::auto_ptr<StatementUtils> mUtils;

};

} // end namespace storage
} // end namespace java

#endif // MESSAGEDISPATCHER_H

