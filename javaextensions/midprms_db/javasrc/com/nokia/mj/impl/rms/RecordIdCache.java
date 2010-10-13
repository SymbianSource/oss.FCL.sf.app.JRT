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
* Description: RecordIdCache
*
*/


package com.nokia.mj.impl.rms;
import javax.microedition.rms.*;

/**
 * RecordIdCache saves all record id's in the store, which is used by
 * record enumerations.
 */
class RecordIdCache implements RecordListener
{
    private int[] iRecordIds;

    public RecordIdCache(Rms aRms)
    {
        iRecordIds = new int[0];
        // cache listener must be the first RecordListener registered
        // so that the other listeners get up-to-date information
        aRms.addRecordListener(this);
    }

    public int[] getRecordIds()
    {
        if (iRecordIds.length == 0)
        {
            return null;
        }
        return iRecordIds;
    }

    public void saveRecordIds(int[] aRecordIds)
    {
        if (aRecordIds == null)
        {
            iRecordIds = new int[0];
        }
        else
        {
            iRecordIds = aRecordIds;
        }
    }

    public void recordAdded(RecordStore aRecordStore, int aRecordId)
    {
        removeRecordId(aRecordId);
        insertRecordId(aRecordId);
    }

    public void recordChanged(RecordStore aRecordStore, int aRecordId)
    {
    }

    public void recordDeleted(RecordStore aRecordStore, int aRecordId)
    {
        removeRecordId(aRecordId);
    }

    private void removeRecordId(int aRecordId)
    {
        for (int i = 0; i < iRecordIds.length; i++)
        {
            if (aRecordId == iRecordIds[i])
            {
                int[] temp = new int[iRecordIds.length - 1];
                System.arraycopy(iRecordIds, 0, temp, 0, i);
                System.arraycopy(iRecordIds, i+1, temp, i, iRecordIds.length - i - 1);
                iRecordIds = temp;
            }
        }
    }

    private void insertRecordId(int aRecordId)
    {
        int[] temp = new int[iRecordIds.length + 1];
        temp[0] = aRecordId;
        System.arraycopy(iRecordIds, 0, temp, 1, iRecordIds.length);
        iRecordIds = temp;
    }

}

