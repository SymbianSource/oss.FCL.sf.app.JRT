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
import java.util.Vector;

/**
 * Instances of this class are user interface objects that contain
 * menu items.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>BAR, DROP_DOWN, POP_UP, NO_RADIO_GROUP</dd>
 * <dd>LEFT_TO_RIGHT, RIGHT_TO_LEFT</dd>
 * <dt><b>Events:</b></dt>
 * <dd>Hide, Show </dd>
 * </dl>
 * <p>
 * Note: Only one of BAR, DROP_DOWN and POP_UP may be specified.
 * Only one of LEFT_TO_RIGHT or RIGHT_TO_LEFT may be specified.
 * </p><p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */

public class Menu extends Widget
{
    public int handle;
    Decorations parent;
    Vector items;
    MenuItem defaultItem;
    MenuItem parentCascadeMenuItem;
    Control control;
    /**
     * Constructs a new instance of this class given its parent,
     * and sets the style for the instance so that the instance
     * will be a popup menu on the given parent's shell.
     *
     * @param parent a control which will be the parent of the new instance (cannot be null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     *
     * @see SWT#POP_UP
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public Menu(Control parent)
    {
        this(errorIfNull(parent).menuDecorations(), SWT.POP_UP);
    }

    /**
     * Constructs a new instance of this class given its parent
     * (which must be a <code>Decorations</code>) and a style value
     * describing its behavior and appearance.
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
     * @param parent a decorations control which will be the parent of the new instance (cannot be null)
     * @param style the style of menu to construct
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     *
     * @see SWT#BAR
     * @see SWT#DROP_DOWN
     * @see SWT#POP_UP
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public Menu(Decorations parent, int style)
    {
        super(parent, checkStyle(style));
        this.parent = parent;
        checkOrientation(parent);
        createWidget();
    }

    /**
     * Constructs a new instance of this class given its parent
     * (which must be a <code>Menu</code>) and sets the style
     * for the instance so that the instance will be a drop-down
     * menu on the given parent's parent.
     *
     * @param parentMenu a menu which will be the parent of the new instance (cannot be null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     *
     * @see SWT#DROP_DOWN
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public Menu(Menu parentMenu)
    {
        this(errorIfNull(parentMenu).parent, SWT.DROP_DOWN);
    }

    /**
     * Constructs a new instance of this class given its parent
     * (which must be a <code>MenuItem</code>) and sets the style
     * for the instance so that the instance will be a drop-down
     * menu on the given parent's parent menu.
     *
     * @param parentItem a menu item which will be the parent of the new instance (cannot be null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     *
     * @see SWT#DROP_DOWN
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public Menu(MenuItem parentItem)
    {
        this(errorIfNull(parentItem).parent);
    }

    void createWidget()
    {
        handle = OS.Menu_New(this, /*parent.compositeHandle,*/ style);
        parent.addMenu(this);
    }

    static Control errorIfNull(Control control)
    {
        if (control == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        return control;
    }

    static Menu errorIfNull(Menu menu)
    {
        if (menu == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        return menu;
    }

    static MenuItem errorIfNull(MenuItem menuItem)
    {
        if (menuItem == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        return menuItem;
    }

    static int checkStyle(int style)
    {
        return internal_checkBits(style, SWT.POP_UP, SWT.BAR, SWT.DROP_DOWN, 0, 0, 0);
    }

    void addItem(MenuItem item, int index)
    {
        if (items == null)
        {
            items = new Vector();
        }
        // ArrayIndexOutOfBoundsException shouldn't be thrown as range is already checked by MenuItem
        items.insertElementAt(item, index);
    }

    void removeItem(MenuItem item)
    {
        items.removeElement(item);
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notified when menus are hidden or shown, by sending it
     * one of the messages defined in the <code>MenuListener</code>
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
     * @see MenuListener
     * @see #removeMenuListener
     */
    public void addMenuListener(MenuListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Hide,typedListener);
        addListener(SWT.Show,typedListener);
    }

    /**
     * Returns the default menu item or null if none has
     * been previously set.
     *
     * @return the default menu item.
     *
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public MenuItem getDefaultItem()
    {
        checkWidget();
        return defaultItem;
    }

    /**
     * Returns <code>true</code> if the receiver is enabled, and
     * <code>false</code> otherwise. A disabled control is typically
     * not selectable from the user interface and draws with an
     * inactive or "grayed" look.
     *
     * @return the receiver's enabled state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #isEnabled
     */
    public boolean getEnabled()
    {
        checkWidget();
        // NTH: Command should use this too
        return (state & DISABLED) == 0;
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
    public MenuItem getItem(int index)
    {
        checkWidget();
        if (index < 0 || index >= getItemCount()) internal_error(SWT.ERROR_INVALID_RANGE);
        return (MenuItem)items.elementAt(index);
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
        if (items != null)
            return items.size();
        else
            return 0;
    }

    /**
     * Returns an array of <code>MenuItem</code>s which are the items
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
    public MenuItem [] getItems()
    {
        checkWidget();
        if (items != null)
        {
            MenuItem [] copyOfItems = new MenuItem[items.size()];
            items.copyInto(copyOfItems);
            return copyOfItems;
        }
        else
        {
            return new MenuItem[0];
        }
    }

    /**
     * Returns the receiver's parent, which must be a <code>Decorations</code>.
     *
     * @return the receiver's parent
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Decorations getParent()
    {
        checkWidget();
        return parent;
    }

    /**
     * Returns the receiver's parent item, which must be a
     * <code>MenuItem</code> or null when the receiver is a
     * root.
     *
     * @return the receiver's parent item
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public MenuItem getParentItem()
    {
        checkWidget();
        return parentCascadeMenuItem;
    }

    /**
     * Returns the receiver's parent item, which must be a
     * <code>Menu</code> or null when the receiver is a
     * root.
     *
     * @return the receiver's parent item
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Menu getParentMenu()
    {
        checkWidget();
        if (parentCascadeMenuItem != null)
            return parentCascadeMenuItem.parent;
        else
            return null;
    }

    /**
     * Returns the receiver's shell. For all controls other than
     * shells, this simply returns the control's nearest ancestor
     * shell. Shells return themselves, even if they are children
     * of other shells.
     *
     * @return the receiver's shell
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #getParent
     */
    public Shell getShell()
    {
        checkWidget();
        return parent.getShell();
    }

    /**
     * Returns <code>true</code> if the receiver is visible, and
     * <code>false</code> otherwise.
     * <p>
     * If one of the receiver's ancestors is not visible or some
     * other condition makes the receiver not visible, this method
     * may still indicate that it is considered visible even though
     * it may not actually be showing.
     * </p>
     *
     * @return the receiver's visibility state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean getVisible()
    {
        checkWidget();
        return (state & HIDDEN) == 0;
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
     *    <li>ERROR_MENU_NOT_BAR - if menu does not have BAR style</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int indexOf(MenuItem item)
    {
        checkWidget();
        if (item == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (item.isDisposed()) internal_error(SWT.ERROR_INVALID_ARGUMENT);
        if (item.parent != this) return -1;
        if (items != null)
            return items.indexOf(item);
        else
            return -1;
    }

// from Widget
    protected void internal_prepareForRelease()
    {
        if (parentCascadeMenuItem != null)
            parentCascadeMenuItem.setMenu(null);

        parent.removeMenu(this);

        if (control != null)
        {
            control.setMenu(null);
        }
    }

    protected void internal_releaseHandle()
    {
        if (handle != 0)
        {
            OS.Menu_Dispose(handle);
            handle = 0;
        }
        super.internal_releaseHandle();
    }

    protected void internal_releaseResources()
    {
        // Release children
        if (items != null)
        {
            int count = items.size();
            for (int i=0; i<count; ++i)
            {
                MenuItem item = (MenuItem)items.elementAt(i);
                if (!item.isDisposed()) item.internal_release();
            }
            items.removeAllElements();
            items = null;
        }

        parent = null;
        parentCascadeMenuItem = null;

        defaultItem = null;

        super.internal_releaseResources();
    }

    /**
     * Returns <code>true</code> if the receiver is enabled and all
     * of the receiver's ancestors are enabled, and <code>false</code>
     * otherwise. A disabled control is typically not selectable from the
     * user interface and draws with an inactive or "grayed" look.
     *
     * @return the receiver's enabled state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #getEnabled
     */
    public boolean isEnabled()
    {
        checkWidget();
        boolean result = getEnabled();
        if (result != true) return false;
        Menu parentMenu = getParentMenu();
        if (parentMenu == null)
            return true;
        else
            return parentMenu.isEnabled();
    }

    /**
     * Returns <code>true</code> if the receiver is visible and all
     * of the receiver's ancestors are visible and <code>false</code>
     * otherwise.
     *
     * @return the receiver's visibility state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #getVisible
     */
    public boolean isVisible()
    {
        checkWidget();
        boolean result = getVisible();
        if (result != true) return false;
        if (this.getParent() != null)
        {
            result = getParent().isVisible();
        }
        return true;
    }

    /**
     * Removes the listener from the collection of listeners who will
     * be notified when the menu events are generated for the control.
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
     * @see MenuListener
     * @see #addMenuListener
     */
    public void removeMenuListener(MenuListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook(SWT.Hide, listener);
        eventTable.unhook(SWT.Show, listener);
    }

    void sendShowEvent()
    {
        internal_sendEvent(SWT.Show);
    }

    /**
     * Sets the default menu item to the argument or removes
     * the default emphasis when the argument is <code>null</code>.
     *
     * @param item the default menu item or null
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the menu item has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setDefaultItem(MenuItem item)
    {
        checkWidget();
        if (item != null && item.isDisposed()) internal_error(SWT.ERROR_INVALID_ARGUMENT);
        defaultItem = item;
        // SetDefaultItem only works with submenu currently
        OS.Menu_SetDefaultItem(handle, item != null ? item.handle : 0);
    }

    /**
     * Enables the receiver if the argument is <code>true</code>,
     * and disables it otherwise. A disabled control is typically
     * not selectable from the user interface and draws with an
     * inactive or "grayed" look.
     *
     * @param enabled the new enabled state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setEnabled(boolean enabled)
    {
        checkWidget();
        if (getEnabled() != enabled)
        {
            if (!enabled)
                state |= DISABLED;
            else
                state &= ~DISABLED;
            OS.Menu_SetEnabled(handle, enabled);
        }
    }

    void setExclusiveParentCascadeMenuItem(MenuItem item)
    {
        if (parentCascadeMenuItem != null)
            parentCascadeMenuItem.dropDownMenu = null;

        parentCascadeMenuItem = item;
    }

    /**
     * Sets the receiver's location to the point specified by
     * the arguments which are relative to the display.
     * <p>
     * Note:  This is different from most widgets where the
     * location of the widget is relative to the parent.
     * </p>
     *
     * @param x the new x coordinate for the receiver
     * @param y the new y coordinate for the receiver
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setLocation(int x, int y)
    {
        checkWidget();

        // Some Symbian devices may allow free setting of popup menus coordinates
        OS.Menu_SetLocation(handle, x, y);
    }

    /**
     * Sets the receiver's location to the point specified by
     * the arguments which are relative to the display.
     * <p>
     * Note:  This is different from most widgets where the
     * location of the widget is relative to the parent.
     * </p>
     *
     * @param location the new location for the receiver
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the point is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 2.1
     */
    public void setLocation(Point location)
    {
        if (location == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        setLocation(location.x, location.y);
    }

    /**
     * Marks the receiver as visible if the argument is <code>true</code>,
     * and marks it invisible otherwise.
     * <p>
     * If one of the receiver's ancestors is not visible or some
     * other condition makes the receiver not visible, marking
     * it visible may not actually cause it to be displayed.
     * </p>
     *
     * @param visible the new visibility state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setVisible(boolean visible)
    {
        checkWidget();
        if (visible)
        {
            state &= ~HIDDEN;
        }
        else
        {
            state |= HIDDEN;
        }
        OS.Menu_SetVisible(handle, visible);
    }

//This is called when a control is reparented
    void setParent(Decorations parent)
    {
        if (items != null)
        {
            int count = items.size();
            for (int i=0; i<count; ++i)
            {
                MenuItem item = (MenuItem)items.elementAt(i);
                item.setParentOfDropDownMenu(parent);
            }
        }
        this.parent.removeMenu(this);
        parent.addMenu(this);
        this.parent = parent;
    }

//This is called when a menu is set on a control
    void setControl(Control control)
    {
        this.control = control;
    }
}
