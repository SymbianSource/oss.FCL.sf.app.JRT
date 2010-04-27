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


#include <sstream>
#include <sqlite3.h>
#include <stdlib.h>
#include <vector>

#include "javacommonutils.h"
#include "javastoragenames.h"
#include "javastoragetables.h"
#include "logger.h"
#include "storagedbhandler.h"

using namespace java::storage;
using namespace java::util;
using namespace std;

StorageDBHandler::StorageDBHandler()
{
    JELOG2(EJavaStorage);
    iDb = 0;
    iResultString = L"";
    mHavingTransaction = false;
}

StorageDBHandler::~StorageDBHandler()
{
    JELOG2(EJavaStorage);

    // close and and remove all open sessions from DB list
    for (iDbListIter = iDbList.begin();
            iDbListIter != iDbList.end();
            iDbListIter++)
    {
        if ((*iDbListIter).second != 0)
        {
            sqlite3_close((*iDbListIter).second);
        }
    }
    iDbList.clear();
}

int StorageDBHandler::open(const wstring& aDbName)
{
    JELOG2(EJavaStorage);
    mHavingTransaction = false;
    char* dbnameAsChar = 0;

    try
    {
        dbnameAsChar = JavaCommonUtils::wstringToUtf8(aDbName);
    }
    catch (ExceptionBase& eb)
    {
        ELOG1(EJavaStorage, "UTF8 conversion error: %s", eb.toString().c_str());
        return -1;
    }

    // Create a random session id number for the connection
    wstring sessionID = JavaCommonUtils::intToWstring(rand());
    sessionID += JavaCommonUtils::intToWstring(rand());
    int rc = sqlite3_open(dbnameAsChar, &iDb);

    if (rc)
    {
        ELOG1(EJavaStorage, "Cannot open database: %s",
              sqlite3_errmsg(iDb));
        sqlite3_close(iDb);
        delete[] dbnameAsChar;
        return rc;
    }
    else
    {
        iCurrentSession.first = sessionID;
        iCurrentSession.second = iDb;
        iDbList.push_back(iCurrentSession);
    }

    if (!isDBInitialized(dbnameAsChar))
    {
        LOG(EJavaStorage, EInfo, "Initializing DB.");
        try
        {
            initializeDatabase(dbnameAsChar);
            LOG(EJavaStorage, EInfo, "DB initialized OK.");
        }
        catch (JavaStorageException& jse)
        {
            ELOG1(EJavaStorage,
                  "Init failed: %s",
                  jse.toString().c_str());
            rc = EInternalError;
        }
    }

    delete[] dbnameAsChar;
    return rc;
}

int StorageDBHandler::close()
{
    JELOG2(EJavaStorage);

    // If active transaction while closing connection. Rollback is done
    // to maintain DB consistency.
    if (mHavingTransaction)
    {
        WLOG(EJavaStorage,
             "Close while active session. Doing rollback");
        int err = rollback();
        if (err < 0)
        {
            WLOG1(EJavaStorage, "Rollback failed: %d", err);
        }
    }

    int rc = sqlite3_close(iDb);

    // remove the session connection object from the connections list
    for (iDbListIter = iDbList.begin();
            iDbListIter != iDbList.end();
            ++iDbListIter)
    {
        if (iCurrentSession.first == (*iDbListIter).first)
        {
            iDbList.erase(iDbListIter++);
            break;
        }
    }
    return rc;
}

int StorageDBHandler::startTransaction()
{
    JELOG2(EJavaStorage);
    int rc = 0;
    string statement = "BEGIN;";
    try
    {
        executeStandalone(statement);
        mHavingTransaction = true;
    }
    catch (JavaStorageException& jse)
    {
        rc = jse.mStatus;
        mHavingTransaction = false;
        ELOG1(EJavaStorage,
              "Transaction start failed: %s",
              jse.toString().c_str());
    }

    return rc;
}

int StorageDBHandler::prepare(const std::wstring& aMessage)
{
    JELOG2(EJavaStorage);
    int rc;
    const char *tail = 0;
    int statementBytes = aMessage.length() * 4;
    char *zErrMsg = 0;
    char* stmAsChar = 0;

    try
    {
        stmAsChar = JavaCommonUtils::wstringToUtf8(aMessage);
    }
    catch (ExceptionBase& eb)
    {
        ELOG1(EJavaStorage, "UTF8 conversion error: %s", eb.toString().c_str());
        return -1;
    }

    rc = sqlite3_prepare_v2(
             iDb, stmAsChar, statementBytes , &iStatement, &tail);


    if (SQLITE_OK != rc)
    {
        ELOG1(EJavaStorage, "SQL error on prepare: %d", rc);
        sqlite3_free(zErrMsg);
        delete [] stmAsChar;
        return rc;
    }

    delete [] stmAsChar;

    return rc;
}

std::wstring& StorageDBHandler::executeStmt()
{
    JELOG2(EJavaStorage);
    int rc;
    iResultString.clear();
    std::wostringstream oss;

    rc = sqlite3_step(iStatement);

    while (SQLITE_ROW == rc)
    {
        const char *columnName(0);
        const unsigned char *column(0);
        oss << L";#\n;";

        for (int i = 0; i < sqlite3_column_count(iStatement); i++)
        {
            column = sqlite3_column_text(iStatement, i);

            if (column)
            {
                wstring columnValue = JavaCommonUtils::utf8ToWstring(
                                          reinterpret_cast<const char*>(column));

                columnName = sqlite3_column_name(iStatement, i);
                oss << JavaCommonUtils::utf8ToWstring(columnName);
                oss << L"=" + columnValue + L";\n;";
            }
        }
        rc = sqlite3_step(iStatement);
    }

    iResultString = oss.str();

    // Return affected rowCount if no other content to be returned.
    // This is to return e.g. how many rows removed from DB using DELETE
    // statement.
    if (L"" == iResultString)
    {
        int changes = sqlite3_changes(iDb);
        iResultString = JavaCommonUtils::intToWstring(changes);
    }

    // Same statement is not reused.
    int fc = sqlite3_finalize(iStatement);
    if (0 != fc)
    {
        WLOG1(EJavaStorage, "Finalize failed: %d", fc);
    }

    return iResultString;
}

int StorageDBHandler::commit()
{
    JELOG2(EJavaStorage);
    int rc = 0;
    string statement = "COMMIT;";
    try
    {
        executeStandalone(statement);
        mHavingTransaction = false;
    }
    catch (JavaStorageException& jse)
    {
        rc = jse.mStatus;
        ELOG1(EJavaStorage,
              "Transaction commit failed: %s",
              jse.toString().c_str());
    }

    return rc;
}

int StorageDBHandler::rollback()
{
    JELOG2(EJavaStorage);
    int rc = 0;
    string statement = "ROLLBACK;";
    try
    {
        executeStandalone(statement);
        mHavingTransaction = false;
    }
    catch (JavaStorageException& jse)
    {
        rc = jse.mStatus;
        ELOG1(EJavaStorage,
              "Transaction rollback failed: %s",
              jse.toString().c_str());
    }

    return rc;
}

bool StorageDBHandler::selectConnection(const wstring& aSessionID)
{
    JELOG2(EJavaStorage);
    for (iDbListIter = iDbList.begin();
            iDbListIter != iDbList.end();
            iDbListIter++)
    {
        iCurrentSession = *iDbListIter;
        if (iCurrentSession.first == aSessionID)
        {
            iDb = iCurrentSession.second;
            return true;
        }
    }
    return false;
}

wstring StorageDBHandler::returnSessionID()
{
    return iCurrentSession.first;
}

bool StorageDBHandler::isDBInitialized(const string& aDbName)
{
    JELOG2(EJavaStorage);
    bool result = false;

    string sqlCommand = "SELECT ID FROM ";

    if (JAVA_DATABASE_NAME == aDbName)
    {
        sqlCommand.append(APPLICATION_PACKAGE_TABLE);
    }
    else if (JAVA_OTA_DATABASE_NAME == aDbName)
    {
        sqlCommand.append(OTA_STATUS_TABLE);
    }
    else
    {
        // Only predefined databases are supported.
        return result;
    }

    sqlCommand.append(";");

    try
    {
        executeStandalone(sqlCommand);
        result = true;
    }
    catch (JavaStorageException& jse)
    {
        WLOG1(EJavaStorage, "DB not init: %s", jse.toString().c_str());
    }
    return result;
}

void StorageDBHandler::initializeDatabase(const string& aDbName)
throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    if (JAVA_DATABASE_NAME == aDbName)
    {
        // Create java database tables.
        executeStandalone(APPLICATION_PACKAGE);
        executeStandalone(APPLICATION);
        executeStandalone(APPLICATION_PACKAGE_ATTRIBUTES);
        executeStandalone(MIDP_PACKAGE);
        executeStandalone(MIDP_PERMISSIONS);
        executeStandalone(MIDP_FUNC_GRP_SETTINGS);
        executeStandalone(PUSH_REGISTRATIONS);
        executeStandalone(ALARM_REGISTRATIONS);
        executeStandalone(RUNTIME_SETTINGS);
        executeStandalone(PREINSTALL);
    }
    else if (JAVA_OTA_DATABASE_NAME == aDbName)
    {
        // Create OTA database table.
        executeStandalone(OTA_STATUS);
    }
    else
    {
        // Only predefined database init is supported. No error thrown as
        // db open is still success.
    }
}

void StorageDBHandler::executeStandalone(const string& aStatement)
throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    char *zErrMsg = 0;
    int rc = -1;

    rc = sqlite3_exec(iDb, aStatement.c_str(), dBCallback, 0, &zErrMsg);

    if (SQLITE_OK != rc)
    {
        ELOG2(EJavaStorage, "Db error code: %d msg: %s", rc, zErrMsg);
        string errString(zErrMsg);

        sqlite3_free(zErrMsg);

        throw JavaStorageException(rc,
                                   errString.c_str(),
                                   __FILE__,
                                   __FUNCTION__,
                                   __LINE__);
    }
}

int StorageDBHandler::dBCallback(void */*NotUsed*/,
                                 int argc,
                                 char **argv,
                                 char **azColName)
{
    JELOG2(EJavaStorage);
    for (int i = 0; i < argc; i++)
    {
        ELOG2(EJavaStorage, "    %s = %s",
              azColName[i], argv[i] ? argv[i] : "NULL");
    }
    return 0;
}

const char* StorageDBHandler::getErrorMessage()
{
    JELOG2(EJavaStorage);
    return sqlite3_errmsg(iDb);
}

int StorageDBHandler::getErrorCode()
{
    JELOG2(EJavaStorage);
    return sqlite3_errcode(iDb);
}
