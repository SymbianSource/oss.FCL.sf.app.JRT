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
 *      Nokia Corporation - Qt implementation
 *******************************************************************************/
package org.eclipse.swt.widgets;


import org.eclipse.ercp.swt.mobile.Internal_MobilePackageSupport;
import org.eclipse.ercp.swt.mobile.MobileDevice;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Device;
import org.eclipse.swt.graphics.DeviceData;
import org.eclipse.swt.graphics.Internal_GfxPackageSupport;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.ApplicationUidWrapper;
import org.eclipse.swt.internal.ExitNotificationWrapper;
import org.eclipse.swt.internal.qt.CommandArranger;
import org.eclipse.swt.internal.qt.GCData;
import org.eclipse.swt.internal.qt.JniUtils;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.QObjectDeleteWrapper;
import org.eclipse.swt.internal.qt.SharedGCWrapper;
import org.eclipse.swt.internal.qt.WidgetState;
import org.eclipse.swt.internal.qt.graphics.GraphicsContext;

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
 * @see Device#dispose
 */
public class Display extends Device {

/* QApplication */
static int handle;

/* QApplication event filter */
static int eventFilterHandle;

static final String SWT_OBJECT_INDEX = "swt_obj_idx";
static final String ADD_WIDGET_KEY = "org.eclipse.swt.internal.addWidget";
static final String REMOVE_WIDGET_KEY = "org.eclipse.swt.internal.removeWidget";
static final String IMAGE_LOAD_SIZE_KEY = "org.eclipse.swt.internal.image.loadSize";
static final String SWT_EVENT_FILTER = "swt_event_filter";

static CommandArranger commandArranger;

static Event[] eventQueue;
static EventTable eventTable, filterTable;

static String APP_NAME;

/* Widget Table */
static int[] indexTable;
static int freeSlot;
static int lastHandle;
static Widget lastWidget;
static Widget[] widgetTable;
final static int GROW_SIZE = 64;

/*
 * Set to have relevant QWidgets have their QObject names set to the SWT
 * widget class names for easier debugging. 
 */
static boolean objectNames /*= true*/;

static MobileDevice mobileDevice;
static int mobileDeviceHandle;

/* Modality */
static Shell [] modalShells;
static Dialog modalDialog;

/* Sync/Async Widget Communication */
static Synchronizer synchronizer;
Thread thread;

static Thread fixedUiThread;

/* Display Shutdown */
Runnable [] disposeList;

/* System Tray */
static Tray tray;

/* Timers */
static int [] timerHandles;
static Runnable [] timerRunnables;

/* Timestamp of the Last Received Event */
static int lastEventTime;

/* If not to interrupt event processing after every event */
static boolean noInterrupt;

/* Used to block one type of events temporarily */
static int blockedQtEventType;

/* Default orientation for Controls when style is not specified */
static int defaultOrientation;
static boolean defaultOrientationIsSysLangDirection;

// The next Image loaded by Image class constructors will be scaled to
// this size. Can be set using Display.setData([key], Point). Useful
// for SVG images. 
static private Point imageLoadSize;

/* Key Mappings */
static final int [] [] KeyTable = {
    /* Keyboard and Mouse Masks */
    {OS.QT_KEY_ALT,     SWT.ALT},
    {OS.QT_KEY_ALTGR,   SWT.ALT},
    {OS.QT_KEY_META,    SWT.ALT},
    {OS.QT_KEY_SHIFT,   SWT.SHIFT},
    {OS.QT_KEY_CONTROL, SWT.CONTROL},
    /* Non-Numeric Keypad Keys */
    {OS.QT_KEY_UP,      SWT.ARROW_UP},
    {OS.QT_KEY_DOWN,    SWT.ARROW_DOWN},
    {OS.QT_KEY_LEFT,    SWT.ARROW_LEFT},
    {OS.QT_KEY_RIGHT,   SWT.ARROW_RIGHT},
    {OS.QT_KEY_PAGEUP,  SWT.PAGE_UP},
    {OS.QT_KEY_PAGEDOWN,SWT.PAGE_DOWN},
    {OS.QT_KEY_HOME,    SWT.HOME},
    {OS.QT_KEY_END,     SWT.END},
    {OS.QT_KEY_INSERT,  SWT.INSERT},
    /* Virtual and Ascii Keys */
    {OS.QT_KEY_BACKSPACE,SWT.BS},
    {OS.QT_KEY_RETURN,   SWT.CR},
    {OS.QT_KEY_DELETE,   SWT.DEL},
    {OS.QT_KEY_ESCAPE,   SWT.ESC},
    {OS.QT_KEY_ENTER,    SWT.CR},
    {OS.QT_KEY_TAB,      SWT.TAB},
    {OS.QT_KEY_BACKTAB,  SWT.TAB},
    /* Functions Keys */
    {OS.QT_KEY_F1,      SWT.F1},
    {OS.QT_KEY_F2,      SWT.F2},
    {OS.QT_KEY_F3,      SWT.F3},
    {OS.QT_KEY_F4,      SWT.F4},
    {OS.QT_KEY_F5,      SWT.F5},
    {OS.QT_KEY_F6,      SWT.F6},
    {OS.QT_KEY_F7,      SWT.F7},
    {OS.QT_KEY_F8,      SWT.F8},
    {OS.QT_KEY_F9,      SWT.F9},
    {OS.QT_KEY_F10,     SWT.F10},
    {OS.QT_KEY_F11,     SWT.F11},
    {OS.QT_KEY_F12,     SWT.F12},
    {OS.QT_KEY_F13,     SWT.F13},
    {OS.QT_KEY_F14,     SWT.F14},
    {OS.QT_KEY_F15,     SWT.F15},
    /* Other keys */
    {OS.QT_KEY_CAPSLOCK,    SWT.CAPS_LOCK},
    {OS.QT_KEY_NUMLOCK,     SWT.NUM_LOCK},
    {OS.QT_KEY_SCROLLLOCK,  SWT.SCROLL_LOCK},
    {OS.QT_KEY_PAUSE,       SWT.PAUSE},
    {OS.QT_KEY_PRINT,       SWT.PRINT_SCREEN},
    {OS.QT_KEY_HELP,        SWT.HELP},

};

/*
 * Feature in QT: The key events that are result of keypad
 * are indicated with the QT_KEYPADMODIFIER. Therefore
 * we have a second key table to map only the KeyPad keys.
 */
static final int [] [] KeyPadKeyTable = {
    /* Numeric Keypad Keys */
    {OS.QT_KEY_ASTERISK,        SWT.KEYPAD_MULTIPLY},
    {OS.QT_KEY_PLUS,            SWT.KEYPAD_ADD},
    {OS.QT_KEY_ENTER,           SWT.KEYPAD_CR},
    {OS.QT_KEY_MINUS,   SWT.KEYPAD_SUBTRACT},
    {OS.QT_KEY_PERIOD,  SWT.KEYPAD_DECIMAL},
    {OS.QT_KEY_SLASH,       SWT.KEYPAD_DIVIDE},
    {OS.QT_KEY_0,           SWT.KEYPAD_0},
    {OS.QT_KEY_1,           SWT.KEYPAD_1},
    {OS.QT_KEY_2,           SWT.KEYPAD_2},
    {OS.QT_KEY_3,           SWT.KEYPAD_3},
    {OS.QT_KEY_4,           SWT.KEYPAD_4},
    {OS.QT_KEY_5,           SWT.KEYPAD_5},
    {OS.QT_KEY_6,           SWT.KEYPAD_6},
    {OS.QT_KEY_7,           SWT.KEYPAD_7},
    {OS.QT_KEY_8,           SWT.KEYPAD_8},
    {OS.QT_KEY_9,           SWT.KEYPAD_9},
    {OS.QT_KEY_EQUAL,   SWT.KEYPAD_EQUAL},
};

/*
 * Virtual key codes for control keys on X11.
 */
static final int X11_SHIFT_LEFT    = 0xffe1;
static final int X11_SHIFT_RIGHT   = 0xffe2;
static final int X11_CONTROL_LEFT  = 0xffe3;
static final int X11_CONTROL_RIGHT = 0xffe4;
static final int X11_META_LEFT     = 0xffe7;
static final int X11_META_RIGHT    = 0xffe8;
static final int X11_ALT_LEFT      = 0xffe9;
static final int X11_ALT_RIGHT     = 0xffea;
static final int X11_SUPER_LEFT    = 0xffeb;
static final int X11_SUPER_RIGHT   = 0xffec;
static final int X11_HYPER_LEFT    = 0xffed;
static final int X11_HYPER_RIGHT   = 0xffee;

/*
 * Scan codes for control keys on Symbian.
 */
static final int SYMBIAN_SHIFT_LEFT  = 0x12;    // EStdKeyLeftShift
static final int SYMBIAN_SHIFT_RIGHT = 0x13;    // EStdKeyRightShift
static final int SYMBIAN_ALT_LEFT    = 0x14;    // EStdKeyLeftAlt
static final int SYMBIAN_ALT_RIGHT   = 0x15;    // EStdKeyRightAlt
static final int SYMBIAN_CTRL_LEFT   = 0x16;    // EStdKeyLeftCtrl
static final int SYMBIAN_CTRL_RIGHT  = 0x17;    // EStdKeyRightCtrl
static final int SYMBIAN_FN_LEFT     = 0x18;    // EStdKeyLeftFunc
static final int SYMBIAN_FN_RIGHT    = 0x19;    // EStdKeyRightFunc

/* Multiple Displays. */
static Display Default;
static Display Internal;
static Display [] Displays = new Display [4];

/* Package name */
static final String PACKAGE_PREFIX = "org.eclipse.swt.widgets.";

/* eRCP package name */
static final String ERCP_MOBILE_PACKAGE_PREFIX = "org.eclipse.ercp.swt.mobile.";

/* extension package name */
static final String INTERNAL_EXTENSION_PACKAGE_PREFIX = "org.eclipse.swt.internal.extension.";

/* Display Data */
static Object data;
static String [] keys;
static Object [] values;

/* Last ShortcutOverride event parameters */
static int shortcutOverrideKey;
static int shortcutOverrideModifier;
static int shortcutOverrideCharacter;

/* Last control that had a mnemonic hit i.e. shortcut event*/
static Control mnemonicControl;

/* It is stored here if we are currently in a focus event */
static int focusEvent;

/* Settings event specific variables */
static final int SETTINGS_DELAY = 2000;
static Runnable settingsRunnable;

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
 *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed </li>
 * </ul>
 *
 * @see #getCurrent
 * @see #getDefault
 * @see Widget#checkSubclass
 * @see Shell
 */
public Display () {
    this (null);
}

/**
 * Constructs a new instance of this class using the parameter.
 *
 * @param data the device data
 */
public Display (DeviceData data) {
    super(data);
}

private Display(DeviceData data, boolean internal) {
	super(data, internal);
}

static Display internalInstance() {
	if(Internal == null) {
		Internal = new Display(null, true);
	}
	return Internal;
}

/**
 * Adds the listener to the collection of listeners who will
 * be notified when an event of the given type occurs anywhere
 * in a widget. The event type is one of the event constants
 * defined in class <code>SWT</code>. When the event does occur,
 * the listener is notified by sending it the <code>handleEvent()</code>
 * message.
 * <p>
 * Setting the type of an event to <code>SWT.None</code> from
 * within the <code>handleEvent()</code> method can be used to
 * change the event type and stop subsequent Java listeners
 * from running. Because event filters run before other listeners,
 * event filters can both block other listeners and set arbitrary
 * fields within an event. For this reason, event filters are both
 * powerful and dangerous. They should generally be avoided for
 * performance, debugging and code maintenance reasons.
 * </p>
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
 * @see SWT
 * @see #removeFilter
 * @see #removeListener
 *
 * @since 3.0
 */
public void addFilter (int eventType, Listener listener) {
    checkDevice ();
    if (listener == null) error (SWT.ERROR_NULL_ARGUMENT);
    if (filterTable == null) filterTable = new EventTable ();
    filterTable.hook (eventType, listener);
}

/**
 * Adds the listener to the collection of listeners who will
 * be notified when an event of the given type occurs. The event
 * type is one of the event constants defined in class <code>SWT</code>.
 * When the event does occur in the display, the listener is notified by
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
 * @see SWT
 * @see #removeListener
 *
 * @since 2.0
 */
public void addListener (int eventType, Listener listener) {
    checkDevice ();
    if (listener == null) error (SWT.ERROR_NULL_ARGUMENT);
    if (eventTable == null) eventTable = new EventTable ();
    eventTable.hook (eventType, listener);
}

static void addWidget (int handle, Widget widget) {
    if (handle == 0) return;
    if (freeSlot == -1) {
        int length = (freeSlot = indexTable.length) + GROW_SIZE;
        int[] newIndexTable = new int[length];
        Widget[] newWidgetTable = new Widget [length];
        System.arraycopy (indexTable, 0, newIndexTable, 0, freeSlot);
        System.arraycopy (widgetTable, 0, newWidgetTable, 0, freeSlot);
        for (int i = freeSlot; i < length - 1; i++) {
            newIndexTable[i] = i + 1;
        }
        newIndexTable[length - 1] = -1;
        indexTable = newIndexTable;
        widgetTable = newWidgetTable;
    }
    int index = freeSlot + 1;
    OS.QObject_setProperty(handle, SWT_OBJECT_INDEX, index);
    int oldSlot = freeSlot;
    freeSlot = indexTable[oldSlot];
    indexTable [oldSlot] = -2;
    widgetTable [oldSlot] = widget;
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
 * @param runnable code to run on the user-interface thread or <code>null</code>
 *
 * @exception SWTException <ul>
 *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
 * </ul>
 *
 * @see #syncExec
 */
public void asyncExec (Runnable runnable) {
    synchronized (Device.class) {
        if (isDisposed ()) error (SWT.ERROR_DEVICE_DISPOSED);
        synchronizer.asyncExec (runnable);
    }
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
public void beep () {
    if (!isValidThread ()) error (SWT.ERROR_THREAD_INVALID_ACCESS);
    OS.QApplication_beep();
}

private static void checkDefaultOrientation(int sysLangDirection) {
    defaultOrientation = SWT.LEFT_TO_RIGHT;
    String s = System.getProperty("org.eclipse.ercp.swt.defaultOrientation");
    if (s != null) {
        if (s.equals("system")) {
            defaultOrientation = sysLangDirection;
            defaultOrientationIsSysLangDirection = true;
        } else if (s.equals("rtl")) {
            defaultOrientation = SWT.RIGHT_TO_LEFT;
        }
    }
}

protected void checkDevice () {
    if (thread == null) error (SWT.ERROR_WIDGET_DISPOSED);
    if (thread != Thread.currentThread ()) error (SWT.ERROR_THREAD_INVALID_ACCESS);
    if (isDisposed ()) error (SWT.ERROR_DEVICE_DISPOSED);
}

final void checkDisplay (Thread thread, boolean multiple) {
    synchronized (Device.class) {
        for (int i=0; i<Displays.length; i++) {
            if (Displays [i] != null) {
            	if(Internal_GfxPackageSupport.internal(this) == Internal_GfxPackageSupport.internal(Displays[i])) {
            		if (!multiple) SWT.error (SWT.ERROR_NOT_IMPLEMENTED, null, " [multiple displays]");
                    if (Displays [i].thread == thread) SWT.error (SWT.ERROR_THREAD_INVALID_ACCESS);
            	}
            }
        }
        // On some platforms it's not supported to dispose the Display and
        // create another one in a different thread. Once a Display is created
        // in a thread that thread becomes the UI thread of the process and no
        // other thread can be a UI thread. I.e. UI thread is fixed.
        if(OS.windowServer == OS.WS_SYMBIAN_S60 && fixedUiThread == null) fixedUiThread = thread;
        if (fixedUiThread != null && !fixedUiThread.equals(thread)) SWT.error (SWT.ERROR_THREAD_INVALID_ACCESS);
    }
}

final void checkSubclass () {
    if (!isValidClass (getClass ())) error (SWT.ERROR_INVALID_SUBCLASS);
}

final void clearModal (Shell shell) {
    if (modalShells == null) return;
    int index = 0, length = modalShells.length;
    while (index < length) {
        if (modalShells [index] == shell) break;
        if (modalShells [index] == null) return;
        index++;
    }
    if (index == length) return;
    System.arraycopy (modalShells, index + 1, modalShells, index, --length - index);
    modalShells [length] = null;
    if (index == 0 && modalShells [0] == null) modalShells = null;
    Shell [] shells = getShells ();
    for (int i=0; i<shells.length; i++) shells [i].updateModal ();
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
public void close () {
    checkDevice ();
    Event event = new Event ();
    sendEvent (SWT.Close, event);
    if (event.doit) dispose ();
}

static void createDisplay() {
    // It's possible that there's already a QApplication from an old Display
    // that has been disposed in an event listener but we were unable to delete
    // the QApplication there. If we are creating this Display from an event
    // listener too then this won't help.
    tryDestroy();
    if(handle != 0 || JniUtils.handle != 0) SWT.error(SWT.ERROR_UNSPECIFIED);

    // Perform the required initialisation before Qt APIs can be used.
    if(OS.initUiThread(ApplicationUidWrapper.uid()) != 0) SWT.error(SWT.ERROR_UNSPECIFIED);

    // Create the object that manages the JNI activity. It's only possible when
    // this object exists.
    JniUtils.handle = OS.JniUtils_new();

    handle = OS.QApplication_swt_new(JniUtils.handle);
    if(handle == 0) SWT.error(SWT.ERROR_UNSPECIFIED);
    OS.QApplication_setQuitOnLastWindowClosed(false);

    if (APP_NAME != null) {
        OS.QCoreApplication_setApplicationName(handle, APP_NAME);
        OS.setSymbianAppName(APP_NAME);
    }

    int sysLangDirection = translateDirection(OS.QApplication_layoutDirection());
    checkDefaultOrientation(sysLangDirection);

    // Create and install the event filter;
    eventFilterHandle = OS.EventHandler_new();
    OS.QObject_installEventFilter(handle, eventFilterHandle);
    OS.QObject_setProperty(handle, SWT_EVENT_FILTER, eventFilterHandle);

    // Hook QApplication signals
    hookEvents();
}

static void deregister (Display display) {
    synchronized (Device.class) {
        for (int i=0; i<Displays.length; i++) {
            if (display == Displays [i]) Displays [i] = null;
        }
    }
}

/**
 * Destroys the device in the operating system and releases
 * the device's handle.  If the device does not have a handle,
 * this method may do nothing depending on the device.
 * <p>
 * This method is called after <code>release</code>.
 * </p>
 * @see Device#dispose
 * @see #release
 */
protected void destroy () {
    boolean notifyExit;
	if (notifyExit = (this == Default)) {
    	Default = null;    	
    } else if(this == Internal) {
    	Internal = null;
    }
    deregister (this);
    if(Default == null && Internal == null) {
	    GraphicsContext sharedGc = SharedGCWrapper.getSharedGC();
	    if(sharedGc != null && !sharedGc.isDisposed()) sharedGc.dispose();
	    destroyDisplay ();
	    super.destroy();
    }
    if(notifyExit) {
        // Notify MIDP runtime to watch that the MIDlet exits. 
        // For other runtime environments this does nothing. 
        ExitNotificationWrapper.uiDisposed();
    }
}

void destroyDisplay () {
    OS.QObject_removeEventFilter(handle, eventFilterHandle);
    OS.EventHandler_destroy(eventFilterHandle);
    OS.QApplication_syncX();
    tryDestroy();
}

int displayCount() {
    int res = 0;
    for (int i = 0; i < Displays.length; i++) {
        if (Displays[i] != null && !Displays[i].isDisposed()) {
            res++;
        }
    }
    return res;
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
public void disposeExec (Runnable runnable) {
    checkDevice ();
    if (disposeList == null) disposeList = new Runnable [4];
    for (int i=0; i<disposeList.length; i++) {
        if (disposeList [i] == null) {
            disposeList [i] = runnable;
            return;
        }
    }
    Runnable [] newDisposeList = new Runnable [disposeList.length + 4];
    System.arraycopy (disposeList, 0, newDisposeList, 0, disposeList.length);
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
void error (int code) {
    SWT.error (code);
}

/*
 * This is the main event handler that is called by native code.
 */
static boolean eventProcess(int widgetHandle, int eventType, int time,
        int arg1, int arg2, int arg3, int arg4, int arg5, String arg6) {
    // Silently let blocked events through
    if(blockedQtEventType == eventType) return false;

    // This goes to time field of the events that possibly get sent
    lastEventTime = time;

    // Get target widget or null if not found or is the Display
    Widget widget = ((widgetHandle == handle) ? null : getWidget ( widgetHandle ));

    if(!noInterrupt) {
        interrupt();
    }
    
    if (eventType < OS.QSIGNAL_ID_RANGE_FIRST) {
            return handleQtEvent(widget, widgetHandle, eventType, arg1, arg2,
                    arg3, arg4, arg5);
    } else if (eventType > OS.QSWTEVENT_ID_RANGE_FIRST) {
        return handleNativeSwtEvent(widget, widgetHandle, eventType, time,
                arg1, arg2, arg3, arg4, arg5);
    } else {
        return handleQtSignal(widget, widgetHandle, eventType, arg1, arg2,
                arg3, arg4, arg5, arg6);
    }
}

static boolean filterEvent (Event event) {
    if (filterTable != null) filterTable.sendEvent (event);
    return false;
}

static boolean filters (int eventType) {
    if (filterTable == null) return false;
    return filterTable.hooks (eventType);
}

static Control findControl (int handle) {
    if (handle == 0) return null;
    do {
        Widget widget = getWidget (handle);
        if(widget != null && widget instanceof Control) return (Control)widget;
        handle = OS.QObject_parent(handle);
    } while (handle != 0 && handle != Display.handle);
    return null;
}

/**
 * Returns the display which the given thread is the
 * user-interface thread for, or null if the given thread
 * is not a user-interface thread for any display.  Specifying
 * <code>null</code> as the thread will return <code>null</code>
 * for the display.
 *
 * @param thread the user-interface thread
 * @return the display for the given thread
 */
public static Display findDisplay (Thread thread) {
    synchronized (Device.class) {
        for (int i=0; i<Displays.length; i++) {
            Display display = Displays [i];
            if (display != null && display.thread == thread && !Internal_GfxPackageSupport.internal(display)) {
                return display;
            }
        }
        return null;
    }
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
public Shell getActiveShell () {
    checkDevice ();
    Shell activeShell = null;
    int activeWindowHandle = OS.QApplication_activeWindow();
    if (activeWindowHandle != 0) {
        Widget widget = getWidget(activeWindowHandle);
        if(widget instanceof org.eclipse.swt.widgets.Shell) {
            if(!Internal_PackageSupport.isInternalWidget(widget)) {
            activeShell = (Shell)widget;
            }
        }
    }
    return activeShell;
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
public Rectangle getBounds () {
    // Ignoring base implementation
    checkDevice ();
    return OS.QDesktopWidget_screenGeometry(OS.QApplication_desktop(), -1);
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
    // Ignoring base implementation
    checkDevice ();
    return OS.QDesktopWidget_availableGeometry(OS.QApplication_desktop(), -1);
}

static Control[] getControls()
{
    // Create a table of all Controls, no duplicates
    Control controlTable[] = new Control[widgetTable.length];
    int controlTableIndex = 0;
    for(int i = 0; i < widgetTable.length; i++) {
        Widget widget = widgetTable[i];
        if(widget == null) continue;
        Control control = (widget instanceof Control)  ? (Control)widget : null;
        if(control != null) {
            boolean alreadyInControlTable = false;
            for(int j = 0; j < controlTableIndex; ++j) {
                if(control == controlTable[j]) {
                    alreadyInControlTable = true;
                    break;
                }
            }
            if(!alreadyInControlTable)
                controlTable[controlTableIndex++] = control;
        }
    }
    Control[] result = new Control[controlTableIndex];
    System.arraycopy(controlTable, 0, result, 0, controlTableIndex);
    return result;
}

/**
 * Returns the display which the currently running thread is
 * the user-interface thread for, or null if the currently
 * running thread is not a user-interface thread for any display.
 *
 * @return the current display
 */
public static Display getCurrent () {
    return findDisplay (Thread.currentThread ());
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
public Object getData (String key) {
    checkDevice ();
    if (key == null) error (SWT.ERROR_NULL_ARGUMENT);
    if (keys == null) return null;
    for (int i=0; i<keys.length; i++) {
        if (keys [i].equals (key)) return values [i];
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
public Object getData () {
    checkDevice ();
    return data;
}

/**
 * Returns the default display. One is created (making the
 * thread that invokes this method its user-interface thread)
 * if it did not already exist.
 *
 * @return the default display
 */
public static Display getDefault () {
    synchronized (Device.class) {
        if (Default == null) Default = new Display ();
        return Default;
    }
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
public int getDismissalAlignment () {
    checkDevice ();
    return SWT.RIGHT;
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
public int getDoubleClickTime () {
    checkDevice ();
    return OS.QApplication_doubleClickInterval();
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
public Control getFocusControl () {
    checkDevice ();
    return _getFocusControl ();
}

static Control _getFocusControl () {
    return findControl( OS.QApplication_focusWidget() );
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
public int getIconDepth () {
    checkDevice ();
    return super.getDepth();
}

static int getLastEventTime () {
    return lastEventTime;
}

static int getMessageCount () {
    return synchronizer.getMessageCount ();
}

static Dialog getModalDialog () {
    return modalDialog;
}

static Menu[] getMenus()
{
    // Create a table of all Menus, no duplicates
    Menu menuTable[] = new Menu[widgetTable.length];
    int menuTableIndex = 0;
    for(int i = 0; i < widgetTable.length; i++) {
        Widget widget = widgetTable[i];
        if(widget == null) continue;
        Menu menu = (widget instanceof Menu)  ? (Menu)widget : null;
        if(menu != null) {
            boolean alreadyInMenuTable = false;
            for(int j = 0; j < menuTableIndex; ++j) {
                if(menu == menuTable[j]) {
                    alreadyInMenuTable = true;
                    break;
                }
            }
            if(!alreadyInMenuTable)
                menuTable[menuTableIndex++] = menu;
        }
    }
    Menu[] result = new Menu[menuTableIndex];
    System.arraycopy(menuTable, 0, result, 0, menuTableIndex);
    return result;
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
public Shell [] getShells () {
    checkDevice ();
    return getShells_();
}

static Shell[] getShells_() {
    int index = 0;
    Shell [] result = new Shell [16];
    for (int i = 0; i < widgetTable.length; i++) {
        Widget widget = widgetTable [i];
        if (widget != null && widget instanceof Shell) {
            int j = 0;
            while (j < index) {
                if (result [j] == widget) break;
                j++;
            }
            if (j == index) {
                if (index == result.length) {
                    Shell [] newResult = new Shell [index + 16];
                    System.arraycopy (result, 0, newResult, 0, index);
                    result = newResult;
                }
                result [index++] = (Shell) widget;
            }
        }
    }
    if (index == result.length) return result;
    Shell [] newResult = new Shell [index];
    System.arraycopy (result, 0, newResult, 0, index);
    return newResult;	
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
public Thread getSyncThread () {
    synchronized (Device.class) {
        if (isDisposed ()) error (SWT.ERROR_DEVICE_DISPOSED);
        return synchronizer.syncThread;
    }
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
 * @since 3.0
 */
public Tray getSystemTray () {
    checkDevice ();
    if (! OS.QSystemTrayIcon_isSystemTrayAvailable() ) return null;
    if (tray != null) return tray;
    return tray = new Tray (this, SWT.NONE);
}

/**
 * Returns the user-interface thread for the receiver.
 *
 * @return the receiver's user-interface thread
 *
 * @exception SWTException <ul>
 *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
 * </ul>
 */
public Thread getThread () {
    synchronized (Device.class) {
        if (isDisposed ()) error (SWT.ERROR_DEVICE_DISPOSED);
        return thread;
    }
}

private static int getTimerHandleIndex(int timerHandle) {
    if(timerHandles == null) return -1;
    int index;
    for(index = 0; index < timerHandles.length; ++index) {
        if(timerHandles[index] == timerHandle) {
            return index;
        }
    }
    return -1;
}

static Widget getWidget (int handle) {
    if (handle == 0) return null;
    if (lastWidget != null && lastHandle == handle) return lastWidget;
    int index = OS.QObject_property( handle, SWT_OBJECT_INDEX) - 1;
    if (0 <= index && index < widgetTable.length) {
        lastHandle = handle;
        return lastWidget = widgetTable [(int)index];
    }
    return null;
}

static private boolean handleNativeSwtEvent(Widget widget, int widgetHandle,
        int eventType, int time, int arg1, int arg2, int arg3, int arg4,
        int arg5) {

    switch (eventType) {
    case OS.QSWTEVENT_SYSTEMSHUTDOWN:
        qt_swt_event_systemShutdown();
        return false;
    case OS.QSWTEVENT_RESOURCECHANGE:
    	if(mobileDevice!=null){
    		Internal_MobilePackageSupport.qt_swt_event_mobiledevice_orientationchanged(mobileDevice);
        }
        return false;    
    case OS.QSWTEVENT_MOBILEDEVICEOPENED:
    	if(mobileDevice!=null){
    		Internal_MobilePackageSupport.qt_swt_event_mobiledevice_changed(mobileDevice, true);
        }
        return false;
    case OS.QSWTEVENT_MOBILEDEVICECLOSED:
    	if(mobileDevice!=null){
    		Internal_MobilePackageSupport.qt_swt_event_mobiledevice_changed(mobileDevice, false);
        }
        return false;    
    case OS.QSWTEVENT_SCREENACTIVATED:
    	if(mobileDevice!=null){
    		Internal_MobilePackageSupport.qt_swt_event_mobiledevice_screen_activate(mobileDevice, true);
        }
        return false;
    case OS.QSWTEVENT_SCREENDEACTIVATED:
    	if(mobileDevice!=null){
    		Internal_MobilePackageSupport.qt_swt_event_mobiledevice_screen_activate(mobileDevice, false);
        }
        return false;    
        
    default:
        break;
     }

    if(widget == null) return false;

    // Note that return values here are meaningless, these events are understood
    // by our implementation only and won't be passed to anywhere after this.

    switch (eventType) {
    case OS.QSWTEVENT_WIDGETPAINTED:
        widget.qt_swt_event_widgetPainted( widgetHandle, arg1,  arg2,  arg3,  arg4, arg5 );
        return false;
    case OS.QSWTEVENT_WIDGETRESIZED:
        if(widget.packageProxy != null) {
            widget.packageProxy.qt_swt_event_widgetResized(widgetHandle, arg1, arg2, arg3, arg4);
        } else {
            widget.qt_swt_event_widgetResized_pp(widgetHandle, arg1, arg2, arg3, arg4, true);
        }
        return false;
    case OS.QSWTEVENT_WIDGETMOVED:
        widget.qt_swt_event_widgetMoved(widgetHandle);
        return false;
    case OS.QSWTEVENT_BUFFERFLUSH:
        widget.qt_swt_event_bufferFlush();
        return false;
    case OS.QSWTEVENT_SYMBIAN_WINDOW_FULLY_VISIBLE:
    case OS.QSWTEVENT_SYMBIAN_WINDOW_PARTIALLY_VISIBLE:
        widget.qt_swt_event_symbianWindowShow();
        return false;
    case OS.QSWTEVENT_SYMBIAN_WINDOW_NOT_VISIBLE:
        widget.qt_swt_event_symbianWindowHide();
        return false;
    default:
        return false;
    }
}

static private boolean handleQtEvent(Widget widget, int widgetHandle, int eventType,
        int arg1, int arg2, int arg3, int arg4, int arg5) {

    // These events are coming from an application event filter of QApplication.
    // The return value of this method will be returned by the filter.
	
    if (eventType == OS.QEVENT_KEYPRESS && arg1 == OS.QT_KEY_NO) {
    	qt_key_event_endKeyPressed();
        return false;
    }
    
    if (widget == null) {
        return false;
    }

    switch (eventType) {
    case OS.QEVENT_PAINT:
        return widget.qt_event_paint( widgetHandle, arg1,  arg2,  arg3,  arg4, arg5 );
    case OS.QEVENT_KEYPRESS:
        if(widget.packageProxy != null) {
            return widget.packageProxy.qt_event_keypress( widgetHandle, arg1,  arg2,  arg3, arg4);
        } else {
            return widget.qt_event_keypress_pp( widgetHandle, arg1,  arg2,  arg3, arg4);
        }
    case OS.QEVENT_KEYRELEASE:
        if(widget.packageProxy != null) {
            return widget.packageProxy.qt_event_keyrelease( widgetHandle, arg1,  arg2,  arg3, arg4 );
        } else {
            return widget.qt_event_keyrelease_pp( widgetHandle, arg1,  arg2,  arg3, arg4 );
        }
    case OS.QEVENT_MOUSEBUTTONPRESS:
        if(widget.packageProxy != null) {
            return widget.packageProxy.qt_event_mouseButtonPress( widgetHandle, arg1, arg2, arg3, arg4, arg5);
        } else {
            return widget.qt_event_mouseButtonPress_pp( widgetHandle, arg1, arg2, arg3, arg4, arg5);
        }
    case OS.QEVENT_MOUSEBUTTONRELEASE:
        if(widget.packageProxy != null) {
            return widget.packageProxy.qt_event_mouseButtonRelease( widgetHandle, arg1, arg2, arg3, arg4, arg5);
        } else {
            return widget.qt_event_mouseButtonRelease_pp( widgetHandle, arg1, arg2, arg3, arg4, arg5);
        }
    case OS.QEVENT_MOUSEBUTTONDBLCLICK:
        if(widget.packageProxy != null) {
            return widget.packageProxy.qt_event_mouseButtonDblClick( widgetHandle, arg1, arg2, arg3, arg4, arg5);
        } else {
            return widget.qt_event_mouseButtonDblClick_pp( widgetHandle, arg1, arg2, arg3, arg4, arg5);
        }
    case OS.QEVENT_MOUSEMOVE:
        return widget.qt_event_mouseMove( widgetHandle, arg1, arg2, arg3, arg4, arg5);
    case OS.QEVENT_CLOSE:
        return widget.qt_event_close();
    case OS.QEVENT_FOCUSIN:
        return widget.qt_event_focusIn( arg1 );
    case OS.QEVENT_FOCUSOUT:
        return widget.qt_event_focusOut( arg1 );
    case OS.QEVENT_CONTEXTMENU:
        return widget.qt_event_contextmenu(arg1, arg2, arg3);
    case OS.QEVENT_SHORTCUT:
        return widget.qt_event_shortcut(shortcutOverrideKey, shortcutOverrideModifier, shortcutOverrideCharacter);
    case OS.QEVENT_SHORTCUTOVERRIDE:
        // Use the stored values from the last shortcutOverride event for the
        // key/traverse event
        shortcutOverrideKey = arg1;
        shortcutOverrideModifier = arg2;
        shortcutOverrideCharacter = arg3;
        return false;
    case OS.QEVENT_WINDOWACTIVATE:
        return widget.qt_event_windowActivate(widgetHandle);
    case OS.QEVENT_WINDOWDEACTIVATE:
        return widget.qt_event_windowDeactivate(widgetHandle);
    case OS.QEVENT_WINDOWSTATECHANGE:
        return widget.qt_event_windowStateChange(widgetHandle, arg1);
    case OS.QEVENT_APPLICATIONFONTCHANGE: // Fall through
    case OS.QEVENT_APPLICATIONLAYOUTDIRECTIONCHANGE: // Fall through
    case OS.QEVENT_APPLICATIONPALETTECHANGE:
        sendSettings();
        return false;
    default:
        return false;
    }
}

static private boolean handleQtSignal(Widget widget, int widgetHandle,
        int eventType, int arg1, int arg2, int arg3, int arg4, int arg5,
        String arg6) {

    // Note that return values here are meaningless. Signals are already
    // coming from the widgets and won't be passed to anywhere after this.

    // If not a widget, it might be a timer.
    int timerHandleIndex = -1;
    if (widget == null && widgetHandle != handle) {
        timerHandleIndex = getTimerHandleIndex(widgetHandle);
        if(timerHandleIndex == -1 ) {
            return false;
        }
    }

    switch (eventType) {
    case OS.QSIGNAL_PRESSED:
        widget.qt_signal_pressed();
        return false;
    case OS.QSIGNAL_RELEASED:
        widget.qt_signal_released();
        return false;
    case OS.QSIGNAL_SLIDER_CHANGED:
        widget.qt_signal_slider_changed(arg1);
        return false;
    case OS.QSIGNAL_SLIDER_ACTION_TRIGGERED:
        widget.qt_signal_slider_action_triggered(arg1);
        return false;
    case OS.QSIGNAL_SLIDER_RELEASED:
        widget.qt_signal_slider_released();
        return false;
    case OS.QSIGNAL_TIMEOUT:
        qt_signal_timer(timerHandleIndex);
        return false;
    case OS.QSIGNAL_ABOUTTOHIDE:
        widget.qt_signal_aboutToHide();
        return false;
    case OS.QSIGNAL_ABOUTTOSHOW:
        widget.qt_signal_aboutToShow();
        return false;
    case OS.QSIGNAL_TRIGGERED:
        if(widget.packageProxy != null) {
            widget.packageProxy.qt_signal_triggered( (arg1 > 0) );
        } else {
            widget.qt_signal_triggered_pp( (arg1 > 0) );
        }
        return false;
    case OS.QSIGNAL_HOVER:
        widget.qt_signal_hover();
        return false;
    case OS.QSIGNAL_SELECTIONCHANGED:
        if(widget.packageProxy != null) {
            widget.packageProxy.qt_signal_selectionChanged( arg1, arg2 );
        } else {
            widget.qt_signal_selectionChanged_pp( arg1, arg2 );
        }
        return false;
    case OS.QSIGNAL_RETURN_PRESSED:
        if(widget.packageProxy != null) {
            widget.packageProxy.qt_signal_return_pressed();
        } else {
            widget.qt_signal_return_pressed_pp();
        }
        return false;
    case OS.QSIGNAL_TABLE_ITEMSELECTION_CHANGED:
        widget.qt_signal_table_itemSelectionChanged();
        return false;
    case OS.QSIGNAL_TABLE_CURRENTCELL_CHANGED:
        widget.qt_signal_table_currentCellChanged(arg1, arg2, arg3, arg4);
        return false;
    case OS.QSIGNAL_TABLE_CELL_CHANGED:
        widget.qt_signal_table_cellChanged(arg1, arg2);
        return false;
    case OS.QSIGNAL_TABLE_CELL_ACTIVATED:
        widget.qt_signal_table_cellActivated(arg1, arg2);
        return false;
    case OS.QSIGNAL_LIST_ITEMSELECTION_CHANGED:
        if(widget.packageProxy != null) {
            widget.packageProxy.qt_signal_list_itemSelectionChanged();
        } else {
            widget.qt_signal_list_itemSelectionChanged_pp();
        }
        return false;
    case OS.QSIGNAL_LIST_ITEM_DOUBLECLICKED:
        if(widget.packageProxy != null) {
            widget.packageProxy.qt_signal_list_itemDoubleClicked( arg1 );
        } else {
            widget.qt_signal_list_itemDoubleClicked_pp( arg1 );
        }
        return false;
    case OS.QSIGNAL_LIST_ITEM_ACTIVATED:
        if(widget.packageProxy != null) {
            widget.packageProxy.qt_signal_list_itemActivated(arg1);
        } else {
            widget.qt_signal_list_itemActivated_pp(arg1);
        }
        return false;
    case OS.QSIGNAL_DATETIMEEDIT_CHANGED:
        if(widget.packageProxy != null) {
            widget.packageProxy.qt_signal_dateedit_dateTimeChanged();
        } else {
            widget.qt_signal_dateedit_dateTimeChanged_pp();
        }
        return false;
    case OS.QSIGNAL_TRAY_ACTIVATED:
        widget.qt_signal_tray_activated( arg1 );
        return false;
    case OS.QSIGNAL_TREE_ITEMSELECTION_CHANGED:
        widget.qt_signal_tree_itemSelectionChanged( );
        return false;
    case OS.QSIGNAL_TREE_ITEM_ACTIVATED:
        widget.qt_signal_tree_itemActivated( arg1, arg2 );
        return false;
    case OS.QSIGNAL_TREE_ITEM_EXPANDED:
        widget.qt_signal_tree_itemExpanded( arg1 );
        return false;
    case OS.QSIGNAL_TREE_ITEM_COLLAPSED:
        widget.qt_signal_tree_itemCollapsed( arg1 );
        return false;
    case OS.QSIGNAL_TEXT_CHANGED:
        if(widget.packageProxy != null) {
            widget.packageProxy.qt_signal_text_changed(arg6);
        } else {
            widget.qt_signal_text_changed_pp(arg6);
        }
        return false;
    case OS.QSIGNAL_LINKACTIVATED:
        if(widget.packageProxy != null) {
            widget.packageProxy.qt_signal_linkActivated(arg6);
        } else {
            widget.qt_signal_linkActivated_pp(arg6);
        }
        return false;
    case OS.QSIGNAL_CURRENT_TAB_CHANGED:
        if(widget.packageProxy != null) {
            widget.packageProxy.qt_signal_current_tab_changed(arg1);            
        } else {
            widget.qt_signal_current_tab_changed_pp(arg1);
        }
        return true;
    case OS.QSIGNAL_ABSTRACTITEMVIEW_ACTIVATED:
        if(widget.packageProxy != null) {
            widget.packageProxy.qt_signal_abstractitemview_activated(arg1, arg2);
        } else {
            widget.qt_signal_abstractitemview_activated_pp(arg1, arg2);
        }
        return true;
    case OS.QSIGNAL_TEXT_CURSORPOSITIONCHANGED:
        if(widget.packageProxy != null) {
            widget.packageProxy.qt_signal_text_cursorPositionChanged(arg1, arg2);
        } else {
            widget.qt_signal_text_cursorPositionChanged_pp(arg1, arg2);
        }
        return true;
    case OS.QSIGNAL_TEXT_SELECTIONCHANGED:
        if(widget.packageProxy != null) {
            widget.packageProxy.qt_signal_text_selectionChanged();
        } else {
            widget.qt_signal_text_selectionChanged_pp();
        }
        return true;
    case OS.QSIGNAL_CLICKED:
    	widget.qt_signal_clicked();
    	return false;
    case OS.QSIGNAL_QAPPLICATION_FOCUSCHANGED:
        qt_signal_qapplication_focusChanged(arg1, arg2);
        return false;
    case OS.QSIGNAL_QAPPLICATION_FOCUSCHANGED_TO_WIDDGET:
        if (widget.packageProxy != null) {
            widget.packageProxy.qt_signal_qapplication_focusChanged(arg1,
                    arg2);
        } else {
            widget.qt_signal_qapplication_focusChanged_pp(arg1, arg2);
        }
        return false;
		case OS.QSIGNAL_QDESKTOPWIDGET_SCREENCOUNTCHANGED:
    	if(mobileDevice!=null){
    		Internal_MobilePackageSupport.qt_signal_screen_changed(mobileDevice, arg1);
        }
       return false;
	case OS.QSIGNAL_REQUESTCOMPLETED:
        if(widget.packageProxy != null) {
            widget.packageProxy.qt_signal_requestCompleted(arg1);
        } else {
            widget.qt_signal_requestCompleted_pp(arg1);
        }
	    
	    return false;
    default:
        return false;
    }
}

static void hookEvents() {
    int focusSignalProxy = OS.SignalHandler_new(handle, OS.QSIGNAL_QAPPLICATION_FOCUSCHANGED);
    OS.QObject_connectOrThrow(handle, "focusChanged(QWidget*, QWidget*)", focusSignalProxy,
            "widgetSignal(QWidget*, QWidget*)", OS.QT_AUTOCONNECTION);
}

protected void init () {
    checkSubclass ();
    checkDisplay(thread = Thread.currentThread (), false);
    if(!Internal_GfxPackageSupport.initialized()) {
    	createDisplay();
	    synchronizer = new Synchronizer (this);
	    commandArranger = new CommandArranger(this);
    }
    register (this);
    if (Default == null && !Internal_GfxPackageSupport.internal(this)) Default = this;
    
    super.init ();
    if(!Internal_GfxPackageSupport.initialized()) {
    	initializeWidgetTable ();
    }
}

static void initializeWidgetTable () {
    indexTable = new int [GROW_SIZE];
    widgetTable = new Widget [GROW_SIZE];
    for (int i=0; i<GROW_SIZE-1; i++) indexTable [i] = i + 1;
    indexTable [GROW_SIZE - 1] = -1;
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
public void internal_dispose_GC(GCData data) {
    checkDevice ();
    data.internalGc.releaseTarget();
    data.internalGc.dispose();
    setScreenPaintingAttributes(false);
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
 *    <li>ERROR_NO_HANDLES if a handle could not be obtained for gc creation</li>
 * </ul>
 */
public int internal_new_GC (GCData data) {
    checkDevice ();

    // GC creation on Display is not currently supported
    SWT.error (SWT.ERROR_NOT_IMPLEMENTED);
    
    setScreenPaintingAttributes(true);

    data.internalGc = new GraphicsContext();
    data.drawable = OS.QWidget_swt_paintDevice(handle);
    data.device = this;
    data.background = Internal_GfxPackageSupport.newColor(this, 0x00FFFFFF); // white
    data.foreground = Internal_GfxPackageSupport.newColor(this, 0x00000000); // black
    data.font = Internal_GfxPackageSupport.newFont(this, OS.SwtFontCache_cache(OS.QApplication_swt_font_new()));
    data.style &= ~SWT.MIRRORED; // Display always left-to-right
    data.buffered = false;
    data.internalGc.bindTarget(this);
    return data.internalGc.getHandle();
}

static void interrupt() {
    /*
     * Try to interrupt to make the native event processing methods return. By
     * default Qt processes all available events in one iteration. This is
     * a problem e.g. when an exception has been thrown in a listener. It
     * depends on the event dispatcher implementation what this actually does.
     * It might be that interrupting is only possible between native UI main
     * event loop iterations and in each iteration several QEvents can get
     * dispatched.
     */
	synchronized(Device.class) {
		if(handle == 0) return;
	}
    int threadHandle = OS.QObject_thread(handle);
    int dispatcher = OS.QAbstractEventDispatcher_instance(threadHandle);
    OS.QAbstractEventDispatcher_interrupt(dispatcher);
}

static boolean isValidClass (Class clazz) {
    String name = clazz.getName();
        int index = name.lastIndexOf('.');
        return (name.substring(0, index + 1).equals(PACKAGE_PREFIX)
                || name.substring(0, index + 1).equals(
                        ERCP_MOBILE_PACKAGE_PREFIX) || name.substring(0,
                index + 1).equals(INTERNAL_EXTENSION_PACKAGE_PREFIX));
    }

boolean isValidThread () {
    return thread == Thread.currentThread ();
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
public Point map (Control from, Control to, Point point) {
    checkDevice ();
    if (point == null) error (SWT.ERROR_NULL_ARGUMENT);
    return map (from, to, point.x, point.y);
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
public Point map (Control from, Control to, int x, int y) {
    checkDevice();
    if (from != null && from.isDisposed()) SWT.error (SWT.ERROR_INVALID_ARGUMENT);
    if (to != null && to.isDisposed()) SWT.error (SWT.ERROR_INVALID_ARGUMENT);

    Point point = new Point(x, y);
    if (to != null && from != null) {
        point = from.toDisplay(point);
        point = to.toControl(point);
    } else if (to != null) {
        point = to.toControl(point);
    } else if (from != null) {
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
public Rectangle map (Control from, Control to, Rectangle rectangle) {
    checkDevice();
    if (rectangle == null) error (SWT.ERROR_NULL_ARGUMENT);
    return map (from, to, rectangle.x, rectangle.y, rectangle.width, rectangle.height);
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
public Rectangle map (Control from, Control to, int x, int y, int width, int height) {
    checkDevice();
    if (from != null && from.isDisposed()) SWT.error (SWT.ERROR_INVALID_ARGUMENT);
    if (to != null && to.isDisposed()) SWT.error (SWT.ERROR_INVALID_ARGUMENT);

    Rectangle rectangle = new Rectangle(x, y, width, height);
    if (to != null && from != null) {
        Point topLeft = from.toDisplay(x, y);
        topLeft = to.toControl(topLeft);
        rectangle.x = topLeft.x;
        rectangle.y = topLeft.y;
    } else if (to != null) {
        Point topLeft = to.toControl(x, y);
        rectangle.x = topLeft.x;
        rectangle.y = topLeft.y;
    } else if (from != null) {
        Point topLeft = from.toDisplay(x, y);
        rectangle.x = topLeft.x;
        rectangle.y = topLeft.y;
    }
    return rectangle;
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
public boolean post (Event event) {
    if (isDisposed ()) error (SWT.ERROR_DEVICE_DISPOSED);
    if (event == null) error (SWT.ERROR_NULL_ARGUMENT);

    int modifiers = unTranslateModifiers(event.stateMask);
    int type = unTranslateEventType(event.type);

    int targetHandle = 0;
    int eventHandle = 0;

    if(event.widget == null) {
        targetHandle = OS.QApplication_focusWidget();
    } else {
        targetHandle = event.widget.handle;
    }
    if(targetHandle == 0) return false;

    switch (type) {
        case OS.QEVENT_KEYPRESS:
        case OS.QEVENT_KEYRELEASE:
            // If neither keyCode nor character is given then abort
            if(event.keyCode == 0 && event.character == 0) {
                return false;
            }

            // Primarily try to map the keyCode to Qt:Key
            int key = untranslateKey(event.keyCode);
            // If keyCode was given in the Event
            if(event.keyCode != 0) {
                // Given keyCode couldn't be translated, use it directly
                if(key == 0) {
                    key = event.keyCode;
                }
            }

            // Secondarily if keyCode couldn't be mapped try the character
            if(key == 0) {
                switch(event.character) {
                    case SWT.BS: key = OS.QT_KEY_BACKSPACE; break;
                    case SWT.CR: key = OS.QT_KEY_RETURN; break;
                    case SWT.DEL: key = OS.QT_KEY_DELETE; break;
                    case SWT.ESC: key = OS.QT_KEY_ESCAPE; break;
                    case SWT.TAB: key = OS.QT_KEY_TAB; break;
                    /*
                    * Since there is no LF key on the keyboard, do not attempt
                    * to map LF to CR or attempt to post an LF key.
                    */
                    case SWT.LF: return false;
                    default:
                        key = Character.toUpperCase(event.character);
                        break;
                }
            }
            String character = event.character + "";
            eventHandle = OS.QKeyEvent_new(type, key, modifiers, character);
            break;
        case OS.QEVENT_MOUSEBUTTONPRESS:
        case OS.QEVENT_MOUSEMOVE:
        case OS.QEVENT_MOUSEBUTTONRELEASE:
        case OS.QEVENT_MOUSEBUTTONDBLCLICK:
            Control widget = (Control) getWidget ( targetHandle );
            if((widget.getStyle() & SWT.MIRRORED) != 0){
                event.x = widget.getClientWidth() - event.x;
            }

            Point global = OS.QWidget_mapToGlobal(targetHandle, event.x, event.y);
            int button = unTranslateButton(event.button);
            if (type != OS.QEVENT_MOUSEMOVE && button == OS.QT_NOBUTTON) {
                return false;
            }
            eventHandle = OS.QMouseEvent_new(type, event.x, event.y, global.x,
                    global.y, button, button, modifiers);
            break;
        default:
            break;
    }

    if(eventHandle != 0 && targetHandle != 0) {
        // Takes ownership of the event handle
        OS.QCoreApplication_postEvent(handle, targetHandle, eventHandle);
        return true;
    }

    return false;
}

static void postEvent (Event event) {
    /*
    * Place the event at the end of the event queue.
    * This code is always called in the Display's
    * thread so it must be re-enterant but does not
    * need to be synchronized.
    */
    if (eventQueue == null) eventQueue = new Event [4];
    int index = 0;
    int length = eventQueue.length;
    while (index < length) {
        if (eventQueue [index] == null) break;
        index++;
    }
    if (index == length) {
        Event [] newQueue = new Event [length + 4];
        System.arraycopy (eventQueue, 0, newQueue, 0, length);
        eventQueue = newQueue;
    }
    eventQueue [index] = event;
}

static void qt_signal_qapplication_focusChanged(int old, int now) {
    Widget oldWidget = null, nowWidget = null;

    if(old != 0) oldWidget = getWidget(old);
    if(now != 0) nowWidget = getWidget(now);

    if(oldWidget == nowWidget) return;

    if(oldWidget != null) {
        oldWidget.qt_swt_event_focusWasLost();
    }

    if(nowWidget != null) {
        // It's possible that focus-lost event listener disposed the widget that
        // gained the focus.
        if(nowWidget.isDisposed()) return;

        nowWidget.qt_swt_event_focusWasGained();
    }
}

static void qt_signal_timer (int timerHandleIndex) {
    if (timerRunnables == null || timerHandles == null) return;

    Runnable runnable = timerRunnables [timerHandleIndex];
    timerRunnables [timerHandleIndex] = null;

    // All timers are one-shot, can be destroyed now.
    // Deletion is scheduled to when control returns to the event loop
    // because it's not allowed to delete the QObject here inside its
    // event handler.
    QObjectDeleteWrapper.deleteSafely(timerHandles [timerHandleIndex]);
    timerHandles [timerHandleIndex] = 0;

    if (runnable != null) runnable.run ();
}

static void qt_key_event_endKeyPressed() {
    Event event = new Event ();
    sendEvent (SWT.Close, event);
    if (event.doit) {
    	if(Default != null) {
    		Default.dispose ();
    	}
    }
}

static void qt_swt_event_systemShutdown(){
    Event event = new Event ();
    sendEvent (SWT.Close, event);
    ExitNotificationWrapper.notifyExit();
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
public boolean readAndDispatch() {
    checkDevice();
    if (OS.QCoreApplication_hasPendingEvents(handle)) {
        if (repostFailedQEvents()
                || sendQEvents()
                || sendPostedQEvents()
                || sendPostedSwtEvents()
                || sendDeferredDeletionQEvents()) {
            // Display has been disposed in a listener
            tryDestroy();
            return false;
        }
        return true;
    }

    boolean asyncRetVal = runAsyncMessages(false);
    if (disposed) {
        tryDestroy();
        return false;
    }
    return asyncRetVal;
}

static Point readAndClearImageLoadSize() {
	Point ilSize = Display.imageLoadSize;
	Display.imageLoadSize = null;
	return ilSize;
}

static void register (Display display) {
    synchronized (Device.class) {
        for (int i=0; i<Displays.length; i++) {
            if (Displays [i] == null) {
                Displays [i] = display;
                return;
            }
        }
        Display [] newDisplays = new Display [Displays.length + 4];
        System.arraycopy (Displays, 0, newDisplays, 0, Displays.length);
        newDisplays [Displays.length] = display;
        Displays = newDisplays;
    }
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
 * @see Device#dispose
 * @see #destroy
 */
protected void release () {
    sendEvent (SWT.Dispose, new Event ());
    Shell [] shells = getShells ();
    for (int i=0; i<shells.length; i++) {
        Shell shell = shells [i];
        if (!shell.isDisposed ())  shell.dispose ();
    }
    final boolean lastDisplay = (displayCount() == 1);
    if(lastDisplay) {
	    commandArranger.dispose();
	    if (tray != null) tray.dispose ();
	    tray = null;
    }
    while (readAndDispatch ()) {}
    if (disposeList != null) {
        for (int i=0; i<disposeList.length; i++) {
            if (disposeList [i] != null) disposeList [i].run ();
        }
    }
    if(lastDisplay) {
	    synchronizer.releaseSynchronizer ();
	    synchronizer = null;
	    lastWidget = null;
	    indexTable = null;
	    widgetTable = null;
	    freeSlot = 0;
	    lastHandle = 0;
    }
    thread = null;
    disposeList = null;
    super.release ();
}

/**
 * Removes the listener from the collection of listeners who will
 * be notified when an event of the given type occurs anywhere in
 * a widget. The event type is one of the event constants defined
 * in class <code>SWT</code>.
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
 * @see SWT
 * @see #addFilter
 * @see #addListener
 *
 * @since 3.0
 */
public void removeFilter (int eventType, Listener listener) {
    checkDevice ();
    if (listener == null) error (SWT.ERROR_NULL_ARGUMENT);
    if (filterTable == null) return;
    filterTable.unhook (eventType, listener);
    if (filterTable.size () == 0) filterTable = null;
}

/**
 * Removes the listener from the collection of listeners who will
 * be notified when an event of the given type occurs. The event type
 * is one of the event constants defined in class <code>SWT</code>.
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
 * @see SWT
 * @see #addListener
 *
 * @since 2.0
 */
public void removeListener (int eventType, Listener listener) {
    checkDevice ();
    if (listener == null) error (SWT.ERROR_NULL_ARGUMENT);
    if (eventTable == null) return;
    eventTable.unhook (eventType, listener);
}

static Widget removeWidget (int handle) {
    if (handle == 0) return null;
    lastWidget = null;
    Widget widget = null;
    int index = (int)/*64*/ OS.QObject_property( handle, SWT_OBJECT_INDEX ) - 1;
    if (0 <= index && index < widgetTable.length) {
        widget = widgetTable [index];
        widgetTable [index] = null;
        indexTable [index] = freeSlot;
        freeSlot = index;
        OS.QObject_setProperty( handle, SWT_OBJECT_INDEX, 0 );
    }
    return widget;
}

/*
 * Repost failed events for trying again in the next round. If posted
 * deferred deletion events get sent by Qt but we notice in SWT that an
 * object is about to get deleted in its event handler then this mechanism
 * is used cancel the event and to retry it later.
 */
final private boolean repostFailedQEvents() {
    OS.SwtApplication_postDeferredEvents();
    return disposed;
}

static boolean runAsyncMessages (boolean all) {
    return synchronizer.runAsyncMessages (all);
}

static boolean runDeferredEvents () {
    /*
    * Run deferred events.  This code is always
    * called in the Display's thread so it must
    * be re-entrant but need not be synchronized.
    */
    while (eventQueue != null) {

        /* Take an event off the queue */
        Event event = eventQueue [0];
        if (event == null) break;
        int length = eventQueue.length;
        System.arraycopy (eventQueue, 1, eventQueue, 0, --length);
        eventQueue [length] = null;

        /* Run the event */
        Widget widget = event.widget;
        if (widget != null && !widget.isDisposed ()) {
            Widget item = event.item;
            if (item == null || !item.isDisposed ()) {
                widget.sendEvent (event);
            }
        }

        /*
        * At this point, the event queue could
        * be null due to a recursive invocation
        * when running the event.
        */
    }

    /* Clear the queue */
    eventQueue = null;
    return true;
}

static void runSettings () {
    Control[] controls = getControls();
    updateImages(controls);
    updateLayoutDirections(controls);
    sendEvent(SWT.Settings, null);
    for(int i = 0; i < controls.length; i++) {
        controls[i].doRedraw();
    }
    Shell [] shells = getShells_();
    for(int i = 0; i < shells.length; i++) {
        Shell shell = shells [i];
        if(!shell.isDisposed ()) {
            shell.layout(true, true);
        }
    }
}

/*
 * Deferred deletion events (from QObject.deleteLater() calls) have to be
 * explicitly posted or they won't get sent within the same level event loop
 * that posted them. Note that in some circumstances also plain
 * sendPostedEvents for all event types may post also these.
 */
final private boolean sendDeferredDeletionQEvents() {
    OS.QCoreApplication_sendPostedEvents(handle, 0, OS.QEVENT_DEFERREDDELETE);
    return disposed;
}

static void sendEvent (int eventType, Event event) {
    if (eventTable == null && filterTable == null) {
        return;
    }
    if (event == null) event = new Event ();
    event.display = Default;
    event.type = eventType;
    if (event.time == 0) event.time = getLastEventTime ();
    if (!filterEvent (event)) {
        if (eventTable != null) eventTable.sendEvent (event);
    }
}

final private boolean sendPostedQEvents() {
    OS.QCoreApplication_sendPostedEvents( handle );
    return disposed;
}

/*
 * Run listeners for the posted SWT events. For these events the corresponding
 * QEvent has been fully processed before calling the listener. E.g. mouse events
 * usually get this treatment so that they appear responsive regardless of what
 * is in the mouse listener.
 */
final private boolean sendPostedSwtEvents() {
    runDeferredEvents();
    return disposed;
}

final private boolean sendQEvents() {
    OS.QCoreApplication_processEvents( handle, OS.QEVENTLOOP_ALLEVENTS);
    return disposed;
}

static private void sendSettings() {
    Display instance = Default != null ? Default : Internal;
	if(settingsRunnable != null) {
        // Cancel the existing timer
        instance.timerExec(-1, settingsRunnable);
    }
    instance.timerExec(SETTINGS_DELAY, settingsRunnable = new Runnable() {
        public void run() {
            runSettings();
            settingsRunnable = null;
        }
    });
}

/**
 * On platforms which support it, sets the application name
 * to be the argument. On Motif, for example, this can be used
 * to set the name used for resource lookup.  Specifying
 * <code>null</code> for the name clears it.
 *
 * @param name the new app name or <code>null</code>
 */
public static void setAppName (String name) {
    APP_NAME = name;
    if (Display.handle != 0 && (!Default.disposed || !Internal.disposed)) {
        OS.QCoreApplication_setApplicationName(handle, name);
        OS.setSymbianAppName(name);
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
public void setData (String key, Object value) {
    checkDevice ();
    if (key == null) error (SWT.ERROR_NULL_ARGUMENT);

    if (key.equals (ADD_WIDGET_KEY)) {
        Object [] data = (Object [])value;
        int handle = ((Integer)data [0]).intValue();
        Widget widget = (Widget)data [1];
        addWidget (handle, widget);
        return;
    }

    if (key.equals (REMOVE_WIDGET_KEY)) {
        Object [] data = (Object [])value;
        int handle = ((Integer)data [0]).intValue();
        removeWidget (handle);
        return;
    }
    
    if(key.equals (IMAGE_LOAD_SIZE_KEY)) {
    	if(value instanceof Point) {
    		imageLoadSize = (Point)value;
    	}
    	return;
    }

    /* Remove the key/value pair */
    if (value == null) {
        if (keys == null) return;
        int index = 0;
        while (index < keys.length && !keys [index].equals (key)) index++;
        if (index == keys.length) return;
        if (keys.length == 1) {
            keys = null;
            values = null;
        } else {
            String [] newKeys = new String [keys.length - 1];
            Object [] newValues = new Object [values.length - 1];
            System.arraycopy (keys, 0, newKeys, 0, index);
            System.arraycopy (keys, index + 1, newKeys, index, newKeys.length - index);
            System.arraycopy (values, 0, newValues, 0, index);
            System.arraycopy (values, index + 1, newValues, index, newValues.length - index);
            keys = newKeys;
            values = newValues;
        }
        return;
    }

    /* Add the key/value pair */
    if (keys == null) {
        keys = new String [] {key};
        values = new Object [] {value};
        return;
    }
    for (int i=0; i<keys.length; i++) {
        if (keys [i].equals (key)) {
            values [i] = value;
            return;
        }
    }
    String [] newKeys = new String [keys.length + 1];
    Object [] newValues = new Object [values.length + 1];
    System.arraycopy (keys, 0, newKeys, 0, keys.length);
    System.arraycopy (values, 0, newValues, 0, values.length);
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
public void setData (Object data) {
    checkDevice ();
    Display.data = data;
}

static void setScreenPaintingAttributes(boolean on) {
    OS.QWidget_setAttribute(OS.QApplication_desktop(), OS.QT_WA_PAINTOUTSIDEPAINTEVENT, on);
    OS.QWidget_setAttribute(OS.QApplication_desktop(), OS.QT_WA_PAINTONSCREEN, on);
    OS.QWidget_setAttribute(OS.QApplication_desktop(), OS.QT_WA_PAINTUNCLIPPED, on);
}

static void setModalDialog (Dialog modalDialog) {
    Display.modalDialog = modalDialog;
    Shell [] shells = getShells_();
    for (int i=0; i<shells.length; i++) shells [i].updateModal ();
}

static void setModalShell (Shell shell) {
    if (modalShells == null) modalShells = new Shell [4];
    int index = 0, length = modalShells.length;
    while (index < length) {
        if (modalShells [index] == shell) return;
        if (modalShells [index] == null) break;
        index++;
    }
    if (index == length) {
        Shell [] newModalShells = new Shell [length + 4];
        System.arraycopy (modalShells, 0, newModalShells, 0, length);
        modalShells = newModalShells;
    }
    modalShells [index] = shell;
    Shell [] shells = getShells_();
    for (int i=0; i<shells.length; i++) shells [i].updateModal ();
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
public void setSynchronizer (Synchronizer synchronizer) {
    checkDevice ();
    if (synchronizer == null) error (SWT.ERROR_NULL_ARGUMENT);
    if (synchronizer == Display.synchronizer) return;
    Synchronizer oldSynchronizer;
    synchronized (Device.class) {
        oldSynchronizer = Display.synchronizer;
        Display.synchronizer = synchronizer;
    }
    if (oldSynchronizer != null) {
        oldSynchronizer.runAsyncMessages(true);
    }
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
public boolean sleep () {
    checkDevice ();
    OS.QCoreApplication_processEvents( Display.handle, OS.QEVENTLOOP_WAITFORMOREEVENTS );
    if(disposed) {
        tryDestroy();
    }
    return true;
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
 *    <li>ERROR_FAILED_EXEC - if an exception occurred when executing the runnable</li>
 *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
 * </ul>
 *
 * @see #asyncExec
 */
public void syncExec (Runnable runnable) {
    Synchronizer synchronizer;
    synchronized (Device.class) {
        if (isDisposed ()) error (SWT.ERROR_DEVICE_DISPOSED);
        synchronizer = Display.synchronizer;
    }
    synchronizer.syncExec (runnable);
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
public void timerExec (int milliseconds, Runnable runnable) {
    checkDevice ();
    if (runnable == null) error (SWT.ERROR_NULL_ARGUMENT);
    // 4 is the initial size for the timer arrays, incremented as needed
    if (timerRunnables == null) timerRunnables = new Runnable [4];
    if (timerHandles == null) timerHandles = new int [4];
    int index = 0;
    // Check if runnable already in the list
    while (index < timerRunnables.length) {
        if (timerRunnables [index] == runnable) break;
        index++;
    }
    if (index != timerRunnables.length) {
        // Runnable was found from the list, remove the old timer and runnable.
        // The deletion is deferred in case timeExec is called from a timer event
        // of the old timer.
        QObjectDeleteWrapper.deleteSafely(timerHandles [index]);
        timerRunnables [index] = null;
        timerHandles [index] = 0;
    } else {
        // Runnable was not found from the list, find locations from the arrays
        // where to store the runnable and the timer
        index = 0;
        while (index < timerRunnables.length) {
            if (timerRunnables [index] == null) break;
            index++;
        }
        // No space in the arrays for more timers/runnables, expand them.
        // Reserve space for 4 more timers/runnables.
        // Note: This space is never freed. Arrays are never shrunk.
        if (index == timerRunnables.length) {
            Runnable [] newTimerRunnables = new Runnable [timerRunnables.length + 4];
            System.arraycopy (timerRunnables, 0, newTimerRunnables, 0, timerRunnables.length);
            timerRunnables = newTimerRunnables;
            int [] newTimerHandles = new int [timerHandles.length + 4];
            System.arraycopy (timerHandles, 0, newTimerHandles, 0, timerHandles.length);
            timerHandles = newTimerHandles;
        }
    }

    if (milliseconds < 0) return;

    int timerHandle = OS.QTimer_new();
    if (timerHandle != 0) {
        OS.QObject_setParent(timerHandle, handle); // QTimer is a child of QApplication
        OS.QTimer_setSingleShot(timerHandle, true);
        int signalProxy = OS.SignalHandler_new(timerHandle, OS.QSIGNAL_TIMEOUT);
        OS.QObject_connectOrThrow(timerHandle, "timeout()", signalProxy, "widgetSignal()", OS.QT_AUTOCONNECTION);
        OS.QTimer_start(timerHandle, milliseconds);
        timerHandles [index] = timerHandle;
        timerRunnables [index] = runnable;
    }
}

// Converts single mouse button from Qt to SWT for the MouseEvent.button
// field which takes integers, 1, 2, 3, ...
static int translateButton(int nativeButton) {
    switch(nativeButton) {
    case OS.QT_LEFTBUTTON:
        return 1;
    case OS.QT_MIDBUTTON:
        return 2;
    case OS.QT_RIGHTBUTTON:
        return 3;
    default:
        return 0;
    }
}

// Converts a mask of buttons from Qt to SWT
static int translateButtons(int buttons) {
    int state = 0;
    if((buttons & OS.QT_LEFTBUTTON) != 0) state |= SWT.BUTTON1;
    if((buttons & OS.QT_MIDBUTTON) != 0) state |= SWT.BUTTON2;
    if((buttons & OS.QT_RIGHTBUTTON) != 0) state |= SWT.BUTTON3;
    return state;
}

// Converts keys from Qt to SWT
static int translateKey (int key, boolean keypad ) {
    if( keypad ){
        for (int i=0; i<KeyPadKeyTable.length; i++) {
            if (KeyPadKeyTable [i] [0] == key) return KeyPadKeyTable [i] [1];
        }
    }
    for (int i = 0; i < KeyTable.length; i++) {
        if (KeyTable[i][0] == key)return KeyTable[i][1];
    }
    return 0;
}

// Converts modifiers from Qt to SWT
static int translateModifiers(int nativeModifiers) {
    int modifiers = 0;
    if ((nativeModifiers & OS.QT_ALTMODIFIER) != 0) modifiers |= SWT.ALT;
    if ((nativeModifiers & OS.QT_SHIFTMODIFIER) != 0) modifiers |= SWT.SHIFT;
    if ((nativeModifiers & OS.QT_CONTROLMODIFIER) != 0) modifiers |= SWT.CONTROL;
    return modifiers;
}

static int translateDirection(int orientation) {
    if(orientation ==  OS.QT_RIGHTTOLEFT) {
        return SWT.RIGHT_TO_LEFT;
    } else {
        return SWT.LEFT_TO_RIGHT;
    }
}

// Converts native scan code to key location (left/right) for CTRL/ALT/SHIFT/etc
static int translateKeyLocation(int nativeScanCode) {
    
    if(OS.windowServer == OS.WS_SYMBIAN_S60) {
        switch (nativeScanCode) {
        case SYMBIAN_SHIFT_LEFT:    //intentional fall through
        case SYMBIAN_ALT_LEFT:
        case SYMBIAN_CTRL_LEFT:
        case SYMBIAN_FN_LEFT:
            return SWT.LEFT;
        case SYMBIAN_SHIFT_RIGHT:   //intentional fall through
        case SYMBIAN_ALT_RIGHT:
        case SYMBIAN_CTRL_RIGHT:
        case SYMBIAN_FN_RIGHT:
            return SWT.RIGHT;
        }
    } else {
        switch (nativeScanCode) {
        case X11_SHIFT_LEFT:    //intentional fall through
        case X11_CONTROL_LEFT:
        case X11_META_LEFT:
        case X11_ALT_LEFT:
        case X11_SUPER_LEFT:
        case X11_HYPER_LEFT:
            return SWT.LEFT;
        case X11_SHIFT_RIGHT:   //intentional fall through
        case X11_CONTROL_RIGHT:
        case X11_META_RIGHT:
        case X11_ALT_RIGHT:
        case X11_SUPER_RIGHT:
        case X11_HYPER_RIGHT:
            return SWT.RIGHT;
        }
    }

    return 0;
}

private static void tryDestroy() {
	// Do nothing if there are Displays that have not been disposed
	if((Default != null && !Default.disposed) || 
			(Internal != null && !Internal.disposed)) {
		return;
	}
	
    // Destroy the QApplication only when all event loops have exited.
    if(handle != 0 && OS.SwtApplication_javaCallbackCount() == 0) {
        
    	OS.QApplication_closeAllWindows();

        if(mobileDevice!=null){
        	Internal_MobilePackageSupport.mobiledevice_dispose(mobileDevice);
        	mobileDevice = null;
        }
        
        if(mobileDeviceHandle != 0){
        	OS.MobileDevice_destroy(mobileDeviceHandle);
        	mobileDeviceHandle = 0;
        }
        QObjectDeleteWrapper.deleteNowWithoutSafetyCheck(handle);
        handle = 0;

        // Delete the JNI utility. After this there can't be any JNI activity.
        QObjectDeleteWrapper.deleteNowWithoutSafetyCheck(JniUtils.handle);
        JniUtils.handle = 0;

        // We are done with Qt, clean up all resources created for it.
        OS.cleanUpUiThread();
    }
}

// Converts mouse buttons from SWT to Qt
static int unTranslateButton(int button) {
    switch(button) {
    case 1:
        return OS.QT_LEFTBUTTON;
    case 2:
        return OS.QT_MIDBUTTON;
    case 3:
        return OS.QT_RIGHTBUTTON;
    default:
        return OS.QT_NOBUTTON;
    }
}

// Converts event types from SWT to Qt
static int unTranslateEventType(int type) {
    switch (type) {
    case SWT.KeyDown:
        return OS.QEVENT_KEYPRESS;
    case SWT.KeyUp:
        return OS.QEVENT_KEYRELEASE;
    case SWT.MouseDown:
        return OS.QEVENT_MOUSEBUTTONPRESS;
    case SWT.MouseMove:
        return OS.QEVENT_MOUSEMOVE;
    case SWT.MouseUp:
        return OS.QEVENT_MOUSEBUTTONRELEASE;
    case SWT.MouseDoubleClick:
        return OS.QEVENT_MOUSEBUTTONDBLCLICK;
    default:
        return 0;
    }
}

// Converts keys from SWT to Qt
static int untranslateKey (int key) {
    for (int i=0; i<KeyTable.length; i++) {
        if (KeyTable [i] [1] == key) return KeyTable [i] [0];
    }
    for (int i=0; i<KeyPadKeyTable.length; i++) {
        if (KeyPadKeyTable [i] [1] == key) return KeyPadKeyTable [i] [0];
    }
    return 0;
}

// Converts modifiers from SWT to Qt
static int unTranslateModifiers(int modifiers) {
    int nativeModifiers = 0;
    if ((modifiers & SWT.ALT) != 0) nativeModifiers |= OS.QT_ALTMODIFIER;
    if ((modifiers & SWT.SHIFT) != 0) nativeModifiers |= OS.QT_SHIFTMODIFIER;
    if ((modifiers & SWT.CONTROL) != 0) nativeModifiers |= OS.QT_CONTROLMODIFIER;
    return nativeModifiers;
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
public void update () {
    checkDevice ();
    if (OS.QCoreApplication_hasPendingEvents(handle)) {
        OS.QCoreApplication_processEvents(handle, OS.QEVENTLOOP_EXCLUDEUSERINPUTEVENTS);
    }
}

static void updateImages (Control[] controls) {
    for(int i = 0; i < controls.length; ++i) {
        controls[i].updateImages();
    }
}

static void updateLayoutDirection(Widget widget) {
    // If the widget doesn't follow system language direction then leave it
    if((widget.state & WidgetState.FOLLOWS_SYSLANG_DIRECTION) != 0) {
        // Update the style flags. We already know LEFT_TO_RIGHT flag is not 
        // set because if it was the widget wouldn't be following the system 
        // language direction. 
        if(defaultOrientation == SWT.RIGHT_TO_LEFT) {
            widget.style |= SWT.RIGHT_TO_LEFT;
        } else {
            widget.style &= ~SWT.RIGHT_TO_LEFT;
        }
        
        // This won't propagate to the children because we have explicitly 
        // called it to every widget's root-most handle. It only propagates
        // to the SWT widget's QWidgets but not to the QWidgets of the child 
        // SWT widgets. 
        OS.QWidget_setLayoutDirection(widget.topHandle, 
                defaultOrientation == SWT.RIGHT_TO_LEFT ? OS.QT_RIGHTTOLEFT : OS.QT_LEFTTORIGHT);
    }
}

static void updateLayoutDirections(Control[] controls) {
    if(!defaultOrientationIsSysLangDirection) return;
    
    // Update the default orientation
    defaultOrientation = translateDirection(OS.QApplication_layoutDirection());
    
    // Update the widgets that use the default orientation
    Menu[] menus = getMenus();
    for(int i = 0; i < menus.length; ++i) {
        updateLayoutDirection(menus[i]);
    }
    for(int i = 0; i < controls.length; ++i) {
        updateLayoutDirection(controls[i]);
        controls[i].checkMirrored();
    }
}

/**
 * If the receiver's user-interface thread was <code>sleep</code>ing,
 * causes it to be awakened and start running again. Note that this
 * method may be called from any thread.
 *
 * @exception SWTException <ul>
 *    <li>ERROR_DEVICE_DISPOSED - if the receiver has been disposed</li>
 * </ul>
 *
 * @see #sleep
 */
public void wake () {
    synchronized (Device.class) {
        if (isDisposed ()) error (SWT.ERROR_DEVICE_DISPOSED);
        if (thread == Thread.currentThread ()) return;
        wakeThread ();
    }
}

static void wakeThread () {
    int threadHandle = OS.QObject_thread( Display.handle );
    int dispatcher = OS.QAbstractEventDispatcher_instance( threadHandle );
    OS.QAbstractEventDispatcher_wakeup( dispatcher );
}

static int initializeMobileDevice(){
	if(mobileDeviceHandle == 0){
		mobileDeviceHandle = OS.MobileDevice_new();
	}
	return mobileDeviceHandle;
}
}
