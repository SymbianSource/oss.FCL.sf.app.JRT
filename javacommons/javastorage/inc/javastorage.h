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
* Description:  JavaStorage
*
*/


#ifndef JAVASTORAGE_H
#define JAVASTORAGE_H

#include <stdio.h>
#include <string>
#include <set>
#include <list>

#include "javaosheaders.h"
#include "javastorageentry.h"
#include "javastorageexception.h"

namespace java
{
namespace util
{
class Uid;
} // end namespace util
namespace storage
{

typedef std::list<std::multiset<java::storage::JavaStorageEntry> > JavaStorageApplicationList_t;
typedef std::multiset<JavaStorageEntry> JavaStorageApplicationEntry_t;

/**
 * JavaStorage provides access to Java platform data. Through this API
 * Java platform related data can be read and manipulated. It provides
 * transactions to enable integrity. However all operations can be executed
 * without transactions but integrity is not provided that way.
 *
 * API is single threaded i.e. same session cannot be shared between threads
 * or processes. If the session is used in multithreaded system each thread
 * must have own storage session.
 */
class JavaStorage
{
public:

    virtual ~JavaStorage() {}

    /**
     * Create API instance.
     *
     * @return Instance to JavaStorage API. Ownership is transfered to caller.
     */
    OS_IMPORT static JavaStorage* createInstance();

    /**
     * Open storage connection. If storage does not exist creates it.
     *
     * @param aStorageName Full path to storage if not predefined storage used
     *                     otherwise storage name.
     * @throws JavaStorageException with following error codes:
     *             EInvalidStorage: Storage name is invalid.
     *             EInternalError: Session creation failed.
     *         Storage error codes if storage handling fails. Codes are
     *         dependant on underlying storage component.
     */
    virtual void open(const std::string& aStorageName)
    throw(JavaStorageException) = 0;

    /**
     * Open connection to default Java storage.
     *
     * @throws JavaStorageException with following error codes:
     *             EInvalidStorage: Storage name is invalid.
     *             EInternalError: Session creation failed.
     *         Storage error codes if storage handling fails. Codes are
     *         dependant on underlying storage component.
     */
    virtual void open() throw(JavaStorageException) = 0;

    /**
     * Close JavaStorage and free its resources.
     *
     * @throws JavaStorageException with following error codes:
     *         Storage error codes if storage handling fails. Codes are
     *         dependant on underlying storage component.
     */
    virtual void close() throw(JavaStorageException) = 0;

    /**
     * Start transaction.
     * Starting transaction aquires exlusive lock to underlying storage
     * component. It means other connections cannot alter storage data
     * until lock is released. Lock is storage not table specific. Data
     * reading is possible while lock is on.
     *
     * For this reason transaction should be as short as possible. It
     * should be considered if storage connection is used only for reading
     * that transaction is not started at all.
     *
     * @throws JavaStorageException with following error codes:
     *         Storage error codes if storage handling fails. Codes are
     *         dependant on underlying storage component.
     */
    virtual void startTransaction() throw(JavaStorageException) = 0;

    /**
     * Commit transaction. After commit data is written to physical storage
     * as soon as it is possible. Data becomes visible to other clients
     * and lock is released.
     *
     * @throws JavaStorageException with following error codes:
     *         Storage error codes if storage handling fails. Codes are
     *         dependant on underlying storage component.
     */
    virtual void commitTransaction() throw(JavaStorageException) = 0;

    /**
     * Rollback changes from storage. After rollback data is retained
     * as soon as it is possible and lock is released.
     *
     * @throws JavaStorageException with following error codes:
     *         Storage error codes if storage handling fails. Codes are
     *         dependant on underlying storage component.
     */
    virtual void rollbackTransaction() throw(JavaStorageException) = 0;

    /**
     * Read storage entries from storage. All entries belogning to Uid
     * on given table are returned.
     *
     * @param aTablename table where entries are read.
     * @param aUid used to identify entries to be read.
     * @param[out] aEntries entries container that is populated by read
     *             entries. Container is left empty if no data read.
     * @throws JavaStorageException with following error codes:
     *             EInvalidConnection: Connection is not open.
     *             EInvalidArgument: Read argument is invalid.
     *         Storage error codes if storage handling fails. Codes are
     *         dependant on underlying storage component.
     */
    virtual void read(
        const std::string& aTablename,
        const java::util::Uid& aUid,
        JavaStorageApplicationEntry_t& aEntries
    ) throw(JavaStorageException) = 0;

    /**
     * Write storage entries. Entries are written to given table and
     * they're identified by given Uid. If table contains entry sharing same
     * primary key write fails. Use update to update these entries.
     *
     * @param aTablename table where entries are written.
     * @param aEntries entries to be written.
     * @throws JavaStorageException with following error codes:
     *             EInvalidConnection: Connection is not open.
     *             EInvalidArgument: Read argument is invalid.
     *         Storage error codes if storage handling fails. Codes are
     *         dependant on underlying storage component.
     */
    virtual void write(
        const std::string& aTablename,
        const JavaStorageApplicationEntry_t& aEntries
    ) throw(JavaStorageException) = 0;

    /**
     * Update storage entries. If application has already entry in the storage
     * table update must be used instead of write.
     *
     * @param aTablename table where entries are updated.
     * @param aUpdateEntry entries to be updated.
     * @param aMatchEntry is used to identify which entries are updated. Usually
     *                    this contains only ID.
     * @throws JavaStorageException with following error codes:
     *             EInvalidConnection: Connection is not open.
     *             EInvalidArgument: If given arguments are empty.
     *         Storage error codes if storage handling fails. Codes are
     *         dependant on underlying storage component.
     */
    virtual void update(
        const std::string& aTablename,
        const JavaStorageApplicationEntry_t& aUpdateEntry,
        const JavaStorageApplicationEntry_t& aMatchEntry
    ) throw(JavaStorageException) = 0;

    /**
     * Search entries from storage.
     * Query is used to search entries based on table attributes.<br>
     *
     * Example1: It can be used to search application Uid based on
     * application name and vendor pair. This case search query defines
     * NAME and VENDOR with their values. Result contain all matching
     * applications in given table. All application entries are returned.<br>
     *
     * @code
     * JavaStorageEntry attr;
     * JavaStorageApplicationEntry_t query;
     * attr.setEntry(NAME, L"MyName"); // Match item.
     * query.insert(attr);
     * attr.setEntry(VENDOR, L"MyVendor"); // Match item.
     * query.insert(attr);
     * attr.setEntry(ID, L""); // Fecth item.
     * query.insert(attr);
     * ...
     * @endcode
     *
     * Example2: It can be used to search all applications that are bound to
     * specific security domain. This case query contains entry name
     * like SECURITY_DOMAIN with intrested security domain value. All
     * applications belogning to given security domain with all entries
     * are returned.<br>
     *
     * @code
     * JavaStorageEntry attr;
     * JavaStorageApplicationEntry_t query;
     * attr.setEntry(SECYRITY_DOMAIN, L"MyDomain"); // Match item
     * query.insert(attr);
     * ...
     * @endcode
     *
     * Example3: It can be used to read only subset of application entries.
     * For this case only subset of entries is defined like PACKAGE_NAME,
     * VENDOR and VERSION without values. To match certain
     * application for instance ID must be specified with value. Otherwise
     * all defined entries of all table applications are returned
     * without possibility to identify which application they belong to.
     *
     * @code
     * JavaStorageEntry attr;
     * JavaStorageApplicationEntry_t query;
     * attr.setEntry(ID, L""); // Fetch item.
     * query.insert(attr);
     * attr.setEntry(NAME, L""); // Fetch item.
     * query.insert(attr);
     * ...  // No match item specified, fecthing all entries.
     * @endcode
     *
     * @param aTablename table where entries are searched.
     * @param[in] aSearchQuery to mach entries.
     * @param[out] aApplicationList matching applications. List contain
     *             JavaApplicationEntry_t objects that contain
     *             JavaStorageEntries depending on search.
     * @throws JavaStorageException with following error codes:
     *             EInvalidConnection: Connection is not open.
     *             EInvalidArgument: Read argument is invalid.
     *         Storage error codes if storage handling fails. Codes are
     *         dependant on underlying storage component.
     */
    virtual void search(
        const std::string& aTablename,
        const JavaStorageApplicationEntry_t& aSearchQuery,
        JavaStorageApplicationList_t& aApplicationList
    ) throw(JavaStorageException) = 0;

    /**
     * Remove entries from storage. All information of matching entries are
     * removed from the table. <br>If part of the entries need to be ramoved
     * update must be used. Update entries as null which represents same as
     * deletion.
     *
     * @param aTablename table where entries are removed.
     * @param aRemovePattern pattern to search removed entries.
     * @throws JavaStorageException with following error codes:
     *             EInvalidConnection: Connection is not open.
     *             EInvalidArgument: Read argument is invalid.
     *             EInvalidDataStructure: Query contains invalid entry.
     *         Storage error codes if storage handling fails. Codes are
     *         dependant on underlying storage component.
     */
    virtual int remove(
        const std::string& aTablename,
        const JavaStorageApplicationEntry_t& aRemoveQuery)
    throw(JavaStorageException) = 0;

    /**
     * Create storage table.
     *
     * @param aTablename table name.
     * @param aTableColumns table structure to be created.
     * @throws JavaStorageException with following error codes:
     *             EInvalidConnection: Connection is not open.
     *             EInvalidArgument: Read argument is invalid.
     *             EInvalidDataStructure: Query contains invalid entry.
     *         Storage error codes if storage handling fails. Codes are
     *         dependant on underlying storage component.
     */
    virtual void createTable(
        const std::string& aTablename,
        const JavaStorageApplicationEntry_t& aTableColumns
    ) throw(JavaStorageException) = 0;

    /**
     * Append column to existing storage table.
     *
     * @param aTablename table name.
     * @param aTableColumns table structure to be appended.
     * @throws JavaStorageException with following error codes:
     *             EInvalidConnection: Connection is not open.
     *             EInvalidArgument: Read argument is invalid.
     *             EInvalidDataStructure: Query contains invalid entry.
     *         Storage error codes if storage handling fails. Codes are
     *         dependant on underlying storage component.
     */
    virtual void appendTable(
        const std::string& aTablename,
        const JavaStorageApplicationEntry_t& aTableColumns
    ) throw(JavaStorageException) = 0;

};

} // end namespace storage
} // end namespace java

#endif // JAVASTORAGE_H
