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
 * This class is the abstract superclass of all device objects,
 * such as the Display device. Devices can have a graphics
 * context (GC) created for them, and they can be drawn on by
 * sending messages to the associated GC.
 */
public abstract class Device implements Drawable
{

    /**
     * The handle to the native display object.
     * (Warning: This field is platform dependent)
     */
    public int handle;

    // Default colours
    private Color COLOR_WHITE;
    private Color COLOR_BLACK;
    private Color COLOR_RED;
    private Color COLOR_DARK_RED;
    private Color COLOR_GREEN;
    private Color COLOR_DARK_GREEN;
    private Color COLOR_YELLOW;
    private Color COLOR_DARK_YELLOW;
    private Color COLOR_BLUE;
    private Color COLOR_DARK_BLUE;
    private Color COLOR_MAGENTA;
    private Color COLOR_DARK_MAGENTA;
    private Color COLOR_CYAN;
    private Color COLOR_DARK_CYAN;
    private Color COLOR_GRAY;
    private Color COLOR_DARK_GRAY;

    /* Debugging */
    public static boolean DEBUG;
    boolean debug = DEBUG;
    boolean tracking = DEBUG;
    Error [] errors;
    Object [] objects;

    protected int createdApplicationId;

    boolean disposing = false;

    protected static Device CurrentDevice;
    protected static Runnable DeviceFinder;
    static
    {
        try
        {
            Class.forName("org.eclipse.swt.widgets.Display");
        }
        catch (Throwable e) {}
    }

    static Device getDevice()
    {
        if (DeviceFinder != null) DeviceFinder.run();
        Device device = CurrentDevice;
        CurrentDevice = null;
        return device;
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
    public Device()
    {
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
    public Device(DeviceData data)
    {
        createdApplicationId = ApplicationIdWrapper.getApplicationId();
        if (createdApplicationId == ApplicationIdWrapper.NO_APPLICATION)
        {
            // No application in the runtime. Something is very wrong.
            // Don't initialise at all so that the Display will appear
            // as disposed.
            return;
        }

        if (data != null)
        {
            debug = data.debug;
            tracking = data.tracking;
        }
        create(data);
        init();
        if (tracking)
        {
            errors = new Error [128];
            objects = new Object [128];
        }

        /* Initialize the system font slot */
//  systemFont = getSystemFont().handle;
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
    protected void checkDevice()
    {
        if (handle == 0)
            SWT.error(SWT.ERROR_DEVICE_DISPOSED);
    }

    /**
     * Creates the device in the operating system.  If the device
     * does not have a handle, this method may do nothing depending
     * on the device.
     * <p>
     * This method is called before <code>init</code>.
     * </p><p>
     * Subclasses are supposed to reimplement this method and not
     * call the <code>super</code> implementation.
     * </p>
     *
     * @param data the DeviceData which describes the receiver
     *
     * @see #init
     */
    protected void create(DeviceData data)
    {
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
    protected void destroy()
    {
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
    public void dispose()
    {

        if (isDisposed() || disposing) return;
        disposing = true;
        checkDevice();
        try
        {
            release();
        }
        finally
        {
            destroy();
            disposing = false;

            if (tracking)
            {
                objects = null;
                errors = null;
            }
        }
    }

    void dispose_Object(Object object)
    {
        for (int i=0; i<objects.length; i++)
        {
            if (objects [i] == object)
            {
                objects [i] = null;
                errors [i] = null;
                return;
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
    public Rectangle getBounds()
    {
        checkDevice();
        return OS.Device_GetBounds(handle);
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
    public Rectangle getClientArea()
    {
        checkDevice();
        return OS.Device_GetClientArea(handle);
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
    public int getDepth()
    {
        checkDevice();
        return OS.Device_GetDepth(handle);
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
    public Point getDPI()
    {
        checkDevice();
        return OS.Device_GetDPI(handle);
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
    public FontData [] getFontList(String faceName, boolean scalable)
    {
        checkDevice();
        FontData [] result = OS.Device_GetFontList(handle, faceName, scalable);
        if (result==null)
        {
            result = new FontData[0];
        }
        return result;
    }

    String getLastError()
    {
        return "";
    }
    /**
     * Returns the matching standard color for the given
     * constant, which should be one of the color constants
     * specified in class <code>SWT</code>. Any value other
     * than one of the SWT color constants which is passed
     * in will result in the color black. This color should
     * not be free'd because it was allocated by the system,
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
    public Color getSystemColor(int id)
    {
        checkDevice();

        switch (id)
        {
        case SWT.COLOR_WHITE:
        {
            if (COLOR_WHITE == null) COLOR_WHITE = new Color(this, 0xFF, 0xFF, 0xFF);
            return COLOR_WHITE;
        }
        case SWT.COLOR_RED:
        {
            if (COLOR_RED == null) COLOR_RED = new Color(this, 0xFF, 0, 0);
            return COLOR_RED;
        }
        case SWT.COLOR_DARK_RED:
        {
            if (COLOR_DARK_RED == null) COLOR_DARK_RED = new Color(this, 0x80, 0, 0);
            return COLOR_DARK_RED;
        }
        case SWT.COLOR_GREEN:
        {
            if (COLOR_GREEN == null) COLOR_GREEN = new Color(this, 0, 0xFF, 0);
            return COLOR_GREEN;
        }
        case SWT.COLOR_DARK_GREEN:
        {
            if (COLOR_DARK_GREEN == null) COLOR_DARK_GREEN = new Color(this, 0, 0x80, 0);
            return COLOR_DARK_GREEN;
        }
        case SWT.COLOR_YELLOW:
        {
            if (COLOR_YELLOW == null) COLOR_YELLOW = new Color(this, 0xFF, 0xFF, 0);
            return COLOR_YELLOW;
        }
        case SWT.COLOR_DARK_YELLOW:
        {
            if (COLOR_DARK_YELLOW == null) COLOR_DARK_YELLOW  = new Color(this, 0x80, 0x80, 0);
            return COLOR_DARK_YELLOW;
        }
        case SWT.COLOR_BLUE:
        {
            if (COLOR_BLUE == null) COLOR_BLUE = new Color(this, 0, 0, 0xFF);
            return COLOR_BLUE;
        }
        case SWT.COLOR_DARK_BLUE:
        {
            if (COLOR_DARK_BLUE == null) COLOR_DARK_BLUE = new Color(this, 0, 0, 0x80);
            return COLOR_DARK_BLUE;
        }
        case SWT.COLOR_MAGENTA:
        {
            if (COLOR_MAGENTA == null) COLOR_MAGENTA = new Color(this, 0xFF, 0, 0xFF);
            return COLOR_MAGENTA;
        }
        case SWT.COLOR_DARK_MAGENTA:
        {
            if (COLOR_DARK_MAGENTA == null) COLOR_DARK_MAGENTA = new Color(this, 0x80, 0, 0x80);
            return COLOR_DARK_MAGENTA;
        }
        case SWT.COLOR_CYAN:
        {
            if (COLOR_CYAN == null) COLOR_CYAN = new Color(this, 0, 0xFF, 0xFF);
            return COLOR_CYAN;
        }
        case SWT.COLOR_DARK_CYAN:
        {
            if (COLOR_DARK_CYAN == null) COLOR_DARK_CYAN = new Color(this, 0, 0x80, 0x80);
            return COLOR_DARK_CYAN;
        }
        case SWT.COLOR_GRAY:
        {
            if (COLOR_GRAY == null) COLOR_GRAY = new Color(this, 0xC0, 0xC0, 0xC0);
            return COLOR_GRAY;
        }
        case SWT.COLOR_DARK_GRAY:
        {
            if (COLOR_DARK_GRAY == null) COLOR_DARK_GRAY = new Color(this, 0x80, 0x80, 0x80);
            return COLOR_DARK_GRAY;
        }
        default:
        {
            if (COLOR_BLACK == null) COLOR_BLACK = new Color(this, 0, 0, 0);
            return COLOR_BLACK;
        }
        }
    }

    /**
     * Returns a reasonable font for applications to use.
     * On some platforms, this will match the "default font"
     * or "system font" if such can be found.  This font
     * should not be free'd because it was allocated by the
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
    public Font getSystemFont()
    {
        // No problem when dispose is called by user as internal_newSystemFont handles this case
        checkDevice();
        return Font.internal_newSystemFont(this, OS.Device_GetSystemFont(handle));
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
    public boolean getWarnings()
    {
        checkDevice();
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
    protected void init()
    {
    }

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
     * @param handle the platform specific GC handle
     * @param data the platform specific GC data
     *
     */
    public abstract void internal_dispose_GC(int handle, GCData data);

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
     *
     */
    public abstract int internal_new_GC(GCData data);

    /**
     * Returns <code>true</code> if the receiver is visible, and
     * <code>false</code> otherwise.
     * <p>
     * <b>IMPORTANT:</b> This method is <em>not</em> part of the public
     * API for <code>Drawable</code>. It is marked public only so that it
     * can be shared within the packages provided by SWT. It is not
     * available on all platforms, and should never be called from
     * application code.
     * </p>
     *
     * @return the receiver's visibility state
     */
    public boolean internal_isVisible()
    {
        return true;
    }

    /**
     * Returns <code>true</code> if the receiver has been disposed,
     * and <code>false</code> otherwise.
     * <p>
     * <b>IMPORTANT:</b> This method is <em>not</em> part of the public
     * API for <code>Drawable</code>. It is marked public only so that it
     * can be shared within the packages provided by SWT. It is not
     * available on all platforms, and should never be called from
     * application code.
     * </p>
     *
     * @return <code>true</code> when the receiver is disposed and <code>false</code> otherwise
     */
    public boolean internal_isDisposed()
    {
        return isDisposed();
    }

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
    public boolean isDisposed()
    {
        return (handle==0);
    }

    void new_Object(Object object)
    {
        for (int i=0; i<objects.length; i++)
        {
            if (objects [i] == null)
            {
                objects [i] = object;
                errors [i] = new Error();
                return;
            }
        }
        Object [] newObjects = new Object [objects.length + 128];
        System.arraycopy(objects, 0, newObjects, 0, objects.length);
        newObjects [objects.length] = object;
        objects = newObjects;
        Error [] newErrors = new Error [errors.length + 128];
        System.arraycopy(errors, 0, newErrors, 0, errors.length);
        newErrors [errors.length] = new Error();
        errors = newErrors;
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
    protected void release()
    {
        if (COLOR_WHITE != null)        COLOR_WHITE.dispose();
        if (COLOR_BLACK != null)        COLOR_BLACK.dispose();
        if (COLOR_RED != null)          COLOR_RED.dispose();
        if (COLOR_DARK_RED != null)     COLOR_DARK_RED.dispose();
        if (COLOR_GREEN != null)        COLOR_GREEN.dispose();
        if (COLOR_DARK_GREEN != null)   COLOR_DARK_GREEN.dispose();
        if (COLOR_YELLOW != null)       COLOR_YELLOW.dispose();
        if (COLOR_DARK_YELLOW != null)  COLOR_DARK_YELLOW.dispose();
        if (COLOR_BLUE != null)         COLOR_BLUE.dispose();
        if (COLOR_DARK_BLUE != null)    COLOR_DARK_BLUE.dispose();
        if (COLOR_MAGENTA != null)      COLOR_MAGENTA.dispose();
        if (COLOR_DARK_MAGENTA != null) COLOR_DARK_MAGENTA.dispose();
        if (COLOR_CYAN != null)         COLOR_CYAN.dispose();
        if (COLOR_DARK_CYAN != null)    COLOR_DARK_CYAN.dispose();
        if (COLOR_GRAY != null)         COLOR_GRAY.dispose();
        if (COLOR_DARK_GRAY != null)    COLOR_DARK_GRAY.dispose();
    }

    /**
     * If the underlying window system supports printing warning messages
     * to the console, setting warnings to <code>true</code> prevents these
     * messages from being printed. If the argument is <code>false</code>
     * message printing is not blocked.
     *
     * @param warnings <code>true</code>if warnings should be handled, and <code>false</code> otherwise
     *
     * @exception SWTException <ul>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void setWarnings(boolean warnings)
    {
        checkDevice();
    }

}
