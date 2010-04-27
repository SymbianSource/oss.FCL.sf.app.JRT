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
* Description:  JavaDataAccess
*
*/


#ifndef JAVADATAACCESS_H
#define JAVADATAACCESS_H

#include <stdio.h>
#include <string>

#include "javaosheaders.h"
#include "javastorageexception.h"

namespace java
{
namespace comms
{
class CommsMessage;
}    // end namespace comms
namespace storage
{

/**
 * JavaDataAccess is JavaStorage internal API to hide different communications
 * between storage server processes.
 */
class JavaDataAccess
{
public:

    virtual ~JavaDataAccess() {}

    /**
     * Create API instance.
     *
     * @return Instance to JavaDataAccess API. Ownership is transfered to
     *         caller.
     * @throws JavaStorageException if communication channel creation fails.
     */
    OS_IMPORT static JavaDataAccess* createInstance()
    throw(JavaStorageException);

    /**
     * Open storage connection. If storage does not exist create it.
     *
     * @param aStorageName Name of the storage
     * @throws JavaStorageException if communication fails.
     */
    virtual void open(const std::string& aHeaders,
                      const std::string& aStorageName,
                      comms::CommsMessage& aReceivedMessage)
    throw(JavaStorageException) = 0;

    /**
     * Close storage connection and free its resources.
     *
     * @param aHeaders message headers.
     * @param aReceivedMessage received message.
     * @throws JavaStorageException if communication fails.
     */
    virtual void close(const std::string& aHeaders,
                       comms::CommsMessage& aReceivedMessage)
    throw(JavaStorageException) = 0;

    /**
     * Execute SQL statement to storage.
     *
     * @param aHeaders message headers.
     * @param aSqlStatement SQL statement.
     * @throws JavaStorageException if communication fails.
     */
    virtual void execute(const std::string& aHeaders,
                         const std::wstring& aSqlStatement,
                         comms::CommsMessage& aReceivedMessage)
    throw(JavaStorageException) = 0;
};

} // end namespace storage
} // end namespace java

#endif // JAVADATAACCESS_H

