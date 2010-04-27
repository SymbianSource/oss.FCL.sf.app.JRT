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
import com.nokia.mj.impl.utils.Logger;

/**
 * StorageSessionImpl implements Java Storage functionality.
 */
final class StorageSessionImpl implements StorageSession
{
    static final int OPEN_CONN = 1;
    static final int CLOSE_CONN = 2;
    static final int START_TRANSACTION = 3;
    static final int COMMIT = 4;
    static final int ROLLBACK = 5;
    static final int READ = 6;
    static final int WRITE = 7;
    static final int SEARCH = 8;
    static final int REMOVE = 9;
    static final int CREATE_TABLE = 10;
    static final int APPEND_TABLE = 11;
    static final int UPDATE = 12;
    static final int DESTROY_SESSION = 13;

    private MessageDispatcher iMsgDispatcher = null;
    private String iSessionID = "";

    private boolean iConnectionOpen;
    private boolean iTransactionOpen;

    /**
     * Constructor.
     *
     * @throws StorageException if connection initiation fails.
     */
    StorageSessionImpl()
    {
        iMsgDispatcher = null;
        iConnectionOpen = false;
        iTransactionOpen = false;
    }

    /*** ----------------------------- PUBLIC ----------------------------- */

    public void open() throws StorageException
    {
        open(StorageNames.JAVA_DATABASE_NAME);
    }

    public void open(String aStorageName) throws StorageException
    {
        if (iMsgDispatcher == null)
        {
            iMsgDispatcher = new MessageDispatcher();
        }

        ensureNonEmpty(aStorageName);
        assertState(iConnectionOpen, false);
        iSessionID = iMsgDispatcher.createAndSendMessage(OPEN_CONN,
                     "",
                     aStorageName);
        iConnectionOpen = true;

        Logger.LOG(Logger.EJavaStorage, Logger.EInfoHeavyLoad,
                   "SessionID: " + iSessionID);
    }

    public void close() throws StorageException
    {
        assertState(iConnectionOpen, true);
        iMsgDispatcher.createAndSendMessage(CLOSE_CONN, iSessionID, "");

        iSessionID = "";
        iConnectionOpen = false;
        // Rollback is done if connection is closed while active transaction.
        iTransactionOpen = false;
    }

    public void destroySession()
    {
        if (iMsgDispatcher != null)
        {
            try
            {
                iMsgDispatcher.createAndSendMessage(
                    DESTROY_SESSION, "", "");
            }
            catch (StorageException se)
            {
                Logger.ELOG(Logger.EJavaStorage,
                            "DestroySession failed: " + se.toString());
            }
        }

        iConnectionOpen = false;
        iTransactionOpen = false;
    }

    public void startTransaction() throws StorageException
    {
        assertState(iConnectionOpen, true);
        assertState(iTransactionOpen, false);

        iMsgDispatcher.createAndSendMessage(START_TRANSACTION, iSessionID, "");
        iTransactionOpen = true;
    }

    public void commitTransaction() throws StorageException
    {
        assertState(iConnectionOpen, true);
        assertState(iTransactionOpen, true);

        iMsgDispatcher.createAndSendMessage(COMMIT, iSessionID, "");
        iTransactionOpen = false;
    }

    public void rollbackTransaction() throws StorageException
    {
        assertState(iConnectionOpen, true);
        assertState(iTransactionOpen, true);

        iMsgDispatcher.createAndSendMessage(ROLLBACK, iSessionID, "");
        iTransactionOpen = false;
    }

    public StorageEntry readEntry(String aTablename, Uid aUid)
    {
        StorageEntry[] entries = readEntries(aTablename, aUid);

        if (entries != null && entries.length > 0)
        {
            return entries[0];
        }
        else
        {
            return null;
        }
    }

    public StorageEntry[] readEntries(String aTablename, Uid aUid)
    {
        assertState(iConnectionOpen, true);
        ensureNonEmpty(aUid);

        StorageEntry entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(StorageNames.ID,
                                                aUid.getStringValue()));

        return search(aTablename, entry);
    }

    public StorageEntry[] search(String aTablename, StorageEntry aSearchQuery)
    {
        assertState(iConnectionOpen, true);

        return iMsgDispatcher.createAndSendMessage(SEARCH,
                iSessionID,
                aTablename,
                aSearchQuery);
    }

    public void write(String aTablename, StorageEntry aEntry)
    throws StorageException
    {
        assertState(iConnectionOpen, true);

        iMsgDispatcher.createAndSendMessage(WRITE,
                                            iSessionID,
                                            aTablename,
                                            aEntry);
    }

    public void update(String aTablename,
                       StorageEntry aUpdate,
                       StorageEntry aMatch)
    {
        assertState(iConnectionOpen, true);

        iMsgDispatcher.handleUpdate(iSessionID,
                                    aTablename,
                                    aUpdate,
                                    aMatch);
    }

    public int remove(String aTablename, Uid aUid) throws StorageException
    {
        ensureNonEmpty(aUid);

        StorageAttribute attribute = new StorageAttribute(
            StorageNames.ID, aUid.getStringValue());
        StorageEntry entry = new StorageEntry();
        entry.addAttribute(attribute);

        return remove(aTablename, entry);
    }

    public int remove(String aTablename, StorageEntry aEntry)
    throws StorageException
    {
        assertState(iConnectionOpen, true);
        StorageEntry[] entries = iMsgDispatcher.createAndSendMessage(REMOVE,
                                 iSessionID,
                                 aTablename,
                                 aEntry);
        if (entries != null)
        {
            return entries.length;
        }
        else
        {
            return -1;
        }
    }

    public void createTable(String aTablename,
                            StorageEntry aTableColumns,
                            StorageAttribute aPrimaryKey)
    {
        assertState(iConnectionOpen, true);

        iMsgDispatcher.handleCreateTable(iSessionID,
                                         aTablename,
                                         aTableColumns,
                                         aPrimaryKey);
    }

    public void appendTable(String aTablename, StorageEntry aTableColumns)
    {
        assertState(iConnectionOpen, true);

        iMsgDispatcher.createAndSendMessage(APPEND_TABLE,
                                            iSessionID,
                                            aTablename,
                                            aTableColumns);
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Assert state.
     *
     * @param aState to be checked.
     * @param aExpectedState to be matched.
     * @throws StorageException if states do not match.
     */
    private void assertState(boolean aState, boolean aExpectedState)
    throws StorageException
    {
        if (aState != aExpectedState)
        {
            Logger.ELOG(Logger.EJavaStorage, "Wrong state: " + aState);
            throw new StorageException(
                "Connection or transaction at wrong state: " + aState);
        }
    }

    /**
     * Ensure value is not null and empty String.
     *
     * @param aValue to be ensured.
     * @throws StorageException if value is null or empty.
     */
    private void ensureNonEmpty(String aValue) throws StorageException
    {
        if (aValue == null || aValue.equals(""))
        {
            Logger.ELOG(Logger.EJavaStorage, "Invalid argument");
            throw new StorageException("Invalid argument");
        }
    }

    /**
     * Ensure Uid is not null and empty String.
     *
     * @param Uid to be ensured.
     * @throws StorageException if value is null or empty.
     */
    private void ensureNonEmpty(Uid aUid) throws StorageException
    {
        if (aUid == null || aUid.equals(""))
        {
            Logger.ELOG(Logger.EJavaStorage, "Invalid argument");
            throw new StorageException("Invalid argument");
        }
    }

    /*** ----------------------------- NATIVE ----------------------------- */
}
