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


package com.nokia.mj.impl.rt.midp;

import java.io.OutputStream;
import java.io.DataOutputStream;
import java.io.IOException;

import com.nokia.mj.impl.fileutils.FileUtility;
import com.nokia.mj.impl.rt.support.JvmInternal;

/**
 * A class for storing the heap usage to a file after fixed time period.
 */
public class MemoryLogger extends Thread
{
    /**
     * Singleton instance of the MemoryLogger.
     */
    private static MemoryLogger     sInstance;

    // Keep the deafult values in sync with native side.
    private static final int        DEFAULT_OLD_SPACE = 64 * 1024;
    private static final int        DEFAULT_NEW_SPACE = 64 * 1024;
    private static final String     FILE_NAME = "heap";
    private static final int        MAX_OLD_SPACE = 1 * 1024 * 1024;

    /**
     * Used to stop montoring if the MIDlet is closed before the fixed timer
     * has elapsed.
     */
    private        boolean          mForcedStop = false;

    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Constructor of the MemoryLogger.
     */
    private MemoryLogger()
    {
    }

    /*** ----------------------------- PACKAGE ---------------------------- */

    /**
     * Starts the heap monitoring by creating a dedicated thread.
     */
    static void startMonitoring()
    {
        if (sInstance == null)
        {
            if (Log.mOn) Log.logI("Memory monitor start request");
            sInstance = new MemoryLogger();
            sInstance.start();
        }

    }

    /**
     * Closes the heap monitoring. If the timer has already elapsed this
     * method does nothing. If the not then the monitoring is interrupted.
     */
    static void close()
    {

        if (Log.mOn) Log.logI("Memory monitor close request");
        if (sInstance != null)
        {
            if (Log.mOn) Log.logI("  interrupting thread");
            sInstance.mForcedStop = true;
            sInstance.interrupt();
        }
    }

    /**
     * Gets the amount of bytes how much the old space heap should be expanded.
     */
    static int getOldHeapSizeToExpand()
    {
        int currentTotalMemory = (int)Runtime.getRuntime().totalMemory();
        int currentOldSpaceSize = currentTotalMemory - DEFAULT_NEW_SPACE;
        int oldSpaceFromPreviousRun = getRecordedHeapSize();

        int expandSize = 0;
        if (oldSpaceFromPreviousRun > DEFAULT_OLD_SPACE)
        {
            // Expanding it to recorded value.
            expandSize = oldSpaceFromPreviousRun - currentOldSpaceSize;
        }
        else
        {
            // Expanding it back to default value.
            expandSize = DEFAULT_OLD_SPACE - currentOldSpaceSize;
        }

        int highLimit = MAX_OLD_SPACE - currentOldSpaceSize;

        if (expandSize < 0)
        {
            expandSize = 0;
        }
        if (expandSize > highLimit)
        {
            expandSize = highLimit;
        }
        if (Log.mOn) Log.logI("getOldHeapSizeToExpand: " + expandSize);
        return expandSize;
    }

    /**
     * Reads the recorded heap size from possible previous runs. The heap size
     * is read from the 'heap' file located in the root path of the MIDlet.
     * @return heap size in bytes if available, 0 if not available.
     */
    static int getRecordedHeapSize()
    {
        // Construct the file name.
        String heapFile = ApplicationInfoImpl.getMidletInfo().getRootPath();
        heapFile += FILE_NAME;

        int heap = _readFile(heapFile);
        return heap;
    }


    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * The thread will wait for fixed time and after elapsing will write the
     * heap usage into the file. If the monitoring is interrupted, then the
     * heap result will not be stored into the file.
     */
    public void run()
    {
        try
        {
            sleep(60000);
        }
        catch (Throwable t)
        {
        }

        if (mForcedStop)
        {
            if (Log.mOn) Log.logI("MIDlet closed, no memory monitoring");
            // The MIDlet was closed before timer elapsed, so no monitoring.
            sInstance = null;
            return;
        }

        try
        {
            Runtime rt = Runtime.getRuntime();
            long freeMem = rt.freeMemory();
            long totMem = rt.totalMemory();
            int usedHeap = (int)(totMem-freeMem);

            int heapSizeToStore = usedHeap - DEFAULT_NEW_SPACE;

            if (Log.mOn) Log.logI("HeapSizeToStore: " + heapSizeToStore);

            _writeFile(ApplicationInfoImpl.getMidletInfo().getRootPath()
                       + FILE_NAME, heapSizeToStore);
        }
        catch (Throwable th)
        {
            Log.logE("Error in MemoryLogger: ", th);
        }
        sInstance = null;
    }

    private native void _writeFile(String file, int heapSize);
    private static native int _readFile(String file);
}
