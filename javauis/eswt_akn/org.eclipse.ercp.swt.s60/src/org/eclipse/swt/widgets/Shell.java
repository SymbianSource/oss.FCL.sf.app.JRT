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
import org.eclipse.swt.*;
import org.eclipse.swt.graphics.*;
import org.eclipse.swt.events.*;
import java.util.*;

/**
 * Instances of this class represent the "windows"
 * which the desktop or "window manager" is managing.
 * Instances that do not have a parent (that is, they
 * are built using the constructor, which takes a
 * <code>Display</code> as the argument) are described
 * as <em>top level</em> shells. Instances that do have
 * a parent are described as <em>secondary</em> or
 * <em>dialog</em> shells.
 * <p>
 * Instances are always displayed in one of the maximized,
 * minimized or normal states:
 * <ul>
 * <li>
 * When an instance is marked as <em>maximized</em>, the
 * window manager will typically resize it to fill the
 * entire visible area of the display, and the instance
 * is usually put in a state where it can not be resized
 * (even if it has style <code>RESIZE</code>) until it is
 * no longer maximized.
 * </li><li>
 * When an instance is in the <em>normal</em> state (neither
 * maximized or minimized), its appearance is controlled by
 * the style constants which were specified when it was created
 * and the restrictions of the window manager (see below).
 * </li><li>
 * When an instance has been marked as <em>minimized</em>,
 * its contents (client area) will usually not be visible,
 * and depending on the window manager, it may be
 * "iconified" (that is, replaced on the desktop by a small
 * simplified representation of itself), relocated to a
 * distinguished area of the screen, or hidden. Combinations
 * of these changes are also possible.
 * </li>
 * </ul>
 * </p>
 * <p>
 * Note: The styles supported by this class must be treated
 * as <em>HINT</em>s, since the window manager for the
 * desktop on which the instance is visible has ultimate
 * control over the appearance and behavior of decorations
 * and modality. For example, some window managers only
 * support resizable windows and will always assume the
 * RESIZE style, even if it is not set. In addition, if a
 * modality style is not supported, it is "upgraded" to a
 * more restrictive modality style that is supported. For
 * example, if <code>PRIMARY_MODAL</code> is not supported,
 * it would be upgraded to <code>APPLICATION_MODAL</code>.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>BORDER, CLOSE, MIN, MAX, NO_TRIM, RESIZE, TITLE</dd>
 * <dd>APPLICATION_MODAL, MODELESS, PRIMARY_MODAL, SYSTEM_MODAL</dd>
 * <dt><b>Events:</b></dt>
 * <dd>Activate, Close, Deactivate, Deiconify, Iconify</dd>
 * </dl>
 * Class <code>SWT</code> provides two "convenience constants"
 * for the most commonly required style combinations:
 * <dl>
 * <dt><code>SHELL_TRIM</code></dt>
 * <dd>
 * the result of combining the constants which are required
 * to produce a typical application top level shell: (that
 * is, <code>CLOSE | TITLE | MIN | MAX | RESIZE</code>)
 * </dd>
 * <dt><code>DIALOG_TRIM</code></dt>
 * <dd>
 * the result of combining the constants which are required
 * to produce a typical application dialog shell: (that
 * is, <code>TITLE | CLOSE | BORDER</code>)
 * </dd>
 * </dl>
 * </p>
 * <p>
 * Note: Only one of the styles APPLICATION_MODAL, MODELESS,
 * PRIMARY_MODAL and SYSTEM_MODAL may be specified.
 * </p><p>
 * IMPORTANT: This class is not intended to be subclassed.
 * </p>
 *
 * @see Decorations
 * @see SWT
 */
public class Shell extends Decorations
{
    private int shellHandle;

    // Widgets registered with this Shell
    private Vector widgets = new Vector();
    private Vector asyncPaintControls = new Vector();
    
    /**
     * Constructs a new instance of this class. This is equivalent
     * to calling <code>Shell((Display) null)</code>.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     */
    public Shell()
    {
        this((Display) null);
    }

    /**
     * Constructs a new instance of this class given only the style
     * value describing its behavior and appearance. This is equivalent
     * to calling <code>Shell((Display) null, style)</code>.
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
     * @param style the style of control to construct
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     *
     * @see SWT#BORDER
     * @see SWT#CLOSE
     * @see SWT#MIN
     * @see SWT#MAX
     * @see SWT#RESIZE
     * @see SWT#TITLE
     * @see SWT#NO_TRIM
     * @see SWT#SHELL_TRIM
     * @see SWT#DIALOG_TRIM
     * @see SWT#MODELESS
     * @see SWT#PRIMARY_MODAL
     * @see SWT#APPLICATION_MODAL
     * @see SWT#SYSTEM_MODAL
     */
    public Shell(int style)
    {
        this((Display) null, style);
    }

    /**
     * Constructs a new instance of this class given only the display
     * to create it on. It is created with style <code>SWT.SHELL_TRIM</code>.
     * <p>
     * Note: Currently, null can be passed in for the display argument.
     * This has the effect of creating the shell on the currently active
     * display if there is one. If there is no current display, the
     * shell is created on a "default" display. <b>Passing in null as
     * the display argument is not considered to be good coding style,
     * and may not be supported in a future release of SWT.</b>
     * </p>
     *
     * @param display the display to create the shell on
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     */
    public Shell(Display display)
    {
        this(display, SWT.SHELL_TRIM);
    }

    /**
     * Constructs a new instance of this class given the display
     * to create it on and a style value describing its behavior
     * and appearance.
     * <p>
     * The style value is either one of the style constants defined in
     * class <code>SWT</code> which is applicable to instances of this
     * class, or must be built by <em>bitwise OR</em>'ing together
     * (that is, using the <code>int</code> "|" operator) two or more
     * of those <code>SWT</code> style constants. The class description
     * lists the style constants that are applicable to the class.
     * Style bits are also inherited from superclasses.
     * </p><p>
     * Note: Currently, null can be passed in for the display argument.
     * This has the effect of creating the shell on the currently active
     * display if there is one. If there is no current display, the
     * shell is created on a "default" display. <b>Passing in null as
     * the display argument is not considered to be good coding style,
     * and may not be supported in a future release of SWT.</b>
     * </p>
     *
     * @param display the display to create the shell on
     * @param style the style of control to construct
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     *
     * @see SWT#BORDER
     * @see SWT#CLOSE
     * @see SWT#MIN
     * @see SWT#MAX
     * @see SWT#RESIZE
     * @see SWT#TITLE
     * @see SWT#NO_TRIM
     * @see SWT#SHELL_TRIM
     * @see SWT#DIALOG_TRIM
     * @see SWT#MODELESS
     * @see SWT#PRIMARY_MODAL
     * @see SWT#APPLICATION_MODAL
     * @see SWT#SYSTEM_MODAL
     */
    public Shell(Display display, int style)
    {
        this(display, null, style);
    }

    Shell(Display display, Shell parent, int style)
    {
        super();
        checkSubclass();
        if (display == null) display = Display.getCurrent();
        if (display == null) display = Display.getDefault();
        if (!display.isValidThread())
        {
            internal_error(SWT.ERROR_THREAD_INVALID_ACCESS);
        }
        this.style = checkStyle(style);
        this.parent = parent;
        this.display = display;
        internal_createWidget();
        if (this.parent == null)
            display.addShell(this);
    }

    /**
     * Constructs a new instance of this class given only its
     * parent. It is created with style <code>SWT.DIALOG_TRIM</code>.
     * <p>
     * Note: Currently, null can be passed in for the parent.
     * This has the effect of creating the shell on the currently active
     * display if there is one. If there is no current display, the
     * shell is created on a "default" display. <b>Passing in null as
     * the parent is not considered to be good coding style,
     * and may not be supported in a future release of SWT.</b>
     * </p>
     *
     * @param parent a shell which will be the parent of the new instance
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     */
    public Shell(Shell parent)
    {
        this(parent, SWT.DIALOG_TRIM);
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
     * </p><p>
     * Note: Currently, null can be passed in for the parent.
     * This has the effect of creating the shell on the currently active
     * display if there is one. If there is no current display, the
     * shell is created on a "default" display. <b>Passing in null as
     * the parent is not considered to be good coding style,
     * and may not be supported in a future release of SWT.</b>
     * </p>
     *
     * @param parent a shell which will be the parent of the new instance
     * @param style the style of control to construct
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     *
     * @see SWT#BORDER
     * @see SWT#CLOSE
     * @see SWT#MIN
     * @see SWT#MAX
     * @see SWT#RESIZE
     * @see SWT#TITLE
     * @see SWT#NO_TRIM
     * @see SWT#SHELL_TRIM
     * @see SWT#DIALOG_TRIM
     * @see SWT#MODELESS
     * @see SWT#PRIMARY_MODAL
     * @see SWT#APPLICATION_MODAL
     */
    public Shell(Shell parent, int style)
    {
        this(parent != null ? parent.display : null, parent, style);
    }

    protected static int checkStyle(int style)
    {
        int mask = SWT.SYSTEM_MODAL | SWT.APPLICATION_MODAL | SWT.PRIMARY_MODAL;
        int bits = style & ~mask;
        if ((style & SWT.SYSTEM_MODAL) != 0) return bits | SWT.SYSTEM_MODAL;
        if ((style & SWT.APPLICATION_MODAL) != 0) return bits | SWT.APPLICATION_MODAL;
        if ((style & SWT.PRIMARY_MODAL) != 0) return bits | SWT.PRIMARY_MODAL;
        return bits;
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notified when operations are performed on the receiver,
     * by sending the listener one of the messages defined in the
     * <code>ShellListener</code> interface.
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
     * @see ShellListener
     * @see #removeShellListener
     */
    public void addShellListener(ShellListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Close,typedListener);
        addListener(SWT.Iconify,typedListener);
        addListener(SWT.Deiconify,typedListener);
        addListener(SWT.Activate, typedListener);
        addListener(SWT.Deactivate, typedListener);
    }

    /**
     * Requests that the window manager close the receiver in
     * the same way it would be closed when the user clicks on
     * the "close box" or performs some other platform specific
     * key or mouse combination that indicates the window
     * should be removed.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see SWT#Close
     * @see #dispose
     */
    public void close()
    {
        checkWidget();
        closeWidget();
    }

    public Point computeSize(int wHint, int hHint, boolean changed)
    {
        Point size    = super.computeSize(wHint, hHint, changed);
        Point minSize = OS.Shell_GetMinimumSize(shellHandle);
        if (size.x < minSize.x) size.x = minSize.x;
        if (size.y < minSize.y) size.y = minSize.y;
        return size;
    }

    protected void internal_setShellHandle(int shellHandle)
    {
        this.shellHandle = shellHandle;
        internal_setDecorationsHandle(OS.Shell_DecorationsHandle(shellHandle));
    }

    public void internal_checkShell()
    {
        checkWidget();
    }

    protected void internal_createHandle()
    {
        int parentHandle = (parent!=null) ? ((Shell)parent).shellHandle : 0;
        internal_setShellHandle(OS.Shell_New(this, parentHandle, style));
    }

    public void dispose()
    {
        if (isDisposed())
        {
            return;
        }
        Display oldDisplay = display;
        super.dispose();
        if (oldDisplay != null && !oldDisplay.isDisposed())
        {
            oldDisplay.update();
            if (parent == null)
            {
                oldDisplay.removeShell(this);
            }
        }
    }

    /**
     * Moves the receiver to the top of the drawing order for
     * the display on which it was created (so that all other
     * shells on that display, which are not the receiver's
     * children will be drawn behind it) and forces the window
     * manager to make the shell active.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 2.0
     * @see Control#moveAbove
     * @see Control#setFocus
     * @see Control#setVisible
     * @see Display#getActiveShell
     * @see Decorations#setDefaultButton
     * @see Shell#open
     * @see Shell#setActive
     */
    public void forceActive()
    {
        checkWidget();
        OS.Shell_ForceActive(shellHandle);
    }

    public boolean getEnabled()
    {
        return super.getEnabled();
    }

    /**
     * Returns the receiver's input method editor mode. This
     * will be the result of bitwise OR'ing together one or
     * more of the following constants defined in class
     * <code>SWT</code>:
     * <code>NONE</code>, <code>ROMAN</code>, <code>DBCS</code>,
     * <code>PHONETIC</code>, <code>NATIVE</code>, <code>ALPHA</code>.
     *
     * @return the IME mode
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see SWT
     */
    public int getImeInputMode()
    {
        checkWidget();
        return OS.Shell_GetImeInputMode(shellHandle);
    }

    public Shell getShell()
    {
        checkWidget();
        return this;
    }

    /**
     * Returns an array containing all shells which are
     * descendents of the receiver.
     * <p>
     * @return the dialog shells
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Shell [] getShells()
    {
        checkWidget();
        return OS.Shell_GetShells(shellHandle);
    }

    public boolean isEnabled()
    {
        return super.isEnabled();
    }

    public boolean isVisible()
    {
        checkWidget();
        return getVisible();
    }

    /**
     * Moves the receiver to the top of the drawing order for
     * the display on which it was created (so that all other
     * shells on that display, which are not the receiver's
     * children will be drawn behind it), marks it visible,
     * sets the focus and asks the window manager to make the
     * shell active.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Control#moveAbove
     * @see Control#setFocus
     * @see Control#setVisible
     * @see Display#getActiveShell
     * @see Decorations#setDefaultButton
     * @see Shell#setActive
     * @see Shell#forceActive
     */
    public void open()
    {
        checkWidget();
        setVisible(true);
        setActive();
    }

    protected void internal_releaseResources()
    {
        int size = widgets.size();
        for (int i=0; i<size; ++i)
        {
            // release the registered widgets
            // that take this shell as parent
            Widget widget = (Widget)widgets.elementAt(i);
            widget.internal_release();
        }
        widgets.removeAllElements();
        asyncPaintControls.removeAllElements();
        super.internal_releaseResources();
    }

    /**
     * Removes the listener from the collection of listeners who will
     * be notified when operations are performed on the receiver.
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
     * @see ShellListener
     * @see #addShellListener
     */
    public void removeShellListener(ShellListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook(SWT.Close, listener);
        eventTable.unhook(SWT.Iconify,listener);
        eventTable.unhook(SWT.Deiconify,listener);
        eventTable.unhook(SWT.Activate, listener);
        eventTable.unhook(SWT.Deactivate, listener);
    }

    /**
     * Moves the receiver to the top of the drawing order for
     * the display on which it was created (so that all other
     * shells on that display, which are not the receiver's
     * children will be drawn behind it) and asks the window
     * manager to make the shell active.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 2.0
     * @see Control#moveAbove
     * @see Control#setFocus
     * @see Control#setVisible
     * @see Display#getActiveShell
     * @see Decorations#setDefaultButton
     * @see Shell#open
     * @see Shell#setActive
     */
    public void setActive()
    {
        // There is no difference between force and set active on Symbian platforms
        forceActive();
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

        if (redraw == true)
        {
            Shell shells[] = getDisplay().getShells();

            for (int i = 0; i < shells.length; i ++)
            {
                shells[i].redraw();
            }
        }
    }

    /**
     * Sets the input method editor mode to the argument which
     * should be the result of bitwise OR'ing together one or more
     * of the following constants defined in class <code>SWT</code>:
     * <code>NONE</code>, <code>ROMAN</code>, <code>DBCS</code>,
     * <code>PHONETIC</code>, <code>NATIVE</code>, <code>ALPHA</code>.
     *
     * @param mode the new IME mode
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see SWT
     */
    public void setImeInputMode(int mode)
    {
        checkWidget();
        OS.Shell_SetImeInputMode(shellHandle, mode);
    }

    public int internal_getShellHandle()
    {
        return shellHandle;
    }

    public void internal_registerWidget(Widget widget)
    {
        widgets.addElement(widget);
    }

    public void internal_unregisterWidget(Widget widget)
    {
        widgets.removeElement(widget);
    }

    public Rectangle internal_getDefaultBounds()
    {
        return OS.Shell_GetDefaultBounds(shellHandle);
    }

    public boolean internal_isVisible()
    {
        // Note: this method can be called in a non-UI thread by a GC.
        // It must not check for thread invalid access.

        Display display = this.display;
        if (display == null) internal_error(SWT.ERROR_WIDGET_DISPOSED);
        if ((state & DISPOSED) != 0) internal_error(SWT.ERROR_WIDGET_DISPOSED);

        return OS.Control_GetVisible(this.display.handle, handle);
    }
    
    void setAsyncPainting(Control child, boolean status) 
    {
        if (status)
        {
            if (!asyncPaintControls.contains(child))
            {
                if (asyncPaintControls.isEmpty())
                {
                    OS.Shell_SetAsyncPainting(shellHandle, true);
                }
                asyncPaintControls.addElement(child);
            }
        }
        else
        {
            if (asyncPaintControls.contains(child))
            {
                asyncPaintControls.removeElement(child);
                if (asyncPaintControls.isEmpty())
                {
                    OS.Shell_SetAsyncPainting(shellHandle, false);
                }
            }
        }
    }

    Rectangle paint(Rectangle rect, boolean paintNatively, boolean paintChildren)
    {
        Rectangle res = super.paint(rect, paintNatively, paintChildren);
        if (!display.firstPaintComplete()) {
            display.notifyFirstPaintComplete(parent == null);
        }
        return res;
    }
}
