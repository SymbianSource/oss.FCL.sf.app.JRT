/*******************************************************************************
 * Copyright (c) 2004 IBM Corp.
 * Portion Copyright (c) 2004, 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Mark Rogalski (IBM Corp.) - initial API specification
 *    Nokia Corporation - S60 implementation
 *    Nokia Corporation - QT implementation
 *******************************************************************************/

package org.eclipse.ercp.swt.mobile;

import org.eclipse.swt.internal.SWTEventListener;

/**
 * Classes implementing this interface provide methods that are called when the
 * screen configuration is updated.
 * 
 * @see Screen#addEventListener
 * @see Screen#removeEventListener
 */
public interface ScreenListener extends SWTEventListener {

	/**
	 * Sent when the screen is activated. That is, the screen is powered up and
	 * the content is visible to the user.
	 * 
	 * @param event
	 *            an event containing information about the screen configuration
	 *            change.
	 * 
	 * @see ScreenEvent
	 */
	public void screenActivated(ScreenEvent event);

	/**
	 * Sent when the screen is deactivated. That is, the screen is powered down
	 * or content is no longer visible to the user.
	 * 
	 * @param event
	 *            an event containing information about the screen configuration
	 *            change.
	 * 
	 * @see ScreenEvent
	 */
	public void screenDeactivated(ScreenEvent event);

	/**
	 * Sent when the screen's orientation is changed.
	 * 
	 * @param event
	 *            an event containing information about the screen orientation
	 *            change.
	 * 
	 * @see ScreenEvent
	 */
	public void screenOrientationChanged(ScreenEvent event);

}