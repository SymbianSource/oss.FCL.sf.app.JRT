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
* Description:
*
*/


package com.nokia.mj.impl.rms;

import javax.microedition.rms.*;
import com.nokia.mj.impl.rms.*;
import java.util.Vector;
import com.nokia.mj.impl.utils.Logger;

/**
 * RecordEnumerationImpl provides bidirectional access to the records in the
 * record store.
 */
public class RecordEnumerationImpl implements RecordEnumeration, RecordListener
{
    private Rms iRms;
    private Object iSync;
    private RecordFilter iRecordFilter;
    private RecordComparator iRecordComparator;
    private boolean iKeepUpdated;
    private int[] iTags;

    private int[] iRecordIDs;
    private int iCurrentPos;

    private static final int INVALID_POS = -1;

    public RecordEnumerationImpl(Rms aRms, Object aSync, RecordFilter aFilter,
                                 RecordComparator aComparator, boolean aKeepUpdated, int[] aTags)
    throws RecordStoreNotOpenException
    {
        iRms = aRms;
        iSync = aSync;
        iRecordFilter = aFilter;
        iRecordComparator = aComparator;
        iTags = makeCopy(aTags);

        iRms.ensureOpen();
        keepUpdated(aKeepUpdated);
        reset();
        rebuild();
    }

    public int numRecords()
    {
        synchronized (iSync)
        {
            ensureNotDestroyed();
            return iRecordIDs.length;
        }
    }

    /*public in MIPD3*/ int getRecordId(int aIndex) throws IllegalArgumentException
    {
        synchronized (iSync)
        {
            try
            {
                return iRecordIDs[aIndex];
            }
            catch (ArrayIndexOutOfBoundsException e)
            {
                throw new IllegalArgumentException();
            }
        }
    }

    public byte[] nextRecord()
    throws InvalidRecordIDException, RecordStoreNotOpenException, RecordStoreException
    {
        synchronized (iSync)
        {
            ensureNotDestroyed();

            int recID = nextRecordId();
            return getRecord(recID);
        }
    }

    public int nextRecordId() throws InvalidRecordIDException
    {
        synchronized (iSync)
        {
            ensureNotDestroyed();
            iCurrentPos++;

            if (iCurrentPos >= iRecordIDs.length)
            {
                throw new InvalidRecordIDException(RmsErrorStrings.NO_NEXT_RECORD);
            }
            return iRecordIDs[iCurrentPos];
        }
    }

    public byte[] previousRecord()
    throws InvalidRecordIDException, RecordStoreNotOpenException, RecordStoreException
    {
        synchronized (iSync)
        {
            ensureNotDestroyed();
            int recID = previousRecordId();
            return getRecord(recID);
        }
    }

    public int previousRecordId() throws InvalidRecordIDException
    {
        synchronized (iSync)
        {
            ensureNotDestroyed();
            if (iCurrentPos == 0 || iRecordIDs.length == 0)
            {
                throw new InvalidRecordIDException(RmsErrorStrings.NO_NEXT_RECORD);
            }

            if (iCurrentPos == INVALID_POS)
            {
                iCurrentPos = iRecordIDs.length - 1;
            }
            else
            {
                iCurrentPos--;
            }
            return iRecordIDs[iCurrentPos];
        }
    }

    public boolean hasNextElement()
    {
        synchronized (iSync)
        {
            ensureNotDestroyed();
            return (iCurrentPos < iRecordIDs.length - 1);
        }
    }

    public boolean hasPreviousElement()
    {
        synchronized (iSync)
        {
            ensureNotDestroyed();
            return ((iRecordIDs.length != 0) && (iCurrentPos != 0));
        }
    }

    public void reset()
    {
        synchronized (iSync)
        {
            ensureNotDestroyed();
            iCurrentPos = INVALID_POS;
        }
    }

    public void rebuild()
    {
        synchronized (iSync)
        {
            ensureNotDestroyed();
            try
            {
                iRecordIDs =  iRms.getRecordIds();
            }
            catch (RecordStoreException e)
            {
                iRecordIDs = new int[0];
            }

            if (iTags != null || iRecordFilter != null)
            {
                doTagFilter();
            }

            if (iRecordComparator != null)
            {
                doSort();
            }
            reset();
        }
    }

    public void keepUpdated(boolean aKeepUpdated)
    {
        synchronized (iSync)
        {
            ensureNotDestroyed();
            iKeepUpdated = aKeepUpdated;

            if (iKeepUpdated)
            {
                rebuild();
                iRms.addRecordListener(this);
            }
            else
            {
                iRms.removeRecordListener(this);
            }
        }
    }

    public boolean isKeptUpdated()
    {
        synchronized (iSync)
        {
            ensureNotDestroyed();
            return iKeepUpdated;
        }
    }

    public void destroy()
    {
        synchronized (iSync)
        {
            ensureNotDestroyed();

            keepUpdated(false);
            iRecordFilter = null;
            iRecordComparator = null;
            iRecordIDs = null;
            iRms = null;
        }
    }

    public void recordAdded(RecordStore aRecordStore, int aRecordId)
    {
        try
        {
            Record r = getFullRecord(aRecordId);
            int tag = r.getTag();
            byte[] newRecord = r.getData();

            if ((iTags != null) && (!tagMatches(tag)))
            {
                return;
            }

            if ((iRecordFilter != null) && (!iRecordFilter.matches(newRecord)))
            {
                return;
            }

            int[] temp = new int[iRecordIDs.length + 1];
            temp[0] = aRecordId;
            System.arraycopy(iRecordIDs, 0, temp, 1, iRecordIDs.length);
            iRecordIDs = temp;

            if (iRecordComparator != null)
            {
                int i = 0;
                for (int j = 1; j < iRecordIDs.length; i++, j++)
                {
                    if (iRecordComparator.compare(newRecord, getRecord(iRecordIDs[j])) == RecordComparator.FOLLOWS)
                    {
                        iRecordIDs[i] = iRecordIDs[j];
                        iRecordIDs[j] = aRecordId;
                    }
                    else
                    {
                        break;
                    }
                }
                if ((iCurrentPos != INVALID_POS) && (i <= iCurrentPos))
                {
                    iCurrentPos++;
                }
            }
        }
        catch (RecordStoreException e)
        {
            Logger.WLOG(Logger.EMidpRms, "recordAdded() failed", e);
        }
    }

    public void recordChanged(RecordStore aRecordStore, int aRecordId)
    {
        recordDeleted(aRecordStore, aRecordId);
        recordAdded(aRecordStore, aRecordId);
    }

    public void recordDeleted(RecordStore aRecordStore, int aRecordId)
    {
        for (int i = 0; i < iRecordIDs.length; i++)
        {
            if (aRecordId == iRecordIDs[i])
            {
                int[] temp = new int[iRecordIDs.length - 1];
                System.arraycopy(iRecordIDs, 0, temp, 0, i);
                System.arraycopy(iRecordIDs, i+1, temp, i, iRecordIDs.length - i - 1);
                if ((iCurrentPos == temp.length) || (i <= iCurrentPos))
                {
                    iCurrentPos--;
                }
                iRecordIDs = temp;
            }
        }
    }

    private void ensureNotDestroyed()
    {
        if (iRms == null)
        {
            throw new IllegalStateException(RmsErrorStrings.NOT_USABLE);
        }
    }

    private void doTagFilter()
    {
        try
        {
            Vector filtered = new Vector();
            for (int i = 0; i < iRecordIDs.length; i++)
            {
                Record r = getFullRecord(iRecordIDs[i]);
                int tag = r.getTag();
                byte[] record = r.getData();
                if (tagMatches(tag) && filterMatches(record))
                {
                    filtered.addElement(new Integer(iRecordIDs[i]));
                }
            }
            iRecordIDs = vectorToIntArray(filtered);
        }
        catch (RecordStoreException noe)
        {
        }
    }

    private boolean tagMatches(int aTag)
    {
        if (iTags == null)
        {
            return true;
        }
        for (int i = 0; i < iTags.length; i++)
        {
            if (aTag == iTags[i])
            {
                return true;
            }
        }
        return false;
    }

    private boolean filterMatches(byte[] aRecord)
    {
        if (iRecordFilter == null || iRecordFilter.matches(aRecord))
        {
            return true;
        }
        return false;
    }

    private int[] vectorToIntArray(Vector aVector)
    {
        int[] array = new int[aVector.size()];
        for (int i = 0; i < array.length; i++)
        {
            Integer id = (Integer) aVector.elementAt(i);
            array[i] = id.intValue();
        }
        return array;
    }

    private void doSort()
    {
        try
        {
            quickSort(iRecordIDs, 0, iRecordIDs.length - 1);
        }
        catch (RecordStoreException e)
        {
        }
    }

    private Record getFullRecord(int aRecordId)
    throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException
    {
        Record r = iRms.getRecord(new Record(aRecordId));
        return r;
    }


    private byte[] getRecord(int aRecordId)
    throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException
    {
        Record r = getFullRecord(aRecordId);
        return r.getData();
    }

    private int[] makeCopy(int[] aArray)
    {
        int[] result = null;
        if (aArray != null)
        {
            result = new int[aArray.length];
            System.arraycopy(aArray, 0, result, 0, aArray.length);
        }
        return result;
    }

    /**
     * Quick sort algorithm uses the divide-and-conquer strategy. Below the recursion step is described:
     * 1. Pick an element, called a pivot, from the list
     * 2. Partition. Reorder the list so that all elements which are less than the pivot come
     *    before the pivot and so that all elements greater than the pivot come after it
     *    (equal values can go either way).
     * 3. Sort both parts. Apply quicksort algorithm recursively to the left and the right parts.
     */
    private void quickSort(int aArray[], int aLeft, int aRight) throws RecordStoreException
    {
        if (aLeft >= aRight)
        {
            return;
        }

        int index = partition(aArray, aLeft, aRight);
        if (aLeft < index - 1)
        {
            quickSort(aArray, aLeft, index - 1);
        }
        if (index < aRight)
        {
            quickSort(aArray, index, aRight);
        }
    }

    /**
     * Partition rearranges elements in such a way, that all elements which are lesser
     * than the pivot go to the left part of the array and all elements greater than the pivot,
     * go to the right part of the array. Values equal to the pivot can stay in any part of the array.
    */
    private int partition(int aArray[], int aLeft, int aRight) throws RecordStoreException
    {
        int i = aLeft, j = aRight;
        // Right shift used instead of division ((aLeft + aRight) / 2).
        // For details see "Computation of average could overflow"
        int pivotIndex = (aLeft + aRight) >>> 1;
        byte[] pivot = getRecord(iRecordIDs[pivotIndex]);

        while (i <= j)
        {
            while (iRecordComparator.compare(getRecord(aArray[i]), pivot) == RecordComparator.PRECEDES)
            {
                i++;
            }
            while (iRecordComparator.compare(getRecord(aArray[j]), pivot) == RecordComparator.FOLLOWS)
            {
                j--;
            }
            if (i <= j)
            {
                int tmp = aArray[i];
                aArray[i] = aArray[j];
                aArray[j] = tmp;
                i++;
                j--;
            }
        };
        return i;
    }

}
