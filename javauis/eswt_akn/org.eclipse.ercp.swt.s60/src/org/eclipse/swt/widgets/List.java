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

/**
 * Instances of this class represent a selectable user interface
 * object that displays a list of strings and issues notificiation
 * when a string selected.  A list may be single or multi select.
 * <p>
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>SINGLE, MULTI</dd>
 * <dt><b>Events:</b></dt>
 * <dd>Selection, DefaultSelection</dd>
 * </dl>
 * <p>
 * Note: Only one of SINGLE and MULTI may be specified.
 * </p><p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */

public class List extends Scrollable
{
    private int listHandle;

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
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public List(Composite parent, int style)
    {
        super(parent, checkStyle(style));
    }

    final void setListHandle(int listHandle)
    {
        this.listHandle = listHandle;
        internal_setScrollableHandle(OS.List_ScrollableHandle(listHandle));
    }

    protected void internal_createHandle()
    {
        setListHandle(OS.List_New(this, parent.compositeHandle, style));
    }

    /**
     * Adds the argument to the end of the receiver's list.
     *
     * @param string the new item
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #add(String,int)
     */
    public void add(String string)
    {
        checkWidget();
        if (string == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        OS.List_Add(listHandle, string);
    }
    /**
     * Adds the argument to the receiver's list at the given
     * zero-relative index.
     * <p>
     * Note: To add an item at the end of the list, use the
     * result of calling <code>getItemCount()</code> as the
     * index or use <code>add(String)</code>.
     * </p>
     *
     * @param string the new item
     * @param index the index for the item
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     *    <li>ERROR_INVALID_RANGE - if the index is not between 0 and the number of elements in the list (inclusive)</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #add(String)
     */
    public void add(String string, int index)
    {
        checkWidget();
        if (string == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        OS.List_Add(listHandle, string, index);
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notified when the receiver's selection changes, by sending
     * it one of the messages defined in the <code>SelectionListener</code>
     * interface.
     * <p>
     * <code>widgetSelected</code> is called when the selection changes.
     * <code>widgetDefaultSelected</code> is typically called when an item is double-clicked.
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
        addListener(SWT.Selection,typedListener);
        addListener(SWT.DefaultSelection,typedListener);
    }


    public Point computeSize(int wHint, int hHint, boolean changed)
    {
        checkWidget();
        return OS.Control_ComputeSize(handle, wHint, hHint);
    }


    static int checkStyle(int style)
    {
        return internal_checkBits(style, SWT.SINGLE, SWT.MULTI, 0, 0, 0, 0);
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
        if ((style & SWT.SINGLE) != 0) return;
        OS.List_Deselect(listHandle, indices);
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
        checkWidget();
        if ((style & SWT.SINGLE) != 0) return;
        OS.List_Deselect(listHandle, index);
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
        if ((style & SWT.SINGLE) != 0) return;
        if (start > end) return;
        if (end < 0) return;
        OS.List_Deselect(listHandle, start, end);
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
        checkWidget();
        if ((style & SWT.SINGLE) != 0) return;
        OS.List_DeselectAll(listHandle);
    }

    /**
     * Returns the zero-relative index of the item which currently
     * has the focus in the receiver, or -1 if no item has focus.
     *
     * @return the index of the selected item
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getFocusIndex()
    {
        checkWidget();
        int result = OS.List_GetFocusIndex(listHandle);
        return result;
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
    public String getItem(int index)
    {
        checkWidget();
        String item = OS.List_GetItem(listHandle, index);
        if (item != null)
        {
            return item;
        }
        else
        {
            return "";
        }
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
        int result = OS.List_GetItemCount(listHandle);
        return result;
    }

    /**
     * Returns the height of the area which would be used to
     * display <em>one</em> of the items in the tree.
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
        int result = OS.List_GetItemHeight(listHandle);
        return result;
    }

    /**
     * Returns an array of <code>String</code>s which are the items
     * in the receiver.
     * <p>
     * Note: This is not the actual structure used by the receiver
     * to maintain its list of items, so modifying the array will
     * not affect the receiver.
     * </p>
     *
     * @return the items in the receiver's list
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public String [] getItems()
    {
        checkWidget();
        int count = getItemCount();
        String [] result = new String [count];
        for (int i=0; i<count; i++) result [i] = getItem(i);
        return result;
    }

    /**
     * Returns an array of <code>String</code>s that are currently
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
    public String [] getSelection()
    {
        checkWidget();
        int [] indices = getSelectionIndices();
        String [] result = new String [indices.length];
        for (int i=0; i<indices.length; i++)
        {
            result [i] = getItem(indices [i]);
        }
        return result;
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
        return OS.List_GetSelectionCount(listHandle);
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
        return OS.List_GetSelectionIndex(listHandle);
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
        return OS.List_GetSelectionIndices(listHandle);
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
        return OS.List_GetTopIndex(listHandle);
    }

    /**
     * Gets the index of an item.
     * <p>
     * The list is searched starting at 0 until an
     * item is found that is equal to the search item.
     * If no item is found, -1 is returned.  Indexing
     * is zero based.
     *
     * @param string the search item
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
    public int indexOf(String string)
    {
        return indexOf(string, 0);
    }

    /**
     * Searches the receiver's list starting at the given,
     * zero-relative index until an item is found that is equal
     * to the argument, and returns the index of that item. If
     * no item is found or the starting index is out of range,
     * returns -1.
     *
     * @param string the search item
     * @param start the zero-relative index at which to start the search
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
    public int indexOf(String string, int start)
    {
        checkWidget();
        if (string == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        return OS.List_IndexOf(listHandle, string, start);
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
        return OS.List_IsSelected(listHandle, index);
    }

    /**
     * Removes the items from the receiver at the given
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
        OS.List_Remove(listHandle, newIndices);
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
        OS.List_Remove(listHandle, index);
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
        OS.List_Remove(listHandle, start, end);
    }

    /**
     * Searches the receiver's list starting at the first item
     * until an item is found that is equal to the argument,
     * and removes that item from the list.
     *
     * @param string the item to remove
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if the string is not found in the list</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void remove(String string)
    {
        checkWidget();
        if (string == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        int index = indexOf(string, 0);
        if (index == -1) internal_error(SWT.ERROR_INVALID_ARGUMENT);
        remove(index);
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
        OS.List_RemoveAll(listHandle);
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
     * @see #addSelectionListener
     */
    public void removeSelectionListener(SelectionListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook(SWT.Selection, listener);
        eventTable.unhook(SWT.DefaultSelection,listener);
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
     * @see List#setSelection(int[])
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
     * Selects the item at the given zero-relative index in the receiver's
     * list.  If the item at the index was already selected, it remains
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
        OS.List_Select(listHandle, index, scroll);
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
     * @see List#setSelection(int,int)
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
        OS.List_Select(listHandle, start, end);
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
        OS.List_SelectAll(listHandle);
    }

    void setFocusIndex(int index)
    {
        if ((style & SWT.SINGLE) != 0)
        {
            // We consider that setting the focus in a single list means selecting this item.
            if (index < 0) return;
            OS.List_Select(listHandle, index, true);
        }
        else
        {
            OS.List_SetFocusIndex(listHandle, index);
        }
    }

    public void setFont(Font font)
    {
        checkWidget();
        super.setFont(font);
    }

    /**
     * Sets the text of the item in the receiver's list at the given
     * zero-relative index to the string argument. This is equivalent
     * to <code>remove</code>'ing the old item at the index, and then
     * <code>add</code>'ing the new item at that index.
     *
     * @param index the index for the item
     * @param string the new text for the item
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_RANGE - if the index is not between 0 and the number of elements in the list minus 1 (inclusive)</li>
     *    <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setItem(int index, String string)
    {
        checkWidget();
        if (string == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        int focusedIndex = getFocusIndex();
        int topIndex = getTopIndex();
        boolean isSelected = isSelected(index);
        remove(index);
        add(string, index);
        if (isSelected) select(index, false);
        if (focusedIndex != getFocusIndex())
            setFocusIndex(focusedIndex);
        setTopIndex(topIndex);
    }

    /**
     * Sets the receiver's items to be the given array of items.
     *
     * @param items the array of items
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the items array is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setItems(String [] items)
    {
        checkWidget();
        if (items == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        OS.List_SetItems(listHandle, items);
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
     * @see List#deselectAll()
     * @see List#select(int[])
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
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see List#deselectAll()
     * @see List#select(int[])
     * @see List#setSelection(int[])
     */
    public void setSelection(String [] items)
    {
        checkWidget();
        if (items == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        deselectAll();
        int length = items.length;
        if (length == 0 || ((style & SWT.SINGLE) != 0 && length > 1)) return;
        int focusIndex = -1;
        for (int i=length-1; i>=0; --i)
        {
            String string = items [i];
            int index = 0;
            if (string != null)
            {
                int localFocus = -1;
                while ((index = indexOf(string, index)) != -1)
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
     * If the item at the index was already selected, it remains selected.
     * The current selected is first cleared, then the new items are selected.
     * Indices that are out of range are ignored.
     *
     * @param index the index of the item to select
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     * @see List#deselectAll()
     * @see List#select(int)
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
     * @see List#deselectAll()
     * @see List#select(int,int)
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
        OS.List_SetTopIndex(listHandle, index);
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
     */
    public void showSelection()
    {
        checkWidget();
        OS.List_ShowSelection(listHandle);
    }


    private final void sendSelectionEvent()
    {
        Event event = new Event();
        internal_sendEvent(SWT.Selection, event);
    }

}
