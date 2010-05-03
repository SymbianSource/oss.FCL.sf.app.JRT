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
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Internal_GfxPackageSupport;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.QObjectDeleteWrapper;
import org.eclipse.swt.internal.qt.WidgetState;

/**
 * Instances of this class provide the appearance and
 * behavior of <code>Shells</code>, but are not top
 * level shells or dialogs. Class <code>Shell</code>
 * shares a significant amount of code with this class,
 * and is a subclass.
 * <p>
 * IMPORTANT: This class was intended to be abstract and
 * should <em>never</em> be referenced or instantiated.
 * Instead, the class <code>Shell</code> should be used.
 * </p>
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
 * Note: The styles supported by this class must be treated
 * as <em>HINT</em>s, since the window manager for the
 * desktop on which the instance is visible has ultimate
 * control over the appearance and behavior of decorations.
 * For example, some window managers only support resizable
 * windows and will always assume the RESIZE style, even if
 * it is not set.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>BORDER, CLOSE, MIN, MAX, NO_TRIM, RESIZE, TITLE, ON_TOP</dd>
 * <dt><b>Events:</b></dt>
 * <dd>(none)</dd>
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
 * <p>
 * IMPORTANT: This class is intended to be subclassed <em>only</em>
 * within the SWT implementation.
 * </p>
 *
 * @see #getMinimized
 * @see #getMaximized
 * @see Shell
 * @see SWT
 */
public class Decorations extends Canvas {
    Image image;
    Menu menuBar;
    Menu [] menus;
    Control savedFocus;
    Button defaultButton, saveDefault;

    // Window state information for restoring the previous state
    int restoreState;
    Point fixedSize;

    // Holds the handle to the internal menu bar
    private int ownMenuBar;

    // Learned dimensions of normal window trim with title bar
    static int topTitleFrame = -1;
    static int leftTitleFrame = -1;
    static int rightTitleFrame = -1;
    static int bottomTitleFrame = -1;

    // Learned dimensions of thin window trim without title bar
    static int topThinFrame = -1;
    static int leftThinFrame = -1;
    static int rightThinFrame = -1;
    static int bottomThinFrame = -1;

Decorations () {
    /* Do nothing */
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
 * @see SWT#CLOSE
 * @see SWT#MIN
 * @see SWT#MAX
 * @see SWT#RESIZE
 * @see SWT#TITLE
 * @see SWT#NO_TRIM
 * @see SWT#SHELL_TRIM
 * @see SWT#DIALOG_TRIM
 * @see SWT#ON_TOP
 * @see Widget#checkSubclass
 * @see Widget#getStyle
 */
public Decorations (Composite parent, int style) {
    super (parent, checkStyle (style, parent));
}

static int checkStyle (int style, Composite parent) {
    if ((style & SWT.NO_TRIM) != 0) {
        style &= ~(SWT.CLOSE | SWT.TITLE | SWT.MIN | SWT.MAX | SWT.RESIZE | SWT.BORDER);
    }
    if ((style & (SWT.MENU | SWT.MIN | SWT.MAX | SWT.CLOSE)) != 0) {
        style |= SWT.TITLE;
    }
    return style;
}

protected void checkSubclass () {
    if (!isValidSubclass ()) error (SWT.ERROR_INVALID_SUBCLASS);
}

void checkBorder_pp() {
    // For Shells SWT.BORDER means dialog window border. Not QFrame border.
    if ((state & WidgetState.CANVAS) != 0) {
        if(frameHandle != 0) {
            OS.QFrame_setFrameStyle(frameHandle, OS.QT_NOFRAME);
        }
    } else {
        super.checkBorder_pp ();
    }
}

void addMenu (Menu menu) {
    if (menus == null) menus = new Menu [4];
    for (int i=0; i<menus.length; i++) {
        if (menus [i] == null) {
            menus [i] = menu;
            return;
        }
    }
    Menu [] newMenus = new Menu [menus.length + 4];
    newMenus [menus.length] = menu;
    System.arraycopy (menus, 0, newMenus, 0, menus.length);
    menus = newMenus;
}

Control computeTabGroup () {
    return this;
}

Control computeTabRoot () {
    return this;
}

public void dispose () {
    if (isDisposed()) return;
    if (!isValidThread ()) error (SWT.ERROR_THREAD_INVALID_ACCESS);
    super.dispose ();
}

/**
 * Returns the receiver's default button if one had
 * previously been set, otherwise returns null.
 *
 * @return the default button or null
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see #setDefaultButton(Button)
 */
public Button getDefaultButton () {
    checkWidget();
    return defaultButton;
}

/**
 * Returns the receiver's image if it had previously been
 * set using <code>setImage()</code>. The image is typically
 * displayed by the window manager when the instance is
 * marked as iconified, and may also be displayed somewhere
 * in the trim when the instance is in normal or maximized
 * states.
 * <p>
 * Note: This method will return null if called before
 * <code>setImage()</code> is called. It does not provide
 * access to a window manager provided, "default" image
 * even if one exists.
 * </p>
 *
 * @return the image
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public Image getImage () {
    checkWidget ();
    return image;
}

/**
 * Returns <code>true</code> if the receiver is currently
 * maximized, and false otherwise.
 * <p>
 *
 * @return the maximized state
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see #setMaximized
 */
public boolean getMaximized () {
    checkWidget();
    return hasState(OS.QT_WINDOWMAXIMIZED);
}

/**
 * Returns the receiver's menu bar if one had previously
 * been set, otherwise returns null.
 *
 * @return the menu bar or null
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public Menu getMenuBar () {
    checkWidget();
    return menuBar;
}

/*
 *  Returns the handle to the menu bar. Creates an internal
 *  Menu bar if there is no menu bar set. If there is already
 *  a menu bar it returns the handle for that.
 */
public int internal_getOwnMenuBar(){
    if( menuBar != null && !menuBar.isDisposed() ){
        return menuBar.handle;
    }
    if ( ownMenuBar == 0 ){
        ownMenuBar = OS.QMenuBar_new(topHandle);
        if (parent == null ){
            OS.QMainWindow_setMenuBar(topHandle, ownMenuBar);
        }
        else{
            int layout = OS.QWidget_layout(topHandle);
            OS.QLayout_setMenuBar( layout,  ownMenuBar );
        }

    }
    return ownMenuBar;
}

/*
 * Destroys the own menu bar if there is one.
 */
public void internal_destroyOwnMenuBar(){
    if (ownMenuBar != 0 ){
        QObjectDeleteWrapper.deleteSafely( ownMenuBar );
        ownMenuBar = 0;
    }
}


/**
 * Returns <code>true</code> if the receiver is currently
 * minimized, and false otherwise.
 * <p>
 *
 * @return the minimized state
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see #setMinimized
 */
public boolean getMinimized () {
    checkWidget();
    return hasState(OS.QT_WINDOWMINIMIZED);
}

String getNameText () {
    return getText ();
}

/**
 * Returns the receiver's text, which is the string that the
 * window manager will typically display as the receiver's
 * <em>title</em>. If the text has not previously been set,
 * returns an empty string.
 *
 * @return the text
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public String getText () {
    checkWidget();
    return OS.QWidget_windowTitle(topHandle);
}

public boolean isReparentable () {
    checkWidget ();
    return false;
}

boolean isTabGroup () {
    return true;
}

boolean isTabItem () {
    return false;
}

Decorations menuShell () {
    return this;
}

void removeMenu (Menu menu) {
    if (menus == null) return;
    for (int i=0; i<menus.length; i++) {
        if (menus [i] == menu) {
            menus [i] = null;
            return;
        }
    }
}

void releaseChildren_pp (boolean destroy) {
    if (menuBar != null) {
        menuBar.release (false);
        menuBar = null;
    }
    internal_destroyOwnMenuBar();
    super.releaseChildren_pp (destroy);
    if (menus != null) {
        for (int i=0; i<menus.length; i++) {
            Menu menu = menus [i];
            if (menu != null && !menu.isDisposed ()) {
                menu.dispose ();
            }
        }
        menus = null;
    }
}

void releaseWidget_pp () {
    super.releaseWidget_pp ();
    image = null;
    savedFocus = null;
    defaultButton = saveDefault = null;
}

boolean restoreFocus () {
    if (savedFocus != null && savedFocus.isDisposed ()) savedFocus = null;
    boolean restored = savedFocus != null && savedFocus.setFocus (OS.QT_OTHERFOCUSREASON);
    /*
    * This code is intentionally commented.  When no widget
    * has been given focus, some platforms give focus to the
    * default button. We don't do this.
    */
//  if (restored) return true;
//  if (defaultButton != null && !defaultButton.isDisposed ()) {
//      if (defaultButton.setFocus ()) return true;
//  }
    return restored;
}

/**
 * If the argument is not null, sets the receiver's default
 * button to the argument, and if the argument is null, sets
 * the receiver's default button to the first button which
 * was set as the receiver's default button (called the
 * <em>saved default button</em>). If no default button had
 * previously been set, or the saved default button was
 * disposed, the receiver's default button will be set to
 * null.
 * <p>
 * The default button is the button that is selected when
 * the receiver is active and the user presses ENTER.
 * </p>
 *
 * @param button the new default button
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_INVALID_ARGUMENT - if the button has been disposed</li>
 *    <li>ERROR_INVALID_PARENT - if the control is not in the same widget tree</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void setDefaultButton (Button button) {
    checkWidget ();
    if (button != null) {
        if (button.isDisposed ()) error (SWT.ERROR_INVALID_ARGUMENT);
        if (button.menuShell () != this) error(SWT.ERROR_INVALID_PARENT);
    }
    setDefaultButton (button, true);
}

void setDefaultButton (Button button, boolean save) {
    if (button == null) {
        if (defaultButton == saveDefault) {
            if (save) saveDefault = null;
            return;
        }
    } else {
        if ((button.style & SWT.PUSH) == 0) return;
        if (button == defaultButton) return;
    }
    if (defaultButton != null) {
        if (!defaultButton.isDisposed ()) defaultButton.setDefault (false);
    }
    if ((defaultButton = button) == null) defaultButton = saveDefault;
    if (defaultButton != null) {
        if (!defaultButton.isDisposed ()) defaultButton.setDefault (true);
    }
    if (save) saveDefault = defaultButton;
    if (saveDefault != null && saveDefault.isDisposed ()) saveDefault = null;
}

/**
 * Sets the receiver's image to the argument, which may
 * be null. The image is typically displayed by the window
 * manager when the instance is marked as iconified, and
 * may also be displayed somewhere in the trim when the
 * instance is in normal or maximized states.
 *
 * @param image the new image (or null)
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_INVALID_ARGUMENT - if the image has been disposed</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void setImage (Image image) {
    checkWidget ();
    if (image != null && image.isDisposed()) error(SWT.ERROR_INVALID_ARGUMENT);
    this.image = image;
    int icon = (image != null ? Internal_GfxPackageSupport.getIconHandle(image) :
        Internal_GfxPackageSupport.getNullIconHandle());
    OS.QWidget_setWindowIcon(topHandle, icon);
}

/**
 * Sets the maximized state of the receiver.
 * If the argument is <code>true</code> causes the receiver
 * to switch to the maximized state, and if the argument is
 * <code>false</code> and the receiver was previously maximized,
 * causes the receiver to switch back to either the minimized
 * or normal states.
 * <p>
 * Note: The result of intermixing calls to <code>setMaximized(true)</code>
 * and <code>setMinimized(true)</code> will vary by platform. Typically,
 * the behavior will match the platform user's expectations, but not
 * always. This should be avoided if possible.
 * </p>
 *
 * @param maximized the new maximized state
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see #setMinimized
 */
public void setMaximized (boolean maximized) {
    checkWidget();
    // On Symbian platform top-level Shells are not resizeable.
    if(OS.windowServer == OS.WS_SYMBIAN_S60 && parent == null) {
        // If canceling maximized state then it must be to return to maximized
        // or minimized. It should not be possible to clear the maximized state
        // because then automatic resizing won't happen anymore.
        if(!maximized &&
                (restoreState != OS.QT_WINDOWMAXIMIZED) &&
                (restoreState != OS.QT_WINDOWMINIMIZED))
            return;
    }
    setRestoreState(OS.QT_WINDOWMAXIMIZED, !maximized);
}

final void storeState(int state) {
    if((state & OS.QT_WINDOWMINIMIZED) != 0) {
        restoreState = OS.QT_WINDOWMINIMIZED;
    } else if((state & OS.QT_WINDOWMAXIMIZED) != 0) {
        restoreState = OS.QT_WINDOWMAXIMIZED;
    } else {
        restoreState = 0;
    }
}

final boolean hasState(int state) {
    int bits = OS.QWidget_windowState(topHandle);
    return (bits & state) != 0;
}

final boolean sizeFixed() {
    return OS.QWidget_minimumSize(topHandle).equals(OS.QWidget_maximumSize(topHandle));
}

final void unlockSize() {
    OS.QWidget_setMaximumSize(topHandle, OS.QWIDGETSIZE_MAX, OS.QWIDGETSIZE_MAX);
    OS.QWidget_setMinimumSize(topHandle, 0, 0);
}

final void setState(final int state) {
    final int oldState = OS.QWidget_windowState(topHandle);
    if((oldState & state) != 0) return;
    storeState(oldState);
    int newState = oldState;
    int statesToRemove = (OS.QT_WINDOWFULLSCREEN|OS.QT_WINDOWMAXIMIZED|OS.QT_WINDOWMINIMIZED) & ~state;
    newState &= ~statesToRemove;
    newState |= state;

    // Full screen and maximize won't work if size is fixed. The fixed size
    // needs to be stored and then restored when going back to normal state.
    if (((newState & (OS.QT_WINDOWFULLSCREEN|OS.QT_WINDOWMAXIMIZED)) != 0) &&
            ((oldState & (OS.QT_WINDOWFULLSCREEN|OS.QT_WINDOWMAXIMIZED)) == 0)) {
        if (sizeFixed()) {
            fixedSize = OS.QWidget_minimumSize(topHandle);
            style |= SWT.Resize;
            unlockSize();
        }
    }

    // Going directly from full screen to maximized doesn't work. Need to go normal first.
    if((newState & OS.QT_WINDOWMAXIMIZED)!= 0 && (oldState & OS.QT_WINDOWFULLSCREEN)!= 0) {
        OS.QWidget_setWindowState(topHandle, 0);
    }

    OS.QWidget_setWindowState(topHandle, newState);

    // Restore the fixed size if back to normal
    if (((newState & (OS.QT_WINDOWFULLSCREEN|OS.QT_WINDOWMAXIMIZED)) == 0)) {
        if (fixedSize != null) {
            OS.QWidget_setFixedSize(topHandle, fixedSize.x, fixedSize.y);
            style &= ~SWT.Resize;
            fixedSize = null;
        }
    }
}

final void restoreState() {
    // This might not restore it from minimized if wm doesn't allow it.
    // It might e.g. blink in the task bar to indicate to the user
    // that the window wants to become active.
    if(restoreState == OS.QT_WINDOWMINIMIZED) {
        setState(OS.QT_WINDOWMINIMIZED);
    } else if(restoreState == OS.QT_WINDOWMAXIMIZED) {
        setState(OS.QT_WINDOWMAXIMIZED);
        bringToTop(false);
    } else {
        setState(OS.QT_WINDOWNOSTATE);
        bringToTop(false);
    }
    restoreState = 0;
}

void bringToTop (boolean force) {
    if(isDisposed()) return;
    OS.QWidget_activateWindow(topHandle);
    OS.QWidget_raise(topHandle);
}

/**
 * Sets the receiver's menu bar to the argument, which
 * may be null.
 *
 * @param menu the new menu bar
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_INVALID_ARGUMENT - if the menu has been disposed</li>
 *    <li>ERROR_INVALID_PARENT - if the menu is not in the same widget tree</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void setMenuBar (Menu menu) {
    checkWidget();
    if (menuBar == menu) return;

    if (menu != null) {
        if (menu.isDisposed()) error(SWT.ERROR_INVALID_ARGUMENT);
        if ((menu.style & SWT.BAR) == 0) error (SWT.ERROR_MENU_NOT_BAR);
        if (menu.parent != this) error (SWT.ERROR_INVALID_PARENT);
        // Feature on Qt: Setting a new menu bar causes deletion of the old menu bar.
        // in order to adjust shell disposes the menu.
        if ( menuBar !=null ) menuBar.dispose();
        if (parent == null ){
        OS.QMainWindow_setMenuBar(topHandle, menu.handle);
        }else{
            int layout = OS.QWidget_layout(topHandle);
            OS.QLayout_setMenuBar( layout,  menu.handle );
        }

        menuBar = menu;
        ownMenuBar = 0;
        //Inform CommandArranger
        display.commandArranger.menuBarChanged( this );
    }
}

/**
 * Sets the minimized stated of the receiver.
 * If the argument is <code>true</code> causes the receiver
 * to switch to the minimized state, and if the argument is
 * <code>false</code> and the receiver was previously minimized,
 * causes the receiver to switch back to either the maximized
 * or normal states.
 * <p>
 * Note: The result of intermixing calls to <code>setMaximized(true)</code>
 * and <code>setMinimized(true)</code> will vary by platform. Typically,
 * the behavior will match the platform user's expectations, but not
 * always. This should be avoided if possible.
 * </p>
 *
 * @param minimized the new maximized state
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 *
 * @see #setMaximized
 */
public void setMinimized (boolean minimized) {
    checkWidget();
    setRestoreState(OS.QT_WINDOWMINIMIZED, !minimized);
}

final void setRestoreState(int state, boolean restore) {
    int oldState = OS.QWidget_windowState(topHandle);
    if(!restore) {
        setState(state);
    } else {
        if((oldState & state) == 0) return;
        restoreState();
    }

}

void setSavedFocus (Control control) {
    savedFocus = control;
}

/**
 * Sets the receiver's text, which is the string that the
 * window manager will typically display as the receiver's
 * <em>title</em>, to the argument, which must not be null.
 *
 * @param string the new text
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the text is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public void setText (String string) {
    checkWidget();
    if (string == null) error (SWT.ERROR_NULL_ARGUMENT);
    OS.QWidget_setWindowTitle(topHandle, string);
    OS.QWidget_setWindowIconText(topHandle, string);
}

boolean traverseItem (boolean next) {
    return false;
}

boolean invokeDefaultButton() {
    if (defaultButton != null && !defaultButton.isDisposed()) {
        int dbHandle = defaultButton.handle;
        if (OS.QPushButton_isDefault(dbHandle) && defaultButton.isVisible()
                && defaultButton.isEnabled()) {
            OS.QAbstractButton_click(dbHandle);
            return true; // success
        }
    }
    return false; // failure
}

void saveFocus () {
    Control control = display._getFocusControl ();
    if (control != null && control != this && this == control.menuShell ()) {
        setSavedFocus (control);
    }
}

// If the window will get decoration eventually
boolean willBeDecorated() {
    if(OS.windowServer == OS.WS_SYMBIAN_S60) return false;
    int flags = OS.QWidget_windowFlags(topHandle);
    if((flags & OS.QT_WINDOWFLAGS_FRAMELESSWINDOWHINT) != 0)
        return false;
    return true;
}

// If the window will get the normal thick decoration (title bar, buttons, etc.)
boolean thickFrame() {
    return willBeDecorated() && !thinFrame();
}

// If the window will get the thin decoration
boolean thinFrame() {
    if(OS.windowServer == OS.WS_SYMBIAN_S60) return false;
    if((style & SWT.RESIZE) != 0) return false;
    int flags = OS.QWidget_windowFlags(topHandle);
    if((flags &
            (OS.QT_WINDOWFLAGS_WINDOWMAXIMIZEBUTTONHINT|
            OS.QT_WINDOWFLAGS_WINDOWMINIMIZEBUTTONHINT|
            OS.QT_WINDOWFLAGS_WINDOWSYSTEMMENUHINT|
            OS.QT_WINDOWFLAGS_WINDOWTITLEHINT)) == 0) {
        return true;
    }
    return false;
}

// The window trim around the left/bottom/right edges
int windowFrameTrim() {
    if(!willBeDecorated())
        return 0;

    if(thinFrame()) {
        return leftThinFrame != -1 ? leftThinFrame : 4;
    } else {
        return leftTitleFrame != -1 ? leftTitleFrame : 5;
    }
}

// The window trim above the top edge
int windowTitleTrim() {
    if(!willBeDecorated())
        return 0;

    if(thinFrame()) {
        return topThinFrame > -1 ? topThinFrame : 4;
    } else {
        return topTitleFrame > -1 ? topTitleFrame : 24;
    }
}

boolean verifyTrim(int topTrim, int leftTrim, int rightTrim, int bottomTrim) {
    if(willBeDecorated() && (topTrim == 0 || leftTrim == 0 || rightTrim == 0 || bottomTrim == 0))
        return false;
    if(!willBeDecorated() && (topTrim != 0 || leftTrim != 0 || rightTrim != 0 || bottomTrim != 0))
        return false;
    if(leftTrim < 0 || topTrim < 0 || bottomTrim < 0 || rightTrim < 0)
        return false;
    if(leftTrim != rightTrim)
        return false;
    if(thickFrame() && (topTrim == bottomTrim))
        return false;
    return true;
}

void learnTrim() {
    if(!willBeDecorated()) return;

    int state = OS.QWidget_windowState(topHandle);
    if((state & OS.QT_WINDOWMINIMIZED) != 0) return;
    if((state & OS.QT_WINDOWFULLSCREEN) != 0) return;

    // Try to measure the window frame from the current state of the widget.
    // This doesn't return correctly if window manager has not decorated the
    // window yet.
    Rectangle frameRect = OS.QWidget_frameGeometry(topHandle);
    Rectangle rect = OS.QWidget_geometry(topHandle);
    int leftTrim = rect.x - frameRect.x;
    int topTrim = rect.y - frameRect.y;
    int bottomTrim = frameRect.height - rect.height - topTrim;
    int rightTrim = frameRect.width - rect.width - leftTrim;

    // Ignore wrong results
    if(!verifyTrim(topTrim, leftTrim, rightTrim, bottomTrim)) return;

    // Learn the normal frame
    if(thickFrame()) {
        if(topTrim > topTitleFrame) topTitleFrame = topTrim;
        if(leftTrim > leftTitleFrame) leftTitleFrame = leftTrim;
        if(rightTrim > rightTitleFrame) rightTitleFrame = rightTrim;
        if(bottomTrim > bottomTitleFrame) bottomTitleFrame = bottomTrim;
    }
    // Learn the thin frame
    else if (thinFrame()) {
        if(topTrim > topThinFrame) topThinFrame = topTrim;
        if(leftTrim > leftThinFrame) leftThinFrame = leftTrim;
        if(rightTrim > rightThinFrame) rightThinFrame = rightTrim;
        if(bottomTrim > bottomThinFrame) bottomThinFrame = bottomTrim;
    }
}

boolean qt_event_focusOut(int reason) {
    boolean result = super.qt_event_focusOut(reason);
    saveFocus ();
    return result;
}


void qt_swt_event_focusWasGained() {
    super.qt_swt_event_focusWasGained();
    if (savedFocus != this) restoreFocus ();
}

void closeWidget () {
    Event event = new Event ();
    sendEvent (SWT.Close, event);
    if (event.doit && !isDisposed ()) dispose ();
}


boolean qt_event_close() {
    if (isEnabled()) closeWidget ();
    return true;
}

boolean qt_event_windowActivate(int widgetHandle) {
    if(widgetHandle == topHandle) {
        sendEvent(SWT.Activate);
    }
    return false;
}

boolean qt_event_windowDeactivate(int widgetHandle) {
    if(widgetHandle == topHandle) {
        sendEvent(SWT.Deactivate);
        saveFocus();
    }
    return false;
}

boolean qt_event_windowStateChange(int widgetHandle, int oldState) {
    int newState = OS.QWidget_windowState(topHandle);
    if((oldState & OS.QT_WINDOWMINIMIZED) != 0 && (newState & OS.QT_WINDOWMINIMIZED) == 0) {
        sendEvent(SWT.Deiconify);
        return false;
    }
    if((oldState & OS.QT_WINDOWMINIMIZED) == 0 && (newState & OS.QT_WINDOWMINIMIZED) != 0) {
        sendEvent(SWT.Iconify);
        /* Save the focus widget when the shell is minimized */
        menuShell ().saveFocus ();
        return false;
    }
    return false;
}
}
