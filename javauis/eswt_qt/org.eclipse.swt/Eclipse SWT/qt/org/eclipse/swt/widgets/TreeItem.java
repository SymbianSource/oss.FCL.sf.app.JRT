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
import org.eclipse.swt.SWTException;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Internal_GfxPackageSupport;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.OS;

/**
 * Instances of this class represent a selectable user interface object that
 * represents a hierarchy of tree items in a tree widget.
 * 
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>(none)</dd>
 * <dt><b>Events:</b></dt>
 * <dd>(none)</dd>
 * </dl>
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */

public class TreeItem extends Item {
    static Tree checkNull(Tree control) {
        if (control == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        return control;
    }
    static TreeItem checkNull(TreeItem item) {
        if (item == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        return item;
    }

    Tree tree;
    int itemHandle;
    TreeItem parentItem;
    TreeItem[] childrenItems;

    int childrenItemCount;
    int[] childrenItemsHandle;
    Font font;

    Color background;
    Color foreground;
    boolean grayed;
    // Tells if the item has all the data fetched (can be false only in 
    // SWT.VIRTUAL style Tree)
    boolean cached;

    int checkedState;

    boolean selection;

    /**
     * Constructs a new instance of this class given its parent (which must be a
     * <code>Tree</code> or a <code>TreeItem</code>) and a style value
     * describing its behavior and appearance. The item is added to the end of
     * the items maintained by its parent.
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
     *            a tree control which will be the parent of the new instance
     *            (cannot be null)
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
     * @see SWT
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public TreeItem(Tree parent, int style) {
        this(parent, style, checkNull(parent).getItemCount(), true);
    }

    /**
     * Constructs a new instance of this class given its parent (which must be a
     * <code>Tree</code> or a <code>TreeItem</code>), a style value describing
     * its behavior and appearance, and the index at which to place it in the
     * items maintained by its parent.
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
     *            a tree control which will be the parent of the new instance
     *            (cannot be null)
     * @param style
     *            the style of control to construct
     * @param index
     *            the index to store the receiver in its parent
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
     * @see SWT
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public TreeItem(Tree parent, int style, int index) {
        this(parent, style, index, true);
    }

    TreeItem(Tree parent, int style, int index, boolean create) {
        super(parent, style);
        this.tree = parent;
        if( create ){
            itemHandle = OS.QTreeWidgetItem_new();
            OS.QTreeWidget_insertTopLevelItem(parent.topHandle,
                    itemHandle, index);
            parent.addTopLevelItem(this, index, itemHandle);
            ensureValid( index );
        }
    }

    /**
     * Constructs a new instance of this class given its parent (which must be a
     * <code>Tree</code> or a <code>TreeItem</code>) and a style value
     * describing its behavior and appearance. The item is added to the end of
     * the items maintained by its parent.
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or must
     * be built by <em>bitwise OR</em>'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. The class description lists the style constants that are
     * applicable to the class. Style bits are also inherited from superclasses.
     * </p>
     * 
     * @param parentItem
     *            a tree control which will be the parent of the new instance
     *            (cannot be null)
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
     * @see SWT
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public TreeItem(TreeItem parent, int style) {
        this(parent, style, checkNull(parent).getItemCount(), true);
    }

    /**
     * Constructs a new instance of this class given its parent (which must be a
     * <code>Tree</code> or a <code>TreeItem</code>), a style value describing
     * its behavior and appearance, and the index at which to place it in the
     * items maintained by its parent.
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or must
     * be built by <em>bitwise OR</em>'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. The class description lists the style constants that are
     * applicable to the class. Style bits are also inherited from superclasses.
     * </p>
     * 
     * @param parentItem
     *            a tree control which will be the parent of the new instance
     *            (cannot be null)
     * @param style
     *            the style of control to construct
     * @param index
     *            the index to store the receiver in its parent
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
     * @see SWT
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public TreeItem(TreeItem parent, int style, int index) {
        this(parent, style, index, true);
    }

    TreeItem(TreeItem parent, int style, int index, boolean create) {
        super(parent, style);
        this.parentItem = parent;
        this.tree = parentItem.tree;
        if( create ){
            itemHandle = OS.QTreeWidgetItem_new();

            OS.QTreeWidgetItem_insertChild(parent.internal_itemHandle(),
                    itemHandle, index);
            parentItem.addChild(this, index, itemHandle);
            ensureValid( index );
        }
    }

    TreeItem _getItem (int index) {
        if ((tree.getStyle() & SWT.VIRTUAL) == 0) return childrenItems [index];
        if (childrenItems [index] != null) return childrenItems [index];
        return childrenItems [index] = new TreeItem (this, SWT.NONE, index, false);
    }

    int _getItemHandle(){
        int newItemHandle = 0;
        TreeItem parentItem = getParentItem();
        if(parentItem!=null) {
            for(int i=0;i<parentItem.childrenItemCount;i++){
                if(this==parentItem.childrenItems[i]){ 
                    newItemHandle = parentItem.childrenItemsHandle[i];
                    break;
                }
            }
        } else {
            for(int i=0;i<tree.topLevelItemCount;i++){
                if(this==tree.topLevelItems[i]){ 
                    newItemHandle = tree.topLevelItemsHandle[i];
                    break;
                }
            }
        }

        return newItemHandle;
    }

    /* Add a new child to the TreeItem's list of children. Add the item at the
     * given index. An index of -1 implies that the child should be added to the
     * end of the TreeItem's list of children.
     */
    void addChild(TreeItem item, int index, int itemHandle) {

        increaseChildrenArraySize();
        if (!(0 <= index && index <= childrenItemCount)) 
            error (SWT.ERROR_INVALID_RANGE);

        // Put the new item in the correct place in item array
        System.arraycopy (childrenItems, index, childrenItems, 
                index + 1, childrenItemCount+1 - index);
        childrenItems[index] = item;

        //itemCount is already increase in the previous line.
        System.arraycopy (childrenItemsHandle, index, childrenItemsHandle,
                index + 1, childrenItemCount+1 - index);
        childrenItemsHandle[index] = itemHandle;

        childrenItemCount++;

    }

    protected void checkSubclass() {
        if (!isValidSubclass())
            error(SWT.ERROR_INVALID_SUBCLASS);
    }

    void clearSelection() {
        for (int i = 0; i < childrenItemCount; ++i) {
            TreeItem child = _getItem(i);
            child.selection=false;
            child.clearSelection();
        }
    }

    /*
     * Ensures that the TreeItem has all the native and Java side
     * resources created.
     * @param index Index of the item in Tree
     */
    void ensureValid(int index) {
        if (cached) {
            return;
        }
        cached = true;

        if(internal_itemHandle() == 0) {
            if(parentItem == null){
                itemHandle = tree.topLevelItemsHandle[index];
            } else {
                itemHandle = parentItem.childrenItemsHandle[index];
            }
        }

        if ((tree.getStyle() & SWT.CHECK) != 0) {
            OS.QTreeWidgetItem_setFlags(itemHandle, OS.QT_ITEMISENABLED
                    | OS.QT_ITEMISUSERCHECKABLE | OS.QT_ITEMISSELECTABLE);
            OS.QTreeWidgetItem_setCheckState(itemHandle, checkedState);
        } else {
            OS.QTreeWidgetItem_setFlags(itemHandle, OS.QT_ITEMISENABLED
                    | OS.QT_ITEMISSELECTABLE);
        }

        // If the user has set background, foreground or font for the whole item
        // set that here for the new column also            
        if (background != null) OS.QTreeWidgetItem_swt_setBackground(itemHandle, background.getRed(), background.getGreen(), background.getBlue(), false);
        if (foreground != null) OS.QTreeWidgetItem_swt_setForeground(itemHandle, foreground.getRed(), foreground.getGreen(), foreground.getBlue(), false);
        if (font != null) OS.QTreeWidgetItem_swt_setFont(itemHandle, font.handle);
        if (text != null) OS.QTreeWidgetItem_setText(itemHandle, text);
        if (image != null) OS.QTreeWidgetItem_setIcon(itemHandle, Internal_GfxPackageSupport.getIconHandle(image));

        tree.ignoreSelectionEvent = true;

        // this code will be executed when we call setSelection(TreeItem[]) in multi style. 
        // if the number of items going to select are 1 and style is single also works
        if(selection && ((tree.getStyle() & SWT.VIRTUAL) != 0)) {
            OS.QTreeWidgetItem_setSelected(internal_itemHandle(), selection);
            // we need to set the selection to false. bcz there is also possible
            //to deselect the item with mouse
            selection = false;
        }
        tree.ignoreSelectionEvent = false;
        tree.updateColumnWidth();
    }

    TreeItem findItem(int itemHandle) {
        TreeItem item = null;
        for (int i = 0; i < childrenItemCount; i++) {
            if (childrenItemsHandle[i] == itemHandle) {
                item = _getItem(i);
                break;
            } else {
                TreeItem childItem = _getItem(i);
                if(childItem.cached){
                    if(childItem.getExpanded())
                        item = childItem.findItem(itemHandle);
                }
                if (item != null)
                    break;
            }
        }
        return item;
    }

    /**
     * Returns the receiver's background color.
     * 
     * @return the background color
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @since 2.0
     * 
     */
    public Color getBackground() {
        checkWidget();
        if(cached) {
            if (!tree.checkData(this, getParentItem()))
                error(SWT.ERROR_WIDGET_DISPOSED);
        }

        if (background != null)
            return Internal_GfxPackageSupport.newColor(display, background.handle);

        return tree.getBackground();
    }

    /**
     * Returns a rectangle describing the receiver's size and location relative
     * to its parent.
     * 
     * @return the receiver's bounding rectangle
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public Rectangle getBounds() {
        checkWidget();
        if(cached){
            if (!tree.checkData(this, getParentItem()))
                error(SWT.ERROR_WIDGET_DISPOSED);
            return OS.QTreeWidget_visualItemRect(tree.topHandle,
                    internal_itemHandle());
        } else {
            return OS.QTreeWidget_visualItemRect(tree.topHandle, _getItemHandle());
        }
    }

    /**
     * Returns <code>true</code> if the receiver is checked, and false
     * otherwise. When the parent does not have the
     * <code>CHECK style, return false.
     * <p>
     * 
     * @return the checked state
     * 
     * @exception SWTException
     *                <ul> <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public boolean getChecked() {
        checkWidget();
        if(cached){
            if (!tree.checkData(this, getParentItem()))
                error(SWT.ERROR_WIDGET_DISPOSED);

            if ((tree.style & SWT.CHECK) == 0)
                return false;
            return OS.QTreeWidgetItem_checkState(internal_itemHandle()) == OS.QT_CHECKED;
        } else {
            if(checkedState == OS.QT_CHECKED) {
                return true;
            } else {
                return false;
            }
        }
    }

    /**
     * Gets the index of an item.
     * <p>
     * -1 is returned if the item is not found from children.
     * 
     * @param time
     *            the search item
     * @return the index of the item
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
    int getChildItemIndex(TreeItem item) {
        if (item == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        if (childrenItemCount == 0 || childrenItems == null
                || item.getParent() != tree)
            return -1;

        return OS.QTreeWidgetItem_indexOfChild(internal_itemHandle(), item.internal_itemHandle());
    }

    /**
     * Returns <code>true</code> if the receiver is expanded, and false
     * otherwise.
     * <p>
     * 
     * @return the expanded state
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public boolean getExpanded() {
        checkWidget();
        if(cached){
            if (!tree.checkData(this, getParentItem()))
                error(SWT.ERROR_WIDGET_DISPOSED);
            return OS.QTreeWidgetItem_isExpanded(internal_itemHandle());
        } else {
            return false;
        }
    }

    /**
     * Returns the font that the receiver will use to paint textual information
     * for this item.
     * 
     * @return the receiver's font
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
    public Font getFont() {
        checkWidget();
        if(cached){
            if (!tree.checkData(this, getParentItem()))
                error(SWT.ERROR_WIDGET_DISPOSED);
        }
        return (this.font == null) ? getParent().getFont() : this.font;
    }

    /**
     * Returns the foreground color that the receiver will use to draw.
     * 
     * @return the receiver's foreground color
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     * 
     * @since 2.0
     * 
     */
    public Color getForeground() {
        checkWidget();
        if(cached){
            if (!tree.checkData(this, getParentItem()))
                error(SWT.ERROR_WIDGET_DISPOSED);
        }

        if (foreground != null)
            return Internal_GfxPackageSupport.newColor(display, foreground.handle);

        return tree.getForeground();
    }

    /**
     * Returns <code>true</code> if the receiver is grayed, and false otherwise.
     * When the parent does not have the <code>CHECK style, return false.
     * <p>
     * 
     * @return the grayed state
     * 
     * @exception SWTException
     *                <ul> <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public boolean getGrayed() {
        checkWidget();
        if(cached){
            if (!tree.checkData(this, getParentItem()))
                error(SWT.ERROR_WIDGET_DISPOSED);
        }
        if ((tree.getStyle() & SWT.CHECK) == 0)
            return false;

        return grayed;
    }


    TreeItem getItem(int itemHandle) {
        TreeItem item = null;
        for (int i = 0; i < childrenItemCount; i++) {
            if (childrenItemsHandle[i] == itemHandle) {
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
     * item children of the receiver.
     * 
     * @return the number of items
     * 
     * @exception SWTException
     *                <ul> <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public int getItemCount() {
        checkWidget();
        return childrenItemCount;
    }

    /**
     * Returns a (possibly empty) array of <code>TreeItem</code>s which are the
     * direct item children of the receiver.
     * <p>
     * Note: This is not the actual structure used by the receiver to maintain
     * its list of items, so modifying the array will not affect the receiver.
     * </p>
     * 
     * @return the receiver's items
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
        if (childrenItemCount == 0)
            return new TreeItem[0];
        TreeItem[] items = new TreeItem[childrenItemCount];
        if ((tree.getStyle() & SWT.VIRTUAL) != 0) {
            for (int i = 0; i < childrenItemCount; i++) {
                items[i] = _getItem(i);
                tree.checkData(items[i], i);
            }
        } else {
            System.arraycopy(childrenItems, 0, items, 0, childrenItemCount);
        }
        return items;
    }

    /**
     * Returns the receiver's parent, which must be a <code>Tree</code>.
     * 
     * @return the receiver's parent
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public Tree getParent() {
        checkWidget();
        return tree;
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
        if(parentItem!=null){
            return parentItem;
        } else {
            return null;
        }
    }

    void increaseChildrenArraySize(){
        if (childrenItems == null) {
            childrenItems = new TreeItem[4];
        }
        if (childrenItemCount+1 == childrenItems.length) {
            int length = Math.max (4, childrenItems.length * 3 / 2);
            TreeItem [] newItems = new TreeItem [length]; 
            System.arraycopy (childrenItems, 0, newItems, 0, childrenItems.length);
            childrenItems = newItems;
        }

        if (childrenItemsHandle == null) {
            childrenItemsHandle = new int[4];
        }
        if (childrenItemCount+1 == childrenItemsHandle.length) {
            int length = Math.max (4, childrenItemsHandle.length * 3 / 2);
            int [] handles = new int[length]; 
            System.arraycopy (childrenItemsHandle, 0, handles, 0,
                    childrenItemsHandle.length);
            childrenItemsHandle = handles;
        }
    }

    public int indexOf(TreeItem item){
        checkWidget();
        return getChildItemIndex(item);
    }
    
    public int internal_itemHandle() {
        return itemHandle;
    }

    public boolean isDisposed() {
        if (tree != null && (tree.style & SWT.VIRTUAL) == 0) {
            if (itemHandle == 0)
                return true;
        }
        return super.isDisposed();
    }

    boolean isLastItemOfLastChild(){
        boolean traverse = false;
        if(parentItem != null ){
            if ((parentItem.childrenItems[parentItem.childrenItemCount-1]== this)){
                traverse = parentItem.isLastItemOfLastChild();
            } 
        } else {
            if (tree.topLevelItems[tree.topLevelItemCount-1] == this){
                traverse = true;
            } else {
                traverse = false;
            }
        }
        return traverse;
    }

    void releaseChildren_pp(boolean destroy) {
        if (childrenItems != null) {
            for (int i = childrenItemCount-1; i >= 0; i--) {
                // Don't use _getItem() here, in a VIRTUAL style Tree it would
                // cause children items to be created.
                TreeItem item = childrenItems[i];

                if (item != null && !item.isDisposed()) {
                    item.release(destroy);
                }
            }
        }
    }

    void releaseHandle_pp() {

        if(tree !=null){
            int newItemHandle = internal_itemHandle();
            if((tree.getStyle() & SWT.VIRTUAL)!=0 && !cached){
                newItemHandle = _getItemHandle();
            }

            if(cached || ((tree.getStyle() & SWT.VIRTUAL)!=0 && !cached)){
                if(getParentItem() != null){
                    int index = OS.QTreeWidgetItem_indexOfChild(parentItem.internal_itemHandle(), 
                            newItemHandle);

                    OS.QTreeWidgetItem_removeChild(parentItem.internal_itemHandle(), newItemHandle);

                    int[] newHandles = new int[parentItem.childrenItemsHandle.length - 1];
                    System.arraycopy (parentItem.childrenItemsHandle, 0, newHandles, 0, index);
                    System.arraycopy (parentItem.childrenItemsHandle, index + 1, newHandles,
                            index, newHandles.length - index);
                    parentItem.childrenItemsHandle = newHandles;

                    TreeItem[] newItems = new TreeItem [parentItem.childrenItems.length - 1];
                    System.arraycopy (parentItem.childrenItems, 0, newItems, 0, index);
                    System.arraycopy (parentItem.childrenItems, index + 1, newItems, index,
                            newItems.length - index);

                    parentItem.childrenItems = newItems;
                    parentItem.childrenItemCount--;
                } else {
                    int index = OS.QTreeWidget_indexOfTopLevelItem(tree.topHandle,
                            newItemHandle);

                    OS.QTreeWidget_takeTopLevelItem(tree.topHandle, index);

                    int[] newHandles = new int[tree.topLevelItemsHandle.length - 1];
                    System.arraycopy (tree.topLevelItemsHandle, 0, newHandles, 0, index);
                    System.arraycopy (tree.topLevelItemsHandle, index + 1, newHandles,
                            index, newHandles.length - index);
                    tree.topLevelItemsHandle = newHandles;

                    TreeItem[] newItems = new TreeItem [tree.topLevelItems.length - 1];
                    System.arraycopy (tree.topLevelItems, 0, newItems, 0, index);
                    System.arraycopy (tree.topLevelItems, index + 1, newItems, 
                            index, newItems.length - index);

                    tree.topLevelItems = newItems;
                    tree.topLevelItemCount--;
                }
            }
        }
        childrenItemCount = 0;
        childrenItems = null;
        childrenItemsHandle = null;
        itemHandle = 0;
        parentItem = null;
        tree = null;
        cached = false;
        super.releaseHandle_pp();
    }

    void releaseWidget_pp() {
        super.releaseWidget_pp();
        font = null;
        background = null;
        foreground = null;
        checkedState = OS.QT_UNCHECKED;
        selection = false;
    }

    void remove (int start, int end) {
        checkWidget();
        if (start > end) return;
        if (!(0 <= start && start <= end && end < childrenItemCount)) {
            error (SWT.ERROR_INVALID_RANGE);
        }

        int index = end;
        while (index >= start) {
            TreeItem item = _getItem(index);
            if (item != null && !item.isDisposed ()) {
                item.release(false);
            }
            index--;
        }
    }

    /**
     * Sets the receiver's background color to the color specified by the
     * argument, or to the default system color for the item if the argument is
     * null.
     * 
     * @param color
     *            the new color (or null)
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the argument has been
     *                disposed</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @since 2.0
     * 
     */
    public void setBackground(Color color) {
        checkWidget();
        if (color != null && color.isDisposed()) {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }

        if(color == null) {
            background = null;
        } else {
            // Store a copy
            background = Internal_GfxPackageSupport.newColor(display, color.handle);
        }

        if(cached){
            Color itemBg = getBackground();
            boolean restore =  (itemBg == null);
            int red = itemBg != null ? itemBg.getRed() : 0;
            int green = itemBg != null ? itemBg.getGreen() : 0;
            int blue = itemBg != null ? itemBg.getBlue() : 0;
            OS.QTreeWidgetItem_swt_setBackground(internal_itemHandle(), red, green, blue, restore);
        }
    }

    /**
     * Sets the checked state of the receiver.
     * <p>
     * 
     * @param checked
     *            the new checked state
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public void setChecked(boolean checked) {
        checkWidget();
        if ((tree.getStyle() & SWT.CHECK) == 0)
            return;
        checkedState = checked ? OS.QT_CHECKED : OS.QT_UNCHECKED;
        if(cached){
            OS.QTreeWidgetItem_setCheckState(internal_itemHandle(), checkedState);
        }
    }

    /**
     * Sets the expanded state of the receiver.
     * <p>
     * 
     * @param expanded
     *            the new expanded state
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public void setExpanded(boolean expanded) {
        checkWidget();
        if(cached){
            if (childrenItemCount == 0)
                return;
            tree.ignoreExpandEvent = true;
            OS.QTreeWidgetItem_setExpanded(internal_itemHandle(), expanded);
            tree.ignoreExpandEvent = false;
        }
    }

    /**
     * Sets the font that the receiver will use to paint textual information for
     * this item to the font specified by the argument, or to the default font
     * for that kind of control if the argument is null.
     * 
     * @param font
     *            the new font (or null)
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the argument has been
     *                disposed</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     * 
     * @since 3.0
     */
    public void setFont(Font font) {
        checkWidget();
        if (font != null && font.isDisposed())
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);

        Font oldFont = this.font;
        if (oldFont == font)
            return;
        this.font = font;
        if (oldFont != null && oldFont.equals(font))
            return;

        int fontHandle = font != null ? font.handle : 0;
        if(cached){
            OS.QTreeWidgetItem_swt_setFont(internal_itemHandle(), fontHandle);
            tree.updateColumnWidth();
        }
    }

    /**
     * Sets the receiver's foreground color to the color specified by the
     * argument, or to the default system color for the item if the argument is
     * null.
     * 
     * @param color
     *            the new color (or null)
     * 
     * @since 2.0
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the argument has been
     *                disposed</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     * 
     * @since 2.0
     * 
     */
    public void setForeground(Color color) {
        checkWidget();
        if (color != null && color.isDisposed()) {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }

        if(color == null) {
            foreground = null;
        } else {
            // Store a copy
            foreground = Internal_GfxPackageSupport.newColor(display, color.handle);
        }

        if(cached){
            Color itemFg = getForeground();
            boolean restore =  (itemFg == null);
            int red = itemFg != null ? itemFg.getRed() : 0;
            int green = itemFg != null ? itemFg.getGreen() : 0;
            int blue = itemFg != null ? itemFg.getBlue() : 0;
            OS.QTreeWidgetItem_swt_setForeground(internal_itemHandle(), red, green, blue, restore);
        }
    }

    /**
     * Sets the grayed state of the receiver.
     * <p>
     * 
     * @param grayed
     *            the new grayed state
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public void setGrayed(boolean grayed) {
        checkWidget();
        if ((tree.getStyle() & SWT.CHECK) == 0)
            return;
        this.grayed = grayed;
    }

    /**
     * Sets the receiver's image.
     * 
     * @param image
     *            the new image
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the image has been
     *                disposed</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     * 
     * @since 3.1
     */
    public void setImage(Image image) {
        checkWidget();
        if(cached){
            if (image != null && image.isDisposed())
                error(SWT.ERROR_INVALID_ARGUMENT);
            if (image != null) {
                OS.QTreeWidgetItem_setIcon(internal_itemHandle(), 
                        Internal_GfxPackageSupport.getIconHandle(image));
            } else {
                OS.QTreeWidgetItem_setIcon(internal_itemHandle(), 
                        Internal_GfxPackageSupport.getNullIconHandle());
            }
        }
        super.setImage(image);
    }

    /**
     * Sets the number of items contained in the receiver.
     *
     * @param count the number of items
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 3.0
     */
    public void setItemCount (int count) {
        checkWidget ();
        count = Math.max (0, count);
        if (count == childrenItemCount) return;
        boolean isVirtual = (tree.style & SWT.VIRTUAL) != 0;
        if (!isVirtual) tree.setRedraw (false);
        int oldItemCount = childrenItemCount;
        remove (count, childrenItemCount - 1);
        int length = Math.max (4, (count + 3) / 4 * 4);
        TreeItem [] newItems = new TreeItem [length];
        if(childrenItems == null){
            childrenItems = new TreeItem[count];
        }
        System.arraycopy (childrenItems, 0, newItems, 0, childrenItemCount);
        childrenItems = newItems;
        if (isVirtual) {
            if(oldItemCount<count){
                int newItemHandle = internal_itemHandle();
                if((tree.getStyle() & SWT.VIRTUAL)!=0 && !cached){
                    newItemHandle = _getItemHandle();
                }
                int[] handles = OS.QTreeWidgetItem_addChildren(newItemHandle,
                        count-childrenItemCount);
                int[] temp = new int[count];
                if(childrenItemsHandle == null){
                    childrenItemsHandle = new int[count];
                }
                System.arraycopy (childrenItemsHandle, 0, temp, 0, childrenItemCount);
                System.arraycopy (handles, 0, temp, childrenItemCount, handles.length);
                childrenItemsHandle = temp;
            }
            childrenItemCount = count;
        } else {
            for (int i=childrenItemCount; i<count; i++) {
                new TreeItem(this, SWT.NONE, i);
            }
        }
        if (!isVirtual) tree.setRedraw (true);
    }

    /**
     * Sets the receiver's text at a column
     * 
     * @param index
     *            the column index
     * @param string
     *            the new text
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the text is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     * 
     * @since 3.1
     */
    public void setText(String string) {
        checkWidget();
        if (string == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        if (string.equals(text))
            return;
        super.setText(string);
        if(cached) {
            OS.QTreeWidgetItem_setText(this.itemHandle, string);
            tree.updateColumnWidth();
        }
    }
}
