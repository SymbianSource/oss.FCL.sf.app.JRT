/*******************************************************************************
 * Copyright (c) 2004, 2007 Nokia Corporation.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Yu You (Nokia Corp.)- initial API specification
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.ercp.swt.mobile;

import org.eclipse.ercp.swt.mobile.internal.OS;
import org.eclipse.swt.*;
import org.eclipse.swt.events.*;
import org.eclipse.swt.graphics.*;
import org.eclipse.swt.widgets.*;
import java.util.*;

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
 * <code>setLayoutDensity(int)</code>. Some platforms may use
 * context-sensitive device keys to allow the user to change the ListView's
 * density level at run-time. For example, by activating "zoom in/out" device
 * keys when the ListView is focused.
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
 * <code>ScrollBar.setSelection(int)</code>) will affect the scrolling
 * position of the ListView control. However, changing the range of scrollbars,
 * such as <code>setMaximum(int)</code> and <code>setMinimum(int)</code>,
 * will not affect the ListView at all.
 * </p>
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */
public class ListView extends Scrollable
{
    private int listViewHandle;
    Vector icons = new Vector();
    int density;

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
    public ListView(Composite parent, int style)
    {
        super(parent, checkStyle(style));
        density = MEDIUM;
    }

    /**
     * Constructs a new instance of this class given its parent and a style
     * value describing its behavior and appearance styles.
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or
     * must be built by <em>bitwise OR</em> 'ing together (that is, using the
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
     *                <li>ERROR_INVALID_ARGUMENT - if the density is invalid
     *                </li>
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
    public ListView(Composite parent, int style, int density)
    {
        super(parent, checkStyle(style));
        if (density != LOW && density != MEDIUM && density != HIGH)
        {
            internal_error(SWT.ERROR_INVALID_ARGUMENT);
        }
        this.density = density;
        OS.ListView_SetLayoutDensity(listViewHandle, density);
    }

    final void setListViewHandle(int listViewHandle)
    {
        this.listViewHandle = listViewHandle;
        internal_setScrollableHandle(OS.ListView_ScrollableHandle(listViewHandle));
    }

    protected void internal_createHandle()
    {
        setListViewHandle(OS.ListView_New(this, getParent().internal_getCompositeHandle(), internal_getStyle()));
    }

    static int checkStyle(int style)
    {
        // VERTICAL and V_SCROLL have same values. Same goes for HORIZONTAL
        // and H_SCROLL. Vertical layout corresponds to horizontal scrollbar
        // and the other way arround. Since Scrollable needs the scrollbar
        // bits, we have to replace the layout with scrollbar bits.
        boolean isLayoutHorizontal = ((style & SWT.HORIZONTAL) != 0);
        style &= ~SWT.VERTICAL;
        style &= ~SWT.HORIZONTAL;
        if (isLayoutHorizontal)
        {
            style |= SWT.V_SCROLL;
        }
        else
        {
            style |= SWT.H_SCROLL;
        }
        return internal_checkBits(style, SWT.SINGLE, SWT.MULTI, 0, 0, 0, 0);
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
    public int getLayoutDensity()
    {
        checkWidget();
        return density;
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
    public int getLayout()
    {
        checkWidget();
        // See checkStyle() for the explanation of the following lines
        if ((getStyle() & SWT.V_SCROLL) != 0)
        {
            return SWT.HORIZONTAL;
        }
        else
        {
            return SWT.VERTICAL;
        }
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
    public void add(String item, Image icon)
    {
        checkWidget();
        if (item == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        OS.ListView_Add(listViewHandle, item, (icon == null)? 0 : icon.handle);
        icons.addElement(icon);
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
    public void add(String string, Image icon, int index)
    {
        checkWidget();
        if (string == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        OS.ListView_Add(listViewHandle, string, (icon == null) ? 0 : icon.handle, index);
        icons.insertElementAt(icon, index);
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
    public void addSelectionListener(SelectionListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Selection,typedListener);
        addListener(SWT.DefaultSelection,typedListener);
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
    public void deselect(int index)
    {
        checkWidget();
        OS.ListView_Deselect(listViewHandle, index);
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
    public void deselect(int start, int end)
    {
        checkWidget();
        if (start > end) return;
        if (end < 0) return;
        OS.ListView_Deselect(listViewHandle, start, end);
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
     *                <li>ERROR_NULL_ARGUMENT - if the set of indices is null
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
    public void deselect(int[] indices)
    {
        checkWidget();
        if (indices == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (indices.length == 0) return;
        OS.ListView_Deselect(listViewHandle, indices);
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
    public void deselectAll()
    {
        checkWidget();
        if ((internal_getStyle() & SWT.MULTI) == 0)
        {
            return;
        }
        OS.ListView_DeselectAll(listViewHandle);
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
    public int getFocusIndex()
    {
        checkWidget();
        return OS.ListView_GetFocusIndex(listViewHandle);
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
     *                <li>ERROR_CANNOT_GET_ITEM - if the operation fails
     *                because of an operating system failure</li>
     *                </ul>
     */
    public String getItem(int index)
    {
        checkWidget();
        String item = OS.ListView_GetItem(listViewHandle, index);
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
     *                <li>ERROR_CANNOT_GET_ITEM - if the operation fails
     *                because of an operating system failure</li>
     *                </ul>
     */
    public Image getIcon(int index)
    {
        checkWidget();
        if (index < 0 || index >= OS.ListView_GetItemCount(listViewHandle))
        {
            internal_error(SWT.ERROR_INVALID_RANGE);
        }
        if (index >= icons.size())
        {
            internal_error(SWT.ERROR_CANNOT_GET_ITEM);
        }
        return (Image)icons.elementAt(index);
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
     *                <li>ERROR_INVALID_ARGUMENT - if the style value is
     *                invalid</li>
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
    public Point getIconSize(int style)
    {
        checkWidget();
        if (style != LOW && style != MEDIUM && style != HIGH)
        {
            internal_error(SWT.ERROR_INVALID_ARGUMENT);
        }
        return OS.ListView_GetIconSize(listViewHandle, style);
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
    public int getItemCount()
    {
        checkWidget();
        return OS.ListView_GetItemCount(listViewHandle);
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
     *                <li>ERROR_CANNOT_GET_ITEM - if the operation fails
     *                because of an operating system failure while getting an
     *                item</li>
     *                <li>ERROR_CANNOT_GET_COUNT - if the operation fails
     *                because of an operating system failure while getting the
     *                item count</li>
     *                </ul>
     */
    public String[] getItems()
    {
        checkWidget();
        String[] result = OS.ListView_GetItems(listViewHandle);
        if (result == null)
        {
            result = new String[0];
        }
        return result;
    }

    /**
     * Returns an array of <code>String</code> s that are currently selected
     * in the receiver. An empty array indicates that no items are selected.
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
     *                <li>ERROR_CANNOT_GET_ITEM - if the operation fails
     *                because of an operating system failure while getting an
     *                item</li>
     *                </ul>
     */
    public String[] getSelection()
    {
        checkWidget();
        String[] result = OS.ListView_GetSelection(listViewHandle);
        if (result == null)
        {
            result = new String[0];
        }
        return result;
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
    public int getSelectionCount()
    {
        checkWidget();
        return OS.ListView_GetSelectionCount(listViewHandle);
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
    public int[] getSelectionIndices()
    {
        checkWidget();
        return OS.ListView_GetSelectionIndices(listViewHandle);
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
    public int getTopIndex()
    {
        checkWidget();
        return OS.ListView_GetTopIndex(listViewHandle);
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public int indexOf(String string)
    {
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * @exception SWTError
     *                <ul>
     *                <li>ERROR_CANNOT_GET_COUNT - if the operation fails
     *                because of an operating system failure while getting the
     *                item count</li>
     *                <li>ERROR_CANNOT_GET_ITEM - if the operation fails
     *                because of an operating system failure while getting an
     *                item</li>
     *                </ul>
     */
    public int indexOf(String string, int start)
    {
        checkWidget();
        if (string == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        return OS.ListView_IndexOf(listViewHandle, string, start);
    }

    /**
     * Returns <code>true</code> if the item is selected, and
     * <code>false</code> otherwise. Indices out of range are ignored.
     *
     * @param index
     *            the index of the item
     * @return the visibility state of the item at the index
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public boolean isSelected(int index)
    {
        checkWidget();
        return OS.ListView_IsSelected(listViewHandle, index);
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * @exception SWTError
     *                <ul>
     *                <li>ERROR_ITEM_NOT_REMOVED - if the operation fails
     *                because of an operating system failure</li>
     *                </ul>
     */
    public void remove(int index)
    {
        checkWidget();
        OS.ListView_Remove(listViewHandle, index);
        icons.removeElementAt(index);
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * @exception SWTError
     *                <ul>
     *                <li>ERROR_ITEM_NOT_REMOVED - if the operation fails
     *                because of an operating system failure</li>
     *                </ul>
     */
    public void remove(int start, int end)
    {
        checkWidget();
        if (start > end) return;
        OS.ListView_Remove(listViewHandle, start, end);
        for (int i = 0; i < start - end + 1; i++)
        {
            icons.removeElementAt(start);
        }
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
     *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     *                <li>ERROR_INVALID_ARGUMENT - if the string is not found
     *                in the list</li>
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
     *                <li>ERROR_ITEM_NOT_REMOVED - if the operation fails
     *                because of an operating system failure</li>
     *                </ul>
     */
    public void remove(String string)
    {
        if (string == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        int index = indexOf(string, 0);
        if (index == -1) internal_error(SWT.ERROR_INVALID_ARGUMENT);
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
     *                </li>
     *                <li>ERROR_NULL_ARGUMENT - if the indices array is null
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
     *                <li>ERROR_ITEM_NOT_REMOVED - if the operation fails
     *                because of an operating system failure</li>
     *                </ul>
     */
    public void remove(int[] indices)
    {
        checkWidget();
        if (indices == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (indices.length == 0) return;
        int [] newIndices = new int [indices.length];
        System.arraycopy(indices, 0, newIndices, 0, indices.length);
        internal_sort(newIndices);
        OS.ListView_Remove(listViewHandle, newIndices);
        // Array is ordered from high to low
        int prevIndex = -1;
        for (int i = 0; i < newIndices.length; i++)
        {
            // Avoid trying to remove the same item repeateadly
            if (prevIndex != newIndices[i])
            {
                icons.removeElementAt(newIndices[i]);
            }
            prevIndex = newIndices[i];
        }
    }

    /**
     * Removes all of the items from the receiver.
     * <p>
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void removeAll()
    {
        checkWidget();
        OS.ListView_RemoveAll(listViewHandle);
        icons.removeAllElements();
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *
     * @see SelectionListener
     * @see #addSelectionListener
     */
    public void removeSelectionListener(SelectionListener listener)
    {
        removeListener(SWT.Selection, listener);
        removeListener(SWT.DefaultSelection,listener);
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void select(int index)
    {
        checkWidget();
        checkWidget();
        select(index, false);
    }

    void select(int index, boolean scroll)
    {
        if (index < 0) return;
        OS.ListView_Select(listViewHandle, index, scroll);
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *
     * @see List#setSelection(int,int)
     */
    public void select(int start, int end)
    {
        checkWidget();
        if (end < 0 || start > end || ((internal_getStyle() & SWT.SINGLE) != 0 && start != end)) return;
        start = Math.max(0, start);
        if ((internal_getStyle() & SWT.SINGLE) != 0)
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
        OS.ListView_Select(listViewHandle, start, end);
        if (scroll) showSelection();
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *
     * @see List#setSelection(int[])
     */
    public void select(int[] indices)
    {
        checkWidget();
        if (indices == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        int length = indices.length;
        if (length == 0 || ((internal_getStyle() & SWT.SINGLE) != 0 && length > 1)) return;
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
     * Selects all of the items in the receiver.
     * <p>
     * If the receiver is single-select, do nothing.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void selectAll()
    {
        checkWidget();
        if ((internal_getStyle() & SWT.SINGLE) != 0) return;
        OS.ListView_SelectAll(listViewHandle);
    }

    void setFocusIndex(int index)
    {
        if ((internal_getStyle() & SWT.SINGLE) != 0)
        {
            // We consider that setting the focus in a single list means selecting this item.
            if (index < 0) return;
            OS.ListView_Select(listViewHandle, index, true);
        }
        else
        {
            OS.ListView_SetFocusIndex(listViewHandle, index);
        }
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
     *                </li>
     *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
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
     *                <li>ERROR_ITEM_NOT_REMOVED - if the remove operation
     *                fails because of an operating system failure</li>
     *                <li>ERROR_ITEM_NOT_ADDED - if the add operation fails
     *                because of an operating system failure</li>
     *                </ul>
     */
    public void setItem(int index, String string, Image icon)
    {
        checkWidget();
        if (string == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        boolean isSelected = isSelected(index);
        // Disable redrawing to prevent flickering. Ensure that redrawing is
        // restored by using the try-finally block.
        setRedraw(false);
        try
        {
            remove(index);
            add(string, icon, index);
            if (isSelected)
            {
                select(index);
            }
        }
        finally
        {
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
     *                <li>ERROR_NULL_ARGUMENT - if the <code>items</code>
     *                array is null</li>
     *                <li>ERROR_INVALID_RANGE - if number of elements in
     *                <code>icons</code> does not match the number in
     *                <code>items</code></li>
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
     */
    public void setItems(String[] items, Image[] icons)
    {
        checkWidget();
        if (items == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (icons != null && icons.length != items.length) internal_error(SWT.ERROR_INVALID_RANGE);
        int [] iconsHandles = null;
        if (icons != null)
        {
            iconsHandles = new int[icons.length];
            for (int i = 0; i < icons.length; i++)
            {
                if (icons[i] != null)
                {
                    iconsHandles[i] = icons[i].handle;
                }
                else
                {
                    iconsHandles[i] = 0;
                }
            }
        }

        OS.ListView_SetItems(listViewHandle, items, iconsHandles);
        this.icons.removeAllElements();
        for (int i = 0; i < items.length; i++)
        {
            this.icons.addElement((icons == null) ? null : icons[i]);
        }
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
     *                <li>ERROR_INVALID_ARGUMENT - if the style value is
     *                invalid</li>
     *                </ul>
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
    public void setLayout(int layout)
    {
        checkWidget();
        if (layout != SWT.HORIZONTAL && layout != SWT.VERTICAL)
        {
            internal_error(SWT.ERROR_INVALID_ARGUMENT);
        }
        OS.ListView_SetLayout(listViewHandle, layout);
    }

    /**
     * Changes the item density level.
     *
     * @param style
     *            the density level. Only HIGH, MEDIUM, LOW can be used.
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the style value is
     *                invalid</li>
     *                </ul>
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
    public void setLayoutDensity(int style)
    {
        checkWidget();
        if (style != LOW && style != MEDIUM && style != HIGH) internal_error(SWT.ERROR_INVALID_ARGUMENT);
        if (density != style)
        {
            OS.ListView_SetLayoutDensity(listViewHandle, style);
            density = style;
        }
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * @see List#deselectAll()
     * @see List#select(int)
     */
    public void setSelection(int index)
    {
        checkWidget();
        deselectAll();
        select(index, true);
        if ((internal_getStyle() & SWT.MULTI) != 0)
        {
            if (index >= 0) setFocusIndex(index);
        }
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *
     * @see List#deselectAll()
     * @see List#select(int,int)
     */
    public void setSelection(int start, int end)
    {
        checkWidget();
        deselectAll();
        if (end < 0 || start > end || ((internal_getStyle() & SWT.SINGLE) != 0 && start != end)) return;
        start = Math.max(0, start);
        if ((internal_getStyle() & SWT.SINGLE) != 0)
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *
     * @see List#deselectAll()
     * @see List#select(int[])
     */
    public void setSelection(int[] indices)
    {
        checkWidget();
        if (indices == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        deselectAll();
        int length = indices.length;
        if (length == 0 || ((internal_getStyle() & SWT.SINGLE) != 0 && length > 1)) return;
        select(indices, true);
        if ((internal_getStyle() & SWT.MULTI) != 0)
        {
            int focusIndex = indices [0];
            if (focusIndex >= 0) setFocusIndex(focusIndex);
        }
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *
     * @see List#deselectAll()
     * @see List#select(int[])
     * @see List#setSelection(int[])
     */
    public void setSelection(String[] items)
    {
        checkWidget();
        if (items == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        deselectAll();
        int length = items.length;
        if (length == 0 || ((internal_getStyle() & SWT.SINGLE) != 0 && length > 1)) return;
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
                    if ((internal_getStyle() & SWT.SINGLE) != 0)
                    {
                        showSelection();
                        return;
                    }
                    index++;
                }
                if (localFocus != -1) focusIndex = localFocus;
            }
        }
        if ((internal_getStyle() & SWT.MULTI) != 0)
        {
            if (focusIndex >= 0) setFocusIndex(focusIndex);
        }
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void showSelection()
    {
        checkWidget();
        OS.ListView_ShowSelection(listViewHandle);
    }

}