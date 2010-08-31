/*******************************************************************************
 * Copyright (c) 2004 IBM Corp.
 * Portion Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Mark Rogalski (IBM Corp.) - initial API specification
 *    Nokia Corporation - S60 implementation
 *    Nokia Corporation - Qt implementation
 *******************************************************************************/

package org.eclipse.ercp.swt.mobile;

import org.eclipse.swt.events.TypedEvent;

/**
 * Instances of this class are sent as a result of screen configuration changes.
 * 
 * @see ScreenListener
 */

public class ScreenEvent extends TypedEvent {

	/**
	 * whether screen is active or not
	 */
	public boolean isActive;

	/**
	 * orientation of screen
	 */
	public int orientation;

	/**
	 * Constructs a new instance of this class and associates it with the
	 * given screen.
	 * 
	 * @param screen
	 *            the screen object which is the source of this event
	 * 
	 * @see Screen
	 */
	public ScreenEvent(Screen screen) {
		super(screen);
	}

}

