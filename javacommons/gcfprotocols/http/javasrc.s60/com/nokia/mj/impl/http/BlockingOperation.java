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


package com.nokia.mj.impl.http;

/**
 * BlockingOperation is a generic class designed to handle Symbian OS
 * asynchronous methods, where a wait() and a callback to Java are usually
 * required. It can contain an integer as result of the operation. The provided
 * lock Object allows for keeping several threads from executing the same
 * operation on the same object at the same time.
 *
 * @see com.symbian.gcf.ConnectionEndPoint for examples of how to use
 *      BlockingOperation.
 */
public final class BlockingOperation
{
    /**
     * BLOCKED is set to the maximum integer value so that the number of bytes
     * received can be returned, or BLOCKED, or an error code. The number of
     * bytes received is unlikely to ever be the value of Integer.MAX_VALUE.
     */
    public static final int BLOCKED = Integer.MAX_VALUE;

    public static final int COMPLETED = 1;

    private final Object iThreadBlocker = new Object();

    private int iResult = 0;

    public BlockingOperation()
    {
    }

    public Object getLock()
    {
        return iThreadBlocker;
    }

    public synchronized void waitForCompletion()
    {
        while (true)
        {
            try
            {
                wait();
                return;
            }
            catch (InterruptedException ex)
            {
                // ignore and continue
                ;
            }
        }
    }

    /**
     * We must use a synchronized block here as this method could be called from
     * a native callback. Native callbacks ignore the synchronized keyword on
     * method signatures.
     */
    public void notifyCompleted(int aResult)
    {
        synchronized (this)
        {
            iResult = aResult;
            notify();
        }
    }

    public void notifyCompleted()
    {
        notifyCompleted(0);
    }

    public synchronized int getResult()
    {
        return iResult;
    }

    public synchronized void setResult(int aResult)
    {
        iResult = aResult;
    }
}
