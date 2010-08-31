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

import org.eclipse.swt.internal.qt.midp.UIThreadLauncher;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Internal_PackageSupport;

/**
 * A class that encapsulates services that enable the use of eSWT UI before the
 * application has initialized its UI. This is achieved by utilizing an
 * 'internal' instance of the Display class. I.e. an instance that is not
 * revealed to the application but is only used internally within the API
 * implementation.
 * 
 * The same rules apply to the internal Display instance that apply to the
 * Display normally. When the Display is created then the creating thread
 * becomes the UI thread of the process. The application needs to be able to
 * obtain control of the UI thread. The services for interfacing with the
 * application are provided by the class ApplicationUI.
 * 
 * @see ApplicationUI
 */
public final class InternalUI {
	/**
	 * Obtains the platform specific UI thread where SWT UI APIs can be accessed
	 * in and calls the provided Runnable in it. This method instantly returns
	 * leaving two threads running concurrently: the calling thread and the UI
	 * thread. Only one UI thread can exist per process and this method can be
	 * called only once to obtain it. Subsequent calls will result in an
	 * exception being thrown. This method can be called by any thread.
	 * 
	 * @param runnable
	 *            A Runnable to call back in the UI thread.
	 * @exception RuntimeException
	 *                If starting the UI thread fails
	 */
	public static void startInUIThread(Runnable runnable) {
		if(!UIThreadLauncher.startInUIThread(runnable)) {
			throw new RuntimeException("Failed to start the UI thread");
		}
	}
	
	/**
	 * Creates the internal instance of Display. Must be called only once.
	 * 
	 * This means that the native resources required by the eSWT UI will be
	 * initialized and a Display instance is returned that can be used to access
	 * them normally via the public eSWT APIs. 
	 * 
	 * @return The internal Display instance
	 * @see getInternalDisplayInstance
	 */
	public synchronized static Display createInternalDisplay() {
		if(getInternalDisplayInstance() != null) {
			throw new RuntimeException("Display already exists");
		}
		return Internal_PackageSupport.internalInstance();
	}
	
	/**
	 * Returns the internal Display instance or null if it doesn't exist. 
	 * @return The internal Display instance or null
	 */
	public synchronized static Display getInternalDisplayInstance() {
		return Internal_PackageSupport.getInternalDisplayInstance();
	}
}
