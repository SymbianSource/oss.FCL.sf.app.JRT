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

package org.eclipse.swt.internal.extension;

import org.eclipse.swt.internal.qt.UIThreadManager;

/**
 * A class that provides services for transferring the control over the
 * execution of the UI thread to the application, and services for obtaining
 * notifications of the state of the UI until the UI thread will return back to the
 * caller.
 * 
 * @see InternalUI
 * @see ApplicationUIListener
 */
public final class ApplicationUI {
	
	/**
	 * Hand over the execution of the UI thread to the application. If the
	 * application has not yet requested the UI thread then it will be waiting
	 * until that happens. The waiting can be aborted by calling
	 * <code>abortWait()</code>.
	 * 
	 * The control of the thread will be passed to the application. If the
	 * application allows the UI thread to return then this method will also
	 * return.
	 * 
	 * After calling this method and before the method has returned the state of
	 * the UI thread can be tracked by setting an
	 * <code>ApplicationUIListener</code>.
	 */
	public static void run() {
		UIThreadManager.runApplicationUI();
	}
	
	/**
	 * Aborts waiting the application to request the UI thread. This waiting can
	 * occur after <code>run()</code> has been called if the application has not
	 * yet at that point requested the UI thread and thus has not provided the
	 * Runnable to call back. Aborting the waiting will cause the UI thread to
	 * return from <code>run()</code>. If the application already obtained the
	 * control of the UI thread then this method does nothing. 
	 */
	public static void abortWait() {
		UIThreadManager.abortWait();
	}
	
	/**
	 * Sets the listener that will be notified of the state of the UI during its
	 * life-cycle under the application's control. 
	 * 
	 * @param listener The listener, or null to disable the notifications
	 */	
	public static void setListener(ApplicationUIListener listener) {
		final ApplicationUIListener applicationUIListener = listener;
		org.eclipse.swt.internal.qt.ApplicationUIListener internalListener; 
		if(listener != null) {
			internalListener = new org.eclipse.swt.internal.qt.ApplicationUIListener() {
				public void applicationUIThreadRequest() {
					applicationUIListener.applicationUIThreadRequest();
				}
			};
		} else {
			internalListener = null;
		}
		UIThreadManager.setApplicationUIListener(internalListener);
	}
}
