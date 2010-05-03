/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - initial implementation 
 *******************************************************************************/
package org.eclipse.ercp.swt.midp;

import org.eclipse.swt.SWT;
import org.eclipse.swt.internal.qt.s60.OS;

/**
 * <p>
 * To use eSWT API a MIDlet needs to dedicate a thread as the UI thread for
 * running the event loop. This can't happen in the MIDlet.startApp() because
 * it's required by the MIDP specification that the call to startApp() returns
 * quickly. Additionally, many native UI toolkits have restrictions for the
 * thread that can be used as the UI thread. Thus, a random Java thread can't be
 * used either.
 * </p>
 * <p>
 * This class provides means to obtain a thread that can be used as the UI
 * thread on the platform. All MIDlets must use this class to obtain the UI
 * thread. If a MIDlet uses some other thread as the UI thread then the behavior
 * is undefined.
 * </p>
 * 
 * @see MIDlet
 * @see MIDlet#startApp
 */
public class UIThreadSupport {

private static boolean started;

/**
 * <p>
 * Calls back the given Runnable in a thread that can be used as the UI thread.
 * The method must not be called more than once as there might be only one
 * thread in the process capable of being used as the UI thread.
 * </p>
 * 
 * An example: <code><pre>
 * ...
 * class MyMIDlet extends javax.microedition.midlet.MIDlet {
 *     ...
 *     public void startApp() {
 *         UIThreadSupport.startInUIThread(new Runnable() {
 *             public void run() {
 *                 Display display = new Display();
 *                 ...
 *             }
 *          });
 *     }
 *     ...
 * }
 * </pre></code>
 * 
 * @param runnable The Runnable object to call back
 * @exception SWTError <ul>
 *                <li>ERROR_NO_HANDLES if a handle could not be obtained for
 *                thread creation</li>
 * @exception SWTError <ul>
 *                <li>ERROR_FAILED_EXEC if called more than once</li>
 * @see MIDlet
 * @see MIDlet#startApp
 */
public static void startInUIThread(Runnable runnable) {
    synchronized(UIThreadSupport.class) {
        if(started) {
            SWT.error(SWT.ERROR_FAILED_EXEC);
        }
        started = true;
    }
    int status = OS.startUI(runnable);
    if(status != 0) {
        SWT.error(SWT.ERROR_NO_HANDLES);
    }
}

}
