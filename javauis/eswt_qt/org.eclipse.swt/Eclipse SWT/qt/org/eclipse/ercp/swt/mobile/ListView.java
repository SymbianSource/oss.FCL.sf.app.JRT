/*******************************************************************************
 * Copyright (c) 2004, 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Yu You (Nokia Corp.)- initial API specification 
 *     Nokia Corporation - S60 implementation
 *     Nokia Corporation - QT implementation
 *******************************************************************************/
package org.eclipse.ercp.swt.mobile;

import java.util.Vector;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.ListUtils;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.PackageProxy;
import org.eclipse.swt.internal.qt.WidgetState;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Scrollable;
import org.eclipse.swt.widgets.TypedListener;

/**
 * 
 * A widget that allows the user to select one or more items from a collection
 * of items that can be displayed in a multi-column way with different styles.
 * 
 * <p>
 * ListView lays out its children items in one or more columns from top to
 * bottom. If a layout orientation hint is not specified, the implementation
 * chooses the orientation. If there is only enough screen width for one column,
 * the list scrolls vertically. If there is room to display multiple columns
 * within the widget, then the list scrolls horizontally. The list never scrolls
 * in more than one direction. The layout orientation may be set at runtime by
 * calling method <code>setLayout(int)</code>.
 * <p>
 * The item density hint determines the size and positioning of items in order
 * to fit more or less within the widget. Applications can query the preferred
 * sizes of the icons for each density level. Note that the sizes may be diverse
 * in different platforms. When the given icons do not match the prefered size,
 * the implementation may adjust icon sizes without throwing any exception.
 * Applications can change the item density level at runtime by calling method
 * <code>setLayoutDensity(int)</code>. Some platforms may use context-sensitive
 * device keys to allow the user to change the ListView's density level at
 * run-time. For example, by activating "zoom in/out" device keys when the
 * ListView is focused.
 * <p>
 * Applications can query the current layout and density values by calling the
 * corresponding "get" methods.
 * 
 * <dl>
 * <dt><b>SWT styles: </b></dt>
 * <dd>SINGLE, MULTI: single or multiple selection</dd>
 * <dd>VERTICAL, HORIZONTAL: defines the layout orientation</dd>
 * <dt><b>ListView density levels: </b></dt>
 * <dd>HIGH, MEDIUM (default), LOW: defines the item density levels that
 * determine the numbers of visible list items per screen</dd>
 * <dt><b>Events: </b></dt>
 * <dd>Selection, DefaultSelection</dd>
 * </dl>
 * <p>
 * Note: If styles not applicable to ListView are specified, they are ignored.
 * </p>
 * <p>
 * Note: ListView is a subclass of Scrollable. Changing the scrollbar values (
 * <code>ScrollBar.setSelection(int)</code>) will affect the scrolling position
 * of the ListView control. However, changing the range of scrollbars, such as
 * <code>setMaximum(int)</code> and <code>setMinimum(int)</code>, will not
 * affect the ListView at all.
 * </p>
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */
public class ListView extends Scrollable {
private Vector icons = new Vector();
private int density;
private int spacing;
private boolean ignoreSelectionEvent;

/**
 * A low density level.
 * 
 * <P>
 * Value <code>1</code> is assigned to <code>LOW</code>.
 * </P>
 * 
 */
public static final int LOW = 1;

/**
 * A medium density level.
 * 
 * <P>
 * Value <code>2</code> is assigned to <code>MEDIUM</code>.
 * </P>
 * 
 */
public static final int MEDIUM = 2;

/**
 * A high density level.
 * <P>
 * Value <code>3</code> is assigned to <code>HIGH</code>.
 * </P>
 * </DL>
 * 
 */
public static final int HIGH = 3;

static final class ListViewPackageProxy extends PackageProxy {
    public void createHandle(int index) {
        ((ListView)w).createHandle(index);
    }
    public void hookEvents() {
        ((ListView)w).hookEvents();
    }
    public void qt_signal_list_itemDoubleClicked(int selectionHandle) {
        ((ListView)w).qt_signal_list_itemDoubleClicked(selectionHandle);
    }
    public void qt_signal_list_itemSelectionChanged() {
        ((ListView)w).qt_signal_list_itemSelectionChanged();
    }
    public void setTraversalFlags(int type, int key, int modifier, int character) {
        ((ListView)w).setTraversalFlags(type, key, modifier, character);
    }
    public Point getPreferredClientAreaSize() {
        return ((ListView)w).getPreferredClientAreaSize();
    }
}
    
/**
 * Constructs a new instance of this class given its parent and a style
 * value describing its behavior and appearance.
 * <p>
 * The style value is either one of the style constants defined in class
 * <code>SWT</code> which is applicable to instances of this class, or must
 * be built by <em>bitwise OR</em> 'ing together (that is, using the
 * <code>int</code> "|" operator) two or more of those <code>SWT</code>
 * style constants. The class description lists the style constants that are
 * applicable to the class. Style bits are also inherited from superclasses.
 * </p>
 * 
 * @param parent
 *            a widget which will be the parent of the new instance (cannot
 *            be null)
 * @param style
 *            the style value of the widget to construct. See <a
 *            href="#description">Description </a> for details.
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
 * 
 * @see SWT#SINGLE
 * @see SWT#MULTI
 * @see SWT#HORIZONTAL
 * @see SWT#VERTICAL
 */
public ListView(Composite parent, int style) {
    super(parent, checkStyle(style), 0, new ListViewPackageProxy(), false);
}

/**
 * Constructs a new instance of this class given its parent and a style
 * value describing its behavior and appearance styles.
 * <p>
 * The style value is either one of the style constants defined in class
 * <code>SWT</code> which is applicable to instances of this class, or must
 * be built by <em>bitwise OR</em> 'ing together (that is, using the
 * <code>int</code> "|" operator) two or more of those <code>SWT</code>
 * style constants. The class description lists the style constants that are
 * applicable to the class. Style bits are also inherited from superclasses.
 * </p>
 * 
 * <p>
 * The density style is one of HIGH, MEDIUM and LOW.
 * </p>
 * 
 * @param parent
 *            a widget which will be the parent of the new instance (cannot
 *            be null)
 * @param style
 *            the style value of the widget to construct. See <a
 *            href="#description">Description </a> for details.
 * @param density
 *            the density style value.
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
 *                <li>ERROR_INVALID_ARGUMENT - if the density is invalid</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the parent</li>
 *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
 *                allowed subclass</li>
 *                </ul>
 * 
 * @see SWT#SINGLE
 * @see SWT#MULTI
 * @see SWT#HORIZONTAL
 * @see SWT#VERTICAL
 * @see #HIGH
 * @see #MEDIUM
 * @see #LOW
 * @see #getLayoutDensity
 */
public ListView(Composite parent, int style, int density) {
    super(parent, style, 0, new ListViewPackageProxy(), false);
    if (density != LOW && density != MEDIUM && density != HIGH) {
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
    this.density = density;
}

/**
 * Adds the string item and an optional icon to the end of the receiver's
 * list.
 * 
 * @param item
 *            the new item text
 * @param icon
 *            the icon of the item, or null.
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the item is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_ITEM_NOT_ADDED - if the operation fails because
 *                of an operating system failure</li>
 *                </ul>
 * 
 * @see #add(String,Image, int)
 */
public void add(String item, Image icon) {
    checkWidget();
    ListUtils.add(topHandle(), item, icon);
    icons.addElement(icon);
    resetGeometry(getItemCount() - 1);
}

/**
 * Adds the string item and an optional icon to the receiver's list at the
 * given zero-relative index.
 * <p>
 * Note: To add an item at the end of the list, use the result of calling
 * <code>getItemCount()</code> as the index or use
 * <code>add(String, Image)</code>.
 * </p>
 * 
 * @param string
 *            the new item
 * @param icon
 *            the icon part of the item, or null.
 * @param index
 *            the index for the item
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
 *                <li>ERROR_INVALID_RANGE - if the index is not between 0
 *                and the number of elements in the list (inclusive)</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_ITEM_NOT_ADDED - if the operation fails because
 *                of an operating system failure</li>
 *                </ul>
 * 
 * @see #add(String, Image)
 */
public void add(String item, Image icon, int index) {
    checkWidget();
    ListUtils.add(topHandle(), item, icon, index);
    icons.insertElementAt(icon, index);
    resetGeometry(index);
}

/**
 * Adds the listener to the collection of listeners who will be notified
 * when the receiver's selection changes. The listener receives the messages
 * defined in the <code>SelectionListener</code> interface.
 * <p>
 * <code>widgetSelected</code> is called when the selection changes.
 * <code>widgetDefaultSelected</code> is typically called when an item is
 * double-clicked.
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
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 * 
 * @see SelectionListener
 * @see #removeSelectionListener
 * @see org.eclipse.swt.events.SelectionEvent
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
 * Deselects the item at the given zero-relative index in the receiver. If
 * the item at the index was already deselected, it remains deselected.
 * Indices that are out of range are ignored.
 * 
 * @param index
 *            the index of the item to deselect
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public void deselect(int index) {
    checkWidget();
    ignoreSelectionEvent = true;
    ListUtils.deselect(topHandle(), index);
    ignoreSelectionEvent = false;
}

/**
 * Deselects the items at the given zero-relative indices in the receiver.
 * If the items at the given zero-relative indexes in the receiver are
 * selected, they are deselected. If they were not selected, they remain
 * deselected. Indices that are out of range and duplicate indices are
 * ignored.
 * 
 * @param indices
 *            the array of indices for the items to deselect
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the set of indices is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public void deselect(int[] indices) {
    checkWidget();
    ignoreSelectionEvent = true;
    ListUtils.deselect(topHandle(), indices);
    ignoreSelectionEvent = false;
}

/**
 * Deselects the items at the given zero-relative indices in the receiver.
 * If the items at the given zero-relative indexes in the receiver were
 * selected, they are deselected. If they were not selected, they remain
 * deselected. The range of the indices is inclusive. Indices that are out
 * of range are ignored.
 * 
 * @param start
 *            the start index of the items to deselect
 * @param end
 *            the end index of the items to deselect
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public void deselect(int start, int end) {
    checkWidget();
    ignoreSelectionEvent = true;
    ListUtils.deselect(topHandle(), start, end);
    ignoreSelectionEvent = false;
}

/**
 * Deselects all selected items in the receiver.
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public void deselectAll() {
    checkWidget();
    ignoreSelectionEvent = true;
    ListUtils.deselectAll(topHandle());
    ignoreSelectionEvent = false;
}

/**
 * Returns the zero-relative index of the item which currently has the focus
 * in the receiver, or -1 if no item has focus.
 * 
 * @return the index of the selected item
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public int getFocusIndex() {
    checkWidget();
    return ListUtils.getFocusIndex(topHandle());
}

/**
 * Returns the item icon at the given, zero-relative index in the receiver.
 * Throws an exception if the index is out of range.
 * 
 * @param index
 *            the index of the item to return
 * @return the associated icon at the given index. Null means no icon
 *         defined.
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_RANGE - if the index is not between 0
 *                and the number of elements in the list minus 1 (inclusive)
 *                </li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_CANNOT_GET_ITEM - if the operation fails because
 *                of an operating system failure</li>
 *                </ul>
 */
public Image getIcon(int index) {
    checkWidget();
    if (index < 0 || index >= icons.size()) {
        SWT.error(SWT.ERROR_INVALID_RANGE);
    }
    return (Image) icons.elementAt(index);
}

/**
 * Gets the preferred icon size that matches the density style. The style
 * must be one of SWT.HIGH, SWT.MEDIUM, or or SWT.LOW.
 * 
 * @param style
 *            the size style. Only SWT.HIGH, SWT.MEDIUM, and SWT.LOW can be
 *            used.
 * 
 * @return Point the best icon size in pixel (width and height). Must be
 *         positive values.
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_ARGUMENT - if the style value is invalid
 *                </li>
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
public Point getIconSize(int style) {
    checkWidget();
    if (style != LOW && style != MEDIUM && style != HIGH) {
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
    return OS.QAbstractItemView_iconSize(topHandle());
}

/**
 * Returns the item at the given, zero-relative index in the receiver.
 * Throws an exception if the index is out of range.
 * 
 * @param index
 *            the index of the item to return
 * @return the item at the given index
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_RANGE - if the index is not between 0
 *                and the number of elements in the list minus 1 (inclusive)
 *                </li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_CANNOT_GET_ITEM - if the operation fails because
 *                of an operating system failure</li>
 *                </ul>
 */
public String getItem(int index) {
    checkWidget();
    return ListUtils.getItem(topHandle(), index);
}

/**
 * Returns the number of items contained in the receiver.
 * 
 * @return the number of items
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_CANNOT_GET_COUNT - if the operation fails
 *                because of an operating system failure</li>
 *                </ul>
 */
public int getItemCount() {
    checkWidget();
    return ListUtils.getItemCount(topHandle());
}

/**
 * Returns an array of <code>String</code> s which are the items in the
 * receiver.
 * <p>
 * Note: This is not the actual structure used by the receiver to maintain
 * its list of items, so modifying the array will not affect the receiver.
 * </p>
 * 
 * @return the items in the receiver's list
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_CANNOT_GET_ITEM - if the operation fails because
 *                of an operating system failure while getting an item</li>
 *                <li>ERROR_CANNOT_GET_COUNT - if the operation fails
 *                because of an operating system failure while getting the
 *                item count</li>
 *                </ul>
 */
public String[] getItems() {
    checkWidget();
    return ListUtils.getItems(topHandle());
}

/**
 * Returns the value of the layout orientation.
 * 
 * 
 * @return the scrolling orientation value. One of SWT.VERTICAL and
 *         SWT.HORIZONTAL
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 * 
 * @see SWT#HORIZONTAL
 * @see SWT#VERTICAL
 */
public int getLayout() {
    checkWidget();
    if (OS.QT_LISTVIEW_FLOW_LEFTTORIGHT == OS.QListView_flow(topHandle())) {
        // 0 1 2
        // 3 4 5
        // 6 7 8
        return SWT.HORIZONTAL;
    }
    else {
        // 0 3 6
        // 1 4 7
        // 2 5 8
        return SWT.VERTICAL;
    }
}

/**
 * Returns the value of the item density level.
 * 
 * @return the density level. One of HIGH, MEDIUM, or LOW.
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 * 
 * @see ListView#LOW
 * @see ListView#MEDIUM
 * @see ListView#HIGH
 */
public int getLayoutDensity() {
    checkWidget();
    return density;
}

/**
 * Returns an array of <code>String</code> s that are currently selected in
 * the receiver. An empty array indicates that no items are selected.
 * <p>
 * Note: This is not the actual structure used by the receiver to maintain
 * its selection, so modifying the array will not affect the receiver.
 * </p>
 * 
 * @return an array representing the selection
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_CANNOT_GET_SELECTION - if the operation fails
 *                because of an operating system failure while getting the
 *                selection</li>
 *                <li>ERROR_CANNOT_GET_ITEM - if the operation fails because
 *                of an operating system failure while getting an item</li>
 *                </ul>
 */
public String[] getSelection() {
    checkWidget();
    return ListUtils.getSelection(topHandle());
}

/**
 * Returns the number of selected items contained in the receiver.
 * 
 * @return the number of selected items
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_CANNOT_GET_COUNT - if the operation fails
 *                because of an operating system failure</li>
 *                </ul>
 */
public int getSelectionCount() {
    checkWidget();
    return ListUtils.getSelectionCount(topHandle());
}

/**
 * Returns the zero-relative indices of the items which are currently
 * selected in the receiver. The array is empty if no items are selected.
 * <p>
 * Note: This is not the actual structure used by the receiver to maintain
 * its selection, so modifying the array will not affect the receiver.
 * </p>
 * 
 * @return the array of indices of the selected items
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_CANNOT_GET_SELECTION - if the operation fails
 *                because of an operating system failure</li>
 *                </ul>
 */
public int[] getSelectionIndices() {
    checkWidget();
    return ListUtils.getSelectionIndices(topHandle());
}

/**
 * Returns the zero-relative index of the item which is currently at the top
 * of the receiver. This index can change when items are scrolled or new
 * items are added or removed.
 * 
 * @return the index of the top item
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li>
 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
 *                thread that created the receiver</li>
 *                </ul>
 */
public int getTopIndex() {
    checkWidget();
    return OS.QAbstractItemView_swt_indexAt(topHandle(), 1, 1);
}

    /**
 * Gets the index of an item.
 * <p>
 * The list is searched starting at 0 until an item is found that is equal
 * to the search item. If no item is found, -1 is returned. Indexing is zero
 * based.
 * 
 * @param string
 *            the search item
 * @return the index of the item
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 */
public int indexOf(String string) {
    return indexOf(string, 0);
}

/**
 * Searches the receiver's list starting at the given, zero-relative index
 * until an item is found that is equal to the argument, and returns the
 * index of that item. If no item is found or the starting index is out of
 * range, returns -1.
 * 
 * @param string
 *            the search item
 * @param start
 *            the zero-relative index at which to start the search
 * @return the index of the item
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_CANNOT_GET_COUNT - if the operation fails
 *                because of an operating system failure while getting the
 *                item count</li> <li>ERROR_CANNOT_GET_ITEM - if the
 *                operation fails because of an operating system failure
 *                while getting an item</li>
 *                </ul>
 */
public int indexOf(String string, int start) {
    checkWidget();
    return ListUtils.indexOf(topHandle(), string, start);
}

/**
 * Returns <code>true</code> if the item is selected, and <code>false</code>
 * otherwise. Indices out of range are ignored.
 * 
 * @param index
 *            the index of the item
 * @return the visibility state of the item at the index
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 */
public boolean isSelected(int index) {
    checkWidget();
    return ListUtils.isSelected(topHandle(), index);
}

/**
 * Removes the item from the receiver at the given zero-relative index.
 * 
 * @param index
 *            the index for the item
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_RANGE - if the index is not between 0
 *                and the number of elements in the list minus 1 (inclusive)
 *                </li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_ITEM_NOT_REMOVED - if the operation fails
 *                because of an operating system failure</li>
 *                </ul>
 */
public void remove(int index) {
    checkWidget();
    ListUtils.remove(topHandle(), index);
    icons.removeElementAt(index);
    resetGeometry();
}

/**
 * Removes the items from the receiver which are between the given
 * zero-relative start and end indices (inclusive).
 * 
 * @param start
 *            the start of the range
 * @param end
 *            the end of the range
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_RANGE - if either the start or end are
 *                not between 0 and the number of elements in the list minus
 *                1 (inclusive)</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_ITEM_NOT_REMOVED - if the operation fails
 *                because of an operating system failure</li>
 *                </ul>
 */
public void remove(int start, int end) {
    checkWidget();
    ListUtils.remove(topHandle(), start, end);
    for (int i = end; i >= start; i--) {
        icons.removeElementAt(i);
    }
    resetGeometry();
}

/**
 * Searches the receiver's list starting at the first item until an item is
 * found that is equal to the argument, and removes that item from the list.
 * 
 * @param string
 *            the item to remove
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
 *                called from the thread that created the receiver</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_ITEM_NOT_REMOVED - if the operation fails
 *                because of an operating system failure</li>
 *                </ul>
 */
public void remove(String string) {
    if (string == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    int index = indexOf(string, 0);
    if (index == -1) {
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
    remove(index);
}

/**
 * Removes the items from the receiver at the given zero-relative indices.
 * 
 * @param indices
 *            the array of indices of the items
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_RANGE - if the index is not between 0
 *                and the number of elements in the list minus 1 (inclusive)
 *                </li> <li>ERROR_NULL_ARGUMENT - if the indices array is
 *                null </li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_ITEM_NOT_REMOVED - if the operation fails
 *                because of an operating system failure</li>
 *                </ul>
 */
public void remove(int[] indices) {
    checkWidget();
    int[] sortedIdxs = ListUtils.remove(topHandle(), indices);
    int prevIndex = -1;
    for (int i = sortedIdxs.length - 1; i >= 0; i--) {
        if (prevIndex != sortedIdxs[i]) {
            icons.removeElementAt(sortedIdxs[i]);
        }
        prevIndex = sortedIdxs[i];
    }
    resetGeometry();
}

/**
 * Removes all of the items from the receiver.
 * <p>
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 */
public void removeAll() {
    checkWidget();
    ListUtils.removeAll(topHandle());
    resetGeometry();
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
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 * 
 * @see SelectionListener
 * @see #addSelectionListener
 */
public void removeSelectionListener(SelectionListener listener) {
    removeListener(SWT.Selection, listener);
    removeListener(SWT.DefaultSelection, listener);
}

/**
 * Selects the item at the given zero-relative index in the receiver's list.
 * If the item at the index was already selected, it remains selected.
 * Indices that are out of range are ignored.
 * 
 * @param index
 *            the index of the item to select
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 */
public void select(int index) {
    checkWidget();
    ignoreSelectionEvent = true;
    ListUtils.select(topHandle(), index, (SWT.MULTI & getStyle()) != 0);
    ignoreSelectionEvent = false;
}

/**
 * Selects the items in the range specified by the given zero-relative
 * indices in the receiver. The range of indices is inclusive. The current
 * selection is not cleared before the new items are selected.
 * <p>
 * If an item in the given range is not selected, it is selected. If an item
 * in the given range was already selected, it remains selected. Indices
 * that are out of range are ignored and no items will be selected if start
 * is greater than end. If the receiver is single-select and there is more
 * than one item in the given range, then all indices are ignored.
 * 
 * @param start
 *            the start of the range
 * @param end
 *            the end of the range
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 * 
 * @see List#setSelection(int,int)
 */
public void select(int start, int end) {
    checkWidget();
    ignoreSelectionEvent = true;
    ListUtils.select(topHandle(), start, end, (SWT.MULTI & getStyle()) != 0);
    ignoreSelectionEvent = false;
}

/**
 * Selects the items at the given zero-relative indices in the receiver. The
 * current selection is not cleared before the new items are selected.
 * <p>
 * If the item at a given index is not selected, it is selected. If the item
 * at a given index was already selected, it remains selected. Indices that
 * are out of range and duplicate indices are ignored. If the receiver is
 * single-select and multiple indices are specified, then all indices are
 * ignored.
 * 
 * @param indices
 *            the array of indices for the items to select
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the array of indices is null
 *                </li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 * 
 * @see List#setSelection(int[])
 */
public void select(int[] indices) {
    checkWidget();
    ignoreSelectionEvent = true;
    ListUtils.select(topHandle(), indices, (SWT.MULTI & getStyle()) != 0);
    ignoreSelectionEvent = false;
}

/**
 * Selects all of the items in the receiver.
 * <p>
 * If the receiver is single-select, do nothing.
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 */
public void selectAll() {
    checkWidget();
    ignoreSelectionEvent = true;
    ListUtils.selectAll(topHandle(), (SWT.MULTI & getStyle()) != 0);
    ignoreSelectionEvent = false;
}

/**
 * Sets the text and icon of the item in the receiver's list at the given
 * zero-relative index to the string argument. This is equivalent to
 * <code>remove</code> 'ing the old item at the index, and then
 * <code>add</code> 'ing the new item at that index.
 * 
 * @param index
 *            the index for the item
 * @param string
 *            the new text for the item
 * @param icon
 *            the icon image for the item, can be Null.
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_RANGE - if the index is not between 0
 *                and the number of elements in the list minus 1 (inclusive)
 *                </li> <li>ERROR_NULL_ARGUMENT - if the string is null</li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_ITEM_NOT_REMOVED - if the remove operation fails
 *                because of an operating system failure</li> <li>
 *                ERROR_ITEM_NOT_ADDED - if the add operation fails because
 *                of an operating system failure</li>
 *                </ul>
 */
public void setItem(int index, String string, Image icon) {
    checkWidget();
    if (string == null) {
        SWT.error(SWT.ERROR_NULL_ARGUMENT);
    }
    boolean isSelected = isSelected(index);
    // Disable redrawing to prevent flickering. Ensure that redrawing is
    // restored by using the try-finally block.
    setRedraw(false);
    try {
        remove(index);
        add(string, icon, index);
        if (isSelected) {
            select(index);
        }
        resetGeometry();
    } finally {
        setRedraw(true);
    }
}

/**
 * Sets the receiver's items to be the given array of items and icons.
 * 
 * @param items
 *            the array of items
 * @param icons
 *            the array of icons. Can be NULL.
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the <code>items</code> array
 *                is null</li> <li>ERROR_INVALID_RANGE - if number of
 *                elements in <code>icons</code> does not match the number
 *                in <code>items</code></li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 * @exception SWTError
 *                <ul>
 *                <li>ERROR_ITEM_NOT_ADDED - if the operation fails because
 *                of an operating system failure</li>
 *                </ul>
 */
public void setItems(String[] items, Image[] icons) {
    checkWidget();
    ListUtils.setItems(topHandle(), items, icons);
    this.icons.removeAllElements(); 
    for (int i = 0; i < items.length; i++) {
        this.icons.addElement((icons == null) ? null : icons[i]);
    }
    resetGeometry();
}

/**
 * Changes the widget layout orientation, which affects which way the widget
 * scrolls.
 * 
 * @param style
 *            the orientation style. Only SWT.VERTICAL and SWT.HORIZONTAL
 *            can be used.
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_ARGUMENT - if the style value is invalid
 *                </li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 * 
 * @see SWT#HORIZONTAL
 * @see SWT#VERTICAL
 */
public void setLayout(int layout) {
    checkWidget();
    if (layout != SWT.HORIZONTAL && layout != SWT.VERTICAL) {
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
    
    // ListView gets the other scrollbar created always when the layout is changed.
    setStyle(style() & ~SWT.VERTICAL);
    setStyle(style() & ~SWT.HORIZONTAL);
    int scrollAreaHandle = Internal_PackageSupport.scrollAreaHandle(this);
    if (layout == SWT.HORIZONTAL) {
        // 0 1 2
        // 3 4 5
        // 6 7 8
        setStyle(style() | SWT.V_SCROLL);
        if (getVerticalBar() == null) {
            Internal_PackageSupport.setVerticalBar(this, Internal_PackageSupport.createScrollBar(this, SWT.V_SCROLL));
        }
        OS.QAbstractScrollArea_setVerticalScrollBarPolicy(scrollAreaHandle, OS.QT_SCROLLBARALWAYSON);
        OS.QAbstractScrollArea_setHorizontalScrollBarPolicy(scrollAreaHandle, OS.QT_SCROLLBARALWAYSOFF);
        OS.QListView_setFlow(topHandle(), OS.QT_LISTVIEW_FLOW_LEFTTORIGHT);
    }
    else {
        // 0 3 6
        // 1 4 7
        // 2 5 8
        setStyle(style() | SWT.H_SCROLL);
        if (getHorizontalBar() == null) {
            Internal_PackageSupport.setHorizontalBar(this, Internal_PackageSupport.createScrollBar(this, SWT.H_SCROLL));
        }
        OS.QAbstractScrollArea_setHorizontalScrollBarPolicy(scrollAreaHandle, OS.QT_SCROLLBARALWAYSON);
        OS.QAbstractScrollArea_setVerticalScrollBarPolicy(scrollAreaHandle, OS.QT_SCROLLBARALWAYSOFF);
        OS.QListView_setFlow(topHandle(), OS.QT_LISTVIEW_FLOW_TOPTOBOTTOM);
    }
}

/**
 * Changes the item density level.
 * 
 * @param style
 *            the density level. Only HIGH, MEDIUM, LOW can be used.
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_INVALID_ARGUMENT - if the style value is invalid
 *                </li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 * 
 * @see ListView#LOW
 * @see ListView#MEDIUM
 * @see ListView#HIGH
 */
public void setLayoutDensity(int style) {
    checkWidget();
    if (style != LOW && style != MEDIUM && style != HIGH)
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    this.density = style;
    resetGeometry();
}

/**
 * Selects the item at the given zero-relative index in the receiver. If the
 * item at the index was already selected, it remains selected. The current
 * selected is first cleared, then the new items are selected. Indices that
 * are out of range are ignored.
 * 
 * @param index
 *            the index of the item to select
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 * @see List#deselectAll()
 * @see List#select(int)
 */
public void setSelection(int index) {
    checkWidget();
    ignoreSelectionEvent = true;
    ListUtils.setSelection(topHandle(), index);
    ignoreSelectionEvent = false;
}

/**
 * Selects the items in the range specified by the given zero-relative
 * indices in the receiver. The range of indices is inclusive. The current
 * selection is cleared before the new items are selected.
 * <p>
 * Indices that are out of range are ignored and no items will be selected
 * if start is greater than end. If the receiver is single-select and there
 * is more than one item in the given range, then all indices are ignored.
 * 
 * @param start
 *            the start index of the items to select
 * @param end
 *            the end index of the items to select
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 * 
 * @see List#deselectAll()
 * @see List#select(int,int)
 */
public void setSelection(int start, int end) {
    checkWidget();
    ignoreSelectionEvent = true;
    ListUtils.setSelection(topHandle(), start, end, (SWT.MULTI & getStyle()) != 0);
    ignoreSelectionEvent = false;
}

/**
 * Selects the items at the given zero-relative indices in the receiver. The
 * current selection is cleared before the new items are selected.
 * <p>
 * Indices that are out of range and duplicate indices are ignored. If the
 * receiver is single-select and multiple indices are specified, then all
 * indices are ignored.
 * 
 * @param indices
 *            the indices of the items to select
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the array of indices is null
 *                </li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 * 
 * @see List#deselectAll()
 * @see List#select(int[])
 */
public void setSelection(int[] indices) {
    checkWidget();
    ignoreSelectionEvent = true;
    ListUtils.setSelection(topHandle(), indices, (SWT.MULTI & getStyle()) != 0);
    ignoreSelectionEvent = false;
}

/**
 * Sets the receiver's selection to be the given array of items. The current
 * selection is cleared before the new items are selected.
 * <p>
 * Items that are not in the receiver are ignored. If the receiver is
 * single-select and multiple items are specified, then all items are
 * ignored.
 * 
 * @param items
 *            the array of items
 * 
 * @exception IllegalArgumentException
 *                <ul>
 *                <li>ERROR_NULL_ARGUMENT - if the array of items is null
 *                </li>
 *                </ul>
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 * 
 * @see List#deselectAll()
 * @see List#select(int[])
 * @see List#setSelection(int[])
 */
public void setSelection(String[] items) {
    checkWidget();
    ignoreSelectionEvent = true;
    ListUtils.setSelection(topHandle(), items, (SWT.MULTI & getStyle()) != 0);
    ignoreSelectionEvent = false;
}

/**
 * Shows the selection. If the selection is already showing in the receiver,
 * this method simply returns. Otherwise, the items are scrolled until the
 * selection is visible. When multiple items are selected, the selection of
 * which item becomes visible is implementation-dependent.
 * 
 * @exception SWTException
 *                <ul>
 *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
 *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
 *                called from the thread that created the receiver</li>
 *                </ul>
 */
public void showSelection() {
    checkWidget();
    ListUtils.showSelection(topHandle());
}

void createHandle(int index) {
    Internal_PackageSupport.createHandle(this, index);
    int scrollAreaHandle = OS.QListWidget_new();
    Internal_PackageSupport.setScrollAreaHandle(this, scrollAreaHandle);
    Internal_PackageSupport.setFrameHandle(this, scrollAreaHandle);
    Internal_PackageSupport.setHandle(this, OS.QAbstractScrollArea_viewPort(scrollAreaHandle));
    Internal_PackageSupport.setTopHandle(this, scrollAreaHandle);
    
    OS.QListView_setMovement(scrollAreaHandle, OS.QT_LISTVIEW_MOVEMENT_STATIC);
    OS.QListView_setViewMode(scrollAreaHandle, OS.QT_LISTVIEW_VIEWMODE_ICON);
    OS.QListView_setResizeMode(scrollAreaHandle, OS.QT_LISTVIEW_RESIZEMODE_ADJUST);
    OS.QAbstractItemView_setSelectionBehavior(scrollAreaHandle, OS.QT_ABSTRACTITEMVIEW_SELECTIONBEHAVIOR_ITEMS);
    if ((getStyle() & SWT.MULTI) != 0) {
        OS.QAbstractItemView_setSelectionMode(scrollAreaHandle, OS.QT_ABSTRACTITEMVIEW_SELECTIONMODE_MULTI);
    }
    else {
        OS.QAbstractItemView_setSelectionMode(scrollAreaHandle, OS.QT_ABSTRACTITEMVIEW_SELECTIONMODE_SINGLE);
    }
    if ((getStyle() & SWT.H_SCROLL) != 0) {
        setLayout(SWT.VERTICAL);
    }
    else {
        setLayout(SWT.HORIZONTAL);
    }
    spacing = OS.QListView_spacing(scrollAreaHandle);
    density = MEDIUM;
    
    Internal_PackageSupport.setState(this, Internal_PackageSupport.state(this) | WidgetState.HANDLE);
}

void hookEvents () {
    Internal_PackageSupport.hookEvents(this);
    int handle = topHandle();
    int signalProxySel = OS.SignalHandler_new(handle, Internal_PackageSupport.display(this), 
            OS.QSIGNAL_LIST_ITEMSELECTION_CHANGED);
    OS.QObject_connectOrThrow(handle, "itemSelectionChanged()", 
            signalProxySel, "widgetSignal()", OS.QT_AUTOCONNECTION);
    int signalProxyDefSel = OS.SignalHandler_new(handle, Internal_PackageSupport.display(this), 
            OS.QSIGNAL_LIST_ITEM_DOUBLECLICKED);
    OS.QObject_connectOrThrow(handle, "itemDoubleClicked(QListWidgetItem*)", 
            signalProxyDefSel, "widgetSignal(QListWidgetItem*)", OS.QT_AUTOCONNECTION);
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

public void setFont(Font font) {
    super.setFont(font);
    resetGeometry();
}

void setTraversalFlags(int type, int key, int modifier, int character) {
    Internal_PackageSupport.setTraverseDoIt(this, false);
    Internal_PackageSupport.setTraverseCancel(this, false);
    
    int handle = topHandle();
    
    switch (key) {    
    case OS.QT_KEY_UP: {
        // ^  ^  ^  ^       ^  ^  ^  ^  
        // 4  5  6  7       1  5  9  13
        // 8  9  10 11      2  6  10
        // 12 13            3  7  11
        if (getVisualRow(ListUtils.getFocusIndex(handle)) == 0) {
            Internal_PackageSupport.setTraverseDoIt(this, true);
            Internal_PackageSupport.setTraverseCancel(this, true);
        }
        break;
    }    
    case OS.QT_KEY_LEFT: {
        // <  1  2  3       <  4  8  12
        // <  5  6  7       <  5  9  13
        // <  9  10 11      <  6  10
        // <  13            <  7  11
        if (getVisualCol(ListUtils.getFocusIndex(handle)) == 0) {
            Internal_PackageSupport.setTraverseDoIt(this, true);
            Internal_PackageSupport.setTraverseCancel(this, true);
        }
        break;
    }
    case OS.QT_KEY_DOWN: {
        int focusIndex = ListUtils.getFocusIndex(handle);
        int itemCount = ListUtils.getItemCount(handle);
        int rowCount = getVisualRowCount();
        int focusRow = getVisualRow(focusIndex);
        if (getLayout() == SWT.HORIZONTAL) {
            // 0  1  2  3
            // 4  5  6  7
            // 8  9  v  v
            // v  v
            if ((focusRow == rowCount - 1)
                || ((focusRow == rowCount - 2) 
                    && (getVisualCol(focusIndex) > getVisualCol(itemCount - 1)))) {
                Internal_PackageSupport.setTraverseDoIt(this, true);
                Internal_PackageSupport.setTraverseCancel(this, true);
            }
        }
        else {
            // 0  4  8  12
            // 1  5  9  v
            // 2  6  10
            // v  v  v
            if ((focusIndex == itemCount - 1)
                || (focusRow == rowCount - 1)) {
                Internal_PackageSupport.setTraverseDoIt(this, true);
                Internal_PackageSupport.setTraverseCancel(this, true);
            }
        }
        break;
    }
    case OS.QT_KEY_RIGHT: {
        int focusIndex = ListUtils.getFocusIndex(handle);
        int itemCount = ListUtils.getItemCount(handle);
        int colCount = getVisualColCount();
        int focusCol = getVisualCol(focusIndex);
        if (getLayout() == SWT.HORIZONTAL) {
            // 0  1  2  >
            // 4  5  6  > 
            // 8  9  10 >
            // 12 >
            if ((focusIndex == itemCount - 1)
                || (focusCol == getVisualColCount() - 1)) {
                Internal_PackageSupport.setTraverseDoIt(this, true);
                Internal_PackageSupport.setTraverseCancel(this, true);
            }
        }
        else {
            // 0  4  8  >
            // 1  5  9  >
            // 2  6  >
            // 3  7  >
            if ((focusCol == colCount - 1)
                || ((focusCol == colCount - 2) 
                    && (getVisualRow(focusIndex) > getVisualRow(itemCount - 1)))) {
                Internal_PackageSupport.setTraverseDoIt(this, true);
                Internal_PackageSupport.setTraverseCancel(this, true);
            }
        }
        break;
    }    
    // All other traverse keys can traverse
    default: {
        Internal_PackageSupport.setTraverseDoIt(this, true);
        Internal_PackageSupport.setTraverseCancel(this, true);
        break;
    }
    }
}

Point getPreferredClientAreaSize() {
    int count = getItemCount();
    if (count > 0) {
        int x = 0;
        int y = 0;
        Point gridSize = OS.QListView_gridSize(topHandle());
        if (getLayout() == SWT.HORIZONTAL) {
            x += gridSize.x * count;
            y += gridSize.y;
            x += 16;
        } else {
            x += gridSize.x;
            y += gridSize.y * count;
            y += 16;
        }
        
        return new Point(x, y);
    } 
    else {
        return OS.QWidget_sizeHint(topHandle());
    }
}

/*
 * Style filter
 */
private static int checkStyle(int style) {
    // VERTICAL layout goes with H_SCROLL, HORIZONTAL layout goes with V_SCROLL
    // Default layout: HORIZONTAL
    int layoutStyle = Internal_PackageSupport.checkBits (style, SWT.HORIZONTAL, SWT.VERTICAL, 0, 0, 0, 0);
    style &= ~SWT.VERTICAL;
    style &= ~SWT.HORIZONTAL;
    if ((layoutStyle & SWT.VERTICAL) != 0) {
        style |= SWT.H_SCROLL;
    }
    else {
        style |= SWT.V_SCROLL;
    }
    return Internal_PackageSupport.checkBits (style, SWT.SINGLE, SWT.MULTI, 0, 0, 0, 0);
}

/*
 * Get spacing based on density level
 */
private int getSpacing(int density) {
    switch(density) {
    case LOW:
        if (spacing != 0) {
            return spacing * 2;
        }
        else {
//          Rectangle dispRect = getDisplay().getBounds(); 
//          return Math.max(dispRect.height,dispRect.width) / 50;
          return 20;
        }
    case HIGH: {
          return 0;
        }
    case MEDIUM:
    // fall trough
    default:
        return spacing;
    }
}

/*
 * Visual column index for index 
 */
private int getVisualCol(int index) {
    if (getLayout() == SWT.HORIZONTAL) {
        return index % getVisualColCount();
    }
    else {
        return (int)Math.floor(index/ getVisualRowCount());
    }
}

/*
 * Visual row index for index 
 */
private int getVisualRow(int index) {
    if (getLayout() == SWT.HORIZONTAL) {
        return (int)Math.floor(index / getVisualColCount());
    }
    else {
        return index % getVisualRowCount();
    }
}

/*
 * Count visual columns 
 */
private int getVisualColCount() {
    int handle = topHandle();
    int res = 1;
    int count = ListUtils.getItemCount(handle);
    if (getLayout() == SWT.HORIZONTAL) {
        int y = OS.QAbstractItemView_visualRect(handle, 0, 0).y;
        int i = 1;
        while (i < count) {
            int yTemp = y;
            y = OS.QAbstractItemView_visualRect(handle, i, 0).y;
            if (yTemp != y) {
                break;
            }
            res++;
            i++;
        }
    }
    else {
        int rowCount = getVisualRowCount();
        res = (int)Math.floor(count / rowCount);
        if (count % rowCount > 0) {
            res++;
        }
    }
    return res;
}

/*
 * Count visual rows 
 */
private int getVisualRowCount() {
    int handle = topHandle();
    int res = 1;
    int count = ListUtils.getItemCount(handle);
    if (getLayout() == SWT.HORIZONTAL) {
        int colCount = getVisualColCount();
        res = (int)Math.floor(count / colCount);
        if (count % colCount > 0) {
            res++;
        }
    }
    else {
        int x = OS.QAbstractItemView_visualRect(handle, 0, 0).x;
        int i = 1;
        while (i < count) {
            int xTemp = x;
            x = OS.QAbstractItemView_visualRect(handle, i, 0).x;
            if (xTemp != x) {
                break;
            }
            res++;
            i++;
        }
    }
    return res;
}

/*
 * Recalculates the geometry of the list (cell size, icon size, etc.).
 */
private void resetGeometry() {
    resetGeometry(-1);
}

/*
 * Recalculates the geometry of the list (cell size, icon size, etc.). Optimized. 
 */
private void resetGeometry(int lastAddedItemIndex) {
    // In this order
    resetIconSize(lastAddedItemIndex);
    resetGridSize(lastAddedItemIndex);
}

/*
 * Resets the cell size. Optimized if lastAddedItemIndex not -1.
 */
private void resetGridSize(int lastAddedItemIndex) {
    int handle = topHandle();
    Point size;
    if (lastAddedItemIndex > -1 ) {
        size = OS.QListView_gridSize(handle);
        Point s = OS.QAbstractItemView_sizeHintForIndex(handle, lastAddedItemIndex, 0);
        size.x = Math.max(size.x, s.x);
        size.y = Math.max(size.y, s.y);        
    }
    else {
        size = new Point(0, 0);
        for (int i = 0; i < getItemCount(); i++) {
            Point s = OS.QAbstractItemView_sizeHintForIndex(handle, i, 0);
            size.x = Math.max(size.x, s.x);
            size.y = Math.max(size.y, s.y);
        }
    }
    int extra = getSpacing(density);
    OS.QListView_setGridSize(handle, size.x + extra, size.y + extra);
}

/*
 * Resets the icon size. Optimized if lastAddedItemIndex not -1.
 */
private void resetIconSize(int lastAddedItemIndex) {
    int handle = topHandle();
    Point size;
    if (lastAddedItemIndex > -1 && getIcon(lastAddedItemIndex) != null) {
        size = OS.QAbstractItemView_iconSize(handle);
        Rectangle iconRect = getIcon(lastAddedItemIndex).getBounds();
        size.x = Math.max(size.x, iconRect.width);
        size.y = Math.max(size.y, iconRect.height);        
    }
    else {
        size = new Point(0, 0);
        for (int i = 0; i < getItemCount(); i++) {
            Image icon = getIcon(i);
            if (icon != null) {
                Rectangle iconRect = icon.getBounds();
                size.x = Math.max(size.x, iconRect.width);
                size.y = Math.max(size.y, iconRect.height);
            }
        }
    }
    OS.QAbstractItemView_setIconSize(handle, size.x, size.y);
}

private final int topHandle() {
    return Internal_PackageSupport.topHandle(this);
}

private final int style() {
    return Internal_PackageSupport.style(this);
}

private final void setStyle(int style) {
    Internal_PackageSupport.setStyle(this, style);
}
}
