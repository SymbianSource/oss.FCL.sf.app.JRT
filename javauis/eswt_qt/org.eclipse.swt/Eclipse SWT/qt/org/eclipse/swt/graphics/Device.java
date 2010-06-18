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

import java.util.Vector;

import org.eclipse.swt.SWT;
import org.eclipse.swt.internal.qt.GCData;
import org.eclipse.swt.internal.qt.OS;

/**
 * This class is the abstract superclass of all device objects,
 * such as the Display device and the Printer device. Devices
 * can have a graphics context (GC) created for them, and they
 * can be drawn on by sending messages to the associated GC.
 *
 * @see <a href="http://www.eclipse.org/swt/">Sample code and further information</a>
 */
public abstract class Device implements Drawable {
    
    // Native device handle
    static int paintDevice;

    // Device instance tracking
    static Device[] Devices = new Device[4];
    
    // Debugging
    public static boolean DEBUG; 
    static boolean debug = DEBUG;
    static boolean tracking = DEBUG;
    static Error [] errors;
    static Object [] objects;
    static Object trackingLock;

    // Themed system colors
    static Color COLOR_LIST_BACKGROUND;
    static Color COLOR_LIST_FOREGROUND;
    static Color COLOR_LIST_SELECTION;
    static Color COLOR_LIST_SELECTION_TEXT;
    static Color COLOR_TITLE_BACKGROUND;
    static Color COLOR_TITLE_BACKGROUND_GRADIENT;
    static Color COLOR_TITLE_FOREGROUND;
    static Color COLOR_TITLE_INACTIVE_BACKGROUND;
    static Color COLOR_TITLE_INACTIVE_BACKGROUND_GRADIENT;
    static Color COLOR_TITLE_INACTIVE_FOREGROUND;
    static Color COLOR_WIDGET_BACKGROUND;
    static Color COLOR_WIDGET_BORDER;
    static Color COLOR_WIDGET_DARK_SHADOW;
    static Color COLOR_WIDGET_FOREGROUND;
    static Color COLOR_WIDGET_HIGHLIGHT_SHADOW;
    static Color COLOR_WIDGET_LIGHT_SHADOW;
    static Color COLOR_WIDGET_NORMAL_SHADOW;
    
    // Disposed state flag, instance data
    protected boolean disposed;
    
    // Initialized state flag
    static boolean initialized;
    
	// Internal state flag telling if this is the application's instance
	// or an internal one
    boolean internal;
    
    // Handle to the null Icon. Eventually set by Image.internal_getNullIconHandle
    static int nullIconHandle;
    
    // Default height for FontData
    static final int FONT_DEF_HEIGHT = 12;
        
    static {
        try {
            Class.forName ("org.eclipse.swt.widgets.Display");
        } catch (Throwable e) {}
    }   

/**
 * Constructs a new instance of this class.
 * <p>
 * You must dispose the device when it is no longer required. 
 * </p>
 *
 * @see #create
 * @see #init
 * 
 * @since 3.1
 */
public Device() {
    this(null);
}

/**
 * Constructs a new instance of this class.
 * <p>
 * You must dispose the device when it is no longer required. 
 * </p>
 *
 * @param data the DeviceData which describes the receiver
 *
 * @see #create
 * @see #init
 * @see DeviceData
 */
public Device(DeviceData data) {
	this(data, false);
}

/**
 * <p>
 * <b>IMPORTANT:</b> This constructor is <em>not</em> part of the SWT
 * public API. It should never be referenced from application code.
 * </p>
 */
protected Device(DeviceData data, boolean internal) {
    synchronized (Device.class) {
    	this.internal = internal;
        if (data != null) {
            debug = data.debug;
            tracking = data.tracking;
        }
        if (tracking) {
            errors = new Error [128];
            objects = new Object [128];
            trackingLock = new Object ();
        }
        // In this order
        init();
        register(this);
    	initialized = true;
    }
}

/**
 * Throws an <code>SWTException</code> if the receiver can not
 * be accessed by the caller. This may include both checks on
 * the state of the receiver and more generally on the entire
 * execution context. This method <em>should</em> be called by
 * device implementors to enforce the standard SWT invariants.
 * <p>
 * Currently, it is an error to invoke any method (other than
 * <code>isDisposed()</code> and <code>dispose()</code>) on a
 * device that has had its <code>dispose()</code> method called.
 * </p><p>
 * In future releases of SWT, there may be more or fewer error
 * checks and exceptions may be thrown for different reasons.
 * <p>
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 * </ul>
 */
protected void checkDevice () {
    if (disposed) SWT.error(SWT.ERROR_DEVICE_DISPOSED);
}

synchronized static void deregister (Device device) {
    for (int i=0; i<Devices.length; i++) {
        if (device == Devices [i]) Devices [i] = null;
    }
}

/**
 * Destroys the device in the operating system and releases
 * the device's handle.  If the device does not have a handle,
 * this method may do nothing depending on the device.
 * <p>
 * This method is called after <code>release</code>.
 * </p><p>
 * Subclasses are supposed to reimplement this method and not
 * call the <code>super</code> implementation.
 * </p>
 *
 * @see #dispose
 * @see #release
 */
protected void destroy () {
}

int deviceCount() {
    int res = 0;
    for (int i = 0; i < Devices.length; i++) {
        if (Devices[i] != null && !Devices[i].isDisposed()) {
            res++;
        }
    }
    return res;
}

/**
 * Disposes of the operating system resources associated with
 * the receiver. After this method has been invoked, the receiver
 * will answer <code>true</code> when sent the message
 * <code>isDisposed()</code>.
 *
 * @see #release
 * @see #destroy
 * @see #checkDevice
 */
public void dispose () {
    synchronized (Device.class) {
        if (isDisposed()) return;
        checkDevice ();
        try {
            release();
            destroy();
            deregister(this);
        } finally {
            disposed = true;
        	initialized = false;
        }
        if (tracking) {
            synchronized (trackingLock) {
                printErrors ();
                objects = null;
                errors = null;
                trackingLock = null;
            }
        }
    }
}

/**
 * Must be called from Font, Color, Image and other resources upon destruction.
 * @param object
 */
void dispose_Object (Object object) {
    synchronized (trackingLock) {
        for (int i=0; i<objects.length; i++) {
            if (objects [i] == object) {
                objects [i] = null;
                errors [i] = null;
                return;
            }
        }
    }
}

/**
 * Returns a rectangle describing the receiver's size and location.
 *
 * @return the bounding rectangle
 *
 * @exception SWTException <ul>
 *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
 * </ul>
 */
public Rectangle getBounds () {
    checkDevice ();
    return new Rectangle(0, 0, OS.QPaintDevice_width(paintDevice), 
            OS.QPaintDevice_height(paintDevice));
}

/**
 * Returns a rectangle which describes the area of the
 * receiver which is capable of displaying data.
 * 
 * @return the client area
 *
 * @exception SWTException <ul>
 *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
 * </ul>
 *
 * @see #getBounds
 */
public Rectangle getClientArea () {
    checkDevice ();
    return getBounds ();
}

/**
 * Returns a <code>DeviceData</code> based on the receiver.
 * Modifications made to this <code>DeviceData</code> will not
 * affect the receiver.
 *
 * @return a <code>DeviceData</code> containing the device's data and attributes
 *
 * @exception SWTException <ul>
 *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
 * </ul>
 *
 * @see DeviceData
 */
public DeviceData getDeviceData () {
    checkDevice();
    DeviceData data = new DeviceData ();
    data.debug = debug;
    data.tracking = tracking;
    if (tracking) {
        synchronized (trackingLock) {
            int count = 0, length = objects.length;
            for (int i=0; i<length; i++) {
                if (objects [i] != null) count++;
            }
            int index = 0;
            data.objects = new Object [count];
            data.errors = new Error [count];
            for (int i=0; i<length; i++) {
                if (objects [i] != null) {
                    data.objects [index] = objects [i];
                    data.errors [index] = errors [i];
                    index++;
                }
            }
        }
    } else {
        data.objects = new Object [0];
        data.errors = new Error [0];
    }
    return data;
}

/**
 * Returns the bit depth of the screen, which is the number of
 * bits it takes to represent the number of unique colors that
 * the screen is currently capable of displaying. This number 
 * will typically be one of 1, 8, 15, 16, 24 or 32.
 *
 * @return the depth of the screen
 *
 * @exception SWTException <ul>
 *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
 * </ul>
 */
public int getDepth () {
    checkDevice ();
    return OS.QPaintDevice_depth(paintDevice);
}

/**
 * Returns a point whose x coordinate is the horizontal
 * dots per inch of the display, and whose y coordinate
 * is the vertical dots per inch of the display.
 *
 * @return the horizontal and vertical DPI
 *
 * @exception SWTException <ul>
 *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
 * </ul>
 */
public Point getDPI () {
    checkDevice ();
    return new Point(OS.QPaintDevice_physicalDpiX(paintDevice), 
            OS.QPaintDevice_physicalDpiY(paintDevice));
}

/**
 * Returns <code>FontData</code> objects which describe
 * the fonts that match the given arguments. If the
 * <code>faceName</code> is null, all fonts will be returned.
 *
 * @param faceName the name of the font to look for, or null
 * @param scalable if true only scalable fonts are returned, otherwise only non-scalable fonts are returned.
 * @return the matching font data
 *
 * @exception SWTException <ul>
 *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
 * </ul>
 */
public FontData[] getFontList(String faceName, boolean scalable) {
    checkDevice();
    int fontDbHandle = 0;
    fontDbHandle = OS.QFontDatabase_new();
    String fontFamilies[] = OS.QFontDatabase_families(fontDbHandle);
    Vector fontData = new Vector();// for storing FontData arrays

    for (int i = 0; i < fontFamilies.length; i++) {
        String family = null;
        if (faceName == null)
            family = fontFamilies[i];
        else {
            if (faceName.equals(fontFamilies[i])) {
                family = fontFamilies[i];
            }
            else {
                continue;
            }
        }
        getFontList(fontData, fontDbHandle, family, scalable);
        if (faceName != null) {
            break;
        }
    }

    OS.QFontDatabase_delete(fontDbHandle);
    fontDbHandle = 0;
    
    if (fontData.size() > 0) {
        FontData[] result = new FontData[fontData.size()];
        int size = fontData.size();
        for (int i = 0; i < size; ++i) {
            result[i] = (FontData) fontData.elementAt(i);
        }
        return result;
    } else {
        return new FontData[0];
    }
}

private void getFontList(Vector fontData, int fontDbHandle, String family, boolean onlyScalable) {
    String nativeStyles[] = OS.QFontDatabase_styles(fontDbHandle, family);
    int prevStyle = -1;
    int i = 0;
    // Go trough each native style of the font family, map them to SWT styles 
    // and create a new FontData for each newly found SWT style of the family.
    // Still, it may be possible that there are no native styles for a family,
    // in which case we add a single FontData with default SWT style: NORMAL.
    // In addition we go trough all heights of non scalable font families,
    // creating a new FontData for all heights of all native styles.
    do {
        String nativeStyle = nativeStyles.length > 0 ? nativeStyles[i] : null; 
        boolean isScalable = OS.QFontDatabase_isScalable(fontDbHandle, family, nativeStyle);
        int style = SWT.NORMAL;
        if (OS.QFontDatabase_bold(fontDbHandle, family, nativeStyle))
            style |= SWT.BOLD;
        if (OS.QFontDatabase_italic(fontDbHandle, family, nativeStyle))
            style |= SWT.ITALIC;
        if (style != prevStyle) {
            prevStyle = style;
            int[] heights = null;
            int j = 0;
            if (onlyScalable && isScalable) {
                heights = new int[1];
                heights[0] = FONT_DEF_HEIGHT;
            }
            else if (!onlyScalable && !isScalable) {
                heights = OS.QFontDatabase_swt_heights(fontDbHandle, family, nativeStyle);
            }
            if (heights != null) {
                do {
                    FontData fd = new FontData(family, heights[j], style);
                    fontData.addElement(fd);
                    j++;
                }
                while (!onlyScalable && !isScalable && j < heights.length);
            }
        }
        i++;
    } while (i < nativeStyles.length);
}

/**
 * Returns the matching standard color for the given
 * constant, which should be one of the color constants
 * specified in class <code>SWT</code>. Any value other
 * than one of the SWT color constants which is passed
 * in will result in the color black. This color should
 * not be freed because it was allocated by the system,
 * not the application.
 *
 * @param id the color constant
 * @return the matching color
 *
 * @exception SWTException <ul>
 *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
 * </ul>
 *
 * @see SWT
 */
public Color getSystemColor (int id) {
    checkDevice ();
    int color;
    switch (id) {
        case SWT.COLOR_WHITE:                       color = 0x00FFFFFF;  break;
        case SWT.COLOR_BLACK:                       color = 0x00000000;  break;
        case SWT.COLOR_RED:                         color = 0x000000FF;  break;
        case SWT.COLOR_DARK_RED:                    color = 0x00000080;  break;
        case SWT.COLOR_GREEN:                       color = 0x0000FF00;  break;
        case SWT.COLOR_DARK_GREEN:                  color = 0x00008000;  break;
        case SWT.COLOR_YELLOW:                      color = 0x0000FFFF;  break;
        case SWT.COLOR_DARK_YELLOW:                 color = 0x00008080;  break;
        case SWT.COLOR_BLUE:                        color = 0x00FF0000;  break;
        case SWT.COLOR_DARK_BLUE:                   color = 0x00800000;  break;
        case SWT.COLOR_MAGENTA:                     color = 0x00FF00FF;  break;
        case SWT.COLOR_DARK_MAGENTA:                color = 0x00800080;  break;
        case SWT.COLOR_CYAN:                        color = 0x00FFFF00;  break;
        case SWT.COLOR_DARK_CYAN:                   color = 0x00808000;  break;
        case SWT.COLOR_GRAY:                        color = 0x00C0C0C0;  break;
        case SWT.COLOR_DARK_GRAY:                   color = 0x00808080;  break;
        case SWT.COLOR_LIST_BACKGROUND:             color = COLOR_LIST_BACKGROUND.handle; break;
        case SWT.COLOR_LIST_FOREGROUND:             color = COLOR_LIST_FOREGROUND.handle; break;
        case SWT.COLOR_LIST_SELECTION:              color = COLOR_LIST_SELECTION.handle; break;
        case SWT.COLOR_LIST_SELECTION_TEXT:         color = COLOR_LIST_SELECTION_TEXT.handle; break;
        case SWT.COLOR_TITLE_BACKGROUND:            color = COLOR_TITLE_BACKGROUND.handle; break;
        case SWT.COLOR_TITLE_BACKGROUND_GRADIENT:   color = COLOR_TITLE_BACKGROUND_GRADIENT.handle; break;
        case SWT.COLOR_TITLE_FOREGROUND:            color = COLOR_TITLE_FOREGROUND.handle; break;
        case SWT.COLOR_TITLE_INACTIVE_BACKGROUND:   color = COLOR_TITLE_INACTIVE_BACKGROUND.handle; break;
        case SWT.COLOR_TITLE_INACTIVE_BACKGROUND_GRADIENT: color = COLOR_TITLE_INACTIVE_BACKGROUND_GRADIENT.handle; break;
        case SWT.COLOR_TITLE_INACTIVE_FOREGROUND:   color = COLOR_TITLE_INACTIVE_FOREGROUND.handle; break;
        case SWT.COLOR_WIDGET_BACKGROUND:           color = COLOR_WIDGET_BACKGROUND.handle; break;
        case SWT.COLOR_WIDGET_BORDER:               color = COLOR_WIDGET_BORDER.handle; break;
        case SWT.COLOR_WIDGET_DARK_SHADOW:          color = COLOR_WIDGET_DARK_SHADOW.handle; break;
        case SWT.COLOR_WIDGET_FOREGROUND:           color = COLOR_WIDGET_FOREGROUND.handle; break;
        case SWT.COLOR_WIDGET_HIGHLIGHT_SHADOW:     color = COLOR_WIDGET_HIGHLIGHT_SHADOW.handle; break;
        case SWT.COLOR_WIDGET_LIGHT_SHADOW:         color = COLOR_WIDGET_LIGHT_SHADOW.handle; break;
        case SWT.COLOR_WIDGET_NORMAL_SHADOW:        color = COLOR_WIDGET_NORMAL_SHADOW.handle; break;
        default:
            color = 0; // black
            break;
        }
        return Color.qt_new(this, color);
    }

/**
 * Returns a reasonable font for applications to use.
 * On some platforms, this will match the "default font"
 * or "system font" if such can be found.  This font
 * should not be freed because it was allocated by the
 * system, not the application.
 * <p>
 * Typically, applications which want the default look
 * should simply not set the font on the widgets they
 * create. Widgets are always created with the correct
 * default font for the class of user-interface component
 * they represent.
 * </p>
 *
 * @return a font
 *
 * @exception SWTException <ul>
 *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
 * </ul>
 */
public Font getSystemFont () {
    checkDevice ();
    return Font.qt_new(this, OS.SwtFontCache_cache(OS.QApplication_swt_font_new()));
}

/**
 * Returns <code>true</code> if the underlying window system prints out
 * warning messages on the console, and <code>setWarnings</code>
 * had previously been called with <code>true</code>.
 *
 * @return <code>true</code>if warnings are being handled, and <code>false</code> otherwise
 *
 * @exception SWTException <ul>
 *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
 * </ul>
 */
public boolean getWarnings () {
    checkDevice ();
    return false;
}

/**
 * Initializes any internal resources needed by the
 * device.
 * <p>
 * This method is called after <code>create</code>.
 * </p><p>
 * If subclasses reimplement this method, they must
 * call the <code>super</code> implementation.
 * </p>
 * 
 * @see #create
 */
protected void init () {
	if(!initialized) {
	    int palette = OS.QApplication_swt_palette_new();
	    try {
	        int[] rgb;
	        
	        // List like widgets
	        rgb = OS.QPalette_color(palette, OS.QPALETTE_COLORGROUP_NORMAL, OS.QPALETTE_COLORROLE_BASE);
	        COLOR_LIST_BACKGROUND = new Color(this, rgb[0], rgb[1], rgb[2]);
	        rgb = OS.QPalette_color(palette, OS.QPALETTE_COLORGROUP_NORMAL, OS.QPALETTE_COLORROLE_TEXT);
	        COLOR_LIST_FOREGROUND = new Color(this, rgb[0], rgb[1], rgb[2]);
	    
	        // Highlight
	        rgb = OS.QPalette_color(palette, OS.QPALETTE_COLORGROUP_NORMAL, OS.QPALETTE_COLORROLE_HIGHLIGHT);
	        COLOR_LIST_SELECTION = new Color(this, rgb[0], rgb[1], rgb[2]);
	        COLOR_TITLE_BACKGROUND = new Color(this, rgb[0], rgb[1], rgb[2]);    
	        COLOR_TITLE_BACKGROUND_GRADIENT = new Color(this, rgb[0], rgb[1], rgb[2]);
	        COLOR_WIDGET_HIGHLIGHT_SHADOW = new Color(this, rgb[0], rgb[1], rgb[2]);
	        rgb = OS.QPalette_color(palette, OS.QPALETTE_COLORGROUP_NORMAL, OS.QPALETTE_COLORROLE_HIGHLIGHTTEXT);
	        COLOR_LIST_SELECTION_TEXT = new Color(this, rgb[0], rgb[1], rgb[2]);
	        COLOR_TITLE_FOREGROUND = new Color(this, rgb[0], rgb[1], rgb[2]);
	    
	        // Inactive highlight
	        rgb = OS.QPalette_color(palette, OS.QPALETTE_COLORGROUP_DISABLED, OS.QPALETTE_COLORROLE_HIGHLIGHT);
	        COLOR_TITLE_INACTIVE_BACKGROUND = new Color(this, rgb[0], rgb[1], rgb[2]);
	        COLOR_TITLE_INACTIVE_BACKGROUND_GRADIENT = new Color(this, rgb[0], rgb[1], rgb[2]);
	        rgb = OS.QPalette_color(palette, OS.QPALETTE_COLORGROUP_DISABLED, OS.QPALETTE_COLORROLE_TEXT);
	        COLOR_TITLE_INACTIVE_FOREGROUND = new Color(this, rgb[0], rgb[1], rgb[2]);
	        
	        // Window
	        rgb = OS.QPalette_color(palette, OS.QPALETTE_COLORGROUP_NORMAL, OS.QPALETTE_COLORROLE_WINDOW);
	        COLOR_WIDGET_BACKGROUND = new Color(this, rgb[0], rgb[1], rgb[2]);
	        rgb = OS.QPalette_color(palette, OS.QPALETTE_COLORGROUP_NORMAL, OS.QPALETTE_COLORROLE_WINDOWTEXT);
	        COLOR_WIDGET_FOREGROUND = new Color(this, rgb[0], rgb[1], rgb[2]);
	        
	        // Shadows: Black <= Shadow < [Dark] < [Mid] < [Button] < Midlight < Light <= White
	        rgb = OS.QPalette_color(palette, OS.QPALETTE_COLORGROUP_NORMAL, OS.QPALETTE_COLORROLE_DARK);
	        COLOR_WIDGET_DARK_SHADOW = new Color(this, rgb[0], rgb[1], rgb[2]);    
	        rgb = OS.QPalette_color(palette, OS.QPALETTE_COLORGROUP_NORMAL, OS.QPALETTE_COLORROLE_MID);
	        COLOR_WIDGET_BORDER = new Color(this, rgb[0], rgb[1], rgb[2]);
	        COLOR_WIDGET_NORMAL_SHADOW = new Color(this, rgb[0], rgb[1], rgb[2]);
	        rgb = OS.QPalette_color(palette, OS.QPALETTE_COLORGROUP_NORMAL, OS.QPALETTE_COLORROLE_BUTTON);
	        COLOR_WIDGET_LIGHT_SHADOW = new Color(this, rgb[0], rgb[1], rgb[2]);
	    } finally {
	        OS.QPalette_delete(palette);
	    }
	    paintDevice = OS.QApplication_swt_desktopPaintDevice();
	}
}

/**  
 * Invokes platform specific functionality to allocate a new GC handle.
 * <p>
 * <b>IMPORTANT:</b> This method is <em>not</em> part of the public
 * API for <code>Device</code>. It is marked public only so that it
 * can be shared within the packages provided by SWT. It is not
 * available on all platforms, and should never be called from
 * application code.
 * </p>
 *
 * @param data the platform specific GC data 
 * @return the platform specific GC handle
 */
public abstract int internal_new_GC (GCData data);

/**  
 * Invokes platform specific functionality to dispose a GC handle.
 * <p>
 * <b>IMPORTANT:</b> This method is <em>not</em> part of the public
 * API for <code>Device</code>. It is marked public only so that it
 * can be shared within the packages provided by SWT. It is not
 * available on all platforms, and should never be called from
 * application code.
 * </p>
 *
 * @param hDC the platform specific GC handle
 * @param data the platform specific GC data 
 */
public abstract void internal_dispose_GC(GCData data);

/**
 * Returns <code>true</code> if the device has been disposed,
 * and <code>false</code> otherwise.
 * <p>
 * This method gets the dispose state for the device.
 * When a device has been disposed, it is an error to
 * invoke any other method using the device.
 *
 * @return <code>true</code> when the device is disposed and <code>false</code> otherwise
 */
public boolean isDisposed () {
    synchronized (Device.class) {
        return disposed;
    }
}

void new_Object (Object object) {
    synchronized (trackingLock) {
        for (int i=0; i<objects.length; i++) {
            if (objects [i] == null) {
                objects [i] = object;
                errors [i] = new Error ();
                return;
            }
        }
        Object [] newObjects = new Object [objects.length + 128];
        System.arraycopy (objects, 0, newObjects, 0, objects.length);
        newObjects [objects.length] = object;
        objects = newObjects;
        Error [] newErrors = new Error [errors.length + 128];
        System.arraycopy (errors, 0, newErrors, 0, errors.length);
        newErrors [errors.length] = new Error ();
        errors = newErrors;
    }
}

static synchronized void register (Device device) {
    for (int i=0; i<Devices.length; i++) {
        if (Devices [i] == null) {
            Devices [i] = device;
            return;
        }
    }
    Device [] newDevices = new Device [Devices.length + 4];
    System.arraycopy (Devices, 0, newDevices, 0, Devices.length);
    newDevices [Devices.length] = device;
    Devices = newDevices;
}

void printErrors () {
    if (!DEBUG) return;
    if (tracking) {
        synchronized (trackingLock) {
            if (objects == null || errors == null) return;
            int objectCount = 0;
            int colors = 0;
            int fonts = 0;
            int gcs = 0;
            int images = 0;
            for (int i=0; i<objects.length; i++) {
                Object object = objects [i];
                if (object != null) {
                    objectCount++;
                    if (object instanceof Color) colors++;
                    if (object instanceof Font) fonts++;
                    if (object instanceof GC) gcs++;
                    if (object instanceof Image) images++;
                }
            }
            if (objectCount != 0) {
                String string = "Summary: ";
                if (colors != 0) string += colors + " Color(s), ";
                if (fonts != 0) string += fonts + " Font(s), ";
                if (gcs != 0) string += gcs + " GC(s), ";
                if (images != 0) string += images + " Image(s), ";
                if (string.length () != 0) {
                    string = string.substring (0, string.length () - 2);
                    System.err.println (string);
                }
                for (int i=0; i<errors.length; i++) {
                    if (errors [i] != null) errors [i].printStackTrace ();
                }
            }
        }
    }
}

/**
 * Releases any internal resources back to the operating
 * system and clears all fields except the device handle.
 * <p>
 * When a device is destroyed, resources that were acquired
 * on behalf of the programmer need to be returned to the
 * operating system.  For example, if the device allocated a
 * font to be used as the system font, this font would be
 * freed in <code>release</code>.  Also,to assist the garbage
 * collector and minimize the amount of memory that is not
 * reclaimed when the programmer keeps a reference to a
 * disposed device, all fields except the handle are zero'd.
 * The handle is needed by <code>destroy</code>.
 * </p>
 * This method is called before <code>destroy</code>.
 * </p><p>
 * If subclasses reimplement this method, they must
 * call the <code>super</code> implementation.
 * </p>
 *
 * @see #dispose
 * @see #destroy
 */
protected void release () {
    // Static data follows
    if (deviceCount() == 1) {
        if ( nullIconHandle != 0 ){ 
            OS.QIcon_delete( nullIconHandle );
            nullIconHandle = 0;
        }
        COLOR_LIST_BACKGROUND.dispose();
        COLOR_LIST_FOREGROUND.dispose();
        COLOR_LIST_SELECTION.dispose();
        COLOR_LIST_SELECTION_TEXT.dispose();
        COLOR_TITLE_BACKGROUND.dispose();
        COLOR_TITLE_BACKGROUND_GRADIENT.dispose();
        COLOR_TITLE_FOREGROUND.dispose();
        COLOR_TITLE_INACTIVE_BACKGROUND.dispose();
        COLOR_TITLE_INACTIVE_BACKGROUND_GRADIENT.dispose();
        COLOR_TITLE_INACTIVE_FOREGROUND.dispose();
        COLOR_WIDGET_BACKGROUND.dispose();
        COLOR_WIDGET_BORDER.dispose();
        COLOR_WIDGET_DARK_SHADOW.dispose();
        COLOR_WIDGET_FOREGROUND.dispose();
        COLOR_WIDGET_HIGHLIGHT_SHADOW.dispose();
        COLOR_WIDGET_LIGHT_SHADOW.dispose();
        COLOR_WIDGET_NORMAL_SHADOW.dispose();
        
        COLOR_LIST_BACKGROUND = null;
        COLOR_LIST_FOREGROUND = null;
        COLOR_LIST_SELECTION = null;
        COLOR_LIST_SELECTION_TEXT = null;
        COLOR_TITLE_BACKGROUND = null;
        COLOR_TITLE_BACKGROUND_GRADIENT = null;
        COLOR_TITLE_FOREGROUND = null;
        COLOR_TITLE_INACTIVE_BACKGROUND = null;
        COLOR_TITLE_INACTIVE_BACKGROUND_GRADIENT = null;
        COLOR_TITLE_INACTIVE_FOREGROUND = null;
        COLOR_WIDGET_BACKGROUND = null;
        COLOR_WIDGET_BORDER = null;
        COLOR_WIDGET_DARK_SHADOW = null;
        COLOR_WIDGET_FOREGROUND = null;
        COLOR_WIDGET_HIGHLIGHT_SHADOW = null;
        COLOR_WIDGET_LIGHT_SHADOW = null;
        COLOR_WIDGET_NORMAL_SHADOW = null;
        
        paintDevice = 0;
    }
    
    // Non static data follows
    // No non-static data to release
}

/**
 * If the underlying window system supports printing warning messages
 * to the console, setting warnings to <code>false</code> prevents these
 * messages from being printed. If the argument is <code>true</code> then
 * message printing is not blocked.
 *
 * @param warnings <code>true</code>if warnings should be printed, and <code>false</code> otherwise
 *
 * @exception SWTException <ul>
 *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
 * </ul>
 */
public void setWarnings (boolean warnings) {
    checkDevice ();
}

}
