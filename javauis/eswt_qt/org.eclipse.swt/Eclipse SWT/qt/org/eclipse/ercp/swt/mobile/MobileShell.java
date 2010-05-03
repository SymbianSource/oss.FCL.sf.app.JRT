/*******************************************************************************
 * Copyright (c) 2004, 2008 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (c) 2005 Motorola Inc.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Yu You (Nokia Corp.) - initial API specification
 *     Motorola Inc. - key state polling
 *     Nokia Corporation - S60 implementation
 *     Nokia Corporation - QT implementation
 *******************************************************************************/
package org.eclipse.ercp.swt.mobile;

import java.util.Enumeration;
import java.util.Vector;

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.PackageProxy;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Shell;

/**
 * A shell particularly suitable for devices that require dynamic change of
 * trims at run-time.
 * 
 * <p>
 * The shell is a "top-level" window that can usually be visible and managed by
 * the user or a window manager. Two unique features of this shell over a normal
 * Shell are a new method <code>changeTrim(int, int)</code> to dynamically
 * change Shell trim styles, and full-screen mode.
 * </p>
 * <p>
 * MobileShell uses the whole available application space in the normal mode. In
 * this mode, a little screen area (title bar) is often reserved to display text
 * with or without an icon, and optional trim information. The text shows the
 * application name text by default. See <code>setText(String)</code> to change
 * the text value and <code>setImage(Image)</code> to set the icon.
 * </p>
 * <p>
 * Outside the application screen space, mobile devices often reserve a screen
 * space, named status area, for displaying some extra information regarding the
 * device and other useful data. Such reserved space is usually accessable and
 * programmable. MobileShell provides a method <code>setStatus(String)</code> to
 * display text on the status area. It is up to the implementation to decide how
 * to process the string, for example, if the content is too long to display.
 * </p>
 * <p>
 * Unlike a normal shell, MobileShell supports a special screen mode to use the
 * whole device screen space, instead of the application space in the normal
 * mode. Applications can call a method <code>setFullScreenMode(boolean)</code>,
 * to switch between normal and full-screen modes at run-time. This feature is
 * often used in applications like media player and web browser, where those
 * types of applications can request for a full-screen mode for better rendering
 * effects.
 * </p>
 * <p>
 * Note: The style SWT.NO_TRIM does not force changing to full-screen mode, but
 * is a trimming style in which usually no title bar is visible.
 * </p>
 * 
 * <dl>
 * <dt><b>Trim Styles: </b></dt>
 * MobileShell inherits styles from the Shell class, including:
 * <dd>SHELL_TRIM</dd>
 * <dd>DIALOG_TRIM</dd>
 * <dd>NO_TRIM</dd>
 * <dd>TITLE</dd>
 * <dd>CLOSE</dd>
 * <dd>BORDER</dd>
 * </dl>
 * 
 * <dl>
 * <dt><b>Status Styles: </b></dt>
 * MobileShell introduces the concept of status pane and status pane style
 * hints. When a status pane hint is not specified, the platform will choose a
 * status pane style or no status pane.
 * <dd>NO_STATUS_PANE</dd>
 * <dd>SMALL_STATUS_PANE</dd>
 * <dd>LARGE_STATUS_PANE</dd>
 * </dl>
 * 
 * @see org.eclipse.swt.SWT#BORDER
 * @see org.eclipse.swt.SWT#CLOSE
 * @see org.eclipse.swt.SWT#TITLE
 * @see org.eclipse.swt.SWT#NO_TRIM
 * @see org.eclipse.swt.SWT#SHELL_TRIM
 * @see org.eclipse.swt.SWT#DIALOG_TRIM
 * @see org.eclipse.swt.SWT#MODELESS
 * @see org.eclipse.swt.SWT#PRIMARY_MODAL
 * @see org.eclipse.swt.SWT#APPLICATION_MODAL
 * @see org.eclipse.swt.SWT#SYSTEM_MODAL
 * @see #NO_STATUS_PANE
 * @see #SMALL_STATUS_PANE
 * @see #LARGE_STATUS_PANE <p>
 *      IMPORTANT: This class is <em>not</em> intended to be subclassed.
 *      </p>
 */
public class MobileShell extends Shell {

    String title;
    Vector keyCodeVector = new Vector(10,10);
    Listener keyListener;

    /**
     * Hint to show no status pane
     */
    public static final int NO_STATUS_PANE = 1;

    /**
     * Hint to show small status pane with shell
     */
    public static final int SMALL_STATUS_PANE = 2;

    /**
     * Hint show large status pane with shell
     */
    public static final int LARGE_STATUS_PANE = 3;
    
    static final class MobileShellPackageProxy extends PackageProxy {
    }

    /**
     * Constructs a new instance of this class given only the style value
     * describing its behavior and appearance. This is equivalent to calling
     * <code>Shell((Display) null, style)</code>.
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or must
     * be built by <em>bitwise OR</em> 'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. The class description lists the style constants that are
     * applicable to the class. Style bits are also inherited from superclasses.
     * </p>
     * 
     * @param style
     *            the style of control to construct
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
     *                allowed subclass</li>
     *                </ul>
     * 
     * @see org.eclipse.swt.SWT#BORDER
     * @see org.eclipse.swt.SWT#CLOSE
     * @see org.eclipse.swt.SWT#TITLE
     * @see org.eclipse.swt.SWT#NO_TRIM
     * @see org.eclipse.swt.SWT#SHELL_TRIM
     * @see org.eclipse.swt.SWT#DIALOG_TRIM
     * @see org.eclipse.swt.SWT#MODELESS
     * @see org.eclipse.swt.SWT#PRIMARY_MODAL
     * @see org.eclipse.swt.SWT#APPLICATION_MODAL
     * @see org.eclipse.swt.SWT#SYSTEM_MODAL
     */
    public MobileShell(int style) {
        this((Display) null, style);
    }

    /**
     * Constructs a new instance of this class given only the display to create
     * it on. It is created with style <code>SWT.SHELL_TRIM</code>.
     * <p>
     * Note: Currently, null can be passed in for the display argument. This has
     * the effect of creating the shell on the currently active display if there
     * is one. If there is no current display, the shell is created on a
     * "default" display. <b>Passing in null as the display argument is not
     * considered to be good coding style, and may not be supported in a future
     * release of SWT. </b>
     * </p>
     * 
     * @param display
     *            the display to create the shell on. Can be null.
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
     *                allowed subclass</li>
     *                </ul>
     */
    public MobileShell(Display display) {
        this(display, SWT.SHELL_TRIM);
    }

    /**
     * Constructs a new instance of this class given the display to create it on
     * and a style value describing its behavior and appearance.
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or must
     * be built by <em>bitwise OR</em> 'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. The class description lists the style constants that are
     * applicable to the class. Style bits are also inherited from superclasses.
     * </p>
     * <p>
     * Note: Currently, null can be passed in for the display argument. This has
     * the effect of creating the shell on the currently active display if there
     * is one. If there is no current display, the shell is created on a
     * "default" display. <b>Passing in null as the display argument is not
     * considered to be good coding style, and may not be supported in a future
     * release of SWT. </b>
     * </p>
     * 
     * @param display
     *            the display to create the shell on. Can be null.
     * @param style
     *            the style of control to construct
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
     *                allowed subclass</li>
     *                </ul>
     * 
     * @see org.eclipse.swt.SWT#CLOSE
     * @see org.eclipse.swt.SWT#TITLE
     * @see org.eclipse.swt.SWT#NO_TRIM
     * @see org.eclipse.swt.SWT#SHELL_TRIM
     * @see org.eclipse.swt.SWT#DIALOG_TRIM
     * @see org.eclipse.swt.SWT#MODELESS
     * @see org.eclipse.swt.SWT#PRIMARY_MODAL
     * @see org.eclipse.swt.SWT#APPLICATION_MODAL
     * @see org.eclipse.swt.SWT#SYSTEM_MODAL
     */
    public MobileShell(Display display, int style) {
        this(display, style, LARGE_STATUS_PANE);
    }

    /**
     * Constructs a new instance of this class given the display to create it
     * on, a style value describing its behavior and appearance, and a status
     * style describing the appearance of a status pane.
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or must
     * be built by <em>bitwise OR</em> 'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. The class description lists the style constants that are
     * applicable to the class. Style bits are also inherited from superclasses.
     * </p>
     * <p>
     * Note: Currently, null can be passed in for the display argument. This has
     * the effect of creating the shell on the currently active display if there
     * is one. If there is no current display, the shell is created on a
     * "default" display. <b>Passing in null as the display argument is not
     * considered to be good coding style, and may not be supported in a future
     * release of SWT. </b>
     * </p>
     * 
     * @param display
     *            the display to create the shell on. Can be null.
     * @param style
     *            the style of control to construct
     * @param statusStyle
     *            the style of the status pane
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
     *                allowed subclass</li>
     *                </ul>
     * 
     * @see org.eclipse.swt.SWT#CLOSE
     * @see org.eclipse.swt.SWT#TITLE
     * @see org.eclipse.swt.SWT#NO_TRIM
     * @see org.eclipse.swt.SWT#SHELL_TRIM
     * @see org.eclipse.swt.SWT#DIALOG_TRIM
     * @see org.eclipse.swt.SWT#MODELESS
     * @see org.eclipse.swt.SWT#PRIMARY_MODAL
     * @see org.eclipse.swt.SWT#APPLICATION_MODAL
     * @see org.eclipse.swt.SWT#SYSTEM_MODAL
     * @see #NO_STATUS_PANE
     * @see #SMALL_STATUS_PANE
     * @see #LARGE_STATUS_PANE
     */
    public MobileShell(Display display, int style, int statusStyle) {
        super(display, null, style, new MobileShellPackageProxy());
        if (!Internal_PackageSupport.isValidSubclass(this)) SWT.error(SWT.ERROR_INVALID_SUBCLASS);
        changeTrim(style, statusStyle);
        init();
    }

    /**
     * Report whether the key has been pressed. A return value of true indicates
     * the key is pressed or has been pressed since the state of this key was
     * last checked. A return value of false indicates the key has not been
     * pressed since the state of this key was last checked. Only the active
     * Shell can return true for this method. Calling this method on a Shell
     * that is not the currently active Shell will always return false and will
     * not clear the key's "has-been-pressed" state.
     * <p>
     * Values of keyCode that are not valid for a given platform will result in
     * a return value of false.
     * <p>
     * An application gaining focus should call <code>getKeyState()</code> for
     * each key it is nterested in polling in order to clear the key's
     * "has-been-pressed" state.
     * 
     * @param keyCode
     *            keyCode of the key to be check
     * 
     * @return true if this shell is the active Shell and the key is pressed or
     *         has been pressed since the last time the state of this key was
     *         checked. Otherwise, return false.
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public boolean getKeyState(int keyCode) {
        checkWidget();
        Display display = getDisplay();
        if (display.getActiveShell() != this) return false; // Current shell is not active
        
        Enumeration keyCodeIterator = keyCodeVector.elements();
        int index = 0;
        while(keyCodeIterator.hasMoreElements()) {
            Integer keyCodeInteger = (Integer)(keyCodeIterator.nextElement());
            if (keyCodeInteger.intValue() == keyCode) {
                keyCodeVector.removeElementAt(index);
                return true;
            }
            index ++;
        }   
        return false;
    }

    /**
     * Sets the screen mode
     * 
     * This method controls whether the shell is in normal or full-screen mode.
     * If the argument is <code>true</code> it causes the shell to switch to the
     * full-screen state, and if the argument is <code>false</code> and the
     * shell was in full-screen state, it causes the shell to switch back to the
     * normal state.
     * <p>
     * 
     * @param mode
     *            the boolean value
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public void setFullScreenMode(boolean mode) {
        checkWidget();
        Internal_PackageSupport.setRestoreState(this, OS.QT_WINDOWFULLSCREEN, !mode);
    }

    /**
     * Sets the screen mode
     * 
     * This method controls whether the shell is in normal or full-screen mode.
     * If the <code>fullScreenMode</code> argument is <code>true</code>, it causes the shell 
     * to switch to the full-screen state, and if the argument is <code>false</code> it 
     * causes the shell to switch back to the normal state.
     * If the <code>showVKBSelector</code> argument is <code>true</code>,
     * the Virtual Keyboard Selector used for displaying a virtual keyboard or other input
     * mechanism will be made visible, if the platform supports it. Otherwise, a
     * Virtual Keyboard Selector will not be visible.
     * If any Commands are present, the Commands can still be invoked. This means
     * a Command menu or softkey label area may use a portion of the screen.
     * <p>
     * 
     * @param fullScreenMode
     *            whether the shell should use the full screen
     *            
     * @param showVKBSelector
     *            whether the Virtual Keyboard Selector should be displayed
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setFullScreenMode(boolean fullScreenMode, boolean showVKBSelector) {
        // TODO: VKB
        setFullScreenMode(fullScreenMode);
    }
    
    /**
     * Changes the window trim mode. The implementation decides how to re-layout
     * the content if the style changes. The method will throw an
     * IllegalArgumentException if the given style is invalid.
     * 
     * @param style
     *            the trim style
     * @param statusStyle
     *            the status pane style
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the given style is invalid
     *                </li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     * 
     * @see org.eclipse.swt.SWT#CLOSE
     * @see org.eclipse.swt.SWT#TITLE
     * @see org.eclipse.swt.SWT#NO_TRIM
     * @see org.eclipse.swt.SWT#SHELL_TRIM
     * @see org.eclipse.swt.SWT#DIALOG_TRIM
     * @see #NO_STATUS_PANE
     * @see #SMALL_STATUS_PANE
     * @see #LARGE_STATUS_PANE
     */
    public void changeTrim(int style, int statusStyle) {
        checkWidget();
        Internal_PackageSupport.checkStyle(style, null);
        int validStatusStyle = ~(NO_STATUS_PANE | SMALL_STATUS_PANE | LARGE_STATUS_PANE);
        if ((statusStyle & validStatusStyle) != 0) {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        
        // Setting of style
        if(OS.windowServer == OS.WS_X11) {
            /*
             * Setting window flags on-the-fly doesn't change the window frame. Thus,
             * it's not supported. It's a bad idea to change the flags if they don't do
             * anything because there are assumptions made based on the current flags
             * about what kind of frame the window has.
             */
        } else {
            // TODO: Check if this is ok for S60
            style &= ~(SWT.SHELL_TRIM|SWT.BORDER);
            style |= style;
            OS.QWidget_setWindowFlags(topHandle(), Internal_PackageSupport.windowFlagsFromStyle(this));
        }
        
        // TODO: Setting of statusStyle
    }

    /**
     * 
     * Gets the full-screen state.
     * 
     * Returns true if in full screen mode, false otherwise.
     * 
     * @return Boolean the current screen mode.
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public boolean getFullScreenMode() {
        checkWidget();
        return Internal_PackageSupport.hasState(this, OS.QT_WINDOWFULLSCREEN);
    }

    /**
     * Sets the status text.
     * 
     * The change will take effect immediately.
     * 
     * @param statusText
     *            the status string value. It can be NULL.
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public void setStatusText(String statusText) {
        checkWidget();
        title = statusText;
    }

    /**
     * 
     * Returns the status text.
     * 
     * @return String - The current status text.
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public String getStatusText() {
        checkWidget();
        return title;
    }

    private void init() {
    final Display display = this.getDisplay();
    final MobileShell mobileShell = this;
    keyListener = new Listener() {

        public void handleEvent(Event event) {
            // Only records the key event at currently active MobileShell
            if (display.getActiveShell() == mobileShell) { 
                Enumeration keyCodeIterator = keyCodeVector.elements();
                boolean hasEntry = false;
                while (keyCodeIterator.hasMoreElements()) {
                    Integer keyCode = (Integer) (keyCodeIterator.nextElement());
                    if (keyCode.intValue() == event.keyCode) {
                        hasEntry = true;
                        break;
                    }
                }
                // If the keycode doesn't exist in vector, add it
                if (!hasEntry) { 
                    Integer addKeycode = new Integer(event.keyCode);
                    keyCodeVector.addElement(addKeycode);
                }
            }

        }
    };
    display.addFilter(SWT.KeyDown, keyListener);

    mobileShell.addDisposeListener(new DisposeListener() {
        public void widgetDisposed(DisposeEvent e) {
            if (mobileShell != null && !mobileShell.isDisposed()) {
                mobileShell.getDisplay().removeFilter(SWT.KeyDown, keyListener);
            }
        }

    });
}
    
private final int topHandle() {
    return Internal_PackageSupport.topHandle(this);
}
}
