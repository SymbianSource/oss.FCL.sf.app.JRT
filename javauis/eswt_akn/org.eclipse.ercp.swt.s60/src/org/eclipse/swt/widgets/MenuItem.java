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

import org.eclipse.swt.*;
import org.eclipse.swt.graphics.*;
import org.eclipse.swt.internal.symbian.OS;
import org.eclipse.swt.events.*;

/**
 * Instances of this class represent a selectable user interface object
 * that issues notification when pressed and released.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>CHECK, CASCADE, PUSH, RADIO, SEPARATOR</dd>
 * <dt><b>Events:</b></dt>
 * <dd>Selection</dd>
 * </dl>
 * <p>
 * Note: Only one of the styles CHECK, CASCADE, PUSH, RADIO and SEPARATOR
 * may be specified.
 * </p><p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */

public class MenuItem extends Item
{
    public int handle;
    Menu parent, dropDownMenu;
    int accelerator;

    /**
     * Constructs a new instance of this class given its parent
     * (which must be a <code>Menu</code>) and a style value
     * describing its behavior and appearance. The item is added
     * to the end of the items maintained by its parent.
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
     * @param parent a menu control which will be the parent of the new instance (cannot be null)
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
     * @see SWT#CHECK
     * @see SWT#CASCADE
     * @see SWT#PUSH
     * @see SWT#RADIO
     * @see SWT#SEPARATOR
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public MenuItem(Menu parent, int style)
    {
        this(parent, style, errorIfNull(parent).getItemCount());
    }

    /**
     * Constructs a new instance of this class given its parent
     * (which must be a <code>Menu</code>), a style value
     * describing its behavior and appearance, and the index
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
     * @param parent a menu control which will be the parent of the new instance (cannot be null)
     * @param style the style of control to construct
     * @param index the index to store the receiver in its parent
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
     * @see SWT#CHECK
     * @see SWT#CASCADE
     * @see SWT#PUSH
     * @see SWT#RADIO
     * @see SWT#SEPARATOR
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public MenuItem(Menu parent, int style, int index)
    {
        super(parent, checkStyle(style, parent));
        if (index < 0 || index > parent.getItemCount()) internal_error(SWT.ERROR_INVALID_RANGE);
        this.parent = parent;
        // SWT.LEFT_TO_RIGHT and SWT.RIGHT_TO_LEFT are not valid values for style of MenuItem
        // checkOrientation(parent);
        createWidget(index);
    }

    void createWidget(int index)
    {
        handle = OS.MenuItem_New(this, parent.handle, index, style);
        parent.addItem(this, index);
    }

    static Menu errorIfNull(Menu menu)
    {
        if (menu == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        return menu;
    }

    static int checkStyle(int style, Menu parent)
    {
        int checkedStyle = internal_checkBits(style, SWT.PUSH, SWT.CHECK, SWT.RADIO, SWT.SEPARATOR, SWT.CASCADE, 0);
        // If NO_RADIO_GROUP then RADIO behaves like CHECK (means RADIO becomes CHECK on Symbian as this feature doesn't exist on native menus)
        if (((parent.getStyle() & SWT.NO_RADIO_GROUP) != 0) && ((checkedStyle & SWT.RADIO) != 0))
        {
            checkedStyle &= ~SWT.RADIO;
            checkedStyle |= SWT.CHECK;
        }
        return checkedStyle;
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notified when the control is selected, by sending
     * it one of the messages defined in the <code>SelectionListener</code>
     * interface.
     * <p>
     * When <code>widgetSelected</code> is called, the stateMask field of the event object is valid.
     * <code>widgetDefaultSelected</code> is not called.
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
    }

    protected void checkSubclass()
    {
        if (!Display.isValidClass(getClass()))internal_error(SWT.ERROR_INVALID_SUBCLASS);
    }
    /**
     * Return the widget accelerator.  An accelerator is the bit-wise
     * OR of zero or more modifier masks and a key. Examples:
     * <code>SWT.CONTROL | SWT.SHIFT | 'T', SWT.ALT | SWT.F2</code>.
     * The default value is zero, indicating that the menu item does
     * not have an accelerator.
     *
     * @return the accelerator
     *
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getAccelerator()
    {
        checkWidget();
        return accelerator;
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
        return (state & DISABLED) == 0;
    }

    /**
     * Returns the receiver's cascade menu if it has one or null
     * if it does not. Only <code>CASCADE</code> menu items can have
     * a pull down menu. The sequence of key strokes, button presses
     * and/or button releases that are used to request a pull down
     * menu is platform specific.
     *
     * @return the receiver's menu
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Menu getMenu()
    {
        checkWidget();
        return dropDownMenu;
    }

    /**
     * Returns the receiver's parent, which must be a <code>Menu</code>.
     *
     * @return the receiver's parent
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Menu getParent()
    {
        checkWidget();
        return parent;
    }

    /**
     * Returns <code>true</code> if the receiver is selected,
     * and false otherwise.
     * <p>
     * When the receiver is of type <code>CHECK</code> or <code>RADIO</code>,
     * it is selected when it is checked.
     *
     * @return the selection state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean getSelection()
    {
        checkWidget();
        if ((style & (SWT.CHECK | SWT.RADIO)) != 0)
            return OS.MenuItem_GetSelection(handle);
        else
            return false;
    }

//from Widget
    protected void internal_prepareForRelease()
    {
        parent.removeItem(this);
    }

    protected void internal_releaseHandle()
    {
        if (handle != 0)
        {
            OS.MenuItem_Dispose(handle);
            handle = 0;
        }
        super.internal_releaseHandle();
    }

    protected void internal_releaseResources()
    {
        // Release attached dropdown menu
        if (dropDownMenu != null)
        {
            // The native needs to know in advance if a whole menu is being diposed.
            OS.MenuItem_SetMenu(handle, 0);
            if (!dropDownMenu.isDisposed()) dropDownMenu.internal_release();
            dropDownMenu = null;
        }

        parent = null;

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
        return getEnabled() && parent.isEnabled();
    }

    /**
     * Removes the listener from the collection of listeners who will
     * be notified when the control is selected.
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
        removeListener(SWT.Selection, listener);
    }

    /**
     * Sets the widget accelerator.  An accelerator is the bit-wise
     * OR of zero or more modifier masks and a key. Examples:
     * <code>SWT.MOD1 | SWT.MOD2 | 'T', SWT.MOD3 | SWT.F2</code>.
     * <code>SWT.CONTROL | SWT.SHIFT | 'T', SWT.ALT | SWT.F2</code>.
     * The default value is zero, indicating that the menu item does
     * not have an accelerator.
     *
     * @param accelerator an integer that is the bit-wise OR of masks and a key
     *
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setAccelerator(int accelerator)
    {
        checkWidget();
        this.accelerator = accelerator;
        if ((style & SWT.SEPARATOR) == 0)
        {
            OS.MenuItem_SetAccelerator(handle, accelerator);
        }
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
            OS.MenuItem_SetEnabled(handle, enabled);
        }
    }

    /**
     * Sets the image the receiver will display to the argument.
     * <p>
     * Note: This feature is not available on all window systems (for example, Window NT),
     * in which case, calling this method will silently do nothing.
     *
     * @param image the image to display
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setImage(Image image)
    {
        super.setImage(image);
        if ((style & SWT.SEPARATOR) == 0)
        {
            OS.MenuItem_SetImage(handle, (image == null) ? 0 : image.handle);
        }
    }

    /**
     * Sets the receiver's pull down menu to the argument.
     * Only <code>CASCADE</code> menu items can have a
     * pull down menu. The sequence of key strokes, button presses
     * and/or button releases that are used to request a pull down
     * menu is platform specific.
     *
     * @param menu the new pull down menu
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_MENU_NOT_DROP_DOWN - if the menu is not a drop down menu</li>
     *    <li>ERROR_MENUITEM_NOT_CASCADE - if the menu item is not a <code>CASCADE</code></li>
     *    <li>ERROR_INVALID_ARGUMENT - if the menu has been disposed</li>
     *    <li>ERROR_INVALID_PARENT - if the menu is not in the same widget tree</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setMenu(Menu menu)
    {
        checkWidget();

        if ((style & SWT.CASCADE) == 0) internal_error(SWT.ERROR_MENUITEM_NOT_CASCADE);

        if (menu != null)
        {
            if (menu.isDisposed()) internal_error(SWT.ERROR_INVALID_ARGUMENT);
            if (menu.parent != parent.parent) internal_error(SWT.ERROR_INVALID_PARENT);
            if ((menu.style & SWT.DROP_DOWN) == 0) internal_error(SWT.ERROR_MENU_NOT_DROP_DOWN);
        }

        if (dropDownMenu != null)
            dropDownMenu.parentCascadeMenuItem = null;

        dropDownMenu = menu;
        if (menu != null)
            menu.setExclusiveParentCascadeMenuItem(this);

        OS.MenuItem_SetMenu(handle, (menu == null) ? 0 : menu.handle);
    }

    /**
     * Sets the selection state of the receiver.
     * <p>
     * When the receiver is of type <code>CHECK</code> or <code>RADIO</code>,
     * it is selected when it is checked.
     *
     * @param selected the new selection state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setSelection(boolean selected)
    {
        checkWidget();
        if ((style & (SWT.CHECK | SWT.RADIO)) != 0)
        {
            OS.MenuItem_SetSelection(handle, selected);
            // If item is a radio then unselect previous selected radio item
            if (((style & SWT.RADIO) != 0) && (selected == true))
            {
                int indexOfSelected = parent.indexOf(this);
                int count = parent.items.size();
                // Go through next items
                for (int i=indexOfSelected+1; i<count; ++i)
                {
                    MenuItem item = (MenuItem)parent.items.elementAt(i);
                    if ((item.style & SWT.RADIO) == 0)
                    {
                        break;
                    }
                    if (item.getSelection())
                    {
                        OS.MenuItem_SetSelection(item.handle, false);
                        break;
                    }
                }
                // Go through previous items
                for (int i=indexOfSelected-1; i>=0; --i)
                {
                    MenuItem item = (MenuItem)parent.items.elementAt(i);
                    if ((item.style & SWT.RADIO) == 0)
                    {
                        break;
                    }
                    if (item.getSelection())
                    {
                        OS.MenuItem_SetSelection(item.handle, false);
                        break;
                    }
                }
            }
        }
    }
    /**
     * Sets the receiver's text. The string may include
     * the mnemonic character and accelerator text.
     * <p>
     * Mnemonics are indicated by an '&amp' that causes the next
     * character to be the mnemonic.  When the user presses a
     * key sequence that matches the mnemonic, a selection
     * event occurs. On most platforms, the mnemonic appears
     * underlined but may be emphasised in a platform specific
     * manner.  The mnemonic indicator character '&amp' can be
     * escaped by doubling it in the string, causing a single
     *'&amp' to be displayed.
     * </p>
     * <p>
     * Accelerator text is indicated by the '\t' character.
     * On platforms that support accelerator text, the text
     * that follows the '\t' character is displayed to the user,
     * typically indicating the key stroke that will cause
     * the item to become selected.  On most platforms, the
     * accelerator text appears right aligned in the menu.
     * Setting the accelerator text does not install the
     * accelerator key sequence. The accelerator key sequence
     * is installed using #setAccelerator.
     * </p>
     *
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
     * @see #setAccelerator
     */
    public void setText(String string)
    {
        super.setText(string);
        if ((style & SWT.SEPARATOR) == 0)
        {
            OS.MenuItem_SetText(handle, clearText(string));
        }
    }

// removes the mnemonics and accelerator text as
// they are not supported.
    private String clearText(String string)
    {
        StringBuffer buf = new StringBuffer(string);
        int lastFound = -10;
        for (int i=0; i<buf.length(); i++)
        {
            if (buf.charAt(i)=='&' && i != (lastFound +1))
            {
                buf.deleteCharAt(i);
                lastFound = i;
                continue;
            }
            if (buf.charAt(i)=='\t')
            {
                buf.delete(i, buf.length());
            }
        }
        return buf.toString();
    }

// This is called when a control is reparented
    void setParentOfDropDownMenu(Decorations parent)
    {
        if (dropDownMenu != null) dropDownMenu.setParent(parent);
    }

}
