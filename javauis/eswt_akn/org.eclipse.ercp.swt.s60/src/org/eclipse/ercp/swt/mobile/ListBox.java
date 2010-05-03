/*******************************************************************************
 * Copyright (c) 2004, 2007 Nokia Corporation.
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

import org.eclipse.swt.*;
import org.eclipse.swt.widgets.*;
import org.eclipse.swt.events.*;
import org.eclipse.swt.graphics.*;
import org.eclipse.ercp.swt.mobile.internal.OS;

/**
 * Instances of this class represent a selectable user interface object that
 * displays a list of items consisting of text and icons from a data model. Each
 * list item may include combinations of heading text, heading icons, detail
 * text, and detail icons. The layout and display of the various text and icons
 * is variable depending upon the style and modifier constants passed to the
 * constructor. Only one LB_STYLE_xxxx layout style constant may be specified.
 * The LB_STYLE_NO_HEADING_TEXT style displays detail text on a single line per
 * item. The LB_STYLE_1LINE_ITEM style displays heading text next to detail text
 * in a single row. The LB_STYLE_2LINE_ITEM style displays heading text above
 * detail text in a two line cell.
 * <p>
 * The following diagrams show the general layout for each layout style:
 * <p>
 * LB_STYLE_NO_HEADING_TEXT <Table width=200 border=1>
 * <tr>
 * <td>Detail Text</td>
 * </tr>
 * <tr>
 * <td>Detail Text</td>
 * </tr>
 * <tr>
 * <td>Detail Text</td>
 * </tr>
 * <tr>
 * <td>Detail Text</td>
 * </tr>
 * </table>
 * <p>
 * LB_STYLE_1LINE_ITEM <Table width=200 border=1>
 * <tr>
 * <td>Heading Text</td>
 * <td>Detail Text</td>
 * </tr>
 * <tr>
 * <td>Heading Text</td>
 * <td>Detail Text</td>
 * </tr>
 * <tr>
 * <td>Heading Text</td>
 * <td>Detail Text</td>
 * </tr>
 * <tr>
 * <td>Heading Text</td>
 * <td>Detail Text</td>
 * </tr>
 * </table>
 * <p>
 * LB_STYLE_2LINE_ITEM <Table width=200 border=1>
 * <tr>
 * <td><b>Heading Text </b> <br>
 * Detail Text</td>
 * </tr>
 * <tr>
 * <td><b>Heading Text </b> <br>
 * Detail Text</td>
 * </tr>
 * <tr>
 * <td><B>Heading Text </b> <br>
 * Detail Text</td>
 * </tr>
 * </table>
 * <p>
 * The specific placement of item elements is locale and platform-dependent.
 * LB_MOD_xxxx style modifiers are hints which are implemented in a
 * platform-dependent fashion or perhaps not at all. They may be specified by
 * or-ing any number of LB_MOD_xxxx constants with one selected layout style.
 * Heading and detail icons are optionally displayed on any of the style layouts
 * by specifying the LB_MOD_SHOW_HEADING_ICONS or LB_MOD_SHOW_DETAILS_ICONS
 * modifiers. Heading and detail icons, if both displayed, are separated within
 * the layout to indicate that they are distinct from each other. The
 * LB_MOD_SHOW_SELECTION_NUMBER modifier is a hint for the ListBox to display a
 * single digit next to each item which indicates that pressing that number
 * selects the item. A list may be SINGLE or MULTI select. However, due to
 * typical layout contraints, MULTI select is a hint that is guaranteed to be
 * implemented only when no style modifiers are used.
 * <p>
 * The following is a possible layout for LB_STYLE_1LINE_ITEM |
 * LB_MOD_SHOW_DETAIL_ICONS | LB_MOB_SHOW_SELECTION_NUMBER: <Table width=200
 * border=1>
 * <tr>
 * <td>1</td>
 * <td>Heading</td>
 * <td>Detail Text</td>
 * <td>icon</td>
 * </tr>
 * <tr>
 * <td>2</td>
 * <td>Heading</td>
 * <td>Detail Text</td>
 * <td>icon</td>
 * </tr>
 * <tr>
 * <td>3</td>
 * <td>Heading</td>
 * <td>Detail Text</td>
 * <td>icon</td>
 * </tr>
 * <tr>
 * <td>4</td>
 * <td>Heading</td>
 * <td>Detail Text</td>
 * <td>icon</td>
 * </tr>
 * </table>
 *
 * <p>
 * The model data is provided by an array of ListBoxItems. The array is not
 * copied. The array is used until the ListBox is destroyed or a new data model
 * is set.
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed. On some
 * platforms, the fonts of the heading and detail texts are defined by the
 * layout styles. ListBox allows setting the fonts even if the underlying
 * platform does not, but in this case the appearance of the control will not
 * change.
 * <p>
 * <dl>
 * <dt><b>Styles: </b></dt>
 * <dd>SINGLE, MULTI</dd>
 * <p>
 * Note: Only one of SINGLE and MULTI may be specified.
 * <p>
 * </dl>
 * <dl>
 * <dt><b>Layout Styles: </b></dt>
 * <dd>LB_STYLE_NO_HEADING_TEXT</dd>
 * <dd>LB_STYLE_1LINE_ITEM</dd>
 * <dd>LB_STYLE_2LINE_ITEM</dd>
 * <p>
 * Note: Only one layout style may be specified. If more than one layout style
 * is specified, the actual style used is implementation dependent.
 * <p>
 * <dl>
 * <dt><b>Layout Style Modifiers: </b></dt>
 * <dd>LB_MOD_SHOW_SELECTION_NUMBER</dd>
 * <dd>LB_MOD_SHOW_HEADING_ICONS</dd>
 * <dd>LB_MOD_SHOW_DETAIL_ICONS</dd>
 * </dl>
 * <p>
 * Note: Style modifiers are hints. Any number of modifier styles may be
 * combined with a layout style
 * </dl>
 * <p>
 * <dt><b>Events: </b></dt>
 * <dd>Selection, DefaultSelection</dd>
 * </dl>
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 * @see ListBoxItem
 */
public class ListBox extends Scrollable
{
    private int listBoxHandle;
    ListBoxItem[] items;

    /**
     * single line item, 1 column (default)
     */
    public static final int LB_STYLE_NO_HEADING_TEXT = 0;

    /**
     * single line item, 2 columns
     */
    public static final int LB_STYLE_1LINE_ITEM = 1;

    /**
     * double line item, 1 column with heading and detail combined
     */
    public static final int LB_STYLE_2LINE_ITEM = 2;

    /**
     * shows a single digit number aligned with each item which may be used to
     * select the item.
     */
    public static final int LB_MOD_SHOW_SELECTION_NUMBER = 0x10;

    /**
     * shows icon associated with heading text
     */
    public static final int LB_MOD_SHOW_HEADING_ICONS = 0x20;

    /**
     * shows icons associated with detail text
     */
    public static final int LB_MOD_SHOW_DETAIL_ICONS = 0x40;

    /**
     * Constructs a new instance of this class given its parent, a style value
     * describing its selection behavior, and a style value describing its
     * layout.
     * <p>
     * The <code>style</code> value is either one of the style constants
     * defined in class <code>SWT</code> which is applicable to instances of
     * this class, or must be built by <em>bitwise OR</em> 'ing together (that
     * is, using the <code>int</code> "|" operator) two or more of those
     * <code>SWT</code> style constants. Style bits are also inherited from
     * superclasses.
     * <p>
     * The <code>layoutStyle</code> value is one or more of the class layout
     * style constants which may be combined by <em>bitwise OR</em> 'ing. Use
     * of styles not applicaple to this class are ignored.
     * </p>
     *
     * @param parent
     *            a composite control which will be the parent of the new
     *            instance (cannot be null)
     * @param style
     *            the style of control to construct
     *
     * @param layoutStyle
     *            the layout style of control to construct
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
     */
    public ListBox(Composite parent, int style, int layoutStyle)
    {
        super(parent, checkStyle(style), checkLayoutStyle(layoutStyle));
    }

    final void setListBoxHandle(int listBoxHandle)
    {
        this.listBoxHandle = listBoxHandle;
        internal_setScrollableHandle(OS.ListBox_ScrollableHandle(listBoxHandle));
    }

    protected void internal_createHandle()
    {
        setListBoxHandle(OS.ListBox_New(this, getParent().internal_getCompositeHandle(), internal_getStyle()));
        OS.ListBox_SetLayoutStyle(listBoxHandle, internal_getLayoutStyle());
    }

    static int checkStyle(int style)
    {
        return internal_checkBits(style, SWT.SINGLE, SWT.MULTI, 0, 0, 0, 0);
    }

    static int checkLayoutStyle(int layoutStyle)
    {
        return internal_checkBits(layoutStyle, LB_STYLE_NO_HEADING_TEXT, LB_STYLE_1LINE_ITEM, LB_STYLE_2LINE_ITEM, 0, 0, 0);
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
        addListener(SWT.Selection, typedListener);
        addListener(SWT.DefaultSelection, typedListener);
    }

    public Point computeSize(int wHint, int hHint, boolean changed)
    {
        checkWidget();
        return org.eclipse.swt.internal.symbian.OS.Control_ComputeSize(handle, wHint, hHint);
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
        if ((internal_getStyle() & SWT.SINGLE) != 0) return;
        OS.ListBox_Deselect(listBoxHandle, index);
    }

    /**
     * Deselects the items at the given zero-relative indices in the receiver.
     * If the items at the given zero-relative indexes in the receiver are
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
        if ((internal_getStyle() & SWT.SINGLE) != 0) return;
        if (start > end) return;
        if (end < 0) return;
        OS.ListBox_Deselect(listBoxHandle, start, end);
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
        if ((internal_getStyle() & SWT.SINGLE) != 0) return;
        OS.ListBox_Deselect(listBoxHandle, indices);
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
        if ((internal_getStyle() & SWT.SINGLE) != 0) return;
        OS.ListBox_DeselectAll(listBoxHandle);
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
     * </ul>
     */
    public int getFocusIndex()
    {
        checkWidget();
        int result = OS.ListBox_GetFocusIndex(listBoxHandle);
        return result;
    }

    /**
     * Returns the number of items currently selected.
     * <p>
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
     * @see #getSelectionIndices
     */
    public int getSelectionCount()
    {
        checkWidget();
        return OS.ListBox_GetSelectionCount(listBoxHandle);
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
        return OS.ListBox_GetSelectionIndices(listBoxHandle);
    }

    /**
     * Notifies this ListBox that the data for the item at the given index has
     * been updated and the item display needs to be refreshed.
     *
     * @param index
     *            an index into the data model array
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
     *                <li>ERROR_INVALID_RANGE - if the index is not between 0
     *                and the number of elements in the array minus 1
     *                (inclusive)</li>
     *                </ul>
     *
     * @see ListBox#refreshList
     */
    public void refreshItem(int index)
    {
        checkWidget();
        if (index < 0 || index >= items.length) internal_error(SWT.ERROR_INVALID_RANGE);
        OS.ListBox_RefreshItem(listBoxHandle, index, items[index]);
    }

    /**
     * Notifies this ListBox that multiple items may have been updated and the
     * entire list display needs to be refreshed.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     *
     * @see ListBox#refreshItem
     */
    public void refreshList()
    {
        checkWidget();
        OS.ListBox_RefreshList(listBoxHandle, items);
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
     * @see ListBox#addSelectionListener
     * @see SelectionListener
     */
    public void removeSelectionListener(SelectionListener listener)
    {
        removeListener(SWT.Selection,        listener);
        removeListener(SWT.DefaultSelection, listener);
    }

    /**
     * Sets the font for the details text of an item. If the argument is null,
     * the default font will be used.
     *
     * @param font
     *            the new font or null
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
     *                <li>ERROR_INVALID_ARGUMENT - if the argument has been
     *                disposed</li>
     *                </ul>
     *
     * @see ListBox#setHeadingFont
     */
    public void setFont(Font font)
    {
        super.setFont(font);
    }

    /**
     * Sets the font for the heading text of an item. If the argument is null,
     * the default font will be used.
     *
     * @param font
     *            the new font or null
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
     *                <li>ERROR_INVALID_ARGUMENT - if the argument has been
     *                disposed</li>
     *                </ul>
     *
     * #see ListBox#setFont
     */
    public void setHeadingFont(Font font)
    {
        checkWidget();
        int fontHandle;
        if (font != null)
        {
            if (font.isDisposed()) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            fontHandle = font.handle;
        }
        else
        {
            fontHandle =  0;
        }
        OS.ListBox_SetHeadingFont(listBoxHandle, fontHandle);
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
        select(index , false);
    }

    void select(int index, boolean scroll)
    {
        if (index < 0) return;
        OS.ListBox_Select(listBoxHandle, index, scroll);
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
     * @see #setSelection(int,int)
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
        OS.ListBox_Select(listBoxHandle, start, end);
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
     * @see #setSelection(int[])
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
        OS.ListBox_SelectAll(listBoxHandle);
    }

    void setFocusIndex(int index)
    {
        if ((internal_getStyle() & SWT.SINGLE) != 0)
        {
            // We consider that setting the focus in a single list means selecting this item.
            if (index < 0) return;
            OS.ListBox_Select(listBoxHandle, index, true);
        }
        else
        {
            OS.ListBox_SetFocusIndex(listBoxHandle, index);
        }
    }
    /**
     * Establishes the data model for this ListBox. The provided array is used
     * for the life of the ListBox or until a new data model is set. Elements of
     * a ListBoxItem which are <code>null</code> or disposed are not displayed,
     * leaving a blank area within the layout.
     *
     * @param items
     *            an array of ListBoxItems or <code>null</code>
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
     *                <li>ERROR_NULL_ARGUMENT - if the items is null</li>
     *                </ul>
     *
     * @see ListBoxItem
     */
    public void setDataModel(ListBoxItem[] items)
    {
        checkWidget();
        if (items == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        this.items = items;
        OS.ListBox_SetDataModel(listBoxHandle, items);
    }

    /**
     * Selects the item at the given zero-relative index in the receiver. If the
     * item at the index was already selected, it remains selected. The current
     * selected is first cleared, then the new items are selected. Index that
     * is out of range are ignored.
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
     * @see #deselectAll()
     * @see #select(int[])
     */
    public void setSelection(int[] indices)
    {
        checkWidget();
        if (indices == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        deselectAll();
        int length = indices.length;
        if (length == 0 || ((internal_getStyle() & SWT.SINGLE) != 0 && length > 1)) return;
        select(indices, true);
        //setting the focus to the first index item
        if ((internal_getStyle() & SWT.MULTI) != 0)
        {
            int i=0;
            while (i < length)
            {
                int focusIndex = indices [i];
                if (focusIndex >= 0)
                {
                    setFocusIndex(focusIndex);
                    break;
                }
                i++;
            }
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
        OS.ListBox_ShowSelection(listBoxHandle);
    }

}

