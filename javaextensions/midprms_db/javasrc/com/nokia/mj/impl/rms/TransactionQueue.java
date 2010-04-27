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
* Description: TransactionQueue
*
*/


package com.nokia.mj.impl.rms;
import javax.microedition.rms.*;
import java.util.Timer;
import java.util.TimerTask;
import com.nokia.mj.impl.utils.Logger;

/**
 * TransactionQueue collects multiple transactions before committing
 * them in single operation.
 * Transactions are collected until queue max size is reached or
 * commit timeout occurs.
 */
class TransactionQueue
{
    private Transaction iTransaction;
    private int iOpenTransactionCount = 0;
    private Timer iTimer;
    private TransactionQueueTask iTask;
    private Object iSync;
    private TransactionQueueListener iListener;

    private static final int TRANSACTION_QUEUE_SIZE = 100;
    public static final long COMMIT_TIMEOUT = 300; // milliseconds

    public TransactionQueue(SQLiteConnection aCon, TransactionQueueListener aListener, Object aSync)
    {
        iTransaction = new Transaction(aCon);
        iListener = aListener;
        iSync = aSync;
    }

    public void begin() throws RecordStoreException
    {
        synchronized (iSync)
        {
            if (iOpenTransactionCount == 0)
            {
                iTransaction.begin();
                initTimer();
                scheduleTask();
            }
            iOpenTransactionCount++;
        }
    }

    public void commit() throws RecordStoreException
    {
        synchronized (iSync)
        {
            if (iOpenTransactionCount > TRANSACTION_QUEUE_SIZE)
            {
                iTransaction.commit();
                iOpenTransactionCount = 0;
                cancelTask();
            }
        }
    }

    public void rollback() throws RecordStoreException
    {
        synchronized (iSync)
        {
            if (iOpenTransactionCount > 0)
            {
                iTransaction.rollback();
                iOpenTransactionCount = 0;
            }
        }
    }

    /**
     * Commits all operations in queue.
     * Note: caller must ensure that when this method is called there are no
     * ongoing unfinished transactions i.e. begin() is called but no commit() yet
     */
    public void commitQueuedTransactions()
    {
        synchronized (iSync)
        {
            try
            {
                if (iOpenTransactionCount > 0)
                {
                    iTransaction.commit();
                    iListener.allCommitted();
                }
                iOpenTransactionCount = 0;
                cancelTask();
            }
            catch (RecordStoreException e)
            {
                Logger.ELOG(Logger.EMidpRms, "commitQueuedTransactions() failed", e);
            }
        }
    }

    public void close()
    {
        synchronized (iSync)
        {
            commitQueuedTransactions();
            stopTimer();
        }
    }

    class TransactionQueueTask extends TimerTask
    {
        public void run()
        {
            synchronized (iSync)
            {
                // sync object ensures that there are no ongoing transactions
                // so it's safe to commit all queued transactions
                commitQueuedTransactions();
            }
        }
    }

    private void initTimer()
    {
        if (iTimer == null)
        {
            iTimer = new Timer();
        }
    }

    private void stopTimer()
    {
        if (iTimer != null)
        {
            iTimer.cancel();
        }
    }

    private void scheduleTask()
    {
        iTask = new TransactionQueueTask();
        iTimer.schedule(iTask, COMMIT_TIMEOUT);
    }

    private void cancelTask()
    {
        if (iTask != null)
        {
            iTask.cancel();
            iTask = null;
        }
    }

}

