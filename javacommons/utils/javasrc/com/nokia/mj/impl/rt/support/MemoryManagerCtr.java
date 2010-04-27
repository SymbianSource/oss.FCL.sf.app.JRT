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

package com.nokia.mj.impl.rt.support;

import com.nokia.mj.impl.utils.Logger;

/**
 * MemoryManagerCtr is a class which can be used to try to keep number of
 * allocated objects in defined limits. This is handy in such objects that
 * consume very little Java heap, but consumes fair big amount of native
 * resources. In some cases (TCK, benchmarks) objects are allocated in a tight
 * loop which can cause out of memory situations.
 * <p>
 * The user must define two thresholds of object counts: the lower limit
 * defines a threshold when a young GC (if supported by the VM) should be
 * triggered. The upper limit defines a threshold when a full GC should be
 * triggered. If VM does not support young GC, then full GC is triggered
 * also when young GC threshold is exceeded. GC triggering happens
 * upon every instantiation of a new object when the number of objects
 * exceeds the defined limits.
 * <p>
 * A static instance of the memory manager is created per use case - which is
 * normally one problematic class. There can be more than one instances of
 * memory managers. When the user wants to increase instance count it should
 * get a new memory token using {@link #setSize} method and keep the reference
 * to memory token active as long as the problematic object is active. When a
 * memory token is created it will increse the object count of the associated
 * memory manager. If a threshold is exceeded, the memory manager will trigger
 * either young GC or full GC and asks the JVM to run finalizers.
 * <p>
 * When the problematic object is GC'ed also the memory token will be GC'ed.
 * Once the Memory token is finalized the token will decrease the amount of
 * allocated objects.
 * <p>
 * <pre>
 *
 * import com.nokia.mj.impl.rt.support.MemoryManagerCtr;
 *
 * class GraphicsObject
 * {
 *     private static MemoryManagerCtr mMemoryManager = new
                                 MemoryManagerCtr(20, 40);
 *     private Object mMemoryToken = mMemoryManager.getMemoryToken();
 *
 *     public GraphicsObject()
 *     {
 *         // ...
 *     }
 * }
 * </pre>
 * <p>
 * Note: There should be one static memory manager per problematic case/class.
 * There can be more than one instances of memory managers.
 */

public class MemoryManagerCtr
{

    /** A count of created objects.*/
    private int mInstanceCount = 0;

    /** A Threshold when a young GC should start.*/
    private int mYoungGcThreshold = 0;

    /** A Threshold when a full GC should start.*/
    private int mFullGcThreshold = 0;

    /** Tells if collection is going on */
    private boolean mCollectionOngoing = false;

    /** Should we trace operations.*/
    private boolean mTraceEnabled = false;


    /**
     * Creates a new counter based manager with given limits.
     *
     * @param youngGcThreshold number of objects when the young GC should
     *        be started to be used.
     * @param fullGcThreshold number of objects when the full GC should
     *        be started to be used.
     */
    public MemoryManagerCtr(int youngGcThreshold, int fullGcThreshold)
    {
        mYoungGcThreshold = youngGcThreshold;
        mFullGcThreshold = fullGcThreshold;
    }

    /**
     * Creates a new counter based memory token and associates it to the
     * memory manager. It will increase the object count of the memory manager
     * by one. The user should store the token as long the problematic object
     * is alive. Normally this is achived by defining the token as a member
     * variable and ensure that it is always instantiated when the problematic
     * object is created.
     * <p>
     * Once the token is finalized it will decrease the object count of the
     * memory manager.
     * @return A memory token.
     */
    public Object getMemoryToken()
    {
        return new MemoryToken();
    }

    /**
     * Informs the manager about the addition of one token.
     */
    private void incCtr()
    {
        // Info whether to run young GC, full GC or no GC at all.
        Boolean runYoungGc = null;
        synchronized (this)
        {
            mInstanceCount++;

            // Some other thread is doing the collection.
            if (mCollectionOngoing)
            {
                return;
            }

            if (mInstanceCount >= mYoungGcThreshold)
            {
                // Mark that we will request GC.
                mCollectionOngoing = true;

                traceString("Threshold exceeded: " + mInstanceCount);
                if (mInstanceCount < mFullGcThreshold)
                {
                    // Mark that we will request young GC.
                    runYoungGc = new Boolean(true);
                }
                else
                {
                    // Mark that we will request full GC.
                    runYoungGc = new Boolean(false);
                }
            }
        }

        // Do we need to run GC at all.
        if (runYoungGc != null)
        {
            if (runYoungGc.booleanValue() == true)
            {
                // Run fast collection if supported.
                traceString("Asking YoungGc");
                if (!JvmInternal.runYoungGenerationGc())
                {
                    // Run full collection.
                    traceString("Asking fullGc - youngGc not supported");
                    System.gc();
                }
            }
            else
            {
                // Run full collection.
                traceString("Asking fullGc");
                System.gc();
            }

            traceString("Asking finalization");
            // Ask the JVM to run finalizers.
            JvmInternal.runFinalization();

            synchronized (this)
            {
                // Mark that GC request has been sent.
                mCollectionOngoing = false;
            }
        }
    }

    /**
     * Informs the manager about finalization of one token.
     */
    private synchronized void decCtr()
    {
        mInstanceCount--;
    }

    private void traceString(String s)
    {
        if (mTraceEnabled)
        {
            Logger.WLOG(Logger.EUtils, "MemoryManagerCtr: " + s);
        }
    }

    /**
     * A class for increasing an decreasing the count of unfinalized
     * objects.
     */
    private class MemoryToken
    {
        /** Enables finalization */
        private Finalizer finalizer = new Finalizer()
        {
            public void finalizeImpl()
            {
                decCtr();
            }
        };

        /**
         * Creates a new counter based token.
         */
        private MemoryToken()
        {
            incCtr();
        }
    }

}
