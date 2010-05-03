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

/**
 * JAVADOCS ARE UNDER CONSTRUCTION!!
 * A component wanting to listen for thread events implements this interface.
 * The listener is registered to the runtime process using the method
 * setThreadEventListener of the interface VmPort.
 */
public interface ThreadEventListener
{

    /**
     * Called when a new Java thread has been created.
     *
     * @param newThread The new Thread which has not been started yet
     * @param parentThread The Thread creating the new Thread
     */
    public void threadStarting(Thread newThread, Thread parentThread);

    /**
     * Called when a Java thread has died.
     *
     * @param thread The Thread which has completed.
     */
    public void threadDied(Thread thread);

    /**
     * Called when an uncaught exception occurs in some Java thread.
     *
     * @param thread The Thread which caused the exception
     * @param e The uncaught Exception
     */
    public void uncaughtException(Thread thread, Throwable e);
}
