/*******************************************************************************
 * Copyright (c) 2000, 2006 IBM Corporation and others.
 * Portion Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Nokia Corporation - S60 implementation
 *     Nokia Corporation - Qt implementation
 *******************************************************************************/
package org.eclipse.swt.widgets;

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.events.TreeListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.WidgetState;

/**
 * Instances of this class provide a selectable user interface object that
 * displays a hierarchy of items and issues notification when an item in the
 * hierarchy is selected.
 * <p>
 * The item children that may be added to instances of this class must be of
 * type <code>TreeItem</code>.
 * </p>
 * <p>
 * Style <code>VIRTUAL</code> is used to create a <code>Tree</code> whose
 * <code>TreeItem</code>s are to be populated by the client on an on-demand
 * basis instead of up-front. This can provide significant performance
 * improvements for trees that are very large or for which <code>TreeItem</code>
 * population is expensive (for example, retrieving values from an external
 * source).
 * </p>
 * <p>
 * 
 * Here is an example of using a <code>Tree</code> with style
 * <code>VIRTUAL</code>: <code><pre>
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
 * </p>
 * <p>
 * Note that although this class is a subclass of <code>Composite</code>, it
 * does not normally make sense to add <code>Control</code> children to it, or
 * set a layout on it, unless implementing something like a cell editor.
 * </p>
 * <p>
 * 
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>SINGLE, MULTI, CHECK, VIRTUAL</dd>
 * <dt><b>Events:</b></dt>
 * <dd>Selection, DefaultSelection, Collapse, Expand, SetData,</dd>
 * </dl>
 * </p>
 * <p>
 * 
 * Note: Only one of the styles SINGLE and MULTI may be specified.
 * </p>
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 **/
public class Tree extends Composite {

    static int checkStyle(int style) {
        style |= SWT.H_SCROLL | SWT.V_SCROLL;
        return checkBits(style, SWT.SINGLE, SWT.MULTI, 0, 0, 0, 0);
    }

    // The root items of the Tree.
    TreeItem[] topLevelItems;

    int topLevelItemCount;
    boolean ignoreSelectionEvent;

    boolean ignoreExpandEvent;

    int[] topLevelItemsHandle;

    // Tells if any item has expanded used for SWT.VIRTUAL style Tree
    int expandedItemCount = 0;

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
     * @see SWT#CHECK
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public Tree(Composite parent, int style) {
        super(parent, checkStyle(style));
    }

    TreeItem _getItem(int index) {
        if ((style & SWT.VIRTUAL) == 0)
            return topLevelItems[index];
        if (topLevelItems[index] != null)
            return topLevelItems[index];
        return topLevelItems[index] = new TreeItem(this, SWT.NONE, index, false);
    }

    void _showItem(TreeItem item, int aScrollHint ){
        if (item == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        if (item.isDisposed())
            error(SWT.ERROR_INVALID_ARGUMENT);
        if (item.getParent() != this)
            return;

        int itemHandle = item.internal_itemHandle();
        if((style & SWT.VIRTUAL)!=0 && !item.cached){
            TreeItem parentItem = item.getParentItem();
            if(parentItem!=null) {
                for(int i=0;i<parentItem.childrenItemCount;i++){
                    if(item==parentItem.childrenItems[i]){ 
                        itemHandle = parentItem.childrenItemsHandle[i];
                        break;
                    }
                }
            } else {
                for(int i=0;i<topLevelItemCount;i++){
                    if(item==topLevelItems[i]){ 
                        itemHandle = topLevelItemsHandle[i];
                        break;
                    }
                }
            }
        }
        OS.QTreeWidget_scrollToItem(topHandle, itemHandle, aScrollHint);
    }

    /**
     * Adds the listener to the collection of listeners who will be notified
     * when the receiver's selection changes, by sending it one of the messages
     * defined in the <code>SelectionListener</code> interface.
     * <p>
     * When <code>widgetSelected</code> is called, the item field of the event
     * object is valid. If the reciever has <code>SWT.CHECK</code> style set and
     * the check selection changes, the event object detail field contains the
     * value <code>SWT.CHECK</code>. <code>widgetDefaultSelected</code> is
     * typically called when an item is double-clicked. The item field of the
     * event object is valid for default selection, but the detail field is not
     * used.
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

    /*
     * Add a new child to the Tree's list of children. Add the item at the given
     * index. An index of -1 implies that the child should be added to the end
     * of the Tree's list of children.
     */
    void addTopLevelItem(TreeItem item, int index, int itemHandle) {

        if (!(0 <= index && index <= topLevelItemCount))
            error(SWT.ERROR_INVALID_RANGE);

        increaseArraySize();
        // Put the new item in the correct place in item array
        System.arraycopy(topLevelItems, index, topLevelItems, index + 1,
                topLevelItemCount + 1 - index);
        topLevelItems[index] = item;

        // itemCount is already increase in the previous line.
        System.arraycopy(topLevelItemsHandle, index, topLevelItemsHandle,
                index + 1, topLevelItemCount + 1 - index);
        topLevelItemsHandle[index] = itemHandle;

        topLevelItemCount++;
    }

    /**
     * Adds the listener to the collection of listeners who will be notified
     * when an item in the receiver is expanded or collapsed by sending it one
     * of the messages defined in the <code>TreeListener</code> interface.
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
     * @see TreeListener
     * @see #removeTreeListener
     */
    public void addTreeListener(TreeListener listener) {
        checkWidget();
        if (listener == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Expand, typedListener);
        addListener(SWT.Collapse, typedListener);
    }

    boolean checkData(TreeItem item, int index) {
        if (item.cached)
            return true;
        if ((style & SWT.VIRTUAL) != 0) {
            if (index == -1)
                return false;
            Event event = new Event();
            event.item = item;
            event.index = index;
            item.ensureValid(index);
            sendEvent(SWT.SetData, event);
            if (isDisposed())
                return false;
            if (item.isDisposed())
                return false;
        }
        if (isDisposed())
            return false;
        if (item == null || item.isDisposed())
            return false;
        return true;

    }

    boolean checkData(TreeItem item, TreeItem parentItem) {
        return checkData(item, parentItem == null ? getItemIndex(item)
                : parentItem.getChildItemIndex(item));
    }

    protected void checkSubclass() {
        if (!isValidSubclass())
            error(SWT.ERROR_INVALID_SUBCLASS);
    }

    void clearSelection(){
        for(int i=0;i<topLevelItemCount;i++){
            TreeItem topItem = _getItem(i);
            topItem.selection=false;
            topItem.clearSelection();
        }
    }

    void createHandle_pp(int index) {
        topHandle = scrollAreaHandle = frameHandle = OS.QTreeWidget_new();
        handle = OS.QAbstractScrollArea_viewPort(topHandle);
        state |= WidgetState.HANDLE;
        state &= ~WidgetState.CANVAS;

        OS.QTreeWidget_setColumnCount(topHandle, 1);

        // Set up selection mode (MULTI/SINGLE)
        if ((style & SWT.MULTI) != 0) {
            OS.QAbstractItemView_setSelectionMode(topHandle,
                    OS.QT_ABSTRACTITEMVIEW_SELECTIONMODE_MULTI);
        } else {
            OS.QAbstractItemView_setSelectionMode(topHandle,
                    OS.QT_ABSTRACTITEMVIEW_SELECTIONMODE_SINGLE);
        }

        OS.QTreeWidget_swt_setHeaderVisible(topHandle, false);
    }

    void createWidget(int index) {
        super.createWidget(index);
        topLevelItems = new TreeItem[4];
        topLevelItemCount = 0;
        topLevelItemsHandle = new int[4];
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
        if ((style & SWT.SINGLE) != 0) {
            return;
        }
        ignoreSelectionEvent = true;
        OS.QAbstractItemView_clearSelection(topHandle);

        if((style&SWT.VIRTUAL)!=0){
            clearSelection();
        }
        ignoreSelectionEvent = false;
    }

    /*
     *  Its a recursive function executed only in virtual tree style.
     *  It takes item handle as a parameter, finds and return. If it 
     *  can't find the item with that handle it just returns null. 
     */
    TreeItem findParentItem(int itemHandle) {
        TreeItem item = null;
        for (int i = 0; i < topLevelItemCount; i++) {
            if (topLevelItemsHandle[i] == itemHandle) {
                item = _getItem(i);
                break;
            } else {
                TreeItem topLevelItem = _getItem(i);
                if(topLevelItem.cached){
                    if (topLevelItem.getExpanded()) {
                        item = topLevelItem.findItem(itemHandle);
                    }}
                if (item != null)
                    break;
            }
        }
        return item;
    }

    /**
     * Returns the item at the given point in the receiver or null if no such
     * item exists. The point is in the coordinate system of the receiver.
     * 
     * @param point
     *            the point used to locate the item
     * @return the item at the given point
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the point is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public TreeItem getItem(Point point) {
        checkWidget();
        if (point == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        int itemHandle = OS.QTreeWidget_itemAt(topHandle, point.x,
                point.y);
        if (itemHandle == -1)
            return null;
        TreeItem item = null;
        for (int i = 0; i < topLevelItemCount; i++) {

            if (topLevelItemsHandle[i] == itemHandle) {
                item = _getItem(i);
                break;
            } else {
                item = _getItem(i).getItem(itemHandle);
                if (item != null)
                    break;
            }
        }
        return item;
    }

    /**
     * Returns the number of items contained in the receiver that are direct
     * item children of the receiver. The number that is returned is the number
     * of roots in the tree.
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
        return topLevelItemCount;
    }

    /**
     * Returns the height of the area which would be used to display
     * <em>one</em> of the items in the tree.
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
        int itemHeight = 0;
        if (topLevelItemCount > 0) {
            itemHeight = OS.QAbstractItemView_sizeHintForRow(
                    topHandle, 0);
        } else {
            OS.QTreeWidget_addTopLevelItem(topHandle, OS
                    .QTreeWidgetItem_new());
            itemHeight = OS.QAbstractItemView_sizeHintForRow(
                    topHandle, 0);
            OS.QTreeWidget_clear(topHandle);
        }
        return itemHeight;
    }

    int getItemIndex(TreeItem item) {
        if (item == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        if (topLevelItemCount == 0 || topLevelItems == null || item.getParent() != this)
            return -1;

        return OS.QTreeWidget_indexOfTopLevelItem(topHandle,
                item.internal_itemHandle());
    }

    /**
     * Returns a (possibly empty) array of items contained in the receiver that
     * are direct item children of the receiver. These
     * <p>
     * Note: This is not the actual structure used by the receiver to maintain
     * its list of items, so modifying the array will not affect the receiver.
     * </p>
     * 
     * @return the items
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public TreeItem[] getItems() {
        checkWidget();
        if (topLevelItemCount == 0)
            return new TreeItem[0];
        TreeItem[] result = new TreeItem[topLevelItemCount];
        if ((style & SWT.VIRTUAL) != 0) {
            for (int i = 0; i < topLevelItemCount; i++) {
                result[i] = _getItem(i);
                checkData(result[i], i);
            }
        } else {
            System.arraycopy(topLevelItems, 0, result, 0, topLevelItemCount);
        }
        return result;
    }

    /**
     * Returns the receiver's parent item, which must be a <code>TreeItem</code>
     * or null when the receiver is a root.
     * 
     * @return the receiver's parent item
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public TreeItem getParentItem() {
        checkWidget();
        return null;
    }

    Point getPreferredClientAreaSize_pp() {
        Point size = OS.QWidget_sizeHint(topHandle);

        if (topLevelItemCount > 0) {
            size.y = topLevelItemCount * getItemHeight();
            size.x = OS.QTreeView_columnWidth(topHandle, 0);
        }

        return size;
    }

    /**
     * Returns an array of <code>TreeItem</code>s that are currently selected in
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
    public TreeItem[] getSelection() {
        checkWidget();
        int[] selectionHandles = OS.QTreeWidget_selectedItems(topHandle);

        TreeItem[] selectedItems = new TreeItem[selectionHandles.length];

        for (int j = 0; j < selectionHandles.length; j++) {
            for (int i = 0; i < topLevelItemCount; i++) {
                if (topLevelItems[i].itemHandle == selectionHandles[j]) {
                    selectedItems[j] = topLevelItems[i];
                    break;
                } else {
                    TreeItem item = topLevelItems[i].getItem(selectionHandles[j]);
                    if (item != null) {
                        selectedItems[j] = item;
                        break;
                    }
                }
            }
        }
        return selectedItems;
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
        return OS.QTreeWidget_swt_selectionCount(topHandle);
    }

    /**
     * Returns the item which is currently at the top of the receiver. This item
     * can change when items are expanded, collapsed, scrolled or new items are
     * added or removed.
     * 
     * @return the item at the top of the receiver
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @since 2.1
     */
    public TreeItem getTopItem() {
        return getItem(new Point(1, 1));
    }

    void hookEvents_pp() {
        super.hookEvents_pp();

        int selectionSignalProxy = OS.SignalHandler_new(topHandle,
                OS.QSIGNAL_TREE_ITEMSELECTION_CHANGED);

        OS.QObject_connectOrThrow(topHandle,
                "itemSelectionChanged()", selectionSignalProxy,
                "widgetSignal()", OS.QT_AUTOCONNECTION);

        int itemActivatedSignalProxy = OS.SignalHandler_new(
                topHandle, OS.QSIGNAL_TREE_ITEM_ACTIVATED);

        OS.QObject_connectOrThrow(topHandle,
                "itemActivated(QTreeWidgetItem*,int)",
                itemActivatedSignalProxy, "widgetSignal(QTreeWidgetItem*,int)",
                OS.QT_AUTOCONNECTION);

        int itemExpandedSignalProxy = OS.SignalHandler_new(
                topHandle, OS.QSIGNAL_TREE_ITEM_EXPANDED);

        OS.QObject_connectOrThrow(topHandle,
                "itemExpanded(QTreeWidgetItem*)", itemExpandedSignalProxy,
                "widgetSignal(QTreeWidgetItem*)", OS.QT_AUTOCONNECTION);

        int itemCollapsedSignalProxy = OS.SignalHandler_new(
                topHandle, OS.QSIGNAL_TREE_ITEM_COLLAPSED);

        OS.QObject_connectOrThrow(topHandle,
                "itemCollapsed(QTreeWidgetItem*)", itemCollapsedSignalProxy,
                "widgetSignal(QTreeWidgetItem*)", OS.QT_AUTOCONNECTION);
    }

    void increaseArraySize(){
        if (topLevelItems == null) {
            topLevelItems = new TreeItem[4];
        }
        if (topLevelItemCount + 1 == topLevelItems.length) {
            int length = Math.max(4, topLevelItems.length * 3 / 2);
            TreeItem[] newItems = new TreeItem[length];
            System.arraycopy(topLevelItems, 0, newItems, 0,
                    topLevelItems.length);
            topLevelItems = newItems;
        }

        if (topLevelItemsHandle == null) {
            topLevelItemsHandle = new int[4];
        }
        if (topLevelItemCount + 1 == topLevelItemsHandle.length) {
            int length = Math.max(4, topLevelItemsHandle.length * 3 / 2);
            int[] handles = new int[length];
            System.arraycopy(topLevelItemsHandle, 0, handles, 0,
                    topLevelItemsHandle.length);
            topLevelItemsHandle = handles;
        }
    }
    
    /**
     * Gets the index of an item.
     * <p>
     * -1 is returned if the item has a parent or the item is not found in the tree.
     * 
     * @param time
     *            the search item
     * @return the index of the item in the tree
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the item is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public int indexOf(TreeItem item){
        checkWidget();
        return getItemIndex(item);
    }

    boolean isTopLevelLastItemAndLastChildren() {
        boolean traverse = false;
        if (topLevelItemCount > 0) {
            TreeItem item = null;
            int currentItemHandle = OS.QTreeWidget_currentItem(topHandle);
            if ((topLevelItems[topLevelItemCount - 1].itemHandle == currentItemHandle)
                    && ((topLevelItems[topLevelItemCount - 1].childrenItemCount == 0) || 
                            !topLevelItems[topLevelItemCount - 1]
                                           .getExpanded())) {
                return true;
            } else if ((item = topLevelItems[topLevelItemCount - 1]
                                             .getItem(currentItemHandle)) != null) {
                if (item.isLastItemOfLastChild()
                        && (item.childrenItemCount == 0 || !item.getExpanded())) {
                    return true;
                }
            }
        }
        return traverse;
    }

    boolean qt_event_paint(int widgetHandle, int x, int y, int width,
            int height, int regionHandle) {
        if (widgetHandle == handle && ((style & SWT.VIRTUAL) != 0)
                && topLevelItemCount > 0) {

            int start = OS.QTreeWidget_swt_rowAt(topHandle,
                    (style & SWT.MIRRORED) != 0 ? OS.QWidget_rect(handle).width-x:x, y);

            //  If first item to draw is any one of the top level item
            if (start != -1) {
                int end = 0;
                int itemHeight = getItemHeight();
                
                if (expandedItemCount > 0) {
                    // number of top level items to draw. Here making partially visible items to fully drawn
                    end = start + (height % itemHeight == 0 ? height/itemHeight : height/itemHeight + 1) - 1;
                } else {
                    // if none of the items are expanded and top level item are
                    // less than the number of items to to draw.
                    end = Math.min(topLevelItemCount - 1, start + (height % itemHeight == 0 ? height/itemHeight :
                        height/itemHeight + 1) - 1);
                }

                for (int i = start; i <= end; i++) {

                    TreeItem item = _getItem(i);
                    checkData(item, i);
                    // expanded child items to draw.
                    if (item.getExpanded()) {
                        int left = setAndGetRemainingItemsToPaint(item, end - i);
                        end = i + left;
                    }
                    // Total number of visible items in the tree is less than the 
                    // items can draw even though after expanding some of the items 
                    if (i == topLevelItemCount - 1)
                        break;
                }

            } else if (start == -1) {

                // first Item to draw is not top level item 
                int firstItemHandle = OS.QTreeWidget_itemAt(topHandle,
                        (style & SWT.MIRRORED) != 0 ? OS.QWidget_rect(handle).width-x:x, y);
                int lastItemHandle = OS.QTreeWidget_itemAt(topHandle, 
                        (style & SWT.MIRRORED) != 0 ? OS.QWidget_rect(handle).width-x:x,
                        y + height);
                
                if(!(lastItemHandle==0 && firstItemHandle == 0)){
                    int parentItemHandle = 0;
                    // if the last item is partially visible or items are not enough to fill,
                    // then lastItemHandle is 0.
                    if(lastItemHandle==0){
                        parentItemHandle = OS.QTreeWidgetItem_swt_isSameParent(
                            firstItemHandle, firstItemHandle);
                    } else {
                        // if the first and last item has same parent, then 
                        //  returns some parentItemHandle otherwise returns -1
                        parentItemHandle = OS.QTreeWidgetItem_swt_isSameParent(
                            firstItemHandle, lastItemHandle);
                    }
                    
                    // First item and last item to draw has same parent or 
                    // items are not full or lastItem is partially visible 
                    if (parentItemHandle != -1) {
                        int begin = OS.QTreeWidgetItem_indexOfChild(
                            parentItemHandle, firstItemHandle);
                        int last = 0;

                        if(lastItemHandle == 0){
                            last = OS.QTreeWidget_swt_parentChildCount( firstItemHandle )-1;
                        } else {
                            last = OS.QTreeWidgetItem_indexOfChild(
                                parentItemHandle, lastItemHandle);
                        }
                    
                        TreeItem parentItem = findParentItem(parentItemHandle);
                        int itemHeight = getItemHeight();
                        int numItemsToFit = (height % itemHeight == 0 ? height/itemHeight :
                                height/itemHeight + 1);
                    
                        // First item and last item to draw has same parent
                        // inner level drawing of children
                        for (int a = begin; a <= last; a++) {
                            TreeItem item = parentItem._getItem(a);
                            checkData(item, a);
                            numItemsToFit = numItemsToFit - 1;
                            numItemsToFit = setAndGetRemainingItemsToPaint(item, numItemsToFit);
                        }

                        // This is fix to suddenly scrolling, firstItem to draw is some inner level item and 
                        // last item to draw is root item or nothing (no enough item to fit) 
                        TreeItem rootItem = null;
                        while (parentItem != null && numItemsToFit>0) {
                            rootItem = parentItem;
                            parentItem = rootItem.getParentItem();
                            
                            if(parentItem != null){
                                int index = OS.QTreeWidgetItem_indexOfChild(parentItem.internal_itemHandle(), 
                                            rootItem.internal_itemHandle());
                                int endIndex = parentItem.childrenItemCount - (index+1) > numItemsToFit ? numItemsToFit: 
                                        parentItem.childrenItemCount-1;
                                    
                                for (int a = index+1; a <= endIndex; a++) {
                                    TreeItem item = parentItem._getItem(a);
                                    checkData(item, a);
                                    numItemsToFit = numItemsToFit - 1;
                                    numItemsToFit = setAndGetRemainingItemsToPaint(item, numItemsToFit);
                                    if(numItemsToFit <= 0)
                                        break;
                                }
                            } else{
                                int index = OS.QTreeWidget_indexOfTopLevelItem(topHandle, 
                                            rootItem.internal_itemHandle());
                                int endIndex = topLevelItemCount - (index+1) > numItemsToFit ? numItemsToFit: 
                                        topLevelItemCount-1;
                                for (int a = index+1; a <= endIndex; a++) {
                                    TreeItem item = _getItem(a);
                                    checkData(item, a);
                                    numItemsToFit = numItemsToFit - 1;
                                    numItemsToFit = setAndGetRemainingItemsToPaint(item, numItemsToFit);
                                    if(numItemsToFit <= 0)
                                        break;
                                }
                            }
                        }
                    } else {

                        // First item and last item to draw has different parent
                        TreeItem firstItem = findParentItem(firstItemHandle);
                        TreeItem parentItem = firstItem.getParentItem();

                        int begin = OS.QTreeWidgetItem_indexOfChild(parentItem
                            .internal_itemHandle(), firstItemHandle);

                        TreeItem lastItem = findParentItem(lastItemHandle);
                        TreeItem itemFind = null;

                        //search for last item in the child of first item (under the first item), 
                        // child of first item siblings and siblings. And paint if some children 
                        // are expanded.(lower level search)
                        for (int i = begin; i < parentItem.childrenItemCount; i++) {
                            itemFind = parentItem._getItem(i);
                            checkData(itemFind, i);
                            if (itemFind == lastItem) 
                                break;
                        
                            if(itemFind.getExpanded())
                                itemFind = upperItemPaintAndSearch(itemFind, lastItem, -1);
                            if (itemFind == lastItem) 
                                break;
                        }

                        // if last item to paint is not child of first item or child of siblings or 
                        // siblings. do the following( upper level search )
                        if (itemFind != lastItem) {
                            TreeItem rootItem = null;
                            // search for last item to paint until next topLevel item
                            while (parentItem != null) {
                                rootItem = parentItem;
                                parentItem = parentItem.getParentItem();

                                if(parentItem != null){
                                    // current Parent is already searched and painted in the 
                                    // above for loop. so pass that parent index so that 
                                    // next search will start from next parent item
                                    int currParentIndex = OS.QTreeWidgetItem_indexOfChild(parentItem
                                        .internal_itemHandle(), rootItem.internal_itemHandle());

                                    itemFind = upperItemPaintAndSearch(parentItem, lastItem, currParentIndex);
                                }
                                if (itemFind == lastItem)
                                    break;
                            }

                            // search last item to paint in all remaining top level items and 
                            // their children.( last one to search)
                            if (itemFind != lastItem) {
                                int index = OS.QTreeWidget_indexOfTopLevelItem(
                                    topHandle, rootItem
                                    .internal_itemHandle());
                                // we need to search the next parent
                                for (int i = index+1; i < topLevelItemCount; i++) {
                                    TreeItem topItem = _getItem(i);
                                    checkData(topItem, i);
                                    if (topItem == lastItem)
                                        break;
                                    if(topItem.getExpanded())
                                        itemFind = upperItemPaintAndSearch(topItem, lastItem,-1);
                                    if (itemFind == lastItem)
                                        break;
                                }
                            }
                        }
                    }
                }
            }
        }
        return super.qt_event_paint(widgetHandle, x, y, width, height,
                regionHandle);
    }

    void qt_signal_tree_itemActivated(int selectionHandle, int column) {
        if (!ignoreSelectionEvent) {
            sendEvent(SWT.DefaultSelection);
        }
    }

    void qt_signal_tree_itemCollapsed(int selectionHandle) {
        --expandedItemCount;
        if (!ignoreExpandEvent) {
            sendEvent(SWT.Collapse);
        }
    }

    void qt_signal_tree_itemExpanded(int selectionHandle) {
        ++expandedItemCount;
        if (!ignoreExpandEvent) {
            sendEvent(SWT.Expand);
        }
        updateColumnWidth();
    }

    void qt_signal_tree_itemSelectionChanged() {
        if (!ignoreSelectionEvent) {
            sendEvent(SWT.Selection);
        }
    }

    void releaseChildren_pp(boolean destroy) {
        if (topLevelItems != null) {
            for (int i = topLevelItemCount-1; i >= 0; i--) {
                // Cannot use _getItem here, since in a VIRTUAL style Tree it may cause
                // creating a new TreeItem, which doesn't make sense when trying to release
                // all children.
                TreeItem item = topLevelItems[i];
                if(item != null && item.cached){
                    if (item != null && !item.isDisposed() ) {
                        item.childrenItemCount = 0;
                        item.childrenItems = null;
                        item.childrenItemsHandle = null;
                        item.itemHandle = 0;
                        item.parentItem = null;
                        item.tree = null;
                        item.cached = false;
                        item.release(destroy);
                    }
                }
            }
        }
        OS.QTreeWidget_clear(topHandle);
        super.releaseChildren_pp(destroy);
    }

    void releaseHandle_pp(){
        expandedItemCount = 0;
        topLevelItems = null;
        topLevelItemCount = 0;
        topLevelItemsHandle = null;
        super.releaseHandle_pp();
    }

    void remove(int start, int end) {
        checkWidget();
        if (start > end)
            return;
        if (!(0 <= start && start <= end && end < topLevelItemCount)) {
            error(SWT.ERROR_INVALID_RANGE);
        }

        int index = end;
        while (index >= start) {
            TreeItem item = _getItem(index);
            if (item != null && !item.isDisposed()) {
                item.release(false);
            }
            index--;
        }
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
        OS.QTreeWidget_clear(topHandle);
        expandedItemCount = 0;
        topLevelItems = null;
        topLevelItemCount = 0;
        topLevelItemsHandle = null;
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
        checkWidget();
        if (listener == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        eventTable.unhook(SWT.Selection, listener);
        eventTable.unhook(SWT.DefaultSelection, listener);
    }

    /**
     * Removes the listener from the collection of listeners who will be
     * notified when items in the receiver are expanded or collapsed..
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
     * @see TreeListener
     * @see #addTreeListener
     */
    public void removeTreeListener(TreeListener listener) {
        checkWidget();
        if (listener == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null)
            return;
        eventTable.unhook(SWT.Expand, listener);
        eventTable.unhook(SWT.Collapse, listener);
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
        if ((style & SWT.SINGLE) != 0)
            return;

        ignoreSelectionEvent = true;
        OS.QTreeView_selectAll(topHandle);
        ignoreSelectionEvent = false;
    }

    /*
     *  Its a recursive function executed only in virtual tree style.
     *  It takes tree item and number of items can paint as a parameters.
     *  It do two things, if the given tree item has more number of 
     *  expanded items under it, it paints only given parameter number
     *  of first available items and return 0. If it has less number
     *  of expanded tree items under it, it paints only available 
     *  tree items and return remaining count.
     */
    int setAndGetRemainingItemsToPaint(TreeItem item, int num) {
        int left = num;

        if (item.getExpanded()) {
            int count = Math.min(num, item.childrenItemCount);
            for (int a = 0; a < count; a++) {
                if (left > 0) {
                    TreeItem childItem = item._getItem(a);
                    checkData(childItem, a);
                    left = left - 1;

                    if (childItem.getExpanded()) {
                        left = setAndGetRemainingItemsToPaint(childItem, left);
                    }
                }
            }
        }

        return left;
    }

    /**
     * Sets the number of items contained in the receiver.
     * 
     * @param count
     *            the number of items
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     * 
     * @since 3.0
     */
    public void setItemCount(int count) {
        checkWidget();
        count = Math.max(0, count);
        if (count == topLevelItemCount)
            return;
        boolean isVirtual = (style & SWT.VIRTUAL) != 0;
        if (!isVirtual)
            setRedraw(false);
        int oldItemCount = topLevelItemCount;
        remove(count, topLevelItemCount - 1);
        int length = Math.max(4, (count + 3) / 4 * 4);
        TreeItem[] newItems = new TreeItem[length];
        if (topLevelItems == null) {
            topLevelItems = new TreeItem[count];
        }
        System.arraycopy(topLevelItems, 0, newItems, 0, topLevelItemCount);
        topLevelItems = newItems;
        if (isVirtual) {
            if (oldItemCount < count) {
                int[] handles = OS.QTreeWidget_addTopLevelItems(
                        topHandle, count - topLevelItemCount);
                int[] temp = new int[count];
                if (topLevelItemsHandle == null) {
                    topLevelItemsHandle = new int[count];
                }
                System.arraycopy(topLevelItemsHandle, 0, temp, 0,
                        topLevelItemCount);
                System.arraycopy(handles, 0, temp, topLevelItemCount,
                        handles.length);
                topLevelItemsHandle = temp;
            }
            topLevelItemCount = count;
        } else {
            for (int i = topLevelItemCount; i < count; i++) {
                new TreeItem(this, SWT.NONE, i);
            }
        }
        if (!isVirtual)
            setRedraw(true);
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
     *                </li> <li>ERROR_INVALID_ARGUMENT - if one of the items has
     *                been disposed</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     * 
     * @see Tree#deselectAll()
     */
    public void setSelection(TreeItem[] items) {
        checkWidget();
        if (items == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        ignoreSelectionEvent = true;
        OS.QAbstractItemView_clearSelection(topHandle);
        if (items.length == 0
                || (((style & SWT.SINGLE) != 0) && items.length > 1)) {
            return;
        }

        TreeItem[] selectedItems = new TreeItem[items.length];
        int validItemCount = 0;

        // Remove invalid items
        for (int i = 0; i < items.length; i++) {
            if (items[i] != null && !(items[i].isDisposed())) {
                if (items[i].getParent() == this) {
                    boolean found = false;
                    // Remove duplicates
                    for (int j = 0; j < validItemCount; j++) {
                        if (selectedItems[j].equals(items[i])) {
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        selectedItems[validItemCount] = items[i];
                        validItemCount++;
                    }
                }
            }
        }

        // Resize the array if there were some invalid items
        if (validItemCount < selectedItems.length) {
            TreeItem[] temp = new TreeItem[validItemCount];
            System.arraycopy(selectedItems, 0, temp, 0, validItemCount);
            selectedItems = temp;
        }

        for (int i = 0; i < selectedItems.length; i++) {
            if((style & SWT.VIRTUAL) != 0) {
                if(selectedItems[i].cached){
                    OS.QTreeWidget_setCurrentItem(topHandle,
                            selectedItems[i].internal_itemHandle());
                } 
                selectedItems[i].selection=true;
            } else {
                OS.QTreeWidget_setCurrentItem(topHandle,
                        selectedItems[i].internal_itemHandle());
            } 

        }
        ignoreSelectionEvent = false;
    }

    /**
     * Sets the item which is currently at the top of the receiver. This item
     * can change when items are expanded, collapsed, scrolled or new items are
     * added or removed.
     * 
     * @param item
     *            the item to be shown
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the item is null</li> <li>
     *                ERROR_INVALID_ARGUMENT - if the item has been disposed
     *                </li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     * 
     * @see Tree#getTopItem()
     * 
     * @since 2.1
     */
    public void setTopItem(TreeItem item) {
        checkWidget();
        _showItem(item, OS.QT_ABSTRACTITEMVIEW_SCROLLHINT_POSITIONATTOP);
    }

    void setTraversalFlags_pp(int type, int key, int modifier,
            int character) {
        traverseDoit = false;
        traverseCancel = false;
        if (type == SWT.TRAVERSE_RETURN) {
            // Selecting TreeItems is done with enter
            return;
        }

        switch (key) {
        case OS.QT_KEY_LEFT:
        case OS.QT_KEY_RIGHT:
            traverseDoit = false;
            break;
        case OS.QT_KEY_UP:
            // TopLevel first item
            if (topLevelItems[0].itemHandle == OS
                    .QTreeWidget_currentItem(topHandle)) {
                traverseDoit = true;
            }
            break;
        case OS.QT_KEY_DOWN:
            if (isTopLevelLastItemAndLastChildren()) {
                traverseDoit = true;
            }
            break;
        default:
            super.setTraversalFlags_pp(type, key, modifier, character);
        break;
        }
    }

    /**
     * Shows the item. If the item is already showing in the receiver, this
     * method simply returns. Otherwise, the items are scrolled and expanded
     * until the item is visible.
     * 
     * @param item
     *            the item to be shown
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the item is null</li> <li>
     *                ERROR_INVALID_ARGUMENT - if the item has been disposed
     *                </li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     * 
     * @see Tree#showSelection()
     */
    public void showItem(TreeItem item) {
        checkWidget();
        _showItem(item, OS.QT_ABSTRACTITEMVIEW_SCROLLHINT_ENSUREVISIBLE);
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
     * 
     * @see Tree#showItem(TreeItem)
     */
    public void showSelection() {
        // Shows first visible selected item in the Tree.
        checkWidget();
        TreeItem[] selection = getSelection();
        if (selection.length == 0)
            return;
        _showItem(selection[0], OS.QT_ABSTRACTITEMVIEW_SCROLLHINT_ENSUREVISIBLE);
    }

    void updateColumnWidth(){
        if (isDisposed()) return;
        OS.QTreeWidget_swt_setHeaderStretch(topHandle, false);
        OS.QTreeView_resizeColumnToContents(topHandle, 0);
    }

    /*
     *  Its a recursive function executed only in virtual tree style.
     *  It takes two tree items and one int as a parameter, first item to
     *   find expanded children under it and second tree item is that 
     *  we need to compare. This function try to search expanded items 
     *  under first parameter item and paint until it is equal to 
     *  second parameter item. if it doesn't find second parameter item 
     *  then it paints all expanded items under first parameter item and  
     *  returns last painted child item. int parameter is from where to 
     *  start the search 
     */
    TreeItem upperItemPaintAndSearch(TreeItem item, TreeItem itemToFind, int index) {
        TreeItem childItem = null;
        if (item != null) {
            if (item.getExpanded()) {
                for (int i = index+1; i < item.childrenItemCount; i++) {
                    childItem = item._getItem(i);
                    checkData(childItem, i);
                    if (childItem == itemToFind) {
                        break;
                    }
                    if (childItem.getExpanded()) {
                        childItem = upperItemPaintAndSearch(childItem, itemToFind,-1);
                    }
                    if (childItem == itemToFind) {
                        break;
                    }   
                }
            }
        }
        return childItem;
    }

}
