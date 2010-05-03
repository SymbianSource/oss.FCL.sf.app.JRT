/*******************************************************************************
 * Copyright (c) 2004 IBM Corp.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Mark Rogalski (IBM Corp.) - initial API specification
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/

package org.eclipse.ercp.swt.mobile;

import org.eclipse.ercp.swt.mobile.internal.OS;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Scrollable;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.TypedListener;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Point;

/**
 * Instances of this class represent a selectable user interface object that
 * displays a sorted list of text items. The items may be displayed in ascending
 * or descending order.
 * <p>
 * If the FILTER style is specified during construction, an assoicated
 * label is also displayed showing characters entered which are then used
 * to filter the list to show fewer items. The selection state of items filtered
 * out of the list is cleared.
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
public class SortedList extends Scrollable
{
    private int sortedListHandle;
    private int modeStyle;


    /**
     * constructor style specifying filter field should be displayed
     */
    public static final int FILTER = 1;

    /**
     * Construct a new instance of this class given its parent and a style value
     * describing its behavior and appearance.
     * <p>
     * The <code>style</code> value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or
     * must be built by <em>bitwise OR</em> 'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. Style bits are also inherited from superclasses.
     * <code>SWT.UP</code> (the default) means the list is sorted so that numbers go from
     * low to high when the list is examined from top to bottom. <code>SWT.DOWN</code>
     * means the list is sorted so that numbers go from high to low when
     * examined from top to bottom.
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
    public SortedList(Composite parent, int style)
    {
        super(parent, checkStyle(style));
        checkSubclass();
    }


    /**
     * Construct a new instance of this class given its parent, a style value
     * describing its behavior and appearance, and a mode style describing
     * additional behavior modes.
     * <p>
     * The <code>style</code> value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or
     * must be built by <em>bitwise OR</em> 'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. Style bits are also inherited from superclasses.
     * <code>SWT.UP</code> (the default) means the list is sorted so that numbers go from
     * low to high when the list is examined from top to bottom. <code>SWT.DOWN</code>
     * means the list is sorted so that numbers go from high to low when
     * examined from top to bottom.
     *
     * The <code>modeStyle</code> may specify the class constant <code>FILTER</code>.
     * This style adds a text entry field to
     * the widget which does not receive focus, but will display characters
     * entered while the list has focus. These characters are used to filter the
     * list items so that fewer items are displayed in the list. The selection is
     * cleared for any items not shown as a result of filtering.
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
    public SortedList(Composite parent, int style, int modeStyle)
    {
        super(parent, checkStyle(style));
        try
        {
            if (modeStyle != FILTER)
            {
                this.modeStyle = 0;
            }
            else
            {
                this.modeStyle = FILTER;
            }

            OS.SortedList_SetModeStyle(sortedListHandle, modeStyle);
        }
        catch (Error e)
        {
            if (handle != 0) org.eclipse.swt.internal.symbian.OS.Control_Dispose(handle);
            throw e;
        }
    }

    static int checkStyle(int style)
    {
        style = checkStyleSingleOrMulti(style);
        style = checkStyleUpOrDown(style);
        return style;
    }
    static int checkStyleSingleOrMulti(int style)
    {
        return internal_checkBits(style, SWT.SINGLE, SWT.MULTI, 0, 0, 0, 0);
    }

    static int checkStyleUpOrDown(int style)
    {
        return internal_checkBits(style, SWT.UP, SWT.DOWN, 0, 0, 0, 0);
    }

    final void setSortedHandle(int sortedListHandle)
    {
        this.sortedListHandle = sortedListHandle;
        internal_setScrollableHandle(OS.SortedList_ScrollableHandle(sortedListHandle));
    }

    protected void internal_createHandle()
    {
        setSortedHandle(OS.SortedList_New(this, getParent().internal_getCompositeHandle(), internal_getStyle()));
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
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
    public void add(String item)
    {
        checkWidget();
        if (item == null) internal_error(SWT.ERROR_NULL_ARGUMENT);

        try
        {
            OS.SortedList_Add(sortedListHandle, item);
        }
        catch (Throwable e)
        {
            internal_error(SWT.ERROR_ITEM_NOT_ADDED);
        }
    }

    /**
     * Adds the listener to the collection of listeners who will be notified
     * when the receiver's selection changes, by sending it one of the messages
     * defined in the SelectionListener interface.
     * <p>
     * <code>widgetSelected</code> is called when the selection changes. <br>
     * <code>widgetDefaultSelected</code> is typically called when selection
     * is finalized.
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
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
    public void addSelectionListener(SelectionListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Selection,typedListener);
        addListener(SWT.DefaultSelection,typedListener);
    }

    /**
     * Returns the prefered size of the receiver.
     * <p>
     * <b>Description copied from class: <code>Control</code> </b>
     * <p>
     * The preferred size of a control is the size that it would be best
     * displayed at. The width hint and height hint arguments allow the caller
     * to ask a control questions such as "Given a particular width, how high
     * does the control need to be to show all of the contents?" To indicate
     * that the caller does not wish to constrain a particular dimension, the
     * constant SWT.DEFAULT is passed for the hint.
     * <p>
     * If the changed flag is true, it indicates that the receiver's contents
     * have changed, therefore any caches that a layout manager containing the
     * control may have been keeping need to be flushed. When the control is
     * resized, the changed flag will be <code>false</code>, so layout
     * manager caches can be retained.
     * <p>
     * <b>Overrides: </b> <code>computeSize</code> in class <b>
     * <code>Control</code> </b>
     *
     * @param wHint
     *            the width hint (can be SWT.DEFAULT)
     * @param hHint
     *            the height hint (can be SWT.DEFAULT)
     * @param changed
     *            true if the control's contents have changed, and false
     *            otherwise
     *
     * @return the preferred size of the control
     */
    public Point computeSize(int wHint, int hHint, boolean changed)
    {
        checkWidget();
        return org.eclipse.swt.internal.symbian.OS.Control_ComputeSize(handle, wHint, hHint);
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     */
    public String getFocus()
    {
        checkWidget();
        String result = OS.SortedList_GetFocusedItem(sortedListHandle);
        return result;
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     * @exception SWTError
     *                <ul>
     *                <li>ERROR_CANNOT_GET_HEIGHT - if the operation fails
     *                because of an operating system failure</li>
     *                </ul>
     */
    public int getItemHeight()
    {
        checkWidget();
        int height=0;
        try
        {
            height = OS.SortedList_GetItemHeight(sortedListHandle);
        }
        catch (Throwable e)
        {
            SWT.error(SWT.ERROR_CANNOT_GET_ITEM_HEIGHT);
        }
        return height;
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     * @exception SWTError
     *                <ul>
     *                <li>ERROR_CANNOT_GET_SELECTION - if the operation fails
     *                because of an operating system failure</li>
     *                </ul>
     *
     * @see SortedList#getSelectionCount
     */
    public String[] getSelection()
    {
        checkWidget();
        String[] result=null;

        try
        {
            result = OS.SortedList_GetSelection(sortedListHandle);
        }
        catch (Throwable e)
        {
            SWT.error(SWT.ERROR_CANNOT_GET_SELECTION);
        }

        return result;
    }

    /**
     * Returns the number of items currently selected.
     *
     * @return count of selected items
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     * @exception SWTError
     *                <ul>
     *                <li>ERROR_CANNOT_GET_COUNT - if the operation fails
     *                because of an operating system failure</li>
     *                </ul>
     *
     * @see SortedList#getSelection
     */
    public int getSelectionCount()
    {
        checkWidget();
        int result=0;
        try
        {
            result = OS.SortedList_GetSelectionCount(sortedListHandle);
        }
        catch (Throwable e)
        {
            SWT.error(SWT.ERROR_CANNOT_GET_COUNT);
        }
        return result;
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
     *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     *                <li>ERROR_INVALID_ARGUMENT - if the string is not found
     *                in the list</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
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
    public void remove(String item)
    {
        checkWidget();
        if (item == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        boolean result = true;
        try
        {
            result = OS.SortedList_Remove(sortedListHandle, item);
        }
        catch (Throwable e)
        {
            SWT.error(SWT.ERROR_ITEM_NOT_REMOVED);
        }
        if (result == true)
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }

    /**
     * Removes all of the items from the receiver.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
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
    public void removeAll()
    {
        checkWidget();
        try
        {
            OS.SortedList_RemoveAll(sortedListHandle);
        }
        catch (Throwable e)
        {
            SWT.error(SWT.ERROR_ITEM_NOT_REMOVED);
        }
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
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
    public void removeSelectionListener(SelectionListener listener)
    {
        removeListener(SWT.Selection,        listener);
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
     *                <li>ERROR_INVALID_ARGUMENT - if the string is not found
     *                in the list</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     */
    public void select(String item)
    {
        checkWidget();
        if (OS.SortedList_Select(sortedListHandle, item))
        {
            if (item != null) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     * @exception SWTError
     *                <ul>
     *                <li>ERROR_ITEM_NOT_ADDED - if the operation fails because
     *                of an operating system failure</li>
     *                </ul>
     *
     * @see SortedList#add
     */
    public void setItems(String[] items)
    {
        checkWidget();
        if (items==null)
        {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }
        try
        {
            OS.SortedList_SetItems(sortedListHandle, items);
        }
        catch (Throwable e)
        {
            SWT.error(SWT.ERROR_ITEM_NOT_ADDED);
        }
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     *
     * @see SortedList#select
     */
    public void setSelection(String[] items)
    {
        checkWidget();
        if (items==null)
        {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }
        OS.SortedList_SetSelection(sortedListHandle, items);
    }

    /**
     * Scrolls the list as necessary to show the selected items.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     *
     * @see SortedList#select
     * @see SortedList#setSelection
     */
    public void showSelection()
    {
        checkWidget();
        OS.SortedList_ShowSelection(sortedListHandle);
    }
}

