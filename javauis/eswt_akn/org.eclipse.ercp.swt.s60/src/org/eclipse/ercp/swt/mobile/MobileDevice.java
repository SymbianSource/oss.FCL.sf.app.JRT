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

import java.util.Vector;

import org.eclipse.ercp.swt.mobile.internal.OS;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;

/**
 * Instances of this class represent the device that is being used.
 * It provides methods that enable applications to learn more about the device
 * specific characteristics and capabilities.
 * <p>
 * Applications can query what input features and display screens are permanently
 * part of the device. These are considered <i>local</i> features. Some devices also
 * allow input mechanisms or screens to be attached to the device at runtime. These
 * are considered <i>remote</i> features.  Since local features do not come and go,
 * it is sufficient to query for them once. On the other hand, since remote devices
 * can be added or removed at any time, an application needs to add a <code>
 * MobileDeviceListener</code> to be informed of these events.
 * <p>
 * Local features may also be <i>internal</i> or <i>external</i>.
 * External features are only available when a device is closed.
 * Internal features are only available when a device is opened.
 *
 * @see Screen
 * @see Input
 * @see MobileDeviceListener
 */
public class MobileDevice
{

    private int         handleMobileDevice;
    private boolean     isDisposed;
    private Vector      mobileDeviceListeners;
    private Screen[]    screens;
    private static int  nbScreens = 0;
    private Input[]     inputs;
    private static int  nbInputs = 0;
    private int         alertLevel;
    private boolean     isOpen;

    static MobileDevice currentMobileDevice;

    private static MobileDevice This;

    /**
     * feature which is permanently attached to the mobile device and always available
     */
    public static final int LOCAL = 0;

    /**
     * local feature which is only available when mobile device is opened
     */
    public static final int INTERNAL = 1;

    /**
     * local feature which is only available when mobile device is closed
     */
    public static final int EXTERNAL = 2;

    /**
     * feature which is not part of the mobile device unless connected via wire or wireless signal
     */
    public static final int REMOTE = 3;

    /**
     * alert level indicating simple notification (ex. e-mail arrival)
     */
    public static final int ALERT_LOW_IMPORTANCE = 1;

    /**
     * alert level indicating user attention desired (ex. instant message arrival)
     */
    public static final int ALERT_HIGH_IMPORTANCE = 2;

    /**
     * alert level indicating immediate attention is required (ex. battery level at 1%)
     */
    public static final int ALERT_CRITICAL = 3;
    /**
     * virtual keyboard status indicating normal show and hide operation
     */
    public static final int VK_NORMAL = 1;

    /**
     * virtual keyboard status indicating keyboard is always visible
     */
    public static final int VK_ALWAYS_ON = 2;

    /**
     * virtual keyboard status indicating keyboard is always hidden
     */
    public static final int VK_ALWAYS_OFF = 3;

    /**
     * Constructs a new instance of this class. Non-public to prevent
     * instantiation outside this package.
     *
     */
    MobileDevice()
    {
        This = this;

        (Display.getDefault()).syncExec(new Runnable()
        {
            public void run()
            {
                This.handleMobileDevice = OS.MobileDevice_New(This);

                Runnable disposeRunnable = new Runnable()
                {

                    public void run()
                    {
                        internal_dispose();
                    }
                };
                (Display.getDefault()).disposeExec(disposeRunnable);
            }
        });
        mobileDeviceListeners = new Vector();
    }
    private void internal_sendMobileDeviceEvent(int eventType)
    {
        int count = mobileDeviceListeners.size();
        for (int i = 0; i <count; ++i)
        {
            MobileDeviceEvent event = new MobileDeviceEvent(this);
            event.type = eventType;
            MobileDeviceListener listener = (MobileDeviceListener) mobileDeviceListeners.elementAt(i);
            listener.deviceChanged(event);
        }
    }

    /**
     * Adds the listener to the collection of listeners who will be notified when a device
     * configuration change occurs, by calling one of the methods defined in the
     * <code>MobileDeviceListener</code> interface.
     * <p>
     * <code>screenChanged</code> is called when a monitor configuration changes.<br>
     * <code>inputChanged</code> is called when an input configuration changes.<br>
     * <code>deviceChanged</code> is called when the device is opened or closed.<br>
     *
     * @param listener instance called when device events occur
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_ITEM_NOT_ADDED - if the operation fails because of an operating system failure</li>
     * </ul>
     *
     * @see #removeMobileDeviceListener
     * @see MobileDeviceListener
     */
    public void addMobileDeviceListener(MobileDeviceListener listener)
    {
        if (listener == null)
        {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }

        // Add listener
        mobileDeviceListeners.addElement(listener);
    }


    /**
     * Alerts the user using device specific mechanisms such as sounds, flashing, or
     * vibration. <code>Level</code> must be one of ALERT_LOW_IMPORTANCE,
     * ALERT_HIGH_IMPORTANCE, or ALERT_CRITICAL. The method maps these levels to
     * device specific features that may also be dependent upon user configuration
     * or current profile.
     *
     * @param level
     *            constant describing the importance of the alert
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if level is not one of the
     *                allowed class constants</li>
     *                </ul>
     *
     * @see #ALERT_LOW_IMPORTANCE
     * @see #ALERT_HIGH_IMPORTANCE
     * @see #ALERT_CRITICAL
     */
    public void alert(int level)
    {
        if (level != ALERT_LOW_IMPORTANCE &&
                level != ALERT_HIGH_IMPORTANCE &&
                level != ALERT_CRITICAL)
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        alertLevel = level;
        (Display.getDefault()).syncExec(new Runnable()
        {
            public void run()
            {
                OS.MobileDevice_Alert(handleMobileDevice, alertLevel);
            }
        });
    }


    /**
     * Returns an array of Input objects describing the input features available to the device.
     * The return value may be <code>null</code> if there are no input features available.
     *
     * @exception SWTError <ul>
     *    <li>ERROR_CANNOT_GET_SELECTION - if the operation fails because
     *      of an operating system failure</li>
     * </ul>
     *
     * @see Input
     */
    public Input[] getInputs()
    {
        if (inputs == null)
        {
            try
            {
                (Display.getDefault()).syncExec(new Runnable()
                {
                    public void run()
                    {
                        nbInputs = OS.MobileDevice_GetInputNumber(handleMobileDevice);
                        inputs = new Input[nbInputs];
                        for (int i=0; i<nbInputs; i++)
                        {
                            inputs[i] = new Input(i);
                        }
                    }
                });
            }
            catch (Throwable e)
            {
                SWT.error(SWT.ERROR_CANNOT_GET_SELECTION);
            }
        }
        return inputs;
    }


    /**
     * Returns an array of Screen objects describing the display features available to the device.
     * The return value may be <code>null</code> if there are no display screens available.
     *
     * @exception SWTError <ul>
     *    <li>ERROR_CANNOT_GET_SELECTION - if the operation fails because
     *      of an operating system failure</li>
     * </ul>
     *
     * @see Screen
     */
    public Screen[] getScreens()
    {
        if (screens == null)
        {
            try
            {
                (Display.getDefault()).syncExec(new Runnable()
                {
                    public void run()
                    {
                        nbScreens = OS.MobileDevice_GetScreenNumber(handleMobileDevice);
                        screens = new Screen[nbScreens];
                        for (int i=0; i<nbScreens; i++)
                        {
                            screens[i] = new Screen(i);
                        }
                    }
                });
            }
            catch (Throwable e)
            {
                SWT.error(SWT.ERROR_CANNOT_GET_SELECTION);
            }
        }
        return screens;
    }


    /**
     * Returns singleton instance of MobileDevice class.
     *
     * @return singleton of MobileDevice class. Must not be Null.
     *
     * @exception SWTError <ul>
     *    <li>ERROR_CANNOT_GET_SELECTION - if the operation fails because of
     *      an operating system failure</li>
     * </ul>
     */
    public static MobileDevice getMobileDevice()
    {
        if (currentMobileDevice == null)
        {
            try
            {
                currentMobileDevice = new MobileDevice();
            }
            catch (Throwable e)
            {
                SWT.error(SWT.ERROR_CANNOT_GET_SELECTION);
            }
        }
        return currentMobileDevice;
    }


    /**
     * Returns whether device is opened. For devices that have no internal screens
     * or input features, the method always returns <code>true</code>.
     */
    public boolean isOpen()
    {
        (Display.getDefault()).syncExec(new Runnable()
        {
            public void run()
            {
                isOpen = OS.MobileDevice_IsOpen(handleMobileDevice);
            }
        });

        return isOpen;
    }


    /**
     * Call to dispose the Screen object.
     *
     */
    int internal_dispose()
    {
        int savehandleMobileDevice = handleMobileDevice;

        if (inputs != null)
        {
            for (int i = 0; i < inputs.length; i++)
            {
                if (inputs[i] != null)
                {
                    inputs[i].internal_dispose();
                }
            }
        }

        if (screens != null)
        {
            for (int i = 0; i < screens.length; i++)
            {
                if (screens[i] != null)
                {
                    screens[i].internal_dispose();
                }
            }
        }

        isDisposed = true;
        currentMobileDevice = null;
        handleMobileDevice = 0;
        inputs = null;
        screens = null;

        OS.MobileDevice_Dispose(savehandleMobileDevice);

        return savehandleMobileDevice;
    }


    /**
     * Removes the listener from the collection of listeners who will be notified when a device
     * configuration change occurs.
     *
     * @param listener instance called when device events occur
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_ITEM_NOT_REMOVED - if the operation fails because
     *      of an operating system failure</li>
     * </ul>
     *
     * @see #addMobileDeviceListener
     * @see MobileDeviceListener
     */
    public void removeMobileDeviceListener(MobileDeviceListener listener)
    {
        if (listener == null)
        {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }

        //remove listener
        //Remove method of Vector class never fails,
        //so ERROR_ITEM_NOT_REMOVED could never be thrown.
        mobileDeviceListeners.removeElement(listener);
    }

    /**
     * Sets the status of the system virtual keyboard (if one is available).
     * By default, or when status is set to VK_NORMAL, a system virtual keyboard
     * is displayed when a widget capable of text input gains focus and is
     * hidden when that widget looses focus. However, there are cases where
     * an application may wish to continuously display the virtual keyboard
     * or always keep it hidden. Setting the status to VK_ALWAYS_ON or VK_ALWAYS_OFF
     * will accomplish this and the effect is immediate. Changing focus will
     * then have no affect on the virtual keyboard. Note: By default, widgets which
     * normally accept input but are set to <i>read only</i> do not cause the
     * the virtual keyboard to display.
     *
     * @param status virtual keyboard mode. One of VK_NORMAL, VK_ALWAYS_ON, VK_ALWAYS_OFF
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the status parameter is not valid</li>
     * </ul>
     *
     * @see #VK_NORMAL
     * @see #VK_ALWAYS_ON
     * @see #VK_ALWAYS_OFF
     */
    public void setVKStatus(int status)
    {

    }
}
