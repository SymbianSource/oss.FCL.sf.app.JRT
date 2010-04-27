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


#ifndef JAVASTORAGEIMPL_H
#define JAVASTORAGEIMPL_H

#include <string>
#include <memory>

#include "javastorage.h"
#include "javastorageentry.h"

namespace java
{
namespace storage
{
class MessageDispatcher;

/**
 * JavaStorageImpl implements JavaStorage interface. This class takes care of
 * connection status handling.
 */
OS_NONSHARABLE_CLASS(JavaStorageImpl) : public JavaStorage
{

    friend class JavaStorage;

public:
    OS_IMPORT virtual ~JavaStorageImpl();

    OS_IMPORT virtual void open() throw(JavaStorageException);

    OS_IMPORT virtual void open(const std::string& aStorageName)
    throw(JavaStorageException);

    OS_IMPORT virtual void close() throw(JavaStorageException);

    OS_IMPORT virtual void startTransaction() throw(JavaStorageException);

    OS_IMPORT virtual void commitTransaction() throw(JavaStorageException);

    OS_IMPORT virtual void rollbackTransaction() throw(JavaStorageException);

    OS_IMPORT virtual void read(
        const std::string& aTablename,
        const java::util::Uid& aUid,
        JavaStorageApplicationEntry_t& aEntries) throw(JavaStorageException);

    OS_IMPORT virtual void write(
        const std::string& aTablename,
        const JavaStorageApplicationEntry_t& aEntries
    ) throw(JavaStorageException);

    OS_IMPORT virtual void update(
        const std::string& aTablename,
        const JavaStorageApplicationEntry_t& aUpdateEntry,
        const JavaStorageApplicationEntry_t& aMatchEntry
    ) throw(JavaStorageException);

    OS_IMPORT virtual void search(
        const std::string& aTablename,
        const JavaStorageApplicationEntry_t& aSearchPattern,
        JavaStorageApplicationList_t& aApplicationList
    ) throw(JavaStorageException);

    OS_IMPORT virtual int remove(
        const std::string& aTablename,
        const JavaStorageApplicationEntry_t& aRemoveQuery
    ) throw(JavaStorageException);

    OS_IMPORT virtual void createTable(
        const std::string& aTablename,
        const JavaStorageApplicationEntry_t& aTableColumns
    ) throw(JavaStorageException);

    OS_IMPORT virtual void appendTable(
        const std::string& aTablename,
        const JavaStorageApplicationEntry_t& aTableColumns
    ) throw(JavaStorageException);

private:
    JavaStorageImpl();

private:
    std::auto_ptr<MessageDispatcher> mMsgDispatcher;
    std::string mSessionID;
    int mConnectionStatus;
    int mTransactionStatus;
};

} // end namespace storage
} // end namespace java

#endif // JAVASTORAGEIMPL_H

