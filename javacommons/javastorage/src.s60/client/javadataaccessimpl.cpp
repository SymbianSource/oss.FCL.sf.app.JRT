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

#include <memory>

#include "commsmessage.h"
#include "javacommonutils.h"
#include "javadataaccessimpl.h"
#include "javastorageexception.h"
#include "javastoragemessage.h"
#include "javastoragenames.h"
#include "javastoragetables.h"
#include "javasymbianoslayer.h"
#include "javauids.h"
#include "logger.h"

using namespace java::comms;
using namespace java::storage;
using namespace java::util;
using namespace std;

const int OK = 0;
_LIT8(KJavaDbTimeoutOption, "QSQLITE_BUSY_TIMEOUT==10000;");

JavaDataAccessImpl::JavaDataAccessImpl()
        : mConnOpen(false), mHavingTransaction(false)
{
}

OS_EXPORT JavaDataAccessImpl::~JavaDataAccessImpl()
{
    JELOG2(EJavaStorage);

    if (mHavingTransaction)
    {
        WLOG(EJavaStorage,
             "Active transaction while deleting. Rollback.");

        _LIT(KRollbackStatement, "ROLLBACK;");
        TInt err = mDatabase.Exec(KRollbackStatement);
    }

    if (mConnOpen)
    {
        mDatabase.Close();
    }
}

OS_EXPORT void JavaDataAccessImpl::open(
    const string& /*aHeaders*/,
    const string& aStorageName,
    CommsMessage& aReceivedMessage) throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    int status = OK;

    if (!mConnOpen)
    {
        try
        {
            openDatabase(aStorageName);
        }
        catch (JavaStorageException& aJse)
        {
            status = aJse.mStatus;
        }
    }
    else
    {
        WLOG(EJavaStorage, "Connection already open");
    }

    // SessionID is not needed in this platform context.
    mSessionId = "N/A";

    aReceivedMessage<<status;
    aReceivedMessage<<mSessionId;
}

bool JavaDataAccessImpl::isDBInitialized(const string& aStorageName)
{
    JELOG2(EJavaStorage);
    bool result = false;
    string sqlStatement = "SELECT ";

    if (JAVA_DATABASE_NAME == aStorageName)
    {
        sqlStatement.append("NAME FROM ");
        sqlStatement.append(PREINSTALL_TABLE);
    }
    else if (JAVA_OTA_DATABASE_NAME == aStorageName)
    {
        sqlStatement.append("ID FROM ");
        sqlStatement.append(OTA_STATUS_TABLE);
    }
    else
    {
        // External DB initialisation is not supported.
        // returning true skips other checks.
        return true;
    }

    sqlStatement.append(";");

    auto_ptr<HBufC> sqlDes(stringToDes(sqlStatement.c_str()));

    TInt err = mDatabase.Exec(sqlDes->Des());

    if (err >= 0)
    {
        result = true;
    }
    else
    {
        WLOG1(EJavaStorage, "Database is not initialized: %d", err);
    }

    return result;
}

void JavaDataAccessImpl::initializeDatabase(const string& aStorageName)
throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    if (JAVA_DATABASE_NAME == aStorageName
            && !isDBInitialized(aStorageName))
    {
        auto_ptr<HBufC> sqlDes(stringToDes(APPLICATION_PACKAGE));
        LOG(EJavaStorage, EInfo, "Creating APPLICATION PACKAGE");
        createTable(sqlDes->Des());

        sqlDes.reset(stringToDes(APPLICATION));
        LOG(EJavaStorage, EInfo, "Creating APPLICATION");
        createTable(sqlDes->Des());

        sqlDes.reset(stringToDes(APPLICATION_PACKAGE_ATTRIBUTES));
        LOG(EJavaStorage, EInfo, "Creating APPLICATION_PACKAGE_ATTRIBUTES");
        createTable(sqlDes->Des());

        sqlDes.reset(stringToDes(MIDP_PACKAGE));
        LOG(EJavaStorage, EInfo, "Creating MIDP_PACKAGE");
        createTable(sqlDes->Des());

        sqlDes.reset(stringToDes(MIDP_PERMISSIONS));
        LOG(EJavaStorage, EInfo, "Creating MIDP_PERMISSIONS");
        createTable(sqlDes->Des());

        sqlDes.reset(stringToDes(MIDP_FUNC_GRP_SETTINGS));
        LOG(EJavaStorage, EInfo, "Creating MIDP_FUNC_GRP_SETTINGS");
        createTable(sqlDes->Des());

        sqlDes.reset(stringToDes(PUSH_REGISTRATIONS));
        LOG(EJavaStorage, EInfo, "Creating PUSH_REGISTRATIONS");
        createTable(sqlDes->Des());

        sqlDes.reset(stringToDes(ALARM_REGISTRATIONS));
        LOG(EJavaStorage, EInfo, "Creating ALARM_REGISTRATIONS");
        createTable(sqlDes->Des());

        sqlDes.reset(stringToDes(RUNTIME_SETTINGS));
        LOG(EJavaStorage, EInfo, "Creating RUNTIME_SETTINGS");
        createTable(sqlDes->Des());

        sqlDes.reset(stringToDes(PREINSTALL));
        LOG(EJavaStorage, EInfo, "Creating PREINSTALL");
        createTable(sqlDes->Des());
    }
    else if (JAVA_OTA_DATABASE_NAME == aStorageName
             && !isDBInitialized(aStorageName))
    {
        auto_ptr<HBufC> sqlDes(stringToDes(OTA_STATUS));
        LOG(EJavaStorage, EInfo, "Creating OTA_STATUS");
        createTable(sqlDes->Des());
    }
}

OS_EXPORT void JavaDataAccessImpl::close(const string& /*aHeaders*/,
        CommsMessage& aReceivedMessage)
throw(JavaStorageException)
{
    JELOG2(EJavaStorage);
    int status = OK;

    // If active transaction while closing connection. Rollback is tried.
    if (mHavingTransaction)
    {
        WLOG(EJavaStorage,
             "Active transaction while closing connection. Rollback.");

        _LIT(KRollbackStatement, "ROLLBACK;");
        TInt err = mDatabase.Exec(KRollbackStatement);
    }

    mDatabase.Close();
    mConnOpen = false;
    mHavingTransaction = false;

    aReceivedMessage<<status;
    aReceivedMessage<<mSessionId;
}

OS_EXPORT void JavaDataAccessImpl::execute(const string& aHeaders,
        const wstring& aSqlStatement,
        CommsMessage& aReceivedMessage)
throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    int status = OK;
    wstring resultData;

    // First char contains msgId len
    int msgIdLen = JavaCommonUtils::stringToInt(aHeaders.substr(0, 1));
    // Next chars contain msgId
    int msgId = JavaCommonUtils::stringToInt(aHeaders.substr(1, msgIdLen));

    auto_ptr<HBufC> sqlStatement(HBufC16::New(aSqlStatement.size() + 1));

    if (!sqlStatement.get())
    {
        throw JavaStorageException(KErrNoMemory,
                                   "Cannot allocate statement Buffer",
                                   __FILE__, __FUNCTION__, __LINE__);
    }

    TPtr16 statementPtr(sqlStatement->Des());
    statementPtr = (const TUint16*)aSqlStatement.c_str();

    // ###################### TEMP ############################################
    // LOG1WSTR(EJavaStorage, EInfo, "Sql statement: %s", aSqlStatement);
    // ####################### END OF TEMP ####################################

    switch (msgId)
    {
    case JavaStorageMessage::EStartTransaction:
    {
        TInt err = mDatabase.Exec(sqlStatement->Des());
        status = err;
        resultData = L"OK";

        if (status < 0)
        {
            mHavingTransaction = false;
        }
        else
        {
            mHavingTransaction = true;
        }
        break;
    }
    case JavaStorageMessage::ECommit:
    case JavaStorageMessage::ERollback:
    {
        TInt err = mDatabase.Exec(sqlStatement->Des());
        status = err;
        resultData = L"OK";

        if (status >= 0)
        {
            mHavingTransaction = false;
        }
        break;
    }
    case JavaStorageMessage::EWrite:
    case JavaStorageMessage::ECreateTable:
    case JavaStorageMessage::EUpdate:
    case JavaStorageMessage::EAppendTable:
    {
        TInt err = mDatabase.Exec(sqlStatement->Des());
        status = err;
        resultData = L"OK";
        break;
    }
    case JavaStorageMessage::ERemove:
    {
        TInt err = mDatabase.Exec(sqlStatement->Des());
        status = err;

        if (0 <= status)
        {
            // Add affected row amount
            resultData = JavaCommonUtils::intToWstring(status);
        }
        break;
    }
    case JavaStorageMessage::ERead:
    case JavaStorageMessage::ESearch:
    {
        RSqlStatement statement;
        TInt err = statement.Prepare(mDatabase, sqlStatement->Des());

        if (err < 0)
        {
            ELOG1(EJavaStorage, "Prepare error: %d", err);
            status = err;
            statement.Close();
            break;
        }

        while ((err = statement.Next()) == KSqlAtRow)
        {
            resultData.append(L";#\n;");

            for (int i = 0; i < statement.ColumnCount(); i++)
            {
                TSqlColumnType colType = statement.ColumnType(i);

                if (colType != ESqlNull)
                {
                    wstring colValue = L"";
                    columnValue(i, statement, colType, colValue);

                    wstring colName = L"";
                    columnName(i, statement, colName);
                    resultData.append(colName).append(L"=")
                    .append(colValue).append(L";\n;");
                }
            }
        }

        if (err != KSqlAtEnd)
        {
            ELOG1(EJavaStorage,
                  "Error during statement execution: %d", err);
        }

        status = err;

        if (resultData.size() == 0)
        {
            resultData = L"OK"; // Indicate no match found.
        }

        statement.Close();
        break;
    }
    default:
    {
        LOG1(EJavaStorage, EInfo, "Unknown command: %d", msgId);
        break;
    }
    }

    if (status < 0)
    {
        ELOG1WSTR(EJavaStorage, "ErrorStmt: %s", aSqlStatement);
        TPtrC errMsg = mDatabase.LastErrorMessage();
        auto_ptr<HBufC16> tempBuf(HBufC16::New(errMsg.Size() + 1));

        if (!tempBuf.get())
        {
            throw JavaStorageException(
                KErrNoMemory, "Cannot allocate errorMsg buffer",
                __FILE__, __FUNCTION__, __LINE__);
        }

        TPtr16 tempBufPtr(tempBuf->Des());
        tempBufPtr.Append(errMsg);
        wstring tempStr(desToWstring(tempBufPtr));
        resultData = tempStr;

        if (resultData.size() == 0)
        {
            resultData = L"No error description available";
        }
        ELOG1WSTR(EJavaStorage, "EMsg: %s", resultData);
    }

    aReceivedMessage<<status;
    aReceivedMessage<<resultData;
}

void JavaDataAccessImpl::openDatabase(const string& aStorageName)
throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    auto_ptr<HBufC> dbName(0);
    bool isDefaultDB = false;

    if (JAVA_DATABASE_NAME == aStorageName
            || JAVA_OTA_DATABASE_NAME == aStorageName)
    {
        TUid tempUid = TUid::Uid(KJavaCaptainUid);
        java::util::Uid uid;
        TUidToUid(tempUid, uid);
        const char* tempStr = JavaCommonUtils::wstringToUtf8(uid.toString());
        string uidStr(tempStr);
        delete [] tempStr;
        string tempName = "c:" + uidStr + aStorageName + ".db";

        dbName.reset(stringToDes(tempName.c_str()));
        isDefaultDB = true;
    }
    else
    {
        dbName.reset(stringToDes(aStorageName.c_str()));
    }

    if (KErrNone != mDatabase.Open(dbName->Des(), &KJavaDbTimeoutOption))
    {
        createDatabase(dbName->Des(), isDefaultDB);
        initializeDatabase(aStorageName);
    }

    LOG1(EJavaStorage, EInfo, "Database '%s' opened", aStorageName.c_str());
    mConnOpen = true;
}

void JavaDataAccessImpl::createDatabase(const TDesC& aDbName, bool aIsDefault)
throw(JavaStorageException)
{
    JELOG2(EJavaStorage);
    mDatabase.Close();

    TInt err = KErrNone;

    if (aIsDefault)
    {
        TSecurityPolicy defaultPolicy;
        RSqlSecurityPolicy securityPolicy;

        securityPolicy.Create(defaultPolicy);

        // Create similar security policy to all Java platform system databases.

#ifdef RD_JAVA_S60_RELEASE_9_2_ONWARDS
        TSecurityPolicy schemaPolicy(ECapabilityTrustedUI);
        TSecurityPolicy writePolicy(ECapabilityTrustedUI);
#else  // RD_JAVA_S60_RELEASE_9_2_ONWARDS
        // Beta release must be run with lower capability set as
        // SBE engine capability set cannot anymore be modified.
        TSecurityPolicy schemaPolicy(ECapabilityWriteDeviceData);
        TSecurityPolicy writePolicy(ECapabilityWriteDeviceData);
#endif  // RD_JAVA_S60_RELEASE_9_2_ONWARDS

        TSecurityPolicy readPolicy(TSecurityPolicy::EAlwaysPass);

        securityPolicy.SetDbPolicy(RSqlSecurityPolicy::ESchemaPolicy,
                                   schemaPolicy);
        securityPolicy.SetDbPolicy(RSqlSecurityPolicy::EWritePolicy,
                                   writePolicy);
        securityPolicy.SetDbPolicy(RSqlSecurityPolicy::EReadPolicy, readPolicy);
        err = mDatabase.Create(aDbName, securityPolicy);
        securityPolicy.Close();
    }
    else
    {
        err = mDatabase.Create(aDbName);
    }

    if (KErrNone != err)
    {
        ELOG1(EJavaStorage, "Cannot create database: %d", err);

        // LastErrorMessage cannot be called as create or open
        // didn't succeeded.
        mSessionId = "Database creation error";
        mConnOpen = false;

        throw JavaStorageException(err, mSessionId.c_str(),
                                   __FILE__, __FUNCTION__, __LINE__);
    }
    mConnOpen = true;
}

void JavaDataAccessImpl::createTable(const TDesC& aStatement)
throw(JavaStorageException)
{
    TInt err = mDatabase.Exec(aStatement);

    if (err < 0)
    {
        ELOG1(EJavaStorage, "Table create failed: %d", err);
        throw JavaStorageException(
            EInvalidDataStructure,
            "Table creation failed",
            __FILE__,
            __FUNCTION__,
            __LINE__);
    }
}

void JavaDataAccessImpl::columnName(int aIndex,
                                    RSqlStatement& aStmt,
                                    wstring& aColName)
{
    TPtrC16 columnNamePtr;
    TInt err = aStmt.ColumnName(aIndex, columnNamePtr);

    if (KErrNone != err)
    {
        WLOG1(EJavaStorage, "Get ColumnName: %d", err);
    }

    // Deep copy needed.
    auto_ptr<HBufC16> colName(HBufC16::New(columnNamePtr.Length() + 1));

    if (!colName.get())
    {
        throw JavaStorageException(KErrNoMemory,
                                   "Cannot allocate column name buffer",
                                   __FILE__, __FUNCTION__, __LINE__);
    }

    TPtr16 colName2Ptr(colName->Des());
    colName2Ptr.Append(columnNamePtr);
    wstring temp(desToWstring(colName2Ptr));
    aColName = temp;
}

void JavaDataAccessImpl::columnValue(const int aIndex,
                                     RSqlStatement& aStmt,
                                     const TSqlColumnType aColumnType,
                                     wstring& aColValue)
{
    // Only INT and STRING currently supported
    if (ESqlInt == aColumnType)
    {
        TInt value = aStmt.ColumnInt(aIndex);
        aColValue = JavaCommonUtils::intToWstring(value);
    }
    else
    {
        // Deep copy needed.
        auto_ptr<HBufC16> column(HBufC16::New(aStmt.ColumnSize(aIndex) + 1));

        if (!column.get())
        {
            throw JavaStorageException(KErrNoMemory,
                                       "Cannot allocate column value buffer",
                                       __FILE__, __FUNCTION__, __LINE__);
        }

        TPtr16 columnPtr(column->Des());
        TInt colErr = aStmt.ColumnText(aIndex, columnPtr);

        wstring temp(desToWstring(columnPtr));
        aColValue = temp;
    }
}
