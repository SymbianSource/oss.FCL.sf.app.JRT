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

#include <sqldb.h>

#include "javadataaccess.h"
#include "javaosheaders.h"

namespace java
{
namespace comms
{
class CommsMessage;
}    // end namespace comms
namespace storage
{

class JavaStorageException;

/**
 * JavaDataAccessImpl implements JavaDataAccess interface.
 */
OS_NONSHARABLE_CLASS(JavaDataAccessImpl) : public JavaDataAccess
{

    friend class JavaDataAccess;

public:
    /**
     * Destructor. If destructor is called while having active connection
     * Connection is rollbacked and closed before closing database connection.
     */
    OS_IMPORT virtual ~JavaDataAccessImpl();

    /**
     * Open given storage. If storage does not exists it is created. If storage
     * is default system storage also database schema is created.
     *
     * @param aHeaders connection headers.
     * @param aStorageName storage name.
     * @param[out] aReceiveMessage response message.
     * @throws JavaStorageException if communication fails.
     */
    OS_IMPORT virtual void open(const std::string& aHeaders,
                                const std::string& aStorageName,
                                comms::CommsMessage& aReceivedMessage)
    throw(JavaStorageException);

    /**
     * Close storage. If storage has ongoing session it is rollbacked.
     *
     * @param aHeaders connection headers.
     * @param[out] aReceiveMessage response message.
     * @throws JavaStorageException if communication fails.
     */
    OS_IMPORT virtual void close(const std::string& aHeaders,
                                 comms::CommsMessage& aReceivedMessage)
    throw(JavaStorageException);

    /**
     * Execute given SQL statement against database and create response.
     *
     * @param aHeaders connection headers.
     * @param aSqlStatement SQL statement.
     * @param[out] aReceiveMessage received comms message.
     * @throws JavaStorageException if communication fails.
     */
    OS_IMPORT virtual void execute(const std::string& aHeaders,
                                   const std::wstring& aSqlStatement,
                                   comms::CommsMessage& aReceivedMessage)
    throw(JavaStorageException);

private:
    JavaDataAccessImpl();

    /**
     * Check whether given storage is initialized or not.
     * Supports only predefined tables. If table is not predefined
     * false is returned.
     *
     * @param aStorageName to be checked is initialized.
     * @return true if already initialized, false otherwise.
     */
    bool isDBInitialized(const std::string& aStorageName);

    /**
     * Open given storage. If open fails storage is created and
     * initialized. If storage is not predefined storage is created but not
     * initialized.
     *
     * @param aStorageName to be opened.
     * @throws JavaStorageException if storage creation fails.
     */
    void openDatabase(const std::string& aStorageName)
    throw(JavaStorageException);

    /**
     * Create database. Database is created to Symbian's SqlServer private
     * data cage with predefined security policy.
     *
     * @param aDbName to be created.
     * @param aIsDefault is this default database.
     * @throws JavaStorageException if database creation fails. Failure code
     *         is stored to Exception status code.
     */
    void createDatabase(const TDesC& aDbName, bool aIsDefault)
    throw(JavaStorageException);

    /**
     * Create single table to storage.
     *
     * @param aSqlStatement containing storage creation statement.
     * @throws JavaStorageException if table creation failed.
     */
    void createTable(const TDesC& aStatement) throw(JavaStorageException);

    /**
     * Initialise given storage. Create database structures.
     *
     * @param aStorageName storage to be initialized.
     * @throws JavaStorageException if table creation fails.
     */
    void initializeDatabase(const std::string& aStorageName)
    throw(JavaStorageException);

    /**
     * Read column name from statement.
     *
     * @param aIndex Index where name is read.
     * @param aStmt Statement where name is read.
     * @param[out] aColName column name.
     */
    void columnName(int aIndex,
                    RSqlStatement& aStmt,
                    std::wstring& aColName);

    /**
     * Read column value from statement.
     *
     * @param aIndex Index where value is read.
     * @param aStmt Statement where value is read.
     * @param aColumnType Column type.
     * @param[out] aColValue column value.
     */
    void columnValue(const int aIndex,
                     RSqlStatement& aStmt,
                     const TSqlColumnType aColumnType,
                     std::wstring& aColValue);

private:
    bool mConnOpen;
    bool mHavingTransaction;
    std::string mSessionId;
    RSqlDatabase mDatabase;
};

} // end namespace storage
} // end namespace java

#endif // JAVADATAACCESSIMPL_H

