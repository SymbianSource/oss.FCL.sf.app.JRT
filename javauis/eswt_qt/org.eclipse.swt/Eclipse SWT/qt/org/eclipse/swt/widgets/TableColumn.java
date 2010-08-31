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
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Internal_GfxPackageSupport;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.events.ControlListener;
import org.eclipse.swt.events.SelectionListener;

/**
 * Instances of this class represent a column in a table widget.
 * <p><dl>
 * <dt><b>Styles:</b></dt>
 * <dd>LEFT, RIGHT, CENTER</dd>
 * <dt><b>Events:</b></dt>
 * <dd> Move, Resize, Selection</dd>
 * </dl>
 * </p><p>
 * Note: Only one of the styles LEFT, RIGHT and CENTER may be specified.
 * </p><p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */
public class TableColumn extends Item {
    Table parent;

    int headerHandle;
    
    boolean useFixedWidth;
    
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
    public TableColumn (Table parent, int style) {
        super (parent, checkStyle (style));
        this.parent = parent;
        createWidget (parent.getColumnCount ());
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
     * @see SWT#LEFT
     * @see SWT#RIGHT
     * @see SWT#CENTER
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public TableColumn (Table parent, int style, int index) {
        super (parent, checkStyle (style));
        this.parent = parent;
        createWidget (index);
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
    public void addControlListener(ControlListener listener) {
        checkWidget();
        if (listener == null) error (SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener (listener);
        addListener (SWT.Resize,typedListener);
        addListener (SWT.Move,typedListener);
    }
    
    /**
     * Adds the listener to the collection of listeners who will
     * be notified when the control is selected by the user, by sending
     * it one of the messages defined in the <code>SelectionListener</code>
     * interface.
     * <p>
     * <code>widgetSelected</code> is called when the column header is selected.
     * <code>widgetDefaultSelected</code> is not called.
     * </p>
     *
     * @param listener the listener which should be notified when the control is selected by the user
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
    public void addSelectionListener (SelectionListener listener) {
        checkWidget();
        if (listener == null) error (SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener (listener);
        addListener (SWT.Selection,typedListener);
        addListener (SWT.DefaultSelection,typedListener);
    }
    
    static int checkStyle (int style) {
        return checkBits (style, SWT.LEFT, SWT.CENTER, SWT.RIGHT, 0, 0, 0);
    }
    
    protected void checkSubclass () {
        if (!isValidSubclass ()) error (SWT.ERROR_INVALID_SUBCLASS);
    }
    
    void createWidget (int index) {
        if (index < 0 || index > parent.columnCount) error(SWT.ERROR_INVALID_ARGUMENT);
    
        useFixedWidth = false;
        headerHandle = 0;
        text = "";
        
        parent.createColumn(this, index);
        hookEvents_pp();
        register_pp();
        headerHandle = OS.QTableWidget_swt_header(parent.topHandle, true);
        
        setText(text);
    
        // By default columns are resizable
        OS.QHeaderView_setResizeMode(headerHandle, parent.getNativeColumnIndex(index), OS.QT_RESIZEMODEINTERACTIVE);
    }
    
    void destroyWidget () {
        parent.destroyItem (this);
        releaseHandle_pp ();
    }
    
    int getQtAlignment() {
        int align = 0;
        
        if ((style & SWT.RIGHT) != 0) align = OS.QT_ALIGNRIGHT;
        else if ((style & SWT.CENTER) != 0) align = OS.QT_ALIGNHCENTER;
        else align = OS.QT_ALIGNLEFT;
    
        align |= OS.QT_ALIGNVCENTER;
            
        return align;
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
    public int getAlignment () {
        checkWidget();
        if ((style & SWT.CENTER) != 0) return SWT.CENTER;
        if ((style & SWT.RIGHT) != 0) return SWT.RIGHT;
        return SWT.LEFT;
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
    public Table getParent () {
        checkWidget();
        return parent;
    }
    
    int getPreferredColumnWidth(int index) {
        if (index != -1) {
            int width = OS.QAbstractItemView_sizeHintForColumn(parent.topHandle, index);
            if (parent.getHeaderVisible()) {
                width = Math.max(width, OS.QHeaderView_sectionSizeHint(headerHandle, index));
            }
            return width;
        }
        return 0;
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
    public boolean getResizable () {
        checkWidget();
        int index = parent.getNativeColumnIndex(this);
    
        if (index != -1) {
            int resizeMode = OS.QHeaderView_resizeMode(headerHandle, index);
            return resizeMode == OS.QT_RESIZEMODEINTERACTIVE;
        }
        
        return false;
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
    public int getWidth () {
        checkWidget();
        int index = parent.getNativeColumnIndex(this);
    
        if (index != -1) {
            return OS.QTableView_columnWidth(parent.topHandle, index);
        }
        return 0;
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
    public void pack () {
        checkWidget();
            
        int index = parent.getNativeColumnIndex(this);
        
        OS.QTableView_setColumnWidth(parent.topHandle, index, 
                                     getPreferredColumnWidth(index));
        
        if (parent.getWordWrap()) {
            parent.updateRowHeight(false);
        }
    }
    
    void releaseHandle_pp () {
        super.releaseHandle_pp ();
        deregister_pp();
        parent = null;
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
    public void removeControlListener (ControlListener listener) {
        checkWidget();
        if (listener == null) error (SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook (SWT.Move, listener);
        eventTable.unhook (SWT.Resize, listener);
    }
    
    /**
     * Removes the listener from the collection of listeners who will
     * be notified when the control is selected by the user.
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
    public void removeSelectionListener(SelectionListener listener) {
        checkWidget();
        if (listener == null) error (SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook (SWT.Selection, listener);
        eventTable.unhook (SWT.DefaultSelection,listener);    
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
    public void setAlignment (int alignment) {
        checkWidget();
        if ((alignment & (SWT.LEFT | SWT.RIGHT | SWT.CENTER)) == 0) return;
        int index = parent.getColumnIndex(this);
        if (index <= 0) return;
        style &= ~(SWT.LEFT | SWT.RIGHT | SWT.CENTER);
        style |= alignment & (SWT.LEFT | SWT.RIGHT | SWT.CENTER);
        
        int align = getQtAlignment();
    
        for (int i = 0; i < parent.itemCount; i++) {
            int itemHandle = parent._getItem(i).getHandle(index);
            OS.QTableWidgetItem_setTextAlignment(itemHandle, align);
        }
    }
    
    
    public void setImage (Image image) {
        checkWidget ();
        super.setImage (image);    
        int index = parent.getNativeColumnIndex(this);    
        if (index != -1) {
            int imageHandle = image != null ? 
                    Internal_GfxPackageSupport.getIconHandle(image) : Internal_GfxPackageSupport.getNullIconHandle();
            int itemHandle = OS.QTableWidget_swt_headerItem(parent.topHandle, index);
            
            OS.QTableWidgetItem_setIcon(itemHandle, imageHandle);
        }
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
    public void setResizable (boolean resizable) {
        checkWidget();
        int resizeMode = resizable ? OS.QT_RESIZEMODEINTERACTIVE : OS.QT_RESIZEMODEFIXED;
        int index = parent.getNativeColumnIndex(this);
        if (index != -1) {
            OS.QHeaderView_setResizeMode(headerHandle, index, resizeMode);
        }
    }
    
    
    public void setText (String string) {
        checkWidget();
        if (string == null) error (SWT.ERROR_NULL_ARGUMENT);
        super.setText (string);    
        int index = parent.getNativeColumnIndex(this);    
        if (index != -1) {
            int itemHandle = OS.QTableWidget_swt_headerItem(parent.topHandle, index);
            OS.QTableWidgetItem_setText(itemHandle, string);
        }
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
    public void setWidth (int width) {
        checkWidget();
        if (width < 0) return;    
        int index = parent.getNativeColumnIndex(this);    
        if (index != -1) {
            useFixedWidth = true;
            OS.QTableView_setColumnWidth(parent.topHandle, index, width);
            
            if (parent.getWordWrap()) {
                parent.updateRowHeight(false);
            }
        }
        
        sendEvent(SWT.Resize);
    }
}
