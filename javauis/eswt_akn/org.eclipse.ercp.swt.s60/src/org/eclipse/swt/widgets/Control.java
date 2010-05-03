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

import org.eclipse.swt.internal.symbian.*;
import org.eclipse.swt.graphics.*;
import org.eclipse.swt.*;
import org.eclipse.swt.events.*;

/**
 * Control is the abstract superclass of all windowed user interface classes.
 * <p>
 * <dl>
 * <dt><b>Styles:</b>
 * <dd>BORDER</dd>
 * <dd>LEFT_TO_RIGHT, RIGHT_TO_LEFT</dd>
 * <dt><b>Events:</b>
 * <dd>FocusIn, FocusOut, KeyDown, KeyUp, MouseDoubleClick, MouseDown,
 *     MouseUp, MouseMove, Move, Paint, Resize, Traverse</dd>
 * </dl>
 * <p>
 * Only one of LEFT_TO_RIGHT or RIGHT_TO_LEFT may be specified.
 * </p><p>
 * IMPORTANT: This class is intended to be subclassed <em>only</em>
 * within the SWT implementation.
 * </p>
 */

public abstract class Control extends Widget implements Drawable
{
    /**
     * the handle to the OS resource
     * (Warning: This field is platform dependent)
     */
    public int handle;
    Composite parent;
    Menu menu;
    String toolTipText;
    Object layoutData;
    private MobileCommands commandChildren;

    boolean visible = true;
    Color background;
    Color foreground;
    GC gc = null;
    GCBuffer buffer = null;

    /**
     * Prevents uninitialized instances from being created outside the package.
     */
    Control()
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
     * @param parent a composite control which will be the parent of the new instance (cannot be null)
     * @param style the style of control to construct
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     *
     * @see SWT#BORDER
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public Control(Composite parent, int style)
    {
        this(parent, style, 0);
    }

    protected Control(Composite parent, int style, int layoutStyle)
    {
        super(parent, style, layoutStyle);
        this.parent = parent;
        internal_createWidget();
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notified when the control is moved or resized, by sending
     * it one of the messages defined in the <code>ControlListener</code>
     * interface.
     *
     * @param listener the listener which should be notified
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see ControlListener
     * @see #removeControlListener
     */
    public void addControlListener(ControlListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Resize,typedListener);
        addListener(SWT.Move,typedListener);
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notified when the control gains or loses focus, by sending
     * it one of the messages defined in the <code>FocusListener</code>
     * interface.
     *
     * @param listener the listener which should be notified
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see FocusListener
     * @see #removeFocusListener
     */
    public void addFocusListener(FocusListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.FocusIn,typedListener);
        addListener(SWT.FocusOut,typedListener);
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notified when keys are pressed and released on the system keyboard, by sending
     * it one of the messages defined in the <code>KeyListener</code>
     * interface.
     *
     * @param listener the listener which should be notified
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see KeyListener
     * @see #removeKeyListener
     */
    public void addKeyListener(KeyListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.KeyUp,typedListener);
        addListener(SWT.KeyDown,typedListener);
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notified when mouse buttons are pressed and released, by sending
     * it one of the messages defined in the <code>MouseListener</code>
     * interface.
     *
     * @param listener the listener which should be notified
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see MouseListener
     * @see #removeMouseListener
     */
    public void addMouseListener(MouseListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.MouseDown,typedListener);
        addListener(SWT.MouseUp,typedListener);
        addListener(SWT.MouseDoubleClick,typedListener);
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notified when the mouse moves, by sending it one of the
     * messages defined in the <code>MouseMoveListener</code>
     * interface.
     *
     * @param listener the listener which should be notified
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see MouseMoveListener
     * @see #removeMouseMoveListener
     */
    public void addMouseMoveListener(MouseMoveListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.MouseMove,typedListener);
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notified when the receiver needs to be painted, by sending it
     * one of the messages defined in the <code>PaintListener</code>
     * interface.
     *
     * @param listener the listener which should be notified
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see PaintListener
     * @see #removePaintListener
     */
    public void addPaintListener(PaintListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Paint,typedListener);
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notified when traversal events occur, by sending it
     * one of the messages defined in the <code>TraverseListener</code>
     * interface.
     *
     * @param listener the listener which should be notified
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see TraverseListener
     * @see #removeTraverseListener
     */
    public void addTraverseListener(TraverseListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Traverse,typedListener);
    }

    /**
     * Returns the preferred size of the receiver.
     * <p>
     * The <em>preferred size</em> of a control is the size that it would
     * best be displayed at. The width hint and height hint arguments
     * allow the caller to ask a control questions such as "Given a particular
     * width, how high does the control need to be to show all of the contents?"
     * To indicate that the caller does not wish to constrain a particular
     * dimension, the constant <code>SWT.DEFAULT</code> is passed for the hint.
     * </p>
     *
     * @param wHint the width hint (can be <code>SWT.DEFAULT</code>)
     * @param hHint the height hint (can be <code>SWT.DEFAULT</code>)
     * @return the preferred size of the control
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Layout
     * @see #getBorderWidth
     * @see #getBounds
     * @see #getSize
     * @see #pack
     * @see "computeTrim, getClientArea for controls that implement them"
     */
    public Point computeSize(int wHint, int hHint)
    {
        return computeSize(wHint, hHint, true);
    }

    /**
     * Returns the preferred size of the receiver.
     * <p>
     * The <em>preferred size</em> of a control is the size that it would
     * best be displayed at. The width hint and height hint arguments
     * allow the caller to ask a control questions such as "Given a particular
     * width, how high does the control need to be to show all of the contents?"
     * To indicate that the caller does not wish to constrain a particular
     * dimension, the constant <code>SWT.DEFAULT</code> is passed for the hint.
     * </p><p>
     * If the changed flag is <code>true</code>, it indicates that the receiver's
     * <em>contents</em> have changed, therefore any caches that a layout manager
     * containing the control may have been keeping need to be flushed. When the
     * control is resized, the changed flag will be <code>false</code>, so layout
     * manager caches can be retained.
     * </p>
     *
     * @param wHint the width hint (can be <code>SWT.DEFAULT</code>)
     * @param hHint the height hint (can be <code>SWT.DEFAULT</code>)
     * @param changed <code>true</code> if the control's contents have changed, and <code>false</code> otherwise
     * @return the preferred size of the control.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Layout
     * @see #getBorderWidth
     * @see #getBounds
     * @see #getSize
     * @see #pack
     * @see "computeTrim, getClientArea for controls that implement them"
     */
    public Point computeSize(int wHint, int hHint, boolean changed)
    {
        checkWidget();
        return OS.Control_ComputeSize(handle, wHint, hHint);
    }

    /*
     * Creates the native side handle of the control.
     */
    protected abstract void internal_createHandle();
    protected void internal_createHandle(int style) {}

    protected void internal_createWidget()
    {
        checkOrientation(parent);
        internal_createHandle();
        internal_createHandle(style);

        try
        {
            if (parent != null)
            {
                parent.addControl(this);
            }
        }
        catch (Error e)
        {
            if (handle != 0) OS.Control_Dispose(handle);
            throw e;
        }
    }

    protected void internal_releaseHandle()
    {
        if (parent != null)
        {
            // See comment for Control.childBeingDisposed variable. Parent Composite
            // hasnt been Disposed from this Control's dispose listener.
            parent.childBeingDisposed = null;
        }

        if (handle != 0)
        {
            OS.Control_Dispose(handle);
            handle = 0;
        }
        super.internal_releaseHandle();
    }

    /**
     * Forces the receiver to have the <em>keyboard focus</em>, causing
     * all keyboard events to be delivered to it.
     *
     * @return <code>true</code> if the control got focus, and <code>false</code> if it was unable to.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #setFocus
     */
    public boolean forceFocus()
    {
        checkWidget();
        return OS.Control_ForceFocus(handle);
    }

    /**
     * Returns the receiver's background color.
     *
     * @return the background color
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Color getBackground()
    {
        checkWidget();
        if (background == null)
            setBackground(null); //will set the default system colour
        return background;
    }

    /**
     * Returns the receiver's border width.
     *
     * @return the border width
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getBorderWidth()
    {
        checkWidget();
        return OS.Control_GetBorderWidth(handle);
    }

    /**
     * Returns a rectangle describing the receiver's size and location
     * relative to its parent (or its display if its parent is null),
     * unless the receiver is a shell. In this case, the location is
     * relative to the display.
     *
     * @return the receiver's bounding rectangle
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Rectangle getBounds()
    {
        checkWidget();
        return OS.Control_GetBounds(handle);
    }

    Rectangle getClientArea()
    {
        return OS.Control_GetClientArea(handle);
    }

    /**
     * Returns <code>true</code> if the receiver is enabled, and
     * <code>false</code> otherwise. A disabled control is typically
     * not selectable from the user interface and draws with an
     * inactive or "grayed" look.
     *
     * @return the receiver's enabled state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #isEnabled
     */
    public boolean getEnabled()
    {
        checkWidget();
        return (state & DISABLED) == 0;
    }

    /**
     * Returns the font that the receiver will use to paint textual information.
     *
     * @return the receiver's font
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Font getFont()
    {
        checkWidget();
        int fontHandle = OS.Control_GetFont(handle);
        // NTH: this test is probably useless as OS.Control_GetFont(handle) should always return a non-zero value
        if (fontHandle == 0)
            return display.getSystemFont();
        // NTH: OS.Control_GetFont(handle) should return the Font object, instead of the font Handle.
        // Returned font should not be disposed by user so we set it as system.
        // In case the font was originally allocated by user, a reference to the font should be
        // kept by user when allocating the font and font should be disposed through this reference,
        // not through Control.getFont.
        else return Font.internal_newSystemFont(display,fontHandle);
    }

    /**
     * Returns the foreground color that the receiver will use to draw.
     *
     * @return the receiver's foreground color
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Color getForeground()
    {
        checkWidget();
        if (foreground == null)
            setForeground(null); //will set the default system colour
        return foreground;
    }

    /**
     * Returns layout data which is associated with the receiver.
     *
     * @return the receiver's layout data
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Object getLayoutData()
    {
        checkWidget();
        return layoutData;
    }

    /**
     * Returns a point describing the receiver's location relative
     * to its parent (or its display if its parent is null), unless
     * the receiver is a shell. In this case, the point is
     * relative to the display.
     *
     * @return the receiver's location
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Point getLocation()
    {
        checkWidget();
        return OS.Control_GetLocation(handle);
    }

    /**
     * Returns the receiver's pop up menu if it has one, or null
     * if it does not. All controls may optionally have a pop up
     * menu that is displayed when the user requests one for
     * the control. The sequence of key strokes, button presses
     * and/or button releases that are used to request a pop up
     * menu is platform specific.
     *
     * @return the receiver's menu
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Menu getMenu()
    {
        checkWidget();
        return menu;
    }

    /**
     * Returns the receiver's parent, which must be a <code>Composite</code>
     * or null when the receiver is a shell that was created with null or
     * a display for a parent.
     *
     * @return the receiver's parent
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Composite getParent()
    {
        checkWidget();
        return parent;
    }

    /**
     * Returns the receiver's shell. For all controls other than
     * shells, this simply returns the control's nearest ancestor
     * shell. Shells return themselves, even if they are children
     * of other shells.
     *
     * @return the receiver's shell
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #getParent
     */
    public Shell getShell()
    {
        checkWidget();
        return parent.getShell();
    }

    /**
     * Returns a point describing the receiver's size. The
     * x coordinate of the result is the width of the receiver.
     * The y coordinate of the result is the height of the
     * receiver.
     *
     * @return the receiver's size
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Point getSize()
    {
        checkWidget();
        return OS.Control_GetSize(handle);
    }

    public int getStyle()
    {
        return OS.Control_GetStyle(handle);
    }

    /**
     * Returns the receiver's tool tip text, or null if it has
     * not been set.
     *
     * @return the receiver's tool tip text
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public String getToolTipText()
    {
        checkWidget();
        return toolTipText;
    }

    /**
     * Returns <code>true</code> if the receiver is visible, and
     * <code>false</code> otherwise.
     * <p>
     * If one of the receiver's ancestors is not visible or some
     * other condition makes the receiver not visible, this method
     * may still indicate that it is considered visible even though
     * it may not actually be showing.
     * </p>
     *
     * @return the receiver's visibility state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean getVisible()
    {
        checkWidget();
        return OS.Control_GetVisible(this.display.handle, handle);
    }

    /**
     * Invokes platform specific functionality to allocate a new GC handle.
     * <p>
     * <b>IMPORTANT:</b> This method is <em>not</em> part of the public
     * API for <code>Control</code>. It is marked public only so that it
     * can be shared within the packages provided by SWT. It is not
     * available on all platforms, and should never be called from
     * application code.
     * </p>
     *
     * @param data the platform specific GC data
     * @return the platform specific GC handle
     */
    public int internal_new_GC(GCData data)
    {
        return OS.Drawable_NewGc(handle);
    }

    /**
     * Invokes platform specific functionality to dispose a GC handle.
     * <p>
     * <b>IMPORTANT:</b> This method is <em>not</em> part of the public
     * API for <code>Control</code>. It is marked public only so that it
     * can be shared within the packages provided by SWT. It is not
     * available on all platforms, and should never be called from
     * application code.
     * </p>
     *
     * @param hDC the platform specific GC handle
     * @param data the platform specific GC data
     */
    public void internal_dispose_GC(int gcHandle, GCData data)
    {
        OS.GC_Dispose(gcHandle);
    }

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
        // Note: this method can be called in a non-UI thread by a GC.
        // It must not check for thread invalid access.

        Display display = this.display;
        if (display == null) internal_error(SWT.ERROR_WIDGET_DISPOSED);
        if ((state & DISPOSED) != 0) internal_error(SWT.ERROR_WIDGET_DISPOSED);

        if (parent!=null && !parent.internal_isVisible())
            return false;
        return OS.Control_GetVisible(display.handle, handle);
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
     * Returns <code>true</code> if the receiver is enabled and all
     * ancestors up to and including the receiver's nearest ancestor
     * shell are enabled.  Otherwise, <code>false</code> is returned.
     * A disabled control is typically not selectable from the user
     * interface and draws with an inactive or "grayed" look.
     *
     * @return the receiver's enabled state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #getEnabled
     */
    public boolean isEnabled()
    {
        checkWidget();
        if (parent != null)
            return getEnabled() && parent.isEnabled();
        return getEnabled();
    }

    /**
     * Returns <code>true</code> if the receiver has the user-interface
     * focus, and <code>false</code> otherwise.
     *
     * @return the receiver's focus state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean isFocusControl()
    {
        checkWidget();
        return OS.Control_IsFocusControl(handle);
    }

    /**
     * Returns <code>true</code> if the underlying operating
     * system supports this reparenting, otherwise <code>false</code>
     *
     * @return <code>true</code> if the widget can be reparented, otherwise <code>false</code>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean isReparentable()
    {
        checkWidget();
        return false; // ==> For now we do not support reparenting
        // as from specs, this might change however
    }

    /**
     * Returns <code>true</code> if the receiver is visible and all
     * ancestors up to and including the receiver's nearest ancestor
     * shell are visible. Otherwise, <code>false</code> is returned.
     *
     * @return the receiver's visibility state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #getVisible
     */
    public boolean isVisible()
    {
        checkWidget();
        if (parent!=null && !parent.isVisible())
            return false;
        return getVisible();
    }

    Decorations menuDecorations()
    {
        return parent.menuDecorations();
    }

    /**
     * Moves the receiver above the specified control in the
     * drawing order. If the argument is null, then the receiver
     * is moved to the top of the drawing order. The control at
     * the top of the drawing order will not be covered by other
     * controls even if they occupy intersecting areas.
     *
     * @param control the sibling control (or null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the control has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Control#moveBelow
     * @see Composite#getChildren
     */
    public void moveAbove(Control control)
    {
        checkWidget();
        int controlHandle = 0;
        if (control != null)
        {
            if (control.isDisposed()) internal_error(SWT.ERROR_INVALID_ARGUMENT);
            if (parent != control.parent) return;
            controlHandle = control.handle;
            if (controlHandle == handle) return;
        }
        OS.Control_MoveAbove(handle, controlHandle);
    }

    /**
     * Moves the receiver below the specified control in the
     * drawing order. If the argument is null, then the receiver
     * is moved to the bottom of the drawing order. The control at
     * the bottom of the drawing order will be covered by all other
     * controls which occupy intersecting areas.
     *
     * @param control the sibling control (or null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the control has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Control#moveAbove
     * @see Composite#getChildren
     */
    public void moveBelow(Control control)
    {
        checkWidget();
        int controlHandle = 0;
        if (control != null)
        {
            if (control.isDisposed()) internal_error(SWT.ERROR_INVALID_ARGUMENT);
            if (parent != control.parent) return;
            controlHandle = control.handle;
            if (controlHandle == handle) return;
        }
        OS.Control_MoveBelow(handle, controlHandle);
    }

    /**
     * Causes the receiver to be resized to its preferred size.
     * For a composite, this involves computing the preferred size
     * from its layout, if there is one.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #computeSize
     */
    public void pack()
    {
        checkWidget();
        pack(true);
    }

    /**
     * Causes the receiver to be resized to its preferred size.
     * For a composite, this involves computing the preferred size
     * from its layout, if there is one.
     * <p>
     * If the changed flag is <code>true</code>, it indicates that the receiver's
     * <em>contents</em> have changed, therefore any caches that a layout manager
     * containing the control may have been keeping need to be flushed. When the
     * control is resized, the changed flag will be <code>false</code>, so layout
     * manager caches can be retained.
     * </p>
     *
     * @param changed whether or not the receiver's contents have changed
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #computeSize
     */
    public void pack(boolean changed)
    {
        checkWidget();
        setSize(computeSize(SWT.DEFAULT, SWT.DEFAULT, changed));
    }

    /**
     * Causes the entire bounds of the receiver to be marked
     * as needing to be redrawn. The next time a paint request
     * is processed, the control will be completely painted,
     * including the background.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #update
     * @see PaintListener
     * @see SWT#Paint
     * @see SWT#NO_BACKGROUND
     * @see SWT#NO_REDRAW_RESIZE
     * @see SWT#NO_MERGE_PAINTS
     */
    public void redraw()
    {
        checkWidget();
        if (!isVisible()) return;
        OS.Control_Redraw(handle);
    }

    /**
     * Causes the rectangular area of the receiver specified by
     * the arguments to be marked as needing to be redrawn.
     * The next time a paint request is processed, that area of
     * the receiver will be painted, including the background.
     * If the <code>all</code> flag is <code>true</code>, any
     * children of the receiver which intersect with the specified
     * area will also paint their intersecting areas. If the
     * <code>all</code> flag is <code>false</code>, the children
     * will not be painted.
     *
     * @param x the x coordinate of the area to draw
     * @param y the y coordinate of the area to draw
     * @param width the width of the area to draw
     * @param height the height of the area to draw
     * @param all <code>true</code> if children should redraw, and <code>false</code> otherwise
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #update
     * @see PaintListener
     * @see SWT#Paint
     * @see SWT#NO_BACKGROUND
     * @see SWT#NO_REDRAW_RESIZE
     * @see SWT#NO_MERGE_PAINTS
     */
    public void redraw(int x, int y, int width, int height, boolean all)
    {
        checkWidget();
        if (!getVisible()) return;
        if (width <= 0 || height <= 0) return;
        if (all)
        {
            OS.Control_RedrawRegion(handle, x, y, width, height);
        }
        else
        {
            Rectangle rect = new Rectangle(x, y, width, height);
            paint(toDisplay(rect), true, false);
        }
    }

    protected void internal_prepareForRelease()
    {
        // Help the Composites dispose faster.
        doSetVisible(false);
        
        if (hooks(SWT.Paint))
        {
            Display display = Display.getCurrent();
            if (display != null)
            {
                display.internal_free_sharedBuffer();
            }
            getShell().setAsyncPainting(this, false);
        }
        if (parent != null)
        {
            parent.removeControl(this);

            // Store reference to this Control in case that parent Composite will be
            // disposed in this Control's DisposeListener. See comment for
            // Control.childBeingDisposed variable.
            parent.childBeingDisposed = this;
        }
    }

    protected void internal_releaseResources()
    {
        if (background != null)
        {
            background.dispose();
            background = null;
        }

        if (foreground != null)
        {
            foreground.dispose();
            foreground = null;
        }

        toolTipText = null;
        if (menu != null && !menu.isDisposed())
        {
            menu.dispose();
        }
        menu = null;
        parent = null;
        layoutData = null;

        if (commandChildren != null)
        {
            commandChildren.release();
            commandChildren = null;
        }

        super.internal_releaseResources();
    }

    /**
     * Removes the listener from the collection of listeners who will
     * be notified when the control is moved or resized.
     *
     * @param listener the listener which should no longer be notified
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see ControlListener
     * @see #addControlListener
     */
    public void removeControlListener(ControlListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook(SWT.Move, listener);
        eventTable.unhook(SWT.Resize, listener);
    }

    /**
     * Removes the listener from the collection of listeners who will
     * be notified when the control gains or loses focus.
     *
     * @param listener the listener which should no longer be notified
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see FocusListener
     * @see #addFocusListener
     */
    public void removeFocusListener(FocusListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook(SWT.FocusIn, listener);
        eventTable.unhook(SWT.FocusOut, listener);
    }

    /**
     * Removes the listener from the collection of listeners who will
     * be notified when keys are pressed and released on the system keyboard.
     *
     * @param listener the listener which should no longer be notified
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see KeyListener
     * @see #addKeyListener
     */
    public void removeKeyListener(KeyListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook(SWT.KeyUp, listener);
        eventTable.unhook(SWT.KeyDown, listener);
    }

    /**
     * Removes the listener from the collection of listeners who will
     * be notified when mouse buttons are pressed and released.
     *
     * @param listener the listener which should no longer be notified
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see MouseListener
     * @see #addMouseListener
     */
    public void removeMouseListener(MouseListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook(SWT.MouseDown, listener);
        eventTable.unhook(SWT.MouseUp, listener);
        eventTable.unhook(SWT.MouseDoubleClick, listener);
    }

    /**
     * Removes the listener from the collection of listeners who will
     * be notified when the mouse moves.
     *
     * @param listener the listener which should no longer be notified
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see MouseMoveListener
     * @see #addMouseMoveListener
     */
    public void removeMouseMoveListener(MouseMoveListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook(SWT.MouseMove, listener);
    }

    /**
     * Removes the listener from the collection of listeners who will
     * be notified when the receiver needs to be painted.
     *
     * @param listener the listener which should no longer be notified
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see PaintListener
     * @see #addPaintListener
     */
    public void removePaintListener(PaintListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook(SWT.Paint, listener);
        if (!eventTable.hooks(SWT.Paint)) 
        {
            getShell().setAsyncPainting(this, false);
        }
    }

    /**
     * Removes the listener from the collection of listeners who will
     * be notified when traversal events occur.
     *
     * @param listener the listener which should no longer be notified
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see TraverseListener
     * @see #addTraverseListener
     */
    public void removeTraverseListener(TraverseListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook(SWT.Traverse, listener);
    }

    void sendResizeEvent()
    {
        internal_sendEvent(SWT.Resize);
    }

    Event sendTraverseEvent(int detail, boolean doit, char character, int keyCode, int stateMask)
    {
        Event event = new Event();
        event.character = character;
        event.keyCode   = keyCode;
        event.stateMask = stateMask;
        event.detail    = detail;
        event.doit      = doit;
        internal_sendEvent(SWT.Traverse, event);

        // Close the dialog Shell
        if (event.doit && event.detail==SWT.TRAVERSE_ESCAPE)
        {
            Shell shell = getShell();
            if (shell.getParent() != null)   // Dialog shells only
            {
                shell.close();
            }
        }

        // Send the Selection event to the Defaut Button.
        if (event.doit && event.detail==SWT.TRAVERSE_RETURN)
        {
            Shell shell = getShell();

            Button defaultButton = shell.getDefaultButton();

            if (defaultButton != null)
            {
                OS.Button_Click(defaultButton.GetButtonHandle());
            }
        }

        return event;
    }

    private final boolean sendKeyEvent(int type, char character, int keyCode, int stateMask)
    {
        Event event = new Event();
        event.type      = type;
        event.character = character;
        event.keyCode   = keyCode;
        event.stateMask = stateMask;

        internal_sendEvent(type, event);
        // widget could be disposed at this point

        /*
        * It is possible (but unlikely), that application
        * code could have disposed the widget in the key
        * events.  If this happens, end the processing of
        * the key by returning false.
        */
        if (isDisposed()) return false;
        return event.doit;
    }


    /**
     * Sets the receiver's background color to the color specified
     * by the argument, or to the default system color for the control
     * if the argument is null.
     *
     * @param color the new color (or null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the argument has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setBackground(Color color)
    {
        checkWidget();
        int colorHandle;
        if (color != null)
        {
            if (color.isDisposed())
            {
                SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            }
            colorHandle = color.handle;
        }
        else
        {
            colorHandle = 0;
        }
        OS.Control_SetBackground(handle, colorHandle);
        if (background != null)
            background.dispose();
        background = internal_createColor(OS.Control_GetBackground(handle));
    }
    /**
     * Sets the receiver's size and location to the rectangular
     * area specified by the arguments. The <code>x</code> and
     * <code>y</code> arguments are relative to the receiver's
     * parent (or its display if its parent is null), unless
     * the receiver is a shell. In this case, the <code>x</code>
     * and <code>y</code> arguments are relative to the display.
     * <p>
     * Note: Attempting to set the width or height of the
     * receiver to a negative number will cause that
     * value to be set to zero instead.
     * </p>
     *
     * @param x the new x coordinate for the receiver
     * @param y the new y coordinate for the receiver
     * @param width the new width for the receiver
     * @param height the new height for the receiver
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setBounds(int x, int y, int width, int height)
    {
        checkWidget();
        setBounds(x, y, Math.max(0, width), Math.max(0, height), true, true);
    }
    
    int setBounds(int x, int y, int width, int height, boolean move, boolean resize)
    {
        int result = 0;
        
        if (resize)
        {
            Point oldSize = OS.Control_GetSize(handle);
            if (oldSize.x != width || oldSize.y != height)
            {
                OS.Control_SetSize(handle, Math.max(width,0), Math.max(height,0));
                result |= Widget.RESIZED;
            }
        }
        
        if (move)
        {
            Point oldPos = OS.Control_GetLocation(handle);
            if (oldPos.x != x || oldPos.y != y) 
            {
                OS.Control_SetLocation(handle, x, y);
                result |= Widget.MOVED;
            }
        }
        
        return result;
    }

    /**
     * Sets the receiver's size and location to the rectangular
     * area specified by the argument. The <code>x</code> and
     * <code>y</code> fields of the rectangle are relative to
     * the receiver's parent (or its display if its parent is null).
     * <p>
     * Note: Attempting to set the width or height of the
     * receiver to a negative number will cause that
     * value to be set to zero instead.
     * </p>
     *
     * @param rect the new bounds for the receiver
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setBounds(Rectangle rect)
    {
        checkWidget();
        if (rect == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        setBounds(rect.x, rect.y, Math.max(0, rect.width), Math.max(0, rect.height), true, true);
    }

    /**
     * If the argument is <code>true</code>, causes the receiver to have
     * all mouse events delivered to it until the method is called with
     * <code>false</code> as the argument.
     *
     * @param capture <code>true</code> to capture the mouse, and <code>false</code> to release it
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setCapture(boolean capture)
    {
        checkWidget();
        OS.Control_SetCapture(handle, capture);
    }

    /**
     * Enables the receiver if the argument is <code>true</code>,
     * and disables it otherwise. A disabled control is typically
     * not selectable from the user interface and draws with an
     * inactive or "grayed" look.
     *
     * @param enabled the new enabled state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setEnabled(boolean enabled)
    {
        checkWidget();
        if (getEnabled() != enabled)
        {
            OS.Control_SetEnabled(handle, enabled);
            if (!enabled)
                state |= DISABLED;
            else
                state &= ~DISABLED;
        }
    }

    /**
     * Causes the receiver to have the <em>keyboard focus</em>,
     * such that all keyboard events will be delivered to it.  Focus
     * reassignment will respect applicable platform constraints.
     *
     * @return <code>true</code> if the control got focus, and <code>false</code> if it was unable to.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #forceFocus
     */
    public boolean setFocus()
    {
        checkWidget();
        return OS.Control_SetFocus(handle);
    }

    /**
     * Sets the font that the receiver will use to paint textual information
     * to the font specified by the argument, or to the default font for that
     * kind of control if the argument is null.
     *
     * @param font the new font (or null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the argument has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setFont(Font font)
    {
        checkWidget();
        int fontHandle;
        if (font != null)
        {
            if (font.isDisposed()) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            fontHandle = font.handle;
        }
        else
        {
            fontHandle =  0;
        }
        OS.Control_SetFont(handle, fontHandle);
    }

    /**
     * Sets the receiver's foreground color to the color specified
     * by the argument, or to the default system color for the control
     * if the argument is null.
     *
     * @param color the new color (or null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the argument has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setForeground(Color color)
    {
        checkWidget();
        int colorHandle;
        if (color != null)
        {
            if (color.isDisposed())
            {
                SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            }
            colorHandle = color.handle;
        }
        else
        {
            colorHandle = 0;
        }
        OS.Control_SetForeground(handle, colorHandle);
        if (foreground != null)
            foreground.dispose();
        foreground = internal_createColor(OS.Control_GetForeground(handle));
    }

    /**
     * Sets the layout data associated with the receiver to the argument.
     *
     * @param layoutData the new layout data for the receiver.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setLayoutData(Object layoutData)
    {
        checkWidget();
        this.layoutData = layoutData;
    }

    /**
     * Sets the receiver's location to the point specified by
     * the arguments which are relative to the receiver's
     * parent (or its display if its parent is null), unless
     * the receiver is a shell. In this case, the point is
     * relative to the display.
     *
     * @param x the new x coordinate for the receiver
     * @param y the new y coordinate for the receiver
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setLocation(int x, int y)
    {
        checkWidget();
        setBounds(x, y, 0, 0, true, false);
    }

    /**
     * Sets the receiver's location to the point specified by
     * the arguments which are relative to the receiver's
     * parent (or its display if its parent is null), unless
     * the receiver is a shell. In this case, the point is
     * relative to the display.
     *
     * @param location the new location for the receiver
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setLocation(Point location)
    {
        checkWidget();
        if (location == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        setBounds(location.x, location.y, 0, 0, true, false);
    }

    /**
     * Sets the receiver's pop up menu to the argument.
     * All controls may optionally have a pop up
     * menu that is displayed when the user requests one for
     * the control. The sequence of key strokes, button presses
     * and/or button releases that are used to request a pop up
     * menu is platform specific.
     *
     * @param menu the new pop up menu
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_MENU_NOT_POP_UP - the menu is not a pop up menu</li>
     *    <li>ERROR_INVALID_PARENT - if the menu is not in the same widget tree</li>
     *    <li>ERROR_INVALID_ARGUMENT - if the menu has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setMenu(Menu menu)
    {
        checkWidget();
        if (menu != null)
        {
            if (menu.isDisposed()) internal_error(SWT.ERROR_INVALID_ARGUMENT);
            if ((menu.style & SWT.POP_UP) == 0) internal_error(SWT.ERROR_MENU_NOT_POP_UP);
            if (menu.parent != menuDecorations()) internal_error(SWT.ERROR_INVALID_PARENT);
        }
        if (this.menu != null)
        {
            this.menu.setControl(null);
        }
        this.menu = menu;
        if (menu != null)
        {
            menu.setControl(this);
        }
        OS.Control_SetMenu(handle, (menu == null) ? 0 : menu.handle);
    }

    /**
     * If the argument is <code>false</code>, causes subsequent drawing
     * operations in the receiver to be ignored. No drawing of any kind
     * can occur in the receiver until the flag is set to true.
     * Graphics operations that occurred while the flag was
     * <code>false</code> are lost. When the flag is set to <code>true</code>,
     * the entire widget is marked as needing to be redrawn.  Nested calls
     * to this method are stacked.
     * <p>
     * Note: This operation is a hint and may not be supported on some
     * platforms or for some widgets.
     * </p>
     *
     * @param redraw the new redraw state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #redraw
     * @see #update
     */
    public void setRedraw(boolean redraw)
    {
        checkWidget();
        OS.Control_SetRedraw(handle, redraw);
    }

    /**
     * Sets the receiver's size to the point specified by the arguments.
     * <p>
     * Note: Attempting to set the width or height of the
     * receiver to a negative number will cause that
     * value to be set to zero instead.
     * </p>
     *
     * @param width the new width for the receiver
     * @param height the new height for the receiver
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setSize(int width, int height)
    {
        checkWidget();
        setBounds(0, 0, Math.max(0, width), Math.max(0, height), false, true);
    }

    /**
     * Sets the receiver's size to the point specified by the argument.
     * <p>
     * Note: Attempting to set the width or height of the
     * receiver to a negative number will cause them to be
     * set to zero instead.
     * </p>
     *
     * @param size the new size for the receiver
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the point is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setSize(Point size)
    {
        checkWidget();
        if (size == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        setBounds(0, 0, Math.max(0, size.x), Math.max(0, size.y), false, true);
    }

    /**
     * Sets the receiver's tool tip text to the argument, which
     * may be null indicating that no tool tip text should be shown.
     *
     * @param string the new tool tip text (or null)
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setToolTipText(String string)
    {
        checkWidget();
        toolTipText = string;
    }

    /**
     * Marks the receiver as visible if the argument is <code>true</code>,
     * and marks it invisible otherwise.
     * <p>
     * If one of the receiver's ancestors is not visible or some
     * other condition makes the receiver not visible, marking
     * it visible may not actually cause it to be displayed.
     * </p>
     *
     * @param visible the new visibility state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setVisible(boolean visible)
    {
        checkWidget();
        doSetVisible(visible);
        internal_sendEvent((visible) ? SWT.Show : SWT.Hide);
    }
    
    private void doSetVisible(boolean visible)
    {
        this.visible = visible;
        OS.Control_SetVisible(handle, visible);
    }

    protected void internal_sort(int [] items)
    {
        /* Shell Sort from K&R, pg 108 */
        int length = items.length;
        for (int gap=length/2; gap>0; gap/=2)
        {
            for (int i=gap; i<length; i++)
            {
                for (int j=i-gap; j>=0; j-=gap)
                {
                    if (items [j] <= items [j + gap])
                    {
                        int swap = items [j];
                        items [j] = items [j + gap];
                        items [j + gap] = swap;
                    }
                }
            }
        }
    }

    /**
     * Returns a point which is the result of converting the
     * argument, which is specified in display relative coordinates,
     * to coordinates relative to the receiver.
     * <p>
     * @param x the x coordinate to be translated
     * @param y the y coordinate to be translated
     * @return the translated coordinates
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 2.1
     */
    public Point toControl(int x, int y)
    {
        checkWidget();
        return OS.Control_ToControl(handle,x,y);
    }

    /**
     * Returns a point which is the result of converting the
     * argument, which is specified in display relative coordinates,
     * to coordinates relative to the receiver.
     * <p>
     * @param point the point to be translated (must not be null)
     * @return the translated coordinates
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the point is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Point toControl(Point point)
    {
        if (point == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        return toControl(point.x, point.y);
    }

    Rectangle toControl(Rectangle rect)
    {
        if (rect == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        Point origin = toControl(rect.x, rect.y);
        return new Rectangle(origin.x, origin.y, rect.width, rect.height);
    }

    /**
     * Returns a point which is the result of converting the
     * argument, which is specified in coordinates relative to
     * the receiver, to display relative coordinates.
     * <p>
     * @param x the x coordinate to be translated
     * @param y the y coordinate to be translated
     * @return the translated coordinates
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 2.1
     */
    public Point toDisplay(int x, int y)
    {
        checkWidget();
        return OS.Control_ToDisplay(handle,x,y);
    }

    /**
     * Returns a point which is the result of converting the
     * argument, which is specified in coordinates relative to
     * the receiver, to display relative coordinates.
     * <p>
     * @param point the point to be translated (must not be null)
     * @return the translated coordinates
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the point is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Point toDisplay(Point point)
    {
        if (point == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        return toDisplay(point.x, point.y);
    }

    Rectangle toDisplay(Rectangle rect)
    {
        if (rect == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        Point origin = toDisplay(rect.x, rect.y);
        return new Rectangle(origin.x, origin.y, rect.width, rect.height);
    }

    /**
     * Based on the argument, perform one of the expected platform
     * traversal action. The argument should be one of the constants:
     * <code>SWT.TRAVERSE_ESCAPE</code>, <code>SWT.TRAVERSE_RETURN</code>,
     * <code>SWT.TRAVERSE_TAB_NEXT</code>, <code>SWT.TRAVERSE_TAB_PREVIOUS</code>,
     * <code>SWT.TRAVERSE_ARROW_NEXT</code> and <code>SWT.TRAVERSE_ARROW_PREVIOUS</code>.
     *
     * @param traversal the type of traversal
     * @return true if the traversal succeeded
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean traverse(int traversal)
    {
        checkWidget();
        return OS.Control_PostTraverseEvent(handle, traversal);
    }

    /**
     * Forces all outstanding paint requests for the widget
     * to be processed before this method returns.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #redraw
     * @see PaintListener
     * @see SWT#Paint
     */
    public void update()
    {
        checkWidget();
        OS.Control_Update(handle);
    }


    /**
     * Changes the parent of the widget to be the one provided if
     * the underlying operating system supports this feature.
     * Answers <code>true</code> if the parent is successfully changed.
     *
     * @param parent the new parent for the control.
     * @return <code>true</code> if the parent is changed and <code>false</code> otherwise.
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the argument has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *  </ul>
     */
    public boolean setParent(Composite parent)
    {
        checkWidget();
        if (parent == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (parent.isDisposed()) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        if (this.parent == parent) return true;
        if (!isReparentable()) return false;

        // If reparenting is supported, then menu of control must be reparented too
//  if (menu != null && menu.parent != this && menuDecorations() != parent.menuDecorations())
//      menu.setParent(parent.menuDecorations());

        return false;   // ==> For now we do not support reparenting
        // as from specs, this might change however
    }

    /**
     * Computes the intersection of two rectangles
     *
     * @return The resulting intersection rectangle or null if the two rectangles
     *         do not intersect each other
     */
    static Rectangle rectangleIntersection(Rectangle rect1, Rectangle rect2)
    {
        // rect1's coordinates
        int xmin1 = rect1.x;
        int ymin1 = rect1.y;
        int xmax1 = rect1.x + rect1.width;
        int ymax1 = rect1.y + rect1.height;

        // rect2's coordinates
        int xmin2 = rect2.x;
        int ymin2 = rect2.y;
        int xmax2 = rect2.x + rect2.width;
        int ymax2 = rect2.y + rect2.height;

        // Check for an empty intersection
        if (xmax1 <= xmin2 || xmax2 <= xmin1 || ymax1 <= ymin2 || ymax2 <= ymin1)
            return null;

        // Compute the intersection
        int xmin = Math.max(xmin1, xmin2);
        int ymin = Math.max(ymin1, ymin2);
        int xmax = Math.min(xmax1, xmax2);
        int ymax = Math.min(ymax1, ymax2);
        return new Rectangle(xmin, ymin, xmax-xmin, ymax-ymin);
    }

    Rectangle paint(Rectangle rect, boolean paintNatively, boolean paintChildren)
    {

        if (OS.Control_HasDoNotDrawFlag(handle))
        {
            return null;
        }

        // Compute the client area to be redrawn (in display coordinates)
        Rectangle client      = toDisplay(getClientArea());
        Rectangle dirtyClient = rectangleIntersection(client, rect);

        boolean gcDisposedInListener = false;

        //for nested paint loop
        boolean gcCreatedInPaint = false;

        try
        {
            // Notify paint listeners, unless there is no client area to draw on
            if (dirtyClient != null && hooks(SWT.Paint))
            {
                if (gc==null)
                {
                    gc = new GC(this);
                    gcCreatedInPaint = true;
                }
                // Try to get the shared buffer.
                // If it's already being used then create a temporary buffer.
                // The shared buffer might be in use in the case that the application
                // is dispatching a paint event in the paint listener.
                if (buffer==null)
                {
                    buffer = display.internal_sharedBuffer();
                    if (!buffer.isFree())
                    {
                        buffer = new GCBuffer();
                    }

                    //  Initialise the GC to use the buffer. Also tags the buffer as being in use.
                    gc.internal_buffer_init(buffer);
                }
                // Call the paint listener
                sendPaintEvent(toControl(dirtyClient), gc);
                if (gc.isDisposed())
                {
                    gcDisposedInListener = true;
                }
            }

            // Paint the control, unless it was disposed
            if (!isDisposed())
            {
                if (gcDisposedInListener)
                {
                    // Create a new gc that will be used to flush the buffer.
                    // All the required initialisation is done by the buffered commands.
                    gc = new GC(this);
                }
                int gcHandle         = (gc == null) ? 0 : gc.handle;
                int[] intParams      = (buffer == null) ? null : buffer.intParams();
                int intParamCount    = (buffer == null) ? 0 : buffer.intParamCount();
                String strParams     = (buffer == null) ? null : buffer.strParams();
                OS.Control_Paint(display.handle, handle, rect.x, rect.y, rect.width, rect.height,
                                 paintNatively, gcHandle, intParams, intParamCount, strParams);
            }
            else
            {
                dirtyClient = null;
            }
        }
        finally
        {
            // Tag the buffer as free and remove references from GC to the buffer.
            if (gc != null && !buffer.isFree() && gcCreatedInPaint)
            {
                gc.internal_buffer_free();
                buffer = null;
            }
            // Temporary buffer can be garbage collected now.
            // The shared buffer is always referenced to from Display.

        }

        if (gc != null && !gc.isDisposed() && gcCreatedInPaint)
        {
            gc.dispose();
            gc=null;
        }

        return dirtyClient;
    }

// if display was disposed from controls paint listener, gc should be cleanedup
// which was created in the paint method before disposing the control
    protected void internal_controlsPaintGcClean()
    {
        if (gc != null)
        {
            gc.internal_buffer_free();
        }
        buffer=null;
        if (gc != null && !gc.isDisposed())
        {
            gc.dispose();
            gc=null;
        }
    }

    private final void sendPaintEvent(Rectangle rect, GC gc)
    {
        if (rect.isEmpty())
            return;

        // Set the clipping rectangle, gets buffered so it's always
        // set in the beginning of a buffer flush.
        gc.setClipping(rect.x, rect.y, rect.width, rect.height);

        // Add to the buffer so that font is always set back to default
        // in the beginning of a buffer flush.
        gc.setFont(null);

        Event event = new Event();
        event.gc     = gc;
        event.x      = rect.x;
        event.y      = rect.y;
        event.width  = rect.width;
        event.height = rect.height;
        /*
        * It is possible (but unlikely), that application
        * code could have disposed the widget in the paint
        * event.  If this happens, attempt to give back the
        * paint GC anyway because this is a scarce resource.
        */
        internal_sendEvent(SWT.Paint, event);
        // widget could be disposed at this point

        event.gc = null;
    }

    private final void sendMouseEvent(int type, int button, int x, int y, int stateMask)
    {
        Event event     = new Event();
        event.button    = button;
        Point p         = toControl(x, y);
        event.x         = p.x;
        event.y         = p.y;
        event.stateMask = stateMask;
        internal_sendEvent(type, event);
    }

    public MobileCommands internal_getCommandChildren()
    {
        return commandChildren;
    }

    public void internal_setCommandChildren(MobileCommands commandChildren)
    {
        this.commandChildren = commandChildren;
    }

    public void addListener(int eventType, Listener listener)
    {
        if (eventType == SWT.Paint) 
        {
            if (eventTable == null || !eventTable.hooks(SWT.Paint))
            {
                getShell().setAsyncPainting(this, true);
            }
        }
        super.addListener(eventType, listener);
    }
    
    public void removeListener(int eventType, Listener listener)
    {
        super.removeListener(eventType, listener);
        if (eventType == SWT.Paint) 
        {
            if (eventTable != null && !eventTable.hooks(SWT.Paint))
            {
                getShell().setAsyncPainting(this, false);
            }
        }
    }
    
    void markLayout(boolean changed, boolean all)
    {
        // Do nothing
    }
    
    void updateLayout(boolean all, boolean resized)
    {
        // Do nothing
    }
}


