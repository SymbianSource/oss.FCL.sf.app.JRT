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
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.internal.qt.ListUtils;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.WidgetState;

/**
 * Instances of this class represent a selectable user interface object that
 * displays a list of strings and issues notification when a string is selected.
 * A list may be single or multi select.
 * <p>
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>SINGLE, MULTI</dd>
 * <dt><b>Events:</b></dt>
 * <dd>Selection, DefaultSelection</dd>
 * </dl>
 * <p>
 * Note: Only one of SINGLE and MULTI may be specified.
 * </p>
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */
public class List extends Scrollable {
    private boolean ignoreSelectionEvent;

    /**
     * Constructs a new instance of this class given its parent and a style
     * value describing its behavior and appearance.
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or must
     * be built by <em>bitwise OR</em>'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. The class description lists the style constants that are
     * applicable to the class. Style bits are also inherited from superclasses.
     * </p>
     * 
     * @param parent
     *            a composite control which will be the parent of the new
     *            instance (cannot be null)
     * @param style
     *            the style of control to construct
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
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public List(Composite parent, int style) {
        super(parent, checkStyle(style));
    }

    static int checkStyle(int style) {
        return checkBits(style, SWT.SINGLE, SWT.MULTI, 0, 0, 0, 0);
    }

    void createHandle_pp(int index) {
        frameHandle = topHandle = scrollAreaHandle = OS.QListWidget_new();
        handle = OS.QAbstractScrollArea_viewPort(scrollAreaHandle);
        if ((style & SWT.MULTI) != 0) {
            OS.QAbstractItemView_setSelectionMode(scrollAreaHandle,
                    OS.QT_ABSTRACTITEMVIEW_SELECTIONMODE_MULTI);
        }
        state |= WidgetState.HANDLE;
    }

    /**
     * Adds the argument to the end of the receiver's list.
     * 
     * @param string
     *            the new item
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @see #add(String,int)
     */
    public void add(String string) {
        checkWidget();
        ListUtils.add(topHandle, string, null);
    }

    /**
     * Adds the argument to the receiver's list at the given zero-relative
     * index.
     * <p>
     * Note: To add an item at the end of the list, use the result of calling
     * <code>getItemCount()</code> as the index or use <code>add(String)</code>.
     * </p>
     * 
     * @param string
     *            the new item
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
     * 
     * @see #add(String)
     */
    public void add(String string, int index) {
        checkWidget();
        ListUtils.add(topHandle, string, null, index);
    }

    /**
     * Adds the listener to the collection of listeners who will be notified
     * when the user changes the receiver's selection, by sending it one of the
     * messages defined in the <code>SelectionListener</code> interface.
     * <p>
     * <code>widgetSelected</code> is called when the selection changes.
     * <code>widgetDefaultSelected</code> is typically called when an item is
     * double-clicked.
     * </p>
     * 
     * @param listener
     *            the listener which should be notified when the user changes
     *            the receiver's selection
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
     * @see SelectionEvent
     */
    public void addSelectionListener(SelectionListener listener) {
        checkWidget();
        if (listener == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Selection, typedListener);
        addListener(SWT.DefaultSelection, typedListener);
    }

    void hookEvents_pp() {
        super.hookEvents_pp();

        int selectionSignalProxy = OS.SignalHandler_new(topHandle,
                OS.QSIGNAL_LIST_ITEMSELECTION_CHANGED);

        OS.QObject_connectOrThrow(topHandle,
                "itemSelectionChanged()", selectionSignalProxy,
                "widgetSignal()", OS.QT_AUTOCONNECTION);

        int itemActivatedSignalProxy = OS.SignalHandler_new(topHandle,
              OS.QSIGNAL_LIST_ITEM_ACTIVATED);

        OS.QObject_connectOrThrow(topHandle,
              "itemActivated(QListWidgetItem* )", itemActivatedSignalProxy,
              "widgetSignal(QListWidgetItem* )", OS.QT_AUTOCONNECTION);
     }

    void qt_signal_list_itemSelectionChanged_pp() {
        if (!ignoreSelectionEvent) {
            sendEvent(SWT.Selection);
        }
    }

    void qt_signal_list_itemActivated_pp(int selectionHandle) {
        if (!ignoreSelectionEvent) {
            sendEvent(SWT.DefaultSelection);    
        }
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
        ListUtils.deselect(topHandle, index);
        ignoreSelectionEvent = false;
    }

    /**
     * Deselects the items at the given zero-relative indices in the receiver.
     * If the item at the given zero-relative index in the receiver is selected,
     * it is deselected. If the item at the index was not selected, it remains
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
        ListUtils.deselect(topHandle, start, end);
        ignoreSelectionEvent = false;
    }

    /**
     * Deselects the items at the given zero-relative indices in the receiver.
     * If the item at the given zero-relative index in the receiver is selected,
     * it is deselected. If the item at the index was not selected, it remains
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
        ListUtils.deselect(topHandle, indices);
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
        ListUtils.deselectAll(topHandle);
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
        return OS.QListWidget_currentRow(topHandle);

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
     */
    public String getItem(int index) {
        checkWidget();
        return ListUtils.getItem(topHandle, index);
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
     */
    public int getItemCount() {
        checkWidget();
        return OS.QListWidget_count(topHandle);
    }

    /**
     * Returns the height of the area which would be used to display
     * <em>one</em> of the items in the list.
     * 
     * @return the height of one item
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public int getItemHeight() {
        checkWidget();
        return ListUtils.getItemHeight(topHandle);
    }

    /**
     * Returns a (possibly empty) array of <code>String</code>s which are the
     * items in the receiver.
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
     */
    public String[] getItems() {
        checkWidget();
        return ListUtils.getItems(topHandle);
    }

    /**
     * Returns an array of <code>String</code>s that are currently selected in
     * the receiver. The order of the items is unspecified. An empty array
     * indicates that no items are selected.
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
     */
    public String[] getSelection() {
        checkWidget();
        return ListUtils.getSelection(topHandle);
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
     */
    public int getSelectionCount() {
        checkWidget();
        return ListUtils.getSelectionCount(topHandle);
    }

    /**
     * Returns the zero-relative index of the item which is currently selected
     * in the receiver, or -1 if no item is selected.
     * 
     * @return the index of the selected item or -1
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public int getSelectionIndex() {
        int[] indices = getSelectionIndices();
        if (indices.length < 1)
            return -1;
        return indices[0];

    }

    /**
     * Returns the zero-relative indices of the items which are currently
     * selected in the receiver. The order of the indices is unspecified. The
     * array is empty if no items are selected.
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
     */
    public int[] getSelectionIndices() {
        checkWidget();
        return ListUtils.getSelectionIndices(topHandle);
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
        if (getItemCount() < 1)
            return 0;
        return OS.QAbstractItemView_swt_indexAt(topHandle, 1, 1);
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
     */
    public int indexOf(String string, int start) {
        checkWidget();
        return ListUtils.indexOf(topHandle, string, start);
    }

    /**
     * Returns <code>true</code> if the item is selected, and <code>false</code>
     * otherwise. Indices out of range are ignored.
     * 
     * @param index
     *            the index of the item
     * @return the selection state of the item at the index
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
        return ListUtils.isSelected(topHandle, index);
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
     */
    public void remove(int index) {
        checkWidget();
        ListUtils.remove(topHandle, index);
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
     */
    public void remove(int start, int end) {
        checkWidget();
        ListUtils.remove(topHandle, start, end);
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
     */
    public void remove(String string) {
        checkWidget();
        ListUtils.remove(topHandle, string);
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
     *                null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public void remove(int[] indices) {
        checkWidget();
        ListUtils.remove(topHandle, indices);
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
        ListUtils.removeAll(topHandle);
    }

    /**
     * Removes the listener from the collection of listeners who will be
     * notified when the user changes the receiver's selection.
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
        checkWidget();
        if (listener == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null)
            return;
        eventTable.unhook(SWT.Selection, listener);
        eventTable.unhook(SWT.DefaultSelection, listener);
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
        ListUtils.select(topHandle, index, (SWT.MULTI & getStyle()) != 0);
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
        ListUtils.select(topHandle, start, end, (SWT.MULTI & getStyle()) != 0);
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
        ListUtils.select(topHandle, indices, (SWT.MULTI & getStyle()) != 0);
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
        ListUtils.selectAll(topHandle, (SWT.MULTI & getStyle()) != 0);
        ignoreSelectionEvent = false;
    }

    /**
     * Sets the text of the item in the receiver's list at the given
     * zero-relative index to the string argument.
     * 
     * @param index
     *            the index for the item
     * @param string
     *            the new text for the item
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
     */
    public void setItem(int index, String string) {
        checkWidget();
        if (string == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
              
        if (index < 0 || index >= OS.QListWidget_count(topHandle) ) { 
            SWT.error(SWT.ERROR_INVALID_RANGE);
        }
       
        boolean isSelected = isSelected(index);
        OS.QListWidget_swt_setText(topHandle, index, string);
        if (isSelected) {
          select(index);
        }
    }

    /**
     * Sets the receiver's items to be the given array of items.
     * 
     * @param items
     *            the array of items
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the items array is null</li>
     *                <li>ERROR_INVALID_ARGUMENT - if an item in the items array
     *                is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public void setItems(String[] items) {
        checkWidget();
        if (items == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);

        removeAll();
        int count = items.length;
        if (count < 1)
            return;

        OS.QListWidget_addItems(topHandle, items, null);
    }

    /**
     * Selects the item at the given zero-relative index in the receiver. If the
     * item at the index was already selected, it remains selected. The current
     * selection is first cleared, then the new item is selected. Indices that
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
        ListUtils.setSelection(topHandle, index);
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
        ListUtils.setSelection(topHandle, start, end, (SWT.MULTI & getStyle()) != 0);
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
        ListUtils.setSelection(topHandle, indices, (SWT.MULTI & getStyle()) != 0);
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
        ListUtils.setSelection(topHandle, items, (SWT.MULTI & getStyle()) != 0);
        ignoreSelectionEvent = false;
    }

    /**
     * Sets the zero-relative index of the item which is currently at the top of
     * the receiver. This index can change when items are scrolled or new items
     * are added and removed.
     * 
     * @param index
     *            the index of the top item
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public void setTopIndex(int index) {
        checkWidget();
        if (index < 0 || index > getItemCount() - 1)
            return;
        OS.QListWidget_scrollToItem(topHandle, index, 
            OS.QT_ABSTRACTITEMVIEW_SCROLLHINT_POSITIONATTOP);
    }

    /**
     * Shows the selection. If the selection is already showing in the receiver,
     * this method simply returns. Otherwise, the items are scrolled until the
     * selection is visible.
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
        ListUtils.showSelection(topHandle);
    }

    /*
     * @see Scrollable.getPrefferedClientAreaSize
     */
    Point getPreferredClientAreaSize_pp() {
        int x = OS.QAbstractItemView_sizeHintForColumn(topHandle, 0);
        int y = OS.QAbstractItemView_sizeHintForIndex(
                topHandle, 0, 0).y*getItemCount();
        return new Point(x, y);
    }

    /*
     * @see Control.setTraversalFlags
     */
    void setTraversalFlags_pp(int type, int key, int modifier,
            int character) {
        int handle = topHandle;
        traverseDoit = false;
        traverseCancel = false;
        if (type == SWT.TRAVERSE_RETURN) {
            return;
        }
        
        switch (key) {
        case OS.QT_KEY_UP:
        case OS.QT_KEY_LEFT:
            if (ListUtils.getFocusIndex(handle) == 0) {
                traverseDoit = true;
            }
            break;
        case OS.QT_KEY_DOWN:
        case OS.QT_KEY_RIGHT:
            if (ListUtils.getFocusIndex(handle) == ListUtils.getItemCount(handle) - 1) {
                traverseDoit = true;
            }
            break;
        default:
            super.setTraversalFlags_pp(type, key, modifier, character);
            break;
        }
    }

    void setHBarPolicy(boolean status) {
        if (status)
            super.setHBarPolicy(status);
        // Qt bug: Cannot turn H Bar completely OFF because getTopIndex() would fail
    }
}
