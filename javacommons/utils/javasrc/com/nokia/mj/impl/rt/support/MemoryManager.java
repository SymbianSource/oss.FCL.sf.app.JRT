/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.rt.support;

import com.nokia.mj.impl.utils.Logger;

/**
 * Memory token based native memory estimation and management.
 *
 * @author Nokia Corporation
 * @version 1.0
 */

class MemoryManager
{
    /** Property name for window size in kilobytes */
    private static final String WINDOW_SIZE_PROPERTY_NAME =
        "com.nokia.mj.memorymanager.windowsize";

    /** Property name for trace flag */
    private static final String TRACE_PROPERTY_NAME =
        "com.nokia.mj.memorymanager.trace";

    /** Size of the memory management window, in kilobytes */
    private int windowSize = 100;

    /** Current low border of the memory management window */
    private int lowBorder;

    /** Current high border of the memory management window */
    private int highBorder;

    /** Current memory allocation size */
    private int size = 0;

    /** Tells if collection is going on */
    private boolean collectionOngoing = false;

    /** Should we trace operatios */
    private boolean trace = true;

    /**
     * Creates the memory manager, initializing internal structures.
     */
    MemoryManager()
    {
        windowSize = getInt(WINDOW_SIZE_PROPERTY_NAME, windowSize);
        trace = System.getProperty(TRACE_PROPERTY_NAME) != null;

        setWindow();
    }

    /**
     * Informs the manager about size change of an allocation.
     *
     * @param oldSize old number of bytes assigned for this token
     * @param newSize new number of bytes assigned for this token
     */
    void changeSize(int oldSize, int newSize)
    {
        int change = newSize - oldSize;

        synchronized (this)
        {
            // Apply size change
            size += change;

            if (collectionOngoing)
            {
                // Collection is ongoing - no need to do anything else
                return;
            }

            if (size > highBorder)
            {
                // Current size is over high border - collection needed
                // There is no collection ongoing - set it
                collectionOngoing = true;

                if (trace)
                {
                    traceString("collection requested, low, high, size = " +
                                lowBorder + ", " + highBorder + ", " + size);

                }
            }
            else if (size < lowBorder)
            {
                // Move window
                setWindow();
                return;
            }
        }

        // Collection has been requested
        runCollection();
    }

    /**
     * Run the memory collection. Current implementation runs the collection
     * on the same thread, as GC in any case stops the world in used JVMs.
     *
     * Also the finalizations are executed in different threads with high
     * priority, therefore there is no need to make collection in separate
     * thread for finalization either.
     */
    private void runCollection()
    {
        // Run fast collection
        JvmInternal.runYoungGenerationGc();
        JvmInternal.runFinalization();

        // Check if collected & freed enough memory
        synchronized (this)
        {
            if (size < highBorder)
            {
                // Fast collection released enough memory, end here
                collectionOngoing = false;

                if (size < lowBorder)
                {
                    // Current size even lower than low border, move window
                    setWindow();
                }
                return;
            }
        }

        // Did not release enough - try full collection
        System.gc();
        JvmInternal.runFinalization();

        // All that is relased should now have been released. We can consider
        // current memory allocation to be the new window low border.
        synchronized (this)
        {
            // Move window
            setWindow();

            // Mark collection as finished
            collectionOngoing = false;
        }
    }

    /**
     * Return system property value
     *
     * @param propertyName name string of the property
     * @param defaultValue default value to use if property does not exist or is
     *                     not an integer
     * @return property value or the default
     */
    private static int getInt(String propertyName, int defaultValue)
    {
        int value = defaultValue;
        String string = System.getProperty(propertyName);
        if (string != null)
        {
            try
            {
                value = Integer.parseInt(string);
            }
            catch (NumberFormatException  ex)
            {
                Logger.ELOG(Logger.EUtils,
                            "MemoryManager: property \"" + propertyName +
                            "\" value \"" + string + "\" is not integer",
                            ex);
            }
        }
        return value;
    }

    /**
     * Set the current window, with current size as low border.
     */
    private synchronized void setWindow()
    {
        lowBorder = size;
        highBorder = lowBorder + (windowSize * 1024);
        if (trace)
        {
            traceString("window set to low, high = " +
                        lowBorder + ", " + highBorder);
        }
    }

    private void traceString(String s)
    {
        Logger.LOG(Logger.EUtils, Logger.EInfo, "MemoryManager: " + s);
    }
}
