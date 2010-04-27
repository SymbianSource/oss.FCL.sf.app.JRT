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
* Description: RecordListeners
*
*/


package com.nokia.mj.impl.rms;
import java.util.Vector;
import javax.microedition.rms.*;
import com.nokia.mj.impl.rt.support.*;

/**
 * RecordListeners forwards record changed, added and deleted events
 * for registered listeners
 */
public class RecordListeners
{

    private Vector iListeners;
    private RecordStore iStore;
    private Object iSync;

    public RecordListeners(RecordStore aRecordStore, Object aSync)
    {
        iListeners = new Vector();
        iStore = aRecordStore;
        iSync = aSync;
    }

    public void addRecordListener(RecordListener aListener)
    {
        synchronized (iSync)
        {
            if (!iListeners.contains(aListener))
            {
                iListeners.addElement(aListener);
            }
        }
    }

    public void removeRecordListener(RecordListener aListener)
    {
        synchronized (iSync)
        {
            if (iListeners.contains(aListener))
            {
                iListeners.removeElement(aListener);
            }
        }
    }

    public void recordAdded(int aRecordId)
    {
        synchronized (iSync)
        {
            for (int i = 0; i < iListeners.size(); i++)
            {
                RecordListener listener = (RecordListener)iListeners.elementAt(i);
                listener.recordAdded(iStore, aRecordId);
            }
        }
    }

    public void recordDeleted(int aRecordId)
    {
        synchronized (iSync)
        {
            for (int i = 0; i < iListeners.size(); i++)
            {
                RecordListener listener = (RecordListener)iListeners.elementAt(i);
                listener.recordDeleted(iStore, aRecordId);
            }
        }
    }

    public void recordChanged(int aRecordId)
    {
        synchronized (iSync)
        {
            for (int i = 0; i < iListeners.size(); i++)
            {
                RecordListener listener = (RecordListener)iListeners.elementAt(i);
                listener.recordChanged(iStore, aRecordId);
            }
        }
    }

    public void removeAllListeners()
    {
        synchronized (iSync)
        {
            iListeners.removeAllElements();
        }
    }

}

