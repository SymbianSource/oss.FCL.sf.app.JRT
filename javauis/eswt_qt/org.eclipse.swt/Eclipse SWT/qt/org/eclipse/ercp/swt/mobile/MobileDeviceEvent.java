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
 *     Nokia Corporation - S60 implementation
 *     Nokia Corporation - QT implementation
 *******************************************************************************/

package org.eclipse.ercp.swt.mobile;

import org.eclipse.swt.events.TypedEvent;

/**
 * Instances of this class are sent as a result of device configuration changes.
 * In the case of an <code>inputChanged</code> event, the <code>data</code>
 * field contains an <code>Input</code> object. In the case of a
 * <code>screenChanged</code> event, the <code>data</code> field contains a
 * <code>Screen</code> object.
 * 
 * @see MobileDeviceListener
 */

public class MobileDeviceEvent extends TypedEvent {

	/**
	 * This indicates that internal screens and input features are now active.
	 */
	public static final int OPENED = 1;

	/**
	 * This indicates that internal screens and input features are no longer
	 * active.
	 */
	public static final int CLOSED = 2;

	/**
	 * This indicates that a new feature has become available.
	 */
	public static final int ADDED = 3;

	/**
	 * This indicates that a feature is no longer available.
	 */
	public static final int REMOVED = 4;

	/**
	 * the event type.
	 */
	public int type;

	/**
	 * The instance of an {@link Input Input} or {@link Screen Screen} object
	 * being added or removed when the device configuration changes. The 
	 * value is null when the <code>type</code> value is not ADDED or REMOVED.
	 */
	public Object feature;

	/**
	 * Constructs a new instance of this class and associates it with the 
	 * given mobile device.
	 * 
	 * @param device
	 *            the device object which is the source of this event
	 * 
	 * @see MobileDevice
	 */
	public MobileDeviceEvent(MobileDevice device) {
		super(device);
	}

}

