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
* Description: Simple concurrency abstractions for multihreaded programming. 
*
*/

package com.nokia.mj.impl.utils.concurrent;

/**
 * Simple class for waiting on a condition on another thread. When the
 * condition is signalled, all threads waiting on it will become runnable.
 */
public class ConditionObject
{
    /** Synchronization object. */
    Object mSync = new Object();

    /** Condition value */
    boolean mCondition = false;
    
    /**
     * Wait until the condition has been reached. If condition has already
     * been reached, return immediately.
     */
    public void await()
    {
        synchronized (mSync)
        {
            // Loop due to possible InterruptedException
            while (!mCondition)
            {
                try
                {
                    mSync.wait();
                }
                catch (InterruptedException ex)
                {
                    // Ignore interrupts
                }
            }
        }
    }

    /**
     * Signals that the condition has been reached. All waiters will
     * continue execution, and any future await() calls return immediately.
     */
    public void signal()
    {
        synchronized (mSync)
        {
            mCondition = true;
            mSync.notifyAll();
        }
    }
}
