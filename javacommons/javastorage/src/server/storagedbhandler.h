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
* Description:  StorageDBHandler
*
*/


#ifndef STORAGEDBHANDLER_H
#define STORAGEDBHANDLER_H

#include <stdio.h>
#include <string>
#include <sqlite3.h>
#include <vector>

#include "javaosheaders.h"
#include "javastorageexception.h"

namespace java
{
namespace storage
{

/**
 * StorageDBHandler handles database related operations.
 */
class StorageDBHandler
{
public:
    /**
     * Constructor.
     */
    StorageDBHandler();

    /**
     * Destructor. Close all active database connections.
     */
    ~StorageDBHandler();

    /**
     * Open database connection. Session id for the connection is created and
     * if the database has not been created yet, it is created now.
     * If the database is one of the default databases and it is has not
     * been created yet, the database schema is created along with the database.
     *
     * Database can be created anywhere access rights are enough. If only
     * the name of the database is provided it is created to default location.
     *
     * @param aDbName the name of the database to be opened.
     * @return result code. Negative if problems occurred.
     */
    int open(const std::wstring& aDbName);

    /**
     * Close database connection. If connection has open transaction it is
     * rollbacked. When connection is closed it's removed from open connection
     * list.
     *
     * @return result code. Negative if problems occurred.
     */
    int close();

    /**
     * Start transaction.
     *
     * @return result code. Negative if problems occurred.
     */
    int startTransaction();

    /**
     * Prepare SQL statement.
     *
     * @param aMessage SQL statement to be prepared.
     * @return result code. Negative if problems occurred.
     */
    int prepare(const std::wstring& aMessage);

    /**
     * Execute prepared statement.
     *
     * @return reference to database response.
     */
    std::wstring& executeStmt();

    /**
     * Commit transaction.
     *
     * @return result code. Negative if problems occurred.
     */
    int commit();

    /**
     * Rollback transaction.
     *
     * @return result code. Negative if problems occurred.
     */
    int rollback();

    /**
     * Select used connection from connection list.
     *
     * @param aSessionID is used to identify correct connection.
     * @return true if connection found from the list, false otherwise.
     */
    bool selectConnection(const std::wstring& aSessionID);

    /**
     * Get error message from the database.
     *
     * @return error message.
     */
    const char* getErrorMessage();

    /**
     * Get error code from the database.
     *
     * @return error code.
     */
    int getErrorCode();

    /**
     * Return session id of the current connection.
     *
     * @return session id.
     */
    std::wstring returnSessionID();

    /**
     * Database callback method. This is used to log errors.
     *
     * @param aNotUsed not used.
     * @param aArgc received argument amount.
     * @param aArgv argument value.
     * @param aAzColName column name.
     * @return error code.
     */
    static int dBCallback(void *aNotUsed,
                          int aArgc,
                          char **aArgv,
                          char **aAzColName);

private:

    /**
     * Initialise given storage. Create database structures.
     *
     * @param aDbName database to be initialized.
     * @throws JavaStorageException if table creation fails.
     */
    void initializeDatabase(const std::string& aDbName)
    throw(JavaStorageException);

    /**
     * Check whether given storage is initialized or not.
     * Supports only predefined tables. If table is not predefined
     * false is returned.
     *
     * @param aStorageName to be checked is initialized.
     * @return true if already initialized, false otherwise.
     */
    bool isDBInitialized(const std::string& aDbName);

    /**
     * Execute standalone statement against database. Only operation result
     * code is checked.
     *
     * @param aStatement SQL statement to be executed.
     * @throws JavaStorageException if execution fails.
     */
    void executeStandalone(const std::string& aStatement)
    throw(JavaStorageException);

private:
    /**
     * Current database connection.
     */
    sqlite3 *iDb;

    /**
     * Current statement. This is initialized at prepare and
     * finalized at executeStmt method.
     * If error occurs while preparing the statement SQLite will finalize
     * statement.
     */
    sqlite3_stmt *iStatement;

    /**
     * Current database response. This is cleared when executeStmt is
     * called again.
     */
    std::wstring iResultString;
    bool mHavingTransaction;
    std::pair<std::wstring, sqlite3*> iCurrentSession;
    std::vector<std::pair<std::wstring, sqlite3*> > iDbList;
    std::vector<std::pair<std::wstring, sqlite3*> >::iterator iDbListIter;

};

}    // java
}    // storage

#endif // STORAGEDBHANDLER_H
