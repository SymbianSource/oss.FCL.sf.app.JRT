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
package org.eclipse.swt.internal.qt.midp;

/**
 * Linux specific implementation of launching the UI thread for a MIDlet. 
 */
public final class UIThreadLauncher {
	public static boolean startInUIThread(Runnable runnable) {
		new Thread(runnable, "SwtQtUiThread").start();
		return true;
	}

}
