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

import java.util.Vector;
import org.eclipse.swt.internal.symbian.*;
import org.eclipse.swt.*;
import org.eclipse.swt.graphics.*;

/**
 * Instances of this class provide the appearance and
 * behavior of <code>Shells</code>, but are not top
 * level shells or dialogs. Class <code>Shell</code>
 * shares a significant amount of code with this class,
 * and is a subclass.
 * <p>
 * IMPORTANT: This class was intended to be abstract and
 * should <em>never</em> be referenced or instantiated.
 * Instead, the class <code>Shell</code> should be used.
 * </p>
 * <p>
 * Instances are always displayed in one of the maximized,
 * minimized or normal states:
 * <ul>
 * <li>
 * When an instance is marked as <em>maximized</em>, the
 * window manager will typically resize it to fill the
 * entire visible area of the display, and the instance
 * is usually put in a state where it can not be resized
 * (even if it has style <code>RESIZE</code>) until it is
 * no longer maximized.
 * </li><li>
 * When an instance is in the <em>normal</em> state (neither
 * maximized or minimized), its appearance is controlled by
 * the style constants which were specified when it was created
 * and the restrictions of the window manager (see below).
 * </li><li>
 * When an instance has been marked as <em>minimized</em>,
 * its contents (client area) will usually not be visible,
 * and depending on the window manager, it may be
 * "iconified" (that is, replaced on the desktop by a small
 * simplified representation of itself), relocated to a
 * distinguished area of the screen, or hidden. Combinations
 * of these changes are also possible.
 * </li>
 * </ul>
 * </p>
 * Note: The styles supported by this class must be treated
 * as <em>HINT</em>s, since the window manager for the
 * desktop on which the instance is visible has ultimate
 * control over the appearance and behavior of decorations.
 * For example, some window managers only support resizable
 * windows and will always assume the RESIZE style, even if
 * it is not set.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>BORDER, CLOSE, MIN, MAX, NO_TRIM, RESIZE, TITLE, ON_TOP</dd>
 * <dt><b>Events:</b></dt>
 * <dd>(none)</dd>
 * </dl>
 * Class <code>SWT</code> provides two "convenience constants"
 * for the most commonly required style combinations:
 * <dl>
 * <dt><code>SHELL_TRIM</code></dt>
 * <dd>
 * the result of combining the constants which are required
 * to produce a typical application top level shell: (that
 * is, <code>CLOSE | TITLE | MIN | MAX | RESIZE</code>)
 * </dd>
 * <dt><code>DIALOG_TRIM</code></dt>
 * <dd>
 * the result of combining the constants which are required
 * to produce a typical application dialog shell: (that
 * is, <code>TITLE | CLOSE | BORDER</code>)
 * </dd>
 * </dl>
 * <p>
 * IMPORTANT: This class is intended to be subclassed <em>only</em>
 * within the SWT implementation.
 * </p>
 *
 * @see #getMinimized
 * @see #getMaximized
 * @see Shell
 * @see SWT
 */

public class Decorations extends Canvas
{
    private int decorationsHandle;
    Image image;
    Menu menuBar;
    Vector menus;
    Button saveDefault;

    /**
     * Prevents uninitialized instances from being created outside the package.
     */
    Decorations()
    {
    }

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
     * @see SWT#BORDER
     * @see SWT#CLOSE
     * @see SWT#MIN
     * @see SWT#MAX
     * @see SWT#RESIZE
     * @see SWT#TITLE
     * @see SWT#NO_TRIM
     * @see SWT#SHELL_TRIM
     * @see SWT#DIALOG_TRIM
     * @see SWT#ON_TOP
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public Decorations(Composite parent, int style)
    {
        super(parent, style);
        image       = null;
        saveDefault = null;
    }
    protected void checkSubclass()
    {
        if (!internal_isValidSubclass()) internal_error(SWT.ERROR_INVALID_SUBCLASS);
    }

    void closeWidget()
    {
        Event event = new Event();
        internal_sendEvent(SWT.Close, event);
        if (event.doit && !isDisposed()) dispose();
    }

    final void internal_setDecorationsHandle(int decorationsHandle)
    {
        this.decorationsHandle = decorationsHandle;
        setCanvasHandle(OS.Decorations_CanvasHandle(decorationsHandle));
    }

    protected void internal_createHandle()
    {
        internal_setDecorationsHandle(OS.Decorations_New(this, parent.compositeHandle, style));
    }

    Decorations menuDecorations()
    {
        return this;
    }

    void addMenu(Menu menu)
    {
        if (menus == null)
        {
            menus = new Vector();
        }
        menus.addElement(menu);
    }

    void removeMenu(Menu menu)
    {
        menus.removeElement(menu);
        if (menu == menuBar)
        {
            menuBar = null;
            OS.Decorations_SetMenuBar(decorationsHandle, 0);
        }
    }

    /**
     * Returns the receiver's default button if one had
     * previously been set, otherwise returns null.
     *
     * @return the default button or null
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #setDefaultButton
     */
    public Button getDefaultButton()
    {
        checkWidget();
        return OS.Decorations_GetDefaultButton(decorationsHandle);
    }

    /**
     * Returns the receiver's image if it had previously been
     * set using <code>setImage()</code>. The image is typically
     * displayed by the window manager when the instance is
     * marked as iconified, and may also be displayed somewhere
     * in the trim when the instance is in normal or maximized
     * states.
     * <p>
     * Note: This method will return null if called before
     * <code>setImage()</code> is called. It does not provide
     * access to a window manager provided, "default" image
     * even if one exists.
     * </p>
     *
     * @return the image
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Image getImage()
    {
        checkWidget();
        return image;
    }


    /**
     * Returns <code>true</code> if the receiver is currently
     * maximized, and false otherwise.
     * <p>
     *
     * @return the maximized state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #setMaximized
     */
    public boolean getMaximized()
    {
        checkWidget();
        return OS.Decorations_GetMaximized(decorationsHandle);
    }

    /**
     * Returns the receiver's menu bar if one had previously
     * been set, otherwise returns null.
     *
     * @return the menu bar or null
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Menu getMenuBar()
    {
        checkWidget();
        return menuBar;
    }

    /**
     * Returns <code>true</code> if the receiver is currently
     * minimized, and false otherwise.
     * <p>
     *
     * @return the minimized state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #setMinimized
     */
    public boolean getMinimized()
    {
        checkWidget();
        return OS.Decorations_GetMinimized(decorationsHandle);
    }

    String getNameText()
    {
        return getText();
    }


    /**
     * Returns the receiver's text, which is the string that the
     * window manager will typically display as the receiver's
     * <em>title</em>. If the text has not previously been set,
     * returns an empty string.
     *
     * @return the text
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public String getText()
    {
        checkWidget();
        String valRet = OS.Decorations_GetText(decorationsHandle);
        return (valRet!=null) ? valRet : "" ;
    }

    protected void internal_releaseResources()
    {
        super.internal_releaseResources();
        if (menus != null)
        {
            if (menuBar != null)
            {
                // Since the menubar is disposed,  the pointer to this menubar is updated on native side also
                // in order to avoid some wrong use of this pointer while Decorations is disposed.
                menuBar = null;
                OS.Decorations_SetMenuBar(decorationsHandle, 0);
            }
            int count = menus.size();
            for (int i=0; i<count; ++i)
            {
                Menu menu = (Menu)menus.elementAt(i);
                // A disposed menu may have not been removed from our list (e.g. when disposed by its parent MenuItem)
                if (!menu.isDisposed()) menu.internal_release();
            }
            menus.removeAllElements();
            menus = null;
        }
    }

    /**
     * If the argument is not null, sets the receiver's default
     * button to the argument, and if the argument is null, sets
     * the receiver's default button to the first button which
     * was set as the receiver's default button (called the
     * <em>saved default button</em>). If no default button had
     * previously been set, or the saved default button was
     * disposed, the receiver's default button will be set to
     * null.
     *
     * @param button the new default button
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the button has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setDefaultButton(Button button)
    {
        checkWidget();
        if (button != null)
        {
            if (button.isDisposed())internal_error(SWT.ERROR_INVALID_ARGUMENT);
            if ((button.style & SWT.PUSH) == 0) return;
        }

        // Save the saved default button.
        if (saveDefault == null)
        {
            saveDefault = button;
        }

        if (button == null)
        {
            if (saveDefault != null)
            {
                if (!saveDefault.isDisposed())
                {
                    button = saveDefault;
                }
                else
                {
                    saveDefault = null;
                }
            }
        }

        OS.Decorations_SetDefaultButton(decorationsHandle,
                                        ((button == null) ? 0 : button.GetButtonHandle()));
    }

    /**
     * Sets the receiver's image to the argument, which may
     * be null. The image is typically displayed by the window
     * manager when the instance is marked as iconified, and
     * may also be displayed somewhere in the trim when the
     * instance is in normal or maximized states.
     *
     * @param image the new image (or null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the image has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setImage(Image image)
    {
        checkWidget();
        if (image != null)
        {
            if (image.isDisposed()) internal_error(SWT.ERROR_INVALID_ARGUMENT);
        }
        this.image = image;

        int imageHandle = 0;
        if (this.image != null) imageHandle = this.image.handle;
        else imageHandle = 0;

        OS.Decorations_SetImage(decorationsHandle,imageHandle);

    }




    /**
     * Sets the maximized state of the receiver.
     * If the argument is <code>true</code> causes the receiver
     * to switch to the maximized state, and if the argument is
     * <code>false</code> and the receiver was previously maximized,
     * causes the receiver to switch back to either the minimized
     * or normal states.
     * <p>
     * Note: The result of intermixing calls to<code>setMaximized(true)</code>
     * and <code>setMinimized(true)</code> will vary by platform. Typically,
     * the behavior will match the platform user's expectations, but not
     * always. This should be avoided if possible.
     * </p>
     *
     * @param maximized the new maximized state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #setMinimized
     */
    public void setMaximized(boolean maximized)
    {
        checkWidget();
        OS.Decorations_SetMaximized(decorationsHandle, maximized);
    }

    /**
     * Sets the receiver's menu bar to the argument, which
     * may be null.
     *
     * @param menu the new menu bar
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the menu has been disposed</li>
     *    <li>ERROR_INVALID_PARENT - if the menu is not in the same widget tree</li>
     *    <li>ERROR_MENU_NOT_BAR - if menu does not have BAR style</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setMenuBar(Menu menu)
    {
        checkWidget();
        int menuHandle;
        if (menu != null)
        {
            if (menu.isDisposed()) internal_error(SWT.ERROR_INVALID_ARGUMENT);
            if ((menu.style & SWT.BAR) == 0) internal_error(SWT.ERROR_MENU_NOT_BAR);
            if (menu.parent != this) internal_error(SWT.ERROR_INVALID_PARENT);
            menuHandle = menu.handle;
        }
        else
        {
            menuHandle = 0;
        }
        menuBar = menu;
        OS.Decorations_SetMenuBar(decorationsHandle, menuHandle);
    }

    /**
     * Sets the minimized stated of the receiver.
     * If the argument is <code>true</code> causes the receiver
     * to switch to the minimized state, and if the argument is
     * <code>false</code> and the receiver was previously minimized,
     * causes the receiver to switch back to either the maximized
     * or normal states.
     * <p>
     * Note: The result of intermixing calls to<code>setMaximized(true)</code>
     * and <code>setMinimized(true)</code> will vary by platform. Typically,
     * the behavior will match the platform user's expectations, but not
     * always. This should be avoided if possible.
     * </p>
     *
     * @param minimized the new maximized state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #setMaximized
     */
    public void setMinimized(boolean minimized)
    {
        checkWidget();
        OS.Decorations_SetMinimized(decorationsHandle, minimized);
    }

    /**
     * Sets the receiver's text, which is the string that the
     * window manager will typically display as the receiver's
     * <em>title</em>, to the argument, which may not be null.
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
     */
    public void setText(String string)
    {
        checkWidget();
        if (string == null)
        {
            internal_error(SWT.ERROR_NULL_ARGUMENT);
        }
        OS.Decorations_SetText(decorationsHandle, string);
    }


}
