/*******************************************************************************
 * Copyright (c) 2000, 2006 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.swt.widgets;

import org.eclipse.ercp.swt.expanded.internal.OS;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.events.TreeListener;
import org.eclipse.swt.graphics.Point;

import java.util.Vector;

/**
 * Instances of this class provide a selectable user interface object
 * that displays a hierarchy of items and issues notification when an
 * item in the hierarchy is selected.
 * <p>
 * The item children that may be added to instances of this class
 * must be of type <code>TreeItem</code>.
 * </p><p>
 * Style <code>VIRTUAL</code> is used to create a <code>Tree</code> whose
 * <code>TreeItem</code>s are to be populated by the client on an on-demand basis
 * instead of up-front.  This can provide significant performance improvements for
 * trees that are very large or for which <code>TreeItem</code> population is
 * expensive (for example, retrieving values from an external source).
 * </p><p>
 * Here is an example of using a <code>Tree</code> with style <code>VIRTUAL</code>:
 * <code><pre>
 *  final Tree tree = new Tree(parent, SWT.VIRTUAL | SWT.BORDER);
 *  tree.setItemCount(20);
 *  tree.addListener(SWT.SetData, new Listener() {
 *      public void handleEvent(Event event) {
 *          TreeItem item = (TreeItem)event.item;
 *          TreeItem parentItem = item.getParentItem();
 *          String text = null;
 *          if (parentItem == null) {
 *              text = "node " + tree.indexOf(item);
 *          } else {
 *              text = parentItem.getText() + " - " + parentItem.indexOf(item);
 *          }
 *          item.setText(text);
 *          System.out.println(text);
 *          item.setItemCount(10);
 *      }
 *  });
 * </pre></code>
 * </p><p>
 * Note that although this class is a subclass of <code>Composite</code>,
 * it does not make sense to add <code>Control</code> children to it,
 * or set a layout on it.
 * </p><p>
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>SINGLE, MULTI, CHECK, FULL_SELECTION, VIRTUAL</dd>
 * <dt><b>Events:</b></dt>
 * <dd>Selection, DefaultSelection, Collapse, Expand, SetData, MeasureItem, EraseItem, PaintItem</dd>
 * </dl>
 * </p><p>
 * Note: Only one of the styles SINGLE and MULTI may be specified.
 * </p><p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */
public class Tree extends Composite
{
    int treeHandle;
    Vector items = new Vector();

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
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public Tree(Composite parent, int style)
    {
        super(parent, checkStyle(style));
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notified when the receiver's selection changes, by sending
     * it one of the messages defined in the <code>SelectionListener</code>
     * interface.
     * <p>
     * When <code>widgetSelected</code> is called, the item field of the event object is valid.
     * If the receiver has <code>SWT.CHECK</code> style set and the check selection changes,
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
        if (listener == null) error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Selection, typedListener);
        addListener(SWT.DefaultSelection, typedListener);
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notified when an item in the receiver is expanded or collapsed
     * by sending it one of the messages defined in the <code>TreeListener</code>
     * interface.
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
     * @see TreeListener
     * @see #removeTreeListener
     */
    public void addTreeListener(TreeListener listener)
    {
        checkWidget();
        if (listener == null) error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Expand, typedListener);
        addListener(SWT.Collapse, typedListener);
    }

    protected void checkSubclass()
    {
        if (!internal_isValidSubclass()) error(SWT.ERROR_INVALID_SUBCLASS);
    }

    public Point computeSize(int wHint, int hHint, boolean changed)
    {
        checkWidget();
        return org.eclipse.swt.internal.symbian.OS.Control_ComputeSize(handle, wHint, hHint);
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
        OS.Tree_SelectAll(treeHandle, false);
    }

    /**
     * Returns the item at the given point in the receiver
     * or null if no such item exists. The point is in the
     * coordinate system of the receiver.
     * <p>
     * The item that is returned represents an item that could be selected by the user.
     * For example, if selection only occurs in items in the first column, then null is
     * returned if the point is outside of the item.
     * Note that the SWT.FULL_SELECTION style hint, which specifies the selection policy,
     * determines the extent of the selection.
     * </p>
     *
     * @param point the point used to locate the item
     * @return the item at the given point, or null if the point is not in a selectable item
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the point is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public TreeItem getItem(Point point)
    {
        checkWidget();
        if (point == null) error(SWT.ERROR_NULL_ARGUMENT);
        return itemFromHandle(OS.Tree_ItemAt(treeHandle, point.x, point.y));
    }

    /**
     * Returns the number of items contained in the receiver
     * that are direct item children of the receiver.  The
     * number that is returned is the number of roots in the
     * tree.
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
        return items.size();
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
        return OS.Tree_ItemHeight(treeHandle);
    }
    /**
     * Returns a (possibly empty) array of items contained in the
     * receiver that are direct item children of the receiver.  These
     * are the roots of the tree.
     * <p>
     * Note: This is not the actual structure used by the receiver
     * to maintain its list of items, so modifying the array will
     * not affect the receiver.
     * </p>
     *
     * @return the items
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public TreeItem[] getItems()
    {
        checkWidget();
        int count = items.size();
        TreeItem[] result = new TreeItem [count];
        if (count == 0) return result;
        items.copyInto(result);
        return result;
    }

    /**
     * Returns the receiver's parent item, which must be a
     * <code>TreeItem</code> or null when the receiver is a
     * root.
     *
     * @return the receiver's parent item
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public TreeItem getParentItem()
    {
        checkWidget();
        // No implementation support
        return null;
    }

    /**
     * Returns an array of <code>TreeItem</code>s that are currently
     * selected in the receiver. The order of the items is unspecified.
     * An empty array indicates that no items are selected.
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
    public TreeItem[] getSelection()
    {
        checkWidget();
        int[] handles = OS.Tree_Selection(treeHandle);
        TreeItem[] res = new TreeItem[handles.length];
        for (int i = 0; i < res.length; i++)
        {
            res[i] = itemFromHandle(handles[i]);
        }
        return res;
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
        return OS.Tree_SelectionCount(treeHandle);
    }

    /**
     * Returns the item which is currently at the top of the receiver.
     * This item can change when items are expanded, collapsed, scrolled
     * or new items are added or removed.
     *
     * @return the item at the top of the receiver
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 2.1
     */
    public TreeItem getTopItem()
    {
        checkWidget();
        return itemFromHandle(OS.Tree_TopItem(treeHandle));
    }

    protected void internal_createHandle()
    {
        this.treeHandle = OS.Tree_New(this, parent.compositeHandle, style);
        internal_setCompositeHandle(OS.Tree_CompositeHandle(this.treeHandle));
    }

    protected void internal_sendEvent(int eventType, int itemHandle)
    {
        if (eventTable == null && !display.filters(eventType))
        {
            return;
        }
        Event event = new Event();
        event.display = display;
        event.widget  = this;
        event.type    = eventType;
        event.detail  = ((SWT.CHECK & getStyle()) != 0) ? SWT.CHECK : 0;
        event.item    = itemFromHandle(itemHandle);
        if (event.time == 0)
        {
            event.time = display.getLastEventTime();
        }
        sendEvent(event);
    }

    TreeItem itemFromHandle(int id)
    {
        for (int i = 0; i < items.size(); i++)
        {
            TreeItem item = (TreeItem)items.elementAt(i);
            TreeItem res = item.itemFromHandle(id);
            if (res != null) return res;
        }
        return null;
    }

    protected void releaseChildren()
    {
        if (items != null)
        {
            TreeItem[] items = getItems();
            for (int i=0; i<items.length; i++)
            {
                TreeItem item = items [i];
                if (item != null && !item.isDisposed())
                {
                    items [i].dispose();
                }
            }
            items = null;

            super.releaseChildren();
        }
    }

    /**
     * Removes all of the items from the receiver.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void removeAll()
    {
        checkWidget();
        TreeItem[] items = getItems();
        OS.Tree_RemoveAll(treeHandle);
        for (int i = 0; i < items.length; i++)
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
     * @see #addSelectionListener
     */
    public void removeSelectionListener(SelectionListener listener)
    {
        checkWidget();
        if (listener == null) error(SWT.ERROR_NULL_ARGUMENT);
        removeListener(SWT.Selection, listener);
        removeListener(SWT.DefaultSelection, listener);
    }

    /**
     * Removes the listener from the collection of listeners who will
     * be notified when items in the receiver are expanded or collapsed.
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
     * @see TreeListener
     * @see #addTreeListener
     */
    public void removeTreeListener(TreeListener listener)
    {
        checkWidget();
        if (listener == null) error(SWT.ERROR_NULL_ARGUMENT);
        removeListener(SWT.Expand, listener);
        removeListener(SWT.Collapse, listener);
    }

    /**
     * Selects all of the items in the receiver.
     * <p>
     * If the receiver is single-select, do nothing.
     * </p>
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
        OS.Tree_SelectAll(treeHandle, true);
    }

    /**
     * Sets the receiver's selection to be the given array of items.
     * The current selection is cleared before the new items are selected.
     * <p>
     * Items that are not in the receiver are ignored.
     * If the receiver is single-select and multiple items are specified,
     * then all items are ignored.
     * </p>
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
     * @see Tree#deselectAll()
     */
    public void setSelection(TreeItem[] items)
    {
        checkWidget();
        if (items == null) error(SWT.ERROR_NULL_ARGUMENT);
        OS.Tree_SelectAll(treeHandle, false);
        int length = items.length;
        if (length == 0 || ((style & SWT.SINGLE) != 0 && length > 1)) return;
        int[] itemHandles = new int[length];
        for (int i = 0; i < length; i++)
        {
            itemHandles[i] = items[i].handle;
        }
        OS.Tree_Select(treeHandle, itemHandles, true);
    }

    /**
     * Sets the item which is currently at the top of the receiver.
     * This item can change when items are expanded, collapsed, scrolled
     * or new items are added or removed.
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
     * @see Tree#getTopItem()
     *
     * @since 2.1
     */
    public void setTopItem(TreeItem item)
    {
        checkWidget();
        if (item == null) error(SWT.ERROR_NULL_ARGUMENT);
        if (item.isDisposed()) error(SWT.ERROR_INVALID_ARGUMENT);
        // No platform support
    }

    final void setTreeHandle(int treeHandle)
    {
        this.treeHandle = treeHandle;
        internal_setCompositeHandle(OS.Tree_CompositeHandle(treeHandle));
    }

    /**
     * Shows the item.  If the item is already showing in the receiver,
     * this method simply returns.  Otherwise, the items are scrolled
     * and expanded until the item is visible.
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
     * @see Tree#showSelection()
     */
    public void showItem(TreeItem item)
    {
        checkWidget();
        if (item == null) error(SWT.ERROR_NULL_ARGUMENT);
        if (item.isDisposed()) error(SWT.ERROR_INVALID_ARGUMENT);
        // No platform support
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
     * @see Tree#showItem(TreeItem)
     */
    public void showSelection()
    {
        checkWidget();
        // No platform support
    }

    static int checkStyle(int style)
    {
        return internal_checkBits(style, SWT.SINGLE, SWT.MULTI, 0, 0, 0, 0);
    }

}

