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

/**
 * A listener interface used in connection with the class
 * <code>NetworkStatus</code>.
 */
public interface NetworkStatusListener {
	/**
	 * Called by the class <code>NetworkStatus</code> in the UI thread to
	 * provide the current state of the network connections.
	 * 
	 * @param newStateFlags
	 *            Combination of flags representing the currently active network
	 *            connections. The flags are defined in the class
	 *            <code>NetworkStatus</code>.
	 * @see NetworkStatus
	 */
	public void stateChanged(int newStateFlags);
}
