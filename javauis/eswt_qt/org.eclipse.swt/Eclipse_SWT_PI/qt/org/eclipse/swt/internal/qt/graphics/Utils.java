/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/
package org.eclipse.swt.internal.qt.graphics;

import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Internal_PackageSupport;

/** 
 * Class for general utilities for common graphics
 */
final class Utils {
	
	/**
	 * Validates that ui thread is created and execution is 
	 * currently in ui thread.
	 * 
	 * @throws java.lang.Error if ui thread is not initialized or 
	 * this method is not called within ui thread.
	 */
	static void validateUiThread() {
		if (!Config.ENABLE_UI_THREAD_VALIDATION) {
			return;
		}
		Display display = Internal_PackageSupport.getDisplayInstance();
		if(display == null) {
			display = Internal_PackageSupport.getInternalDisplayInstance();
		}
		if (display == null) {
			throw new Error("Ui thread not initialized or call was made outside ui thread");
		} else {
			if (Thread.currentThread() != display.getThread()) {
				throw new Error("Call to common graphics object occured outside ui thread");
			}
		}
	}

}
