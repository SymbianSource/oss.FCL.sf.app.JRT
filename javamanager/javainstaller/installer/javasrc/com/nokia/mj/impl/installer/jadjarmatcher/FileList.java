/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.jadjarmatcher;

import com.nokia.mj.impl.installer.utils.FileUtils;

/**
 * FileList is a holder for a list of file related information.
 *
 * @author Nokia Corporation
 * @version $Rev: 0 $
 */
public class FileList
{
    /*** ----------------------------- PUBLIC ------------------------------ */

    public String[] iFilenames = null;
    public int[] iHandles = null;
    public long[] iTimestamps = null;
    public long[] iTimeDistances = null;

    /**
     * Constructor.
     */
    private FileList()
    {
    }

    /**
     * Constructor.
     */
    public FileList(String[] aFilenames, int[] aHandles, long[] aTimestamps)
    {
        if (aFilenames == null || aTimestamps == null)
        {
            throw new NullPointerException(
                "FileList constructed with illegal null parameter");
        }
        if (aFilenames.length != aTimestamps.length ||
                (aHandles != null && aFilenames.length != aHandles.length))
        {
            throw new IllegalArgumentException(
                "FileList constructed with invalid array lengths");
        }
        iFilenames = aFilenames;
        iHandles = aHandles;
        iTimestamps = aTimestamps;
    }

    /**
     * Initialises iTimeDistances member by calculating absolute value
     * between given time and each iTimestamps member value.
     */
    public void initTimeDistances(long aTime)
    {
        if (iTimestamps == null)
        {
            return;
        }
        iTimeDistances = new long[iTimestamps.length];
        for (int i = 0; i < iTimestamps.length; i++)
        {
            if (aTime > iTimestamps[i])
            {
                iTimeDistances[i] = aTime - iTimestamps[i];
            }
            else
            {
                iTimeDistances[i] = iTimestamps[i] - aTime;
            }
        }
    }

    /**
     * Sort the member arrays to the order of increasing time distance.
     */
    public void sortByTimeDistance()
    {
        if (iTimeDistances == null)
        {
            return;
        }
        // Sort with simple in-place selection sort algorithm.
        int len = iTimeDistances.length;
        int min = 0;
        for (int i = 0; i < len - 1; i++)
        {
            min = i;
            for (int j = i + 1; j < len; j++)
            {
                if (iTimeDistances[j] < iTimeDistances[min])
                {
                    min = j;
                }
            }
            if (i != min)
            {
                swap(i, min);
            }
        }
    }

    /**
     * Sorts the member arrays so that the files which have the same
     * basename as the aStartFile will be the first in the array.
     * If the aStartFile parameter is null, method uses the first
     * file from iFilenames member as a start file.
     */
    public void sortByBasename(String aStartFile)
    {
        String startBasename = null;
        String basename = null;
        int insertPos = 0;
        String insertPosBasename = null;
        if (aStartFile == null)
        {
            startBasename = FileUtils.getBasename(iFilenames[0]);
            insertPos = 1;
        }
        else
        {
            startBasename = FileUtils.getBasename(aStartFile);
        }
        for (int i = insertPos + 1; i < iFilenames.length; i++)
        {
            insertPosBasename = FileUtils.getBasename(iFilenames[insertPos]);
            if (insertPosBasename.equals(startBasename))
            {
                // Insertion position already has the same basename.
                continue;
            }
            basename = FileUtils.getBasename(iFilenames[i]);
            if (basename.equals(startBasename))
            {
                swap(i, insertPos);
                insertPos++;
            }
        }
    }

    /**
     * Sorts the member arrays starting from aStartIndex position
     * so that the files which have the same name as the aStartFile
     * will be in the array starting from aStartIndex position.
     */
    public void sortByName(String aStartFile, int aStartIndex)
    {
        String name = null;
        int insertPos = aStartIndex;
        String insertPosName = null;
        for (int i = insertPos + 1; i < iFilenames.length; i++)
        {
            insertPosName = FileUtils.getName(iFilenames[insertPos]);
            if (insertPosName.equals(aStartFile))
            {
                // Insertion position already has the same name.
                continue;
            }
            name = FileUtils.getName(iFilenames[i]);
            if (name.equals(aStartFile))
            {
                swap(i, insertPos);
                insertPos++;
            }
        }
    }

    /**
     * Returns string representation of this object.
     */
    public String toString()
    {
        StringBuffer buf = new StringBuffer();
        for (int i = 0; i < iFilenames.length; i++)
        {
            buf.append(i);
            buf.append(": file=").append(iFilenames[i]);
            if (iHandles != null)
            {
                buf.append(", handle=").append(iHandles[i]);
            }
            if (iTimestamps != null)
            {
                buf.append(", timestamp=").append(iTimestamps[i]);
            }
            if (iTimeDistances != null)
            {
                buf.append(", timedistance=").append(iTimeDistances[i]);
            }
            buf.append("\n");
        }
        return buf.toString();
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Swaps entries with indices i and j in the member arrays.
     */
    private void swap(int i, int j)
    {
        String tmpFile = iFilenames[j];
        iFilenames[j] = iFilenames[i];
        iFilenames[i] = tmpFile;
        if (iHandles != null)
        {
            int tmp = iHandles[j];
            iHandles[j] = iHandles[i];
            iHandles[i] = tmp;
        }
        if (iTimestamps != null)
        {
            long tmp = iTimestamps[j];
            iTimestamps[j] = iTimestamps[i];
            iTimestamps[i] = tmp;
        }
        if (iTimeDistances != null)
        {
            long tmp = iTimeDistances[j];
            iTimeDistances[j] = iTimeDistances[i];
            iTimeDistances[i] = tmp;
        }
    }

    /*** ----------------------------- NATIVE ----------------------------- */
}
