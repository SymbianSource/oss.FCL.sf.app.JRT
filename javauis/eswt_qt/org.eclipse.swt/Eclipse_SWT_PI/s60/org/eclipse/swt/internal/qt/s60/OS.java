/*******************************************************************************
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - initial implementation 
 *******************************************************************************/
package org.eclipse.swt.internal.qt.s60;

import org.eclipse.swt.internal.Library;

public final class OS {
    static {
        Library.loadLibrary("eswtqt");
    }

    private static boolean started;
    
	/**
	 * Obtains the platform specific UI thread where SWT UI APIs can be accessed
	 * in and calls the provided Runnable in it. This method instantly returns
	 * leaving two threads running concurrently: the calling thread and the UI
	 * thread. Only one UI thread can exist per process and this method can be
	 * called only once to obtain it. Subsequent calls will result in an
	 * exception being thrown. This method can be called by any thread.
	 * 
	 * @param callback
	 *            An object to call in the UI thread.
	 * @return 0 In case of success, non-zero if starting failed and callback
	 *         object will never be called.
	 * @exception RuntimeException
	 *                If called again after already started
	 */
    public synchronized static final int startUI(
            final Runnable callback) {
        if (started) {
            throw new RuntimeException(
                    "UI already started. There can be only 1 UI thread per process. ");
        }
        started = true;
        return _startUI(callback, org.eclipse.swt.internal.ApplicationUidWrapper.uid());
    }
    
    private static final native int _startUI(Object runner, int uid);
}
