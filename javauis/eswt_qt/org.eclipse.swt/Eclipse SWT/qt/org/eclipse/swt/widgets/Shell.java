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

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ShellListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.WidgetState;
import org.eclipse.swt.internal.qt.graphics.WindowSurface;

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
 * </p><p>
 * The <em>modality</em> of an instance may be specified using
 * style bits. The modality style bits are used to determine
 * whether input is blocked for other shells on the display.
 * The <code>PRIMARY_MODAL</code> style allows an instance to block
 * input to its parent. The <code>APPLICATION_MODAL</code> style
 * allows an instance to block input to every other shell in the
 * display. The <code>SYSTEM_MODAL</code> style allows an instance
 * to block input to all shells, including shells belonging to
 * different applications.
 * </p><p>
 * Note: The styles supported by this class are treated
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
 * A modality style may also be "downgraded" to a less
 * restrictive style. For example, most operating systems
 * no longer support <code>SYSTEM_MODAL</code> because
 * it can freeze up the desktop, so this is typically
 * downgraded to <code>APPLICATION_MODAL</code>.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>BORDER, CLOSE, MIN, MAX, NO_TRIM, RESIZE, TITLE, ON_TOP, TOOL</dd>
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
public class Shell extends Decorations {
    
/**
 * Handle of the QMainWindow for a top-level Shell.
 * For non top-level Shells this handle is zero.
 */
int mainWindowHandle;

/**
 * Handle of the QWidget that owns the window for a dialog Shell. For non-dialog
 * Shells this handle is zero.
 */
int dialogWindowHandle;

Rectangle defBounds;

/**
 * The WindowSurface wrapper for top level shells. Window surface
 * provides access for Qt created draw surface.
 */
WindowSurface windowSurface;

/**
 * Constructs a new instance of this class. This is equivalent
 * to calling <code>Shell((Display) null)</code>.
 *
 * @exception SWTException <ul>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
 *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
 * </ul>
 */
public Shell () {
    this ((Display) null);
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
public Shell (int style) {
    this ((Display) null, style);
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
public Shell (Display display) {
    this (display, SWT.SHELL_TRIM);
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
public Shell (Display display, int style) {
    this (display, null, style, null);
}

/**
 * <p>
 * <b>IMPORTANT:</b> This constructor is <em>not</em> part of the SWT
 * public API. It should never be referenced from application code. 
 * </p>
 */
protected Shell (Display display, Shell parent, int style, Object packageProxy) {
    super ();
    checkSubclass ();
    setPackageProxy(packageProxy);
    if (display == null) display = Display.getCurrent ();
    if (display == null) display = Display.getDefault ();
    if (!display.isValidThread ()) {
        error (SWT.ERROR_THREAD_INVALID_ACCESS);
    }
    if (parent != null && parent.isDisposed ()) {
        error (SWT.ERROR_INVALID_ARGUMENT); 
    }
    this.style = checkStyle (style, parent);
    this.parent = parent;
    this.display = display;
    createWidget (0);
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
 *    <li>ERROR_INVALID_ARGUMENT - if the parent is disposed</li> 
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
 *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
 * </ul>
 */
public Shell (Shell parent) {
    this (parent, SWT.DIALOG_TRIM);
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
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_INVALID_ARGUMENT - if the parent is disposed</li> 
 * </ul>
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
 * @see SWT#ON_TOP
 * @see SWT#MODELESS
 * @see SWT#PRIMARY_MODAL
 * @see SWT#APPLICATION_MODAL
 * @see SWT#SYSTEM_MODAL
 */
public Shell (Shell parent, int style) {
    this (parent != null ? parent.display : null, parent, style, null);
}

final static int checkStyle (int style, Composite parent) {
    style = Decorations.checkStyle (style, parent);
    style &= ~SWT.TRANSPARENT;
    if(OS.windowServer == OS.WS_SYMBIAN_S60) {
        // Set the RESIZE flag to prevent fixing the size so that the
        // always-maximized top-level Shell will be automatically resized
        // when available space changes. 
        if (parent == null) {
            style |= SWT.RESIZE;
        }
    }
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
public void addShellListener (ShellListener listener) {
    checkWidget();
    if (listener == null) error (SWT.ERROR_NULL_ARGUMENT);
    TypedListener typedListener = new TypedListener (listener);
    addListener (SWT.Close,typedListener);
    addListener (SWT.Iconify,typedListener);
    addListener (SWT.Deiconify,typedListener);
    addListener (SWT.Activate, typedListener);
    addListener (SWT.Deactivate, typedListener);
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
public void close () {
    checkWidget ();
    closeWidget ();
}

public Point computeSize (int wHint, int hHint, boolean changed) {
    // On Symbian the top-level Shells are always maximized. 
    // They have no trim. No need to compute anything. Just return the size. 
    if(OS.windowServer == OS.WS_SYMBIAN_S60 && parent == null) {
        return OS.QWidget_size(topHandle);
    }
    
    Point size = super.computeSize(wHint, hHint, changed);
    
    if(OS.windowServer == OS.WS_X11) {
        if((style & SWT.RESIZE) != 0) {
            // Resizeable windows won't go smaller than this
            if(size.x < 81) size.x = 81;
            if(size.y < 100) size.y = 100;
        } else if(willBeDecorated()) {
            // Windows won't go smaller than their trim
            size.x = Math.max(size.x, windowFrameTrim() * 2);
            size.y = Math.max(size.y, windowFrameTrim() * 2);
        }
    }
    return size;
}

/*
 * On non-Symbian platform the Shell size is initially set to some nice
 * looking default. On Symbian platform Qt implements automatic resizing for
 * maximized widgets. Thus, top-level Shells are always maximized and it's
 * prevented from the application to set their size freely. 
 */
Point setInitialSize () {
    Rectangle area = display.getClientArea();
    if(OS.windowServer != OS.WS_SYMBIAN_S60) {
        area.width /= 3;
        area.height /= 3;
    }
    Point size =  computeSize(area.width, area.height, true);
    size.x -= windowFrameTrim() * 2;
    size.y -= windowTitleTrim() + windowFrameTrim();
    if(OS.windowServer == OS.WS_SYMBIAN_S60 && parent == null) {
        setRestoreState(OS.QT_WINDOWMAXIMIZED, false);
    } else {
        OS.QWidget_resize(topHandle, size.x, size.y);
    }
    return size;
}

public Rectangle computeTrim (int x, int y, int width, int height) {
    checkWidget();
    Rectangle scrollableTrim = super.computeTrim (x, y, width, height);

    // Normally window frame is learned on setVisible(true). If it has not been
    // called then try to learn the window trim dimensions here. 
    learnTrim();
    
    int leftTrim, rightTrim, topTrim, bottomTrim;
    leftTrim = rightTrim = bottomTrim = windowFrameTrim();
    topTrim = windowTitleTrim();
    
    scrollableTrim.x -= leftTrim;
    scrollableTrim.width += (leftTrim + rightTrim);
    scrollableTrim.y -= topTrim;
    scrollableTrim.height += (topTrim + bottomTrim);
    
    return scrollableTrim;
}

boolean bypassWindowManager() {
    return ((style & SWT.ON_TOP) != 0) && ((style & SWT.NO_FOCUS) != 0)
            && ((style & SWT.NO_TRIM) != 0);
}

final int windowFlagsFromStyle() {
    // Bypassing window manager won't work with dialog-flag. If that is desired
    // then window-flag is used. 
    int flags = (parent == null || bypassWindowManager() ? OS.QT_WINDOWFLAGS_WINDOW
            : OS.QT_WINDOWFLAGS_DIALOG);
    flags |= OS.QT_WINDOWFLAGS_CUSTOMIZEWINDOWHINT;
    if((style & SWT.CLOSE) != 0) flags |= OS.QT_WINDOWFLAGS_WINDOWSYSTEMMENUHINT |
                                          OS.QT_WINDOWFLAGS_WINDOWCLOSEBUTTONHINT;
    if((style & SWT.MIN) != 0) flags |= OS.QT_WINDOWFLAGS_WINDOWMINIMIZEBUTTONHINT;
    if((style & SWT.MAX) != 0) flags |= OS.QT_WINDOWFLAGS_WINDOWMAXIMIZEBUTTONHINT;
    if((style & SWT.NO_TRIM) != 0) flags |= OS.QT_WINDOWFLAGS_FRAMELESSWINDOWHINT;
    if((style & SWT.TITLE) != 0) flags |= OS.QT_WINDOWFLAGS_WINDOWTITLEHINT;
    if((style & SWT.ON_TOP) != 0) flags |= OS.QT_WINDOWFLAGS_WINDOWSTAYSONTOPHINT;
    
    // Don't use tooltip flag on Symbian, it causes nothing to be focusable. 
    if(OS.windowServer != OS.WS_SYMBIAN_S60 && bypassWindowManager()) {
        flags |= OS.QT_WINDOWFLAGS_TOOLTIP;
    }
    
    return flags;
}

void createHandle_pp (int index) {
    if(parent == null) {
        // Top-level Shell 
        topHandle = mainWindowHandle = OS.QMainWindow_new(0,  windowFlagsFromStyle());
        frameHandle = scrollAreaHandle = OS.QScrollArea_new(mainWindowHandle);
        handle = OS.QAbstractScrollArea_viewPort(scrollAreaHandle);

        // Scrollarea is the central widget of the QMainWindow. This
        // connects it to the main window's layout. 
        // At least on X11 this call has a side effect that mainWindow can't 
        // be made very small anymore. The minimumSize still returns (0,0) 
        // though. 
        OS.QMainWindow_setCentralWidget(mainWindowHandle, scrollAreaHandle);
    } else {
        // Dialog Shell 
        topHandle = dialogWindowHandle = OS.QDialog_new(parent.handle, windowFlagsFromStyle());
        frameHandle = scrollAreaHandle = OS.QScrollArea_new(dialogWindowHandle);
        handle = OS.QAbstractScrollArea_viewPort(scrollAreaHandle);

        // A QLayout is needed where menubar can be inserted. 
        // Dialog window takes ownership of the layout instance. 
        int layoutHandle = OS.QVBoxLayout_new(dialogWindowHandle);
        OS.QLayout_addWidget(layoutHandle, scrollAreaHandle);
        OS.QLayout_setContentsMargins(layoutHandle, 0, 0, 0, 0);
    }

    // To make getClientArea return correct location already before
    // setVisible(true) move it to it's final position here. 
    OS.QWidget_move(handle, 0, 0);
    
    // If no size is set Qt returns completely weird "pre-initial" values before
    // the Shell is made visible. 
    Point size = setInitialSize();

    defBounds = OS.QWidget_rect(topHandle);
    
    // Set size constraints. See QLayout::SizeConstraint. 
    if((style & SWT.RESIZE) == 0) {
        // Setting min/max sizes to the same value disables the resize handle in the
        // window corner. There's no window style for this purpose. 
        OS.QWidget_setFixedSize(topHandle, size.x, size.y);
    } else {
        // If no-one has set size constraints then Qt will apply a default
        // constraint. Prevent this or the Shell won't go very small. 
        int layoutHandle = OS.QWidget_layout(topHandle);
        if(layoutHandle  != 0) {
            if(OS.QLayout_sizeConstraint(layoutHandle) == OS.QLAYOUT_SIZECONSTRAINT_SETDEFAULTCONSTRAINT)
                OS.QLayout_setSizeConstraint(layoutHandle, OS.QLAYOUT_SIZECONSTRAINT_SETNOCONSTRAINT);
        }
    }
    
    // Shell must not by default accept focus by clicking. 
    int policy = OS.QWidget_focusPolicy(topHandle) & ~OS.QT_FOCUSPOLICY_CLICKFOCUS;
    OS.QWidget_setFocusPolicy(topHandle, policy);

    // Stand-alone Composites, Shells, Canvases set the CANVAS flag
    state |= WidgetState.HANDLE | WidgetState.CANVAS;
}

Composite findDeferredControl () {
    return layoutCount > 0 ? this : null;
}

public boolean isEnabled () {
    checkWidget ();
    return getEnabled ();
}

boolean isParentMirrored() {
    // Shells are positioned in Display coordinate system which is always LTR. 
    return false;
}

public boolean isVisible () {
    checkWidget();
    return getVisible ();
}

void register_pp () {
    // Viewport & scroll area
    super.register_pp ();
    // Main window, if top-level
    if(mainWindowHandle != 0) display.addWidget (mainWindowHandle, this);    
    // Dialog window, if a dialog Shell
    if(dialogWindowHandle != 0) display.addWidget (dialogWindowHandle, this);    
}

void releaseParent_pp () {
    /* Do nothing */
}

/**
 * Returns the receiver's alpha value. The alpha value
 * is between 0 (transparent) and 255 (opaque).
 *
 * @return the alpha value
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 * 
 * @since 3.4
 */
public int getAlpha () {
    checkWidget ();
    double opacity = OS.QWidget_windowOpacity(topHandle);
    return (int)(255*opacity);
}

public boolean getMaximized () {
    checkWidget();
    return !hasState(OS.QT_WINDOWFULLSCREEN) && super.getMaximized ();
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
public int getImeInputMode () {
    checkWidget();
    return SWT.NONE;
}

Shell _getShell () {
    return this;
}

/**
 * Returns an array containing all shells which are 
 * descendants of the receiver.
 * <p>
 * @return the dialog shells
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public Shell [] getShells () {
    checkWidget();
    int count = 0;
    Shell [] shells = display.getShells ();
    for (int i=0; i<shells.length; i++) {
        Control shell = shells [i];
        do {
            shell = shell.getParent ();
        } while (shell != null && shell != this);
        if (shell == this) count++;
    }
    int index = 0;
    Shell [] result = new Shell [count];
    for (int i=0; i<shells.length; i++) {
        Control shell = shells [i];
        do {
            shell = shell.getParent ();
        } while (shell != null && shell != this);
        if (shell == this) {
            result [index++] = shells [i];
        }
    }
    return result;
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
 * @see Decorations#setDefaultButton(Button)
 * @see Shell#setActive
 * @see Shell#forceActive
 */
public void open () {
    checkWidget ();
    setVisible (true);
    bringToTop (false);
    if (isDisposed ()) return;
    if (!restoreFocus () && !traverseGroup (true)) setFocus (OS.QT_OTHERFOCUSREASON);
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
public void removeShellListener (ShellListener listener) {
    checkWidget();
    if (listener == null) error (SWT.ERROR_NULL_ARGUMENT);
    if (eventTable == null) return;
    eventTable.unhook (SWT.Close, listener);
    eventTable.unhook (SWT.Iconify,listener);
    eventTable.unhook (SWT.Deiconify,listener);
    eventTable.unhook (SWT.Activate, listener);
    eventTable.unhook (SWT.Deactivate, listener);
}

/**
 * If the receiver is visible, moves it to the top of the 
 * drawing order for the display on which it was created 
 * (so that all other shells on that display, which are not 
 * the receiver's children will be drawn behind it) and asks 
 * the window manager to make the shell active 
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
 * @see Decorations#setDefaultButton(Button)
 * @see Shell#open
 * @see Shell#setActive
 */
public void setActive () {
    checkWidget ();
    bringToTop (false);
    
    // Wait for active window to change unless the widget isn't visible.
    // Then setActive won't do anything.
    if(OS.windowServer == OS.WS_X11 && OS.QWidget_isVisible(topHandle)) { 
        waitXRequestComplete();
    }
}

/**
 * Sets the receiver's alpha value which must be
 * between 0 (transparent) and 255 (opaque).
 * <p>
 * This operation requires the operating system's advanced
 * widgets subsystem which may not be available on some
 * platforms.
 * </p>
 * @param alpha the alpha value
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 * 
 * @since 3.4
 */
public void setAlpha (int alpha) {
    checkWidget ();
    if(alpha < 0 || alpha > 255) return;
    OS.QWidget_setWindowOpacity(topHandle, ((double)alpha)/255);
}

public Point getSize() {
    checkWidget();
    Point size = OS.QWidget_size(topHandle);
    size.x += windowFrameTrim() * 2;
    size.y += windowFrameTrim() + windowTitleTrim();
    return size;    
}

public Rectangle getBounds() {
    checkWidget();
    Rectangle bounds = OS.QWidget_geometry(topHandle);
    bounds.x -= windowFrameTrim();
    bounds.y -= windowTitleTrim();
    bounds.width += windowFrameTrim() * 2;
    bounds.height += windowFrameTrim() + windowTitleTrim();
    return bounds;
}

int setBounds (int x, int y, int width, int height, boolean move, boolean resize) {
    // On Symbian platform top-level Shells are forced to maximized
    if(OS.windowServer == OS.WS_SYMBIAN_S60 && parent == null) {
        return 0;
    }
    
    if (hasState(OS.QT_WINDOWFULLSCREEN)) {
        setRestoreState(OS.QT_WINDOWFULLSCREEN, true);
    }

    Rectangle geometry = OS.QWidget_geometry(topHandle);
    int result = 0;
    if(move) {
        int oldX = geometry.x - windowFrameTrim();
        int oldY = geometry.y - windowTitleTrim();
        if(oldX != x || oldY != y) {
            result |= WidgetState.MOVED;
        }
    }
    if(resize) {
        int oldW = geometry.width + windowFrameTrim() * 2;
        int oldH = geometry.height + windowTitleTrim() + windowFrameTrim();
        if(oldW != width || oldH != height) {
            result |= WidgetState.RESIZED;
        }
    }
    if((result & WidgetState.MOVED) != 0) {
        int newX = x + windowFrameTrim();
        int newY = y + windowTitleTrim();
        OS.QWidget_setGeometry(topHandle, newX, newY, geometry.width, geometry.height);
    }
    if((result & WidgetState.RESIZED) != 0) {
        int newW = width - windowFrameTrim() * 2;
        int newH = height - (windowFrameTrim() + windowTitleTrim());
        
        // If size has been fixed it needs to be temporarily unfixed
        if((style & SWT.RESIZE) == 0) unlockSize();
        
        OS.QWidget_resize(topHandle, newW, newH);

        // Fix the new size if not resizeable
        if((style & SWT.RESIZE) == 0) {
            OS.QWidget_setFixedSize(topHandle, Math.max(0, newW), 
                    Math.max(0, newH));
        }
    }
    return result;
}

public Point getLocation() {
    checkWidget();
    Rectangle geometry = OS.QWidget_geometry(topHandle);
    Point pos = new Point(geometry.x, geometry.y);
    pos.x -= windowFrameTrim();
    pos.y -= windowTitleTrim();
    return pos;
}

public void setEnabled (boolean enabled) {
    checkWidget ();
    if (((state & WidgetState.DISABLED) == 0) == enabled) return;
    super.setEnabled (enabled);
    if (enabled && topHandle == OS.QApplication_activeWindow()) {
        if (!restoreFocus ()) traverseGroup (true);
    }
}

void enableWidget(boolean enabled) {
    Control oldFocus = display._getFocusControl();
    super.enableWidget(enabled);
    if(!enabled) {
        // All children were looped through and disabled by QWidget unless they
        // were explicitly disabled. SWT behavior is not to disable the dialog
        // Shells so we have to restore their states.
        Shell shells[] = getShells();
        for(int i = 0; i < shells.length; ++i) {
            if(shells[i].parent == this) {
                if(shells[i]._getEnabled()) {
                    OS.QWidget_setEnabled(shells[i].topHandle, true);
                }
            }
        }
        // Because dialog Shell might have been temporarily disabled the focus might
        // have been lost and must be restored. 
        if(oldFocus != null) oldFocus.menuShell().restoreFocus();
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
public void setImeInputMode (int mode) {
    checkWidget();
}

private final int modalityFromStyle() {
    int modality = OS.QT_WINDOWMODALITY_NONMODAL;
    if((style & SWT.SYSTEM_MODAL) != 0)
        return OS.QT_WINDOWMODALITY_APPLICATIONMODAL;
    if((style & SWT.APPLICATION_MODAL) != 0)
        return OS.QT_WINDOWMODALITY_APPLICATIONMODAL;
    if((style & SWT.PRIMARY_MODAL) != 0) {
        return OS.QT_WINDOWMODALITY_APPLICATIONMODAL;
    }
    return modality;
}

public void setVisible (boolean visible) {
    checkWidget();
    
   int mask = SWT.PRIMARY_MODAL | SWT.APPLICATION_MODAL | SWT.SYSTEM_MODAL;
    if ((style & mask) != 0) {
        if (visible) {
            display.setModalShell (this);
            if ((style & (SWT.APPLICATION_MODAL | SWT.SYSTEM_MODAL)) != 0) {
                display.setModalDialog (null);
            }
            Control control = display._getFocusControl();
            if (control != null && !control.isActive()) {
                bringToTop(false);
                if (isDisposed()) return;
            }
            OS.QWidget_setWindowModality(topHandle, modalityFromStyle());
            int mouseGrabber = OS.QWidget_mouseGrabber();
            if(mouseGrabber != 0) {
                OS.QWidget_releaseMouse(mouseGrabber);
            }
        } else {
            display.clearModal (this);
        }
    } else {
        updateModal ();
    }
    
    // This shell
    super.setVisible(visible);
    if(isDisposed()) return;
        
    // Dialog shells. 
    // SWT behavior is that when a Shell gets setVisible(false) then dialog Shells
    // after that have getVisible == isVisible == false.  
    Shell dialogShells[] = getShells();
    for(int i = 0; i < dialogShells.length; ++i) {
        Shell child = dialogShells[i];
        if(child.isVisible() != visible) {
            child.setVisible(visible);
            if(isDisposed()) return;
        }
    }

    // Ensure that the state is updated
    if(OS.windowServer == OS.WS_X11) {
        waitXRequestComplete();
    }
    if(isDisposed()) return;    
    learnTrim();
}

boolean traverseEscape () {
    if (parent == null) return false;
    if (!isVisible () || !isEnabled ()) return false;
    close ();
    return true;
}

void updateModal () {
    // Nothing to do, the method is here so that modality logic can be similar
    // across the platforms
}

void deregister_pp () {
    super.deregister_pp ();
    if(mainWindowHandle != 0) {
        display.removeWidget(mainWindowHandle);
    }
    if(dialogWindowHandle != 0) {
        display.removeWidget(dialogWindowHandle);
    }
}

/**
 * If the receiver is visible, moves it to the top of the 
 * drawing order for the display on which it was created 
 * (so that all other shells on that display, which are not 
 * the receiver's children will be drawn behind it) and forces 
 * the window manager to make the shell active.
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
 * @see Decorations#setDefaultButton(Button)
 * @see Shell#open
 * @see Shell#setActive
 */
public void forceActive () {
    checkWidget ();
    bringToTop (true);
}

public void moveAbove (Control control) {
    if(parent == null) {
        // There's no way to move windows above other windows in Qt.
        // Therefore here it's raised to the top despite of the control
        // parameter.  
        // Also this sends the window on top of ALL windows including the
        // windows of other applications.
        OS.QWidget_raise(topHandle);
    } else {
        super.moveAbove(control);
    }
}

public void moveBelow (Control control) {
    if(parent == null) {
        // There's no way to move windows below other windows in Qt.
        // Therefore here it's lowered to the bottom despite of the control
        // parameter. 
        // Also this sends the window below ALL the windows including the
        // windows of other applications.
        OS.QWidget_lower(topHandle);
    } else {
        super.moveBelow(control);
    }
}

void releaseHandle_pp () {
    mainWindowHandle = 0;
    dialogWindowHandle = 0;
    super.releaseHandle_pp ();
}

void releaseChildren_pp (boolean destroy) {
    Shell [] shells = getShells ();
    for (int i=0; i<shells.length; i++) {
        Shell shell = shells [i];
        if (shell != null && !shell.isDisposed ()) {
            shell.release(true);
        }
    }
    super.releaseChildren_pp (destroy);
}

void releaseWidget_pp () {
    super.releaseWidget_pp ();
    display.clearModal (this);
    if(windowSurface != null) {
    	windowSurface.dispose();
    	windowSurface = null;
    }
}

boolean qt_event_keypress_pp(int widgetHandle, int key,
        int modifier, int character, int nativeScanCode) {
    super.qt_event_keypress_pp(widgetHandle, key, modifier, character, nativeScanCode);
    // Key event is canceled to prevent propagation from viewport to its parent
    return true;
}

boolean qt_event_keyrelease_pp( int widgetHandle, int key, int modifier, int character, int nativeScanCode )
{
    super.qt_event_keyrelease_pp(widgetHandle, key, modifier, character, nativeScanCode);
    // Key event is canceled to prevent propagation from viewport to its parent
    return true;
}

public Rectangle internal_getDefaultBounds() {
    return defBounds;
}

WindowSurface getWindowSurface() {
	checkWidget();
	// Only top level shells have window surface
	if (parent != null) {
		return null;
	}
	if(windowSurface == null) {
		windowSurface = new WindowSurface(this, true);
	}
	return windowSurface;
}

}
