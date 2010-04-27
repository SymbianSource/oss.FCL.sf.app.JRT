/*
* Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.rt.legacy;

import com.nokia.mj.impl.rt.support.JvmInternal;
import com.nokia.mj.impl.rt.support.Jvm;

public class MemoryUtil
{

    static
    {
        Jvm.loadSystemLibrary("javalegacyutils");
    }


    // Two independent mechanisms are available for managing native
    // heap consumption.

    // The first mechanism checks available system heap, and if
    // beneath a configurable limit forces a garbage collection
    // followed by synchronous execution of all pending
    // finalizers. This mechanism is not used by Symbian JSRs but
    // is preferred by some licensees.

    private static final int FREE_KB_RAM_LIMIT_FOR_FINALIZATION = 4096; // S60 change

    public static void ensureMinFreeRAM()
    {
        final long freeRAM = _getFreeRAMInKB();
        if (freeRAM < FREE_KB_RAM_LIMIT_FOR_FINALIZATION)
        {
            // Attempt GC also if returned an error code (!= KErrNone)
            freeNativeMemory();
        }
    }

    // The second mechanism allows native allocation/deallocation
    // to be tracked and triggers regular garbage collections
    // after a configurable delta. This is the mechanism used by
    // Symbian JSRs since it is proactive in keeping native heap
    // allocation under control rather than allowing the
    // consumption of all system memory before taking action.

    private static final int NATIVE_ALLOCATION_GC_THRESHOLD = 2048 * 1024; // S60 change

    static final Object sNativeAllocationLock = new Object();
    static long sNativeAllocationCounter = 0;


    /**
     * Native heap management memory allocated notification.
     *
     * Called after a native memory allocation to allow
     * heap manager to track native heap usage.
     */
    public static void nativeMemoryAllocated(int aNativeBytes)
    {
        /*
         * Native heap allocation algorith: track known native allocations and
         * deletions. Trigger GC when total allocation reaches a threshold.
         * We zero the allocation count when this happens to avoid pathological
         * cases for applications that have a stable native heap use that just
         * exceeds the threshold.
         */
        boolean gc = false;
        synchronized (sNativeAllocationLock)
        {
            sNativeAllocationCounter += aNativeBytes; // S60 change
            if (sNativeAllocationCounter > NATIVE_ALLOCATION_GC_THRESHOLD)   // S60 change
            {
                sNativeAllocationCounter = 0;
                gc = true;
            }
        }
        if (gc)
        {
            freeNativeMemory(); // S60 change
        }
    }

    /**
     * Native heap management memory released notification.
     *
     * Called after native memory freed to allow heap manager
     * to track native heap usage.
     */
    public static void nativeMemoryFreed(int aNativeBytes)
    {
        /*
         * Decrement native heap counter and zero clamp.
         */
        synchronized (sNativeAllocationLock)
        {
            sNativeAllocationCounter -= aNativeBytes; // S60 change
            if (sNativeAllocationCounter < 0)
            {
                sNativeAllocationCounter = 0;
            }
        }
    }

    /*
     * Attempt to free native peers by forcing GC and finalization.
     */
    public static void freeNativeMemory()
    {
        //attempt GC also if returned an error code (!= KErrNone)
        if (JvmInternal.runYoungGenerationGc())
        {
            JvmInternal.runFinalization();

            if (_getFreeRAMInKB() >= FREE_KB_RAM_LIMIT_FOR_FINALIZATION)
            {
                return;
            }
        }
        System.gc();
        JvmInternal.runFinalization();
    }

    private static native long _getFreeRAMInKB();
}
// eof

