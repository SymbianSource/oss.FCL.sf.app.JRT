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

import java.util.Vector;

import org.eclipse.ercp.swt.expanded.internal.OS;
import org.eclipse.swt.*;
import org.eclipse.swt.graphics.*;

/**
 * Instances of this class represent a selectable user interface object
 * that represents a hierarchy of tree items in a tree widget.
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
public class TreeItem extends Item
{
    int handle;
    Tree parentTree;
    TreeItem parentItem;
    Vector items = new Vector();
    boolean grayed;
    Color foreground = null;
    Color background = null;
    Font font = null;

    /**
     * Constructs a new instance of this class given its parent
     * (which must be a <code>Tree</code> or a <code>TreeItem</code>)
     * and a style value describing its behavior and appearance.
     * The item is added to the end of the items maintained by its parent.
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
     * @param parent a tree control which will be the parent of the new instance (cannot be null)
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
     * @see SWT
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public TreeItem(Tree parent, int style)
    {
        super(parent, style);

        parentTree = parent;
        parentItem = null;
        handle = OS.Tree_Add(parentTree.treeHandle, 0);
        parentTree.items.addElement(this);
    }
    /**
     * Constructs a new instance of this class given its parent
     * (which must be a <code>Tree</code> or a <code>TreeItem</code>),
     * a style value describing its behavior and appearance, and the index
     * at which to place it in the items maintained by its parent.
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
     * @param parent a tree control which will be the parent of the new instance (cannot be null)
     * @param style the style of control to construct
     * @param index the zero-relative index to store the receiver in its parent
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     *    <li>ERROR_INVALID_RANGE - if the index is not between 0 and the number of elements in the parent (inclusive)</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     *
     * @see SWT
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public TreeItem(Tree parent, int style, int index)
    {
        super(parent, style, index);

        if (!(0 <= index && index <= parent.items.size())) error(SWT.ERROR_INVALID_RANGE);

        parentTree = parent;
        parentItem = null;
        if (index == parent.items.size())
        {
            handle = OS.Tree_Add(parentTree.treeHandle, 0);
        }
        else
        {
            handle = OS.Tree_Insert(parentTree.treeHandle, 0, index);
        }
        parentTree.items.insertElementAt(this, index);
    }

    /**
     * Constructs a new instance of this class given its parent
     * (which must be a <code>Tree</code> or a <code>TreeItem</code>)
     * and a style value describing its behavior and appearance.
     * The item is added to the end of the items maintained by its parent.
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
     * @param parentItem a tree control which will be the parent of the new instance (cannot be null)
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
     * @see SWT
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public TreeItem(TreeItem parentItem, int style)
    {
        super(parentItem, style);

        this.parentItem = parentItem;
        parentTree = parentItem.parentTree;
        handle = OS.Tree_Add(parentTree.treeHandle, this.parentItem.handle);
        this.parentItem.items.addElement(this);
    }

    /**
     * Constructs a new instance of this class given its parent
     * (which must be a <code>Tree</code> or a <code>TreeItem</code>),
     * a style value describing its behavior and appearance, and the index
     * at which to place it in the items maintained by its parent.
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
     * @param parentItem a tree control which will be the parent of the new instance (cannot be null)
     * @param style the style of control to construct
     * @param index the zero-relative index to store the receiver in its parent
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     *    <li>ERROR_INVALID_RANGE - if the index is not between 0 and the number of elements in the parent (inclusive)</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     *
     * @see SWT
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public TreeItem(TreeItem parentItem, int style, int index)
    {
        super(parentItem, style, index);

        if (!(0 <= index && index <= parentItem.items.size())) error(SWT.ERROR_INVALID_RANGE);

        this.parentItem = parentItem;
        parentTree = parentItem.parentTree;
        if (index == parentItem.items.size())
        {
            handle = OS.Tree_Add(parentTree.treeHandle, this.parentItem.handle);
        }
        else
        {
            handle = OS.Tree_Insert(parentTree.treeHandle, this.parentItem.handle, index);
        }
        this.parentItem.items.insertElementAt(this, index);
    }

    protected void checkSubclass()
    {
        if (!internal_isValidSubclass()) error(SWT.ERROR_INVALID_SUBCLASS);
    }

    /**
     * Returns the receiver's background color.
     *
     * @return the background color
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 2.0
     *
     */
    public Color getBackground()
    {
        checkWidget();
        if (background != null) return background;
        return parentTree.getBackground();
    }

    /**
     * Returns a rectangle describing the receiver's size and location
     * relative to its parent.
     *
     * @return the receiver's bounding rectangle
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Rectangle getBounds()
    {
        checkWidget();
        return OS.Tree_Bounds(parentTree.treeHandle, handle);
    }

    /**
     * Returns <code>true</code> if the receiver is checked,
     * and false otherwise.  When the parent does not have
     * the <code>CHECK style, return false.
     * <p>
     *
     * @return the checked state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean getChecked()
    {
        checkWidget();
        return OS.Tree_IsSelected(parentTree.treeHandle, handle);
    }

    /**
     * Returns <code>true</code> if the receiver is expanded,
     * and false otherwise.
     * <p>
     *
     * @return the expanded state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean getExpanded()
    {
        checkWidget();
        return OS.Tree_IsExpanded(parentTree.treeHandle, handle);
    }
    /**
     * Returns the font that the receiver will use to paint textual information for this item.
     *
     * @return the receiver's font
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 3.0
     */
    public Font getFont()
    {
        checkWidget();
        if (font != null) return font;
        return parentTree.getFont();
    }

    /**
     * Returns the foreground color that the receiver will use to draw.
     *
     * @return the receiver's foreground color
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 2.0
     *
     */
    public Color getForeground()
    {
        checkWidget();
        if (foreground != null) return foreground;
        return parentTree.getForeground();
    }

    /**
     * Returns <code>true</code> if the receiver is grayed,
     * and false otherwise. When the parent does not have
     * the <code>CHECK style, return false.
     * <p>
     *
     * @return the grayed state of the checkbox
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean getGrayed()
    {
        checkWidget();
        if ((parentTree.getStyle() & SWT.CHECK) == 0)
        {
            return false;
        }
        return grayed;
    }

    /**
     * Returns the number of items contained in the receiver
     * that are direct item children of the receiver.
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
     * Returns a (possibly empty) array of <code>TreeItem</code>s which
     * are the direct item children of the receiver.
     * <p>
     * Note: This is not the actual structure used by the receiver
     * to maintain its list of items, so modifying the array will
     * not affect the receiver.
     * </p>
     *
     * @return the receiver's items
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public TreeItem [] getItems()
    {
        checkWidget();
        int count = items.size();
        TreeItem[] result = new TreeItem [count];
        items.copyInto(result);
        return result;
    }

    /**
     * Returns the receiver's parent, which must be a <code>Tree</code>.
     *
     * @return the receiver's parent
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Tree getParent()
    {
        checkWidget();
        return parentTree;
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
        return parentItem;
    }

    protected void internal_prepareForRelease()
    {
        OS.Tree_Remove(parentTree.treeHandle, handle);
        super.internal_prepareForRelease();
    }

    protected void internal_releaseHandle()
    {
        handle = 0;
        // Native item already removed in OS.Tree_Remove
        super.internal_releaseHandle();
    }

    protected void internal_releaseResources()
    {
        if (parentItem != null)
        {
            parentItem.items.removeElement(this);
        }
        else
        {
            parentTree.items.removeElement(this);
        }
        super.internal_releaseResources();
    }

    TreeItem itemFromHandle(int id)
    {
        if (id == handle) return this;
        for (int i = 0; i < items.size(); i++)
        {
            TreeItem item = (TreeItem)items.elementAt(i);
            TreeItem res = item.itemFromHandle(id);
            if (res != null) return res;
        }
        return null;
    }

    /**
     * Sets the receiver's background color to the color specified
     * by the argument, or to the default system color for the item
     * if the argument is null.
     *
     * @param color the new color (or null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the argument has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 2.0
     *
     */
    public void setBackground(Color value)
    {
        checkWidget();
        if (value != null && value.isDisposed())
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        background = value;
    }

    /**
     * Sets the checked state of the receiver.
     * <p>
     *
     * @param checked the new checked state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setChecked(boolean value)
    {
        checkWidget();
        if ((parentTree.getStyle() & SWT.CHECK) == 0) return;
        OS.Tree_Check(parentTree.treeHandle, handle, value);
    }

    /**
     * Sets the expanded state of the receiver.
     * <p>
     *
     * @param expanded the new expanded state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setExpanded(boolean value)
    {
        checkWidget();
        if (value)
        {
            OS.Tree_Expand(parentTree.treeHandle, handle);
        }
        else
        {
            OS.Tree_Collapse(parentTree.treeHandle, handle);
        }
    }

    /**
     * Sets the font that the receiver will use to paint textual information
     * for this item to the font specified by the argument, or to the default font
     * for that kind of control if the argument is null.
     *
     * @param font the new font (or null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the argument has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 3.0
     */
    public void setFont(Font value)
    {
        checkWidget();
        if (value != null && value.isDisposed())
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        font = value;
    }

    /**
     * Sets the receiver's foreground color to the color specified
     * by the argument, or to the default system color for the item
     * if the argument is null.
     *
     * @param color the new color (or null)
     *
     * @since 2.0
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the argument has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 2.0
     *
     */
    public void setForeground(Color value)
    {
        checkWidget();
        if (value != null && value.isDisposed())
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        foreground = value;
    }

    /**
     * Sets the grayed state of the checkbox for this item.  This state change
     * only applies if the Tree was created with the SWT.CHECK style.
     *
     * @param grayed the new grayed state of the checkbox
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setGrayed(boolean value)
    {
        checkWidget();
        grayed = value;
    }

    /**
     * Sets the receiver's image.
     *
     * @param image the new image
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the image has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 3.1
     */
    public void setImage(Image value)
    {
        checkWidget();
        if (value != null && value.isDisposed()) error(SWT.ERROR_INVALID_ARGUMENT);
        if (value != null && value.equals(image)) return;
        super.setImage(value);
        OS.Tree_SetImage(parentTree.treeHandle, handle, value != null ? value.handle : 0);
    }

    /**
     * Sets the receiver's text at a column
     *
     * @param index the column index
     * @param string the new text
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the text is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 3.1
     */
    public void setText(String value)
    {
        checkWidget();
        if (value == null) error(SWT.ERROR_NULL_ARGUMENT);
        if (value.equals(text)) return;
        super.setText(value);
        OS.Tree_SetText(parentTree.treeHandle, handle, text);
    }

}
