/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Rms
*
*/


package com.nokia.mj.impl.rms;
import javax.microedition.rms.*;
import com.nokia.mj.impl.utils.Logger;

/**
 * Rms provides most of the functionality required by RecordStore
 */
public class Rms implements TransactionQueueListener
{
    private SQLiteConnection iCon;
    private RecordsTable iRecords;
    private InfoTable iInfo;
    private TransactionQueue iTransaction;
    private RecordListeners iListeners;
    private RmsName iName;
    private String iFilename;
    private RmsFileInfo iFile;
    private NotifyQueue iNotifier;
    private RecordIdCache iCache;
    private Object iSync;

    private int iAuthMode = RecordStore.AUTHMODE_PRIVATE;
    private boolean iIsWriteable = false;

    private static RecordStoreObserver iObserver = new RecordStoreObserver();

    public Rms(RecordListeners aListeners, Object aSync)
    {
        iCon = new SQLiteConnection();
        iRecords = new RecordsTable(iCon);
        iInfo = new InfoTable(iCon);
        iTransaction = new TransactionQueue(iCon, this, aSync);
        iListeners = aListeners;
        iSync = aSync;
    }

    public void open(RecordStoreOpenArgs aArgs)
    throws RecordStoreNotFoundException, RecordStoreException
    {
        synchronized (iSync)
        {
            iName = aArgs.getRecordStoreName();
            if (!aArgs.getCreateFlag() && !RmsFileManager.exists(iName))
            {
                Logger.ILOG(Logger.EMidpRms, "open() failed, not found:" +iName);
                throw new RecordStoreNotFoundException(RmsErrorStrings.NOT_FOUND + iName);
            }

            iFilename = RmsFileManager.getFilename(iName);
            iCon.open(iFilename);
            iFile = new RmsFileInfo(iFilename);
            iNotifier = new NotifyQueue(iFilename, iObserver);
            iCache = new RecordIdCache(this);

            if (!isDbInitialized())
            {
                initDb(aArgs);
            }

            getAccessInfo();
            if (!allowAccess())
            {
                close();
                Logger.ILOG(Logger.EMidpRms, "open() failed, not allowed:" + iName);
                throw new SecurityException(RmsErrorStrings.OPEN_FAILED + iName);
            }
            iObserver.addObserver(iFilename, iListeners);
        }
    }

    private boolean isDbInitialized() throws RecordStoreException
    {
        return iInfo.tableExists();
    }

    private void initDb(RecordStoreOpenArgs aArgs) throws RecordStoreException
    {
        try
        {
            iTransaction.begin();
            iRecords.create();
            iInfo.create(aArgs);
            iTransaction.commit();
        }
        catch (RecordStoreException e)
        {
            Logger.ELOG(Logger.EMidpRms, "initDb() failed", e);
            iTransaction.rollback();
            throw e;
        }
        iTransaction.commitQueuedTransactions();
    }

    public void close() throws RecordStoreNotOpenException, RecordStoreException
    {
        synchronized (iSync)
        {
            ensureOpen();
            iTransaction.close();
            iObserver.removeObserver(iFilename);
            iListeners.removeAllListeners();
            iRecords.close();
            iInfo.close();
            iCon.close();
        }
    }

    public int addRecord(Record aRecord)
    throws RecordStoreNotOpenException, RecordStoreException, RecordStoreFullException
    {
        synchronized (iSync)
        {
            ensureOpen();
            ensureWriteable();
            ensureFreeSpace(aRecord.size());
            try
            {
                iTransaction.begin();
                int id = iRecords.addRecord(aRecord);
                iInfo.updateModifiedAndNextRecordId(id+1);
                iTransaction.commit();
                iListeners.recordAdded(id);
                iNotifier.notifyRecordAdded(id);
                return id;
            }
            catch (RecordStoreException e)
            {
                Logger.ELOG(Logger.EMidpRms, "addRecord() failed", e);
                iTransaction.rollback();
                throw e;
            }
        }
    }

    public Record getRecord(Record aRecord)
    throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException
    {
        synchronized (iSync)
        {
            ensureOpen();
            return iRecords.getRecord(aRecord);
        }
    }

    public void setRecord(Record aRecord)
    throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException,
                RecordStoreFullException
    {
        synchronized (iSync)
        {
            ensureOpen();
            ensureWriteable();
            Record r = getRecord(aRecord);
            long spaceNeeded = aRecord.size() - r.size();
            ensureFreeSpace(spaceNeeded);
            if (aRecord.keepExistingTag())
            {
                aRecord.setTag(r.getTag());
            }
            try
            {
                iTransaction.begin();
                iRecords.setRecord(aRecord);
                iInfo.updateModified();
                iTransaction.commit();
                int id = aRecord.getId();
                iListeners.recordChanged(id);
                iNotifier.notifyRecordChanged(id);
            }
            catch (RecordStoreException e)
            {
                Logger.ELOG(Logger.EMidpRms, "setRecord() failed", e);
                iTransaction.rollback();
                throw e;
            }
        }
    }

    public void deleteRecord(Record aRecord)
    throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException
    {
        synchronized (iSync)
        {
            ensureOpen();
            ensureWriteable();
            // Ensure that recordId is valid. This is done because then
            // TransactionQueue does not make unneccessary rollback,
            // which would lose all uncommitted data in the queue.
            getRecord(aRecord);
            try
            {
                iTransaction.begin();
                iRecords.deleteRecord(aRecord);
                iInfo.updateModified();
                iTransaction.commit();
                int id = aRecord.getId();
                iListeners.recordDeleted(id);
                iNotifier.notifyRecordDeleted(id);
            }
            catch (RecordStoreException e)
            {
                if (!(e instanceof InvalidRecordIDException))
                {
                    Logger.ELOG(Logger.EMidpRms, "deleteRecord() failed", e);
                }
                iTransaction.rollback();
                throw e;
            }
        }
    }

    public RmsInfo getInfo()
    throws RecordStoreNotOpenException, RecordStoreException
    {
        synchronized (iSync)
        {
            ensureOpen();
            RmsInfo info = iInfo.getInfo();
            return info;
        }
    }

    public int getNumRecords()
    throws RecordStoreNotOpenException, RecordStoreException
    {
        synchronized (iSync)
        {
            ensureOpen();
            int recordCount = iRecords.getNumRecords();
            return recordCount;
        }
    }


    public void setMode(int aAuthmode, boolean aWritable)
    throws RecordStoreException
    {
        synchronized (iSync)
        {
            ensureOpen();
            ensureIsOwningSuite();
            ensureIsOnlyUser();
            iInfo.updateMode(aAuthmode, aWritable);
        }
    }

    public void addRecordListener(RecordListener aListener)
    {
        synchronized (iSync)
        {
            if (isOpen() && aListener != null)
            {
                iListeners.addRecordListener(aListener);
            }
        }
    }

    public void removeRecordListener(RecordListener aListener)
    {
        synchronized (iSync)
        {
            iListeners.removeRecordListener(aListener);
        }
    }

    public long getSize() throws RecordStoreNotOpenException
    {
        synchronized (iSync)
        {
            ensureOpen();
            // ensure that db size reflects current situation
            iTransaction.commitQueuedTransactions();
            return iFile.size();
        }
    }

    public long getSizeAvailable() throws RecordStoreNotOpenException
    {
        synchronized (iSync)
        {
            ensureOpen();
            // ensure that db size reflects current situation
            iTransaction.commitQueuedTransactions();
            return iFile.availableSize();
        }
    }

    private void ensureFreeSpace(long aRequiredSize)
    throws RecordStoreFullException, RecordStoreNotOpenException
    {
        long availableSize = iFile.availableSize();
        if (availableSize < aRequiredSize)
        {
            Logger.WLOG(Logger.EMidpRms, "ensureFreeSpace() full");
            throw new RecordStoreFullException(RmsErrorStrings.FULL);
        }
    }

    public void ensureOpen() throws RecordStoreNotOpenException
    {
        synchronized (iSync)
        {
            if (!isOpen())
            {
                Logger.ILOG(Logger.EMidpRms, "ensureOpen() not open: " + iName);
                throw new RecordStoreNotOpenException(RmsErrorStrings.NOT_OPEN);
            }
        }
    }

    private void ensureWriteable() throws SecurityException
    {
        if (!iIsWriteable)
        {
            Logger.ILOG(Logger.EMidpRms, "ensureWriteable() false: " + iName);
            throw new SecurityException(RmsErrorStrings.NOT_ALLOWED);
        }
    }

    private void ensureIsOwningSuite() throws SecurityException
    {
        if (!iName.isHomeSuite())
        {
            Logger.ILOG(Logger.EMidpRms, "ensureIsOwningSuite() false: " + iName);
            throw new SecurityException(RmsErrorStrings.NOT_ALLOWED);
        }
    }

    private void ensureIsOnlyUser() throws IllegalStateException
    {
        int storeUsers = iObserver.getObserverCount(iFilename);
        if (storeUsers > 1)
        {
            // some other MIDlet has this store open also
            Logger.ILOG(Logger.EMidpRms, "ensureIsOnlyUser() false: " + iName);
            throw new IllegalStateException();
        }
    }

    private boolean isOpen()
    {
        return iCon.isOpen();
    }

    public int[] getRecordIds() throws RecordStoreNotOpenException, RecordStoreException
    {
        synchronized (iSync)
        {
            ensureOpen();
            int[] ids = iCache.getRecordIds();
            if (ids == null)
            {
                ids = iRecords.getRecordIds();
                iCache.saveRecordIds(ids);
            }
            return ids;
        }
    }

    private void getAccessInfo() throws RecordStoreNotOpenException, RecordStoreException
    {
        RmsInfo info = getInfo();
        iAuthMode = info.getAuthMode();
        if (iName.isHomeSuite())
        {
            iIsWriteable = true; // stores in own suite are always writeable
        }
        else
        {
            iIsWriteable = info.isWriteable();
        }
    }

    private boolean allowAccess()
    {
        if (!iName.isHomeSuite() && iAuthMode == RecordStore.AUTHMODE_PRIVATE)
        {
            return false;
        }
        return true;
    }

    /**
     * This method is called when all queued transactions are committed.
     * This information is used as a trigger for performance optimisations, like
     * file size is fetched only when db is up to date.
     */
    public void allCommitted()
    {
        iFile.refreshData();
        iNotifier.dispatchAll();
    }

}

