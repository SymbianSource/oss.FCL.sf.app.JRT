/*******************************************************************************
 * Copyright (c) 2007 Nokia Corporation.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.ercp.swt.mobile;

import java.util.Vector;

import org.eclipse.ercp.swt.mobile.internal.OS;
import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.ControlListener;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Dialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.TypedListener;

/**
 * Instances of this class represents a tabbed dialog. The dialog contains
 * multiple pages. Each page contains a composite. At any given time, only one
 * page is visible. The page visibility can be selected by end-users or
 * applications programmatically.
 * <p>
 * Each page is assigned with a label. The label can be either a text or an
 * icon, or all together. The size and position of page labels is
 * implementation-dependent.
 * </p>
 * <p>
 * There must not be any limit on the number of pages, when the memory is
 * sufficient. Run-time errors may be thrown when the resource is insufficient
 * to perform this call.</p>
 * <p>
 * Note: The bounds of this dialog is implementation-dependent. Applications can
 * query the page size through <code>Composite.getBounds</code> after the page
 * has been created.
 * </p>
 * <h3>Example:</h3>
 * <code><pre>
 * MultiPageDialog dialog = new MultiPageDialog(shell);
 * Composite page1 = dialog.createPage(&quot;Page 1&quot;, null);
 * // add child controls here
 * Command okCMD = new Command(page1, Command.OK, 1);
 * okCMD.setText(&quot;Ok&quot;);
 * okCMD.addCommandListener(new CommandListener() {
 *  public void commandAction(CommandEvent e) {
 *      // do something here before closing the dialog
 *      dialog.close(); // Make the dialog invisible
 *      dialog.dispose(); // Dispose the dialog and its children.
 *  }
 * });
 * dialog.setSelection(dialog.getPageCount() - 1); // The last page
 * dialog.open(); // open a modal dialog
 * </pre></code>
 * <dl>
 * <dt><b>Styles: </b></dt>
 * <dd>(none)</dd>
 * <dt><b>Events: </b></dt>
 * <dd>Selection</dd>
 * </dl>
  * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
*/
public class MultiPageDialog extends Dialog
{

    private int multiPageDialogHandle;
    private boolean isOpen = false;
    private internal_MultiPageDialogControl multiPageDialogControl;
    private Shell fShell;
    private Vector tabItems = new Vector();
    private ShellSizeListener fShellSizeListener;


    private final class ShellSizeListener implements ControlListener
    {
        public void controlResized(ControlEvent e)
        {
            if (tabItems == null)
            {
                return;
            }
            Rectangle rect = fShell.getClientArea();
            for (int i = 0;  i < tabItems.size(); i++)
            {
                ((internal_TabItem)tabItems.elementAt(i)).composite.setSize(rect.width, rect.height);
            }
        }
        public void controlMoved(ControlEvent e)
        {

        }
    }

    /**
     * This class implements the tabs items of the MultiPageDialog.
     */
    private class internal_TabItem
    {
        private Composite composite;
        private String    title;
        private Image     image;

        internal_TabItem(Composite composite, String title, Image image)
        {
            this.composite = composite;
            this.title = title;
            this.image = image;
        }
    }

    public class internal_MultiPageDialogControl extends Control
    {
        private int multiPageDialogControlHandle;
        internal_MultiPageDialogControl(Shell parent)
        {
            super(parent, SWT.NONE);
        }

        protected void internal_createHandle()
        {
            multiPageDialogControlHandle = OS.MultiPageDialog_New(this, this.getParent().getShell().internal_getShellHandle());
            handle = OS.MultiPageDialog_ControlHandle(multiPageDialogControlHandle);
        }

        int getHandle()
        {
            return multiPageDialogControlHandle;
        }

        public void removeSelectionListener(SelectionListener listener)
        {
            removeListener(SWT.Selection, listener);
        }

    }

    /**
     * Constructs a new instance of this class given only its parent.
     *
     * @param parent
     *            a shell which will be the parent of the new instance
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
     */
    public MultiPageDialog(Shell parent)
    {
        this(parent, SWT.NONE);
    }

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
     *
     * @param parent
     *            a shell which will be the parent of the new instance
     * @param style
     *            the style of dialog to construct
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
     */
    public MultiPageDialog(Shell parent, int style)
    {
        // The only style we take care is MODAL or NON_MODAL.
        // The style of the internal shell is DIALOG_TRIM.
        super(parent, (style & (SWT.APPLICATION_MODAL | SWT.PRIMARY_MODAL)));
        checkSubclass();
        fShell = new Shell(parent.getDisplay(), (SWT.PRIMARY_MODAL));
        fShell.setText(parent.getText());
        fShellSizeListener = new ShellSizeListener();
        fShell.addControlListener(fShellSizeListener);
        multiPageDialogControl = new internal_MultiPageDialogControl(fShell);
        multiPageDialogHandle = multiPageDialogControl.getHandle();
    }

    private void checkDialog()
    {
        fShell.internal_checkShell();
        if (multiPageDialogControl.isDisposed()) SWT.error(SWT.ERROR_WIDGET_DISPOSED);
    }

    /**
     * Adds the listener to the collection of listeners who will be notified
     * when the page changes, by sending it one of the messages defined in the
     * <code>SelectionListener</code> interface.
     * <p>
     * When <code>widgetSelected</code> is called, the item field of the event
     * object is valid. <code>widgetDefaultSelected</code> is not called.
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
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * @see SelectionListener
     * @see #removeSelectionListener
     * @see org.eclipse.swt.events.SelectionEvent
     */
    public void addSelectionListener(SelectionListener listener)
    {
        checkDialog();
        if (listener == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        multiPageDialogControl.addListener(SWT.Selection, typedListener);
    }


    /**
     * Requests that the window manager close the dialog in the same way it
     * would be closed when the user clicks on the "close box" or performs some
     * other platform specific key or mouse combination that indicates the
     * window should be removed.
     *
     * No exception should be thrown even when the dialog has been closed by the
     * end-user.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *
     * @see org.eclipse.swt.widgets.Shell#close
     */
    public void close()
    {
        checkDialog();
        isOpen = false;
        OS.MultiPageDialog_Close(multiPageDialogHandle);
        fShell.setVisible(false);
    }

    /**
     * Creates a new page with the specified title string and icon.
     *
     * @param title -
     *            the title string. Cannot be null.
     * @param icon -
     *            the label icon image. May be null.
     * @return composite - the new composite instance in the page. Cannot be
     *         null.
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the page title is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public Composite createPage(String title, Image icon)
    {
        checkDialog();
        if (title == null)
        {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }

        Composite page = new Composite(fShell, SWT.NONE);
        page.setVisible(false);
        Rectangle rect = fShell.getClientArea();
        page.setSize(rect.width, rect.height);
        int imageHandle = 0;
        if (icon != null)
        {
            imageHandle = icon.handle;
        }
        OS.MultiPageDialog_CreatePage(multiPageDialogHandle, page.internal_getCompositeHandle(),
                                      title, imageHandle);

        tabItems.addElement(new internal_TabItem(page, title, icon));

        return page;
    }


    /**
     * Deletes the page from the receiver at the given zero-relative index
     *
     * @param index -
     *            the index value. Cannot be negative, greater or equal to the
     *            number of pages.
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_RANGE - if the index is not between 0
     *                and the number of pages in the receiver minus 1
     *                (inclusive)</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void deletePage(int index)
    {
        checkDialog();
        checkRange(index);
        OS.MultiPageDialog_DeletePage(multiPageDialogHandle, index);
        ((internal_TabItem)tabItems.elementAt(index)).composite.dispose();
        tabItems.removeElementAt(index);
    }


    /**
     * Disposes of the operating system resources associated with the receiver.
     * After this method has been invoked, all descendents will
     * answer <code>true</code> when sent the message
     * <code>isDisposed()</code>. Any internal connections between the
     * widgets in the tree will have been removed to facilitate garbage
     * collection.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *
     * @see org.eclipse.swt.widgets.Widget#dispose
     */
    public void dispose()
    {
        checkDialog();
        close();
        multiPageDialogControl.dispose();
        fShell.removeControlListener(fShellSizeListener);
        fShell.dispose();
        tabItems.removeAllElements();
    }


    /**
     * Returns the composite of the page at the given zero-relative index in the
     * receiver.
     *
     * @param index
     *            the index of the page to select. Cannot be negative, greater
     *            or equal to the number of pages.
     * @return Composite the composite.
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_RANGE - if the page count is zero, or
     *                if the index is negative or greater and equal to the page
     *                count</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     */
    public Composite getPage(int index)
    {
        checkDialog();
        checkRange(index);
        return ((internal_TabItem)tabItems.elementAt(index)).composite;
    }


    /**
     * Returns the number of pages contained in the receiver.
     *
     * @return int the number of pages. Returns zero when the dialog is
     *         constructed.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     */
    public int getPageCount()
    {
        checkDialog();
        return tabItems.size();
    }


    /**
     * Returns the title string of the page at the given zero-relative index in
     * the receiver.
     *
     * @param index
     *            the index of the page to select.
     * @return String the title string of the page
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_RANGE - if the page count is zero, or
     *                if the index is negative or greater and equal to the page
     *                count</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                </ul>
     * @see #setTitle
     */
    public String getTitle(int index)
    {
        checkDialog();
        checkRange(index);
        return ((internal_TabItem)tabItems.elementAt(index)).title;
    }


    /**
     * Returns the zero-relative index of the page which is currently selected
     * in the receiver, or the newest page created to the receiver.
     *
     * @return int the index of the selected page
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                </ul>
     */
    public int getSelectionIndex()
    {
        checkDialog();
        return OS.MultiPageDialog_GetSelectedIndex(multiPageDialogHandle);
    }


    /**
     * Makes the dialog visible and brings it to the front of the display.
     *
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the dialog</li>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                </ul>
     */
    public void open()
    {
        checkDialog();
        if (!isOpen)
        {
            fShell.open();
            OS.MultiPageDialog_Open(multiPageDialogHandle);
            isOpen = true;
        }
        else
        {
            fShell.forceActive();
        }
        Display display = fShell.getDisplay();
        while (isOpen)
        {
            if (!display.readAndDispatch())
                display.sleep();
        }
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
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                <li>ERROR_WIDGET_DISPOSED - if the dialog has been
     *                disposed</li>
     *                </ul>
     *
     * @see SelectionListener
     * @see #addSelectionListener
     */
    public void removeSelectionListener(SelectionListener listener)
    {
        checkDialog();
        multiPageDialogControl.removeSelectionListener(listener);
    }


    /**
     * Selects the active page by the given zero-relative index.
     *
     * @param index
     *            the index of the page to select. Cannot be negative, greater
     *            or equal to the number of pages.
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_RANGE - if the page count is zero, or
     *                if the index is negative or greater and equal to the page
     *                count</li>
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
    public void setSelection(int index)
    {
        checkDialog();
        checkRange(index);
        OS.MultiPageDialog_SetSelection(multiPageDialogHandle, index);
    }


    /**
     * Sets the icon image of the page at the given zero-relative index. Note
     * that the icon size is implementation dependent so that the icon image may
     * be stretched or shrunk automatically.
     *
     * @param index
     *            the index of the page to select.
     * @param icon
     *            the new icon image.
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the icon is null</li>
     *                <li>ERROR_INVALID_RANGE - if the page count is zero, or
     *                if the index is negative or greater and equal to the page
     *                count</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setImage(int index, Image icon)
    {
        checkDialog();
        if (icon == null)
        {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }
        checkRange(index);
        internal_TabItem item = (internal_TabItem) tabItems.elementAt(index);
        tabItems.setElementAt(new internal_TabItem(item.composite, item.title, icon), index);
        OS.MultiPageDialog_SetTitleAndImage(multiPageDialogHandle, index, item.title, icon.handle);
    }


    // From Dialog class
    public void setText(String text)
    {
        checkDialog();
        super.setText(text);
        fShell.setText(text);
    }


    /**
     * Sets the title string of the page at the given zero-relative index.
     *
     * @param index
     *            the index of the page to select.
     * @param title
     *            the new title string. Cannot be null.
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_RANGE - if the page count is zero, or
     *                if the index is negative or greater and equal to the page
     *                count</li>
     *                <li>ERROR_NULL_ARGUMENT - if the title is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * @see #getTitle
     */
    public void setTitle(int index, String title)
    {
        checkDialog();
        if (title == null)
        {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }
        checkRange(index);
        internal_TabItem item = (internal_TabItem) tabItems.elementAt(index);
        tabItems.setElementAt(new internal_TabItem(item.composite, title, item.image), index);
        OS.MultiPageDialog_SetTitle(multiPageDialogHandle, index, title);
    }


    private void checkRange(int index)
    {
        if ((index < 0) || (getPageCount() <= index))
        {
            SWT.error(SWT.ERROR_INVALID_RANGE);
        }
    }
}
