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
package org.eclipse.swt.widgets;


import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.internal.SWTEventListener;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.PackageProxy;
import org.eclipse.swt.internal.qt.QObjectDeleteWrapper;
import org.eclipse.swt.internal.qt.WidgetConstant;
import org.eclipse.swt.internal.qt.WidgetState;

/**
 * This class is the abstract superclass of all user interface objects.
 * Widgets are created, disposed and issue notification to listeners
 * when events occur which affect them.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>(none)</dd>
 * <dt><b>Events:</b></dt>
 * <dd>Dispose</dd>
 * </dl>
 * <p>
 * IMPORTANT: This class is intended to be subclassed <em>only</em>
 * within the SWT implementation. However, it has not been marked
 * final to allow those outside of the SWT development team to implement
 * patched versions of the class in order to get around specific
 * limitations in advance of when those limitations can be addressed
 * by the team.  Any class built using subclassing to access the internals
 * of this class will likely fail to compile or run between releases and
 * may be strongly platform specific. Subclassing should not be attempted
 * without an intimate and detailed understanding of the workings of the
 * hierarchy. No support is provided for user-written classes which are
 * implemented as subclasses of this class.
 * </p>
 *
 * @see #checkSubclass
 */
public abstract class Widget {
    int handle, topHandle;
    
    int style, state;
    Display display;
    EventTable eventTable;
    Object data;
    PackageProxy packageProxy;
    
static final int checkBits (int style, int int0, int int1, int int2, int int3, int int4, int int5) {
    int mask = int0 | int1 | int2 | int3 | int4 | int5;
    if ((style & mask) == 0) style |= int0;
    if ((style & int0) != 0) style = (style & ~mask) | int0;
    if ((style & int1) != 0) style = (style & ~mask) | int1;
    if ((style & int2) != 0) style = (style & ~mask) | int2;
    if ((style & int3) != 0) style = (style & ~mask) | int3;
    if ((style & int4) != 0) style = (style & ~mask) | int4;
    if ((style & int5) != 0) style = (style & ~mask) | int5;
    return style;
}

/**
 * Prevents uninitialized instances from being created outside the package.
 */
Widget () {}

/**
 * Constructs a new instance of this class given its parent
 * and a style value describing its behavior and appearance.
 * <p>
 * The style value is either one of the style constants defined in
 * class <code>SWT</code> which is applicable to instances of this
 * class, or must be built by <em>bitwise OR</em>'ing together
 * (that is, using the <code>int</code> "|" operator) two or more
 * of those <code>SWT</code> style constants. The class description
 * lists the style constants that are applicable to the class.
 * Style bits are also inherited from superclasses.
 * </p>
 *
 * @param parent a widget which will be the parent of the new instance (cannot be null)
 * @param style the style of widget to construct
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
 *    <li>ERROR_INVALID_ARGUMENT - if the parent is disposed</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
 *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
 * </ul>
 *
 * @see SWT
 * @see #checkSubclass
 * @see #getStyle
 */
public Widget (Widget parent, int style) {
    this(parent, style, 0, null, false);
}

/**
 * <p>
 * <b>IMPORTANT:</b> This constructor is <em>not</em> part of the SWT
 * public API. It should never be referenced from application code. 
 * </p>
 */
protected Widget (Widget parent, int style, int extraStyle, Object packageProxy, 
        boolean isExtended) {
    checkSubclass ();
    checkParent (parent);
    this.style = style;
    if(isExtended) state |= WidgetState.IS_EXTENDED;
    display = parent.display;
    setPackageProxy(packageProxy);
}

/**
 * Adds the listener to the collection of listeners who will
 * be notified when the widget is disposed. When the widget is
 * disposed, the listener is notified by sending it the
 * <code>widgetDisposed()</code> message.
 *
 * @param listener the listener which should be notified when the receiver is disposed
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see DisposeListener
 * @see #removeDisposeListener
 */
public void addDisposeListener (DisposeListener listener) {
    checkWidget ();
    if (listener == null) error (SWT.ERROR_NULL_ARGUMENT);
    TypedListener typedListener = new TypedListener (listener);
    addListener (SWT.Dispose, typedListener);
}

/**
 * Adds the listener to the collection of listeners who will
 * be notified when an event of the given type occurs. When the
 * event does occur in the widget, the listener is notified by
 * sending it the <code>handleEvent()</code> message. The event
 * type is one of the event constants defined in class <code>SWT</code>.
 *
 * @param eventType the type of event to listen for
 * @param listener the listener which should be notified when the event occurs
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see Listener
 * @see SWT
 * @see #removeListener(int, Listener)
 * @see #notifyListeners
 */
public void addListener (int eventType, Listener listener) {
    checkWidget ();
    if (listener == null) error (SWT.ERROR_NULL_ARGUMENT);
    _addListener (eventType, listener);
}

void _addListener (int eventType, Listener listener) {
    if (eventTable == null) eventTable = new EventTable ();
    eventTable.hook (eventType, listener);
}

final void addWidget (int handle) {
    Display.addWidget(handle, this);
}

final void checkOrientation (Widget parent) {
    style &= ~SWT.MIRRORED;
    
    // If orientation explicitly specified then always use that
    if ((style & (SWT.LEFT_TO_RIGHT | SWT.RIGHT_TO_LEFT)) == 0) {
        if (parent != null) {
            // Inherit the flag from parent
            if ((parent.style & SWT.LEFT_TO_RIGHT) != 0) style |= SWT.LEFT_TO_RIGHT;
            if ((parent.style & SWT.RIGHT_TO_LEFT) != 0) style |= SWT.RIGHT_TO_LEFT;
            
            // If the parent follows the system language direction this widget
            // does too (unless the user explicitly specified the direction). 
            state |= (parent.state & WidgetState.FOLLOWS_SYSLANG_DIRECTION);
        } else {
            // Use the default determined by a system property. 
            // Setting RIGHT_TO_LEFT flag triggers the coordinate mirroring. 
            if(Display.defaultOrientation != SWT.LEFT_TO_RIGHT) {
                style |= Display.defaultOrientation;
            }
            
            // If there's no parent this widget follows the system language 
            // direction if that's the default behavior configured by a system 
            // property. 
            if(Display.defaultOrientationIsSysLangDirection)
                state |= WidgetState.FOLLOWS_SYSLANG_DIRECTION;
        }
    }
    style = checkBits (style, SWT.LEFT_TO_RIGHT, SWT.RIGHT_TO_LEFT, 0, 0, 0, 0);
}

/*
 * Throws an exception if the specified widget can not be
 * used as a parent for the receiver.
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
 *    <li>ERROR_INVALID_ARGUMENT - if the parent is disposed</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
 * </ul>
 */
final void checkParent (Widget parent) {
    if (parent == null) error (SWT.ERROR_NULL_ARGUMENT);
    if (parent.isDisposed ()) error (SWT.ERROR_INVALID_ARGUMENT);
    parent.checkWidget ();
}

/**
 * Checks that this class can be subclassed.
 * <p>
 * The SWT class library is intended to be subclassed
 * only at specific, controlled points (most notably,
 * <code>Composite</code> and <code>Canvas</code> when
 * implementing new widgets). This method enforces this
 * rule unless it is overridden.
 * </p><p>
 * <em>IMPORTANT:</em> By providing an implementation of this
 * method that allows a subclass of a class which does not
 * normally allow subclassing to be created, the implementer
 * agrees to be fully responsible for the fact that any such
 * subclass will likely fail between SWT releases and will be
 * strongly platform specific. No support is provided for
 * user-written classes which are implemented in this fashion.
 * </p><p>
 * The ability to subclass outside of the allowed SWT classes
 * is intended purely to enable those not on the SWT development
 * team to implement patches in order to get around specific
 * limitations in advance of when those limitations can be
 * addressed by the team. Subclassing should not be attempted
 * without an intimate and detailed understanding of the hierarchy.
 * </p>
 *
 * @exception SWTException <ul>
 *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
 * </ul>
 */
protected void checkSubclass () {
    if (!isValidSubclass ()) error (SWT.ERROR_INVALID_SUBCLASS);
}

/**
 * Throws an <code>SWTException</code> if the receiver can not
 * be accessed by the caller. This may include both checks on
 * the state of the receiver and more generally on the entire
 * execution context. This method <em>should</em> be called by
 * widget implementors to enforce the standard SWT invariants.
 * <p>
 * Currently, it is an error to invoke any method (other than
 * <code>isDisposed()</code>) on a widget that has had its
 * <code>dispose()</code> method called. It is also an error
 * to call widget methods from any thread that is different
 * from the thread that created the widget.
 * </p><p>
 * In future releases of SWT, there may be more or fewer error
 * checks and exceptions may be thrown for different reasons.
 * </p>
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
protected void checkWidget () {
    Display display = this.display;
    if (display == null) error (SWT.ERROR_WIDGET_DISPOSED);
    if (display.thread != Thread.currentThread ()) error (SWT.ERROR_THREAD_INVALID_ACCESS);
    if ((state & WidgetState.DISPOSED) != 0) error (SWT.ERROR_WIDGET_DISPOSED);
}

/*
 * _pp means that some widgets want to "override" this method in another 
 * package. If this is such a widget then there is a 'package proxy' (pp) that 
 * must be called instead of the implementation in this package. 
 */ 
void createHandle_pp (int index) {
}

void createWidget (int index) {
    if(packageProxy != null) {
        packageProxy.createHandle(index);
    } else {
        createHandle_pp (index);
    }
    // If subclasses didn't specify topHandle then it's assumed that there's
    // only one QWidget. I.e. handle is the root widget. 
    if(topHandle == 0) topHandle = handle;
    if(packageProxy != null) {
        packageProxy.hookEvents();
    } else {
        hookEvents_pp();
    }
    if(packageProxy != null) {
        packageProxy.register();
    } else {
        register_pp();
    }
}

void deregister_pp () {
    if (handle != 0) Display.removeWidget (handle);
}

void destroyWidget () {
    int _topHandle = topHandle;
    if(packageProxy != null) {
        packageProxy.releaseHandle();
    } else {
        releaseHandle_pp ();
    }
    if (_topHandle != 0 && (state & WidgetState.HANDLE) != 0) {
        QObjectDeleteWrapper.deleteSafely( _topHandle );
    }
}

/**
 * Disposes of the operating system resources associated with
 * the receiver and all its descendants. After this method has
 * been invoked, the receiver and all descendants will answer
 * <code>true</code> when sent the message <code>isDisposed()</code>.
 * Any internal connections between the widgets in the tree will
 * have been removed to facilitate garbage collection.
 * <p>
 * NOTE: This method is not called recursively on the descendants
 * of the receiver. This means that, widget implementers can not
 * detect when a widget is being disposed of by re-implementing
 * this method, but should instead listen for the <code>Dispose</code>
 * event.
 * </p>
 *
 * @exception SWTException <ul>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see #addDisposeListener
 * @see #removeDisposeListener
 * @see #checkWidget
 */
public void dispose () {
    if (isDisposed ()) return;
    if (!isValidThread ()) error (SWT.ERROR_THREAD_INVALID_ACCESS);
    release (true);
}

final void error (int code) {
    SWT.error (code);
}

final boolean filters (int eventType) {
    return Display.filters (eventType);
}

static final Command[] getCommands(Control control) {
    return control.getCommands();
}

/**
 * Returns the application defined widget data associated
 * with the receiver, or null if it has not been set. The
 * <em>widget data</em> is a single, unnamed field that is
 * stored with every widget.
 * <p>
 * Applications may put arbitrary objects in this field. If
 * the object stored in the widget data needs to be notified
 * when the widget is disposed of, it is the application's
 * responsibility to hook the Dispose event on the widget and
 * do so.
 * </p>
 *
 * @return the widget data
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - when the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - when called from the wrong thread</li>
 * </ul>
 *
 * @see #setData(Object)
 */
public Object getData () {
    checkWidget();
    return (state & WidgetState.KEYED_DATA) != 0 ? ((Object []) data) [0] : data;
}

/**
 * Returns the application defined property of the receiver
 * with the specified name, or null if it has not been set.
 * <p>
 * Applications may have associated arbitrary objects with the
 * receiver in this fashion. If the objects stored in the
 * properties need to be notified when the widget is disposed
 * of, it is the application's responsibility to hook the
 * Dispose event on the widget and do so.
 * </p>
 *
 * @param   key the name of the property
 * @return the value of the property or null if it has not been set
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the key is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see #setData(String, Object)
 */
public Object getData (String key) {
    checkWidget();
    if (key == null) error (SWT.ERROR_NULL_ARGUMENT);
    if ((state & WidgetState.KEYED_DATA) != 0) {
        Object [] table = (Object []) data;
        for (int i=1; i<table.length; i+=2) {
            if (key.equals (table [i])) return table [i+1];
        }
    }
    return null;
}

/**
 * Returns the <code>Display</code> that is associated with
 * the receiver.
 * <p>
 * A widget's display is either provided when it is created
 * (for example, top level <code>Shell</code>s) or is the
 * same as its parent's display.
 * </p>
 *
 * @return the receiver's display
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 * </ul>
 */
public Display getDisplay () {
    Display display = this.display;
    if (display == null) error (SWT.ERROR_WIDGET_DISPOSED);
    return display;
}

String getName () {
    String string = getClass ().getName ();
    int index = string.length ();
    while ((--index > 0) && (string.charAt (index) != '.')) {}
    return string.substring (index + 1, string.length ());
}

String getNameText () {
    return "";
}

String getPhoneNumber_pp() {
    if (packageProxy != null) {
        return packageProxy.getPhoneNumber();
    }
    return null;
}

/**
 * Returns the receiver's style information.
 * <p>
 * Note that the value which is returned by this method <em>may
 * not match</em> the value which was provided to the constructor
 * when the receiver was created. This can occur when the underlying
 * operating system does not support a particular combination of
 * requested styles. For example, if the platform widget used to
 * implement a particular SWT widget always has scroll bars, the
 * result of calling this method would always have the
 * <code>SWT.H_SCROLL</code> and <code>SWT.V_SCROLL</code> bits set.
 * </p>
 *
 * @return the style bits
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public int getStyle () {
    checkWidget ();
    return style;
}

// Experimental support for StyleSheets
public String getStyleSheet(){
    checkWidget();
    return OS.QWidget_styleSheet(handle);
}

void hookEvents_pp () {
}

/*
 * Returns <code>true</code> if the specified eventType is
 * hooked, and <code>false</code> otherwise. Implementations
 * of SWT can avoid creating objects and sending events
 * when an event happens in the operating system but
 * there are no listeners hooked for the event.
 *
 * @param eventType the event to be checked
 *
 * @return <code>true</code> when the eventType is hooked and <code>false</code> otherwise
 *
 * @see #isListening
 */
final boolean hooks (int eventType) {
    if (eventTable == null) return false;
    return eventTable.hooks (eventType);
}


boolean isInternalWidget_pp() {
    return (state & WidgetState.INTERNAL) != 0;
}

/**
 * Returns <code>true</code> if the widget has been disposed,
 * and <code>false</code> otherwise.
 * <p>
 * This method gets the dispose state for the widget.
 * When a widget has been disposed, it is an error to
 * invoke any other method using the widget.
 * </p>
 *
 * @return <code>true</code> when the widget is disposed and <code>false</code> otherwise
 */
public boolean isDisposed () {
    return (state & WidgetState.DISPOSED) != 0;
}

/**
 * Returns <code>true</code> if there are any listeners
 * for the specified event type associated with the receiver,
 * and <code>false</code> otherwise. The event type is one of
 * the event constants defined in class <code>SWT</code>.
 *
 * @param eventType the type of event
 * @return true if the event is hooked
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see SWT
 */
public boolean isListening (int eventType) {
    checkWidget ();
    return hooks (eventType);
}

/*
 * Checks if key or character code corresponds to selection key
 * @param key Key code
 * @param character Character code
 * @return true if key/character is a selection key, false otherwise
 */
final boolean isSelectionKey(int key, int character) {
    if(OS.windowServer == OS.WS_X11) {
        if (character == ' ') {
            return true;
        }
    } else if (OS.windowServer == OS.WS_SYMBIAN_S60) {
        if (key == OS.QT_KEY_RETURN || character == -5) {
            return true;
        }
    }

    return false;
}

final boolean isValidSubclass() {
    return Display.isValidClass(getClass());
}

final boolean isValidThread () {
    return getDisplay ().isValidThread ();
}

final Event makeKeyEvent(int key, int modifier, int character, int nativeScanCode) {
    Event event = new Event ();
    switch (key) {
    case OS.QT_KEY_CONTROL: //intentional fall through
    case OS.QT_KEY_ALT:
    case OS.QT_KEY_META:
    case OS.QT_KEY_UP:
    case OS.QT_KEY_DOWN:
    case OS.QT_KEY_LEFT:
    case OS.QT_KEY_RIGHT:
        event.character = 0;
        break;
    case OS.QT_KEY_ENTER : //intentional fall through
    case OS.QT_KEY_RETURN :     event.character = SWT.CR; break;
    case OS.QT_KEY_BACKSPACE:   event.character = SWT.BS; break;
    case OS.QT_KEY_DELETE:      event.character = SWT.DEL; break;
    case OS.QT_KEY_ESCAPE:      event.character = SWT.ESC; break;
    case OS.QT_KEY_TAB:         event.character = SWT.TAB; break;
    case OS.QT_KEY_SHIFT:       event.character = (char)SWT.SHIFT; break;
    default:
        event.character = (char)character; break;
    }

    boolean keypad = (modifier & OS.QT_KEYPADMODIFIER) != 0 ;
    event.keyCode = Display.translateKey( key, keypad );
    if ( event.keyCode == 0 ){// keyCode defaults to unicode value of character.
        event.keyCode = character;
    }
    if( (modifier & OS.QT_NOMODIFIER) == 0  ){
        event.stateMask = Display.translateModifiers(modifier);
    }
    if (keypad) {
        event.keyLocation = SWT.KEYPAD;
    }
    
    event.keyLocation |= Display.translateKeyLocation(nativeScanCode);

    return event;
}

/**
 * Notifies all of the receiver's listeners for events
 * of the given type that one such event has occurred by
 * invoking their <code>handleEvent()</code> method.  The
 * event type is one of the event constants defined in class
 * <code>SWT</code>.
 *
 * @param eventType the type of event which has occurred
 * @param event the event data
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see SWT
 * @see #addListener
 * @see #removeListener(int, Listener)
 */
public void notifyListeners (int eventType, Event event) {
    checkWidget();
    if (event == null) event = new Event ();
    sendEvent (eventType, event);
}

final void postEvent (int eventType) {
    sendEvent (eventType, null, false);
}

final void postEvent (int eventType, Event event) {
    sendEvent (eventType, event, false);
}

/*
 *  Qt Events
 */
boolean qt_event_close() {
    return false;
}

boolean qt_event_contextmenu(int x, int y, int reason) {
    return false;
}

boolean qt_event_focusIn( int reason ) {
    return false;
}

boolean qt_event_focusOut( int reason ) {
    return false;
}

boolean qt_event_keypress_pp( int widgetHandle, int key, int modifier, int character, int nativeScanCode ) {
    if (((state & WidgetState.NO_KEY_PROPAGATE) != 0) && (widgetHandle != handle)) {
        return true;
    }
    return sendKeyEvent( SWT.KeyDown, key, modifier, character, nativeScanCode );
}

boolean qt_event_keyrelease_pp( int widgetHandle, int key, int modifier, int character, int nativeScanCode ) {
    if (((state & WidgetState.NO_KEY_PROPAGATE) != 0) && (widgetHandle != handle)) {
        return true;
    }
    return sendKeyEvent( SWT.KeyUp, key, modifier, character, nativeScanCode );
}

boolean qt_event_mouseButtonDblClick_pp( int widgetHandle, int button, int x, int y, int state, int buttons ) {
    return false;
}

boolean qt_event_mouseButtonPress_pp (int widgetHandle, int button, int x, int y, int state, int buttons) {
    return false;
}

boolean qt_event_mouseButtonRelease_pp ( int widgetHandle, int button, int x, int y, int state, int buttons  ) {
    return false;
}

boolean qt_event_mouseMove ( int widgetHandle, int button, int x, int y, int state, int buttons  ) {
    return false;
}

boolean qt_event_paint( int widgetHandle, int x, int y, int width, int height, int regionHandle ) {
    return false;
}

boolean qt_event_shortcut(int key, int modifier, int character) {
    return false;
}

boolean qt_event_windowActivate(int widgetHandle) {
    return false;
}

boolean qt_event_windowDeactivate(int widgetHandle) {
    return false;
}

boolean qt_event_windowStateChange(int widgetHandle, int oldState) {
    return false;
}

/*
 *  Qt Signals
 */
void qt_signal_aboutToHide() {
}

void qt_signal_aboutToShow() {
}

void qt_signal_clicked() {
}

void qt_signal_qapplication_focusChanged_pp(int old, int now) {
}

void qt_signal_current_tab_changed_pp(int index) {
}

void qt_signal_dateedit_dateTimeChanged_pp() {
}

void qt_signal_hover() {
}

void qt_signal_linkActivated_pp(String link) {
}

void qt_signal_list_itemActivated_pp(int selectionHandle) {
}

void qt_signal_list_itemDoubleClicked_pp(int selectionHandle) {
}

void qt_signal_list_itemSelectionChanged_pp() {
}

void qt_signal_pressed() {
}

void qt_signal_released() {
}

void qt_signal_requestCompleted_pp(int value){
}

void qt_signal_return_pressed_pp() {
}

void qt_signal_selectionChanged_pp(int selectionHandle, int deSelectionHandle) {
}

void qt_signal_slider_action_triggered(int action) {
}

void qt_signal_slider_changed(int value) {
}

void qt_signal_slider_released(){
}

void qt_signal_table_cellActivated(int row, int column) {
}

void qt_signal_table_cellChanged(int row, int column) {
}

void qt_signal_table_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn) {
}

void qt_signal_table_itemSelectionChanged() {
}

void qt_signal_text_changed_pp(String text) {
}

void qt_signal_text_cursorPositionChanged_pp(int oldPos, int newPos) {
}

void qt_signal_text_selectionChanged_pp() {
}

void qt_signal_tray_activated(int arg1) {
}

void qt_signal_tree_itemActivated(int selectionHandle, int column) {
}

void qt_signal_tree_itemCollapsed(int selectionHandle) {
}

void qt_signal_tree_itemExpanded(int selectionHandle) {
}

void qt_signal_tree_itemSelectionChanged() {
}

void qt_signal_triggered_pp(boolean checked) {
}

void qt_signal_abstractitemview_activated_pp(int row, int column) {
}

/*
 * QtSwt Events
 */
void qt_swt_event_bufferFlush() {
}

void qt_swt_event_focusWasGained() {
}

void qt_swt_event_focusWasLost() {
}

void qt_swt_event_widgetMoved(int widgetHandle) {
}

void qt_swt_event_widgetPainted(int widgetHandle, int x, int y, int width, int height, int regionHandle) {
}

void qt_swt_event_widgetResized_pp(int widgetHandle, int oldWidth, int oldHeight, int width, int height) {
}

void register_pp () {
    if (handle == 0) return;
    if ((state & WidgetState.HANDLE) != 0) Display.addWidget (handle, this);
}

void release (boolean destroy) {
    if ((state & WidgetState.DISPOSE_SENT) == 0) {
        state |= WidgetState.DISPOSE_SENT;
        sendEvent (SWT.Dispose);
    }
    if ((state & WidgetState.DISPOSED) == 0) {
        if(packageProxy != null) {
            packageProxy.releaseChildren(destroy);
        } else {
            releaseChildren_pp(destroy);
        }
    }
    if ((state & WidgetState.RELEASED) == 0) {
        state |= WidgetState.RELEASED;
        if (destroy) {
            if(packageProxy != null) {
                packageProxy.releaseParent();
                packageProxy.releaseWidget();
            } else {
                releaseParent_pp();
                releaseWidget_pp();
            }
            destroyWidget ();
        } else {
            if(packageProxy != null) {
                packageProxy.releaseWidget ();
                packageProxy.releaseHandle();
            } else {
                releaseWidget_pp ();
                releaseHandle_pp ();
            }
        }
    }
}

void releaseChildren_pp (boolean destroy) {
}

void releaseHandle_pp () {
    handle = topHandle = 0;
    state |= WidgetState.DISPOSED;
    display = null;
}

void releaseParent_pp () {
    /* Do nothing */
}

void releaseWidget_pp () {
    if(packageProxy != null) {
        packageProxy.deregister();
    } else {
        deregister_pp();
    }
    eventTable = null;
    data = null;
}

/**
 * Removes the listener from the collection of listeners who will
 * be notified when the widget is disposed.
 *
 * @param listener the listener which should no longer be notified when the receiver is disposed
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see DisposeListener
 * @see #addDisposeListener
 */
public void removeDisposeListener (DisposeListener listener) {
    checkWidget ();
    if (listener == null) error (SWT.ERROR_NULL_ARGUMENT);
    if (eventTable == null) return;
    eventTable.unhook (SWT.Dispose, listener);
}

/**
 * Removes the listener from the collection of listeners who will
 * be notified when an event of the given type occurs. The event
 * type is one of the event constants defined in class <code>SWT</code>.
 *
 * @param eventType the type of event to listen for
 * @param listener the listener which should no longer be notified when the event occurs
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see Listener
 * @see SWT
 * @see #addListener
 * @see #notifyListeners
 */
public void removeListener (int eventType, Listener handler) {
    checkWidget ();
    if (handler == null) error (SWT.ERROR_NULL_ARGUMENT);
    if (eventTable == null) return;
    eventTable.unhook (eventType, handler);
}

/**
 * Removes the listener from the collection of listeners who will
 * be notified when an event of the given type occurs.
 * <p>
 * <b>IMPORTANT:</b> This method is <em>not</em> part of the SWT
 * public API. It is marked public only so that it can be shared
 * within the packages provided by SWT. It should never be
 * referenced from application code.
 * </p>
 *
 * @param eventType the type of event to listen for
 * @param listener the listener which should no longer be notified when the event occurs
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see Listener
 * @see #addListener
 */
protected void removeListener (int eventType, SWTEventListener handler) {
    checkWidget ();
    if (handler == null) error (SWT.ERROR_NULL_ARGUMENT);
    if (eventTable == null) return;
    eventTable.unhook (eventType, handler);
}

final Widget removeWidget (int handle) {
    return Display.removeWidget(handle);
}

final void sendEvent (Event event) {
    Display display = event.display;
    if (!Display.filterEvent (event)) {
        if (eventTable != null) eventTable.sendEvent (event);
    }
}

final void sendEvent (int eventType) {
    sendEvent (eventType, null, true);
}

final void sendEvent (int eventType, Event event) {
    sendEvent (eventType, event, true);
}

final void sendEvent (int eventType, Event event, boolean send) {
    if (eventTable == null && !Display.filters (eventType)) {
        return;
    }
    if (event == null) event = new Event ();
    event.type = eventType;
    event.display = display;
    event.widget = this;
    if (event.time == 0) {
        event.time = Display.getLastEventTime ();
    }
    if (send) {
        sendEvent (event);
    } else {
        Display.postEvent (event);
    }
}

final boolean sendKeyEvent ( int type, int key, int modifier, int character, int nativeScanCode ) {
    Event event = makeKeyEvent(key, modifier, character, nativeScanCode);
    sendEvent (type, event);
    if (isDisposed () ) return true;
    return !event.doit;
}

/**
 * Sets the application defined widget data associated
 * with the receiver to be the argument. The <em>widget
 * data</em> is a single, unnamed field that is stored
 * with every widget.
 * <p>
 * Applications may put arbitrary objects in this field. If
 * the object stored in the widget data needs to be notified
 * when the widget is disposed of, it is the application's
 * responsibility to hook the Dispose event on the widget and
 * do so.
 * </p>
 *
 * @param data the widget data
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - when the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - when called from the wrong thread</li>
 * </ul>
 *
 * @see #getData()
 */
public void setData (Object data) {
    checkWidget();
    if ((state & WidgetState.KEYED_DATA) != 0) {
        ((Object []) this.data) [0] = data;
    } else {
        this.data = data;
    }
}

/**
 * Sets the application defined property of the receiver
 * with the specified name to the given value.
 * <p>
 * Applications may associate arbitrary objects with the
 * receiver in this fashion. If the objects stored in the
 * properties need to be notified when the widget is disposed
 * of, it is the application's responsibility to hook the
 * Dispose event on the widget and do so.
 * </p>
 *
 * @param key the name of the property
 * @param value the new value for the property
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the key is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see #getData(String)
 */
public void setData (String key, Object value) {
    checkWidget();
    if (key == null) error (SWT.ERROR_NULL_ARGUMENT);
    
    if (key.equals (WidgetConstant.SET_CANVAS_STATE_KEY)) {
        boolean enabled = ((Boolean)value).booleanValue();
        if (enabled) {
            this.state |= WidgetState.CANVAS;
        }
        else {
            this.state &= ~WidgetState.CANVAS;
        }
        return;
    }

    if (key.equals (WidgetConstant.SET_NO_TRAVERSE_STATE_KEY)) {
        boolean enabled = ((Boolean)value).booleanValue();
        if (enabled) {
            this.state |= WidgetState.NO_TRAVERSE;
        }
        else {
            this.state &= ~WidgetState.NO_TRAVERSE;
        }
        return;
    }

    if (key.equals (WidgetConstant.SET_NO_KEY_PROPAGATE_STATE_KEY)) {
        boolean enabled = ((Boolean)value).booleanValue();
        if (enabled) {
            this.state |= WidgetState.NO_KEY_PROPAGATE;
        }
        else {
            this.state &= ~WidgetState.NO_KEY_PROPAGATE;
        }
        return;
    }

    if (key.equals (WidgetConstant.SET_NO_MOUSE_PROPAGATE_STATE_KEY)) {
        boolean enabled = ((Boolean)value).booleanValue();
        if (enabled) {
            this.state |= WidgetState.NO_MOUSE_PROPAGATE;
        }
        else {
            this.state &= ~WidgetState.NO_MOUSE_PROPAGATE;
        }
        return;
    }

    if (key.equals (WidgetConstant.SET_EMBEDDED_SCROLLBARS_STATE_KEY)) {
        boolean enabled = ((Boolean)value).booleanValue();
        if (enabled) {
            this.state |= WidgetState.EMBEDDED_SCROLLBARS;
        }
        else {
            this.state &= ~WidgetState.EMBEDDED_SCROLLBARS;
        }
        return;
    }

    if(key.equals(WidgetConstant.CSS_ID)){
    	OS.QObject_setObjectName(handle,(String)value);
    	// Do not return here and let Widget store the key
    	// value.
    }
    
    int index = 1;
    Object [] table = null;
    if ((state & WidgetState.KEYED_DATA) != 0) {
        table = (Object []) data;
        while (index < table.length) {
            if (key.equals (table [index])) break;
            index += 2;
        }
    }
    if (value != null) {
        if ((state & WidgetState.KEYED_DATA) != 0) {
            if (index == table.length) {
                Object [] newTable = new Object [table.length + 2];
                System.arraycopy (table, 0, newTable, 0, table.length);
                data = table = newTable;
            }
        } else {
            table = new Object [3];
            table [0] = data;
            data = table;
            state |= WidgetState.KEYED_DATA;
        }
        table [index] = key;
        table [index + 1] = value;
    } else {
        if ((state & WidgetState.KEYED_DATA) != 0) {
            if (index != table.length) {
                int length = table.length - 2;
                if (length == 1) {
                    data = table [0];
                    state &= ~WidgetState.KEYED_DATA;
                } else {
                    Object [] newTable = new Object [length];
                    System.arraycopy (table, 0, newTable, 0, index);
                    System.arraycopy (table, index + 2, newTable, index, length - index);
                    data = newTable;
                }
            }
        }
    }
}

void setWidgetInternalOnly_pp(){
    state|=WidgetState.INTERNAL;
}

final void setPackageProxy(Object packageProxy) {
    // If the subclass is in another package then it provides a proxy object 
    // which implements the methods that "override" the package private
    // implementations from this package. We must call the proxy object
    // implementations if they are provided. 
    if(packageProxy != null) {
        PackageProxy proxyObject = (PackageProxy)packageProxy; 
        proxyObject.setWidget(this);
        this.packageProxy = proxyObject;
    }
}

/**
 * Returns a string containing a concise, human-readable
 * description of the receiver.
 *
 * @return a string representation of the receiver
 */
public String toString () {
    String string = "*Disposed*";
    if (!isDisposed ()) {
        string = "*Wrong Thread*";
        if (isValidThread ()) string = getNameText ();
    }
    return getName () + " {" + string + "}";
}

}
