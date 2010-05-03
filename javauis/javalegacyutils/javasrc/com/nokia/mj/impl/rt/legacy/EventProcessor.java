/*
* Copyright (c) 2001 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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

import com.nokia.mj.impl.rt.support.Jvm;

/**
 * A class for dispatcher thread handling.
 */
class EventProcessor extends Thread
{

    /**
     * The Event dispatcher thread.
     */
    private static EventProcessor mEventProcessor;

    /**
     * The Notify dispatcher thread.
     */
    private static EventProcessor mNotifyProcessor;

    /**
     * The thread id. 0=event dispatcher, 1= notify dispatcher.
     */
    private int mThreadId;

    static
    {
        Jvm.loadSystemLibrary("javalegacyutils");
    }

    /**
     * Create the dispatcher threads if don't exist.
     */
    static synchronized void startEventProcessing()
    {
        if (mEventProcessor == null)
        {
            mEventProcessor = new EventProcessor(0);
            mEventProcessor.start();
            mNotifyProcessor = new EventProcessor(1);
            mNotifyProcessor.start();
        }
    }

    private EventProcessor(int threadId)
    {
        super(null, "EventProcessor");
        mThreadId = threadId;
    }


    /**
     * The dispatcher threads makes only upcalls from Java.
     */
    public void run()
    {
        _dispatchCallbacks(mThreadId);
    }

    /**
     * Called to exit the dispatcher threads. Interrupts the threads
     */
    static void exitThreads()
    {
        _exitThreads();
    }


    // Native method to cleanup the native event server stuff
    private static native void _dispatchCallbacks(int threadId);
    private static native void _exitThreads();
}
// eof
