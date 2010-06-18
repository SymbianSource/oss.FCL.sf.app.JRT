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
* Description: Semaphore util for multihreaded programming.
*
*/


package com.nokia.mj.impl.utils.concurrent;

import java.util.Vector;


/**
 * Semaphore.
 *
 * On creation the fairness is selected. Unfair does not guarantee which
 * waiter gets the semaphore, while fair gives semaphore in first-in
 * first-out basis.
 *
 * InterruptedExceptions are siletly ignored.
 */
public class Semaphore
{
    /** When Semaphore is fair, the list of Conditions waiting for
     * semaphore. Null when Semaphore is unfair */
    private Vector mWaiters = new Vector();

    /** Synchronization lock */
    private Object mSync = new Object();

    /** Current semaphore count. When < 0, someone is waiting for the semaphore. */
    private int mCount;

    /**
     * Create the semaphore.
     * @param count initial semaphore count
     * @param isFair is the semaphore fair or unfair
     */
    public Semaphore(int count, boolean isFair)
    {
        mCount = count;

        if (isFair)
        {
            mWaiters = new Vector();
        }
        else
        {
            mWaiters = null;
        }
    }

    /**
     * Acquire the semaphore. If no semaphores are left, wait for one to be
     * released.
     */
    public void acquire()
    {
        if (mWaiters == null)
        {
            acquireUnfair();
        }
        else
        {
            acquireFair();
        }
    }


    /**
     * Release the semaphore.
     */
    public void release()
    {
        if (mWaiters == null)
        {
            releaseUnfair();
        }
        else
        {
            releaseFair();
        }
    }

    /**
     * Acquire the semaphore - fair variant.
     */
    private void acquireFair()
    {
        ConditionObject lock = null;

        synchronized (mSync)
        {
            // Get a semaphore
            if (mCount-- > 0)
            {
                // Got a semaphore, return
                return;
            }

            // No semaphores left, add to queue and wait
            // (manipulate mWaiters within sync, so that lock objects are
            // correctly and fully created and visible to other thread)
            lock = new ConditionObject();
            mWaiters.addElement(lock);
        }
        lock.await();
    }

    /**
     * Release the semaphore, fair variant.
     */
    private void releaseFair()
    {
        ConditionObject lock = null;

        // Release a semaphore
        synchronized (mSync)
        {
            if (mCount++ >= 0)
            {
                return;
            }

            // There was < 0 semaphores, so release first one waiting
            lock = (ConditionObject) mWaiters.firstElement();
            mWaiters.removeElementAt(0);
        }
        lock.signal();
    }

    /**
     * Acquire the semaphore - unfair variant.
     */
    private void acquireUnfair()
    {
        synchronized (mSync)
        {
            if (mCount-- > 0)
            {
                return;
            }
            while (true)
            {
                try
                {
                    mSync.wait();
                    break;
                }
                catch (InterruptedException ex)
                {
                    // Ignore
                }
            }
        }
    }

    /**
     * Release the semaphore - unfair variant.
     */
    private void releaseUnfair()
    {
        synchronized (mSync)
        {
            // Increase count and notify someone if someone was waiting
            if (mCount++ < 0)
            {
                mSync.notify();
            }
        }
    }
}
