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
package org.eclipse.swt.widgets;

import java.util.Vector;

import org.eclipse.swt.internal.symbian.*;
import org.eclipse.swt.*;
import org.eclipse.swt.graphics.*;

/**
 * Instances of this class are responsible for managing the
 * connection between SWT and the underlying operating
 * system. Their most important function is to implement
 * the SWT event loop in terms of the platform event model.
 * They also provide various methods for accessing information
 * about the operating system, and have overall control over
 * the operating system resources which SWT allocates.
 * <p>
 * Applications which are built with SWT will <em>almost always</em>
 * require only a single display. In particular, some platforms
 * which SWT supports will not allow more than one <em>active</em>
 * display. In other words, some platforms do not support
 * creating a new display if one already exists that has not been
 * sent the <code>dispose()</code> message.
 * <p>
 * In SWT, the thread which creates a <code>Display</code>
 * instance is distinguished as the <em>user-interface thread</em>
 * for that display.
 * </p>
 * The user-interface thread for a particular display has the
 * following special attributes:
 * <ul>
 * <li>
 * The event loop for that display must be run from the thread.
 * </li>
 * <li>
 * Some SWT API methods (notably, most of the public methods in
 * <code>Widget</code> and its subclasses), may only be called
 * from the thread. (To support multi-threaded user-interface
 * applications, class <code>Display</code> provides inter-thread
 * communication methods which allow threads other than the
 * user-interface thread to request that it perform operations
 * on their behalf.)
 * </li>
 * <li>
 * The thread is not allowed to construct other
 * <code>Display</code>s until that display has been disposed.
 * (Note that, this is in addition to the restriction mentioned
 * above concerning platform support for multiple displays. Thus,
 * the only way to have multiple simultaneously active displays,
 * even on platforms which support it, is to have multiple threads.)
 * </li>
 * </ul>
 * Enforcing these attributes allows SWT to be implemented directly
 * on the underlying operating system's event model. This has
 * numerous benefits including smaller footprint, better use of
 * resources, safer memory management, clearer program logic,
 * better performance, and fewer overall operating system threads
 * required. The down side however, is that care must be taken
 * (only) when constructing multi-threaded applications to use the
 * inter-thread communication mechanisms which this class provides
 * when required.
 * </p><p>
 * All SWT API methods which may only be called from the user-interface
 * thread are distinguished in their documentation by indicating that
 * they throw the "<code>ERROR_THREAD_INVALID_ACCESS</code>"
 * SWT exception.
 * </p>
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>(none)</dd>
 * <dt><b>Events:</b></dt>
 * <dd>Close, Dispose, Settings</dd>
 * </dl>
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 * @see #syncExec
 * @see #asyncExec
 * @see #wake
 * @see #readAndDispatch
 * @see #sleep
 * @see #dispose
 */

public class Display extends Device
{


    private int displayHandle;

    // System colours
    private Color COLOR_WIDGET_DARK_SHADOW;
    private Color COLOR_WIDGET_NORMAL_SHADOW;
    private Color COLOR_WIDGET_LIGHT_SHADOW;
    private Color COLOR_WIDGET_HIGHLIGHT_SHADOW;
    private Color COLOR_WIDGET_FOREGROUND;
    private Color COLOR_WIDGET_BACKGROUND;
    private Color COLOR_WIDGET_BORDER;
    private Color COLOR_LIST_FOREGROUND;
    private Color COLOR_LIST_BACKGROUND;
    private Color COLOR_LIST_SELECTION;
    private Color COLOR_LIST_SELECTION_TEXT;
    private Color COLOR_TITLE_FOREGROUND;
    private Color COLOR_TITLE_BACKGROUND;
    private Color COLOR_TITLE_BACKGROUND_GRADIENT;
    private Color COLOR_TITLE_INACTIVE_FOREGROUND;
    private Color COLOR_TITLE_INACTIVE_BACKGROUND;
    private Color COLOR_TITLE_INACTIVE_BACKGROUND_GRADIENT;

    private Vector shells = new Vector();
    private boolean firstPaintComplete;

    Event [] eventQueue;
    EventTable eventTable, filterTable;

    /* Sync/Async Widget Communication */
    Synchronizer synchronizer = new Synchronizer(this);
    Thread thread;

    /* Display Shutdown */
    Runnable [] disposeList;

    /* Timer Runnables */
    Runnable[] timers;

    /* Display Data */
    Object data;
    String [] keys;
    Object [] values;

    /* Displays */
    static Display [] Displays = new Display [4];

    /* Application Name */
    static String APP_NAME;

    /* Package Name */
    static final String PACKAGE_PREFIX = "org.eclipse.swt.widgets."; //$NON-NLS-1$

    /* eRCP package name */
    static final String ERCP_MOBILE_PACKAGE_PREFIX = "org.eclipse.ercp.swt.mobile."; //$NON-NLS-1$

    /* extension package name */
    static final String INTERNAL_EXTENSION_PACKAGE_PREFIX = "org.eclipse.swt.internal.extension.";

    /* The Device Finder let us find the current device/display */
    static
    {
        DeviceFinder = new Runnable()
        {
            public void run()
            {
                Device device = getCurrent();
                if (device == null)
                {
                    device = getDefault();
                }
                setDevice(device);
            }
        };
    }

    static void setDevice(Device device)
    {
        CurrentDevice = device;
    }

    int paintListenersCount = 0;

    /* Buffer of paint listener operations, internal */
    GCBuffer sharedBuffer;

    /**
     * Constructs a new instance of this class.
     * <p>
     * Note: The resulting display is marked as the <em>current</em>
     * display. If this is the first display which has been
     * constructed since the application started, it is also
     * marked as the <em>default</em> display.
     * </p>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if called from a thread that already created an existing display</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     *
     * @see #getCurrent
     * @see #getDefault
     * @see Widget#checkSubclass
     * @see Shell
     */
    public Display()
    {
        super();
    }
    /* For the time being this constructor which is not part of eSWT is let unchanged
     * and is not removed since it seems it is required by Device for debug purpose
     */
    public Display(DeviceData data)
    {
        super(data);
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notifed when an event of the given type occurs anywhere
     * in this display. When the event does occur, the listener is
     * notified by sending it the <code>handleEvent()</code> message.
     *
     * Setting the type of an event to <code>SWT.None</code> from
     * within the <code>handleEvent()</code> method can be used to
     * change the event type and stop subsequent Java listeners
     * from running. Because event filters run before other listeners,
     * event filters can both block other listeners and set arbitrary
     * fields within an event. For this reason, event filters are both
     * powerful and dangerous. They should generally be avoided for
     * performance, debugging and code maintenance reasons.
     * </p>
     * @param eventType the type of event to listen for
     * @param listener the listener which should be notified when the event occurs
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see Listener
     * @see #removeFilter
     * @see #removeListener
     *
     * @since 3.0
     */
    public void addFilter(int eventType, Listener listener)
    {
        checkDevice();
        if (listener == null) error(SWT.ERROR_NULL_ARGUMENT);
        if (filterTable == null) filterTable = new EventTable();
        filterTable.hook(eventType, listener);
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notifed when an event of the given type occurs. When the
     * event does occur in the display, the listener is notified by
     * sending it the <code>handleEvent()</code> message.
     *
     * @param eventType the type of event to listen for
     * @param listener the listener which should be notified when the event occurs
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see Listener
     * @see #removeListener
     *
     * @since 2.0
     */
    public void addListener(int eventType, Listener listener)
    {
        checkDevice();
        if (listener == null) error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) eventTable = new EventTable();
        eventTable.hook(eventType, listener);
    }

    /**
     * Causes the <code>run()</code> method of the runnable to
     * be invoked by the user-interface thread at the next
     * reasonable opportunity. The caller of this method continues
     * to run in parallel, and is not notified when the
     * runnable has completed.  Specifying <code>null</code> as the
     * runnable simply wakes the user-interface thread when run.
     * <p>
     * Note that at the time the runnable is invoked, widgets
     * that have the receiver as their display may have been
     * disposed. Therefore, it is necessary to check for this
     * case inside the runnable before accessing the widget.
     * </p>
     *
     * @param runnable code to run on the user-interface thread.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see #syncExec
     */
    public void asyncExec(Runnable runnable)
    {
        if (isDisposed()) error(SWT.ERROR_DEVICE_DISPOSED);
        synchronizer.asyncExec(runnable);
    }

    /**
     * Causes the system hardware to emit a short sound
     * (if it supports this capability).
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void beep()
    {
        checkDevice();
        OS.Display_Beep(displayHandle);
    }

    /**
     * Checks that this class can be subclassed.
     * <p>
     * IMPORTANT: See the comment in <code>Widget.checkSubclass()</code>.
     * </p>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     *
     * @see Widget#checkSubclass
     */
    protected void checkSubclass()
    {
        if (!isValidClass(getClass())) error(SWT.ERROR_INVALID_SUBCLASS);
    }

    protected void checkDevice()
    {
        if (thread == null) error(SWT.ERROR_WIDGET_DISPOSED);
        if (thread != Thread.currentThread()) error(SWT.ERROR_THREAD_INVALID_ACCESS);
        if (isDisposed()) error(SWT.ERROR_DEVICE_DISPOSED);
    }

    static synchronized void checkDisplay(Thread thread)
    {
        for (int i=0; i<Displays.length; i++)
        {
            if (Displays [i] != null && Displays [i].thread == thread)
            {
                SWT.error(SWT.ERROR_THREAD_INVALID_ACCESS);
            }
        }
    }

    /**
     * Requests that the connection between SWT and the underlying
     * operating system be closed.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see Device#dispose
     *
     * @since 2.0
     */
    public void close()
    {
        checkDevice();
        if (sendCloseEvent())
            dispose();
    }

    /**
     * Creates the device in the operating system.  If the device
     * does not have a handle, this method may do nothing depending
     * on the device.
     * <p>
     * This method is called before <code>init</code>.
     * </p>
     *
     * @param data the DeviceData which describes the receiver
     *
     * @see #init
     */
    protected void create(DeviceData data)
    {
        checkSubclass();
        checkDisplay(thread = Thread.currentThread());
        UiToolKitRegistrationWrapper.Register();
        createDisplay(data);
        register(this);
        if (DefaultDisplayWrapper.get() == null) DefaultDisplayWrapper.set(this);
        if (APP_NAME != null)
        {
            OS.Display_SetAppName(APP_NAME);
        }
    }

    void createDisplay(DeviceData data)
    {

        displayHandle = OS.Display_New(this, createdApplicationId);

        handle = OS.Display_DeviceHandle(displayHandle);

        ForegroundNotifyWrapper.setForeground(true);
    }

    static synchronized void deregister(Display display)
    {
        for (int i=0; i<Displays.length; i++)
        {
            if (display == Displays [i]) Displays [i] = null;
        }
    }

    /**
     * Destroys the device in the operating system and releases
     * the device's handle.  If the device does not have a handle,
     * this method may do nothing depending on the device.
     * <p>
     * This method is called after <code>release</code>.
     * </p>
     * @see #dispose
     * @see #release
     */
    protected void destroy()
    {
        this.shells.removeAllElements();
        if (this == DefaultDisplayWrapper.get()) DefaultDisplayWrapper.set(null);
        deregister(this);
        destroyDisplay();
        UiToolKitRegistrationWrapper.UnRegister();
    }

    void destroyDisplay()
    {
        OS.Display_Dispose(displayHandle);
        displayHandle = 0;
        handle = 0;
    }

    /**
     * Causes the <code>run()</code> method of the runnable to
     * be invoked by the user-interface thread just before the
     * receiver is disposed.  Specifying a <code>null</code> runnable
     * is ignored.
     *
     * @param runnable code to run at dispose time.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public void disposeExec(Runnable runnable)
    {
        checkDevice();
        if (disposeList == null) disposeList = new Runnable [4];
        for (int i=0; i<disposeList.length; i++)
        {
            if (disposeList [i] == null)
            {
                disposeList [i] = runnable;
                return;
            }
        }
        Runnable [] newDisposeList = new Runnable [disposeList.length + 4];
        System.arraycopy(disposeList, 0, newDisposeList, 0, disposeList.length);
        newDisposeList [disposeList.length] = runnable;
        disposeList = newDisposeList;
    }

    /**
     * Does whatever display specific cleanup is required, and then
     * uses the code in <code>SWTError.error</code> to handle the error.
     *
     * @param code the descriptive error code
     *
     * @see SWTError#error
     */
    void error(int code)
    {
        SWT.error(code);
    }

    boolean filterEvent(Event event)
    {
        if (filterTable != null) filterTable.sendEvent(event);
        return false;
    }

    boolean filters(int eventType)
    {
        if (filterTable == null) return false;
        return filterTable.hooks(eventType);
    }

    /**
     * Returns the display which the given thread is the
     * user-interface thread for, or null if the given thread
     * is not a user-interface thread for any display.
     *
     * @param thread the user-interface thread
     * @return the display for the given thread
     */
    public static synchronized Display findDisplay(Thread thread)
    {
        for (int i=0; i<Displays.length; i++)
        {
            Display display = Displays [i];
            if (display != null && display.thread == thread)
            {
                return display;
            }
        }
        return null;
    }

    /**
     * Returns the currently active <code>Shell</code>, or null
     * if no shell belonging to the currently running application
     * is active.
     *
     * @return the active shell or null
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public Shell getActiveShell()
    {
        checkDevice();
        return OS.Display_GetActiveShell(displayHandle);
    }

    /**
     * Returns a rectangle describing the receiver's size and location.
     *
     * @return the bounding rectangle
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public Rectangle getBounds()
    {
        return super.getBounds();
    }

    /**
     * Returns the display which the currently running thread is
     * the user-interface thread for, or null if the currently
     * running thread is not a user-interface thread for any display.
     *
     * @return the current display
     */
    public static synchronized Display getCurrent()
    {
        return findDisplay(Thread.currentThread());
    }

    /**
     * Returns a rectangle which describes the area of the
     * receiver which is capable of displaying data.
     *
     * @return the client area
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see #getBounds
     */
    public Rectangle getClientArea()
    {
        return super.getClientArea();
    }


    /**
     * Returns the default display. One is created (making the
     * thread that invokes this method its user-interface thread)
     * if it did not already exist.
     *
     * @return the default display
     */
    public static synchronized Display getDefault()
    {
        Display display = DefaultDisplayWrapper.get();
        if (display == null)
        {
            display = new Display();
            DefaultDisplayWrapper.set(display);
        }
        return display;
    }

    static boolean isValidClass(Class clazz)
    {
        String name = clazz.getName();
        int index = name.lastIndexOf('.');
        return (name.substring(0, index + 1).equals(PACKAGE_PREFIX)
                || name.substring(0, index + 1).equals(
                    ERCP_MOBILE_PACKAGE_PREFIX)|| name.substring(0,
                            index + 1).equals(INTERNAL_EXTENSION_PACKAGE_PREFIX));
    }



    /**
     * Returns the application defined property of the receiver
     * with the specified name, or null if it has not been set.
     * <p>
     * Applications may have associated arbitrary objects with the
     * receiver in this fashion. If the objects stored in the
     * properties need to be notified when the display is disposed
     * of, it is the application's responsibility to provide a
     * <code>disposeExec()</code> handler which does so.
     * </p>
     *
     * @param key the name of the property
     * @return the value of the property or null if it has not been set
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the key is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see #setData(String, Object)
     * @see #disposeExec(Runnable)
     */
    public Object getData(String key)
    {
        checkDevice();
        if (key == null) error(SWT.ERROR_NULL_ARGUMENT);
        if (keys == null) return null;
        for (int i=0; i<keys.length; i++)
        {
            if (keys [i].equals(key)) return values [i];
        }
        return null;
    }

    /**
     * Returns the application defined, display specific data
     * associated with the receiver, or null if it has not been
     * set. The <em>display specific data</em> is a single,
     * unnamed field that is stored with every display.
     * <p>
     * Applications may put arbitrary objects in this field. If
     * the object stored in the display specific data needs to
     * be notified when the display is disposed of, it is the
     * application's responsibility to provide a
     * <code>disposeExec()</code> handler which does so.
     * </p>
     *
     * @return the display specific data
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see #setData(Object)
     * @see #disposeExec(Runnable)
     */
    public Object getData()
    {
        checkDevice();
        return data;
    }

    /**
     * Returns the button dismissal alignment, one of <code>LEFT</code> or <code>RIGHT</code>.
     * The button dismissal alignment is the ordering that should be used when positioning the
     * default dismissal button for a dialog.  For example, in a dialog that contains an OK and
     * CANCEL button, on platforms where the button dismissal alignment is <code>LEFT</code>, the
     * button ordering should be OK/CANCEL.  When button dismissal alignment is <code>RIGHT</code>,
     * the button ordering should be CANCEL/OK.
     *
     * @return the button dismissal order
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @since 2.1
     */
    public int getDismissalAlignment()
    {
        checkDevice();
        return SWT.LEFT;
    }


    /**
     * Returns the longest duration, in milliseconds, between
     * two mouse button clicks that will be considered a
     * <em>double click</em> by the underlying operating system.
     *
     * @return the double click time
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public int getDoubleClickTime()
    {
        checkDevice();
        return OS.Display_getDoubleClickTime(displayHandle);
    }

    /**
     * Returns the control which currently has keyboard focus,
     * or null if keyboard events are not currently going to
     * any of the controls built by the currently running
     * application.
     *
     * @return the control under the cursor
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public Control getFocusControl()
    {
        checkDevice();
        return OS.Display_GetFocusControl(displayHandle);
    }

    /**
     * Returns the maximum allowed depth of icons on this display, in bits per pixel.
     * On some platforms, this may be different than the actual depth of the display.
     *
     * @return the maximum icon depth
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see Device#getDepth
     */
    public int getIconDepth()
    {
        checkDevice();
        return OS.Display_getIconDepth(displayHandle);
    }

    int getLastEventTime()
    {
        return internal_getArbitraryTimeMeasure();
    }

    static public int internal_getArbitraryTimeMeasure()
    {
        return (int)(System.currentTimeMillis()/20L);
    }

    /**
     * Returns a (possibly empty) array containing all shells which have
     * not been disposed and have the receiver as their display.
     *
     * @return the receiver's shells
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public Shell [] getShells()
    {
        checkDevice();
        return OS.Display_GetShells(displayHandle);
    }

    /**
     * Returns the thread that has invoked <code>syncExec</code>
     * or null if no such runnable is currently being invoked by
     * the user-interface thread.
     * <p>
     * Note: If a runnable invoked by asyncExec is currently
     * running, this method will return null.
     * </p>
     *
     * @return the receiver's sync-interface thread
     *
     * @exception SWTException <ul>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public Thread getSyncThread()
    {
        if (isDisposed()) error(SWT.ERROR_DEVICE_DISPOSED);
        return synchronizer.syncThread;
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
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
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
        case SWT.COLOR_WIDGET_DARK_SHADOW:
        {
            if (COLOR_WIDGET_DARK_SHADOW == null) COLOR_WIDGET_DARK_SHADOW = new Color(this, OS.Device_GetSystemColor(handle, SWT.COLOR_WIDGET_DARK_SHADOW));
            return COLOR_WIDGET_DARK_SHADOW;
        }
        case SWT.COLOR_WIDGET_NORMAL_SHADOW:
        {
            if (COLOR_WIDGET_NORMAL_SHADOW == null) COLOR_WIDGET_NORMAL_SHADOW = new Color(this, OS.Device_GetSystemColor(handle, SWT.COLOR_WIDGET_NORMAL_SHADOW));
            return COLOR_WIDGET_NORMAL_SHADOW;
        }
        case SWT.COLOR_WIDGET_LIGHT_SHADOW:
        {
            if (COLOR_WIDGET_LIGHT_SHADOW == null) COLOR_WIDGET_LIGHT_SHADOW = new Color(this, OS.Device_GetSystemColor(handle, SWT.COLOR_WIDGET_LIGHT_SHADOW));
            return COLOR_WIDGET_LIGHT_SHADOW;
        }
        case SWT.COLOR_WIDGET_HIGHLIGHT_SHADOW:
        {
            if (COLOR_WIDGET_HIGHLIGHT_SHADOW == null) COLOR_WIDGET_HIGHLIGHT_SHADOW = new Color(this, OS.Device_GetSystemColor(handle, SWT.COLOR_WIDGET_HIGHLIGHT_SHADOW));
            return COLOR_WIDGET_HIGHLIGHT_SHADOW;
        }
        case SWT.COLOR_WIDGET_FOREGROUND:
        {
            if (COLOR_WIDGET_FOREGROUND == null) COLOR_WIDGET_FOREGROUND = new Color(this, OS.Device_GetSystemColor(handle, SWT.COLOR_WIDGET_FOREGROUND));
            return COLOR_WIDGET_FOREGROUND;
        }
        case SWT.COLOR_WIDGET_BACKGROUND:
        {
            if (COLOR_WIDGET_BACKGROUND == null) COLOR_WIDGET_BACKGROUND = new Color(this, OS.Device_GetSystemColor(handle, SWT.COLOR_WIDGET_BACKGROUND));
            return COLOR_WIDGET_BACKGROUND;
        }
        case SWT.COLOR_WIDGET_BORDER:
        {
            if (COLOR_WIDGET_BORDER == null) COLOR_WIDGET_BORDER = new Color(this, OS.Device_GetSystemColor(handle, SWT.COLOR_WIDGET_BORDER));
            return COLOR_WIDGET_BORDER;
        }
        case SWT.COLOR_LIST_FOREGROUND:
        {
            if (COLOR_LIST_FOREGROUND == null) COLOR_LIST_FOREGROUND = new Color(this, OS.Device_GetSystemColor(handle, SWT.COLOR_LIST_FOREGROUND));
            return COLOR_LIST_FOREGROUND;
        }
        case SWT.COLOR_LIST_BACKGROUND:
        {
            if (COLOR_LIST_BACKGROUND == null) COLOR_LIST_BACKGROUND = new Color(this, OS.Device_GetSystemColor(handle, SWT.COLOR_LIST_BACKGROUND));
            return COLOR_LIST_BACKGROUND;
        }
        case SWT.COLOR_LIST_SELECTION:
        {
            if (COLOR_LIST_SELECTION == null) COLOR_LIST_SELECTION = new Color(this, OS.Device_GetSystemColor(handle, SWT.COLOR_LIST_SELECTION));
            return COLOR_LIST_SELECTION;
        }
        case SWT.COLOR_LIST_SELECTION_TEXT:
        {
            if (COLOR_LIST_SELECTION_TEXT == null) COLOR_LIST_SELECTION_TEXT = new Color(this, OS.Device_GetSystemColor(handle, SWT.COLOR_LIST_SELECTION_TEXT));
            return COLOR_LIST_SELECTION_TEXT;
        }
        case SWT.COLOR_TITLE_FOREGROUND:
        {
            if (COLOR_TITLE_FOREGROUND == null) COLOR_TITLE_FOREGROUND = new Color(this, OS.Device_GetSystemColor(handle, SWT.COLOR_TITLE_FOREGROUND));
            return COLOR_TITLE_FOREGROUND;
        }
        case SWT.COLOR_TITLE_BACKGROUND:
        {
            if (COLOR_TITLE_BACKGROUND == null) COLOR_TITLE_BACKGROUND = new Color(this, OS.Device_GetSystemColor(handle, SWT.COLOR_TITLE_BACKGROUND));
            return COLOR_TITLE_BACKGROUND;
        }
        case SWT.COLOR_TITLE_BACKGROUND_GRADIENT:
        {
            if (COLOR_TITLE_BACKGROUND_GRADIENT == null) COLOR_TITLE_BACKGROUND_GRADIENT = new Color(this, OS.Device_GetSystemColor(handle, SWT.COLOR_TITLE_BACKGROUND_GRADIENT));
            return COLOR_TITLE_BACKGROUND_GRADIENT;
        }
        case SWT.COLOR_TITLE_INACTIVE_FOREGROUND:
        {
            if (COLOR_TITLE_INACTIVE_FOREGROUND == null) COLOR_TITLE_INACTIVE_FOREGROUND = new Color(this, OS.Device_GetSystemColor(handle, SWT.COLOR_TITLE_INACTIVE_FOREGROUND));
            return COLOR_TITLE_INACTIVE_FOREGROUND;
        }
        case SWT.COLOR_TITLE_INACTIVE_BACKGROUND:
        {
            if (COLOR_TITLE_INACTIVE_BACKGROUND == null) COLOR_TITLE_INACTIVE_BACKGROUND = new Color(this, OS.Device_GetSystemColor(handle, SWT.COLOR_TITLE_INACTIVE_BACKGROUND));
            return COLOR_TITLE_INACTIVE_BACKGROUND;
        }
        case SWT.COLOR_TITLE_INACTIVE_BACKGROUND_GRADIENT:
        {
            if (COLOR_TITLE_INACTIVE_BACKGROUND_GRADIENT == null) COLOR_TITLE_INACTIVE_BACKGROUND_GRADIENT = new Color(this, OS.Device_GetSystemColor(handle, SWT.COLOR_TITLE_INACTIVE_BACKGROUND_GRADIENT));
            return COLOR_TITLE_INACTIVE_BACKGROUND_GRADIENT;
        }
        default:
            return super.getSystemColor(id);
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
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public Font getSystemFont()
    {
        return super.getSystemFont();
    }

    /**
     * Returns the single instance of the system tray or null
     * when there is no system tray available for the platform.
     *
     * @return the system tray or <code>null</code>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @since 1.1
     */
    public Tray getSystemTray()
    {
        return null;
    }

    /** Returns the user-interface thread for the receiver.
     *
     * @return the receiver's user-interface thread
     *
     * @exception SWTException <ul>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     */
    public Thread getThread()
    {
        if (isDisposed()) error(SWT.ERROR_DEVICE_DISPOSED);
        return thread;
    }

    /**
     * Invokes platform specific functionality to allocate a new GC handle.
     * <p>
     * <b>IMPORTANT:</b> This method is <em>not</em> part of the public
     * API for <code>Display</code>. It is marked public only so that it
     * can be shared within the packages provided by SWT. It is not
     * available on all platforms, and should never be called from
     * application code.
     * </p>
     *
     * @param data the platform specific GC data
     * @return the platform specific GC handle
     *
     * @exception SWTException <ul>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_NO_HANDLES if a handle could not be obtained for image creation</li>
     * </ul>
     */
    public int internal_new_GC(GCData data)
    {
        return OS.Drawable_NewGc(handle);
    }

    /**
     * Initializes any internal resources needed by the
     * device.
     * <p>
     * This method is called after <code>create</code>.
     * </p>
     *
     * @see #create
     */
    protected void init()
    {
        super.init();
    }

    /**
     * Invokes platform specific functionality to dispose a GC handle.
     * <p>
     * <b>IMPORTANT:</b> This method is <em>not</em> part of the public
     * API for <code>Display</code>. It is marked public only so that it
     * can be shared within the packages provided by SWT. It is not
     * available on all platforms, and should never be called from
     * application code.
     * </p>
     *
     * @param hDC the platform specific GC handle
     * @param data the platform specific GC data
     */
    public void internal_dispose_GC(int hDC, GCData data)
    {
        OS.GC_Dispose(hDC);
    }

    boolean isValidThread()
    {
        return thread == Thread.currentThread();
    }

    public boolean internal_isValidThread()
    {
        return this.isValidThread();
    }

    /**
     * Maps a point from one coordinate system to another.
     * When the control is null, coordinates are mapped to
     * the display.
     * <p>
     * NOTE: On right-to-left platforms where the coordinate
     * systems are mirrored, special care needs to be taken
     * when mapping coordinates from one control to another
     * to ensure the result is correctly mirrored.
     *
     * Mapping a point that is the origin of a rectangle and
     * then adding the width and height is not equivalent to
     * mapping the rectangle.  When one control is mirrored
     * and the other is not, adding the width and height to a
     * point that was mapped causes the rectangle to extend
     * in the wrong direction.  Mapping the entire rectangle
     * instead of just one point causes both the origin and
     * the corner of the rectangle to be mapped.
     * </p>
     *
     * @param from the source <code>Control</code> or <code>null</code>
     * @param to the destination <code>Control</code> or <code>null</code>
     * @param point to be mapped
     * @return point with mapped coordinates
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the point is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if the Control from or the Control to have been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @since 2.1.2
     */
    public Point map(Control from, Control to, Point point)
    {
        checkDevice();
        if (point == null) error(SWT.ERROR_NULL_ARGUMENT);
        return map(from, to, point.x, point.y);
    }

    /**
     * Maps a point from one coordinate system to another.
     * When the control is null, coordinates are mapped to
     * the display.
     * <p>
     * NOTE: On right-to-left platforms where the coordinate
     * systems are mirrored, special care needs to be taken
     * when mapping coordinates from one control to another
     * to ensure the result is correctly mirrored.
     *
     * Mapping a point that is the origin of a rectangle and
     * then adding the width and height is not equivalent to
     * mapping the rectangle.  When one control is mirrored
     * and the other is not, adding the width and height to a
     * point that was mapped causes the rectangle to extend
     * in the wrong direction.  Mapping the entire rectangle
     * instead of just one point causes both the origin and
     * the corner of the rectangle to be mapped.
     * </p>
     *
     * @param from the source <code>Control</code> or <code>null</code>
     * @param to the destination <code>Control</code> or <code>null</code>
     * @param x coordinates to be mapped
     * @param y coordinates to be mapped
     * @return point with mapped coordinates
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the Control from or the Control to have been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @since 2.1.2
     */
    public Point map(Control from, Control to, int x, int y)
    {
        checkDevice();
        if (from != null && from.isDisposed()) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        if (to != null && to.isDisposed()) SWT.error(SWT.ERROR_INVALID_ARGUMENT);

        Point point = new Point(x, y);
        if (to != null && from != null)
        {
            point = from.toDisplay(point);
            point = to.toControl(point);
        }
        else if (to != null)
        {
            point = to.toControl(point);
        }
        else if (from != null)
        {
            point = from.toDisplay(point);
        }
        return point;
    }

    /**
     * Maps a point from one coordinate system to another.
     * When the control is null, coordinates are mapped to
     * the display.
     * <p>
     * NOTE: On right-to-left platforms where the coordinate
     * systems are mirrored, special care needs to be taken
     * when mapping coordinates from one control to another
     * to ensure the result is correctly mirrored.
     *
     * Mapping a point that is the origin of a rectangle and
     * then adding the width and height is not equivalent to
     * mapping the rectangle.  When one control is mirrored
     * and the other is not, adding the width and height to a
     * point that was mapped causes the rectangle to extend
     * in the wrong direction.  Mapping the entire rectangle
     * instead of just one point causes both the origin and
     * the corner of the rectangle to be mapped.
     * </p>
     *
     * @param from the source <code>Control</code> or <code>null</code>
     * @param to the destination <code>Control</code> or <code>null</code>
     * @param rectangle to be mapped
     * @return rectangle with mapped coordinates
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the rectangle is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if the Control from or the Control to have been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @since 2.1.2
     */
    public Rectangle map(Control from, Control to, Rectangle rectangle)
    {
        checkDevice();
        if (rectangle == null) error(SWT.ERROR_NULL_ARGUMENT);
        return map(from, to, rectangle.x, rectangle.y, rectangle.width, rectangle.height);
    }

    /**
     * Generate a low level system event.
     *
     * <code>post</code> is used to generate low level keyboard
     * and mouse events. The intent is to enable automated UI
     * testing by simulating the input from the user.  Most
     * SWT applications should never need to call this method.
     * <p>
     * Note that this operation can fail when the operating system
     * fails to generate the event for any reason.  For example,
     * this can happen when there is no such key or mouse button
     * or when the system event queue is full.
     * </p>
     * <p>
     * <b>Event Types:</b>
     * <p>KeyDown, KeyUp
     * <p>The following fields in the <code>Event</code> apply:
     * <ul>
     * <li>(in) type KeyDown or KeyUp</li>
     * <p> Either one of:
     * <li>(in) character a character that corresponds to a keyboard key</li>
     * <li>(in) keyCode the key code of the key that was typed,
     *          as defined by the key code constants in class <code>SWT</code></li>
     * </ul>
     * <p>MouseDown, MouseUp</p>
     * <p>The following fields in the <code>Event</code> apply:
     * <ul>
     * <li>(in) type MouseDown or MouseUp
     * <li>(in) button the button that is pressed or released
     * </ul>
     * <p>MouseMove</p>
     * <p>The following fields in the <code>Event</code> apply:
     * <ul>
     * <li>(in) type MouseMove
     * <li>(in) x the x coordinate to move the mouse pointer to in screen coordinates
     * <li>(in) y the y coordinate to move the mouse pointer to in screen coordinates
     * </ul>
     * </dl>
     *
     * @param event the event to be generated
     *
     * @return true if the event was generated or false otherwise
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the event is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @since 3.0
     *
     */
    public boolean post(Event event)
    {
        if (isDisposed()) error(SWT.ERROR_DEVICE_DISPOSED);
        if (event == null) error(SWT.ERROR_NULL_ARGUMENT);

        switch (event.type)
        {
        case SWT.KeyDown:
        case SWT.KeyUp:
            if (event.keyCode<0)
            {
                return false;
            }
            return OS.Display_Post(event.type, event.character, event.keyCode, event.stateMask);

        case SWT.MouseDown:
        case SWT.MouseUp:
            if (event.button <= 0 || event.x<0 || event.y<0)
            {
                return false;
            }
            return OS.Display_MousePost(event.type, event.x, event.y, event.stateMask);

        case SWT.MouseMove:
            if (event.button <= 0 || event.x < 0 || event.y < 0)
            {
                return false;
            }
            return OS.Display_MousePost(event.type, event.x, event.y, event.stateMask);

        default:
        }

        return false;
    }

    /**
     * Maps a point from one coordinate system to another.
     * When the control is null, coordinates are mapped to
     * the display.
     * <p>
     * NOTE: On right-to-left platforms where the coordinate
     * systems are mirrored, special care needs to be taken
     * when mapping coordinates from one control to another
     * to ensure the result is correctly mirrored.
     *
     * Mapping a point that is the origin of a rectangle and
     * then adding the width and height is not equivalent to
     * mapping the rectangle.  When one control is mirrored
     * and the other is not, adding the width and height to a
     * point that was mapped causes the rectangle to extend
     * in the wrong direction.  Mapping the entire rectangle
     * instead of just one point causes both the origin and
     * the corner of the rectangle to be mapped.
     * </p>
     *
     * @param from the source <code>Control</code> or <code>null</code>
     * @param to the destination <code>Control</code> or <code>null</code>
     * @param x coordinates to be mapped
     * @param y coordinates to be mapped
     * @param width coordinates to be mapped
     * @param height coordinates to be mapped
     * @return rectangle with mapped coordinates
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the Control from or the Control to have been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @since 2.1.2
     */
    public Rectangle map(Control from, Control to, int x, int y, int width, int height)
    {
        checkDevice();
        if (from != null && from.isDisposed()) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        if (to != null && to.isDisposed()) SWT.error(SWT.ERROR_INVALID_ARGUMENT);

        Rectangle rectangle = new Rectangle(x, y, width, height);
        if (to != null && from != null)
        {
            Point topLeft = from.toDisplay(x, y);
            topLeft = to.toControl(topLeft);
            rectangle.x = topLeft.x;
            rectangle.y = topLeft.y;
        }
        else if (to != null)
        {
            Point topLeft = to.toControl(x, y);
            rectangle.x = topLeft.x;
            rectangle.y = topLeft.y;
        }
        else if (from != null)
        {
            Point topLeft = from.toDisplay(x, y);
            rectangle.x = topLeft.x;
            rectangle.y = topLeft.y;
        }
        return rectangle;

    }


    /**
     * Reads an event from the operating system's event queue,
     * dispatches it appropriately, and returns <code>true</code>
     * if there is potentially more work to do, or <code>false</code>
     * if the caller can sleep until another event is placed on
     * the event queue.
     * <p>
     * In addition to checking the system event queue, this method also
     * checks if any inter-thread messages (created by <code>syncExec()</code>
     * or <code>asyncExec()</code>) are waiting to be processed, and if
     * so handles them before returning.
     * </p>
     *
     * @return <code>false</code> if the caller can sleep upon return from this method
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_FAILED_EXEC - if an exception occurred while running an inter-thread message</li>
     * </ul>
     *
     * @see #sleep
     * @see #wake
     */
    public boolean readAndDispatch()
    {
        checkDevice();

        if (OS.Display_ReadAndDispatch(displayHandle))
        {
            runDeferredEvents();
            return true;
        }
        else
        {
            return runAsyncMessages();
        }
    }

    static synchronized void register(Display display)
    {
        for (int i=0; i<Displays.length; i++)
        {
            if (Displays [i] == null)
            {
                Displays [i] = display;
                return;
            }
        }
        Display [] newDisplays = new Display [Displays.length + 4];
        System.arraycopy(Displays, 0, newDisplays, 0, Displays.length);
        newDisplays [Displays.length] = display;
        Displays = newDisplays;
    }

    /**
     * Releases any internal resources back to the operating
     * system and clears all fields except the device handle.
     * <p>
     * Disposes all shells which are currently open on the display.
     * After this method has been invoked, all related related shells
     * will answer <code>true</code> when sent the message
     * <code>isDisposed()</code>.
     * </p><p>
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
     *
     * @see #dispose
     * @see #destroy
     */
    protected void release()
    {
        sendEvent(SWT.Dispose, new Event());

        try
        {
            while (readAndDispatch()) {};
            runDeferredEvents();
            runAsyncMessages();
        }

        finally
        {
            Shell [] shells = getShells();
            for (int i = 0; i < shells.length;  i++)
            {
                if (shells[i] != null && !shells[i].isDisposed())
                {
                    if (this == shells[i].getDisplay()) shells[i].dispose();
                }
            }

            if (disposeList != null)
            {
                for (int i = 0; i < disposeList.length; i++)
                {
                    if (disposeList[i] != null) disposeList[i].run();
                }
            }
            disposeList = null;

            releaseSystemColors();

            synchronizer.releaseSynchronizer();
            synchronizer = null;

            super.release();
        }
    }

    void releaseSystemColors()
    {
        if (COLOR_WIDGET_DARK_SHADOW != null) COLOR_WIDGET_DARK_SHADOW.dispose();
        if (COLOR_WIDGET_NORMAL_SHADOW != null) COLOR_WIDGET_NORMAL_SHADOW.dispose();
        if (COLOR_WIDGET_LIGHT_SHADOW != null) COLOR_WIDGET_LIGHT_SHADOW.dispose();
        if (COLOR_WIDGET_HIGHLIGHT_SHADOW != null) COLOR_WIDGET_HIGHLIGHT_SHADOW.dispose();
        if (COLOR_WIDGET_FOREGROUND != null) COLOR_WIDGET_FOREGROUND.dispose();
        if (COLOR_WIDGET_BACKGROUND != null) COLOR_WIDGET_BACKGROUND.dispose();
        if (COLOR_WIDGET_BORDER != null) COLOR_WIDGET_BORDER.dispose();
        if (COLOR_LIST_FOREGROUND != null) COLOR_LIST_FOREGROUND.dispose();
        if (COLOR_LIST_BACKGROUND != null) COLOR_LIST_BACKGROUND.dispose();
        if (COLOR_LIST_SELECTION != null) COLOR_LIST_SELECTION.dispose();
        if (COLOR_LIST_SELECTION_TEXT != null) COLOR_LIST_SELECTION_TEXT.dispose();
        if (COLOR_TITLE_FOREGROUND != null) COLOR_TITLE_FOREGROUND.dispose();
        if (COLOR_TITLE_BACKGROUND != null) COLOR_TITLE_BACKGROUND.dispose();
        if (COLOR_TITLE_BACKGROUND_GRADIENT != null) COLOR_TITLE_BACKGROUND_GRADIENT.dispose();
        if (COLOR_TITLE_INACTIVE_FOREGROUND != null) COLOR_TITLE_INACTIVE_FOREGROUND.dispose();
        if (COLOR_TITLE_INACTIVE_BACKGROUND != null) COLOR_TITLE_INACTIVE_BACKGROUND.dispose();
        if (COLOR_TITLE_INACTIVE_BACKGROUND_GRADIENT != null) COLOR_TITLE_INACTIVE_BACKGROUND_GRADIENT.dispose();
    }

    /**
     * Removes the listener from the collection of listeners who will
     * be notifed when an event of the given type occurs anywhere in
     * this display.
     *
     * @param eventType the type of event to listen for
     * @param listener the listener which should no longer be notified when the event occurs
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Listener
     * @see #addFilter
     * @see #addListener
     *
     * @since 3.0
     */
    public void removeFilter(int eventType, Listener listener)
    {
        checkDevice();
        if (listener == null) error(SWT.ERROR_NULL_ARGUMENT);
        if (filterTable == null) return;
        filterTable.unhook(eventType, listener);
        if (filterTable.size() == 0) filterTable = null;
    }

    /**
     * Removes the listener from the collection of listeners who will
     * be notifed when an event of the given type occurs.
     *
     * @param eventType the type of event to listen for
     * @param listener the listener which should no longer be notified when the event occurs
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see Listener
     * @see #addListener
     *
     * @since 2.0
     */
    public void removeListener(int eventType, Listener listener)
    {
        checkDevice();
        if (listener == null) error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook(eventType, listener);
    }

    boolean runAsyncMessages()
    {
        return synchronizer.runAsyncMessages();
    }

    /*
    * Run deferred events.  This code is always
    * called in the Display's thread so it must
    * be re-enterant but need not be synchronized.
    */
    boolean runDeferredEvents()
    {
        while (eventQueue != null)
        {

            /* Take an event off the queue */
            Event event = eventQueue [0];
            if (event == null) break;
            int length = eventQueue.length;
            System.arraycopy(eventQueue, 1, eventQueue, 0, --length);
            eventQueue [length] = null;

            /* Run the event */
            Widget widget = event.widget;
            if (widget != null && !widget.isDisposed())
            {
                Widget item = event.item;
                if (item == null || !item.isDisposed())
                {
                    widget.sendEvent(event);
                }
            }

            /*
            * At this point, the event queue could
            * be null due to a recursive invokation
            * when running the event.
            */
        }

        /* Clear the queue */
        eventQueue = null;
        return true;
    }

    void sendEvent(int eventType, Event event)
    {
        if (eventTable == null && filterTable == null)
        {
            return;
        }
        if (event == null) event = new Event();
        event.display = this;
        event.type = eventType;
        if (event.time == 0) event.time = getLastEventTime();
        if (!filterEvent(event))
        {
            if (eventTable != null) eventTable.sendEvent(event);
        }
    }

    /**
     * Sets the application defined property of the receiver
     * with the specified name to the given argument.
     * <p>
     * Applications may have associated arbitrary objects with the
     * receiver in this fashion. If the objects stored in the
     * properties need to be notified when the display is disposed
     * of, it is the application's responsibility provide a
     * <code>disposeExec()</code> handler which does so.
     * </p>
     *
     * @param key the name of the property
     * @param value the new value for the property
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the key is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see #getData(String)
     * @see #disposeExec(Runnable)
     */
    public void setData(String key, Object value)
    {
        checkDevice();
        if (key == null) error(SWT.ERROR_NULL_ARGUMENT);

        /* Remove the key/value pair */
        if (value == null)
        {
            if (keys == null) return;
            int index = 0;
            while (index < keys.length && !keys [index].equals(key)) index++;
            if (index == keys.length) return;
            if (keys.length == 1)
            {
                keys = null;
                values = null;
            }
            else
            {
                String [] newKeys = new String [keys.length - 1];
                Object [] newValues = new Object [values.length - 1];
                System.arraycopy(keys, 0, newKeys, 0, index);
                System.arraycopy(keys, index + 1, newKeys, index, newKeys.length - index);
                System.arraycopy(values, 0, newValues, 0, index);
                System.arraycopy(values, index + 1, newValues, index, newValues.length - index);
                keys = newKeys;
                values = newValues;
            }
            return;
        }

        /* Add the key/value pair */
        if (keys == null)
        {
            keys = new String [] {key};
            values = new Object [] {value};
            return;
        }
        for (int i=0; i<keys.length; i++)
        {
            if (keys [i].equals(key))
            {
                values [i] = value;
                return;
            }
        }
        String [] newKeys = new String [keys.length + 1];
        Object [] newValues = new Object [values.length + 1];
        System.arraycopy(keys, 0, newKeys, 0, keys.length);
        System.arraycopy(values, 0, newValues, 0, values.length);
        newKeys [keys.length] = key;
        newValues [values.length] = value;
        keys = newKeys;
        values = newValues;
    }

    /**
     * Sets the application defined, display specific data
     * associated with the receiver, to the argument.
     * The <em>display specific data</em> is a single,
     * unnamed field that is stored with every display.
     * <p>
     * Applications may put arbitrary objects in this field. If
     * the object stored in the display specific data needs to
     * be notified when the display is disposed of, it is the
     * application's responsibility provide a
     * <code>disposeExec()</code> handler which does so.
     * </p>
     *
     * @param data the new display specific data
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see #getData()
     * @see #disposeExec(Runnable)
     */
    public void setData(Object data)
    {
        checkDevice();
        this.data = data;
    }

    /**
     * On platforms which support it, sets the application name
     * to be the argument. On Motif, for example, this can be used
     * to set the name used for resource lookup.
     *
     * @param name the new app name
     */
    public static void setAppName(String name)
    {
        APP_NAME = name == null ? "" : name;
        OS.Display_SetAppName(APP_NAME);
    }

    /**
     * Sets the synchronizer used by the display to be
     * the argument, which can not be null.
     *
     * @param synchronizer the new synchronizer for the display (must not be null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the synchronizer is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_FAILED_EXEC - if an exception occurred while running an inter-thread message</li>
     * </ul>
     */
    public void setSynchronizer(Synchronizer synchronizer)
    {
        checkDevice();
        if (synchronizer == null) error(SWT.ERROR_NULL_ARGUMENT);
        if (this.synchronizer != null)
        {
            this.synchronizer.runAsyncMessages();
        }
        this.synchronizer = synchronizer;
    }

    /**
     * Causes the user-interface thread to <em>sleep</em> (that is,
     * to be put in a state where it does not consume CPU cycles)
     * until an event is received or it is otherwise awakened.
     *
     * @return <code>true</code> if an event requiring dispatching was placed on the queue.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see #wake
     */
    public boolean sleep()
    {
        checkDevice();
        boolean result = OS.Display_Sleep(displayHandle);
        runDeferredEvents();
        return result;
    }

    /**
     * Causes the <code>run()</code> method of the runnable to
     * be invoked by the user-interface thread at the next
     * reasonable opportunity. The thread which calls this method
     * is suspended until the runnable completes.  Specifying <code>null</code>
     * as the runnable simply wakes the user-interface thread.
     * <p>
     * Note that at the time the runnable is invoked, widgets
     * that have the receiver as their display may have been
     * disposed. Therefore, it is necessary to check for this
     * case inside the runnable before accessing the widget.
     * </p>
     *
     * @param runnable code to run on the user-interface thread or <code>null</code>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_FAILED_EXEC - if an exception occured when executing the runnable</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see #asyncExec
     */
    public void syncExec(Runnable runnable)
    {
        if (isDisposed()) error(SWT.ERROR_DEVICE_DISPOSED);
        synchronizer.syncExec(runnable);
    }

    /**
     * Causes the <code>run()</code> method of the runnable to
     * be invoked by the user-interface thread after the specified
     * number of milliseconds have elapsed. If milliseconds is less
     * than zero, the runnable is not executed.
     * <p>
     * Note that at the time the runnable is invoked, widgets
     * that have the receiver as their display may have been
     * disposed. Therefore, it is necessary to check for this
     * case inside the runnable before accessing the widget.
     * </p>
     *
     * @param milliseconds the delay before running the runnable
     * @param runnable code to run on the user-interface thread
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the runnable is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see #asyncExec
     */
    public void timerExec(int milliseconds, Runnable runnable)
    {
        checkDevice();

        if (runnable == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (milliseconds < 0) return;

        if (timers == null) timers = new Runnable[4];
        int index = 0;
        while (index < timers.length)
        {
            if (timers[index] == null) break;
            index++;
        }
        if (index == timers.length)
        {
            Runnable[] newTimers = new Runnable[timers.length + 4];
            System.arraycopy(timers, 0, newTimers, 0, timers.length);
            timers = newTimers;
        }

        timers[index] = runnable;
        OS.Display_StartTimer(displayHandle,milliseconds,index);
    }

    void timerCallback(int id)
    {
        if (id < 0 || id >= timers.length) return;
        Runnable r = timers[id];
        if (r != null) syncExec(r);
        timers[id] = null;
    }


    /**
     * Forces all outstanding paint requests for the display
     * to be processed before this method returns.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see Control#update()
     */
    public void update()
    {
        checkDevice();
        Shell[] shells = getShells();
        for (int i=0; i<shells.length; i++)
        {
            Shell shell = shells [i];
            if (!shell.isDisposed()) shell.update();
        }
    }

    /**
     * If the receiver's user-interface thread was <code>sleep</code>'ing,
     * causes it to be awakened and start running again. Note that this
     * method may be called from any thread.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
     * </ul>
     *
     * @see #sleep
     */
    public void wake()
    {
        if (isDisposed()) error(SWT.ERROR_DEVICE_DISPOSED);
        if (thread == Thread.currentThread()) return;
        wakeThread();
    }

    void wakeThread()
    {
        OS.Display_Wake(displayHandle);
    }

    void addShell(Shell shell)
    {
        shells.addElement(shell);
    }

    void removeShell(Shell shell)
    {
        shells.removeElement(shell);
    }

    public void internal_paintListenerHooked()
    {
        if (paintListenersCount == 0)
        {
            OS.Display_SetOptimizedPaint(displayHandle, false);
        }
        paintListenersCount++;
    }

    public void internal_paintListenerUnhooked()
    {
        paintListenersCount--;
        if (paintListenersCount <= 0)
        {
            OS.Display_SetOptimizedPaint(displayHandle, true);
            paintListenersCount = 0; // In case this is called too many times
        }
    }

    public GCBuffer internal_sharedBuffer()
    {
        if (sharedBuffer == null)
        {
            sharedBuffer = new GCBuffer();
        }
        return sharedBuffer;
    }

    public void internal_free_sharedBuffer()
    {
        // Discard the shared paint listener gc buffer to avoid keeping a buffer
        // larger than needed. If there are remaining paint listeners then the buffer
        // will get recreated and resized.
        if (sharedBuffer != null && sharedBuffer.isFree())
        {
            sharedBuffer = null;
        }
    }

    protected void runSettings(boolean themeChanged)
    {
        //System Font is updated on the native code
        if (themeChanged)
        {
            releaseSystemColors();
            init();
        }
        sendEvent(SWT.Settings, null);
        Shell [] shells = getShells();
        for (int i=0; i<shells.length; i++)
        {
            Shell shell = shells [i];
            if (!shell.isDisposed())
            {
                /* This code is intentionally commented */
                //shell.redraw (true);
                shell.layout(true, true);
            }
        }
    }

    boolean sendCloseEvent()
    {
        Event event = new Event();
        sendEvent(SWT.Close, event);
        return event.doit;
    }
    
    boolean firstPaintComplete() {
        return firstPaintComplete;
    }
    
    void notifyFirstPaintComplete(boolean topShellPainted) {
        OS.Display_NotifyFirstPaintComplete(topShellPainted);
        firstPaintComplete = true;
    }
}
