/*******************************************************************************
 * Copyright (c) 2004 IBM Corp.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Mark Rogalski (IBM Corp.) - initial API specification
 *    Nokia Corporation - S60 implementation
 *******************************************************************************/

package org.eclipse.ercp.swt.mobile;

import org.eclipse.swt.internal.SWTEventListener;

/**
 * Classes implementing this interface provide methods that are called when the
 * device configuration is updated.
 *
 * @see MobileDevice#addMobileDeviceListener
 * @see MobileDevice#removeMobileDeviceListener
 */
public interface MobileDeviceListener extends SWTEventListener
{

    /**
     * Called when a device configuration has been changed, such as opened or closed.
     *
     * @param event
     *            an event containing information about the device configuration change.
     *
     * @see MobileDeviceEvent
     */
    public void deviceChanged(MobileDeviceEvent event);


    /**
     * Called when the input configuration has been changed.
     *
     * @param event
     *            an event containing information about an input configuration
     *            change. The event <code>data</code> field contains an
     *            <code>Input</code> object representing the input feature that
     *            has been added or removed.
     *
     * @see MobileDeviceEvent
     */
    public void inputChanged(MobileDeviceEvent event);


    /**
     * Called when a screen configuration has been changed.
     *
     * @param event
     *            an event containing information about a screen configuration
     *            change. The event <code>data</code> field contains a
     *            <code>Screen</code> object representing the screen that has
     *            been added or removed.
     *
     * @see MobileDeviceEvent
     */
    public void screenChanged(MobileDeviceEvent event);

}