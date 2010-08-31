/*******************************************************************************
 * Copyright (c) 2000, 2007 IBM Corporation and others.
 * Portion Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Nokia Corporation - Qt implementation
 *******************************************************************************/
package org.eclipse.swt.graphics;

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTError;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.QtSupplementaryFontData;

/**
 * Instances of this class manage operating system resources that define how
 * text looks when it is displayed. Fonts may be constructed by providing a
 * device and either name, size and style information or a <code>FontData</code>
 * object which encapsulates this data.
 * <p>
 * Application code must explicitly invoke the <code>Font.dispose()</code>
 * method to release the operating system resources managed by each instance
 * when those instances are no longer required.
 * </p>
 * 
 * @see FontData
 */
public final class Font extends Object {
	/**
	 * the handle to the OS font resource (Warning: This field is platform
	 * dependent)
	 * <p>
	 * <b>IMPORTANT:</b> This field is <em>not</em> part of the SWT public API.
	 * It is marked public only so that it can be shared within the packages
	 * provided by SWT. It is not available on all platforms and should never be
	 * accessed from application code.
	 * </p>
	 */
	public int handle;
	
	Device device;
	boolean extraFontStyle;
	String xlfd;

	/**
	 * Prevents uninitialized instances from being created outside the package.
	 */
	Font() {
	}

	/**
	 * Constructs a new font given a device and font data which describes the
	 * desired font's appearance.
	 * <p>
	 * You must dispose the font when it is no longer required.
	 * </p>
	 * 
	 * @param device
	 *            the device to create the font on
	 * @param fd
	 *            the FontData that describes the desired font (must not be
	 *            null)
	 * 
	 * @exception IllegalArgumentException
	 *                <ul>
	 *                <li>ERROR_NULL_ARGUMENT - if device is null and there is
	 *                no current device</li>
	 *                <li>ERROR_NULL_ARGUMENT - if the fd argument is null</li>
	 *                </ul>
	 * @exception SWTError
	 *                <ul>
	 *                <li>ERROR_NO_HANDLES - if a font could not be created from
	 *                the given font data</li>
	 *                </ul>
	 */
	public Font(Device device, FontData fd) {
		if (fd == null)
			SWT.error(SWT.ERROR_NULL_ARGUMENT);
		init(device, fd.getName(), fd.getHeight(), fd.getStyle());
		initExtraFontData(fd);
	}
	
	

	/**
	 * Constructs a new font given a device and an array of font data which
	 * describes the desired font's appearance.
	 * <p>
	 * You must dispose the font when it is no longer required.
	 * </p>
	 * 
	 * @param device
	 *            the device to create the font on
	 * @param fds
	 *            the array of FontData that describes the desired font (must
	 *            not be null)
	 * 
	 * @exception IllegalArgumentException
	 *                <ul>
	 *                <li>ERROR_NULL_ARGUMENT - if device is null and there is
	 *                no current device</li>
	 *                <li>ERROR_NULL_ARGUMENT - if the fds argument is null</li>
	 *                <li>ERROR_INVALID_ARGUMENT - if the length of fds is zero</li>
	 *                <li>ERROR_NULL_ARGUMENT - if any fd in the array is null</li>
	 *                </ul>
	 * @exception SWTError
	 *                <ul>
	 *                <li>ERROR_NO_HANDLES - if a font could not be created from
	 *                the given font data</li>
	 *                </ul>
	 * 
	 * @since 2.1
	 */
	public Font(Device device, FontData[] fds) {
		if (fds == null)
			SWT.error(SWT.ERROR_NULL_ARGUMENT);
		if (fds.length == 0)
			SWT.error(SWT.ERROR_INVALID_ARGUMENT);
		for (int i = 0; i < fds.length; i++) {
			if (fds[i] == null)
				SWT.error(SWT.ERROR_INVALID_ARGUMENT);
		}
		FontData fd = fds[0];
		init(device, fd.getName(), fd.getHeight(), fd.getStyle());
		initExtraFontData(fd);
	}

	/**
	 * Constructs a new font given a device, a font name, the height of the
	 * desired font in points, and a font style.
	 * <p>
	 * You must dispose the font when it is no longer required.
	 * </p>
	 * 
	 * @param device
	 *            the device to create the font on
	 * @param name
	 *            the name of the font (must not be null)
	 * @param height
	 *            the font height in points
	 * @param style
	 *            a bit or combination of NORMAL, BOLD, ITALIC
	 * 
	 * @exception IllegalArgumentException
	 *                <ul>
	 *                <li>ERROR_NULL_ARGUMENT - if device is null and there is
	 *                no current device</li>
	 *                <li>ERROR_NULL_ARGUMENT - if the name argument is null</li>
	 *                <li>ERROR_INVALID_ARGUMENT - if the height is negative</li>
	 *                </ul>
	 * @exception SWTError
	 *                <ul>
	 *                <li>ERROR_NO_HANDLES - if a font could not be created from
	 *                the given arguments</li>
	 *                </ul>
	 */
	public Font(Device device, String name, int height, int style) {
		init(device, name, height, style);
	}

	public void dispose() {
		if (device == null)
			return;
		if (device.isDisposed())
			return;
        if(!OS.SwtFontCache_isCached(handle)) {
            OS.QFont_delete(handle);
        }
        handle = 0;
		if (Device.tracking)
			device.dispose_Object(this);
		device = null;
	}

	/**
	 * Compares the argument to the receiver, and returns true if they represent
	 * the <em>same</em> object using a class specific comparison.
	 * 
	 * @param object
	 *            the object to compare with this object
	 * @return <code>true</code> if the object is the same as this object and
	 *         <code>false</code> otherwise
	 * 
	 * @see #hashCode
	 */
	public boolean equals(Object object) {
		if (object == this)
			return true;
		if (!(object instanceof Font))
			return false;
		return OS.QFont_swt_equals(handle,((Font) object).handle);
	}

	/**
	 * Returns an array of <code>FontData</code>s representing the receiver. On
	 * Windows, only one FontData will be returned per font. On X however, a
	 * <code>Font</code> object <em>may</em> be composed of multiple X fonts. To
	 * support this case, we return an array of font data objects.
	 * 
	 * @return an array of font data objects describing the receiver
	 * 
	 * @exception SWTException
	 *                <ul>
	 *                <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been
	 *                disposed</li>
	 *                </ul>
	 */
	public FontData[] getFontData() {
		if (isDisposed())
			SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);

		String family = OS.QFont_family(handle);
		int pointSize = OS.QFont_pointSize(handle);
		int weight = OS.QFont_weight(handle);
		boolean italic = OS.QFont_italic(handle);

		int style = SWT.NORMAL;
		if (weight > OS.QT_FONTNORMAL)
			style |= SWT.BOLD;
		if (italic == true)
			style |= SWT.ITALIC;
		FontData data = new FontData(family, pointSize, style);
		if (xlfd != null) {
			data.xlfd = xlfd;
		} else if (extraFontStyle) {
			data.extraFontData = new QtSupplementaryFontData();
			QtSupplementaryFontData extraData = data.extraFontData;
			extraData.underline = OS.QFont_underline(handle) == true ? 1 : 0;
			extraData.overline = OS.QFont_overline(handle) == true ? 1 : 0;
			extraData.strikeOut = OS.QFont_strikeOut(handle) == true ? 1 : 0;
			extraData.stretch = OS.QFont_stretch(handle);
			extraData.fixedPitch = OS.QFont_fixedPitch(handle) == true ? 1 : 0;
			extraData.style = OS.QFont_style(handle);
			extraData.weight = OS.QFont_weight(handle);
			extraData.styleStrategy = OS.QFont_styleStrategy(handle);
		}
		return new FontData[] { data };
	}

	/**
	 * Returns an integer hash code for the receiver. Any two objects that
	 * return <code>true</code> when passed to <code>equals</code> must return
	 * the same value for this method.
	 * 
	 * @return the receiver's hash
	 * 
	 * @see #equals
	 */
	public int hashCode() {
		return handle;
	}

	void init(Device device, String name, int height, int style) {
		extraFontStyle = false;
		xlfd = null;
		if (device == null)
			SWT.error(SWT.ERROR_NULL_ARGUMENT);
		this.device = device;
		if (name == null)
			SWT.error(SWT.ERROR_NULL_ARGUMENT);
		if (height < 0)
			SWT.error(SWT.ERROR_INVALID_ARGUMENT);

		// Keep this after dealing with all exceptions!
		if (Device.tracking)
			device.new_Object(this);

		boolean italic = (style & SWT.ITALIC) != 0;
		int weight = OS.QT_FONTNORMAL;
		if ((style & SWT.BOLD) != 0) {
			weight = OS.QT_FONTBOLD;
		}
		handle = OS.QFont_new(name, height, weight, italic);
	}
	
	void initExtraFontData(FontData fd) {
		if (fd.xlfd != null) {
			OS.QFont_setRawName(handle, fd.xlfd);
			xlfd = fd.xlfd;
		} else if (fd.extraFontData != null) {
			QtSupplementaryFontData extraData = fd.extraFontData;
			if (extraData.underline > -1) {
				OS.QFont_setUnderline(handle, extraData.underline > 0 ? true
						: false);
			}
			if (extraData.overline > -1) {
				OS.QFont_setOverline(handle, extraData.overline > 0 ? true
						: false);
			}
			if (extraData.strikeOut > -1) {
				OS.QFont_setStrikeOut(handle, extraData.strikeOut > 0 ? true
						: false);
			}
			if (extraData.stretch > -1) {
				OS.QFont_setStretch(handle, extraData.stretch);
			}
			if (extraData.fixedPitch > -1) {
				OS.QFont_setFixedPitch(handle, extraData.fixedPitch > 0 ? true
						: false);
			}
			if (extraData.style > -1) {
				OS.QFont_setStyle(handle, extraData.style);
			}
			if (extraData.weight > -1) {
				OS.QFont_setWeight(handle, extraData.weight);
			}
			if (extraData.styleStrategy > -1) {
				OS.QFont_setStyleStrategy(handle, extraData.styleStrategy);
			}
			extraFontStyle = true;
		}
	}

	/**
	 * Returns <code>true</code> if the font has been disposed, and
	 * <code>false</code> otherwise.
	 * <p>
	 * This method gets the dispose state for the font. When a font has been
	 * disposed, it is an error to invoke any other method using the font.
	 * 
	 * @return <code>true</code> when the font is disposed and
	 *         <code>false</code> otherwise
	 */
	public boolean isDisposed() {
		return handle == 0;
	}

	/**
	 * Returns a string containing a concise, human-readable description of the
	 * receiver.
	 * 
	 * @return a string representation of the receiver
	 */
	public String toString() {
		if (isDisposed())
			return "Font {*DISPOSED*}";
		return "Font {" + handle + "}";
	}

	/*
	 * Invokes platform specific functionality to allocate a new font.
	 */
	static Font qt_new(Device device, int handle) {
		if (handle <= 0)
			return null;
		if (device == null)
			SWT.error(SWT.ERROR_NULL_ARGUMENT);
		Font font = new Font();
		font.handle = handle;
		font.device = device;
		return font;
	}

}
