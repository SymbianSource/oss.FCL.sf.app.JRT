/*******************************************************************************
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

package org.eclipse.swt.internal.qt;

import org.eclipse.swt.internal.qt.ApplicationUIListener;
import org.eclipse.swt.internal.qt.midp.UIThreadLauncher;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Shell;

/**
 * A class that manages the UI thread hand-over from the internal UI event loop
 * to the application.
 */
public final class UIThreadManager {
	private static Object lock = new Object();
	private static boolean threadWaitingForApplication;
	private static Runnable applicationRunnable;
	private static ApplicationUIListener applicationUIListener;
		
	/**
	 * Pass the control of the thread to the application by calling a Runnable
	 * provided by the application. If the Runnable has not yet been provided
	 * then the thread waits until it becomes available, or the wait is aborted
	 * by calling <code>abortWait()</code>. 
	 */
	public static void runApplicationUI() {
		synchronized(lock) {
			if(!applicationWaiting()) {
				waitForApplication();
			}
			callback();
		}
	}

	/**
	 * If the UI thread is waiting for the application to request the callback
	 * in the UI thread then calling this will abort the wait and make the UI
	 * thread to return back from the <code>runApplicationUI()</code>.
	 */
	public static void abortWait() {
		synchronized(lock) {
			lock.notify();
		}
	}

	/**
	 * Called indirectly by the application when it's requesting the UI thread. 
	 * Application will pass a Runnable as a parameter and expects it to get called
	 * in the UI thread. 
	 * 
	 * @param runnable
	 *            The Runnable to call in the UI thread.
	 * @return True if obtaining the UI thread was successful and the runnable
	 *         will be called.
	 */
	public static boolean startInUIThread(Runnable runnable) {
		synchronized(lock) {
			applicationRunnable = runnable;
			if(threadWaitingForApplication) {
				lock.notify();
			} else {
				if(!waitForUIThread()) {
					// If the internal UI has not been created then it can't be
					// signaled and we can't wait for it. This must mean there's
					// no UI thread yet in the process and we can create it here.
					if(!UIThreadLauncher.startInUIThread(runnable)) {
				    	return false;
				    }	
				}
			}
		}
		return true;
	}
	
	/**
	 * Sets the listener that will be notified of the state of the UI during its
	 * life-time.
	 */
	public static void setApplicationUIListener(ApplicationUIListener listener) {
		synchronized(lock) {
			applicationUIListener = listener;
		}
	}
	
	/*
	 * Signal the internal UI loop and wait until it passes the control of the
	 * UI thread into runApplicationUI(). Return false if the internal UI can't
	 * be signaled. 
	 */
	private static boolean waitForUIThread() {
		if(signalUIThreadRequest()) {
			try {
				lock.wait();
			} catch(InterruptedException e) {
				// Nothing to do
			}
			return true;
		}
		return false;
	}

	/*
	 * Wait until signaled by the application requesting the callback to its
	 * Runnable, or by abortWait().
	 */
	private static void waitForApplication() {
		threadWaitingForApplication = true;
		try {
			lock.wait();
		} catch(InterruptedException e) {
			// Nothing to do
		} finally {
			threadWaitingForApplication = false;
		}		
	}
	
	/*
	 * Returns true if application has provided the Runnable and is thus waiting
	 * for the callback.
	 */
	private static boolean applicationWaiting() {
		return (applicationRunnable != null);
	}
	
	/*
	 * Post an event to the internal Display to request the control of the UI
	 * thread. After this runApplication() should get called in the UI thread.
	 */
	private static boolean signalUIThreadRequest() {
		Display internalDisplay = Internal_PackageSupport.getInternalDisplayInstance();
		if(internalDisplay != null) {
			if(applicationUIListener != null) {
				applicationUIListener.applicationUIThreadRequest();
			}
			return true;
		}
		return false;
	}

	/*
	 * Call back the application's runnable
	 */
	private static void callback() {
		try {
			if(applicationRunnable != null) {
				flushInternals();
				applicationRunnable.run();
			}
		} finally {
			applicationRunnable = null;
		}
	}
	
	/*
	 * Clear any events or other items possibly left over by any previous usage
	 * of the UI resources. There must not be anything that can interfere with
	 * the application. It must appear as the application starts with an
	 * uninitialized UI.
	 */
	private static void flushInternals() {
		Display internalDisplay = Internal_PackageSupport.getInternalDisplayInstance();
		
		// Dispose all widgets
		Shell[] shells = internalDisplay.getShells();
		for(int i = 0; i < shells.length; ++i) {
			if(shells[i] != null && !shells[i].isDisposed()) {
				shells[i].dispose();
			}
		}
		
		// Flush the event queue
		while(internalDisplay.readAndDispatch()) {}
	}
}
