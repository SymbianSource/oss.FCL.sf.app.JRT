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

import org.eclipse.swt.internal.symbian.*;
import org.eclipse.swt.*;

/**
 * Instances of this class manage operating system resources that
 * define how text looks when it is displayed. Fonts may be constructed
 * by providing a device and either name, size and style information
 * or a <code>FontData</code> object which encapsulates this data.
 * <p>
 * Application code must explicitly invoke the <code>Font.dispose()</code>
 * method to release the operating system resources managed by each instance
 * when those instances are no longer required.
 * </p>
 *
 * @see FontData
 */

public final class Font
{

    /**
     * the handle to the OS font resource
     * (Warning: This field is platform dependent)
     */
    public int handle;

    /**
     * the device where this font was created
     */
    Device device;

    /**
     * True if the font represents a system font,
     * therefore it has to not dispose the handle.
     */
    boolean isSystemFont = false;

    /**
     * Prevents uninitialized instances from being created outside the package.
     */
    Font()
    {
    }

    /**
     * Constructs a new font given a device and font data
     * which describes the desired font's appearance.
     * <p>
     * You must dispose the font when it is no longer required.
     * </p>
     *
     * @param device the device to create the font on
     * @param fd the FontData that describes the desired font (must not be null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if device is null and there is no current device</li>
     *    <li>ERROR_NULL_ARGUMENT - if the fd argument is null</li>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_NO_HANDLES - if a font could not be created from the given font data</li>
     * </ul>
     */
    public Font(Device device, FontData fd)
    {
        if (device == null) device = Device.getDevice();
        if (device == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (fd == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        init(device, fd.getName(), fd.getHeight(), fd.getStyle());
        if (handle == 0) SWT.error(SWT.ERROR_NO_HANDLES);
        if (device.tracking) device.new_Object(this);
    }

    /**
     * Constructs a new font given a device and an array
     * of font data which describes the desired font's
     * appearance.
     * <p>
     * You must dispose the font when it is no longer required.
     * </p>
     *
     * @param device the device to create the font on
     * @param fds the array of FontData that describes the desired font (must not be null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if device is null and there is no current device</li>
     *    <li>ERROR_NULL_ARGUMENT - if the fds argument is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if the length of fds is zero</li>
     *    <li>ERROR_NULL_ARGUMENT - if any fd in the array is null</li>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_NO_HANDLES - if a font could not be created from the given font data</li>
     * </ul>
     *
     * @since 2.1
     */
    public Font(Device device, FontData[] fds)
    {
        if (device == null) device = Device.getDevice();
        if (device == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (fds == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (fds.length == 0) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        for (int i=0; i<fds.length; i++)
        {
            if (fds[i] == null) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        init(device, fds[0].getName(), fds[0].getHeight(), fds[0].getStyle());
        if (device.tracking) device.new_Object(this);
    }

    /**
     * Constructs a new font given a device, a font name,
     * the height of the desired font in points, and a font
     * style.
     * <p>
     * You must dispose the font when it is no longer required.
     * </p>
     *
     * @param device the device to create the font on
     * @param name the name of the font (must not be null)
     * @param height the font height in points
     * @param style a bit or combination of NORMAL, BOLD, ITALIC
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if device is null and there is no current device</li>
     *    <li>ERROR_NULL_ARGUMENT - if the name argument is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if the height is negative</li>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_NO_HANDLES - if a font could not be created from the given arguments</li>
     * </ul>
     */
    public Font(Device device, String name, int height, int style)
    {
        if (device == null) device = Device.getDevice();
        if (device == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (name == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        init(device, name, height, style);
        if (handle == 0) SWT.error(SWT.ERROR_NO_HANDLES);
        if (device.tracking) device.new_Object(this);
    }

    /**
     * Disposes of the operating system resources associated with
     * the font. Applications must dispose of all fonts which
     * they allocate.
     */
    public void dispose()
    {
        if (isSystemFont) return;
        if (handle == 0) return;
        if (device.isDisposed()) return;
        OS.Font_Dispose(device.handle, handle);
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
        if (!(object instanceof Font)) return false;
        Font font = (Font) object;
        return device == font.device && handle == font.handle;
    }

    /**
     * Returns an array of <code>FontData</code>s representing the receiver.
     * On Windows, only one FontData will be returned per font. On X however,
     * a <code>Font</code> object <em>may</em> be composed of multiple X
     * fonts. To support this case, we return an array of font data objects.
     *
     * @return an array of font data objects describing the receiver
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public FontData[] getFontData()
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        FontData fd = new FontData(OS.Font_GetName(handle),
                                   OS.Font_GetHeight(handle),
                                   OS.Font_GetStyle(handle));
        return new FontData[] {fd};
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
        return handle;
    }

    /**
     * Allocates the operating system resources associated
     * with the receiver.
     *
     * @param device the device to create the font on
     * @param name the name of the font (must not be null)
     * @param height the font height in points
     * @param style a bit or combination of NORMAL, BOLD, ITALIC
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the height is negative</li>
     * </ul>
     * @see #dispose
     */
    void init(Device device, String name, int height, int style)
    {
        if (height < 0) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        this.device = device;
        handle = OS.Font_New(device.handle, name, height, style);
    }

    /**
     * Returns <code>true</code> if the font has been disposed,
     * and <code>false</code> otherwise.
     * <p>
     * This method gets the dispose state for the font.
     * When a font has been disposed, it is an error to
     * invoke any other method using the font.
     *
     * @return <code>true</code> when the font is disposed and <code>false</code> otherwise
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
        if (isDisposed()) return "Font {*DISPOSED*}";
        return "Font {" + getFontData() + "}";
    }

    /**
     * Invokes platform specific functionality to allocate a new font.
     * <p>
     * <b>IMPORTANT:</b> This method is <em>not</em> part of the public
     * API for <code>Font</code>. It is marked public only so that it
     * can be shared within the packages provided by SWT. It is not
     * available on all platforms, and should never be called from
     * application code.
     * </p>
     *
     * @param device the device on which to allocate the color
     * @param handle the handle for the font
     * @return a new font object containing the specified device and handle
     */
    public static Font internal_new(Device device, int handle)
    {
        if (device == null) device = Device.getDevice();
        Font font = new Font();
        font.handle = handle;
        font.device = device;
        return font;
    }

    /**
     * Invokes platform specific functionality to allocate a new font
     * which has handle to a system font.
     * <p>
     * <b>IMPORTANT:</b> This method is <em>not</em> part of the public
     * API for <code>Font</code>. It is marked public only so that it
     * can be shared within the packages provided by SWT. It is not
     * available on all platforms, and should never be called from
     * application code.
     * </p>
     *
     * @param device the device on which to allocate the color
     * @param handle the handle for the font
     * @return a new font object containing the specified device and
     *         system font handle
     */
    public static Font internal_newSystemFont(Device device, int handle)
    {
        Font font = Font.internal_new(device, handle);
        font.isSystemFont = true;
        return font;
    }

}
