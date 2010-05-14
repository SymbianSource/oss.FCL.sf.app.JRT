/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - QT implementation
 *******************************************************************************/
package org.eclipse.ercp.swt.mobile;

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Internal_GfxPackageSupport;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.PackageProxy;
import org.eclipse.swt.internal.qt.WidgetState;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Dialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.TypedListener;
import org.eclipse.swt.widgets.Widget;
import org.eclipse.swt.layout.FillLayout;;

/**
 * Instances of this class represents a tabbed dialog. The dialog contains
 * multiple pages. Each page contains a composite. At any given time, only one
 * page is visible. The page visibility can be selected by end-users or
 * applications programmatically.
 * <p>
 * Each page is assigned with a label. The label can be either a text or an
 * icon, or all together. The size and position of page labels is
 * implementation-dependent.
 * </p>
 * <p>
 * There must not be any limit on the number of pages, when the memory is
 * sufficient. Run-time errors may be thrown when the resource is insufficient
 * to perform this call.</p>
 * <p>
 * Note: The bounds of this dialog is implementation-dependent. Applications can
 * query the page size through <code>Composite.getBounds</code> after the page
 * has been created.
 * </p>
 * <h3>Example:</h3>
 * <code><pre>
 * MultiPageDialog dialog = new MultiPageDialog(shell);
 * Composite page1 = dialog.createPage(&quot;Page 1&quot;, null);
 * // add child controls here 
 * Command okCMD = new Command(page1, Command.OK, 1);
 * okCMD.setText(&quot;Ok&quot;);
 * okCMD.addCommandListener(new CommandListener() {
 *  public void commandAction(CommandEvent e) {
 *      // do something here before closing the dialog
 *      dialog.close(); // Make the dialog invisible
 *      dialog.dispose(); // Dispose the dialog and its children.
 *  }
 * });
 * dialog.setSelection(dialog.getPageCount() - 1); // The last page
 * dialog.open(); // open a modal dialog
 * </pre></code>
 * <dl>
 * <dt><b>Styles: </b></dt>
 * <dd>(none)</dd>
 * <dt><b>Events: </b></dt>
 * <dd>Selection</dd>
 * </dl>
  * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
*/
public class MultiPageDialog extends Dialog {
    
    /*
     * The dialog shell. 
     */
    private Shell shell;
    
    /*
     * The tab widget of the dialog.
     */
    private internal_TabControl tabControl;
    
    /*
     * Flag that is set while the dialog is open.
     */
    private boolean isOpen;

    /*
     * Flag for ignoring the selection events when selection is modified by application.
     */
    private boolean ignoreSelectionEvent;

    /*
     * The package proxy of internal_tabControl. This is not in internal_Tabcontrol
     * because a non-static inner class can't have a static inner class. 
     */
    static final class internal_TabControlPackageProxy extends PackageProxy {
        public void createHandle(int index) {
            ((internal_TabControl)w).createHandle(index);
        }
        public void hookEvents() {
            ((internal_TabControl)w).hookEvents();
        }
        public boolean qt_event_keypress( int widgetHandle, int key, int modifier, int character, int nativeScanCode ) {
            return ((internal_TabControl)w).qt_event_keypress(widgetHandle, key, modifier, character, nativeScanCode);
        }
        public boolean qt_event_keyrelease( int widgetHandle, int key, int modifier, int character, int nativeScanCode ) {
            return ((internal_TabControl)w).qt_event_keyrelease(widgetHandle, key, modifier, character, nativeScanCode);
        }
        public boolean qt_event_mouseButtonPress (int widgetHandle, int button, int x, int y, int state, int buttons) {
            return ((internal_TabControl)w).qt_event_mouseButtonPress(widgetHandle, button, x, y, state, buttons);
        }
        public boolean qt_event_mouseButtonRelease(int widgetHandle, int button, int x, int y, int state, int buttons) {
            return ((internal_TabControl)w).qt_event_mouseButtonRelease(widgetHandle, button, x, y, state, buttons);
        }
        public void qt_signal_current_tab_changed(int index) {
            ((internal_TabControl)w).qt_signal_current_tab_changed(index);
        }
        public boolean setTabGroupFocus() {
            return ((internal_TabControl)w).setTabGroupFocus();
        }
        public int handleWithChildren() {
            return ((internal_TabControl)w).handleWithChildren();
        }
    }
    
    /*
     * Internal class that is a Java peer for the native QTabWidget. Instance of this class is the parent 
     * for page Composites that are created to the dialog.
     */
    private class internal_TabControl extends Composite {

        boolean virtual;
        int stackedWidgetHandle;
        int tabBarHandle;
        
        internal_TabControl(Shell parent, boolean virtual, int style) {
            super(parent, style & SWT.RIGHT_TO_LEFT, 0, new internal_TabControlPackageProxy(), false);
            this.virtual = virtual;
            OS.QObject_setProperty(tabBarHandle, "swt_obj_idx", OS
                    .QObject_property(handle(), "swt_obj_idx"));
        }
    
        public void checkWidget() {
            super.checkWidget();
        }
        
        void createHandle(int index) {
            Internal_PackageSupport.setScrollAreaHandle(this, 0);
            int tabwidget = OS.QSwtTabWidget_new();
            Internal_PackageSupport.setHandle(this, tabwidget);
            Internal_PackageSupport.setTopHandle(this, tabwidget);
            OS.QWidget_setFocusPolicy(handle(), OS.QT_FOCUSPOLICY_STRONGFOCUS);
            stackedWidgetHandle = OS.QTabWidget_swt_stackedWidgetHandle(handle());
            tabBarHandle = OS.QTabWidget_tabBar(handle());
            Internal_PackageSupport.setState(this, Internal_PackageSupport.state(this) | WidgetState.HANDLE | WidgetState.CANVAS);
        }
      
        /*
         * Overridden from the base class because children i.e. page Composites need to be fetched 
         * from the QStackedWidget which is a child of QTabWidget.
         */
        int handleWithChildren () {
            return stackedWidgetHandle;
        }
        
        void hookEvents() {
            Internal_PackageSupport.hookEvents(this);
            int signalProxy = OS.SignalHandler_new(handle(), Internal_PackageSupport.display(this),
                    OS.QSIGNAL_CURRENT_TAB_CHANGED);
            OS.QObject_connectOrThrow(handle(), "currentChanged(int)", signalProxy,
                    "widgetSignal(int)", OS.QT_AUTOCONNECTION);
        }
        
        int topHandle() {
            return handle();
        } 
        
        void qt_signal_current_tab_changed(int index) {
            if (isOpen) {
                sendSetDataEvent(index);
            }
            
            if (!ignoreSelectionEvent) {
                Event event = createEvent(SWT.Selection, (index < 0) ? null : getPage(index));
                Internal_PackageSupport.sendEvent(this, SWT.Selection, event);
            }
        }
        
        public void removeSelectionListener(SelectionListener listener) {
            this.removeListener(SWT.Selection, listener);
        }
        
        public void sendSetDataEvent(int index) {
            if (!virtual || index < 0) return; 
            
            internal_PageComposite page = (internal_PageComposite)getPage(index);
            if (!page.setDataSent) {
                Internal_PackageSupport.sendEvent(this, SWT.SetData, createEvent(SWT.SetData, page));
                page.setDataSent = true;
                page.setFocus();
            }
        }
        
        private Event createEvent(int type, Composite page) {
            Event event = new Event();
            event.type = type;
            event.display = Internal_PackageSupport.display(this);
            event.item = page;
            event.time = 0; 
            return event;
        }
  
        
        boolean qt_event_keypress(int widgetHandle, int key,
                int modifier, int character, int nativeScanCode) {
            if (!Internal_PackageSupport.hasFocus(this)) {
                return false;
            }
            boolean left = (key == OS.QT_KEY_LEFT);
            if (left || key == OS.QT_KEY_RIGHT) {
                // left key and right are used to switch pages for horizontal layout tab bar
                if ((Internal_PackageSupport.style(this) & SWT.MIRRORED) != 0) {
                    left = !left;
                }
                if (left) {
                    if (OS.QTabWidget_currentIndex(handle()) != 0) {
                        return Internal_PackageSupport.sendKeyEvent(this, SWT.KeyDown, key, modifier,
                                character, nativeScanCode);
                    }
                } else {
                    if (OS.QTabWidget_currentIndex(handle()) < (OS
                            .QTabWidget_count(handle()) - 1)) {
                        return Internal_PackageSupport.sendKeyEvent(this, SWT.KeyDown, key, modifier,
                                character, nativeScanCode);
                    }
                }
            }
            return Internal_PackageSupport.qt_event_keypress(this, widgetHandle, key, modifier,
                    character, nativeScanCode);
        }

        boolean qt_event_keyrelease( int widgetHandle, int key, int modifier, int character, int nativeScanCode )
        {
            if (Internal_PackageSupport.hasFocus(this)) {
                return Internal_PackageSupport.sendKeyEvent(this, SWT.KeyUp, key, modifier, character, nativeScanCode);
            }
            return false;
        }

        boolean qt_event_mouseButtonDblClick(int widgetHandle, int button,
                int x, int y, int state, int buttons) {
            if (isEnabled()) {
                Point p = Internal_PackageSupport.mapToClientArea(this, tabBarHandle, x, y);
                return Internal_PackageSupport.sendMouseEvent(this, SWT.MouseDoubleClick, button, false, p.x, p.y,
                        state, buttons);
            }
            return false;
            }

        boolean qt_event_mouseButtonPress(int widgetHandle, int button,
                int x, int y, int state, int buttons) {
            if (isEnabled()) {
                Point p = Internal_PackageSupport.mapToClientArea(this, tabBarHandle, x, y);
                return Internal_PackageSupport.sendMouseEvent(this, SWT.MouseDown, button, false, p.x, p.y,
                        state, buttons);
            }
            return false;
        }

        boolean qt_event_mouseButtonRelease(int widgetHandle, int button,
                int x, int y, int state, int buttons) {
            if (isEnabled()) {
                Point p = Internal_PackageSupport.mapToClientArea(this, tabBarHandle, x, y);
                return Internal_PackageSupport.sendMouseEvent(this, SWT.MouseUp, button, false, p.x, p.y,
                        state, buttons);
            }
            return false;
        }
        
        // give focus to QTabBar to enable keyboard navigation
        boolean setTabGroupFocus() {
            if (!Internal_PackageSupport.isShowing(this))
                return false;
            return forceFocus();
            }
        
        private final int handle() {
            return Internal_PackageSupport.handle(this); 
        }
    }

    /*
     * Internal class for the page Composites. Adds only one flag to the Composite
     * to make tracking of the sent SWT.setData events easier.  
     */
    private class internal_PageComposite extends Composite {
        boolean setDataSent;
        
        internal_PageComposite(Composite parent, int style) {
            super(parent, style);
            setDataSent = false;
        }
    }
    
    /**
     * Constructs a new instance of this class given only its parent.
     * 
     * @param parent
     *            a shell which will be the parent of the new instance
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
     *                allowed subclass</li>
     *                </ul>
     */
    public MultiPageDialog(Shell parent) {
        this(parent, SWT.NONE);
    }

    /**
     * Constructs a new instance of this class given its parent and a style
     * value describing its behavior and appearance.
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or
     * must be built by <em>bitwise OR</em> 'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. The class description lists the style constants that are
     * applicable to the class. Style bits are also inherited from superclasses.
     * 
     * @param parent
     *            a shell which will be the parent of the new instance
     * @param style
     *            the style of dialog to construct
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
     *                allowed subclass</li>
     *                </ul>
     */
    public MultiPageDialog(Shell parent, int style) {
        super(parent, internal_checkStyle(parent, style));
        Internal_PackageSupport.checkParent(this, parent);
        checkSubclass();
        
        int mask = SWT.RIGHT_TO_LEFT | SWT.LEFT_TO_RIGHT | 
                   SWT.APPLICATION_MODAL | SWT.PRIMARY_MODAL | SWT.SYSTEM_MODAL;
        shell = new Shell(parent, SWT.DIALOG_TRIM | (Internal_PackageSupport.style(this) & mask));
        Internal_PackageSupport.setWidgetInternalOnly(shell);
        shell.setText(getText());
        shell.setLayout(new FillLayout());
        
        shell.addDisposeListener(new DisposeListener() {
           public void widgetDisposed(DisposeEvent e) {
               isOpen = false;
           } 
        });
        
        tabControl = new internal_TabControl(shell, (style & SWT.VIRTUAL) != 0, style);
        shell.layout(true);
        ignoreSelectionEvent = false;
    }

    private void checkDialog() {
        if (shell.isDisposed()) SWT.error(SWT.ERROR_WIDGET_DISPOSED);
        tabControl.checkWidget();
    }
    
    /**
     * Adds the listener to the collection of listeners who will
     * be notified when an event of the given type occurs. When the
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
     * @since eSWT 1.2 
     */ 
    public void addListener(int eventType, Listener listener) {
        checkDialog();
        if (listener == null) SWT.error (SWT.ERROR_NULL_ARGUMENT);
        tabControl.addListener(eventType, listener);
    }
    
    /**
     * Adds the listener to the collection of listeners who will be notified
     * when the page changes, by sending it one of the messages defined in the
     * <code>SelectionListener</code> interface.
     * <p>
     * When <code>widgetSelected</code> is called, the item field of the event
     * object is valid. <code>widgetDefaultSelected</code> is not called.
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
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * @see SelectionListener
     * @see #removeSelectionListener
     * @see org.eclipse.swt.events.SelectionEvent
     */
    public void addSelectionListener(SelectionListener listener) {
        checkDialog();
        if (listener == null) SWT.error (SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener (listener);
        tabControl.addListener(SWT.Selection, typedListener);
    }

    
    /**
     * Requests that the window manager close the dialog in the same way it
     * would be closed when the user clicks on the "close box" or performs some
     * other platform specific key or mouse combination that indicates the
     * window should be removed.
     * 
     * No exception should be thrown even when the dialog has been closed by the
     * end-user.
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @see org.eclipse.swt.widgets.Shell#close
     */
    public void close() {
        checkDialog();
        shell.dispose();
        isOpen = false;
    }

    /**
     * Creates a new page with the specified title string and icon.
     * 
     * @param title -
     *            the title string. Cannot be null.
     * @param icon -
     *            the label icon image. May be null.
     * @return composite - the new composite instance in the page. Cannot be
     *         null.
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the page title is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public Composite createPage(String title, Image icon) {
        checkDialog();
        if (title == null) {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }
        
        Composite page = new internal_PageComposite(tabControl, Internal_PackageSupport.style(this) & SWT.RIGHT_TO_LEFT);
        int iconHandle = (icon != null) ? Internal_GfxPackageSupport.getIconHandle(icon): 
            Internal_GfxPackageSupport.getNullIconHandle();
     
        ignoreSelectionEvent = true;
        OS.QTabWidget_addTab(tabControl.topHandle(), 
                topHandle(page), iconHandle, title);
        ignoreSelectionEvent = false;

        page.setSize(OS.QSwtTabWidget_stackedWidgetSize(tabControl.topHandle()));
        return page;
    }

    
    /**
     * Deletes the page from the receiver at the given zero-relative index
     * 
     * @param index -
     *            the index value. Cannot be negative, greater or equal to the
     *            number of pages.
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_RANGE - if the index is not between 0
     *                and the number of pages in the receiver minus 1
     *                (inclusive)</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void deletePage(int index) {
        checkDialog();
        checkRange(index);
        Composite page = getPage(index);
        ignoreSelectionEvent = true;
        OS.QTabWidget_removeTab(tabControl.topHandle(), index);
        ignoreSelectionEvent = false;
        page.dispose();        
    }

    
    /**
     * Disposes of the operating system resources associated with the receiver.
     * After this method has been invoked, all descendants will
     * answer <code>true</code> when sent the message
     * <code>isDisposed()</code>. Any internal connections between the
     * widgets in the tree will have been removed to facilitate garbage
     * collection.
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @see org.eclipse.swt.widgets.Widget#dispose
     */
    public void dispose() {
        isOpen = false;
        if (shell.isDisposed()) return;
        shell.dispose();
    }

    
    /**
     * Returns the composite of the page at the given zero-relative index in the
     * receiver.
     * 
     * @param index
     *            the index of the page to select. Cannot be negative, greater
     *            or equal to the number of pages.
     * @return Composite the composite.
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_RANGE - if the page count is zero, or
     *                if the index is negative or greater and equal to the page
     *                count</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     */
    public Composite getPage(int index) {
        checkDialog();
        checkRange(index);
        int handle = OS.QTabWidget_widget(tabControl.topHandle(), index);
        Control[] children = Internal_PackageSupport._getChildren(tabControl);
        Composite page = null;
        for (int i = 0; i < children.length; ++i) {
            if (Internal_PackageSupport.topHandle(children[i]) == handle) {
                page = (Composite)children[i];
                break;
            }
        }
        return page; 
    }

    
    /**
     * Returns the number of pages contained in the receiver.
     * 
     * @return int the number of pages. Returns zero when the dialog is
     *         constructed.
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     */
    public int getPageCount() {
        checkDialog();
        return OS.QTabWidget_count(tabControl.topHandle());
    }

    
    /**
     * Returns the title string of the page at the given zero-relative index in
     * the receiver.
     * 
     * @param index
     *            the index of the page to select.
     * @return String the title string of the page
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_RANGE - if the page count is zero, or
     *                if the index is negative or greater and equal to the page
     *                count</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                </ul>
     * @see #setTitle
     */
    public String getTitle(int index) {
        checkDialog();
        checkRange(index);
        return OS.QTabWidget_tabText(tabControl.topHandle(), index);
    }

    
    /**
     * Returns the zero-relative index of the page which is currently selected
     * in the receiver, or the newest page created to the receiver.
     * 
     * @return int the index of the selected page
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                </ul>
     */
    public int getSelectionIndex() {
        checkDialog();
        return OS.QTabWidget_currentIndex(tabControl.topHandle());
    }

    static int internal_checkStyle(Shell parent, int style) {
        if (parent != null) style = Internal_PackageSupport.checkStyle(parent, style);
        int mask = SWT.APPLICATION_MODAL | SWT.PRIMARY_MODAL;
        if ((style & mask) == 0) style |= SWT.APPLICATION_MODAL;
        if ((style & SWT.APPLICATION_MODAL) != 0) style = (style & ~mask) | SWT.APPLICATION_MODAL;
        if ((style & SWT.PRIMARY_MODAL) != 0) style = (style & ~mask) | SWT.PRIMARY_MODAL;
        return style;
    }

    /**
     * Makes the dialog visible and brings it to the front of the display.
     * 
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the dialog</li>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                </ul>
     */
    public void open() {
        checkDialog();
        
        if (!isOpen) {
            tabControl.sendSetDataEvent(getSelectionIndex());
            shell.open();
            isOpen = true;
            
            Display display = shell.getDisplay();
            while (isOpen) {
                if (!display.readAndDispatch()) {
                    display.sleep();
                }
            }
        } else {
            shell.forceActive();
        }
    }
    

    /**
     * Removes the listener from the collection of listeners who will
     * be notified when an event of the given type occurs.
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
     * @since eSWT 1.2 
     */
    public void removeListener(int eventType, Listener listener) {
        checkDialog();
        if (listener == null) SWT.error (SWT.ERROR_NULL_ARGUMENT);
        tabControl.removeListener(eventType, listener);
    }
    
    /**
     * Removes the listener from the collection of listeners who will be
     * notified when the receiver's selection changes.
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
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                </ul>
     * 
     * @see SelectionListener
     * @see #addSelectionListener
     */
    public void removeSelectionListener(SelectionListener listener) {
        checkDialog();
        tabControl.removeSelectionListener(listener);
    }

    
    /**
     * Selects the active page by the given zero-relative index.
     * 
     * @param index
     *            the index of the page to select. Cannot be negative, greater
     *            or equal to the number of pages.
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_RANGE - if the page count is zero, or
     *                if the index is negative or greater and equal to the page
     *                count</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *  
     */
    public void setSelection(int index) {
        checkDialog();
        checkRange(index);
        ignoreSelectionEvent = true;
        OS.QTabWidget_setCurrentIndex(tabControl.topHandle(), index);
        ignoreSelectionEvent = false;
    }

    
    /**
     * Sets the icon image of the page at the given zero-relative index. Note
     * that the icon size is implementation dependent so that the icon image may
     * be stretched or shrunk automatically.
     * 
     * @param index
     *            the index of the page to select.
     * @param icon
     *            the new icon image.
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the icon is null</li>
     *                <li>ERROR_INVALID_RANGE - if the page count is zero, or
     *                if the index is negative or greater and equal to the page
     *                count</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setImage(int index, Image icon) {
        checkDialog();
        if (icon == null) {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }
        checkRange(index);
        OS.QTabWidget_setTabIcon(tabControl.topHandle(), index, Internal_GfxPackageSupport.getIconHandle(icon)); 
    }

    // From Dialog class
    public void setText (String text) {
        checkDialog();
        super.setText(text);
        shell.setText(text);
    }    
    
    /**
     * Sets the title string of the page at the given zero-relative index.
     * 
     * @param index
     *            the index of the page to select.
     * @param title
     *            the new title string. Cannot be null.
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_RANGE - if the page count is zero, or
     *                if the index is negative or greater and equal to the page
     *                count</li>
     *                <li>ERROR_NULL_ARGUMENT - if the title is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * @see #getTitle
     */
    public void setTitle(int index, String title) {
        checkDialog();
        if (title == null) {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }
        checkRange(index);
        OS.QTabWidget_setTabText(tabControl.topHandle(), index, title);
    }
    
    private void checkRange(int index) {
        if ((index < 0) || (getPageCount() <= index)) {
            SWT.error(SWT.ERROR_INVALID_RANGE);
        }
    }
    
    private static final int topHandle(Widget w) {
        return Internal_PackageSupport.topHandle(w);
    }
}