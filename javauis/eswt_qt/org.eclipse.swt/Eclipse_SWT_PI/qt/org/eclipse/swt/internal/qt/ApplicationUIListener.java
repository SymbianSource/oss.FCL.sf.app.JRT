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

/**
 * An internal interface for providing notifications about the state of the UI
 * during the life-time of the application.
 */
public interface ApplicationUIListener {
	/**
	 * Called when the application requests the UI thread to call back a
	 * Runnable that the application provided.
	 */
	public void applicationUIThreadRequest();
}
