/*******************************************************************************
 * Copyright (c) 2004 IBM Corp.
 * Portion Copyright (c) 2004, 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Mark Rogalski (IBM Corp.) - initial API specification
 *     Nokia Corporation - S60 implementation
 *     Nokia Corporation - QT implementation
 *******************************************************************************/

package org.eclipse.ercp.swt.mobile;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Scrollable;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.TypedListener;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.ListUtils;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.PackageProxy;
import org.eclipse.swt.internal.qt.WidgetState;

/**
 * Instances of this class represent a selectable user interface object that
 * displays a sorted list of text items. The items may be displayed in ascending
 * or descending order.
 * <p>
 * If the FILTER style is specified during construction, an assoicated label is
 * also displayed showing characters entered which are then used to filter the
 * list to show fewer items. The selection state of items filtered out of the
 * list is cleared.
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * 
 * <dl>
 * <dt><b>Styles: </b></dt>
 * <dd>SINGLE, MULTI, UP, DOWN</dd>
 * <p>
 * Note: Only one of SINGLE and MULTI may be specified. Only one of UP and DOWN
 * may be specified.
 * <p>
 * <dt><b>Mode Styles:</b></dt>
 * <dd>FILTER</dd>
 * <p>
 * <dt><b>Events: </b></dt>
 * <dd>Selection, DefaultSelection</dd>
 * </dl>
 */
public class SortedList extends Scrollable {
private int containerHandle;
private int layoutHandle;
private int editorHandle;
private int appHandle;
private boolean ignoreSelectionEvent = false;
private int keyEventDestination = 0;
private int firstFilteredIndex = -1;
private int lastFilteredIndex = -1;
private boolean filtering = false;

/**
 * constructor style specifying filter field should be displayed
 */
public static final int FILTER = 1;

static final class SortedListPackageProxy extends PackageProxy {
    public void createHandle(int index) {
        ((SortedList)w).createHandle(index);
    }
    public void deregister () {
        ((SortedList)w).deregister();
    }
    public void hookEvents() {
        ((SortedList)w).hookEvents();
    }
    public boolean qt_event_keypress( int widgetHandle, int key, int modifier, int character, int nativeScanCode ) {
        return ((SortedList)w).qt_event_keypress(widgetHandle, key, modifier, character, nativeScanCode);
    }
    public boolean qt_event_keyrelease( int widgetHandle, int key, int modifier, int character, int nativeScanCode ) {
        return ((SortedList)w).qt_event_keyrelease(widgetHandle, key, modifier, character, nativeScanCode);
    }
    public boolean qt_event_mouseButtonRelease(int widgetHandle, int button, int x, int y, int state, int buttons) {
        return ((SortedList)w).qt_event_mouseButtonRelease(widgetHandle, button, x, y, state, buttons);
    }
    public void qt_signal_list_itemActivated(int selectionHandle) {
        ((SortedList)w).qt_signal_list_itemActivated(selectionHandle);
    }
    public void qt_signal_list_itemDoubleClicked(int selectionHandle) {
        ((SortedList)w).qt_signal_list_itemDoubleClicked(selectionHandle);
    }
    public void qt_signal_list_itemSelectionChanged() {
        ((SortedList)w).qt_signal_list_itemSelectionChanged();
    }
    public void qt_signal_text_changed(String text) {
        ((SortedList)w).qt_signal_text_changed(text);
    }
    public void register() {
        ((SortedList)w).register();
    }
    public void releaseHandle() {
        ((SortedList)w).releaseHandle();
    }
    public void backgroundImageApplied(Image image) {
        ((SortedList)w).backgroundImageApplied(image);
    }
    public boolean _isFocusControl() {
        return ((SortedList)w)._isFocusControl();
    }
    public void setBackground() {
        ((SortedList)w).setBackground();
    }
    public void setTraversalFlags(int type, int key, int modifier, int character) {
        ((SortedList)w).setTraversalFlags(type, key, modifier, character);
    }
    public Point getPreferredClientAreaSize() {
        return ((SortedList)w).getPreferredClientAreaSize();
    }
}

/**
 * Construct a new instance of this class given its parent and a style value
 * describing its behavior and appearance.
 * <p>
 * The <code>style</code> value is either one of the style constants defined
 * in class <code>SWT</code> which is applicable to instances of this class,
 * or must be built by <em>bitwise OR</em> 'ing together (that is, using the
 * <code>int</code> "|" operator) two or more of those <code>SWT</code>
 * style constants. Style bits are also inherited from superclasses.
 * <code>SWT.UP</code> (the default) means the list is sorted so that
 * numbers go from low to high when the list is examined from top to bottom.
 * <code>SWT.DOWN</code> means the list is sorted so that numbers go from
 * high to low when examined from top to bottom.
 * 
 * @param parent
 *            a composite control which will be the parent of the new
 *            instance (cannot be null)
 * @param style
 *            the style of the control
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the parent</li>
 *                </ul>
 * 
 * @see SWT#SINGLE
 * @see SWT#MULTI
 * @see SWT#UP
 * @see SWT#DOWN
 */
public SortedList(Composite parent, int style) {
    this(parent, checkStyle(style), 0);
}

/**
 * Construct a new instance of this class given its parent, a style value
 * describing its behavior and appearance, and a mode style describing
 * additional behavior modes.
 * <p>
 * The <code>style</code> value is either one of the style constants defined
 * in class <code>SWT</code> which is applicable to instances of this class,
 * or must be built by <em>bitwise OR</em> 'ing together (that is, using the
 * <code>int</code> "|" operator) two or more of those <code>SWT</code>
 * style constants. Style bits are also inherited from superclasses.
 * <code>SWT.UP</code> (the default) means the list is sorted so that
 * numbers go from low to high when the list is examined from top to bottom.
 * <code>SWT.DOWN</code> means the list is sorted so that numbers go from
 * high to low when examined from top to bottom.
 * 
 * The <code>modeStyle</code> may specify the class constant
 * <code>FILTER</code>. This style adds a text entry field to the widget
 * which does not receive focus, but will display characters entered while
 * the list has focus. These characters are used to filter the list items so
 * that fewer items are displayed in the list. The selection is cleared for
 * any items not shown as a result of filtering.
 * 
 * @param parent
 *            a composite control which will be the parent of the new
 *            instance (cannot be null)
 * @param style
 *            the style of the control
 * 
 * @param modeStyle
 *            the mode for the control
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the parent</li>
 *                </ul>
 * 
 * @see SWT#SINGLE
 * @see SWT#MULTI
 * @see SWT#UP
 * @see SWT#DOWN
 * @see SortedList#FILTER
 */
public SortedList(Composite parent, int style, int modeStyle) {
    super(parent, checkStyle(style), modeStyle, new SortedListPackageProxy(), false);
    checkSubclass();
}

/**
 * Adds the argument to the receiver's list of items.
 * 
 * @param item
 *            text to be added to the list
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the item is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the parent</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_ITEM_NOT_ADDED - if the operation fails because
 *                of an operating system failure</li>
 *                </ul>
 * 
 * @see SortedList#setItems(String[])
 * @see SortedList#remove
 * @see SortedList#removeAll()
 */
public void add(String item) {
    checkWidget();
    ListUtils.add(scrollAreaHandle(), item, null);
    sortList();
    ignoreSelectionEvent =true;
    filterList();
    ignoreSelectionEvent= false;
}

/**
 * Adds the listener to the collection of listeners who will be notified
 * when the receiver's selection changes, by sending it one of the messages
 * defined in the SelectionListener interface.
 * <p>
 * <code>widgetSelected</code> is called when the selection changes. <br>
 * <code>widgetDefaultSelected</code> is typically called when selection is
 * finalized.
 * 
 * @param listener
 *            instance called when selection events occur
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the parent</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_ITEM_NOT_ADDED - if the operation fails because
 *                of an operating system failure</li>
 *                </ul>
 * 
 * @see ListBox#removeSelectionListener
 * @see SelectionListener
 */
public void addSelectionListener(SelectionListener listener) {
    checkWidget();
    if (listener == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    TypedListener typedListener = new TypedListener(listener);
    addListener(SWT.Selection, typedListener);
    addListener(SWT.DefaultSelection, typedListener);
}

/**
 * Returns the text of the item currently focused in the receiver, or
 * <code>null</code> if no item has focus.
 * <p>
 * 
 * @return the text of the item with focus
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the parent</li>
 *                </ul>
 */
public String getFocus() {
    checkWidget();
    int handle = scrollAreaHandle();
    int index = ListUtils.getFocusIndex(handle);
    if (index != -1) {
        return ListUtils.getItem(handle, index);
    }
    else {
        return null;
    }
}

/**
 * Returns the height (in pixels) of the area which would be used to display
 * one of the items in the tree.
 * 
 * @return height in pixels
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the parent</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_CANNOT_GET_HEIGHT - if the operation fails
 *                because of an operating system failure</li>
 *                </ul>
 */
public int getItemHeight() {
    checkWidget();
    return ListUtils.getItemHeight(scrollAreaHandle());
}

/**
 * Returns an array of Strings of items that are currently selected in the
 * receiver.
 * 
 * @return array of selected items
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the parent</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_CANNOT_GET_SELECTION - if the operation fails
 *                because of an operating system failure</li>
 *                </ul>
 * 
 * @see SortedList#getSelectionCount
 */
public String[] getSelection() {
    checkWidget();
    return ListUtils.getSelection(scrollAreaHandle());
}

/**
 * Returns the number of items currently selected.
 * 
 * @return count of selected items
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the parent</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_CANNOT_GET_COUNT - if the operation fails
 *                because of an operating system failure</li>
 *                </ul>
 * 
 * @see SortedList#getSelection
 */
public int getSelectionCount() {
    checkWidget();
    return ListUtils.getSelectionCount(scrollAreaHandle());
}

/**
 * Searches the receiver's list starting at the first item until an item is
 * found that is equal to the argument, and removes that item from the list.
 * 
 * @param item
 *            text of item to remove
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the string is null</li> <li>
 *                ERROR_INVALID_ARGUMENT - if the string is not found in the
 *                list</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the parent</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_ITEM_NOT_REMOVED - if the operation fails
 *                because of an operating system failure</li>
 *                </ul>
 * 
 * @see SortedList#add(String)
 * @see SortedList#removeAll()
 */
public void remove(String item) {
    checkWidget();
    ListUtils.remove(scrollAreaHandle(), item);
}

/**
 * Removes all of the items from the receiver.
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the parent</li>
 *                </ul>
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_ITEM_NOT_REMOVED - if the operation fails
 *                because of an operating system failure</li>
 *                </ul>
 * 
 * @see SortedList#add(String)
 * @see SortedList#remove(String)
 * @see SortedList#setItems(String[])
 */
public void removeAll() {
    checkWidget();
    ListUtils.removeAll(scrollAreaHandle());
}

/**
 * Removes the listener from the collection of listeners who are notified
 * when the receiver's selection changes.
 * 
 * @param listener
 *            instance called when selection events occur
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the parent</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_ITEM_NOT_REMOVED - if the operation fails
 *                because of an operating system failure</li>
 *                </ul>
 * 
 * @see SortedList#addSelectionListener
 * @see SelectionListener
 */
public void removeSelectionListener(SelectionListener listener) {
    removeListener(SWT.Selection, listener);
    removeListener(SWT.DefaultSelection, listener);
}

/**
 * Selects the first item that has text matching the given string. If the
 * argument is <code>null</code>, the selection is cleared.
 * 
 * @param item
 *            text of item to select
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_ARGUMENT - if the string is not found in
 *                the list</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the parent</li>
 *                </ul>
 */
public void select(String item) {
    checkWidget();
    ignoreSelectionEvent = true;
    ListUtils.select(scrollAreaHandle(), item, (SWT.MULTI & getStyle()) != 0);
    ignoreSelectionEvent = false;
    filterList();
}

/**
 * Sets the receiver's items to be the given array of items.
 * 
 * @param items
 *            array of text strings to be shown in list
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the string array is null 
 *                </li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the parent</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_ITEM_NOT_ADDED - if the operation fails because
 *                of an operating system failure</li>
 *                </ul>
 * 
 * @see SortedList#add
 */
public void setItems(String[] items) {
    checkWidget();
    ListUtils.setItems(scrollAreaHandle(), items, null);
    sortList();
    ignoreSelectionEvent =true;
    filterList();
    ignoreSelectionEvent =false;
}

/**
 * Sets the receiver's selection to be the given array of items.
 * 
 * @param items
 *            array of text strings to be selected in list
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the string array is null 
 *                </li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the parent</li>
 *                </ul>
 * 
 * @see SortedList#select
 */
public void setSelection(String[] items) {
    checkWidget();
    ignoreSelectionEvent = true;
    ListUtils.setSelection(scrollAreaHandle(), items, (SWT.MULTI & getStyle()) != 0);
    ignoreSelectionEvent = false;
    filterList();
    
}

/**
 * Scrolls the list as necessary to show the selected items.
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the parent</li>
 *                </ul>
 * 
 * @see SortedList#select
 * @see SortedList#setSelection
 */
public void showSelection() {
    checkWidget();
    ListUtils.showSelection(scrollAreaHandle());
}

void createHandle(int index) {
    Internal_PackageSupport.createHandle(this, index);
    // Create handles
    appHandle = OS.QCoreApplication_instance();
    containerHandle = OS.QFrame_new();
    Internal_PackageSupport.setFrameHandle(this, containerHandle);
    Internal_PackageSupport.setTopHandle(this, containerHandle);
    layoutHandle = OS.QVBoxLayout_new(containerHandle);
    int scrollAreaHandle = OS.QListWidget_new();
    Internal_PackageSupport.setScrollAreaHandle(this, scrollAreaHandle);
    Internal_PackageSupport.setHandle(this, OS.QAbstractScrollArea_viewPort(scrollAreaHandle));
    if ((Internal_PackageSupport.extraStyle(this) & FILTER) != 0) {
        editorHandle = OS.QLineEdit_new();
    }

    // Setup the container & its layout
    OS.QLayout_setContentsMargins(layoutHandle, 0, 0, 0, 0);
    OS.QLayout_setSpacing(layoutHandle, 0);
    OS.QLayout_addWidget(layoutHandle, scrollAreaHandle);
    if (editorHandle != 0) {
        OS.QLayout_addWidget(layoutHandle, editorHandle);
    }
    OS.QWidget_setLayout(containerHandle, layoutHandle);
    OS.QWidget_setFocusPolicy(containerHandle, OS.QT_FOCUSPOLICY_TABFOCUS);
    OS.QWidget_setFocusProxy(containerHandle, scrollAreaHandle);
    
    // Setup the list
    OS.QAbstractItemView_setSelectionBehavior(scrollAreaHandle, OS.QT_ABSTRACTITEMVIEW_SELECTIONBEHAVIOR_ITEMS);
    if ((getStyle() & SWT.MULTI) != 0) {
        OS.QAbstractItemView_setSelectionMode(scrollAreaHandle, OS.QT_ABSTRACTITEMVIEW_SELECTIONMODE_MULTI);
    }
    else {
        OS.QAbstractItemView_setSelectionMode(scrollAreaHandle, OS.QT_ABSTRACTITEMVIEW_SELECTIONMODE_SINGLE);
    }
    OS.QListWidget_setSortingEnabled(scrollAreaHandle, true);
    OS.QFrame_setFrameStyle(scrollAreaHandle, OS.QT_NOFRAME);
    
    Internal_PackageSupport.setState(this, Internal_PackageSupport.state(this) | WidgetState.HANDLE);
}

void hookEvents () {
    Internal_PackageSupport.hookEvents(this);
    Display display = Internal_PackageSupport.display(this);
    int scrollAreaHandle = scrollAreaHandle();
    int signalProxy1 = OS.SignalHandler_new(scrollAreaHandle, display, 
        OS.QSIGNAL_LIST_ITEMSELECTION_CHANGED);
    OS.QObject_connectOrThrow(scrollAreaHandle, "itemSelectionChanged()", 
        signalProxy1, "widgetSignal()", OS.QT_AUTOCONNECTION);
    int signalProxy2 = OS.SignalHandler_new(scrollAreaHandle, display, 
        OS.QSIGNAL_LIST_ITEM_DOUBLECLICKED);
    OS.QObject_connectOrThrow(scrollAreaHandle, "itemDoubleClicked(QListWidgetItem*)", 
        signalProxy2, "widgetSignal(QListWidgetItem*)", OS.QT_AUTOCONNECTION);
    int signalProxy3 = OS.SignalHandler_new(Internal_PackageSupport.topHandle(this),
        display, OS.QSIGNAL_LIST_ITEM_ACTIVATED);
    OS.QObject_connectOrThrow(scrollAreaHandle,
        "itemActivated(QListWidgetItem* )", signalProxy3,
        "widgetSignal(QListWidgetItem* )", OS.QT_AUTOCONNECTION);
    if (editorHandle != 0) {
        int signalProxy4 = OS.SignalHandler_new(editorHandle, display, OS.QSIGNAL_TEXT_CHANGED);
        OS.QObject_connectOrThrow(editorHandle, "textChanged(const QString&)",
            signalProxy4, "widgetSignal(const QString&)", OS.QT_AUTOCONNECTION);
    }
}

void qt_signal_list_itemSelectionChanged(){
    if (!ignoreSelectionEvent) {
        Internal_PackageSupport.sendEvent(this, SWT.Selection);
    }
}

void qt_signal_list_itemDoubleClicked(int selectionHandle){
    if (!ignoreSelectionEvent) {
        Internal_PackageSupport.sendEvent(this, SWT.DefaultSelection);
    }
}

void qt_signal_list_itemActivated(int selectionHandle) {
    if (!ignoreSelectionEvent) {
        Internal_PackageSupport.sendEvent(this, SWT.DefaultSelection);
    }
}

boolean qt_event_mouseButtonRelease (int widgetHandle, int button, int x, int y, int state, int buttons) {
    // We need to filter the list items if the selection has been changed.
    // For some reason we cannot do this directly in qt_signal_list_itemSelectionChanged.
    if((getStyle() & SWT.MULTI) != 0) {
        filterList();
    }
    return Internal_PackageSupport.qt_event_mouseButtonRelease(this, widgetHandle, button, x, y, state, buttons);
}

void qt_signal_text_changed(String text) {
    firstFilteredIndex = -1;
    lastFilteredIndex = -1;
    int handle = scrollAreaHandle();
    int count = ListUtils.getItemCount(handle);
    if (text != null && text.length() > 0) {
        text = text.toLowerCase();
        
        // If previously not filtering, focus the editor
        if (!filtering) {
            if (isFocusControl()) {
                OS.QWidget_setFocus(editorHandle, OS.QT_OTHERFOCUSREASON);
            }
            OS.QWidget_setFocusProxy(containerHandle, editorHandle);
        }
        filtering = true;
        for (int i = 0; i < count; i++) {
            // Cannot skip any invisible items because the filter text
            // may be deleted or typing may happen in the middle of it. 
            
            // Selected items stay visible when filtering
            if (ListUtils.getItem(handle, i).toLowerCase().startsWith(text)
                || (((getStyle() & SWT.MULTI) != 0) && (OS.QListWidget_swt_isItemSelected(handle, i)))) {
                OS.QListWidget_swt_setItemVisible(handle, i, true);
                if (firstFilteredIndex < 0) {
                    firstFilteredIndex = i;
                }
                lastFilteredIndex = i;
            }
            else {
                OS.QListWidget_swt_setItemVisible(handle, i, false);
            }
        }
        // If the previous selected item got filtered out, focus the first filtered in
        if (!OS.QListWidget_swt_isItemVisible(handle, ListUtils.getFocusIndex(handle))) {
            ListUtils.setFocusIndex(scrollAreaHandle(), firstFilteredIndex, (getStyle() & SWT.MULTI) != 0);
        }
    }
    else {
        // Previously filtering, focus the list.
        if (filtering) {
            if (isFocusControl()) {
                OS.QWidget_setFocus(scrollAreaHandle(), OS.QT_OTHERFOCUSREASON);
            }
            OS.QWidget_setFocusProxy(containerHandle, scrollAreaHandle());
        }
        filtering = false;
        firstFilteredIndex = -1;
        lastFilteredIndex = -1;
        for (int i = 0; i < count; i++) {
            OS.QListWidget_swt_setItemVisible(handle, i, true);
        }
        // If the filtering cleared the focus, restore focus to first item
        if (ListUtils.getFocusIndex(scrollAreaHandle()) < 0) {
            ListUtils.setFocusIndex(scrollAreaHandle(), 0, (getStyle() & SWT.MULTI) != 0);
        }
    }
}

boolean _isFocusControl() {
    int focusedWidget = OS.QApplication_focusWidget();    
    if ((focusedWidget != 0) 
        && (focusedWidget == containerHandle
            || focusedWidget == scrollAreaHandle()
            || focusedWidget == editorHandle)) {
        return true;
    }
    return false;
}

boolean qt_event_keypress(int widgetHandle, int key, int modifier, int character, int nativeScanCode) {
    if (keyEventDestination == 0) {
        // Initial key, non generated
        if (Internal_PackageSupport.qt_event_keypress(this, widgetHandle, key, modifier, character, nativeScanCode)) {
            // The base traversed or the key listeners canceled or we do not have focus
            return true;
        }
    }
    else {
        // Generated key press, forward.
        return false;        
    }
    
    // Key destination can be either the list or the editor
    if ((editorHandle != 0) 
        && key != OS.QT_KEY_UP 
        && key != OS.QT_KEY_DOWN
        && key != OS.QT_KEY_ENTER
        && key != OS.QT_KEY_UNKNOWN) {
        if (keyEventDestination == 0) {
            OS.QWidget_setFocus(editorHandle, OS.QT_OTHERFOCUSREASON);
            OS.QWidget_setFocusProxy(containerHandle, editorHandle);
        }
        keyEventDestination = editorHandle;
    }
    else {
        int scrollAreaHandle = scrollAreaHandle();
        if (keyEventDestination == 0) {
            OS.QWidget_setFocus(scrollAreaHandle, OS.QT_OTHERFOCUSREASON);
            OS.QWidget_setFocusProxy(containerHandle, scrollAreaHandle);
        }
        keyEventDestination = scrollAreaHandle;
    }
    
    // Direct send key press event
    int keyPressEv = OS.QKeyEvent_new(OS.QEVENT_KEYPRESS, key, modifier, "" + (char)character);
    OS.QCoreApplication_sendEvent(appHandle, keyEventDestination, keyPressEv);
    
    // Direct send key release event
    int keyReleaseEv = OS.QKeyEvent_new(OS.QEVENT_KEYRELEASE, key, modifier, "" + (char)character);
    OS.QCoreApplication_sendEvent(appHandle, keyEventDestination, keyReleaseEv);
    
    // Stop forwarding
    keyEventDestination = 0;
    
    // Consume key press
    return true;
}

boolean qt_event_keyrelease(int widgetHandle, int key, int modifier, int character, int nativeScanCode) {
    if (keyEventDestination == widgetHandle) {
        // Forward key release if event listeners allow
        return Internal_PackageSupport.qt_event_keyrelease(this, widgetHandle, key, modifier, character, nativeScanCode);
    }
    else {
        // Consume key release
        return true;
    }
}

void setTraversalFlags(int type, int key, int modifier, int character) {
    Internal_PackageSupport.setTraverseDoIt(this, false);
    Internal_PackageSupport.setTraverseCancel(this, false);

    switch (key) {
    case OS.QT_KEY_UP: {
        // Traverse if:
        // - not filtering & focused first item
        // - filtering & all items filtered out
        // - filtering & focused first filtered in item
        if ((!filtering && ListUtils.getFocusIndex(scrollAreaHandle()) == 0)
            || (filtering && firstFilteredIndex < 0)
            || (filtering && ListUtils.getFocusIndex(scrollAreaHandle()) == firstFilteredIndex)) {
            Internal_PackageSupport.setTraverseDoIt(this, true);
            Internal_PackageSupport.setTraverseCancel(this, true);
        }
        break;
    }
    case OS.QT_KEY_DOWN: {
        // Traverse if:
        // - not filtering & focused last item
        // - filtering & all items filtered out
        // - filtering & focused first filtered in item
        if ((!filtering && ListUtils.getFocusIndex(scrollAreaHandle()) 
                == ListUtils.getItemCount(scrollAreaHandle()) - 1)
            || (filtering && lastFilteredIndex < 0)
            || (filtering && ListUtils.getFocusIndex(scrollAreaHandle()) == lastFilteredIndex)) {
            Internal_PackageSupport.setTraverseDoIt(this, true);
            Internal_PackageSupport.setTraverseCancel(this, true);
        }
        break;    
    }
    case OS.QT_KEY_LEFT: {
        Internal_PackageSupport.setTraverseDoIt(this, true);
        Internal_PackageSupport.setTraverseCancel(this, true);
        int focusWidget = OS.QApplication_focusWidget();
        if (focusWidget != editorHandle) { 
            break;
        }
        if (OS.QLineEdit_cursorPosition(editorHandle) == 0) {
            break;
        }
        Internal_PackageSupport.setTraverseDoIt(this, false);
        Internal_PackageSupport.setTraverseCancel(this, false);
        break;
    }
    case OS.QT_KEY_RIGHT: {
        Internal_PackageSupport.setTraverseDoIt(this, true);
        Internal_PackageSupport.setTraverseCancel(this, true);
        int focusWidget = OS.QApplication_focusWidget();
        if (focusWidget != editorHandle) { 
            break;
        }
        String s = OS.QLineEdit_text(editorHandle);
        if (s == null) {
            break;
        }
        if (OS.QLineEdit_cursorPosition(editorHandle) == s.length()) {
            break;
        }
        Internal_PackageSupport.setTraverseDoIt(this, false);
        Internal_PackageSupport.setTraverseCancel(this, false);
        break;
    }
    // All other traverse keys can traverse
    default: {
        Internal_PackageSupport.setTraversalFlags(this, type, key, modifier, character);
        break;
    }
    }
}

public Rectangle computeTrim(int x, int y, int width, int height) {
    Rectangle rect = super.computeTrim(x, y, width, height);
    if (editorHandle != 0) {
        int h = OS.QWidget_sizeHint(editorHandle).y;
        // Workaround for tackling Qt layout int round up bug. 
        if (h % 2 != 0) {
            h++;
        }
        rect.height += h;
    }
    return rect;
}

Point getPreferredClientAreaSize() {
    int scrollAreaHandle = scrollAreaHandle();
    int count = ListUtils.getItemCount(scrollAreaHandle);
    int x = OS.QAbstractItemView_sizeHintForColumn(scrollAreaHandle, 0);
    int y = count * ListUtils.getItemHeight(scrollAreaHandle);
    return new Point(x, y);
}

void deregister() {
    Internal_PackageSupport.deregister(this);
    Internal_PackageSupport.removeWidget(this, containerHandle);
    if (editorHandle != 0) {
        Internal_PackageSupport.removeWidget(this, editorHandle);
    }
}

void register() {
    Internal_PackageSupport.register(this);
    Internal_PackageSupport.addWidget(this, containerHandle);
    if (editorHandle != 0) {
        Internal_PackageSupport.addWidget(this, editorHandle);
    }
}

void releaseHandle() {
    containerHandle = 0;
    editorHandle = 0;
    layoutHandle = 0;
    appHandle = 0;
    Internal_PackageSupport.releaseHandle(this);
}

/*
 * Style filter
 */
static private int checkStyle(int style) {
    style = Internal_PackageSupport.checkBits(style, SWT.SINGLE, SWT.MULTI, 0, 0, 0, 0);
    style = Internal_PackageSupport.checkBits(style, SWT.UP, SWT.DOWN, 0, 0, 0, 0);
    return style;
}

/*
 * Must sort every time items are added.
 */
private void sortList() { 
    OS.QListWidget_sortItems(scrollAreaHandle(), ((getStyle() & SWT.UP) != 0) ? 
        OS.QT_ASCENDINGORDER : OS.QT_DESCENDINGORDER);
}

/*
 * Must filter every time items are changed.
 */
private void filterList() {
    if (editorHandle != 0) {
        qt_signal_text_changed(OS.QLineEdit_text(editorHandle));
    }
}

void setBackground () {
    if(editorHandle != 0) {
        // Filter editor might have custom palette that needs to be restored
        OS.QWidget_swt_unsetPalette(editorHandle);
    }
    Internal_PackageSupport.setBackground(this);
}

void backgroundImageApplied(Image image) {
    // When background image is set to SortedList then it will look ugly if
    // the filter editor draws its own background image by itself. The image
    // will appear as misplaced starting from the filter editor's origin instead
    // of the sorted list's origin. To fix this the background brush of filter
    // editor is temporarily set to null when image background is in use to make
    // it inherit the background from its parent QFrame container. For the list this
    // doesn't need to be done because its origin is currently the same as the
    // container QFrame's. 
    if(editorHandle == 0) return;
    int palette = 0;
    try {
        palette = OS.QWidget_swt_palette_new(editorHandle);
        int[] bkRoles = Internal_PackageSupport.getBackgroundImageRoles(this);
        for(int i = 0; i < bkRoles.length; ++i) {
            OS.QPalette_swt_setBrush(palette, bkRoles[i], 0);
        }
        OS.QWidget_setPalette(editorHandle, palette);
    } finally {
        OS.QPalette_delete(palette);
    }
}
private final int scrollAreaHandle() {
    return Internal_PackageSupport.scrollAreaHandle(this);
}
}
