/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Lynne Kues (IBM Corp) - modified to reflect eSWT API subset
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.swt.graphics;

import org.eclipse.swt.*;
import org.eclipse.swt.internal.symbian.*;

/**
 * Instances of this class manage the operating system resources that
 * implement SWT's RGB color model. To create a color you can either
 * specify the individual color components as integers in the range
 * 0 to 255 or provide an instance of an <code>RGB</code>.
 * <p>
 * Application code must explicitly invoke the <code>Color.dispose()</code>
 * method to release the operating system resources managed by each instance
 * when those instances are no longer required.
 * </p>
 *
 * @see RGB
 * @see Device#getSystemColor
 */

public final class Color
{

    /**
     * the handle to the OS color resource
     * (Warning: This field is platform dependent)
     */
    public int handle;

    /**
     * the device where this color was created
     */
    Device device;

    /**
     * Prevents uninitialized instances from being created outside the package.
     */
    Color()
    {
    }

    /**
     * Constructs a new instance of this class given a device and the
     * desired red, green and blue values expressed as ints in the range
     * 0 to 255 (where 0 is black and 255 is full brightness). On limited
     * color devices, the color instance created by this call may not have
     * the same RGB values as the ones specified by the arguments. The
     * RGB values on the returned instance will be the color values of
     * the operating system color.
     * <p>
     * You must dispose the color when it is no longer required.
     * </p>
     *
     * @param device the device on which to allocate the color
     * @param red the amount of red in the color
     * @param green the amount of green in the color
     * @param blue the amount of blue in the color
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if device is null and there is no current device</li>
     *    <li>ERROR_INVALID_ARGUMENT - if the red, green or blue argument is not between 0 and 255</li>
     * </ul>
     *
     * @see #dispose
     */
    public Color(Device device, int red, int green, int blue)
    {
        if (device == null) device = Device.getDevice();
        if (device == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        init(device, red, green, blue);
    }

    /**
     * Constructs a new instance of this class given a device and an
     * <code>RGB</code> describing the desired red, green and blue values.
     * On limited color devices, the color instance created by this call
     * may not have the same RGB values as the ones specified by the
     * argument. The RGB values on the returned instance will be the color
     * values of the operating system color.
     * <p>
     * You must dispose the color when it is no longer required.
     * </p>
     *
     * @param device the device on which to allocate the color
     * @param rgb the RGB values of the desired color
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if device is null and there is no current device</li>
     *    <li>ERROR_NULL_ARGUMENT - if the rgb argument is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if the red, green or blue components of the argument are not between 0 and 255</li>
     * </ul>
     *
     * @see #dispose
     */
    public Color(Device device, RGB rgb)
    {
        if (device == null) device = Device.getDevice();
        if (device == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (rgb == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        init(device, rgb.red, rgb.green, rgb.blue);
    }

    /**
     * Disposes of the operating system resources associated with
     * the color. Applications must dispose of all colors which
     * they allocate.
     */
    public void dispose()
    {
        if (handle == 0) return;
        if (device.isDisposed()) return;
        OS.Color_Dispose(this.device.handle, handle);
        handle = 0;
        if (device.tracking) device.dispose_Object(this);
        device = null;
    }

    /**
     * Compares the argument to the receiver, and returns true
     * if they represent the <em>same</em> object using a class
     * specific comparison.
     *
     * @param object the object to compare with this object
     * @return <code>true</code> if the object is the same as this object and <code>false</code> otherwise
     *
     * @see #hashCode
     */
    public boolean equals(Object object)
    {
        if (object == this) return true;
        if (!(object instanceof Color)) return false;
        Color color = (Color) object;
        return (device == color.device) && ((getRgbValue() & 0xFFFFFF) == (color.getRgbValue() & 0xFFFFFF));
    }

    /**
     * Returns the amount of blue in the color, from 0 to 255.
     *
     * @return the blue component of the color
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public int getBlue()
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        return (getRgbValue() & 0xFF0000) >> 16;
    }

    /**
     * Returns the amount of green in the color, from 0 to 255.
     *
     * @return the green component of the color
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public int getGreen()
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        return (getRgbValue() & 0xFF00) >> 8 ;
    }

    /**
     * Returns the amount of red in the color, from 0 to 255.
     *
     * @return the red component of the color
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public int getRed()
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        return getRgbValue() & 0xFF;
    }

    /**
     * Returns an <code>RGB</code> representing the receiver.
     *
     * @return the RGB for the color
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public RGB getRGB()
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        int val = getRgbValue();
        return new RGB(val & 0xFF, (val & 0xFF00) >> 8, (val & 0xFF0000) >> 16);
    }

    /**
     * Returns an integer hash code for the receiver. Any two
     * objects which return <code>true</code> when passed to
     * <code>equals</code> must return the same value for this
     * method.
     *
     * @return the receiver's hash
     *
     * @see #equals
     */
    public int hashCode()
    {
        return getRgbValue();
    }

    /**
     * Allocates the operating system resources associated
     * with the receiver.
     *
     * @param device the device on which to allocate the color
     * @param red the amount of red in the color
     * @param green the amount of green in the color
     * @param blue the amount of blue in the color
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the red, green or blue argument is not between 0 and 255</li>
     * </ul>
     *
     * @see #dispose
     */
    void init(Device device, int red, int green, int blue)
    {
        if (red > 255 || red < 0 || green > 255 || green < 0 || blue > 255 || blue < 0)
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        this.device = device;
        handle = OS.Color_New(this.device.handle, red, green, blue);
        try
        {
            if (device.tracking) device.new_Object(this);
        }
        catch (Error e)
        {
            OS.Color_Dispose(this.device.handle, handle);
            throw e;
        }
    }

    /**
     * Returns <code>true</code> if the color has been disposed,
     * and <code>false</code> otherwise.
     * <p>
     * This method gets the dispose state for the color.
     * When a color has been disposed, it is an error to
     * invoke any other method using the color.
     *
     * @return <code>true</code> when the color is disposed and <code>false</code> otherwise
     */
    public boolean isDisposed()
    {
        return handle == 0;
    }

    /**
     * Returns a string containing a concise, human-readable
     * description of the receiver.
     *
     * @return a string representation of the receiver
     */
    public String toString()
    {
        if (isDisposed()) return "Color {*DISPOSED*}"; //$NON-NLS-1$
        return "Color {" + getRed() + ", " + getGreen() + ", " + getBlue() + "}"; //$NON-NLS-1$//$NON-NLS-2$ //$NON-NLS-3$ //$NON-NLS-4$
    }

    private int getRgbValue()
    {
        return OS.Color_RgbValue(device.handle, handle);
    }

    /**
     * Invokes platform specific functionality to allocate a new Color.
     * <p>
     * <b>IMPORTANT:</b> This method is <em>not</em> part of the public
     * API for <code>Color</code>. It is marked public only so that it
     * can be shared within the packages provided by SWT. It is not
     * available on all platforms, and should never be called from
     * application code.
     * </p>
     *
     * @param device the device on which to allocate the color
     * @param handle the handle for the color
     * @return a new color object containing the specified device and handle
     */
    public static Color internal_new(Device device, int handle)
    {
        if (device == null) device = Device.getDevice();
        Color color = new Color();
        color.handle = handle;
        color.device = device;
        return color;
    }


}
