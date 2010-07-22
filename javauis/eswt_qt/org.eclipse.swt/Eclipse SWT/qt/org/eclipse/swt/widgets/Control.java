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

import java.util.Vector;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.events.ControlListener;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.events.TraverseListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Drawable;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Internal_GfxPackageSupport;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.CommandUtils;
import org.eclipse.swt.internal.qt.GCData;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.QObjectDeleteWrapper;
import org.eclipse.swt.internal.qt.SharedGCWrapper;
import org.eclipse.swt.internal.qt.WidgetConstant;
import org.eclipse.swt.internal.qt.WidgetState;
import org.eclipse.swt.internal.qt.graphics.NativeCommandBuffer;
import org.eclipse.swt.internal.qt.graphics.GraphicsContext;

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
public abstract class Control extends Widget implements Drawable {
    Composite parent;
    int frameHandle, extraStyle;
    Menu menu;
    Font font;
    boolean toolTipTextExists;
    Object layoutData;
    Color foreground, background;
    Image backgroundImage;
    private Command[] commandList;

    // Flags controlling traverse key event handling
    boolean traverseDoit, traverseCancel;

    // Each external GC has its own buffer for now.
    // Not possible to bind several GCs to one buffer.
    Vector/*<GraphicsContext>*/ bufferedGcs;
    Vector/*<Buffer>*/ gcBuffers;

    private byte isPainting;
    boolean bufferFlush;

Control() {
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
public Control(Composite parent, int style) {
    this(parent, style, 0, null, false);
}

/**
 * <p>
 * <b>IMPORTANT:</b> This constructor is <em>not</em> part of the SWT
 * public API. It should never be referenced from application code.
 * </p>
 */
protected Control(Composite parent, int style, int extraStyle, Object packageProxy,
        boolean isExtended) {
    super(parent, style, extraStyle, packageProxy, isExtended);
    this.parent = parent;
    this.extraStyle = extraStyle;
    createWidget(0);
    OS.QWidget_setParent(topHandle, parent.handle);
    OS.QWidget_resize(topHandle, 0, 0);
    if(isParentMirrored()) {
        Point size = OS.QWidget_size(topHandle);
        OS.QWidget_move(topHandle, parent.getClientWidth() - size.x, 0);
    }
    OS.QWidget_setVisible(topHandle, true);
    if(this.parent.packageProxy != null) {
        this.parent.packageProxy.addControl(this);
    } else {
        this.parent.addControl_pp(this);
    }
}

void addCommand(final Command command) {
    if (command.control != this)
        return;
    if (commandList == null) {
        commandList = new Command[1];
        commandList[0] = command;
        return;
    }
    int size = commandList.length;

    for (int i = 0; i < size; i++) {
        if (commandList[i] == command)
            return;
    }
    Command[] newList = new Command[size + 1];
    System.arraycopy(commandList, 0, newList, 0, size);
    newList[size] = command;
    commandList = newList;
}

/**
 * Adds the listener to the collection of listeners who will be notified when
 * the control is moved or resized, by sending it one of the messages defined in
 * the <code>ControlListener</code> interface.
 *
 * @param listener
 *            the listener which should be notified
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed
 *                </li> <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see ControlListener
 * @see #removeControlListener
 */
public void addControlListener(ControlListener listener) {
    checkWidget();
    if (listener == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    TypedListener typedListener = new TypedListener(listener);
    addListener(SWT.Resize, typedListener);
    addListener(SWT.Move, typedListener);
}

/**
 * Adds the listener to the collection of listeners who will be notified when
 * the control gains or loses focus, by sending it one of the messages defined
 * in the <code>FocusListener</code> interface.
 *
 * @param listener
 *            the listener which should be notified
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed
 *                </li> <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see FocusListener
 * @see #removeFocusListener
 */
public void addFocusListener(FocusListener listener) {
    checkWidget();
    if (listener == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    TypedListener typedListener = new TypedListener(listener);
    addListener(SWT.FocusIn, typedListener);
    addListener(SWT.FocusOut, typedListener);
}

/**
 * Adds the listener to the collection of listeners who will be notified when
 * keys are pressed and released on the system keyboard, by sending it one of
 * the messages defined in the <code>KeyListener</code> interface.
 * <p>
 * When a key listener is added to a control, the control will take part in
 * widget traversal. By default, all traversal keys (such as the tab key and so
 * on) are delivered to the control. In order for a control to take part in
 * traversal, it should listen for traversal events. Otherwise, the user can
 * traverse into a control but not out. Note that native controls such as table
 * and tree implement key traversal in the operating system. It is not necessary
 * to add traversal listeners for these controls, unless you want to override
 * the default traversal.
 * </p>
 *
 * @param listener
 *            the listener which should be notified
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see KeyListener
 * @see #removeKeyListener
 */
public void addKeyListener(KeyListener listener) {
    checkWidget();
    if (listener == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    TypedListener typedListener = new TypedListener(listener);
    addListener(SWT.KeyUp, typedListener);
    addListener(SWT.KeyDown, typedListener);
}

/**
 * Adds the listener to the collection of listeners who will be notified when
 * mouse buttons are pressed and released, by sending it one of the messages
 * defined in the <code>MouseListener</code> interface.
 *
 * @param listener
 *            the listener which should be notified
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see MouseListener
 * @see #removeMouseListener
 */
public void addMouseListener(MouseListener listener) {
    checkWidget();
    if (listener == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    TypedListener typedListener = new TypedListener(listener);
    addListener(SWT.MouseDown, typedListener);
    addListener(SWT.MouseUp, typedListener);
    addListener(SWT.MouseDoubleClick, typedListener);
}

/**
 * Adds the listener to the collection of listeners who will be notified when
 * the mouse moves, by sending it one of the messages defined in the
 * <code>MouseMoveListener</code> interface.
 *
 * @param listener
 *            the listener which should be notified
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see MouseMoveListener
 * @see #removeMouseMoveListener
 */
public void addMouseMoveListener(MouseMoveListener listener) {
    checkWidget();
    if (listener == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    TypedListener typedListener = new TypedListener(listener);
    addListener(SWT.MouseMove, typedListener);
}

/**
 * Adds the listener to the collection of listeners who will be notified when
 * the receiver needs to be painted, by sending it one of the messages defined
 * in the <code>PaintListener</code> interface.
 *
 * @param listener
 *            the listener which should be notified
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see PaintListener
 * @see #removePaintListener
 */
public void addPaintListener(PaintListener listener) {
    checkWidget();
    if (listener == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    TypedListener typedListener = new TypedListener(listener);
    addListener(SWT.Paint, typedListener);
}

/**
 * Adds the listener to the collection of listeners who will be notified when
 * traversal events occur, by sending it one of the messages defined in the
 * <code>TraverseListener</code> interface.
 *
 * @param listener
 *            the listener which should be notified
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see TraverseListener
 * @see #removeTraverseListener
 */
public void addTraverseListener(TraverseListener listener) {
    checkWidget();
    if (listener == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    TypedListener typedListener = new TypedListener(listener);
    addListener(SWT.Traverse, typedListener);
}

//This is called with the background control's background color
void applyBackgroundColor(Color color) {
    // Get a temporary copy of the widget's palette, modify the background
    // color, and set the palette back to the widget.
    int palette = OS.QWidget_swt_palette_new(topHandle);
    try {
        setPaletteBgColor(palette, color);
        OS.QWidget_setPalette(topHandle, palette);
    } finally {
        OS.QPalette_delete(palette);
    }
    updateAutoFillBackground();
}

//This is called with the background control's background image
void applyBackgroundImage(Image image) {
    // Get a temporary copy of the widget's palette, modify the background
    // texture, and set the palette back to the widget.
    int palette = OS.QWidget_swt_palette_new(topHandle);
    try {
        setPaletteBgImage(palette, image);
        OS.QWidget_setPalette(topHandle, palette);
    } finally {
        OS.QPalette_delete(palette);
    }
    updateAutoFillBackground();
}

/*
 * Some widgets need to know when background texture is taken into use. They can
 * override this to hook in. E.g. scrolling can't be done but whole area needs
 * repainting if there's a non-single color background in use.
 */
void backgroundImageApplied_pp(Image image) {
}

/*
 * Some widgets need to know when background texture of parent taken into use.
 * They can override this to hook in. E.g. Combo can inherit texture only for
 * some sub-widgets and for some it needs to draw the background image by
 * itself.
 */
void backgroundImageInherited(Image image) {
}

/*
 * Controls that return true here will use null brush for their background
 * role(s) when inheriting a background image. This means they don't draw the
 * background but depend on the parent to draw it. This doesn't work well with
 * some built-in widgets and they will return false here. When this returns
 * false then the background image drawing will be handled by the control itself
 * and the image will appear starting from the control's own origin instead of
 * the parent's.
 */
boolean canInheritBackgroundImage() {
    return true;
}

void checkBackground () {
    Shell shell = getShell ();
    if (this == shell) return;
    state &= ~WidgetState.PARENT_BACKGROUND;
    Composite composite = parent;
    do {
        int mode = composite.backgroundMode;
        if (mode != SWT.INHERIT_NONE) {
            // On some platforms INHERIT_DEFAULT depends on the
            // THEME_BACKGROUND. With Qt we don't have any such criteria
            // but INHERIT_DEFAULT and INHERIT_FORCE have the same effect.
            state |= WidgetState.PARENT_BACKGROUND;
            return;
        }
        if (composite == shell) break;
        composite = composite.parent;
    } while (true);
}

void checkBorder_pp() {
    if (frameHandle != 0) {
        if ((style & SWT.BORDER) == 0) {
            OS.QFrame_setFrameStyle(frameHandle, OS.QT_NOFRAME);
        } else {
            int qStyle = OS.QFrame_frameShape(frameHandle);
            if ((qStyle & (OS.QT_FRAMEPANEL | OS.QT_FRAMEWINPANEL
                    | OS.QT_FRAMESTYLEDPANEL | OS.QT_FRAMEBOX)) == 0) {
                OS.QFrame_setFrameStyle(frameHandle, qStyle
                        | OS.QT_FRAMESTYLEDPANEL);
            }
        }
    }

    // a control has to manage the border by itself if it can't provide a frame
    // handle .

    if (getBorderWidth() == 0)
        // remove border flag if there is no border
        style &= ~SWT.BORDER;
}

void checkBuffered() {
    style &= ~SWT.DOUBLE_BUFFERED;
}

void checkMirrored() {
    if ((style & SWT.RIGHT_TO_LEFT) != 0) {
        style |= SWT.MIRRORED;
    } else {
        style &= ~SWT.MIRRORED;
    }
}

void checkNoBackground() {
    style &= ~SWT.NO_BACKGROUND;
}

void checkNoFocus() {
// Nothing to be done for Control by default. Override case by case.
}

void checkNoRedrawResize() {
    style &= ~SWT.NO_REDRAW_RESIZE;
}

/**
 * Returns the preferred size of the receiver.
 * <p>
 * The <em>preferred size</em> of a control is the size that it would best be
 * displayed at. The width hint and height hint arguments allow the caller to
 * ask a control questions such as "Given a particular width, how high does the
 * control need to be to show all of the contents?" To indicate that the caller
 * does not wish to constrain a particular dimension, the constant
 * <code>SWT.DEFAULT</code> is passed for the hint.
 * </p>
 *
 * @param wHint
 *            the width hint (can be <code>SWT.DEFAULT</code>)
 * @param hHint
 *            the height hint (can be <code>SWT.DEFAULT</code>)
 * @return the preferred size of the control
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see Layout
 * @see #getBorderWidth
 * @see #getBounds
 * @see #getSize
 * @see #pack(boolean)
 * @see "computeTrim, getClientArea for controls that implement them"
 */
public Point computeSize(int wHint, int hHint) {
    return computeSize(wHint, hHint, true);
}

/**
 * Returns the preferred size of the receiver.
 * <p>
 * The <em>preferred size</em> of a control is the size that it would best be
 * displayed at. The width hint and height hint arguments allow the caller to
 * ask a control questions such as "Given a particular width, how high does the
 * control need to be to show all of the contents?" To indicate that the caller
 * does not wish to constrain a particular dimension, the constant
 * <code>SWT.DEFAULT</code> is passed for the hint.
 * </p>
 * <p>
 * If the changed flag is <code>true</code>, it indicates that the receiver's
 * <em>contents</em> have changed, therefore any caches that a layout manager
 * containing the control may have been keeping need to be flushed. When the
 * control is resized, the changed flag will be <code>false</code>, so layout
 * manager caches can be retained.
 * </p>
 *
 * @param wHint
 *            the width hint (can be <code>SWT.DEFAULT</code>)
 * @param hHint
 *            the height hint (can be <code>SWT.DEFAULT</code>)
 * @param changed
 *            <code>true</code> if the control's contents have changed, and
 *            <code>false</code> otherwise
 * @return the preferred size of the control.
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see Layout
 * @see #getBorderWidth
 * @see #getBounds
 * @see #getSize
 * @see #pack(boolean)
 * @see "computeTrim, getClientArea for controls that implement them"
 */
public Point computeSize(int wHint, int hHint, boolean changed) {
    checkWidget();

    if (changed) {
        OS.QWidget_updateGeometry(topHandle);
    }

    // The border, if enabled, is already included in the native size hint
    Point res = OS.QWidget_sizeHint(topHandle);
    if (res.x < 0) {
        res.x = WidgetConstant.DEFAULT_WIDTH;
    }
    if (res.y < 0) {
        res.y = WidgetConstant.DEFAULT_HEIGHT;
    }

    int border = getBorderWidth();
    if (wHint != SWT.DEFAULT) {
        res.x = wHint + 2 * border;
    }
    if (hHint != SWT.DEFAULT) {
        res.y = hHint + 2 * border;
    }

    return res;
}

Control computeTabGroup() {
    if (isTabGroup())
        return this;
    return parent.computeTabGroup();
}

Control[] computeTabList() {
    if (isTabGroup()) {
        if (getVisible() && getEnabled()) {
            return new Control[] { this };
        }
    }
    return new Control[0];
}

Control computeTabRoot() {
    Control[] tabList = parent._getTabList();
    if (tabList != null) {
        int index = 0;
        while (index < tabList.length) {
            if (tabList[index] == this)
                break;
            index++;
        }
        if (index == tabList.length) {
            if (isTabGroup())
                return this;
        }
    }
    return parent.computeTabRoot();
}

void createWidget(int index) {
    checkOrientation(parent);
    super.createWidget(index);
    checkBuffered();
    checkMirrored();
    if(packageProxy != null) {
        packageProxy.checkBorder();
    } else {
        checkBorder_pp();
    }
    checkNoBackground();
    checkNoRedrawResize();
    checkNoFocus();
    checkBackground();
    if ((style & SWT.RIGHT_TO_LEFT) != 0){
        OS.QWidget_setLayoutDirection(topHandle, OS.QT_RIGHTTOLEFT);
    } else {
        OS.QWidget_setLayoutDirection(topHandle, OS.QT_LEFTTORIGHT);
    }
    // This creates a custom palette for the control
    if(packageProxy != null) {
        packageProxy.setBackground();
    } else {
        setBackground_pp();
    }
}

boolean doHandleMouseEvent(int type, int widgetHandle, int button, int x,
        int y, int state, int buttons) {
    if (!isEnabled()) {
        // In SWT parent is supposed to receive events of a disabled
        // control.
        // Many Qt widgets consume the events when they are disabled.
        // To get SWT behavior, a new event is sent to the parent and the
        // native event is cancelled.
        mouseToParent(type, widgetHandle, button, x, y, state, buttons);
        return true;
    }
    Point p = mapToClientArea(widgetHandle, x, y);
    return sendMouseEvent(type, button, false, p.x, p.y, state, buttons);
}

/*
 * Processes a mnemonic hit. Called on the focused control when some control
 * gets a shortcut event. Focused control is then sent traversal/key events.
 */
boolean doProcessShortCut(int key, int modifier, int character) {
    // Send traverse/key event
    Event event = sendTraverseEvent(SWT.TRAVERSE_MNEMONIC, key, modifier,
            character);
    if (isDisposed())
        return true;
    if (event.doit) {
        // Traversal is done respective to the control which has the
        // mnemonic.
        if (doTraversal(event)) {
            return true; // Cancel the shortcut event
        }
    }
    sendKeyEvent(SWT.KeyDown, key, modifier, character, 0);

    return true; // Cancel the shortcut event
}

void doRedraw() {
    OS.QWidget_update(handle);
}

void doRedraw(int x, int y, int width, int height) {
    OS.QWidget_update(handle, x, y, width, height);
}

boolean doTraversal(Event event) {
    if (!event.doit)
        return false;
    switch (event.detail) {
    case SWT.TRAVERSE_NONE:
        return true;
    case SWT.TRAVERSE_ESCAPE:
        return traverseEscape();
    case SWT.TRAVERSE_RETURN:
        return traverseReturn();
    case SWT.TRAVERSE_TAB_NEXT:
        return traverseGroup(true);
    case SWT.TRAVERSE_TAB_PREVIOUS:
        return traverseGroup(false);
    case SWT.TRAVERSE_ARROW_NEXT:
        return traverseItem(true);
    case SWT.TRAVERSE_ARROW_PREVIOUS:
        return traverseItem(false);
    case SWT.TRAVERSE_MNEMONIC:
        return traverseMnemonic(event.character);
    case SWT.TRAVERSE_PAGE_NEXT:
        return traversePage(true);
    case SWT.TRAVERSE_PAGE_PREVIOUS:
        return traversePage(false);
    }
    return false;
}

void enableWidget(boolean enabled) {
    // Disabling a widget implicitly disables all its children. Enabling
    // respectively enables all child widgets unless they have been explicitly
    // disabled.
    OS.QWidget_setEnabled(topHandle, enabled);
}

Control findBackgroundControl () {
    if (background != null || backgroundImage != null) return this;
    return (state & WidgetState.PARENT_BACKGROUND) != 0 ? parent.findBackgroundControl () : null;
}

Menu[] findMenus(Control control) {
    if (menu != null && this != control)
        return new Menu[] { menu };
    return new Menu[0];
}

/*
 * Finds the next control in the parent's child array.
 *
 * @param loopOver
 *            If this control is the last one, should the search loop over and
 *            return the first control as the next one.
 * @return The next control with the same parent or null.
 */
Control findNextControl(boolean loopOver) {
    Composite parent = getParent();
    if (parent == null)
        return null;
    Control nextChild = null;
    boolean next = false;
    Control[] children = parent._getChildren();
    for (int i = 0; i < children.length; i++) {
        Control child = children[i];
        if (!next) {
            if (child.equals(this)) {
                next = true;
            }
        } else {
            nextChild = children[i];
        }
    }
    if (nextChild == null && loopOver && children.length > 0
            && children[0] != this) {
        nextChild = children[0];
    }
    return nextChild;
}

void fixFocus (Control focusControl) {
    Shell shell = getShell ();
    Control control = this;
    while (control != shell && (control = control.parent) != null) {
        if (control.setFixedFocus ()) return;
    }
    shell.setSavedFocus (focusControl);

    // Set the focus to desktop as the last resort
    int desktopWidget = OS.QApplication_desktop();
    if(desktopWidget != 0) {
        OS.QWidget_setFocus(desktopWidget, OS.QT_OTHERFOCUSREASON);
    }
}

void flushBuffers() {
    if (bufferedGcs != null) {
        for (int i = 0; i < bufferedGcs.size(); i++) {
            GraphicsContext igc = (GraphicsContext)bufferedGcs.elementAt(i);
            NativeCommandBuffer buf = (NativeCommandBuffer)gcBuffers.elementAt(i);
            igc.releaseTarget();
            igc.bindTarget(this);
            igc.render(buf);
            igc.releaseTarget();
            igc.bindTarget(buf, this);
        }
    }
}

/**
 * Forces the receiver to have the <em>keyboard focus</em>, causing all keyboard
 * events to be delivered to it.
 *
 * @return <code>true</code> if the control got focus, and <code>false</code> if
 *         it was unable to.
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see #setFocus
 */
public boolean forceFocus() {
    checkWidget();
    return forceFocus(OS.QT_OTHERFOCUSREASON);
}

boolean forceFocus(int focusReason) {
    if (Display.focusEvent == SWT.FocusOut) return false;
    Decorations shell = menuShell();
    shell.setSavedFocus(this);
    if (!isEnabled() || !isVisible() || !isActive())
        return false;
    if (isFocusControl())
        return true;
    shell.setSavedFocus(null);
    shell.bringToTop(false);

    // Wait for requests sent in bringToTop to complete. This doesn't guarantee
    // that focus has moved after this.
    if (OS.windowServer == OS.WS_X11) {
        waitXRequestComplete();
        if (isDisposed())
            return false;
    }

    // Gives focus to a widget regardless of its focus policy.
    // If widget is hidden then it'll get focus when it's shown.
    OS.QWidget_setFocus(handle, focusReason);

    if (isDisposed())
        return false;
    shell.setSavedFocus(this);

    // Because focus transition cannot be guaranteed to have completed a fixed
    // return value needs to be returned for consistent focus behavior.
    if (OS.windowServer == OS.WS_X11)
        return true;

    return (packageProxy != null ? packageProxy._isFocusControl() : _isFocusControl_pp());
}

/**
 * Returns the receiver's background color.
 *
 * @return the background color
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public Color getBackground() {
    checkWidget();
    Control control = findBackgroundControl ();
    if (control == null) control = this;
    if (control.background != null) {
        // Return a copy
        return Internal_GfxPackageSupport.newColor(display, control.background.handle);
    }
    int palette = 0;
    int[] rgb;
    try {
        palette = OS.QWidget_swt_palette_new(topHandle);
        rgb = OS.QPalette_color(palette,
                OS.QPALETTE_COLORGROUP_NORMAL, getBackgroundColorRole());
    } finally {
        OS.QPalette_delete(palette);
    }
    return Internal_GfxPackageSupport.newColor(display, (rgb[0] & 0xFF) | ((rgb[1] & 0xFF) << 8) | ((rgb[2] & 0xFF) << 16));
}

int getBackgroundColorRole() {
    return OS.QPALETTE_COLORROLE_WINDOW;
}

/**
 * Returns the receiver's background image.
 *
 * @return the background image
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @since 3.2
 */
public Image getBackgroundImage () {
    checkWidget ();
    Control control = findBackgroundControl ();
    if (control == null) control = this;
    return control.backgroundImage;
}

int[] getBackgroundColorRoles() {
    return new int[] { OS.QPALETTE_COLORROLE_WINDOW,
            OS.QPALETTE_COLORROLE_BASE, OS.QPALETTE_COLORROLE_BUTTON };
}

int[] getBackgroundImageRoles() {
    // For some reason button color role becomes black if pixmap is set to it.
    // At least with Cleanlooks style this happened.
    return new int[] { OS.QPALETTE_COLORROLE_WINDOW,
            OS.QPALETTE_COLORROLE_BASE/*, OS.QPALETTE_COLORROLE_BUTTON */};
}

/**
 * Returns the receiver's border width.
 *
 * @return the border width
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public int getBorderWidth() {
    checkWidget();
    if (frameHandle != 0) {
        return OS.QFrame_frameWidth(frameHandle);

    }
    // a control should overwrite this method if it has border but a frame
    // handle.
    return 0;
}

/**
 * Returns a rectangle describing the receiver's size and location relative to
 * its parent (or its display if its parent is null), unless the receiver is a
 * shell. In this case, the location is relative to the display.
 *
 * @return the receiver's bounding rectangle
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public Rectangle getBounds() {
    checkWidget();
    // This can return
    // - display relative main window coordinates (if top-level shell)
    // - parent's viewport relative coordinates
    Rectangle bounds = OS.QWidget_frameGeometry(topHandle);
    if (isParentMirrored()) {
        bounds.x = parent.getClientWidth() - bounds.width - bounds.x;
    }
    return bounds;
}

int getClientWidth() {
    return OS.QWidget_rect(handle).width;
}

Command[] getCommands() {
    return commandList == null ? new Command[0] : commandList;
}

/**
 * Returns <code>true</code> if the receiver is enabled, and <code>false</code>
 * otherwise. A disabled control is typically not selectable from the user
 * interface and draws with an inactive or "grayed" look.
 *
 * @return the receiver's enabled state
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see #isEnabled
 */
public boolean getEnabled() {
    checkWidget();
    return _getEnabled();
}

boolean _getEnabled() {
    return (state & WidgetState.DISABLED) == 0;
}

/**
 * Returns the font that the receiver will use to paint textual information.
 *
 * @return the receiver's font
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public Font getFont() {
    checkWidget();
    return font != null ? font : Internal_GfxPackageSupport.newFont(display, OS.SwtFontCache_cache(OS.QWidget_swt_font_new(topHandle)));
}

/**
 * Returns the foreground color that the receiver will use to draw.
 *
 * @return the receiver's foreground color
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public Color getForeground() {
    checkWidget();
    if (foreground != null) {
        return Internal_GfxPackageSupport.newColor(display, foreground.handle);
    }
    int paletteHandle = 0;
    int[] rgb;
    try {
        paletteHandle = OS.QWidget_swt_palette_new(handle);
        rgb = OS.QPalette_color(paletteHandle,
                OS.QPALETTE_COLORGROUP_NORMAL, getForegroundColorRole());
    } finally {
        OS.QPalette_delete(paletteHandle);
    }
    foreground = Internal_GfxPackageSupport.newColor(display, (rgb[0] & 0xFF) | ((rgb[1] & 0xFF) << 8) | ((rgb[2] & 0xFF) << 16));
    return Internal_GfxPackageSupport.newColor(display, foreground.handle);
}

int getForegroundColorRole() {
    return OS.QPALETTE_COLORROLE_WINDOWTEXT;
}

/**
 * Returns layout data which is associated with the receiver.
 *
 * @return the receiver's layout data
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public Object getLayoutData() {
    checkWidget();
    return layoutData;
}

/**
 * Returns a point describing the receiver's location relative to its parent (or
 * its display if its parent is null), unless the receiver is a shell. In this
 * case, the point is relative to the display.
 *
 * @return the receiver's location
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public Point getLocation() {
    checkWidget();
    Point p = OS.QWidget_pos(topHandle);
    if (isParentMirrored()) {
        p.x = parent.getClientWidth() - p.x - OS.QWidget_geometry(topHandle).width;
    }
    return p;
}

/**
 * Returns the receiver's pop up menu if it has one, or null if it does not. All
 * controls may optionally have a pop up menu that is displayed when the user
 * requests one for the control. The sequence of key strokes, button presses
 * and/or button releases that are used to request a pop up menu is platform
 * specific.
 *
 * @return the receiver's menu
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public Menu getMenu() {
    checkWidget();
    return menu;
}

/**
 * Returns the receiver's parent, which must be a <code>Composite</code> or null
 * when the receiver is a shell that was created with null or a display for a
 * parent.
 *
 * @return the receiver's parent
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public Composite getParent() {
    checkWidget();
    return parent;
}

int getParentClientWidth() {
    if(parent != null) {
        return parent.getClientWidth();
    } else {
        return display.getClientArea().width;
    }
}

/**
 * Returns the receiver's shell. For all controls other than shells, this simply
 * returns the control's nearest ancestor shell. Shells return themselves, even
 * if they are children of other shells.
 *
 * @return the receiver's shell
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see #getParent
 */
public Shell getShell() {
    checkWidget();
    return _getShell();
}

Shell _getShell() {
    return parent._getShell();
}

/*
 * Returns the control that will handle the shortcut event. I.e. shortcut
 * parameters are delivered to that control in a doProcessShortcut call.
 */
Control getShortcutHandler() {
    // By default the focused control handles the shortcut.
    return display.getFocusControl();
}

/**
 * Returns a point describing the receiver's size. The x coordinate of the
 * result is the width of the receiver. The y coordinate of the result is the
 * height of the receiver.
 *
 * @return the receiver's size
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public Point getSize() {
    checkWidget();
    return OS.QWidget_frameSize(topHandle);
}

/**
 * Returns the receiver's tool tip text, or null if it has not been set.
 *
 * @return the receiver's tool tip text
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public String getToolTipText() {
    checkWidget();
    String toolTip = null;
    if (toolTipTextExists) {
        toolTip = OS.QWidget_toolTip(handle);
    }
    return toolTip;
}

int getTraversalTypeForKey(int key, int modifier, int character) {
    int type = -1;
    switch (key) {
    case OS.QT_KEY_ESCAPE: {
        if (modifier != 0)
            break;
        type = SWT.TRAVERSE_ESCAPE;
        break;
    }
    case OS.QT_KEY_ENTER:
    case OS.QT_KEY_RETURN: {
        if (modifier == 0
                || (((modifier & OS.QT_KEYPADMODIFIER) != 0) && key == OS.QT_KEY_ENTER)) {
            type = SWT.TRAVERSE_RETURN;
        }
        break;
    }
    case OS.QT_KEY_TAB: {
        if (modifier != 0)
            break;
        type = SWT.TRAVERSE_TAB_NEXT;
        break;
    }
    case OS.QT_KEY_BACKTAB: {
        if ((modifier & ~OS.QT_SHIFTMODIFIER) != 0)
            break;
        type = SWT.TRAVERSE_TAB_PREVIOUS;
        break;
    }
    case OS.QT_KEY_UP:
    case OS.QT_KEY_LEFT:
    case OS.QT_KEY_DOWN:
    case OS.QT_KEY_RIGHT: {
        if (modifier != 0)
            break;
        boolean next = (key == OS.QT_KEY_DOWN || key == OS.QT_KEY_RIGHT);
        if (parent != null && (parent.style & SWT.MIRRORED) != 0) {
            if (key == OS.QT_KEY_LEFT || key == OS.QT_KEY_RIGHT)
                next = !next;
        }
        // Perform tab group traversal also for arrow keys
        type = next ? SWT.TRAVERSE_TAB_NEXT : SWT.TRAVERSE_TAB_PREVIOUS;
        break;
    }
    case OS.QT_KEY_PAGEUP:
    case OS.QT_KEY_PAGEDOWN: {
        if (modifier != 0)
            break;
        type = (key == OS.QT_KEY_PAGEDOWN ? SWT.TRAVERSE_PAGE_NEXT
                : SWT.TRAVERSE_PAGE_PREVIOUS);
        break;
    }
    default:
        break;
    }
    return type;
}

/**
 * Returns <code>true</code> if the receiver is visible, and <code>false</code>
 * otherwise.
 * <p>
 * If one of the receiver's ancestors is not visible or some other condition
 * makes the receiver not visible, this method may still indicate that it is
 * considered visible even though it may not actually be showing.
 * </p>
 *
 * @return the receiver's visibility state
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public boolean getVisible() {
    checkWidget();
    return !OS.QWidget_isHidden(topHandle);
}

final boolean hasFocus() {
    return this == display.getFocusControl();
}

/**
 * Invokes platform specific functionality to dispose a GC.
 * <p>
 * <b>IMPORTANT:</b> This method is <em>not</em> part of the public API for
 * <code>Control</code>. It is marked public only so that it can be shared
 * within the packages provided by SWT. It is not available on all platforms,
 * and should never be called from application code.
 * </p>
 *
 * @param data
 *            the platform specific GC data
 */
public void internal_dispose_GC(GCData data) {
    checkWidget();
    data.internalGc.releaseTarget();

    // If the GraphicsContext we just released is the shared GraphicsContext
    // used by all paint listeners then don't really destroy it.
    if(data.internalGc == SharedGCWrapper.getSharedGC()) {
        return;
    }

    if (bufferedGcs != null) {
        int i = bufferedGcs.indexOf(data.internalGc);
        if (i >= 0) {
            bufferedGcs.removeElementAt(i);
            gcBuffers.removeElementAt(i);
        }
    }

    data.internalGc.dispose();
}

/**
 * Invokes platform specific functionality to allocate a new GC handle.
 * <p>
 * <b>IMPORTANT:</b> This method is <em>not</em> part of the public API for
 * <code>Control</code>. It is marked public only so that it can be shared
 * within the packages provided by SWT. It is not available on all platforms,
 * and should never be called from application code.
 * </p>
 *
 * @param data
 *            the platform specific GC data
 * @return the platform specific GC handle
 */
public int internal_new_GC(GCData data) {
    checkWidget();

    // Try obtaining the shared internal GraphicsContext instance for paint
    // listener drawing if it's available. For outside paint listener drawing
    // always create a new one.
    GraphicsContext sharedGc;
    if(isPainting>0 && (sharedGc = SharedGCWrapper.getSharedGC()) != null) {
        data.internalGc = sharedGc;
    } else {
        data.internalGc = new GraphicsContext();
    }

    data.drawable = OS.QWidget_swt_paintDevice(handle);
    data.device = display;
    data.background = getBackground();
    data.foreground = getForeground();
    data.font = getFont();

    int mask = SWT.LEFT_TO_RIGHT | SWT.RIGHT_TO_LEFT;
    if ((data.style & mask) == 0) {
        data.style |= style & (mask | SWT.MIRRORED);
    } else {
        if ((data.style & SWT.RIGHT_TO_LEFT) != 0) {
            data.style |= SWT.MIRRORED;
        }
    }

    if (isPainting>0) {
        data.buffered = false;
        data.internalGc.bindTarget(this);
    }
    else {
        data.buffered = true;
        if (bufferedGcs == null) {
            bufferedGcs = new Vector();
        }
        if (gcBuffers == null) {
            gcBuffers = new Vector();
        }
        NativeCommandBuffer buf = new NativeCommandBuffer();
        data.internalGc.bindTarget(buf, this);
        bufferedGcs.addElement(data.internalGc);
        gcBuffers.addElement(buf);
    }

    return data.internalGc.getHandle();
}

boolean isActive() {
    Dialog dialog = Display.getModalDialog();
    if (dialog != null) {
        Shell dialogShell = dialog.parent;
        if (dialogShell != null && !dialogShell.isDisposed()) {
            if (dialogShell != getShell())
                return false;
        }
    }
    Shell shell = null;
    Shell[] modalShells = Display.modalShells;
    if (modalShells != null) {
        int bits = SWT.APPLICATION_MODAL | SWT.SYSTEM_MODAL;
        int index = modalShells.length;
        while (--index >= 0) {
            Shell modal = modalShells[index];
            if (modal != null) {
                if ((modal.style & bits) != 0) {
                    Control control = this;
                    while (control != null) {
                        if (control == modal)
                            break;
                        control = control.parent;
                    }
                    if (control != modal)
                        return false;
                    break;
                }
                if ((modal.style & SWT.PRIMARY_MODAL) != 0) {
                    if (shell == null)
                        shell = getShell();
                    if (modal.parent == shell)
                        return false;
                }
            }
        }
    }
    if (shell == null)
        shell = getShell();
    return shell.getEnabled();
}

/**
 * Returns <code>true</code> if the receiver is enabled and all ancestors up to
 * and including the receiver's nearest ancestor shell are enabled. Otherwise,
 * <code>false</code> is returned. A disabled control is typically not
 * selectable from the user interface and draws with an inactive or "grayed"
 * look.
 *
 * @return the receiver's enabled state
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see #getEnabled
 */
public boolean isEnabled() {
    checkWidget();
    return getEnabled() && parent.isEnabled();
}

boolean isFocusAncestor(Control control) {
    while (control != null && control != this && !(control instanceof Shell)) {
        control = control.parent;
    }
    return control == this;
}

/**
 * Returns <code>true</code> if the receiver has the user-interface focus, and
 * <code>false</code> otherwise.
 *
 * @return the receiver's focus state
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public boolean isFocusControl() {
    checkWidget();
    boolean isFocusControl = (packageProxy != null ? packageProxy._isFocusControl() : _isFocusControl_pp());
    if (isFocusControl && !isDisposed()) {
        return true;
    }
    return false;
}

boolean _isFocusControl_pp() {
    int focusHandle = handle;
    int proxy = OS.QWidget_focusProxy(handle);
    if (proxy != 0)
        focusHandle = proxy;
    return (OS.QApplication_focusWidget() == focusHandle);
}

boolean isMirrored() {
    return (style & SWT.MIRRORED) != 0;
}

boolean isParentMirrored() {
    if(parent != null) {
        return parent.isMirrored();
    } else {
        // Display is never mirrored
        return false;
    }
}

/**
 * Returns <code>true</code> if the underlying operating system supports this
 * reparenting, otherwise <code>false</code>
 *
 * @return <code>true</code> if the widget can be reparented, otherwise
 *         <code>false</code>
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public boolean isReparentable() {
    checkWidget();
    return false; // Qt supports but not implemented
}

final boolean isShowing() {
    /*
     * This is not complete. Need to check if the widget is obscurred by a
     * parent or sibling.
     */
    if (!isVisible())
        return false;
    Control control = this;
    while (control != null) {
        Point size = control.getSize();
        if (size.x == 0 || size.y == 0) {
            return false;
        }
        control = control.parent;
    }
    return true;
}

boolean isTabGroup() {
    // If in tablist then a tabgroup
    Control[] tabList = parent._getTabList();
    if (tabList != null) {
        for (int i = 0; i < tabList.length; i++) {
            if (tabList[i] == this)
                return true;
        }
    }
    // If accepts focus by tabbing then a group
    int bits = OS.QWidget_focusPolicy(topHandle);
    return (bits & OS.QT_FOCUSPOLICY_TABFOCUS) != 0;
}

boolean isTabItem() {
    // If in tablist then not a tabitem but a group
    Control[] tabList = parent._getTabList();
    if (tabList != null) {
        for (int i = 0; i < tabList.length; i++) {
            if (tabList[i] == this)
                return false;
        }
    }
    // If accepts focus by tabbing then not a tabitem but a group
    int bits = OS.QWidget_focusPolicy(topHandle);
    if ((bits & OS.QT_FOCUSPOLICY_TABFOCUS) != 0)
        return false;
    return true;
}

boolean isTraversalKey(int key, int modifier, int character) {
    return getTraversalTypeForKey(key, modifier, character) != -1;
}

/**
 * Returns <code>true</code> if the receiver is visible and all ancestors up to
 * and including the receiver's nearest ancestor shell are visible. Otherwise,
 * <code>false</code> is returned.
 *
 * @return the receiver's visibility state
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see #getVisible
 */
public boolean isVisible() {
    checkWidget();
    return getVisible() && parent.isVisible();
}

Event makeMouseEvent(int type, int button, int x, int y, int modifiers,
        int buttons) {
    Event event = new Event();
    event.button = Display.translateButton(button);
    event.detail = 0;
    event.count = 0;
    if(isMirrored()) {
        event.x = getClientWidth() - x;
    } else {
        event.x = x;
    }
    event.y = y;
    event.stateMask = (Display.translateModifiers(modifiers) | Display
            .translateButtons(buttons));
    event.stateMask &= ~Display.translateButtons(button);
    if (type == SWT.MouseUp)
        event.stateMask |= Display.translateButtons(button);
    return event;
}

/*
 * The coordinates are mapped to the client area widget if some other widget
 * received the event.
 */
final Point mapToClientArea(int widgetHandle, int x, int y) {
    if (widgetHandle == handle) {
        return new Point(x, y);
    } else { // scroll area
        Point global = OS.QWidget_mapToGlobal(widgetHandle, x, y);
        return OS.QWidget_mapFromGlobal(handle, global.x, global.y);
    }
}

void markLayout(boolean changed, boolean all) {
    /* Do nothing */
}

Decorations menuShell() {
    return parent.menuShell();
}

void mouseToParent(int type, int widgetHandle, int button, int x, int y,
        int state, int buttons) {
    Composite parent = getParent();
    if (parent != null) {
        Point p = parent.mapToClientArea(widgetHandle, x, y);
        Event event = parent.makeMouseEvent(type, button, p.x, p.y, state, buttons);
        event.widget = parent;
        event.type = type;
        display.post(event);
    }
}

/**
 * Moves the receiver above the specified control in the drawing order. If the
 * argument is null, then the receiver is moved to the top of the drawing order.
 * The control at the top of the drawing order will not be covered by other
 * controls even if they occupy intersecting areas.
 *
 * @param control
 *            the sibling control (or null)
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_ARGUMENT - if the control has been disposed</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see Control#moveBelow
 * @see Composite#getChildren
 */
public void moveAbove(Control control) {
    checkWidget();
    if (control != null) {
        if (control.isDisposed())
            error(SWT.ERROR_INVALID_ARGUMENT);
        if (parent != control.parent)
            return;

        // There's no QWidget::stackOver, only stackUnder. The z order is
        // determined by the order of QObject children. Find out the control
        // that is above the control that we should move above of. Then use
        // stackUnder on that.
        Control controlAbove = null;
        int parentHandle = 0;
        if (parent == null) {
            parentHandle = Display.handle;
        } else {
            parentHandle = parent.handle;
        }
        int children[] = OS.QObject_children(parentHandle);
        int iChild = 0;
        boolean next = false;
        for (; iChild < children.length; ++iChild) {
            if (!next && children[iChild] == control.topHandle) {
                // The next widget we should move below of.
                next = true;
                continue;
            }
            if (next) {
                Widget widget = Display.getWidget(children[iChild]);
                if (widget != null && widget != this) {
                    if (widget instanceof org.eclipse.swt.widgets.Control) {
                        controlAbove = (Control) widget;
                        break;
                    }
                }
            }
        }
        if (controlAbove != null) {
            moveBelow(controlAbove);
        } else {
            // There was no child after the target so move to top.
            OS.QWidget_raise(topHandle);
        }
    } else {
        OS.QWidget_raise(topHandle);
    }
}

/**
 * Moves the receiver below the specified control in the drawing order. If the
 * argument is null, then the receiver is moved to the bottom of the drawing
 * order. The control at the bottom of the drawing order will be covered by all
 * other controls which occupy intersecting areas.
 *
 * @param control
 *            the sibling control (or null)
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_ARGUMENT - if the control has been disposed</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see Control#moveAbove
 * @see Composite#getChildren
 */
public void moveBelow(Control control) {
    checkWidget();
    if (control != null) {
        if (control.isDisposed())
            error(SWT.ERROR_INVALID_ARGUMENT);
        if (parent != control.parent)
            return;
        OS.QWidget_stackUnder(topHandle, control.topHandle);

    } else {
        OS.QWidget_lower(topHandle);
    }
}

/**
 * Causes the receiver to be resized to its preferred size. For a composite,
 * this involves computing the preferred size from its layout, if there is one.
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see #computeSize(int, int, boolean)
 */
public void pack() {
    pack(true);
}

/**
 * Causes the receiver to be resized to its preferred size. For a composite,
 * this involves computing the preferred size from its layout, if there is one.
 * <p>
 * If the changed flag is <code>true</code>, it indicates that the receiver's
 * <em>contents</em> have changed, therefore any caches that a layout manager
 * containing the control may have been keeping need to be flushed. When the
 * control is resized, the changed flag will be <code>false</code>, so layout
 * manager caches can be retained.
 * </p>
 *
 * @param changed
 *            whether or not the receiver's contents have changed
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see #computeSize(int, int, boolean)
 */
public void pack(boolean changed) {
    setSize(computeSize(SWT.DEFAULT, SWT.DEFAULT, changed));
}



boolean qt_event_contextmenu(int x, int y, int reason) {
    Event event = new Event();
    event.x = x;
    event.y = y;
    sendEvent(SWT.MenuDetect, event);
    if (event.doit && !isDisposed()) {
        // Check if there is a menu or commands to show.
        if ((commandList == null || commandList.length == 0) && menu == null)
            return true;

        int menuHandle = 0;
        boolean commandMenu = this.menu == null || menu.isDisposed();
        if (commandMenu) {
            menuHandle = OS.QMenu_new(handle);
        } else {
            menuHandle = menu.handle;
        }

        // Add the actions that may be added by CommandArranger
	     if(commandList != null) {
	        CommandUtils.sort(commandList);
	        for (int i = 0; i < commandList.length; i++) {
	            OS.QMenu_addAction(menuHandle, Internal_PackageSupport.topHandle(commandList[i]));
	        }
	     }

        OS.QMenu_exec(menuHandle, event.x, event.y, 0);

        // Clean-up the QActions from CommandArranger and QMenu
        if(commandList != null && menuHandle != 0)   {
	        for (int i = 0; i < commandList.length; i++) {
	            int action = Internal_PackageSupport.topHandle(commandList[i]);
	        	if(action != 0) {
	        		OS.QWidget_removeAction(menuHandle, action);
	        	}
	        }
        }
        if (commandMenu) {
            QObjectDeleteWrapper.deleteSafely(menuHandle);
            menuHandle = 0;
        }
    }
    return true;
}

boolean qt_event_keypress_pp(int widgetHandle, int key, int modifier,
        int character, int nativeScanCode) {
    if (!hasFocus()) {
        return true;
    }

    // Try if traversal would consume the key
    int traversalType = getTraversalTypeForKey(key, modifier, character);
    boolean isTraversalKey = traversalType != -1 ? true : false;
    if (isTraversalKey) {
        Event event = sendTraverseEvent(traversalType, key, modifier, character);
        if (isDisposed())
            return true;

        if (event.doit) {
            if (doTraversal(event))
                return true; // Key used for traversal
        }
    }


    // Proceed with sending the key event since it wasn't consumed by
    // traversal.

    // Call key listener
    boolean cancelInKeyListener = super.qt_event_keypress_pp(widgetHandle, key,
            modifier, character, nativeScanCode);
    // If KeyListener set doit=false then always obey that
    if (cancelInKeyListener) {
        return true;
    }
    // By default the key event is not passed to native widget if it was a
    // traversal key. Otherwise it would still traverse when traversal doIt
    // has been changed to false. But if Java widget needs to pass the
    // traverse key to native widget anyway then do it.
    return isTraversalKey ? traverseCancel : false;
}

boolean qt_event_keyrelease_pp( int widgetHandle, int key, int modifier, int character, int nativeScanCode )
{
    // Focus checking is necessary because it
    // 1. stops the  key delivery to listeners () when control does not have focus;
    // 2. stops the key delivery to native QWidget if the key event has been consumed as traversal key.
    if (!hasFocus()) {
        return true;
    }

    if (((state & WidgetState.NO_KEY_PROPAGATE) != 0) && (widgetHandle != handle)) {
        return true;
    }
    return sendKeyEvent( SWT.KeyUp, key, modifier, character, nativeScanCode );
}


boolean qt_event_mouseButtonDblClick_pp(int widgetHandle, int button,
        int x, int y, int state, int buttons) {
    if (((this.state & WidgetState.NO_MOUSE_PROPAGATE) != 0) && (widgetHandle != handle)) {
        return true;
    }
    return doHandleMouseEvent(SWT.MouseDoubleClick, widgetHandle, button, x, y,
            state, buttons);
}

boolean qt_event_mouseButtonPress_pp(int widgetHandle, int button,
        int x, int y, int state, int buttons) {
    if (((this.state & WidgetState.NO_MOUSE_PROPAGATE) != 0) && (widgetHandle != handle)) {
        return true;
    }
    return doHandleMouseEvent(SWT.MouseDown, widgetHandle, button, x, y, state,
            buttons);
}

boolean qt_event_mouseButtonRelease_pp(int widgetHandle, int button,
        int x, int y, int state, int buttons) {
    if (((this.state & WidgetState.NO_MOUSE_PROPAGATE) != 0) && (widgetHandle != handle)) {
        return true;
    }
    return doHandleMouseEvent(SWT.MouseUp, widgetHandle, button, x, y, state,
            buttons);
}

boolean qt_event_mouseMove(int widgetHandle, int button, int x, int y,
        int state, int buttons) {
    return doHandleMouseEvent(SWT.MouseMove, widgetHandle, button, x, y, state,
            buttons);
}

void qt_swt_event_widgetMoved(int widgetHandle) {
    // Resize events are sent only for top native widget,
    // as some Controls are made of multiple native widgets.
    if (widgetHandle == topHandle)
        sendEvent(SWT.Move);
}

void qt_swt_event_widgetResized_pp(int widgetHandle, int oldWidth, int oldHeight, int width, int height, boolean sendResizeEvent) {
    // Resize events are sent only for top native widget,
    // as some Controls are made of multiple native widgets.
    if (widgetHandle == topHandle && sendResizeEvent)
        sendEvent(SWT.Resize);
}

boolean qt_event_shortcut(int key, int modifier, int character) {
    Display.mnemonicControl = this;
    Control shortcutHandler = getShortcutHandler();
    if (shortcutHandler == null) {
        // If there's no shortcut handler then just activate the shortcut.
        return false;
    }
    return shortcutHandler.doProcessShortCut(key, modifier, character);
}

void qt_swt_event_bufferFlush() {
    // Next paint event is a buffer flush event
    bufferFlush = true;
}

void qt_swt_event_focusWasGained() {
    try {
        Display.focusEvent = SWT.FocusIn;
        sendEvent(SWT.FocusIn);
    } finally {
        if(display != null && !display.isDisposed()) {
            Display.focusEvent = SWT.None;
        }
    }
    
    
}
void qt_swt_event_focusWasLost() {
    try {
        Display.focusEvent = SWT.FocusOut;
        sendEvent(SWT.FocusOut);
    } finally {
        if(display != null && !display.isDisposed()) {
            Display.focusEvent = SWT.None;
        }
    }
}

void qt_swt_event_widgetPainted(int widgetHandle, int x, int y, int width,
    int height, int regionHandle) {
    try {
        isPainting = 1;
        qt_swt_event_widgetPainted_handler(widgetHandle, x, y, width, height,
                regionHandle);
    } finally {
        isPainting = 0;
    }
}

private void qt_swt_event_widgetPainted_handler(int widgetHandle, int x,
    int y, int width, int height, int regionHandle) {
    if ((state & WidgetState.OBSCURED) != 0)
        return;

    // In the case there are multiple native widgets, the paint event is
    // sent only to the "handle".
    if (widgetHandle != handle) {
        return;
    }

    // Render the buffers created outside the paint event, if any
    flushBuffers();

    // If this is only a buffer flush event then stop here
    if(bufferFlush) {
        bufferFlush = false;
        return;
    }

    if (!hooks(SWT.Paint) && !filters(SWT.Paint))
        return;

    GC gc = new GC(this);
    Event event = new Event();
    event.count = 0;
    if(isMirrored()) {
        event.x = getClientWidth() - x - width;
    } else {
        event.x = x;
    }
    event.y = y;
    event.width = width;
    event.height = height;
    event.gc = gc;
    try {
        event.gc.setClipping(x, y, width - 1, height - 1);
        sendEvent(SWT.Paint, event);
    } finally {
        // Painting needs to be ended also in the case the Control gets disposed in
        // the paint listener. After the control is disposed it's not possible to
        // call GC.dispose() because it will throw an exception.
        if(!gc.isDisposed()) {
            if(isDisposed()) {
                GCData data = gc.getGCData();
                data.internalGc.releaseTarget();
            }
            gc.dispose(); // Will throw if control is disposed
        }
    }
}

/**
 * Causes the entire bounds of the receiver to be marked as needing to be
 * redrawn. The next time a paint request is processed, the control will be
 * completely painted, including the background.
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see #update()
 * @see PaintListener
 * @see SWT#Paint
 * @see SWT#NO_BACKGROUND
 * @see SWT#NO_REDRAW_RESIZE
 * @see SWT#NO_MERGE_PAINTS
 */
public void redraw() {
    checkWidget();
    doRedraw();
}

/**
 * Causes the rectangular area of the receiver specified by the arguments to be
 * marked as needing to be redrawn. The next time a paint request is processed,
 * that area of the receiver will be painted, including the background. If the
 * <code>all</code> flag is <code>true</code>, any children of the receiver
 * which intersect with the specified area will also paint their intersecting
 * areas. If the <code>all</code> flag is <code>false</code>, the children will
 * not be painted.
 *
 * @param x
 *            the x coordinate of the area to draw
 * @param y
 *            the y coordinate of the area to draw
 * @param width
 *            the width of the area to draw
 * @param height
 *            the height of the area to draw
 * @param all
 *            <code>true</code> if children should redraw, and
 *            <code>false</code> otherwise
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see #update()
 * @see PaintListener
 * @see SWT#Paint
 * @see SWT#NO_BACKGROUND
 * @see SWT#NO_REDRAW_RESIZE
 * @see SWT#NO_MERGE_PAINTS
 */
public void redraw(int x, int y, int width, int height, boolean all) {
    checkWidget();
    if(isMirrored()) {
        x = getClientWidth() - x - width;
    }
    doRedraw(x, y, width, height);
    if (all) {
        redrawChildren();
    }
}

void redrawChildren() {
}

void releaseChildren_pp(boolean destroy) {
    Command[] cmds = getCommands();
    for (int i = 0; i < cmds.length; i++) {
        Command child = cmds[i];
        if (child != null && !child.isDisposed()) {
            child.dispose();
        }
    }
    super.releaseChildren_pp(destroy);
}

void releaseHandle_pp() {
    super.releaseHandle_pp();
    parent = null;
}

void releaseParent_pp() {
    if(parent.packageProxy != null) {
        parent.packageProxy.removeControl(this);
    } else {
        parent.removeControl_pp(this);
    }
}

void releaseWidget_pp() {
    super.releaseWidget_pp();

    if (menu != null && !menu.isDisposed()) {
        menu.dispose();
    }
    menu = null;

    layoutData = null;
    background = null;
    foreground = null;
}

void removeCommand(final Command command) {
    if (command.control != this)
        return;
    if (commandList == null || commandList.length == 0)
        return;
    if (commandList.length == 1 && command == commandList[0]) {
        commandList = null;
        return;
    }

    int size = commandList.length;
    Command[] newList = new Command[size - 1];
    int counter = 0;
    for (int i = 0; i < size; i++) {
        if (command != commandList[i]) {
            newList[counter] = commandList[i];
            counter++;
        }
    }
    if (counter == newList.length) {
        commandList = newList;
    }
}

/**
 * Removes the listener from the collection of listeners who will be notified
 * when the control is moved or resized.
 *
 * @param listener
 *            the listener which should no longer be notified
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see ControlListener
 * @see #addControlListener
 */
public void removeControlListener(ControlListener listener) {
    checkWidget();
    if (listener == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    if (eventTable == null)
        return;
    eventTable.unhook(SWT.Move, listener);
    eventTable.unhook(SWT.Resize, listener);
}

/**
 * Removes the listener from the collection of listeners who will be notified
 * when the control gains or loses focus.
 *
 * @param listener
 *            the listener which should no longer be notified
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see FocusListener
 * @see #addFocusListener
 */
public void removeFocusListener(FocusListener listener) {
    checkWidget();
    if (listener == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    if (eventTable == null)
        return;
    eventTable.unhook(SWT.FocusIn, listener);
    eventTable.unhook(SWT.FocusOut, listener);
}

/**
 * Removes the listener from the collection of listeners who will be notified
 * when keys are pressed and released on the system keyboard.
 *
 * @param listener
 *            the listener which should no longer be notified
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see KeyListener
 * @see #addKeyListener
 */
public void removeKeyListener(KeyListener listener) {
    checkWidget();
    if (listener == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    if (eventTable == null)
        return;
    eventTable.unhook(SWT.KeyUp, listener);
    eventTable.unhook(SWT.KeyDown, listener);
}

/**
 * Removes the listener from the collection of listeners who will be notified
 * when mouse buttons are pressed and released.
 *
 * @param listener
 *            the listener which should no longer be notified
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see MouseListener
 * @see #addMouseListener
 */
public void removeMouseListener(MouseListener listener) {
    checkWidget();
    if (listener == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    if (eventTable == null)
        return;
    eventTable.unhook(SWT.MouseDown, listener);
    eventTable.unhook(SWT.MouseUp, listener);
    eventTable.unhook(SWT.MouseDoubleClick, listener);
}

/**
 * Removes the listener from the collection of listeners who will be notified
 * when the mouse moves.
 *
 * @param listener
 *            the listener which should no longer be notified
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see MouseMoveListener
 * @see #addMouseMoveListener
 */
public void removeMouseMoveListener(MouseMoveListener listener) {
    checkWidget();
    if (listener == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    if (eventTable == null)
        return;
    eventTable.unhook(SWT.MouseMove, listener);
}

/**
 * Removes the listener from the collection of listeners who will be notified
 * when the receiver needs to be painted.
 *
 * @param listener
 *            the listener which should no longer be notified
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see PaintListener
 * @see #addPaintListener
 */
public void removePaintListener(PaintListener listener) {
    checkWidget();
    if (listener == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    if (eventTable == null)
        return;
    eventTable.unhook(SWT.Paint, listener);
}

/**
 * Removes the listener from the collection of listeners who will be notified
 * when traversal events occur.
 *
 * @param listener
 *            the listener which should no longer be notified
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see TraverseListener
 * @see #addTraverseListener
 */
public void removeTraverseListener(TraverseListener listener) {
    checkWidget();
    if (listener == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    if (eventTable == null)
        return;
    eventTable.unhook(SWT.Traverse, listener);
}

final boolean sendMouseEvent(int type, int button, boolean send, int x, int y,
        int modifiers, int buttons) {
    Event event = makeMouseEvent(type, button, x, y, modifiers, buttons);
    if (send) {
        sendEvent(type, event);
        if (isDisposed())
            return true;
    } else {
        postEvent(type, event);
    }
    return !event.doit;
}

Event sendTraverseEvent(int traversalType, int key, int modifier, int character) {
    // Make traverse event
    Event event = makeKeyEvent(key, modifier, character, 0);
    event.detail = traversalType;
    event.stateMask = Display.translateModifiers(modifier);

    // Ask from the control if it would traverse with this key
    // and if it should go to the native widget.
    if(packageProxy != null) {
        packageProxy.setTraversalFlags(traversalType, key, modifier, character);
    } else {
        setTraversalFlags_pp(traversalType, key, modifier, character);
    }
    event.doit = traverseDoit;

    // Event to the listener
    if (hooks(SWT.Traverse) || filters(SWT.Traverse)) {
        sendEvent(SWT.Traverse, event);
    }

    return event;
}

void setBackground_pp () {
    if(backgroundImage != null) {
        applyBackgroundImage(backgroundImage);
        return;
    }
    if(background != null) {
        applyBackgroundColor (background);
        return;
    }
    Control control = findBackgroundControl ();
    if(control == null) control = this;
    if(control.backgroundImage != null) {
        applyBackgroundImage(control.backgroundImage);
        return;
    }
    // If this is null then it will reset the default background
    applyBackgroundColor(control.background);
}

/**
 * Sets the receiver's background color to the color specified by the argument,
 * or to the default system color for the control if the argument is null.
 * <p>
 * Note: This operation is a hint and may be overridden by the platform. For
 * example, on Windows the background of a Button cannot be changed.
 * </p>
 *
 * @param color
 *            the new color (or null)
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_ARGUMENT - if the argument has been disposed
 *                </li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public void setBackground(Color color) {
    checkWidget();
    if (color != null) {
        if (color.isDisposed()) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
    if (background == null && color == null) {
        return;
    }
    if(color == null) {
        background = null;
    } else {
        // Store a copy
        background = Internal_GfxPackageSupport.newColor(display, color.handle);
    }
    // Propagate background color change
    updateBackground();
}

/**
 * Sets the receiver's background image to the image specified by the argument,
 * or to the default system color for the control if the argument is null. The
 * background image is tiled to fill the available space.
 * <p>
 * Note: This operation is a hint and may be overridden by the platform. For
 * example, on Windows the background of a Button cannot be changed.
 * </p>
 *
 * @param image
 *            the new image (or null)
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_ARGUMENT - if the argument has been disposed
 *                </li>
 *                <li>ERROR_INVALID_ARGUMENT - if the argument is not a bitmap</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @since 3.2
 */
public void setBackgroundImage(Image image) {
    checkWidget();
    if(image == null && backgroundImage == null) {
        return;
    }
    if (image != null) {
        if (image.isDisposed ()) error (SWT.ERROR_INVALID_ARGUMENT);
        if (image.type != SWT.BITMAP) error (SWT.ERROR_INVALID_ARGUMENT);
    }
    if (backgroundImage == image) return;
    backgroundImage = image;
    // Propagate background image change
    updateBackground();
}

/**
 * Sets the receiver's size and location to the rectangular area specified by
 * the arguments. The <code>x</code> and <code>y</code> arguments are relative
 * to the receiver's parent (or its display if its parent is null), unless the
 * receiver is a shell. In this case, the <code>x</code> and <code>y</code>
 * arguments are relative to the display.
 * <p>
 * Note: Attempting to set the width or height of the receiver to a negative
 * number will cause that value to be set to zero instead.
 * </p>
 *
 * @param x
 *            the new x coordinate for the receiver
 * @param y
 *            the new y coordinate for the receiver
 * @param width
 *            the new width for the receiver
 * @param height
 *            the new height for the receiver
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public void setBounds(int x, int y, int width, int height) {
    checkWidget();
    setBounds(x, y, Math.max(0, width), Math.max(0, height), true, true);
}

int setBounds(int x, int y, int width, int height, boolean move,
        boolean resize) {
    int result = 0;
    Point oldSize = OS.QWidget_size(topHandle);
    Point oldPos = OS.QWidget_pos(topHandle);
    if (resize) {
        if (oldSize.x != width || oldSize.y != height) {
            if(isParentMirrored()) {
                try {
                    Display.blockedQtEventType = OS.QSWTEVENT_WIDGETMOVED;
                    OS.QWidget_move(topHandle, oldPos.x - (width - oldSize.x), oldPos.y);
                } finally {
                    Display.blockedQtEventType = OS.QEVENT_NONE;
                }
            }
            // This will send a resize event
            OS.QWidget_resize(topHandle, width, height);
            result |= WidgetState.RESIZED;
        }
    }
    if (move) {
        if(isParentMirrored()) {
            x = parent.getClientWidth() - x - OS.QWidget_size(topHandle).x;
        }
        if (oldPos.x != x || oldPos.y != y) {
            // This will send a move event
            OS.QWidget_move(topHandle, x, y);
            result |= WidgetState.MOVED;
        }
    }
    return result;
}

/**
 * Sets the receiver's size and location to the rectangular area specified by
 * the argument. The <code>x</code> and <code>y</code> fields of the rectangle
 * are relative to the receiver's parent (or its display if its parent is null).
 * <p>
 * Note: Attempting to set the width or height of the receiver to a negative
 * number will cause that value to be set to zero instead.
 * </p>
 *
 * @param rect
 *            the new bounds for the receiver
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public void setBounds(Rectangle rect) {
    checkWidget();
    if (rect == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    setBounds(rect.x, rect.y, Math.max(0, rect.width),
            Math.max(0, rect.height), true, true);
}

/**
 * If the argument is <code>true</code>, causes the receiver to have all mouse
 * events delivered to it until the method is called with <code>false</code> as
 * the argument.
 *
 * @param capture
 *            <code>true</code> to capture the mouse, and <code>false</code> to
 *            release it
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public void setCapture(boolean capture) {
    checkWidget();
    if(capture) {
        OS.QWidget_grabMouse(handle);
    } else {
        OS.QWidget_releaseMouse(handle);
    }
}

/**
 * Enables the receiver if the argument is <code>true</code>, and disables it
 * otherwise. A disabled control is typically not selectable from the user
 * interface and draws with an inactive or "grayed" look.
 *
 * @param enabled
 *            the new enabled state
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public void setEnabled(boolean enabled) {
    checkWidget();
    if (enabled) {
        state &= ~WidgetState.DISABLED;
    } else {
        state |= WidgetState.DISABLED;
    }
    Control control = null;
    boolean fixFocus = false;
    if (!enabled) {
        if (Display.focusEvent != SWT.FocusOut) {
            control = display.getFocusControl ();
            fixFocus = isFocusAncestor (control);
        }
    }
    enableWidget(enabled);
    if (fixFocus) fixFocus (control);
}

boolean setFixedFocus () {
    if ((style & SWT.NO_FOCUS) != 0) return false;
    return forceFocus (OS.QT_OTHERFOCUSREASON);
}

/**
 * Causes the receiver to have the <em>keyboard focus</em>, such that all
 * keyboard events will be delivered to it. Focus reassignment will respect
 * applicable platform constraints.
 *
 * @return <code>true</code> if the control got focus, and <code>false</code> if
 *         it was unable to.
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see #forceFocus
 */
public boolean setFocus() {
    checkWidget();
    return setFocus(OS.QT_OTHERFOCUSREASON);
}

boolean setFocus(int focusReason) {
    if ((style & SWT.NO_FOCUS) != 0)
        return false;
    return forceFocus(focusReason);
}

/**
 * Sets the font that the receiver will use to paint textual information to the
 * font specified by the argument, or to the default font for that kind of
 * control if the argument is null.
 *
 * @param font
 *            the new font (or null)
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_ARGUMENT - if the argument has been disposed
 *                </li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public void setFont(Font font) {
    checkWidget();
    if (((state & WidgetState.FONT) == 0) && font == null)
        return;
    this.font = font;

    if (font == null) {
        OS.QWidget_setFont(topHandle, OS.SwtFontCache_cache(OS.QApplication_swt_font_new(topHandle)));
    } else {
        if (font.isDisposed())
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        OS.QWidget_setFont(topHandle, font.handle);
    }

    if (font == null) {
        state &= ~WidgetState.FONT;
    } else {
        state |= WidgetState.FONT;
    }
}

/**
 * Sets the receiver's foreground color to the color specified by the argument,
 * or to the default system color for the control if the argument is null.
 * <p>
 * Note: This operation is a hint and may be overridden by the platform.
 * </p>
 *
 * @param color
 *            the new color (or null)
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_ARGUMENT - if the argument has been disposed
 *                </li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public void setForeground(Color color) {
    checkWidget();
    if (color != null) {
        if (color.isDisposed()) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
    if (foreground == null && color == null) {
        return;
    }
    // Get a temporary copy of the widgets current palette, modify the
    // foreground color, and set the modified palette back to the widget.
    int palette = OS.QWidget_swt_palette_new(topHandle);
    try {
        if(color == null) {
            foreground = null;
        } else {
            // Store a copy
            foreground = Internal_GfxPackageSupport.newColor(display, color.handle);
        }
        setPaletteFgColor(palette, color);
        OS.QWidget_setPalette(topHandle, palette);
    } finally {
        OS.QPalette_delete(palette);
    }
}

/**
 * Sets the layout data associated with the receiver to the argument.
 *
 * @param layoutData
 *            the new layout data for the receiver.
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public void setLayoutData(Object layoutData) {
    checkWidget();
    this.layoutData = layoutData;
}

/**
 * Sets the receiver's location to the point specified by the arguments which
 * are relative to the receiver's parent (or its display if its parent is null),
 * unless the receiver is a shell. In this case, the point is relative to the
 * display.
 *
 * @param x
 *            the new x coordinate for the receiver
 * @param y
 *            the new y coordinate for the receiver
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public void setLocation(int x, int y) {
    checkWidget();
    setBounds(x, y, 0, 0, true, false);
}

/**
 * Sets the receiver's location to the point specified by the arguments which
 * are relative to the receiver's parent (or its display if its parent is null),
 * unless the receiver is a shell. In this case, the point is relative to the
 * display.
 *
 * @param location
 *            the new location for the receiver
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public void setLocation(Point location) {
    checkWidget();
    if (location == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    setBounds(location.x, location.y, 0, 0, true, false);
}

/**
 * Sets the receiver's pop up menu to the argument. All controls may optionally
 * have a pop up menu that is displayed when the user requests one for the
 * control. The sequence of key strokes, button presses and/or button releases
 * that are used to request a pop up menu is platform specific.
 * <p>
 * Note: Disposing of a control that has a pop up menu will dispose of the menu.
 * To avoid this behavior, set the menu to null before the control is disposed.
 * </p>
 *
 * @param menu
 *            the new pop up menu
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_MENU_NOT_POP_UP - the menu is not a pop up menu</li>
 *                <li>ERROR_INVALID_PARENT - if the menu is not in the same
 *                widget tree</li>
 *                <li>ERROR_INVALID_ARGUMENT - if the menu has been disposed</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public void setMenu(Menu menu) {
    checkWidget();
    if (menu != null) {
        if ((menu.style & SWT.POP_UP) == 0) {
            error(SWT.ERROR_MENU_NOT_POP_UP);
        }
        if (menu.parent != menuShell()) {
            error(SWT.ERROR_INVALID_PARENT);
        }
    }
    this.menu = menu;
}

private final void setPaletteBgColor(int paletteHandle, Color color) {
    int[] bkRoles = getBackgroundColorRoles();
    if (color != null) {
        for (int j = 0; j < bkRoles.length; j++) {
            OS.QPalette_setColor(paletteHandle, bkRoles[j], color.getRed(),
                    color.getGreen(), color.getBlue());
        }
    } else { // Restore the default colors from application palette
        int defaultPalette = OS.QApplication_swt_palette_new();
        try {
            for (int j = 0; j < bkRoles.length; j++) {
                OS.QPalette_swt_copyBrushFromPalette(paletteHandle,
                        defaultPalette, bkRoles[j]);
            }
        } finally {
            OS.QPalette_delete(defaultPalette);
        }
    }
}

private final void setPaletteBgImage(int paletteHandle, Image image) {
    int[] bkRoles = getBackgroundImageRoles();
    if (image != null) {
        for (int j = 0; j < bkRoles.length; j++) {
            if ((findBackgroundControl () != this) && canInheritBackgroundImage()) {
                // If background is inherited then brush is set to null
                OS.QPalette_swt_setBrush(paletteHandle, bkRoles[j], 0);
                backgroundImageInherited(image);
            } else {
                OS.QPalette_swt_setBrush(paletteHandle, bkRoles[j], Internal_GfxPackageSupport.getPixmapHandle(image));
                if(packageProxy != null) {
                    packageProxy.backgroundImageApplied(image);
                } else {
                    backgroundImageApplied_pp(image);
                }
            }
        }
    } else { // Restore the default brushes from application palette
        int defaultPalette = OS.QApplication_swt_palette_new();
        try {
            for (int j = 0; j < bkRoles.length; j++) {
                OS.QPalette_swt_copyBrushFromPalette(paletteHandle,
                        defaultPalette, bkRoles[j]);
            }
        } finally {
            OS.QPalette_delete(defaultPalette);
        }
    }
}

private final void setPaletteFgColor(int paletteHandle, Color color) {
    int[] fgRoles = new int[] { OS.QPALETTE_COLORROLE_WINDOWTEXT,
            OS.QPALETTE_COLORROLE_TEXT, OS.QPALETTE_COLORROLE_BUTTONTEXT };
    if (color != null) {
        for (int j = 0; j < fgRoles.length; j++) {
            OS.QPalette_setColor(paletteHandle, fgRoles[j], color.getRed(),
                    color.getGreen(), color.getBlue());
        }
    } else { // Restore the default colors from application palette
        int defaultPalette = OS.QApplication_swt_palette_new();
        try {
            for (int j = 0; j < fgRoles.length; j++) {
                OS.QPalette_swt_copyBrushFromPalette(paletteHandle,
                        defaultPalette, fgRoles[j]);
            }
        } finally {
            OS.QPalette_delete(defaultPalette);
        }
    }
}

/**
 * Changes the parent of the widget to be the one provided if the underlying
 * operating system supports this feature. Returns <code>true</code> if the
 * parent is successfully changed.
 *
 * @param parent
 *            the new parent for the control.
 * @return <code>true</code> if the parent is changed and <code>false</code>
 *         otherwise.
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_ARGUMENT - if the argument has been disposed
 *                </li>
 *                <li>ERROR_NULL_ARGUMENT - if the parent is <code>null</code></li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public boolean setParent(Composite parent) {
    checkWidget();
    if (parent == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    if (parent.isDisposed())
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    if (this.parent == parent)
        return true;
    if (!isReparentable())
        return false;
    return false;
}

boolean setRadioFocus(boolean tabbing) {
    return false;
}

boolean setRadioSelection(boolean value) {
    return false;
}

/**
 * If the argument is <code>false</code>, causes subsequent drawing operations
 * in the receiver to be ignored. No drawing of any kind can occur in the
 * receiver until the flag is set to true. Graphics operations that occurred
 * while the flag was <code>false</code> are lost. When the flag is set to
 * <code>true</code>, the entire widget is marked as needing to be redrawn.
 * Nested calls to this method are stacked.
 * <p>
 * Note: This operation is a hint and may not be supported on some platforms or
 * for some widgets.
 * </p>
 *
 * @param redraw
 *            the new redraw state
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see #redraw(int, int, int, int, boolean)
 * @see #update()
 */
public void setRedraw(boolean redraw) {
    checkWidget();
    OS.QWidget_setUpdatesEnabled(handle, redraw);
}

/**
 * Sets the receiver's size to the point specified by the arguments.
 * <p>
 * Note: Attempting to set the width or height of the receiver to a negative
 * number will cause that value to be set to zero instead.
 * </p>
 *
 * @param width
 *            the new width for the receiver
 * @param height
 *            the new height for the receiver
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public void setSize(int width, int height) {
    checkWidget();
    setBounds(0, 0, Math.max(0, width), Math.max(0, height), false, true);
}

/**
 * Sets the receiver's size to the point specified by the argument.
 * <p>
 * Note: Attempting to set the width or height of the receiver to a negative
 * number will cause them to be set to zero instead.
 * </p>
 *
 * @param size
 *            the new size for the receiver
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the point is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public void setSize(Point size) {
    checkWidget();
    if (size == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    setBounds(0, 0, Math.max(0, size.x), Math.max(0, size.y), false, true);
}

boolean setTabGroupFocus_pp() {
    return setTabItemFocus();
}

boolean setTabItemFocus() {
    if (!isShowing())
        return false;
    return forceFocus(OS.QT_TABFOCUSREASON);
}

/**
 * Sets the receiver's tool tip text to the argument, which may be null
 * indicating that no tool tip text should be shown.
 *
 * @param string
 *            the new tool tip text (or null)
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public void setToolTipText(String string) {
    checkWidget();
    if (string == null) {
        string = "";
        toolTipTextExists = false;
    } else {
        toolTipTextExists = true;
    }
    OS.QWidget_setToolTip(handle, string);
}

void setTraversalFlags_pp(int type, int key, int modifier, int character) {
    if ((state & WidgetState.NO_TRAVERSE) != 0) {
        traverseDoit = false;
        traverseCancel = false;
        return;
    }

    // Set doIt to true for the types that will actually traverse
    int traversalFlags = SWT.TRAVERSE_ARROW_NEXT | SWT.TRAVERSE_ARROW_PREVIOUS
            | SWT.TRAVERSE_TAB_NEXT | SWT.TRAVERSE_TAB_PREVIOUS
            | SWT.TRAVERSE_MNEMONIC;
    traverseDoit = ((type & traversalFlags) != 0);

    // If there's a default button then offer invoking it
    if (type == SWT.TRAVERSE_RETURN) {
        traverseDoit = (menuShell().defaultButton != null);
    }

    // Cancel the traverse key event so that the key is not delivered
    // to the native widget. Key events that will cause native widget
    // to traverse must never go through. However, if it is known that
    // the traverse key will not traverse in the current state of the widget
    // then it can be let through by setting this flag.
    traverseCancel = true;
}

/**
 * Marks the receiver as visible if the argument is <code>true</code>, and marks
 * it invisible otherwise.
 * <p>
 * If one of the receiver's ancestors is not visible or some other condition
 * makes the receiver not visible, marking it visible may not actually cause it
 * to be displayed.
 * </p>
 *
 * @param visible
 *            the new visibility state
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public void setVisible(boolean visible) {
    checkWidget();
    if (getVisible() == visible)
        return;
    OS.QWidget_setVisible(topHandle, visible);
    if (visible) {
        sendEvent(SWT.Show);
        if(isDisposed()) return;
    }
    Control control = null;
    boolean fixFocus = false;
    if (!visible) {
        if (Display.focusEvent != SWT.FocusOut) {
            control = display.getFocusControl ();
            fixFocus = isFocusAncestor (control);
        }
    }
    if (!visible) {
        sendEvent(SWT.Hide);
        if (isDisposed ()) return;
    }
    if (fixFocus) fixFocus (control);
}

/**
 * Returns a point which is the result of converting the argument, which is
 * specified in display relative coordinates, to coordinates relative to the
 * receiver.
 * <p>
 *
 * @param x
 *            the x coordinate to be translated
 * @param y
 *            the y coordinate to be translated
 * @return the translated coordinates
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed
 *                </li> <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @since 2.1
 */
public Point toControl(int x, int y) {
    checkWidget();
    Point p = OS.QWidget_mapFromGlobal(handle, x, y);
    if(isMirrored()) {
        p.x = getClientWidth() - p.x;
    }
    return p;
}

/**
 * Returns a point which is the result of converting the argument, which is
 * specified in display relative coordinates, to coordinates relative to the
 * receiver.
 * <p>
 *
 * @param point
 *            the point to be translated (must not be null)
 * @return the translated coordinates
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the point is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed
 *                </li> <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public Point toControl(Point point) {
    checkWidget();
    if (point == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    return toControl(point.x, point.y);
}

/**
 * Returns a point which is the result of converting the argument, which is
 * specified in coordinates relative to the receiver, to display relative
 * coordinates.
 * <p>
 *
 * @param x
 *            the x coordinate to be translated
 * @param y
 *            the y coordinate to be translated
 * @return the translated coordinates
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed
 *                </li> <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @since 2.1
 */
public Point toDisplay(int x, int y) {
    checkWidget();
    if(isMirrored()) {
        x = getClientWidth() - x;
    }
    return OS.QWidget_mapToGlobal(handle, x, y);
}

/**
 * Returns a point which is the result of converting the argument, which is
 * specified in coordinates relative to the receiver, to display relative
 * coordinates.
 * <p>
 *
 * @param point
 *            the point to be translated (must not be null)
 * @return the translated coordinates
 *
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the point is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed
 *                </li> <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public Point toDisplay(Point point) {
    checkWidget();
    if (point == null)
        error(SWT.ERROR_NULL_ARGUMENT);
    return toDisplay(point.x, point.y);
}

/**
 * Based on the argument, perform one of the expected platform traversal action.
 * The argument should be one of the constants: <code>SWT.TRAVERSE_ESCAPE</code>
 * , <code>SWT.TRAVERSE_RETURN</code>, <code>SWT.TRAVERSE_TAB_NEXT</code>,
 * <code>SWT.TRAVERSE_TAB_PREVIOUS</code>, <code>SWT.TRAVERSE_ARROW_NEXT</code>
 * and <code>SWT.TRAVERSE_ARROW_PREVIOUS</code>.
 *
 * @param traversal
 *            the type of traversal
 * @return true if the traversal succeeded
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public boolean traverse(int traversal) {
    checkWidget();
    Event event = new Event();
    event.doit = true;
    event.detail = traversal;
    sendEvent(SWT.Traverse, event);
    if (isDisposed())
        return true;

    // It doesn't make sense to activate mnemonic traversal by calling this
    // but if it's done then traversal is attempted relative to this control.
    // Normally it would be done relative to the control that had the
    // shortcut event.
    Display.mnemonicControl = this;

    return doTraversal(event);
}

boolean traverseEscape() {
    // Not implemented, in mobile devices there's usually no esc key
    return false;
}

boolean traverseGroup(boolean next) {
    Control root = computeTabRoot();
    Control group = computeTabGroup();
    Control[] list = root.computeTabList();

    // Find this Control, traversal is done respective to that
    int length = list.length;
    int index = 0;
    while (index < length) {
        if (list[index] == group)
            break;
        index++;
    }
    if (index >= length)
        return false;

    // Traverse to the next or previous, looping around from the end or the
    // beginning of the array.
    int start = index, offset = (next) ? 1 : -1;
    while ((index = ((index + offset + length) % length)) != start) {
        Control control = list[index];
        if (!control.isDisposed() &&
                (control.packageProxy != null ? control.packageProxy.setTabGroupFocus() : control.setTabGroupFocus_pp())) {
            return true;
        }
    }
    if (group.isDisposed())
        return false;
    return (group.packageProxy != null ? group.packageProxy.setTabGroupFocus() : group.setTabGroupFocus_pp());
}

boolean traverseItem(boolean next) {
    // Mobile devices often don't have tab keys, only the arrows.
    // Therefore the platform always suggests tab group traversal.
    // However, it's possible to attempt item traversal by using
    // TraverseEvents
    // to override the default behavior or by calling the traverse method
    // with
    // item traversal type.

    // Find this Control, traversal is done respective to that
    Control[] children = parent._getChildren();
    int length = children.length;
    int index = 0;
    while (index < length) {
        if (children[index] == this)
            break;
        index++;
    }
    if (index == length)
        return false;

    // Traverse to the next or previous, looping around from the end or the
    // beginning of the array.
    int start = index, offset = (next) ? 1 : -1;
    while ((index = (index + offset + length) % length) != start) {
        Control child = children[index];
        if (!child.isDisposed() && child.isTabItem()) {
            if (child.setTabItemFocus())
                return true;
        }
    }
    return false;
}

/*
 * Traverses to the next control of the control that received the last shortcut
 * event.
 */
boolean traverseMnemonic(char key) {

    // SWT checks for mnemonic hit here

    if (Display.mnemonicControl == null)
        return false;
    Control nextControl = Display.mnemonicControl.findNextControl(false);
    if (nextControl != null) {
        nextControl.setFocus(OS.QT_TABFOCUSREASON);
        return true;
    }
    return false;
}

boolean traversePage(boolean next) {
    // Not implemented, should implement for MultiPageDialog
    return false;
}

boolean traverseReturn() {
    return menuShell().invokeDefaultButton();
}

/**
 * Forces all outstanding paint requests for the widget to be processed before
 * this method returns. If there are no outstanding paint request, this method
 * does nothing.
 * <p>
 * Note: This method does not cause a redraw.
 * </p>
 *
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 *
 * @see #redraw()
 * @see #redraw(int, int, int, int, boolean)
 * @see PaintListener
 * @see SWT#Paint
 */
public void update() {
    checkWidget();
    _update();
}

void _update() {
    // This has a problem that it dispatches other than paint events too.
    OS.QCoreApplication_processEvents(Display.handle,
            OS.QEVENTLOOP_EXCLUDEUSERINPUTEVENTS);
    if (isDisposed())
        return;
    OS.QCoreApplication_sendPostedEvents(Display.handle, handle,
            OS.QEVENT_PAINT);
}

private void updateAutoFillBackground() {
    Control control = findBackgroundControl();
    boolean enabled = false;
    if(control != null) {
        enabled = (control.background != null || control.backgroundImage != null);
    }
    OS.QWidget_setAutoFillBackground(topHandle, enabled);
}

void updateBackground() {
    if(packageProxy != null) {
        packageProxy.setBackground();
    } else {
        setBackground_pp();
    }
}

void updateBackgroundMode () {
    int oldState = state & WidgetState.PARENT_BACKGROUND;
    checkBackground ();
    if (oldState != (state & WidgetState.PARENT_BACKGROUND)) {
        if(packageProxy != null) {
            packageProxy.setBackground();
        } else {
            setBackground_pp ();
        }
    }
}

void updateImages () {
    // Override to update any images that should be updated after a settings change
}

void updateLayout(boolean all) {
    /* Do nothing */
}

void waitXRequestComplete() {
    // Make XServer process all queued requests not discarding any events.
    // Note that this is heavy and slow. Here goes everything, not only the
    // one event we are waiting for.
    OS.QApplication_syncX();
    // Make Qt process all XEvents.
    Display.noInterrupt = true;
    OS.QCoreApplication_processEvents(Display.handle,
            OS.QEVENTLOOP_EXCLUDEUSERINPUTEVENTS);
    Display.noInterrupt = false;
}
}
