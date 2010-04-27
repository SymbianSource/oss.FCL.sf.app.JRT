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

import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.internal.*;
import org.eclipse.swt.*;
import org.eclipse.swt.events.*;

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

public abstract class Widget
{
    int style = 0;
    int layoutStyle = 0;
    int state;
    Display display;
    EventTable eventTable;
    Object data;

    /* Global state flags */
    static final int DISPOSED       = 1<<0;
    static final int CANVAS         = 1<<1;
    static final int KEYED_DATA     = 1<<2;
    static final int DISABLED       = 1<<3;
    static final int HIDDEN         = 1<<4;

    static final int GRAB                = 1 << 5;
    static final int MOVED            = 1 << 6;
    static final int RESIZED          = 1 << 7;
    static final int EXPANDING        = 1 << 8;
    static final int LAYOUT_NEEDED    = 1 << 9;
    static final int LAYOUT_CHANGED   = 1 << 10;
    static final int IGNORE_WHEEL     = 1 << 11;

    /* Default widths for widgets */
    static final int DEFAULT_WIDTH  = 64;
    static final int DEFAULT_HEIGHT = 64;

    /**
     * Prevents uninitialized instances from being created outside the package.
     */
    Widget()
    {
    }

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
    public Widget(Widget parent, int style)
    {
        this(parent, style, 0);
    }

    protected Widget(Widget parent, int style, int layoutStyle)
    {
        checkSubclass();
        checkParent(parent);
        this.style = style;
        this.layoutStyle = layoutStyle;
        display = parent.display;
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notifed when an event of the given type occurs. When the
     * event does occur in the widget, the listener is notified by
     * sending it the <code>handleEvent()</code> message.
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
     * @see #removeListener
     */
    public void addListener(int eventType, Listener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) eventTable = new EventTable();
        eventTable.hook(eventType, listener);
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notifed when the widget is disposed. When the widget is
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
    public void addDisposeListener(DisposeListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Dispose, typedListener);
    }

    /**
     * Returns a style with exactly one style bit set out of
     * the specified set of exclusive style bits. All other
     * possible bits are cleared when the first matching bit
     * is found. Bits that are not part of the possible set
     * are untouched.
     *
     * @param style the original style bits
     * @param int0 the 0th possible style bit
     * @param int1 the 1st possible style bit
     * @param int2 the 2nd possible style bit
     * @param int3 the 3rd possible style bit
     * @param int4 the 4th possible style bit
     * @param int5 the 5th possible style bit
     *
     * @return the new style bits
     */
    protected static int internal_checkBits(int style, int int0, int int1, int int2, int int3, int int4, int int5)
    {
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

    void checkOrientation(Widget parent)
    {
        style &= ~SWT.MIRRORED;
        if ((style & (SWT.LEFT_TO_RIGHT | SWT.RIGHT_TO_LEFT)) == 0)
        {
            if (parent != null)
            {
                if ((parent.style & SWT.LEFT_TO_RIGHT) != 0) style |= SWT.LEFT_TO_RIGHT;
                if ((parent.style & SWT.RIGHT_TO_LEFT) != 0) style |= SWT.RIGHT_TO_LEFT;
            }
        }
        style = internal_checkBits(style, SWT.LEFT_TO_RIGHT, SWT.RIGHT_TO_LEFT, 0, 0, 0, 0);
    }

    /**
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
    void checkParent(Widget parent)
    {
        if (parent == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (parent.isDisposed()) internal_error(SWT.ERROR_INVALID_ARGUMENT);
        internal_checkParentClass(parent);
        parent.checkWidget();
    }

    /**
     * Throws an exception if the parent class is not valid (widget specific).
     *
     * @param parent The parent of the control.
     */
    protected void internal_checkParentClass(Widget parent)
    {
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
    protected void checkSubclass()
    {
        if (!internal_isValidSubclass()) internal_error(SWT.ERROR_INVALID_SUBCLASS);
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
    protected void checkWidget()
    {
        Display display = this.display;
        if (display == null) internal_error(SWT.ERROR_WIDGET_DISPOSED);
        if (display.thread != Thread.currentThread()) internal_error(SWT.ERROR_THREAD_INVALID_ACCESS);
        if ((state & DISPOSED) != 0) internal_error(SWT.ERROR_WIDGET_DISPOSED);
    }

    /**
     * Destroys the widget in the operating system and releases
     * the widget's handle.  If the widget does not have a handle,
     * this method may hide the widget, mark the widget as destroyed
     * or do nothing, depending on the widget.
     * <p>
     * This method is called after <code>internal_releaseResources</code>.
     * </p>
     * @see #dispose
     * @see #internal_prepareForRelease
     * @see #internal_releaseHandle
     */
    protected void internal_releaseHandle()
    {
        state |= DISPOSED;
        display = null;
    }

    /**
     * Disposes of the operating system resources associated with
     * the receiver and all its descendents. After this method has
     * been invoked, the receiver and all descendents will answer
     * <code>true</code> when sent the message <code>isDisposed()</code>.
     * Any internal connections between the widgets in the tree will
     * have been removed to facilitate garbage collection.
     * <p>
     * NOTE: This method is not called recursively on the descendents
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
    public void dispose()
    {
        /*
        * Note:  It is valid to attempt to dispose a widget
        * more than once.  If this happens, fail silently.
        */
        if (isDisposed()) return;
        if (!isValidThread()) internal_error(SWT.ERROR_THREAD_INVALID_ACCESS);
        internal_prepareForRelease();
        internal_releaseResources();
        internal_releaseHandle();
    }

    /**
     * Does whatever widget specific cleanup is required, and then
     * uses the code in <code>SWTError.error</code> to handle the error.
     *
     * @param code the descriptive error code
     *
     * @see SWTError#error
     */
    protected void internal_error(int code)
    {
        SWT.error(code);
    }

    /**
     * Due to common code (class Item) this method is added which
     * acts as internal_error. Note that its signature is different.
     * @param code the descriptive error code
     */
    void error(int code)
    {
        SWT.error(code);
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
     * @see #setData
     */
    public Object getData()
    {
        checkWidget();
        return (state & KEYED_DATA) != 0 ? ((Object []) data)[0] : data;
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
     * @see #setData
     */
    public Object getData(String key)
    {
        checkWidget();
        if (key == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if ((state & KEYED_DATA) != 0)
        {
            Object [] table = (Object []) data;
            for (int i=1; i<table.length; i+=2)
            {
                if (key.equals(table [i])) return table [i+1];
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
    public Display getDisplay()
    {
        Display display = this.display;
        if (display == null) internal_error(SWT.ERROR_WIDGET_DISPOSED);
        if ((state & DISPOSED) != 0) internal_error(SWT.ERROR_WIDGET_DISPOSED);
        return this.display;
    }

    /**
     * Returns the name of the widget. This is the name of
     * the class without the package name.
     *
     * @return the name of the widget
     */
    String getName()
    {
        String string = getClass().getName();
        int index = string.lastIndexOf('.');
        if (index == -1) return string;
        return string.substring(index + 1, string.length());
    }

    /*
     * Returns a short printable representation for the contents
     * of a widget. For example, a button may answer the label
     * text. This is used by <code>toString</code> to provide a
     * more meaningful description of the widget.
     *
     * @return the contents string for the widget
     *
     * @see #toString
     */
    String getNameText()
    {
        return ""; //$NON-NLS-1$
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
    public int getStyle()
    {
        checkWidget();
        return style;
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
    boolean hooks(int eventType)
    {
        if (eventTable == null) return false;
        return eventTable.hooks(eventType);
    }

    /**
     * Returns <code>true</code> if there are any listeners
     * for the specified event type associated with the receiver,
     * and <code>false</code> otherwise.
     *
     * @param   eventType the type of event
     * @return true if the event is hooked
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    protected boolean isListening(int eventType)
    {
        checkWidget();
        return hooks(eventType);
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
    public boolean isDisposed()
    {
        return (state & DISPOSED) != 0;
    }

    /*
     * Returns <code>true</code> when subclassing is
     * allowed and <code>false</code> otherwise
     *
     * @return <code>true</code> when subclassing is allowed and <code>false</code> otherwise
     */
    protected final boolean internal_isValidSubclass()
    {
        return Display.isValidClass(getClass());
    }

    /*
     * Returns <code>true</code> when the current thread is
     * the thread that created the widget and <code>false</code>
     * otherwise.
     *
     * @return <code>true</code> when the current thread is the thread that created the widget and <code>false</code> otherwise
     */
    boolean isValidThread()
    {
        return getDisplay().isValidThread();
    }

    /**
     * Notifies all of the receiver's listeners for events
     * of the given type that one such event has occurred by
     * invoking their <code>handleEvent()</code> method.
     *
     * @param eventType the type of event which has occurred
     * @param event the event data
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void notifyListeners(int eventType, Event event)
    {
        checkWidget();
        if (event == null) event = new Event();
        internal_sendEvent(eventType, event);
    }

    /*
     * Releases the receiver, a child in a widget hierarchy,
     * from its parent.
     * <p>
     * When a widget is destroyed, it may be necessary to remove
     * it from an internal data structure of the parent. When
     * a widget has no handle, it may also be necessary for the
     * parent to hide the widget or otherwise indicate that the
     * widget has been disposed. For example, disposing a menu
     * bar requires that the menu bar first be released from the
     * shell when the menu bar is active.  This could not be done
     * in <code>internat_releaseHandle</code> for the menu bar
     * because the parent shell as well as other fields have been
     * null'd out already by <code>internal_releaseResources</code>.
     * </p>
     * This method is called first when a widget is disposed.
     *
     * @see #dispose
     * @see #internal_releaseResources
     * @see #internal_releaseHandle
     */
    protected void internal_prepareForRelease()
    {
    }

    protected final void internal_release()
    {
        internal_releaseResources();
        internal_releaseHandle();
    }

    /*
     * Releases any internal resources back to the operating
     * system and clears all fields except the widget handle.
     * <p>
     * When a widget is destroyed, resources that were acquired
     * on behalf of the programmer need to be returned to the
     * operating system.  For example, if the widget made a
     * copy of an icon, supplied by the programmer, this copy
     * would be freed in <code>internal_releaseResources</code>.
     * Also, to assist the garbage collector and minimize the amount
     * of memory that is not reclaimed when the programmer keeps
     * a reference to a disposed widget, all fields except the
     * handle are zero'd.  The handle is needed by <code>internal_releaseHandle</code>.
     * </p>
     * <p>
     * Typically, a widget with children will broadcast this
     * message to all children so that they too can release their
     * resources.  </p>
     * This method is called after <code>internal_prepareForRelease</code>.
     *
     * @see #dispose
     * @see #internal_prepareForRelease
     * @see #internal_releaseHandle
     */
    protected void internal_releaseResources()
    {
        internal_sendEvent(SWT.Dispose);
        eventTable = null;
        data = null;
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
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Listener
     * @see #addListener
     */
    public void removeListener(int eventType, Listener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook(eventType, listener);
    }

    /**
     * Removes the listener from the collection of listeners who will
     * be notifed when an event of the given type occurs.
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
    protected void removeListener(int eventType, SWTEventListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook(eventType, listener);
    }

    /**
     * Removes the listener from the collection of listeners who will
     * be notifed when the widget is disposed.
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
    public void removeDisposeListener(DisposeListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook(SWT.Dispose, listener);
    }

    void sendEvent(Event event)
    {
        Display display = event.display;
        if (!display.filterEvent(event))
        {
            if (eventTable != null) eventTable.sendEvent(event);
        }
    }

    protected void internal_sendEvent(int eventType)
    {
        internal_sendEvent(eventType, (Event)null);
    }

    protected void internal_sendEvent(int eventType, String text)
    {
        if (eventTable == null && !display.filters(eventType))
        {
            return;
        }
        Event event = new Event();
        event.display = display;
        event.widget  = this;
        event.type    = eventType;
        event.text    = text;
        if (event.time == 0)
        {
            event.time = display.getLastEventTime();
        }
        sendEvent(event);
    }

    protected void internal_sendEvent(int eventType, int eventDetail, Widget item)
    {
        if (eventTable == null && !display.filters(eventType))
        {
            return;
        }
        Event event = new Event();
        event.display = display;
        event.widget  = this;
        event.type    = eventType;
        event.detail  = eventDetail;
        event.item    = item;
        if (event.time == 0)
        {
            event.time = display.getLastEventTime();
        }
        sendEvent(event);
    }

    protected void internal_sendEvent(int eventType, Event event)
    {
        if (eventTable == null && !display.filters(eventType))
        {
            return;
        }
        if (event == null) event = new Event();
        event.display = display;
        event.widget  = this;
        event.type    = eventType;
        if (event.time == 0)
        {
            event.time = display.getLastEventTime();
        }
        sendEvent(event);
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
     */
    public void setData(Object data)
    {
        checkWidget();
        if ((state & KEYED_DATA) != 0)
        {
            ((Object []) this.data)[0] = data;
        }
        else
        {
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
     * @see #getData
     */
    public void setData(String key, Object value)
    {
        checkWidget();
        if (key == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        int index = 1;
        Object [] table = null;
        if ((state & KEYED_DATA) != 0)
        {
            table = (Object []) data;
            while (index < table.length)
            {
                if (key.equals(table [index])) break;
                index += 2;
            }
        }
        if (value != null)
        {
            if ((state & KEYED_DATA) != 0)
            {
                if (index == table.length)
                {
                    Object [] newTable = new Object [table.length + 2];
                    System.arraycopy(table, 0, newTable, 0, table.length);
                    data = table = newTable;
                }
            }
            else
            {
                table = new Object [3];
                table [0] = data;
                data = table;
                state |= KEYED_DATA;
            }
            table [index] = key;
            table [index + 1] = value;
        }
        else
        {
            if ((state & KEYED_DATA) != 0)
            {
                if (index != table.length)
                {
                    int length = table.length - 2;
                    if (length == 1)
                    {
                        data = table [0];
                        state &= ~KEYED_DATA;
                    }
                    else
                    {
                        Object [] newTable = new Object [length];
                        System.arraycopy(table, 0, newTable, 0, index);
                        System.arraycopy(table, index + 2, newTable, index, length - index);
                        data = newTable;
                    }
                }
            }
        }
    }

    /**
     * Returns a string containing a concise, human-readable
     * description of the receiver.
     *
     * @return a string representation of the receiver
     */
    public String toString()
    {
        String string = "*Disposed*"; //$NON-NLS-1$
        if (!isDisposed())
        {
            string = "*Wrong Thread*"; //$NON-NLS-1$
            if (isValidThread()) string = getNameText();
        }
        return getName() + " {" + string + "}";  //$NON-NLS-1$ //$NON-NLS-2$
    }
    /*
     * This methos is a simple accessor. Note it does not call checkWidget().
     */
    protected int internal_getStyle()
    {
        return style;
    }
    protected void internal_setStyle(int newStyle)
    {
        this.style = newStyle;
    }
    protected int internal_getLayoutStyle()
    {
        return layoutStyle;
    }
    protected void internal_setLayoutStyle(int newStyle)
    {
        this.layoutStyle = newStyle;
    }
    protected Color internal_createColor(int rgb)
    {
        int red   = rgb & 0xFF;
        int green = (rgb >>  8) & 0xFF;
        int blue  = (rgb >> 16) & 0xFF;
        return new Color(display, red, green, blue);
    }



}
