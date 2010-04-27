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
* Description:  JavaStorageImpl
*
*/


#include "javacommonutils.h"
#include "javaoslayer.h"
#include "javastorageimpl.h"
#include "javastorageexception.h"
#include "javastoragenames.h"
#include "javauid.h"
#include "logger.h"
#include "messagedispatcher.h"

const int CLOSED = 0;
const int OPEN = 1;

using namespace java::storage;
using namespace java::util;
using namespace std;

JavaStorageImpl::JavaStorageImpl()
        : mConnectionStatus(CLOSED), mTransactionStatus(CLOSED)
{
    JELOG2(EJavaStorage);
    mMsgDispatcher.reset(new MessageDispatcher());
}

OS_EXPORT JavaStorageImpl::~JavaStorageImpl()
{
    if (OPEN == mConnectionStatus)
    {
        close();
    }
}

OS_EXPORT void JavaStorageImpl::open() throw(JavaStorageException)
{
    open(JAVA_DATABASE_NAME);
}

OS_EXPORT void JavaStorageImpl::open(const std::string& aStorageName)
throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    if (CLOSED == mConnectionStatus)
    {
        LOG1(EJavaStorage, EInfo, "Opening connection to %s ",
             aStorageName.c_str());

        if (aStorageName == "")
        {
            throw JavaStorageException(EInvalidStorage, "Invalid storage name",
                                       __FILE__, __FUNCTION__, __LINE__);
        }

        mSessionID = mMsgDispatcher->createAndSendMessage(
                         JavaStorageMessage::EOpen, "", aStorageName);

        if (mSessionID == "")
        {
            ELOG(EJavaStorage, "No sessionID received");
            throw JavaStorageException(EInternalError, "Internal Server Error",
                                       __FILE__, __FUNCTION__, __LINE__);
        }

        LOG1(EJavaStorage, EInfo, "Connection sessionID %s ",
             mSessionID.c_str());

        mConnectionStatus = OPEN;
    }
    else
    {
        WLOG(EJavaStorage, "Connection already open");
    }
}

OS_EXPORT void JavaStorageImpl::close() throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    if (OPEN == mConnectionStatus)
    {
        string tempSessionId = mSessionID;
        mSessionID = "";    // Clean in one place.
        mConnectionStatus = CLOSED;
        mTransactionStatus = CLOSED;

        mMsgDispatcher->createAndSendMessage(JavaStorageMessage::EClose,
                                             tempSessionId,
                                             "");
    }
    else
    {
        WLOG(EJavaStorage, "Connection not open");
    }
}

OS_EXPORT void JavaStorageImpl::startTransaction() throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    if (OPEN == mConnectionStatus
            && CLOSED == mTransactionStatus)
    {
        mMsgDispatcher->createAndSendMessage(
            JavaStorageMessage::EStartTransaction, mSessionID, "");

        mTransactionStatus = OPEN;
    }
    else
    {
        WLOG(EJavaStorage, "Cannot open new transaction because \
            connection closed or already in trasaction");
    }
}

OS_EXPORT void JavaStorageImpl::commitTransaction()
throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    if (OPEN == mConnectionStatus && OPEN == mTransactionStatus)
    {
        mTransactionStatus = CLOSED;

        mMsgDispatcher->createAndSendMessage(
            JavaStorageMessage::ECommit, mSessionID, "");
    }
    else
    {
        WLOG(EJavaStorage,
             "Cannot commit because connection closed or not in trasaction");
    }
}

OS_EXPORT void JavaStorageImpl::rollbackTransaction()
throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    if (OPEN == mConnectionStatus && OPEN == mTransactionStatus)
    {
        mTransactionStatus = CLOSED;

        mMsgDispatcher->createAndSendMessage(
            JavaStorageMessage::ERollback, mSessionID, "");
    }
    else
    {
        /*
         * Transaction is reset. If connection is already closed there is no
         * possibility to recover transaction. SQLite DB will create hot journal
         * and create auto-rollback at some stage. To prevent that storage
         * server does rollback if close is called while in transaction. This
         * way server and client transactions keep in sync.
         */
        mTransactionStatus = CLOSED;
        WLOG(EJavaStorage,
             "Cannot rollback because connection closed or not in trasaction");
    }
}

OS_EXPORT void JavaStorageImpl::read(const string& aTablename,
                                     const Uid& aUid,
                                     JavaStorageApplicationEntry_t& aEntries)
throw(JavaStorageException)

{
    JELOG2(EJavaStorage);

    if (OPEN != mConnectionStatus)
    {
        throw JavaStorageException(EInvalidConnection, "No connection",
                                   __FILE__, __FUNCTION__, __LINE__);
    }

    if (aTablename == "" || aUid.toString() == L"")
    {
        throw JavaStorageException(EInvalidArgument, "Invalid argument",
                                   __FILE__, __FUNCTION__, __LINE__);
    }

    JavaStorageEntry entry;
    entry.setEntry(ID, aUid.toString());
    aEntries.insert(entry);

    mMsgDispatcher->createAndSendMessage(JavaStorageMessage::ERead,
                                         mSessionID,
                                         aTablename,
                                         aEntries);
}

OS_EXPORT void JavaStorageImpl::write(
    const string& aTablename,
    const JavaStorageApplicationEntry_t& aEntries)
throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    if (OPEN != mConnectionStatus)
    {
        throw JavaStorageException(EInvalidConnection, "No connection",
                                   __FILE__, __FUNCTION__, __LINE__);
    }

    if (aTablename == "" || aEntries.size() == 0)
    {
        throw JavaStorageException(EInvalidArgument, "Invalid argument",
                                   __FILE__, __FUNCTION__, __LINE__);
    }

    mMsgDispatcher->createAndSendMessage(JavaStorageMessage::EWrite,
                                         mSessionID,
                                         aTablename,
                                         aEntries);
}

OS_EXPORT void JavaStorageImpl::update(
    const string& aTablename,
    const JavaStorageApplicationEntry_t& aUpdateEntry,
    const JavaStorageApplicationEntry_t& aMatchEntry)
throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    if (OPEN != mConnectionStatus)
    {
        throw JavaStorageException(EInvalidConnection, "No connection",
                                   __FILE__, __FUNCTION__, __LINE__);
    }

    if (aTablename == "" || aUpdateEntry.size() < 1 || aMatchEntry.size() < 1)
    {
        throw JavaStorageException(EInvalidArgument, "Invalid argument",
                                   __FILE__, __FUNCTION__, __LINE__);
    }

    mMsgDispatcher->handleUpdate(JavaStorageMessage::EUpdate,
                                 mSessionID,
                                 aTablename,
                                 aUpdateEntry,
                                 aMatchEntry);
}

OS_EXPORT void JavaStorageImpl::search(
    const string& aTablename,
    const JavaStorageApplicationEntry_t& aSearchPattern,
    JavaStorageApplicationList_t& aApplicationList)
throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    if (OPEN != mConnectionStatus)
    {
        throw JavaStorageException(EInvalidConnection, "No connection",
                                   __FILE__, __FUNCTION__, __LINE__);
    }

    if (aTablename == "")
    {
        throw JavaStorageException(EInvalidArgument, "Invalid argument",
                                   __FILE__, __FUNCTION__, __LINE__);
    }

    mMsgDispatcher->handleSearch(JavaStorageMessage::ESearch,
                                 mSessionID,
                                 aTablename,
                                 aSearchPattern,
                                 aApplicationList);
}

OS_EXPORT int JavaStorageImpl::remove(
    const std::string& aTablename,
    const JavaStorageApplicationEntry_t& aRemoveQuery)
throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    if (aTablename == "")
    {
        throw JavaStorageException(EInvalidArgument, "Invalid argument",
                                   __FILE__, __FUNCTION__, __LINE__);
    }

    if (OPEN != mConnectionStatus)
    {
        throw JavaStorageException(EInvalidConnection, "No connection",
                                   __FILE__, __FUNCTION__, __LINE__);
    }

    return mMsgDispatcher->createAndSendMessage(JavaStorageMessage::ERemove,
            mSessionID,
            aTablename,
            aRemoveQuery);
}

OS_EXPORT void JavaStorageImpl::createTable(
    const string& aTablename,
    const JavaStorageApplicationEntry_t& aTableColumns)
throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    if (OPEN != mConnectionStatus)
    {
        throw JavaStorageException(EInvalidConnection, "No connection",
                                   __FILE__, __FUNCTION__, __LINE__);
    }

    if (aTablename == "" || aTableColumns.size() < 1)
    {
        throw JavaStorageException(EInvalidArgument, "Invalid argument",
                                   __FILE__, __FUNCTION__, __LINE__);
    }

    mMsgDispatcher->createAndSendMessage(JavaStorageMessage::ECreateTable,
                                         mSessionID,
                                         aTablename,
                                         aTableColumns);
}

OS_EXPORT void JavaStorageImpl::appendTable(
    const string& aTablename,
    const JavaStorageApplicationEntry_t& aTableColumns)
throw(JavaStorageException)
{
    JELOG2(EJavaStorage);

    if (OPEN != mConnectionStatus)
    {
        throw JavaStorageException(EInvalidConnection, "No connection",
                                   __FILE__, __FUNCTION__, __LINE__);
    }

    if (aTablename == "" || aTableColumns.size() < 1)
    {
        throw JavaStorageException(EInvalidArgument, "Invalid argument",
                                   __FILE__, __FUNCTION__, __LINE__);
    }

    mMsgDispatcher->createAndSendMessage(JavaStorageMessage::EAppendTable,
                                         mSessionID,
                                         aTablename,
                                         aTableColumns);
}
