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
* Description: RmsFileInfo
*
*/


package com.nokia.mj.impl.rms;
import javax.microedition.rms.*;

import com.nokia.mj.impl.fileutils.*;
import com.nokia.mj.impl.utils.*;

/**
 * RmsFileInfo provides file size related information.
 * Size information is cached for performance reasons and it can be
 * updated with separate command.
 */
class RmsFileInfo
{
    private FileUtility iFile;
    private long iFileSize;
    private long iAvailableSize;
    private boolean iDataInitialized;

    private static final long MAX_RMS_SIZE = 32000000; // MB

    public RmsFileInfo(String aFilename)
    {
        iFile = new FileUtility(aFilename);
        iDataInitialized = false;
    }

    public void refreshData()
    {
        iFileSize = getSize();
        iAvailableSize = getAvailableSize();
        iDataInitialized = true;
    }

    public long size()
    {
        ensureDataInitialized();
        return iFileSize;
    }

    public long availableSize()
    {
        ensureDataInitialized();
        return iAvailableSize;
    }

    private void ensureDataInitialized()
    {
        if (!iDataInitialized)
        {
            refreshData();
        }
    }

    private long getSize()
    {
        long size = -1;
        try
        {
            size = iFile.fileSize();
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EMidpRms, "size() failed", e);
        }
        if (size < 0)
        {
            Logger.WLOG(Logger.EMidpRms, "fileSize = " + size);
            size = 0;
        }
        return size;
    }

    private long getAvailableSize()
    {
        long sizeLeft = MAX_RMS_SIZE - getSize();
        long result = 0;

        if (sizeLeft > 0)
        {
            long availableSize = getFreeSpace();
            result = Math.min(availableSize, sizeLeft);
        }
        return result;
    }

    private long getFreeSpace()
    {
        long size = -1;
        try
        {
            size = iFile.availableSize();
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EMidpRms, "availableSize() failed" , e);
        }
        if (size < 0)
        {
            Logger.WLOG(Logger.EMidpRms, "availableSize = " + size);
            size = MAX_RMS_SIZE;
        }
        return size;
    }

}

