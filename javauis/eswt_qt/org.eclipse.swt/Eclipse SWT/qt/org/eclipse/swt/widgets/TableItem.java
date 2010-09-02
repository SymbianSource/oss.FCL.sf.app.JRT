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
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Internal_GfxPackageSupport;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;

/**
 * Instances of this class represent a selectable user interface object
 * that represents an item in a table.
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
public class TableItem extends Item {
    Table parent;

    /*
     * Array for storing the native side QTableWidgetItem handles
     */
    int [] handles;

    /*
     * Number of columns, i.e. number of valid items in handles, cellFont,
     * etc. arrays
     */
    int columnCount;
    
    /*
     * Used for storing the general Font/Color for the item,
     * arrays for storing the same for individual TableItem cells
     */
    Font font;
    Font[] cellFont;
    Color background;
    Color[] cellBackground;
    Color foreground;
    Color[] cellForeground; 
    Image[] cellImage;
    
    /*
     * Tells if the item has all the data fetched (can be false only in 
     * SWT.VIRTUAL style Table)
     */
    boolean cached;

    /*
     * Native check box handle (this gets initialized only if Table is
     * created with SWT.CHECK style.
     */
    int checkBoxHandle;
    
    int radioButtonHandle;
    
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
     * @see SWT
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public TableItem (Table parent, int style, int index) {
        this (parent, style, index, true);
    }
    
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
     * @see SWT
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public TableItem (Table parent, int style) {
        this (parent, style, checkNull (parent).getItemCount (), true);
    }
    
    
    TableItem (Table parent, int style, int index, boolean create) {
        super (parent, style);
        this.parent = parent;
        columnCount = parent.columnCount > 0 ? parent.columnCount : 1;
        
        if (create) {
            // We don't want to send any events during creation, so set the
            // flags to false (Qt generates some signals when setting the
            // initial states).
            parent.apiCheck = true;
            parent.apiSelection = true;
            parent.createItem(this, index);        
            OS.QTableWidget_insertRow(parent.topHandle, index);
            OS.QTableView_setRowHeight(parent.topHandle, index, parent.getItemHeight());
            
            ensureValid(index);
            // Default radio selection
            if(index == 0 && (parent.style & SWT.RADIO ) != 0 && (parent.style & SWT.SINGLE ) != 0){
                OS.QAbstractButton_setChecked(radioButtonHandle,true);
            }
            parent.apiCheck = false;
            parent.apiSelection = false;          
        } 
    }
    
    void releaseHandle_pp () {
        handle = 0;
        super.releaseHandle_pp ();
        parent = null;
        deregister_pp();    
    }
    
    void releaseWidget_pp () {
        super.releaseWidget_pp ();
        font = null;
        cellFont = null;
        background = null;
        cellBackground = null;
        foreground = null;
        cellForeground = null;
        cellImage = null;
        handles = null;
        radioButtonHandle = 0;
    }
    
    /*
     * Ensures that the TableItem has all the native and Java side
     * resources created.
     * @param index Index of the item in Table
     */
    void ensureValid(int index) {
        if (cached) {
            return;
        }
        
        cached = true;
        
        if (checkBoxHandle == 0 && (parent.style & SWT.CHECK ) != 0) {
            checkBoxHandle = OS.QTableWidgetItem_new();
            OS.QTableWidgetItem_setFlags(checkBoxHandle, OS.QT_ITEMISENABLED | OS.QT_ITEMISUSERCHECKABLE|OS.QT_ITEMISSELECTABLE);
            OS.QTableWidgetItem_setData(checkBoxHandle, OS.QT_CHECKSTATEROLE, OS.QT_UNCHECKED);
            OS.QTableWidget_setItem(parent.topHandle, index, 0, checkBoxHandle);        
        }  else if(radioButtonHandle == 0 && (parent.style & SWT.RADIO ) != 0 && (parent.style & SWT.SINGLE ) != 0){
            radioButtonHandle = OS.QRadioButton_new();
            OS.QObject_setProperty(radioButtonHandle, "swt_obj_idx", OS.QObject_property(parent.topHandle, "swt_obj_idx"));
            
            OS.QWidget_setFocusPolicy(radioButtonHandle, OS.QT_FOCUSPOLICY_NOFOCUS);
            OS.QTableWidget_setCellWidget(parent.topHandle, index, 0, radioButtonHandle);
        }
        
        for (int i = 0; i < columnCount; i++) {
            createCellItem(index, i);
        }
    }
    
    /*
     * Removes a TableColumn from the item at index
     * @param index Index to remove
     */
    void removeColumn(int index) { 
        // If column count is 1, table goes to list mode and we should not
        // remove the native columns
        if (columnCount == 1) return;
        
        if (index < columnCount - 1) {
            if (handles != null) System.arraycopy (handles, index + 1, handles, index, columnCount - index - 1);
            if (cellForeground != null) System.arraycopy (cellForeground, index + 1, cellForeground, index, columnCount - index - 1);
            if (cellBackground != null) System.arraycopy (cellBackground, index + 1, cellBackground, index, columnCount - index - 1);
            if (cellImage != null) System.arraycopy (cellImage, index + 1, cellImage, index, columnCount - index - 1);
            if (cellFont != null) System.arraycopy (cellFont, index + 1, cellFont, index, columnCount - index - 1);
        }
        
        columnCount--;
        
        if (handles != null) handles[columnCount] = 0;
        if (cellForeground != null) cellForeground[columnCount] = null;
        if (cellBackground != null) cellBackground[columnCount] = null;
        if (cellImage != null) cellImage[columnCount] = null;
        if (cellFont != null) cellFont[columnCount] = null;
    }
    
    /*
     * Adds a column to the TableItem at index
     * @param row Index of the TableItem in Table
     * @param index Index of the column to add
     */
    void addColumn(int row, int index) {
        columnCount++;
        
        if ((parent.style & SWT.VIRTUAL) != 0) {
            // In VIRTUAL style table adding a new column causes the
            // item to contain uninitialized items, so set cached state
            // to false
            cached = false;
        }
               
        // Update handle array
        if (handles != null) {            
            if (columnCount > handles.length) {
                int [] newHandles = new int[columnCount];
            
                System.arraycopy (handles, 0, newHandles, 0, handles.length);
                handles = newHandles;
            }
            
            if (index < columnCount - 1) {
                System.arraycopy (handles, index, handles, index + 1, columnCount - index -1);
            }   
            handles[index] = 0;
        }
        
        // Update the rest of the arrays (if needed)
        cellForeground = updateColorArray(cellForeground, index);
        cellBackground = updateColorArray(cellForeground, index);
        cellImage = updateImageArray(cellImage, index);
        cellFont = updateFontArray(cellFont, index);    
        
        createCellItem(row, index);        
    }
    
    Image [] updateImageArray(Image [] array, int index) {
        if (array != null) {
            // Make the array bigger if needed
            if (columnCount > array.length) {
                Image [] newArray = new Image[columnCount];
                System.arraycopy(array, 0, newArray, 0, array.length);
                array = newArray;
            }
            // Make space for the new item at index
            if (index < columnCount - 1) {
                System.arraycopy (array, index, array, index + 1, columnCount - index -1);
            }        
            array[index] = null;
        }
        return array;
    }
    Color [] updateColorArray(Color [] array, int index) {
        if (array != null) {
            if (columnCount > array.length) {
                Color [] newArray = new Color[columnCount];
                System.arraycopy(array, 0, newArray, 0, array.length);
                array = newArray;
            }
            if (index < columnCount - 1) {
                System.arraycopy (array, index, array, index + 1, columnCount - index -1);
            }        
            array[index] = null;
        }
        return array;
    }
    Font [] updateFontArray(Font [] array, int index) {
        if (array != null) {
            if (columnCount > array.length) {
                Font [] newArray = new Font[columnCount];
                System.arraycopy(array, 0, newArray, 0, array.length);
                array = newArray;
            }
            if (index < columnCount - 1) {
                System.arraycopy (array, index, array, index + 1, columnCount - index -1);
            }  
            array[index] = null;
        }
        return array;
    }
    
    /*
     * Creates a native QTableWidgetItem cell in the item at given index
     * @param index Index of the TableItem in Table
     * @param handleIndex Index inside the TableItem for the new cell (i.e. column)
     */
    void createCellItem(int index, int handleIndex) {
        if (handles == null) {
            handles = new int [columnCount];
        }
        
        if (handles[handleIndex] == 0) {
            handles[handleIndex] = OS.QTableWidgetItem_new();
            
            OS.QTableWidgetItem_setFlags(handles[handleIndex], OS.QT_ITEMISSELECTABLE | OS.QT_ITEMISENABLED);
            
            // Set new item alignment
            if (parent.columnCount > handleIndex) {
                TableColumn column = parent.getColumn(handleIndex);
                OS.QTableWidgetItem_setTextAlignment(handles[handleIndex], column.getQtAlignment());
            }
            
            OS.QTableWidget_setItem(parent.topHandle, index, parent.getNativeColumnIndex(handleIndex), handles[handleIndex]);
            
            // If the user has set background, foreground or font for the whole item
            // set that here for the new column also
            if (background != null) OS.QTableWidgetItem_swt_setBackground(handles[handleIndex], background.getRed(), background.getGreen(), background.getBlue(), false);
            if (foreground != null) OS.QTableWidgetItem_swt_setForeground(handles[handleIndex], foreground.getRed(), foreground.getGreen(), foreground.getBlue(), false);
            if (font != null) OS.QTableWidgetItem_swt_setFont(handles[handleIndex], font.handle);
        }
    }
    
    /*
     * Set selected state for the item
     * @param selected Selection state
     */
    void setSelected(boolean selected) {
        if (handles == null) return;
        parent.apiSelection = true;
        for (int i = 0; i < handles.length; i++) {
            OS.QTableWidgetItem_setSelected(getHandle(i), selected);
        }
        if(((parent.style & SWT.SINGLE) != 0) && ((parent.style & SWT.RADIO) != 0)){
            OS.QAbstractButton_setChecked(radioButtonHandle,selected);
        }
        parent.apiSelection = false;
    }
    
    /*
     * Returns item selection state
     * @return True is selected, false otherwise
     */
    boolean isSelected() {
        // Go through all QTableWidgetItems of the TableItem and return
        // true if any cell is selected
        if(((parent.style & SWT.SINGLE) != 0) && ((parent.style & SWT.RADIO) != 0)){
            return OS.QAbstractButton_isChecked(radioButtonHandle);
        }
        if (handles != null) {
            for (int i = 0; i < handles.length; i++) {
                if (OS.QTableWidgetItem_isSelected(getHandle(i))) {
                    return true;
                }
            }
        }
        
        return false;
    }
    
    /*
     * Return QTableWidgetItem handle at column index.
     * @param index Column for the handle
     * @return QTableWidget handle or 0
     */
    int getHandle(int index) {
        if (index >= 0 && index < handles.length) {
            return handles[index];
        }
        return 0;
    }
    
    static Table checkNull (Table control) {
        if (control == null) SWT.error (SWT.ERROR_NULL_ARGUMENT);
        return control;
    }
    
    protected void checkSubclass () {
        if (!isValidSubclass ()) error (SWT.ERROR_INVALID_SUBCLASS);
    }
    
    public boolean isDisposed() {
        
        if (parent != null && (parent.style & SWT.VIRTUAL) == 0) {
            if (handles == null) return true;
            if (handles.length == 0) return true;
            if (handles[0] == 0) return true;
        }
       
        return super.isDisposed();
    }
    
    void destroyWidget () {
        parent.destroyItem (this);
        releaseHandle_pp();
    }
    
    int topHandle() {
        if (handles != null && handles.length > 0) {
            return handles[0];
        }
        return 0;
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
     */
    public Color getBackground () {
        checkWidget ();
        if (!parent.checkData (this)) error (SWT.ERROR_WIDGET_DISPOSED);
        if (background != null) return background;
        return parent.getBackground();
    }
    
    
    /**
     * Returns the background color at the given column index in the receiver.
     *
     * @param index the column index
     * @return the background color
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     * 
     * @since 3.0
     */
    public Color getBackground (int index) {
        checkWidget ();
        if (!parent.checkData (this)) error (SWT.ERROR_WIDGET_DISPOSED);
        if (0 > index || handles.length < index) return getBackground();
        if (cellBackground != null && cellBackground[index] != null) return cellBackground[index];
        if (background != null) return background;
        return parent.getBackground();
    }
    
    /**
     * Returns a rectangle describing the receiver's size and location
     * relative to its parent at a column in the table.
     *
     * @param index the index that specifies the column
     * @return the receiver's bounding column rectangle
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Rectangle getBounds (int index) {
        checkWidget();
        if (!parent.checkData (this)) error (SWT.ERROR_WIDGET_DISPOSED);
        
        if (index < 0 || handles == null || index >= handles.length) {
            return new Rectangle (0, 0, 0, 0);
        }
        
        int itemHandle = getHandle(index);
        
        return OS.QTableWidget_visualItemRect(parent.topHandle, itemHandle);
    }
    
    /**
     * Returns <code>true</code> if the receiver is checked,
     * and false otherwise.  When the parent does not have
     * the <code>CHECK</code> style, return false.
     *
     * @return the checked state of the checkbox
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean getChecked () {
        checkWidget();
        if (!parent.checkData (this)) error (SWT.ERROR_WIDGET_DISPOSED);
        if ((parent.style & SWT.CHECK) == 0) return false;
        return OS.QTableWidgetItem_data(checkBoxHandle, OS.QT_CHECKSTATEROLE) == OS.QT_CHECKED;
    }
    
    boolean getRadioSelected () {
        checkWidget();
        if (!parent.checkData (this)) error (SWT.ERROR_WIDGET_DISPOSED);
        if ((parent.style & SWT.RADIO) == 0) return false;
        return OS.QAbstractButton_isChecked(radioButtonHandle);
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
    public Font getFont () {
        checkWidget ();
        if (!parent.checkData (this)) error (SWT.ERROR_WIDGET_DISPOSED);
        return font != null ? font : parent.getFont ();
    }
    
    /**
     * Returns the font that the receiver will use to paint textual information
     * for the specified cell in this item.
     *
     * @param index the column index
     * @return the receiver's font
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 3.0
     */
    public Font getFont (int index) {
        checkWidget ();
        if (!parent.checkData (this)) error (SWT.ERROR_WIDGET_DISPOSED);
        int count = Math.max (1, parent.columnCount);
        if (0 > index || index > count - 1) return getFont ();
        if (cellFont == null || cellFont [index] == null) return getFont ();
        return cellFont [index];
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
     */
    public Color getForeground () {
        checkWidget ();
        if (!parent.checkData (this)) error (SWT.ERROR_WIDGET_DISPOSED);
        if (foreground != null) return foreground;
        return parent.getForeground();
    }
    
    /**
     * 
     * Returns the foreground color at the given column index in the receiver.
     *
     * @param index the column index
     * @return the foreground color
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     * 
     * @since 3.0
     */
    public Color getForeground (int index) {
        checkWidget ();
        if (!parent.checkData (this)) error (SWT.ERROR_WIDGET_DISPOSED);
        if (0 > index || columnCount < index) return getForeground();
        if (cellForeground != null && cellForeground[index] != null) return cellForeground[index];
        if (foreground != null) return foreground;
        return  parent.getForeground();
    }
    
    /**
     * Returns <code>true</code> if the receiver is grayed,
     * and false otherwise. When the parent does not have
     * the <code>CHECK</code> style, return false.
     *
     * @return the grayed state of the checkbox
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean getGrayed () {
        checkWidget ();
        if (!parent.checkData (this)) error (SWT.ERROR_WIDGET_DISPOSED);
        if ((parent.style & SWT.CHECK) == 0) return false;  
        return OS.QTableWidgetItem_grayed(checkBoxHandle);
    }
    
    
    /**
     * Returns the image stored at the given column index in the receiver,
     * or null if the image has not been set or if the column does not exist.
     *
     * @param index the column index
     * @return the image stored at the given column index in the receiver
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Image getImage (int index) {
        checkWidget ();
        if (!parent.checkData (this)) error (SWT.ERROR_WIDGET_DISPOSED);
        if (index < 0 || index >= columnCount) return null;
        if (cellImage == null) return null;
        return cellImage[index];
    }
    
    /**
     * Returns a rectangle describing the size and location
     * relative to its parent of an image at a column in the
     * table.  An empty rectangle is returned if index exceeds
     * the index of the table's last column.
     *
     * @param index the index that specifies the column
     * @return the receiver's bounding image rectangle
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Rectangle getImageBounds (int index) {
        Rectangle imageBounds = getBounds(index);
        
        imageBounds.width = 0;
        imageBounds.height = 0;
        
        if (index < 0 || handles == null || index >= columnCount ||
            cellImage == null || cellImage[index] == null) {
            return imageBounds;
        }
        
        Point imageSize = OS.QAbstractItemView_iconSize(parent.topHandle);
        imageBounds.width = imageSize.x;
        imageBounds.height = imageSize.y;
        
        return imageBounds;
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
        checkWidget ();
        return parent;
    }
    
    
    /**
     * Returns the text stored at the given column index in the receiver,
     * or empty string if the text has not been set.
     *
     * @param index the column index
     * @return the text stored at the given column index in the receiver
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public String getText (int index) {
        checkWidget ();
        if (!parent.checkData (this)) error (SWT.ERROR_WIDGET_DISPOSED);
        if (index < 0 || index >= columnCount) return "";
        return OS.QTableWidgetItem_text(getHandle(index));
    }
    
    public String getText() {
        return getText(0);
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
     */
    public void setBackground (Color color) {
        checkWidget ();
        if (color != null && color.isDisposed ()) {
            SWT.error (SWT.ERROR_INVALID_ARGUMENT);
        }
    
        background = color;
        
        if (checkBoxHandle != 0) {
            Color cellBg = getBackground();
            boolean restore =  (cellBg == null);
            int red = cellBg != null ? cellBg.getRed() : 0;
            int green = cellBg != null ? cellBg.getGreen() : 0;
            int blue = cellBg != null ? cellBg.getBlue() : 0;
            OS.QTableWidgetItem_swt_setBackground(checkBoxHandle, red, green, blue, restore);
        }
        
        for (int i = 0; i < columnCount; i++) {
            Color cellBg = getBackground(i);
            boolean restore =  (cellBg == null);
            int red = cellBg != null ? cellBg.getRed() : 0;
            int green = cellBg != null ? cellBg.getGreen() : 0;
            int blue = cellBg != null ? cellBg.getBlue() : 0;
            OS.QTableWidgetItem_swt_setBackground(getHandle(i), red, green, blue, restore);
        }
    }
    
    /**
     * Sets the background color at the given column index in the receiver 
     * to the color specified by the argument, or to the default system color for the item
     * if the argument is null.
     *
     * @param index the column index
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
     * @since 3.0
     */
    public void setBackground (int index, Color color) {
        checkWidget ();
        if (color != null && color.isDisposed ()) SWT.error (SWT.ERROR_INVALID_ARGUMENT);    
        if (index < 0 || index >= handles.length) return;
        
        if (cellBackground == null) {
            if (color == null) return;
            cellBackground = new Color [columnCount];
        }
    
        cellBackground[index] = color;
    
        Color cellBg = getBackground(index);
        boolean restore =  (cellBg == null);
        int red = cellBg != null ? cellBg.getRed() : 0;
        int green = cellBg != null ? cellBg.getGreen() : 0;
        int blue = cellBg != null ? cellBg.getBlue() : 0;
        OS.QTableWidgetItem_swt_setBackground(getHandle(index), red, green, blue, restore);
    }
    
    /**
     * Sets the checked state of the checkbox for this item.  This state change 
     * only applies if the Table was created with the SWT.CHECK style.
     *
     * @param checked the new checked state of the checkbox
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setChecked (boolean checked) {
        checkWidget();
        if ((parent.style & SWT.CHECK) == 0) return;
        if (getGrayed()) return;
        int state = checked ? OS.QT_CHECKED : OS.QT_UNCHECKED;
        parent.apiCheck = true;
        OS.QTableWidgetItem_setData(checkBoxHandle, OS.QT_CHECKSTATEROLE, state);
        parent.apiCheck = false;
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
    public void setFont (Font font){
        checkWidget ();
        if (font != null && font.isDisposed ()) SWT.error (SWT.ERROR_INVALID_ARGUMENT);

        Font oldFont = this.font;
        if (oldFont == font) return;
        this.font = font;
        if (oldFont != null && oldFont.equals (font)) return;
        
        int fontHandle = font != null ? font.handle : 0;
        for (int i = 0; i < columnCount; i++) {
            OS.QTableWidgetItem_swt_setFont(getHandle(i), fontHandle);
        }
        
        parent.updateRowHeight(this);        
    }
    
    /**
     * Sets the font that the receiver will use to paint textual information
     * for the specified cell in this item to the font specified by the 
     * argument, or to the default font for that kind of control if the 
     * argument is null.
     *
     * @param index the column index
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
    public void setFont (int index, Font font) {
        checkWidget ();
        if (font != null && font.isDisposed ()) SWT.error (SWT.ERROR_INVALID_ARGUMENT);
        if (index < 0 || index >= columnCount) return;
        
        if (cellFont == null) {
            if (font == null) return;
            cellFont = new Font [columnCount];
        }
        
        Font oldFont = cellFont [index];
        if (oldFont == font) return;
        cellFont [index] = font;
        if (oldFont != null && oldFont.equals (font)) return;
    
        int fontHandle = font != null ? font.handle : 0;
        OS.QTableWidgetItem_swt_setFont(getHandle(index), fontHandle);
        
        parent.updateRowHeight(this);
    }
    
    /**
     * Sets the receiver's foreground color to the color specified
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
     */
    public void setForeground (Color color){
        checkWidget ();
        if (color != null && color.isDisposed ()) {
            SWT.error (SWT.ERROR_INVALID_ARGUMENT);
        }
    
        foreground = color;
        
        if (checkBoxHandle != 0) {
            Color cellFg = getForeground();
            boolean restore =  (cellFg == null);
            int red = cellFg != null ? cellFg.getRed() : 0;
            int green = cellFg != null ? cellFg.getGreen() : 0;
            int blue = cellFg != null ? cellFg.getBlue() : 0;
            OS.QTableWidgetItem_swt_setForeground(checkBoxHandle, red, green, blue, restore);
        }
       
        for (int i = 0; i < columnCount; i++) {
            Color cellFg = getForeground(i);
            boolean restore =  (cellFg == null);
            int red = cellFg != null ? cellFg.getRed() : 0;
            int green = cellFg != null ? cellFg.getGreen() : 0;
            int blue = cellFg != null ? cellFg.getBlue() : 0;
            OS.QTableWidgetItem_swt_setForeground(getHandle(i), red, green, blue, restore);
        }
    }
    
    /**
     * Sets the foreground color at the given column index in the receiver 
     * to the color specified by the argument, or to the default system color for the item
     * if the argument is null.
     *
     * @param index the column index
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
     * @since 3.0
     */
    public void setForeground (int index, Color color){
        checkWidget ();
        if (color != null && color.isDisposed ()) SWT.error (SWT.ERROR_INVALID_ARGUMENT);    
        if (index < 0 || index >= handles.length) return;
    
        if (cellForeground == null) {
            if (color == null) return;
            cellForeground = new Color [columnCount];
        }
            
        cellForeground[index] = color;
        Color cellFg = getForeground(index);
        boolean restore =  (cellFg == null);
        int red = cellFg != null ? cellFg.getRed() : 0;
        int green = cellFg != null ? cellFg.getGreen() : 0;
        int blue = cellFg != null ? cellFg.getBlue() : 0;
        OS.QTableWidgetItem_swt_setForeground(getHandle(index), red, green, blue, restore);
    }
    
    /**
     * Sets the grayed state of the checkbox for this item.  This state change 
     * only applies if the Table was created with the SWT.CHECK style.
     *
     * @param grayed the new grayed state of the checkbox; 
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setGrayed (boolean grayed) {
        checkWidget();
        if ((parent.style & SWT.CHECK) == 0) return;
        
        OS.QTableWidgetItem_swt_setGrayed(checkBoxHandle, grayed);
    }
    
    /**
     * Sets the receiver's image at a column.
     *
     * @param index the column index
     * @param image the new image
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the image has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setImage (int index, Image image) {
        checkWidget ();
        if (index < 0 || index >= columnCount) return;
        if (image != null && image.isDisposed()) error(SWT.ERROR_INVALID_ARGUMENT);
        if (index == 0) this.image = image;
        
        if (cellImage == null) {
            if (image == null) return;
            cellImage = new Image [columnCount];
        }
        
        cellImage[index] = image;
        if (image != null ) {
            Rectangle imageBounds = image.getBounds();
            parent.updateImageSize(imageBounds.width, imageBounds.height);
            OS.QTableWidgetItem_setIcon(getHandle(index), Internal_GfxPackageSupport.getIconHandle(image));
        } else {
            OS.QTableWidgetItem_setIcon(getHandle(index), Internal_GfxPackageSupport.getNullIconHandle());
        }
    
        parent.updateRowHeight(false);
    }
    
    public void setImage (Image image) {
        checkWidget ();
        if (image != null && image.isDisposed()) error(SWT.ERROR_INVALID_ARGUMENT);
        super.setImage(image);
        setImage(0, image);
    }
    
    /**
     * Sets the image for multiple columns in the table. 
     * 
     * @param images the array of new images
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the array of images is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if one of the images has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setImage (Image [] images) {
        checkWidget ();
        if (images == null) error (SWT.ERROR_NULL_ARGUMENT);
        for (int i=0; i<images.length; i++) {
            setImage (i, images [i]);
        }
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
     */
    public void setText (int index, String string) {
        checkWidget ();
        _setText(index, string);
        if (parent.getWordWrap()) parent.updateRowHeight(this);
    }
    
    public void setText (String string) {
        checkWidget ();
        _setText(0, string);
        if (parent.getWordWrap()) parent.updateRowHeight(this);
    }
    
    /**
     * Sets the text for multiple columns in the table. 
     * 
     * @param strings the array of new strings
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the text is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setText (String [] strings) {
        checkWidget ();
        if (strings == null) error (SWT.ERROR_NULL_ARGUMENT);
        for (int i=0; i<strings.length; i++) {
            String string = strings [i];
            if (string != null) _setText (i, string);
        }
        if (parent.getWordWrap()) parent.updateRowHeight(this);
    }
    
    void _setText(int index, String string) {
        if (string == null) error (SWT.ERROR_NULL_ARGUMENT);
        if (0 > index || index >= columnCount) return;
        OS.QTableWidgetItem_setText(getHandle(index), string);
    }  
}
