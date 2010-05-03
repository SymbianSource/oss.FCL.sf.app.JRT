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


import java.util.Vector;

import org.eclipse.ercp.swt.expanded.internal.OS;
import org.eclipse.swt.*;
import org.eclipse.swt.graphics.*;
import org.eclipse.swt.events.*;

/**
 * Instances of this class implement a selectable user interface
 * object that displays a list of images and strings and issue
 * notificiation when selected.
 * <p>
 * The item children that may be added to instances of this class
 * must be of type <code>TableItem</code>.
 * </p><p>
 * Note that although this class is a subclass of <code>Composite</code>,
 * it does not make sense to add <code>Control</code> children to it,
 * or set a layout on it.
 * </p><p>
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>SINGLE, MULTI, CHECK, FULL_SELECTION, HIDE_SELECTION, VIRTUAL</dd>
 * <dt><b>Events:</b></dt>
 * <dd>Selection, DefaultSelection</dd>
 * </dl>
 * <p>
 * Note: Only one of the styles SINGLE, and MULTI may be specified.
 * </p><p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */

public class Table extends Composite
{

    // keep it package, TableItem needs it
    int tableHandle;
    int columnCount = 0;

    // This is used to keep a reference on the TableItems to prevent the garbage collector to destroy the TableItems.
    Vector tableItems = new Vector();
    Vector tableColumns = new Vector();

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
     * @see SWT#SINGLE
     * @see SWT#MULTI
     * @see SWT#CHECK
     * @see SWT#FULL_SELECTION
     * @see SWT#HIDE_SELECTION
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public Table(Composite parent, int style)
    {
        // Table is supposed to always have scrollbars to align with MS Windows
        // implementation. The style bits are always added because Scrollable
        // determines from them if it should create the scrollbars.
        super(parent, checkStyle(style | SWT.V_SCROLL | SWT.H_SCROLL));
    }

    static int checkStyle(int style)
    {
        return internal_checkBits(style, SWT.SINGLE, SWT.MULTI, 0, 0 , 0, 0);
    }

    final void setTableHandle(int tableHandle)
    {
        this.tableHandle = tableHandle;
        internal_setCompositeHandle(OS.Table_CompositeHandle(tableHandle));
    }


    protected void internal_createHandle()
    {
        setTableHandle(OS.Table_New(this, getParent().internal_getCompositeHandle(), style));
    }

    protected void checkSubclass()
    {
        if (!internal_isValidSubclass()) internal_error(SWT.ERROR_INVALID_SUBCLASS);
    }

    protected void internal_releaseResources()
    {

        // Release children

        // release tableItems
        removeAll();

        // release tableColumns
        TableColumn[] columns = OS.Table_GetTableColumns(tableHandle);
        if (columns != null)
        {
            int count = columns.length;
            for (int i=0; i<count; ++i)
            {
                TableColumn column = columns[i];
                column.internal_release();
            }
        }

        super.internal_releaseResources();
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notified when the receiver's selection changes, by sending
     * it one of the messages defined in the <code>SelectionListener</code>
     * interface.
     * <p>
     * When <code>widgetSelected</code> is called, the item field of the event object is valid.
     * If the reciever has <code>SWT.CHECK</code> style set and the check selection changes,
     * the event object detail field contains the value <code>SWT.CHECK</code>.
     * <code>widgetDefaultSelected</code> is typically called when an item is double-clicked.
     * The item field of the event object is valid for default selection, but the detail field is not used.
     * </p>
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
     * @see SelectionListener
     * @see #removeSelectionListener
     * @see SelectionEvent
     */
    public void addSelectionListener(SelectionListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Selection, typedListener);
        addListener(SWT.DefaultSelection, typedListener);
    }


    public Point computeSize(int wHint, int hHint, boolean changed)
    {
        checkWidget();
        return org.eclipse.swt.internal.symbian.OS.Control_ComputeSize(handle, wHint, hHint);
    }


    /**
     * Deselects the items at the given zero-relative indices in the receiver.
     * If the item at the given zero-relative index in the receiver
     * is selected, it is deselected.  If the item at the index
     * was not selected, it remains deselected. Indices that are out
     * of range and duplicate indices are ignored.
     *
     * @param indices the array of indices for the items to deselect
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the set of indices is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void deselect(int [] indices)
    {
        checkWidget();
        if (indices == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (indices.length == 0) return;
        OS.Table_Deselect(tableHandle, indices);
    }

    /**
     * Deselects the item at the given zero-relative index in the receiver.
     * If the item at the index was already deselected, it remains
     * deselected. Indices that are out of range are ignored.
     *
     * @param index the index of the item to deselect
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void deselect(int index)
    {
        deselect(index, index);
    }

    /**
     * Deselects the items at the given zero-relative indices in the receiver.
     * If the item at the given zero-relative index in the receiver
     * is selected, it is deselected.  If the item at the index
     * was not selected, it remains deselected.  The range of the
     * indices is inclusive. Indices that are out of range are ignored.
     *
     * @param start the start index of the items to deselect
     * @param end the end index of the items to deselect
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void deselect(int start, int end)
    {
        checkWidget();
        if (start > end) return;
        if (end < 0) return;
        OS.Table_Deselect(tableHandle, start, end);
    }

    /**
     * Deselects all selected items in the receiver.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void deselectAll()
    {
        deselect(0, Integer.MAX_VALUE);
    }

    /**
     * Returns the column at the given, zero-relative index in the
     * receiver. Throws an exception if the index is out of range.
     * If no <code>TableColumn</code>s were created by the programmer,
     * this method will throw <code>ERROR_INVALID_RANGE</code> despite
     * the fact that a single column of data may be visible in the table.
     * This occurs when the programmer uses the table like a list, adding
     * items but never creating a column.
     *
     * @param index the index of the column to return
     * @return the column at the given index
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_RANGE - if the index is not between 0 and the number of elements in the list minus 1 (inclusive)</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public TableColumn getColumn(int index)
    {
        checkWidget();
        return OS.Table_GetColumn(tableHandle,index);
    }

    /**
     * Returns the number of columns contained in the receiver.
     * If no <code>TableColumn</code>s were created by the programmer,
     * this value is zero, despite the fact that visually, one column
     * of items is may be visible. This occurs when the programmer uses
     * the table like a list, adding items but never creating a column.
     *
     * @return the number of columns
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getColumnCount()
    {
        checkWidget();
        return columnCount;
    }

    /**
     * Returns an array of <code>TableColumn</code>s which are the
     * columns in the receiver. If no <code>TableColumn</code>s were
     * created by the programmer, the array is empty, despite the fact
     * that visually, one column of items may be visible. This occurs
     * when the programmer uses the table like a list, adding items but
     * never creating a column.
     * <p>
     * Note: This is not the actual structure used by the receiver
     * to maintain its list of items, so modifying the array will
     * not affect the receiver.
     * </p>
     *
     * @return the items in the receiver
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public TableColumn [] getColumns()
    {
        checkWidget();
        return OS.Table_GetTableColumns(tableHandle);
    }

    /**
     * Returns the width in pixels of a grid line.
     *
     * @return the width of a grid line in pixels
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getGridLineWidth()
    {
        checkWidget();
        return OS.Table_GetGridLineWidth(tableHandle);
    }

    /**
     * Returns the height of the receiver's header
     *
     * @return the height of the header or zero if the header is not visible
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 2.0
     */
    public int getHeaderHeight()
    {
        checkWidget();
        return OS.Table_GetHeaderHeight(tableHandle);
    }

    /**
     * Returns <code>true</code> if the receiver's header is visible,
     * and <code>false</code> otherwise.
     * <p>
     * If one of the receiver's ancestors is not visible or some
     * other condition makes the receiver not visible, this method
     * may still indicate that it is considered visible even though
     * it may not actually be showing.
     * </p>
     *
     * @return the receiver's header's visibility state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean getHeaderVisible()
    {
        checkWidget();
        return OS.Table_GetHeaderVisible(tableHandle);
    }

    /**
     * Returns the item at the given, zero-relative index in the
     * receiver. Throws an exception if the index is out of range.
     *
     * @param index the index of the item to return
     * @return the item at the given index
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_RANGE - if the index is not between 0 and the number of elements in the list minus 1 (inclusive)</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public TableItem getItem(int index)
    {
        checkWidget();
        return OS.Table_GetItem(tableHandle,index);
    }

    /**
     * Returns the item at the given point in the receiver
     * or null if no such item exists. The point is in the
     * coordinate system of the receiver.
     *
     * @param point the point used to locate the item
     * @return the item at the given point
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the point is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public TableItem getItem(Point point)
    {
        checkWidget();
        if (point == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        return OS.Table_GetItemFromPoint(tableHandle, point.x, point.y);
    }

    /**
     * Returns the number of items contained in the receiver.
     *
     * @return the number of items
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getItemCount()
    {
        checkWidget();
        return OS.Table_GetItemCount(tableHandle);
    }

    /**
     * Returns the height of the area which would be used to
     * display <em>one</em> of the items in the receiver's.
     *
     * @return the height of one item
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getItemHeight()
    {
        checkWidget();
        return OS.Table_GetItemHeight(tableHandle);
    }

    /**
     * Returns an array of <code>TableItem</code>s which are the items
     * in the receiver.
     * <p>
     * Note: This is not the actual structure used by the receiver
     * to maintain its list of items, so modifying the array will
     * not affect the receiver.
     * </p>
     *
     * @return the items in the receiver
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public TableItem [] getItems()
    {
        checkWidget();
        return OS.Table_GetTableItems(tableHandle);
    }

    /**
     * Returns <code>true</code> if the receiver's lines are visible,
     * and <code>false</code> otherwise.
     * <p>
     * If one of the receiver's ancestors is not visible or some
     * other condition makes the receiver not visible, this method
     * may still indicate that it is considered visible even though
     * it may not actually be showing.
     * </p>
     *
     * @return the visibility state of the lines
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean getLinesVisible()
    {
        checkWidget();
        return OS.Table_GetLinesVisible(tableHandle);
    }

    /**
     * Returns an array of <code>TableItem</code>s that are currently
     * selected in the receiver. An empty array indicates that no
     * items are selected.
     * <p>
     * Note: This is not the actual structure used by the receiver
     * to maintain its selection, so modifying the array will
     * not affect the receiver.
     * </p>
     * @return an array representing the selection
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public TableItem [] getSelection()
    {
        checkWidget();
        return OS.Table_GetSelection(tableHandle);
    }

    /**
     * Returns the number of selected items contained in the receiver.
     *
     * @return the number of selected items
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getSelectionCount()
    {
        checkWidget();
        return OS.Table_GetSelectionCount(tableHandle);
    }

    /**
     * Returns the zero-relative index of the item which is currently
     * selected in the receiver, or -1 if no item is selected.
     *
     * @return the index of the selected item
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getSelectionIndex()
    {
        checkWidget();
        return OS.Table_GetSelectionIndex(tableHandle);
    }

    /**
     * Returns the zero-relative indices of the items which are currently
     * selected in the receiver.  The array is empty if no items are selected.
     * <p>
     * Note: This is not the actual structure used by the receiver
     * to maintain its selection, so modifying the array will
     * not affect the receiver.
     * </p>
     * @return the array of indices of the selected items
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int [] getSelectionIndices()
    {
        checkWidget();
        return OS.Table_GetSelectionIndices(tableHandle);
    }

    /**
     * Returns the zero-relative index of the item which is currently
     * at the top of the receiver. This index can change when items are
     * scrolled or new items are added or removed.
     *
     * @return the index of the top item
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getTopIndex()
    {
        checkWidget();
        return OS.Table_GetTopIndex(tableHandle);
    }

    /**
     * Searches the receiver's list starting at the first column
     * (index 0) until a column is found that is equal to the
     * argument, and returns the index of that column. If no column
     * is found, returns -1.
     *
     * @param column the search column
     * @return the index of the column
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int indexOf(TableColumn column)
    {
        checkWidget();
        if (column == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        return OS.Table_IndexOfColumn(tableHandle, column.handle);
    }

    /**
     * Searches the receiver's list starting at the first item
     * (index 0) until an item is found that is equal to the
     * argument, and returns the index of that item. If no item
     * is found, returns -1.
     *
     * @param item the search item
     * @return the index of the item
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int indexOf(TableItem item, int start)
    {
        checkWidget();
        if (item == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        return OS.Table_IndexOfItem(tableHandle,item.internal_getHandle(), start);
    }

    public int indexOf(TableItem item)
    {
        checkWidget();
        if (item == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        return OS.Table_IndexOfItem(tableHandle,item.internal_getHandle());
    }

    /**
     * Returns <code>true</code> if the item is selected,
     * and <code>false</code> otherwise.  Indices out of
     * range are ignored.
     *
     * @param index the index of the item
     * @return the visibility state of the item at the index
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean isSelected(int index)
    {
        checkWidget();
        return OS.Table_IsSelected(tableHandle, index);
    }

    /**
     * Removes the items from the receiver's list at the given
     * zero-relative indices.
     *
     * @param indices the array of indices of the items
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_RANGE - if the index is not between 0 and the number of elements in the list minus 1 (inclusive)</li>
     *    <li>ERROR_NULL_ARGUMENT - if the indices array is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void remove(int [] indices)
    {
        checkWidget();
        if (indices == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (indices.length == 0) return;
        int [] newIndices = new int [indices.length];
        System.arraycopy(indices, 0, newIndices, 0, indices.length);
        internal_sort(newIndices);
        TableItem[] items = getItems();
        OS.Table_Remove(tableHandle, newIndices);
        int oldindex =-1;
        for (int i=0; i<newIndices.length; i++)
        {
            int index = newIndices[i];
            if (index!=oldindex)
            {
                items[index].internal_release();
            }
            oldindex = index;
        }
    }

    /**
     * Removes the item from the receiver at the given
     * zero-relative index.
     *
     * @param index the index for the item
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_RANGE - if the index is not between 0 and the number of elements in the list minus 1 (inclusive)</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void remove(int index)
    {
        checkWidget();
        TableItem item = getItem(index);
        OS.Table_Remove(tableHandle, index);
        item.internal_release();
    }

    /**
     * Removes the items from the receiver which are
     * between the given zero-relative start and end
     * indices (inclusive).
     *
     * @param start the start of the range
     * @param end the end of the range
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_RANGE - if either the start or end are not between 0 and the number of elements in the list minus 1 (inclusive)</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void remove(int start, int end)
    {
        checkWidget();
        if (start > end) return;
        TableItem[] items = getItems();
        OS.Table_Remove(tableHandle, start, end);
        for (int i=start; i<=end; ++i)
        {
            items[i].internal_release();
        }
    }

    /**
     * Removes all of the items from the receiver.
     * <p>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void removeAll()
    {
        checkWidget();
        TableItem[] items = getItems();
        OS.Table_RemoveAll(tableHandle);
        for (int i=0; i<items.length; ++i)
        {
            if (items[i] != null)
            {
                items[i].internal_release();
            }
        }
    }

    /**
     * Removes the listener from the collection of listeners who will
     * be notified when the receiver's selection changes.
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
     * @see SelectionListener
     * @see #addSelectionListener(SelectionListener)
     */
    public void removeSelectionListener(SelectionListener listener)
    {
        removeListener(SWT.Selection, listener);
        removeListener(SWT.DefaultSelection, listener);
    }

    /**
     * Selects the items at the given zero-relative indices in the receiver.
     * The current selection is not cleared before the new items are selected.
     * <p>
     * If the item at a given index is not selected, it is selected.
     * If the item at a given index was already selected, it remains selected.
     * Indices that are out of range and duplicate indices are ignored.
     * If the receiver is single-select and multiple indices are specified,
     * then all indices are ignored.
     *
     * @param indices the array of indices for the items to select
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the array of indices is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Table#setSelection(int[])
     */
    public void select(int [] indices)
    {
        checkWidget();
        if (indices == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        int length = indices.length;
        if (length == 0 || ((style & SWT.SINGLE) != 0 && length > 1)) return;
        select(indices, false);
    }

    void select(int [] indices, boolean scroll)
    {
        int i = 0;
        while (i < indices.length)
        {
            int index = indices [i];
            if (index != -1)
            {
                select(index, false);
            }
            i++;
        }
        if (scroll) showSelection();
    }
    /**
     * Selects the item at the given zero-relative index in the receiver.
     * If the item at the index was already selected, it remains
     * selected. Indices that are out of range are ignored.
     *
     * @param index the index of the item to select
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void select(int index)
    {
        checkWidget();
        select(index, false);
    }

    void select(int index, boolean scroll)
    {
        if (index < 0) return;
        OS.Table_Select(tableHandle, index, scroll);
    }
    /**
     * Selects the items in the range specified by the given zero-relative
     * indices in the receiver. The range of indices is inclusive.
     * The current selection is not cleared before the new items are selected.
     * <p>
     * If an item in the given range is not selected, it is selected.
     * If an item in the given range was already selected, it remains selected.
     * Indices that are out of range are ignored and no items will be selected
     * if start is greater than end.
     * If the receiver is single-select and there is more than one item in the
     * given range, then all indices are ignored.
     *
     * @param start the start of the range
     * @param end the end of the range
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Table#setSelection(int,int)
     */
    public void select(int start, int end)
    {
        checkWidget();
        if (end < 0 || start > end || ((style & SWT.SINGLE) != 0 && start != end)) return;
        start = Math.max(0, start);
        if ((style & SWT.SINGLE) != 0)
        {
            select(start, false);
        }
        else
        {
            select(start, end, false);
        }
    }

    void select(int start, int end, boolean scroll)
    {
        if (start == end)
        {
            select(start, scroll);
            return;
        }
        OS.Table_Select(tableHandle, start, end);
        if (scroll) showSelection();
    }
    /**
     * Selects all of the items in the receiver.
     * <p>
     * If the receiver is single-select, do nothing.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */

    public void selectAll()
    {
        checkWidget();
        if ((style & SWT.SINGLE) != 0) return;
        OS.Table_SelectAll(tableHandle);
    }

    void setFocusIndex(int index)
    {
        if ((style & SWT.SINGLE) != 0)
        {
            // We consider that setting the focus in a single list means selecting this item.
            if (index < 0) return;
            OS.Table_Select(tableHandle, index, true);
        }
        else
        {
            OS.Table_SetFocusIndex(tableHandle, index);
        }
    }
    /**
     * Marks the receiver's header as visible if the argument is <code>true</code>,
     * and marks it invisible otherwise.
     * <p>
     * If one of the receiver's ancestors is not visible or some
     * other condition makes the receiver not visible, marking
     * it visible may not actually cause it to be displayed.
     * </p>
     *
     * @param show the new visibility state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setHeaderVisible(boolean show)
    {
        checkWidget();
        OS.Table_SetHeaderVisible(tableHandle, show);
    }

    /**
     * Marks the receiver's lines as visible if the argument is <code>true</code>,
     * and marks it invisible otherwise.
     * <p>
     * If one of the receiver's ancestors is not visible or some
     * other condition makes the receiver not visible, marking
     * it visible may not actually cause it to be displayed.
     * </p>
     *
     * @param show the new visibility state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setLinesVisible(boolean show)
    {
        checkWidget();
        OS.Table_SetLinesVisible(tableHandle, show);
    }

    /**
     * Selects the items at the given zero-relative indices in the receiver.
     * The current selection is cleared before the new items are selected.
     * <p>
     * Indices that are out of range and duplicate indices are ignored.
     * If the receiver is single-select and multiple indices are specified,
     * then all indices are ignored.
     *
     * @param indices the indices of the items to select
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the array of indices is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Table#deselectAll()
     * @see Table#select(int[])
     */
    public void setSelection(int [] indices)
    {
        checkWidget();
        if (indices == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        deselectAll();
        int length = indices.length;
        if (length == 0 || ((style & SWT.SINGLE) != 0 && length > 1)) return;
        select(indices, true);
        if ((style & SWT.MULTI) != 0)
        {
            int focusIndex = indices [0];
            if (focusIndex >= 0) setFocusIndex(focusIndex);
        }
    }

    /**
     * Sets the receiver's selection to be the given array of items.
     * The current selection is cleared before the new items are selected.
     * <p>
     * Items that are not in the receiver are ignored.
     * If the receiver is single-select and multiple items are specified,
     * then all items are ignored.
     *
     * @param items the array of items
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the array of items is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if one of the items has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Table#deselectAll()
     * @see Table#select(int[])
     * @see Table#setSelection(int[])
     */
    public void setSelection(TableItem [] items)
    {
        checkWidget();
        if (items == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        int length = items.length;
        deselectAll();
        if (length == 0 || ((style & SWT.SINGLE) != 0 && length > 1)) return;
        int focusIndex = -1;
        for (int i=length-1; i>=0; --i)
        {
            TableItem item = items [i];
            int index = 0;
            if (item != null)
            {
                int localFocus = -1;
                while ((index = indexOf(item, index)) != -1)
                {
                    if (localFocus == -1) localFocus = index;
                    select(index, false);
                    if ((style & SWT.SINGLE) != 0)
                    {
                        showSelection();
                        return;
                    }
                    index++;
                }
                if (localFocus != -1) focusIndex = localFocus;
            }
        }
        if ((style & SWT.MULTI) != 0)
        {
            if (focusIndex >= 0) setFocusIndex(focusIndex);
        }
    }

    /**
     * Selects the item at the given zero-relative index in the receiver.
     * The current selected is first cleared, then the new item is selected.
     *
     * @param index the index of the item to select
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Table#deselectAll()
     * @see Table#select(int)
     */
    public void setSelection(int index)
    {
        checkWidget();
        deselectAll();
        select(index, true);
        if ((style & SWT.MULTI) != 0)
        {
            if (index >= 0) setFocusIndex(index);
        }
    }
    /**
     * Selects the items in the range specified by the given zero-relative
     * indices in the receiver. The range of indices is inclusive.
     * The current selection is cleared before the new items are selected.
     * <p>
     * Indices that are out of range are ignored and no items will be selected
     * if start is greater than end.
     * If the receiver is single-select and there is more than one item in the
     * given range, then all indices are ignored.
     *
     * @param start the start index of the items to select
     * @param end the end index of the items to select
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Table#deselectAll()
     * @see Table#select(int,int)
     */
    public void setSelection(int start, int end)
    {
        checkWidget();
        deselectAll();
        if (end < 0 || start > end || ((style & SWT.SINGLE) != 0 && start != end)) return;
        start = Math.max(0, start);
        if ((style & SWT.SINGLE) != 0)
        {
            select(start, true);
        }
        else
        {
            select(start, end, true);
            setFocusIndex(start);
        }
    }
    /**
     * Sets the zero-relative index of the item which is currently
     * at the top of the receiver. This index can change when items
     * are scrolled or new items are added and removed.
     *
     * @param index the index of the top item
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setTopIndex(int index)
    {
        checkWidget();
        if (index < 0) return;
        OS.Table_SetTopIndex(tableHandle, index);
    }

    /**
     * Shows the column.  If the column is already showing in the receiver,
     * this method simply returns.  Otherwise, the columns are scrolled until
     * the column is visible.
     *
     * @param column the column to be shown
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the item is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if the item has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 3.0
     */
    public void showColumn(TableColumn column)
    {
        checkWidget();
        if (column==null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (column.isDisposed()) internal_error(SWT.ERROR_INVALID_ARGUMENT);
        OS.Table_ShowColumn(tableHandle, column.handle);
    }

    /**
     * Shows the item.  If the item is already showing in the receiver,
     * this method simply returns.  Otherwise, the items are scrolled until
     * the item is visible.
     *
     * @param item the item to be shown
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the item is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if the item has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Table#showSelection()
     */
    public void showItem(TableItem item)
    {
        checkWidget();
        if (item==null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (item.isDisposed()) internal_error(SWT.ERROR_INVALID_ARGUMENT);
        OS.Table_ShowItem(tableHandle, item.internal_getHandle());
    }

    /**
     * Shows the selection.  If the selection is already showing in the receiver,
     * this method simply returns.  Otherwise, the items are scrolled until
     * the selection is visible.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Table#showItem(TableItem)
     */
    public void showSelection()
    {
        checkWidget();
        OS.Table_ShowSelection(tableHandle);
    }

    /**
     *  This is called by TableItem.dipose() when called explicitly
     */
    void internal_removeItem(int index)
    {
        OS.Table_Remove(tableHandle, index);
    }

    void columnAdded(int index)
    {
        ++columnCount;
        TableItem[] items = getItems();
        for (int i=0; i<items.length; ++i)
        {
            items[i].columnAdded(index);
        }
    }

    void columnRemoved(TableColumn column)
    {
        --columnCount;
        int index = indexOf(column);
        TableItem[] items = getItems();
        for (int i=0; i<items.length; ++i)
        {
            items[i].columnRemoved(index);
        }
    }

}
