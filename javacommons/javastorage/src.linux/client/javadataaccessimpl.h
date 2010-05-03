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
* Description:  JavaDataAccessImpl
*
*/


#ifndef JAVADATAACCESSIMPL_H
#define JAVADATAACCESSIMPL_H

#include <memory>

#include "javadataaccess.h"
#include "javaosheaders.h"

namespace java
{
namespace comms
{
class CommsClientEndpoint;
class CommsMessage;
}    // end namespace comms
namespace storage
{

/**
 * JavaDataAccessImpl implements JavaDataAccess interface.
 */
class JavaDataAccessImpl : public JavaDataAccess
{

    friend class JavaDataAccess;

public:
    /**
     * Destructor. If destructor is called while having active connection
     * Connection is rollbacked and closed before closing comms connection
     * to storage server.
     */
    OS_IMPORT virtual ~JavaDataAccessImpl();

    /**
     * Open connection to given storage. Send connection open comms mesasge to
     * storage server and caches received session ID to disconnect connection
     * gracefully at error situation.
     *
     * @param aHeaders connection headers.
     * @param aStorageName storage name.
     * @param aReceiveMessage received comms message.
     * @throws JavaStorageException if communication fails.
     */
    OS_IMPORT virtual void open(const std::string& aHeaders,
                                const std::string& aStorageName,
                                comms::CommsMessage& aReceivedMessage)
    throw(JavaStorageException);

    /**
     * Send close message to storage server.
     *
     * @param aHeaders connection headers.
     * @param aReceiveMessage received comms message.
     * @throws JavaStorageException if communication fails.
     */
    OS_IMPORT virtual void close(const std::string& aHeaders,
                                 comms::CommsMessage& aReceivedMessage)
    throw(JavaStorageException);

    /**
     * Send SQL statement to storage server for execution.
     *
     * @param aHeaders connection headers.
     * @param aSqlStatement SQL statement.
     * @param aReceiveMessage received comms message.
     * @throws JavaStorageException if communication fails.
     */
    OS_IMPORT virtual void execute(const std::string& aHeaders,
                                   const std::wstring& aSqlStatement,
                                   comms::CommsMessage& aReceivedMessage)
    throw(JavaStorageException);

private:
    /**
     * Constructor. Initiate comms connection to storage server
     *
     * @throws JavaStorageException if cannot connect to storage server using
     * comms.
     */
    JavaDataAccessImpl() throw(JavaStorageException);

    /**
     * Send and receive comms messages.
     *
     * @param aSendMessage message to be sent.
     * @param aReceivedMessage received message.
     * @throws JavaStorageException if communication fails. Exception is thrown
     * if comms returns error code. Comms error code is set as exception error
     * code.
     */
    void sendReceiveMessage(comms::CommsMessage& aSendMessage,
                            comms::CommsMessage& aReceivedMessage)
    throw(JavaStorageException);

private:
    std::auto_ptr<comms::CommsClientEndpoint> mCliConn;
    bool mConnOpen;
    std::string mCachedSessionID;
};

} // end namespace storage
} // end namespace java

#endif // JAVADATAACCESSIMPL_H

