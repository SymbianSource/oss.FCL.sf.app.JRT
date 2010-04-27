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
* Description:
*
*/


package com.nokia.mj.impl.storage;

import com.nokia.mj.impl.utils.Uid;

/**
 * StorageSession provides access to Java platform data. Through this API
 * Java platform related data can be read and manipulated. It provides
 * transactions to enable integrity. However all operations can be executed
 * without transactions but integrity is not provided that way.
 *
 * API is single threaded i.e. same session cannot be shared between threads
 * or processes. If the session is used in multithreaded system each thread
 * must have own storage session.
 */
public interface StorageSession
{
    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Open storage connection. If storage does not exists creates it.
     * API is single threaded i.e. same session cannot be shared between threads
     * or processes. If the session is used in multithreaded system each thread
     * must have own storage session.
     *
     * @param aStorageName Full path to storage if not predefined storage used
     *                     otherwise storage name.
     * @throws StorageException if open fails or invalid storage name.
     */
    public void open(String aStorageName) throws StorageException;

    /**
     * Open session to default storage.
     * API is single threaded i.e. same session cannot be shared between threads
     * or processes. If the session is used in multithreaded system each thread
     * must have own storage session.
     *
     * @throws StorageException if open fails.
     */
    public void open() throws StorageException;

    /**
     * Close session connection. If transaction is open it will be rollbacked.
     *
     * @throws StorageException if closing fails.
     */
    public void close() throws StorageException;

    /**
     * Free reserved resources. This must be called before StorageSession
     * object goes out of the context.
     */
    public void destroySession();

    /**
     * Start transaction. Transactions must be as short as possible to prevent
     * other clients starving. Transaction must be started immediately before
     * storage data is altered. Other clients can perform reads from
     * storage while other client is having transaction.
     *
     * @throws StorageException if communication fails.
     */
    public void startTransaction() throws StorageException;

    /**
     * Commit transaction. After commit data is written to physical storage
     * as soon as it is possible. Changes become visible to other clients.
     *
     * @throws StorageException if communication fails.
     */
    public void commitTransaction() throws StorageException;

    /**
     * Rollback changes from storage. After rollback data is retained
     * as soon as it is possible.
     *
     * @throws StorageException if communication fails.
     */
    public void rollbackTransaction() throws StorageException;

    /**
     * Read storage entries from storage.
     * If subset of data needs to be read search must be used. This returns
     * exactly one entry.
     *
     * It is recommended to use readEntry without transaction.
     *
     * @param aTablename table where entries are read.
     * @param aUid key used to identify entries to be read.
     * @return StorageEntry entries read from storage.
     *                 Null if no entry.
     * @see StorageSession#search
     * @see StorageSession#readEntries
     * @throws StorageException if table does not exist or communication fails.
     */
    public StorageEntry readEntry(String aTablename, Uid aUid)
    throws StorageException;

    /**
     * Read all entries having same UID.
     *
     * It is recommended to use readEntries without transaction.
     *
     * @return array of storage entries. Null if no entries.
     * @throws StorageException if table does not exist or communication fails.
     */
    public StorageEntry[] readEntries(String aTablename, Uid aUid)
    throws StorageException;

    /**
     * Search entries from storage.
     * Query is used to search entries based on attributes. All attributes of
     * the maching entries are returned.<br>
     * Example1: Query can be used to search application identifier (UID) based on
     * application name and vendor pair. For this case StorageEntry must define
     * name, vendor and their values.<br>
     * Example2: Query can be used to search all applications that are bound to
     * specific security domain. For this case StorateEntry must define only
     * attribute names without their values.<br>
     *
     * To define which parameters are returned they must be added to query
     * without value.
     *
     * It is recommended to use readEntry without transaction.
     *
     * @param aTablename table where entries are searched.
     * @param aSearchQuery to be used in search.
     * @return array containing all matching StorageEntry entries.
     *         Null if none.
     * @throws StorageException if table does not exist, communication fails or
     *         invalid search query.
     */
    public StorageEntry[] search(String aTablename, StorageEntry aSearchQuery)
    throws StorageException;

    /**
     * Write storage entry. Data can be written with or without transaction.
     * If write is executed without transaction storage will do autocommit and
     * data is visible to all clients after performing write call.
     * If transaction is used data becomes visible to other clients after
     * committing transaction.
     *
     * @param aTablename table where entry is written.
     * @param aEntry to be written.
     * @see StorageSession#update
     * @throws StorageException if table does not exist, communication fails,
     *         invalid entry or entry exists.
     */
    public void write(String aTablename, StorageEntry aEntry)
    throws StorageException;

    /**
     * Update storage entry. If application has already entry in the storage
     * table update must be used instead of write. Update entry contains
     * StorageAttributes to be updated whereas match entry defines entries
     * to be updated.
     *
     * Data can be updated with or without transaction. If update is executed
     * without transaction storage will do autocommit and data is visible
     * to all clients after performing the call. If transaction is used data
     * becomes visible to other clients after committing transaction.
     *
     * @param aTablename table where entry is updated.
     * @param aUpdateEntry updated attributes.
     * @param aMatchEntry used to match entries to be updated.
     * @see StorageSession#write
     * @throws StorageException if table does not exist, communication fails or
     *         invalid entry.
     */
    public void update(String aTablename,
                       StorageEntry aUpdateEntry,
                       StorageEntry aMatchEntry)
    throws StorageException;

    /**
     * Remove entries from storage. All entries by given Uid are removed
     * from given table. If one column must be deleted instead of row update
     * must be done. StorageAttribute type must be null and type also set as
     * NULL_TYPE.
     *
     * Data can be removed with or without transaction. If remove is executed
     * without transaction storage will do autocommit and removal is visible
     * to all clients after performing the call. If transaction is used removal
     * becomes visible to other clients after committing transaction.
     *
     * @param aTablename table where entries are removed.
     * @param aUid key used to identify entries to be removed.
     * @return number of removed entries. -1 if problems occurred.
     * @throws StorageException if table does not exist, communication fails or
     *         invalid entry.
     */
    public int remove(String aTablename, Uid aUid)
    throws StorageException;

    /**
     * Remove entries from storage. All entries matching to StorageEntry are
     * removed from given table. Whole table contents can be removed by
     * calling remove with empty StorageEntry.
     *
     * Data can be removed with or without transaction. If remove is executed
     * without transaction storage will do autocommit and removal is visible
     * to all clients after performing the call. If transaction is used removal
     * becomes visible to other clients after committing transaction.
     *
     * @param aTablename table where entries are removed.
     * @param aEntry used to search removed entries.
     * @return number of removed entries. -1 if problems occurred.
     * @throws StorageException if table does not exist, communication fails or
     *         invalid entry.
     */
    public int remove(String aTablename, StorageEntry aEntry)
    throws StorageException;

    /**
     * Create storage table. PrimaryKey is created automatically by the
     * first StorageAttribute of the StorageEntry. If StorageAttribute
     * type is not set column type is defaultly set as VARCHAR.
     *
     * @param aTablename table name to be created.
     * @param aTableColumns table structure to be created.
     * @param aPrimaryKey primary key for the table. Leave null
     *                    if not needed.
     * @throws StorageException if communication fails or
     *         table structure is invalid.
     */
    public void createTable(String aTablename,
                            StorageEntry aTableColumns,
                            StorageAttribute aPrimaryKey)
    throws StorageException;

    /**
     * Append column to existing storage table. If StorageAttribute
     * type is not defined column is marked as default ie. VARCHAR.
     *
     * @param aTablename table name.
     * @param aTableColumns table structure to be appended.
     * @throws StorageException if table does not exist, communication fails or
     *         appended table structure is invalid.
     */
    public void appendTable(String aTablename, StorageEntry aTableColumns)
    throws StorageException;

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */
}
