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
* Description: NotifyQueue
*
*/


package com.nokia.mj.impl.rms;
import javax.microedition.rms.*;
import java.util.Vector;
import com.nokia.mj.impl.utils.Logger;

/**
 * NotifyQueue collects record store changes to a buffer and sends
 * changes as one burst when so instructed.
 * This ensures that record store changes are not communicated too early
 * to other suites.
 */
class NotifyQueue
{
    private Vector iNotifications;
    private String iFilename;
    private RecordStoreObserver iObserver;

    public NotifyQueue(String aFilename, RecordStoreObserver aObserver)
    {
        iNotifications = new Vector();
        iFilename = aFilename;
        iObserver = aObserver;
    }

    public void notifyRecordAdded(int aRecordId)
    {
        insertNotification(RecordStoreObserver.ADD_RECORD, aRecordId);
    }

    public void notifyRecordChanged(int aRecordId)
    {
        insertNotification(RecordStoreObserver.SET_RECORD, aRecordId);
    }

    public void notifyRecordDeleted(int aRecordId)
    {
        insertNotification(RecordStoreObserver.DELETE_RECORD, aRecordId);
    }

    public void dispatchAll()
    {
        for (int i = 0; i < iNotifications.size(); i++)
        {
            NotifyQueueItem item = (NotifyQueueItem)iNotifications.elementAt(i);
            notifyObservers(item.getOperation(), item.getRecordId());
        }
        iNotifications.removeAllElements();
    }

    private void insertNotification(int aOperation, int aRecordId)
    {
        NotifyQueueItem item = new NotifyQueueItem(aOperation, aRecordId);
        iNotifications.addElement(item);
    }

    private void notifyObservers(int aOperation, int aRecordId)
    {
        iObserver.notifyObservers(iFilename, aOperation, aRecordId);
    }

    class NotifyQueueItem
    {
        private int iRecordId;
        private int iOperation;

        public NotifyQueueItem(int aOperation, int aRecordId)
        {
            iRecordId = aRecordId;
            iOperation = aOperation;
        }

        public int getRecordId()
        {
            return iRecordId;
        }

        public int getOperation()
        {
            return iOperation;
        }
    }

}
