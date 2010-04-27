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
import org.eclipse.swt.events.*;
import org.eclipse.ercp.swt.expanded.internal.OS;

/**
* Instances of this class represent a column in a table widget.
*  <dl>
* <dt><b>Styles:</b></dt>
* <dd>LEFT, RIGHT, CENTER</dd>
* <dt><b>Events:</b></dt>
* <dd> Move, Resize, Selection</dd>
* </dl>
* <p>
* Note: Only one of the styles LEFT, RIGHT and CENTER may be specified.
* </p><p>
* IMPORTANT: This class is <em>not</em> intended to be subclassed.
* </p>
*/
public class TableColumn extends Item
{
    int   handle;
    private Table parent;

    /**
    * Constructs a new instance of this class given its parent
    * (which must be a <code>Table</code>) and a style value
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
    * @see SWT#LEFT
    * @see SWT#RIGHT
    * @see SWT#CENTER
    * @see Widget#checkSubclass
    * @see Widget#getStyle
    */
    public TableColumn(Table parent, int style)
    {
        super(parent, style);
        init(parent, parent.getColumnCount());
    }

    /**
    * Constructs a new instance of this class given its parent
    * (which must be a <code>Table</code>), a style value
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
    * @param parent a composite control which will be the parent of the new instance (cannot be null)
    * @param style the style of control to construct
    * @param index the index to store the receiver in its parent
    *
    * @exception IllegalArgumentException <ul>
    *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
    * </ul>
    * @exception SWTException <ul>
    *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
    *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
    * </ul>
    *
    * @see SWT#LEFT
    * @see SWT#RIGHT
    * @see SWT#CENTER
    * @see Widget#checkSubclass
    * @see Widget#getStyle
    */
    public TableColumn(Table parent, int style, int index)
    {
        super(parent, checkStyle(style), index);
        init(parent, index);
    }

    static int checkStyle(int style)
    {
        return internal_checkBits(style, SWT.LEAD, SWT.CENTER, SWT.TRAIL, 0, 0, 0);
    }

// Initialise the TableColumn, and create the native resources.
    private void init(Table parent, int index)
    {
        // ERROR_INVALID_RANGE not in javadoc but we assume it is the expected behaviour
        if (index < 0 || index > parent.getColumnCount())
        {
            internal_error(SWT.ERROR_INVALID_RANGE); // Not specified in the JavaDoc, but that's what is done in the Win32 implementation
        }

        this.parent = parent;
        this.handle = OS.TableColumn_New(this, parent.tableHandle, this.style, index);
        parent.columnAdded(index);

        parent.tableColumns.addElement(this);
    }

    protected void checkSubclass()
    {
        if (!internal_isValidSubclass()) internal_error(SWT.ERROR_INVALID_SUBCLASS);
    }

    /**
    * Adds the listener to the collection of listeners who will
    * be notified when the control is moved or resized, by sending
    * it one of the messages defined in the <code>ControlListener</code>
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
    * @see ControlListener
    * @see #removeControlListener
    */
    public void addControlListener(ControlListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Resize,typedListener);
        addListener(SWT.Move,typedListener);
    }

    /**
    * Adds the listener to the collection of listeners who will
    * be notified when the control is selected, by sending
    * it one of the messages defined in the <code>SelectionListener</code>
    * interface.
    * <p>
    * <code>widgetSelected</code> is called when the column header is selected.
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
        if (listener == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Selection,typedListener);
        addListener(SWT.DefaultSelection,typedListener);
    }

    /**
    * Returns a value which describes the position of the
    * text or image in the receiver. The value will be one of
    * <code>LEFT</code>, <code>RIGHT</code> or <code>CENTER</code>.
    *
    * @return the alignment
    *
    * @exception SWTException <ul>
    *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
    *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
    * </ul>
    */
    public int getAlignment()
    {
        checkWidget();
        return OS.TableColumn_GetAlignment(handle);
    }

    /**
    * Returns the receiver's parent, which must be a <code>Table</code>.
    *
    * @return the receiver's parent
    *
    * @exception SWTException <ul>
    *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
    *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
    * </ul>
    */
    public Table getParent()
    {
        checkWidget();
        return parent;
    }

    /**
    * Gets the resizable attribute. A column that is
    * not resizable cannot be dragged by the user but
    * may be resized by the programmer.
    *
    * @return the resizable attribute
    *
    * @exception SWTException <ul>
    *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
    *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
    * </ul>
    */
    public boolean getResizable()
    {
        checkWidget();
        return OS.TableColumn_GetResizable(handle);
    }

    public int getStyle()
    {
        checkWidget();
        return OS.TableColumn_GetStyle(handle);
    }

    /**
    * Gets the width of the receiver.
    *
    * @return the width
    *
    * @exception SWTException <ul>
    *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
    *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
    * </ul>
    */
    public int getWidth()
    {
        checkWidget();
        return OS.TableColumn_GetWidth(handle);
    }

    /**
    * Causes the receiver to be resized to its preferred size.
    * For a composite, this involves computing the preferred size
    * from its layout, if there is one.
    *
    * @exception SWTException <ul>
    *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
    *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
    * </ul>
    *
    */
    public void pack()
    {
        checkWidget();
        OS.TableColumn_Pack(handle);
    }

    /**
    * Removes the listener from the collection of listeners who will
    * be notified when the control is moved or resized.
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
    * @see ControlListener
    * @see #addControlListener
    */
    public void removeControlListener(ControlListener listener)
    {
        removeListener(SWT.Move, listener);
        removeListener(SWT.Resize, listener);
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
        removeListener(SWT.Selection,        listener);
        removeListener(SWT.DefaultSelection, listener);
    }

    /**
    * Controls how text and images will be displayed in the receiver.
    * The argument should be one of <code>LEFT</code>, <code>RIGHT</code>
    * or <code>CENTER</code>.
    *
    * @param alignment the new alignment
    *
    * @exception SWTException <ul>
    *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
    *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
    * </ul>
    */
    public void setAlignment(int alignment)
    {
        checkWidget();
        OS.TableColumn_SetAlignment(handle, alignment);
    }

// From Item
    public void setImage(Image image)
    {
        super.setImage(image);
        int imgHandle = (image!=null) ? image.handle : 0;
        OS.TableColumn_SetImage(handle, imgHandle);
    }

    /**
    * Sets the resizable attribute.  A column that is
    * resizable can be resized by the user dragging the
    * edge of the header.  A column that is not resizable
    * cannot be dragged by the user but may be resized
    * by the programmer.
    *
    * @param resizable the resize attribute
    *
    * @exception SWTException <ul>
    *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
    *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
    * </ul>
    */
    public void setResizable(boolean resizable)
    {
        checkWidget();
        OS.TableColumn_SetResizable(handle, resizable);
    }

    /*
     * From Item
     */
    public String getText()
    {
        checkWidget();
        return OS.TableColumn_GetText(handle);
    }


    /*
     * From Item
     */
    public void setText(String string)
    {
        checkWidget();
        if (string == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        OS.TableColumn_SetText(handle, string);
    }

    /**
    * Sets the width of the receiver.
    *
    * @param width the new width
    *
    * @exception SWTException <ul>
    *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
    *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
    * </ul>
    */
    public void setWidth(int width)
    {
        checkWidget();
        OS.TableColumn_SetWidth(handle, width);
    }


    protected void internal_prepareForRelease()
    {
        parent.columnRemoved(this);
        parent.tableColumns.removeElement(this);
        super.internal_prepareForRelease();
    }

    protected void internal_releaseHandle()
    {
        if (handle != 0)
        {
            OS.TableColumn_Dispose(handle);
            handle = 0;
        }
        super.internal_releaseHandle();
    }

    void sendResizeEvent()
    {
        internal_sendEvent(SWT.Resize);
    }

}
