/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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

package javax.microedition.lcdui;

/**
 * Condition variable used to track completion of asynchronous
 * requests to the server.
 */
class AsyncCall
{
    private static final int KRequestPending = 0x80000000;

    /**
     * Limit of wait in waitForRequest().
     * 1/25s is close to human persistance of vision, so
     * users probably won't notice the glitch.
     *
     * See comment in waitForRequest().
     */
    private static final long TIMEOUT = 40; // == 1/25 second

    private int     iStatus;
    private int     iWaiting;

    public AsyncCall()
    {
    }

    public final synchronized void request()
    {
        iStatus = KRequestPending;
    }

    public final synchronized void complete(int aStatus)
    {
        iStatus = aStatus;

        if (iWaiting > 0)
        {
            notifyAll();
        }
    }

    public final boolean isPending()
    {
        return (iStatus == KRequestPending);
    }

    public final synchronized int waitForRequest()
    {
        while (isPending())
        {
            ++iWaiting;
            try
            {
                wait(TIMEOUT);
            }
            catch (InterruptedException e)
            {
                e.printStackTrace();
            }
            --iWaiting;
        }
        return iStatus;
    }
}

