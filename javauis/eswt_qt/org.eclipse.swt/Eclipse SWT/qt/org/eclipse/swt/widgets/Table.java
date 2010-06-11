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
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.ControlListener;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.ListUtils;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.WidgetState;

/** 
 * Instances of this class implement a selectable user interface
 * object that displays a list of images and strings and issues
 * notification when selected.
 * <p>
 * The item children that may be added to instances of this class
 * must be of type <code>TableItem</code>.
 * </p><p>
 * Style <code>VIRTUAL</code> is used to create a <code>Table</code> whose
 * <code>TableItem</code>s are to be populated by the client on an on-demand basis
 * instead of up-front.  This can provide significant performance improvements for
 * tables that are very large or for which <code>TableItem</code> population is
 * expensive (for example, retrieving values from an external source).
 * </p><p>
 * Here is an example of using a <code>Table</code> with style <code>VIRTUAL</code>:
 * <code><pre>
 *  final Table table = new Table (parent, SWT.VIRTUAL | SWT.BORDER);
 *  table.setItemCount (1000000);
 *  table.addListener (SWT.SetData, new Listener () {
 *      public void handleEvent (Event event) {
 *          TableItem item = (TableItem) event.item;
 *          int index = table.indexOf (item);
 *          item.setText ("Item " + index);
 *          System.out.println (item.getText ());
 *      }
 *  }); 
 * </pre></code>
 * </p><p>
 * Note that although this class is a subclass of <code>Composite</code>,
 * it does not make sense to add <code>Control</code> children to it,
 * or set a layout on it.
 * </p><p>
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>SINGLE, MULTI, CHECK, FULL_SELECTION, HIDE_SELECTION, VIRTUAL</dd>
 * <dt><b>Events:</b></dt>
 * <dd>Selection, DefaultSelection, SetData, MeasureItem, EraseItem, PaintItem</dd>
 * </dl>
 * </p><p>
 * Note: Only one of the styles SINGLE, and MULTI may be specified.
 * </p><p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */
public class Table extends Composite {
    /*
     * Items and columns of the Table
     */
    TableItem[] items;
    TableColumn[] columns;

    /*
     * Number of valid items in items and columns arrays (the arrays can be
     * bigger than the amount of items stored in them)
     */
    int itemCount;
    int columnCount; 
    
    /*
     * The item that currently has focus
     */
    TableItem currentItem;
    
    /*
     * Minimum item height
     */
    int minimumHeight;
    
    /*
     * The row that contains the item with biggest preferred height
     */
    int heightRow;
    
    /*
     * The value returned in the last indexOf() call (in case the item was
     * not found from native QTableWidget. This value gets used when Table
     * is created with SWT.VIRTUAL style.
     */
    int lastIndexOf;
      
    /*
     * Key code that causes the state of check box to change. Internally focus
     * is always in some other QTableWidget cell than the one that has the 
     * check box, so we must intercept the state change key and change the check 
     * box state manually. 
     */
    int itemStateChangeKey;
    
    /*
     * Flags apiSelection and apiCheck are used to prevent us from sending
     * selection events when Table state is changed/items are selected through 
     * the API.
     */
    boolean apiSelection;
    boolean apiCheck;

    /*
     * This is used to store the state we think currently focused item is in.
     * If user changes the checked state by clicking on the check box we can
     * check in the signal handler if the current item's check state is the
     * one we think it is. If it's not, a selection event is generated.
     */
    boolean currentItemChecked;
    
    /*
     * This is used to store the image size of the Table (the first image that
     * is added to the Table sets the size of all images) 
     */
    Point imageSize = null;
    
    /*
     * Flag indicating if TableItem word wrap extension is in use.
     */
    boolean wrap;
    
    /*
     * Flag to check between the press and release event, 
     * mouse is not moved over the clicked item.
     */
    boolean mouseSelection;
    
    /*
     * For press and relase mouse events are occured on the same item.
     */
    int oldIndex = -1;
    
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
     * @see SWT#SINGLE
     * @see SWT#MULTI
     * @see SWT#CHECK
     * @see SWT#FULL_SELECTION
     * @see SWT#HIDE_SELECTION
     * @see SWT#VIRTUAL
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public Table (Composite parent, int style) {
        this(parent, style, false);
    }
    
    /**
     * <p>
     * <b>IMPORTANT:</b> This constructor is <em>not</em> part of the SWT
     * public API. It should never be referenced from application code. 
     * </p>
     */
    protected Table(Composite parent, int style, boolean isExtended) {
        super (parent, checkStyle (style), 0, null, isExtended);
    }

    TableItem _getItem (int index) {
        if ((style & SWT.VIRTUAL) == 0) return items [index];
        if (items [index] != null) return items [index];    
        return items [index] = new TableItem (this, SWT.NONE, index, false);
    }
    
    static int checkStyle (int style) {
        /*
         * To be compatible with Windows, force the H_SCROLL
         * and V_SCROLL style bits.  On Windows, it is not
         * possible to create a table without scroll bars.
         */
        style |= SWT.FULL_SELECTION | SWT.H_SCROLL | SWT.V_SCROLL;
        
        if(((style & SWT.CHECK) != 0 || (style & SWT.MULTI) != 0) && (style & SWT.RADIO) != 0){
            style = style & (~SWT.RADIO);
        }
        return checkBits (style, SWT.SINGLE, SWT.MULTI, 0, 0, 0, 0);
    }
    
    boolean checkData (TableItem item, int index) {
        if (item.cached) return true;
        if ((style & SWT.VIRTUAL) != 0) {
            Event event = new Event ();
            event.item = item;
            event.index = index;
            item.ensureValid(index);
            sendEvent (SWT.SetData, event);
            if (isDisposed ()) return false;
            if (item.isDisposed ()) return false;
        }
        if (isDisposed ()) return false;
        if (item == null || item.isDisposed ()) return false;
        return true;
        
    }
    
    boolean checkData (TableItem item) {
        return checkData(item, indexOf(item));
    }
    
    protected void checkSubclass () {
        if (!isValidSubclass ()) error (SWT.ERROR_INVALID_SUBCLASS);
    }
    
    int getBackgroundColorRole() {
        return OS.QPALETTE_COLORROLE_BASE;
    }
    
    int getFocusIndex() {
        checkWidget();
        return OS.QTableWidget_currentRow(topHandle);
    }
    
    int getForegroundColorRole() {
        return OS.QPALETTE_COLORROLE_TEXT;
    }
    
    /*
     * Gets the index of an item
     * @param item Item whose index is needed
     * @return the index or -1 if not found
     */
    int getItemIndex(TableItem item) {
        if (itemCount == 0 || items == null ||
            item == null || item.parent != this) return -1;
        
        int index = OS.QTableWidget_row(topHandle, item.topHandle());
        
        if (index != -1) return index;
        
        /**
         * Usually we don't come here at all, but in VIRTUAL style table case
         * we can be searching for an item that does not yet have a native counterpart.
         * In that case use the following algorithm (directly from the GTK implementation)
         *  - See if the wanted item is the one that was searched previously
         *    or a neighbor of the previously searched item
         *  - If last searched item was in the first half of the item list,
         *    start search from the beginning of the list
         *  - If not, start search from the end
         */
        
        if (1 <= lastIndexOf && lastIndexOf < itemCount - 1) {
            if (items [lastIndexOf] == item) return lastIndexOf;
            if (items [lastIndexOf + 1] == item) return ++lastIndexOf;
            if (items [lastIndexOf - 1] == item) return --lastIndexOf;
        }
        if (lastIndexOf < itemCount / 2) {
            for (int i=0; i<itemCount; i++) {
                if (items [i] == item) return lastIndexOf = i;
            }
        } else {
            for (int i=itemCount - 1; i>=0; --i) {
                if (items [i] == item) return lastIndexOf = i;
            }
        }
        return -1;
    }
    
    /*
     * Returns the native QTableWidget index of a column
     * @param index Java side index
     * @return native side index
     */
    int getNativeColumnIndex(int index) {
        /**
         * Native table always has check box column, although it may be 
         * invisible. Therefore native column index is always Java side
         * column index + 1.
         */
        return index + 1;
    }
    
    /*
     * Returns the native QTableWidget index of a column
     * @param column the TableColumn
     * @return native side index
     */
    int getNativeColumnIndex(TableColumn column) {
        int index = getColumnIndex(column);
        
        if (index != -1) return getNativeColumnIndex(index);
        return -1;
    }
    
    /*
     * Returns the Java side index of a column
     * @param column the TableColumn
     * @return Java side index
     */
    int getColumnIndex(TableColumn column) {
        if (columnCount == 0 || columns == null) return -1;
            
        for (int i = 0; i < columns.length; i++) {
            if (columns[i] == column) {
                return i;
            }
        }
        
        return -1;
    }
    
    /*
     * Returns the Java side index of the currently focused column
     * @return Java side index
     */
    int getCurrentColumnIndex() {
        return getCurrentColumnIndex(false);
    }
    
    /*
     * Returns the Native or Java side index of currently focused column
     * @param nativeIndex if true, return native side index, if false return Java side index
     * @return the index
     */
    int getCurrentColumnIndex(boolean nativeIndex) {
        int index = OS.QTableWidget_currentColumn(topHandle);
        
        if (nativeIndex) {
            return index;
        }
        // Native Table always has check box column which does not exist on
        // the Java side.
        if (index != -1) return index - 1;
        return index;
    }
    
    /*
     * Inserts a new TableColumn at index
     * @param column the new column
     * @param index index where column is inserted
     */
    void createColumn(TableColumn column, int index) {
        if (columnCount == columns.length) {
            int length = Math.max (4, columns.length * 3 / 2);
            TableColumn [] newColumns = new TableColumn [length]; 
            System.arraycopy (columns, 0, newColumns, 0, columns.length);
            columns = newColumns;
        }
    
        // Put the new item in the correct place in item array
        System.arraycopy (columns, index, columns, index + 1, columnCount++ - index);
        columns[index] = column;
        
        // If the table switched from list mode to column mode we don't need
        // to add a native column, it is already there.
        if (columnCount > 1) {
            OS.QTableWidget_insertColumn(topHandle, getNativeColumnIndex(index));
            
            for (int i = 0; i < itemCount; i++) {
                items[i].addColumn(i, index);
            }
        }
    }
    
    /*
     * Inserts a new TableItem at index
     * @param item the new item
     * @param index index where item is inserted
     */
    void createItem (TableItem item, int index) {
        if (!(0 <= index && index <= itemCount)) error (SWT.ERROR_INVALID_RANGE);
        if (itemCount == items.length) {
            int length = Math.max (4, items.length * 3 / 2);
            TableItem [] newItems = new TableItem [length]; 
            System.arraycopy (items, 0, newItems, 0, items.length);
            items = newItems;
        }
    
        // Put the new item in the correct place in item array
        System.arraycopy (items, index, items, index + 1, itemCount++ - index);
        items[index] = item;
    }
    
    /*
     * Removes item from index
     * @param index index to remove from
     */
    void removeItem(int index) {
        if (!(0 <= index && index <= itemCount)) error (SWT.ERROR_INVALID_RANGE);
        TableItem item = _getItem(index);
        
        if (item != null && !item.isDisposed()) {
            // in radio style at least one button should select
            if ((style & SWT.SINGLE) != 0 && (style & SWT.RADIO) != 0 && itemCount > 1) {
                if(OS.QAbstractButton_isChecked(items[index].radioButtonHandle)){
                    if(index>0){
                        OS.QAbstractButton_setChecked(items[index-1].radioButtonHandle, true);
                    } else if(index == 0){
                        OS.QAbstractButton_setChecked(items[index+1].radioButtonHandle, true);
                    }
                }
            }
            
            item.release(false);
        }
        
        if (index == heightRow) heightRow = -1;
        OS.QTableWidget_removeRow(topHandle, index);
        System.arraycopy (items, index + 1, items, index, --itemCount - index);
        items [itemCount] = null;
    }
    
    /*
     * Updates the global image size for Table. After the image size has been set
     * once it cannot be reset.
     * @param width new width
     * @param height new height
     */
    void updateImageSize(int width, int height) {
        if (imageSize == null) {
            imageSize = new Point(width, height);
            OS.QAbstractItemView_setIconSize(topHandle, width, height);
        }
        
        if (imageSize.y > getItemHeight()) {
            updateRowHeight(false);
        }
    }
    
    /*
     * Sets row height for a single item in a wrap style Table
     */
    void updateRowHeight(TableItem item) {
        int handle = topHandle;
        int index = indexOf(item);
        if (wrap) {
            int height = OS.QTableView_rowHeight(handle, index);        
            int newHeight = OS.QAbstractItemView_sizeHintForRow(handle, index);
            
            if (imageSize != null && imageSize.y > newHeight) {
                newHeight = imageSize.y;
            }
            
            newHeight = Math.max(minimumHeight, newHeight);
            
            if (newHeight != height) {
                OS.QTableView_setRowHeight(handle, index, newHeight);
            }
        } else {
            int heightHint = OS.QAbstractItemView_sizeHintForRow(handle, index);
            int itemHeight = _getItemHeight();
                        
            if (heightHint > itemHeight) {
                // If height hint is bigger than the current row height, no need
                // to go through the rest of the items, just set the new height
                heightHint = Math.max(minimumHeight, heightHint);
                heightRow = index;
                
                for (int row = 0; row < itemCount; row++) {
                    OS.QTableView_setRowHeight(handle, row, heightHint);
                }
            } else if (index == heightRow && heightHint < itemHeight) {
                // If the content of the row with bigger preferred height
                // changes, update the row heights
                updateRowHeight(false);
            }
        }
    }
    
    /*
     * Finds out if some row in the Table would want to be higher than the
     * current row height, and updates the row height if necessary.
     * If ignoreState == true, current state of the table is ignored and
     * the sizes are updated to all items regardless of it seems necessary
     * or not (needed for changing between wrap states).
     */
    void updateRowHeight(boolean ignoreState) {
        if (wrap) {
            int handle = topHandle;
            for (int row = 0; row < itemCount; row++) {
                int height = OS.QTableView_rowHeight(handle, row);
                int newHeight = OS.QAbstractItemView_sizeHintForRow(handle, row);
                
                if (imageSize != null && imageSize.y > newHeight) {
                    newHeight = imageSize.y;
                }
                
                newHeight = Math.max(minimumHeight, newHeight);
                
                if (newHeight != height) {
                    OS.QTableView_setRowHeight(handle, row, newHeight);
                }
            }
        } else {
            int itemHeight = getItemHeight();
            if (ignoreState) itemHeight = minimumHeight;
            int maxHeight = 0;
            int handle = topHandle;
            
            // Find out the maximum row height
            for (int row = 0; row < itemCount; row++) {
                int heightHint = OS.QAbstractItemView_sizeHintForRow(handle, row);
                
                if (heightHint > maxHeight) {
                    heightRow = row;
                    maxHeight = heightHint;
                }
            }
        
            if (imageSize != null && imageSize.y > maxHeight) {
                maxHeight = imageSize.y;
            }
            
            maxHeight = Math.max(minimumHeight, maxHeight);

            // Apply the maximum row height to all rows 
            if (ignoreState || maxHeight != itemHeight) {
                for (int row = 0; row < itemCount; row++) {
                    OS.QTableView_setRowHeight(handle, row, maxHeight);
                }
            } 
        }
    }
    
    /**
     * Adds the listener to the collection of listeners who will
     * be notified when the user changes the receiver's selection, by sending
     * it one of the messages defined in the <code>SelectionListener</code>
     * interface.
     * <p>
     * When <code>widgetSelected</code> is called, the item field of the event object is valid.
     * If the receiver has the <code>SWT.CHECK</code> style and the check selection changes,
     * the event object detail field contains the value <code>SWT.CHECK</code>.
     * <code>widgetDefaultSelected</code> is typically called when an item is double-clicked.
     * The item field of the event object is valid for default selection, but the detail field is not used.
     * </p>
     *
     * @param listener the listener which should be notified when the user changes the receiver's selection
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
        checkWidget ();
        if (listener == null) error (SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener (listener);
        addListener (SWT.Selection, typedListener);
        addListener (SWT.DefaultSelection, typedListener);
    }
    
    
    void createHandle_pp (int index) {
    
        topHandle = scrollAreaHandle = frameHandle = OS.QTableWidget_new();
        handle = OS.QAbstractScrollArea_viewPort(topHandle);
        state |= WidgetState.HANDLE;
        state &= ~WidgetState.CANVAS;
    
        if(OS.windowServer == OS.WS_X11) { 
            itemStateChangeKey = ' ';
        } else if (OS.windowServer == OS.WS_SYMBIAN_S60) {
            itemStateChangeKey = -5;
        } else {
            itemStateChangeKey = 0;
        }
        
        // Store the default height of an item, this is used as the minimum
        // height any item can be
        minimumHeight = _getItemHeight();
        
        // By default text of table items is not wrapped
        OS.QTableView_setWordWrap(topHandle, false);
        
        // Check box column and list mode column
        OS.QTableWidget_setColumnCount(topHandle, 2);
        
        // Set up check box column
        OS.QHeaderView_setResizeMode(OS.QTableWidget_swt_header(topHandle, true), 0, OS.QT_RESIZEMODEFIXED);
        
        // Set header texts of the two columns to "" by default
        OS.QTableWidget_swt_setHeaderText(topHandle, 0, "");
        OS.QTableWidget_swt_setHeaderText(topHandle, 1, "");
           
        if ((style & SWT.CHECK) == 0 && (style & SWT.RADIO) == 0) {
            OS.QTableView_setColumnHidden(topHandle, 0, true);
        } else {
            // For check style table, create a temporary item to find
            // out proper size for the check box column
            int width = 0;
            OS.QTableWidget_insertRow(topHandle, 0);
            if(((style & SWT.SINGLE) != 0 && (style & SWT.RADIO) != 0) || ((style & SWT.CHECK) != 0)){
                int radioButtonHandle = OS.QRadioButton_new();
                OS.QWidget_setFocusPolicy(radioButtonHandle, OS.QT_FOCUSPOLICY_NOFOCUS);
                OS.QTableWidget_setCellWidget(topHandle, 0, 0, radioButtonHandle);
                width = OS.QWidget_sizeHint(radioButtonHandle).x;
            }
            OS.QTableView_setColumnWidth(topHandle, 0, width);
            OS.QTableWidget_removeRow(topHandle, 0);
        }
        
        // Set up selection mode (MULTI/SINGLE)
        if ((style & SWT.MULTI) != 0) {
            OS.QAbstractItemView_setSelectionMode(topHandle, OS.QT_ABSTRACTITEMVIEW_SELECTIONMODE_MULTI);
        } else {
            OS.QAbstractItemView_setSelectionMode(topHandle, OS.QT_ABSTRACTITEMVIEW_SELECTIONMODE_SINGLE);
        }
        
        // This should always be true, since we force the FULL_SELECTION style
        if ((style & SWT.FULL_SELECTION) != 0) {
            OS.QAbstractItemView_setSelectionBehavior(topHandle, OS.QT_ABSTRACTITEMVIEW_SELECTIONBEHAVIOR_ROWS);
        }
        
        // Set both headers invisible by default
        OS.QTableWidget_swt_setHeaderVisible(topHandle, true, false);
        OS.QTableWidget_swt_setHeaderVisible(topHandle, false, false);
        
        OS.QTableView_setShowGrid(topHandle, false);
        
        addControlListener(new ControlListener() {
            public void controlMoved(ControlEvent e) {
            }

            public void controlResized(ControlEvent e) {
                if (columnCount == 0) {
                    // For a list mode Table make content fill the available space
                    int availableWidth = getClientArea().width;
                    
                    if ((style & SWT.CHECK) != 0 || (style & SWT.RADIO) != 0) {
                        availableWidth -= OS.QTableView_columnWidth(topHandle, 0);
                    }
                     
                    OS.QTableView_setColumnWidth(topHandle, 1, availableWidth);
                    
                    if (wrap) {
                        updateRowHeight(false);
                    }
                }               
            }});    
    }
    
    void createWidget (int index) {
        super.createWidget (index);
        items = new TableItem [4];
        columns = new TableColumn [4];
        itemCount = columnCount = 0;
    }
    
    void destroyItem (TableColumn column) {
        int index = 0;
        while (index < columnCount) {
            if (columns [index] == column) break;
            index++;
        }
        if (index == columnCount) return;
        
        System.arraycopy (columns, index + 1, columns, index, --columnCount - index);
        columns [columnCount] = null;
        
        if (columnCount > 0) {
            OS.QTableWidget_removeColumn(topHandle, getNativeColumnIndex(index));
    
            for (int i = 0; i < itemCount; i++) {
                items[i].removeColumn(index);
            }
        }
    }
    
    void destroyItem (TableItem item) {
        int index = 0;
        while (index < itemCount) {
            if (_getItem(index) == item) break;
            index++;
        }
        if (index == itemCount) return;
    
        removeItem(index);
    }
        
    /**
     * Deselects the item at the given zero-relative index in the receiver.
     * If the item at the index was already deselected, it remains
     * deselected. Indices that are out of range are ignored.
     *
     * @param index the index of the item to deselect
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void deselect (int index) {
        checkWidget();
        if (index < 0 || index >= itemCount) return;
        // single selection radio style doesn't support deselect
        if (!((style & SWT.SINGLE) != 0 && (style & SWT.RADIO) != 0)) {
            _getItem(index).setSelected(false);
        }
    }
    
    /**
     * Deselects the items at the given zero-relative indices in the receiver.
     * If the item at the given zero-relative index in the receiver 
     * is selected, it is deselected.  If the item at the index
     * was not selected, it remains deselected.  The range of the
     * indices is inclusive. Indices that are out of range are ignored.
     *
     * @param start the start index of the items to deselect
     * @param end the end index of the items to deselect
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void deselect (int start, int end) {
        checkWidget();
        for (int i = start; i <= end; i++) {
            deselect(i);
        }
    }
    
    /**
     * Deselects the items at the given zero-relative indices in the receiver.
     * If the item at the given zero-relative index in the receiver 
     * is selected, it is deselected.  If the item at the index
     * was not selected, it remains deselected. Indices that are out
     * of range and duplicate indices are ignored.
     *
     * @param indices the array of indices for the items to deselect
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the set of indices is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void deselect (int [] indices) {
        checkWidget();
        if (indices == null) error (SWT.ERROR_NULL_ARGUMENT);
        if (indices.length == 0) return;
        for (int i = 0; i < indices.length; i++) {
            deselect(indices[i]);
        }
    }
    
    /**
     * Deselects all selected items in the receiver.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void deselectAll () {
        checkWidget();
        apiSelection = true;
        OS.QAbstractItemView_clearSelection(topHandle);
        if ((style & SWT.SINGLE) != 0) {
            OS.QTableWidget_setCurrentItem(topHandle, 0);
            if ((style & SWT.RADIO) != 0 && itemCount > 0) {
                OS.QAbstractButton_setChecked(items[0].radioButtonHandle, true);
            }
        }
        apiSelection = false;
    }
        
    /**
     * Returns the column at the given, zero-relative index in the
     * receiver. Throws an exception if the index is out of range.
     * Columns are returned in the order that they were created.
     * If no <code>TableColumn</code>s were created by the programmer,
     * this method will throw <code>ERROR_INVALID_RANGE</code> despite
     * the fact that a single column of data may be visible in the table.
     * This occurs when the programmer uses the table like a list, adding
     * items but never creating a column.
     *
     * @param index the index of the column to return
     * @return the column at the given index
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_RANGE - if the index is not between 0 and the number of elements in the list minus 1 (inclusive)</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     * 
     * @see Table#getColumnOrder()
     * @see Table#setColumnOrder(int[])
     * @see TableColumn#getMoveable()
     * @see TableColumn#setMoveable(boolean)
     * @see SWT#Move
     */
    public TableColumn getColumn (int index) {
        checkWidget();
        if (!(0 <= index && index < columnCount)) error (SWT.ERROR_INVALID_RANGE);
        return columns [index];
    }
    
    /**
     * Returns the number of columns contained in the receiver.
     * If no <code>TableColumn</code>s were created by the programmer,
     * this value is zero, despite the fact that visually, one column
     * of items may be visible. This occurs when the programmer uses
     * the table like a list, adding items but never creating a column.
     *
     * @return the number of columns
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getColumnCount () {
        checkWidget();
        return columnCount;
    }
    
    /**
     * Returns an array of <code>TableColumn</code>s which are the
     * columns in the receiver.  Columns are returned in the order
     * that they were created.  If no <code>TableColumn</code>s were
     * created by the programmer, the array is empty, despite the fact
     * that visually, one column of items may be visible. This occurs
     * when the programmer uses the table like a list, adding items but
     * never creating a column.
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
     * 
     * @see Table#getColumnOrder()
     * @see Table#setColumnOrder(int[])
     * @see TableColumn#getMoveable()
     * @see TableColumn#setMoveable(boolean)
     * @see SWT#Move
     */
    public TableColumn [] getColumns () {
        checkWidget();
        TableColumn [] result = new TableColumn [columnCount];
        System.arraycopy (columns, 0, result, 0, columnCount);
        return result;
    }
    
    /**
     * Returns the width in pixels of a grid line.
     *
     * @return the width of a grid line in pixels
     * 
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getGridLineWidth () {
        checkWidget();
        return OS.QTableView_showGrid(topHandle) ? 1 : 0;
    }
    
    /**
     * Returns the height of the receiver's header 
     *
     * @return the height of the header or zero if the header is not visible
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     * 
     * @since 2.0 
     */
    public int getHeaderHeight () {
        checkWidget ();
        return _getHeaderHeight();
    }
    
    int _getHeaderHeight() {
        if (OS.QTableWidget_swt_headerVisible(topHandle, true) == false) {
            return 0;
        }
        return OS.QTableWidget_swt_headerHeight(topHandle, true);    
    }
    
    /**
     * Returns <code>true</code> if the receiver's header is visible,
     * and <code>false</code> otherwise.
     * <p>
     * If one of the receiver's ancestors is not visible or some
     * other condition makes the receiver not visible, this method
     * may still indicate that it is considered visible even though
     * it may not actually be showing.
     * </p>
     *
     * @return the receiver's header's visibility state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean getHeaderVisible () {
        checkWidget();
        return OS.QTableWidget_swt_headerVisible(topHandle, true);
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
    public TableItem getItem (int index) {
        checkWidget();
        if (!(0 <= index && index < itemCount)) error (SWT.ERROR_INVALID_RANGE);
        
        // If table is VIRTUAL, the item may not be fully initialized,
        // make sure that it is before returning to user
        TableItem item = _getItem (index);
        item.ensureValid(index);
        return item;
    }
    
    /**
     * Returns the item at the given point in the receiver
     * or null if no such item exists. The point is in the
     * coordinate system of the receiver.
     * <p>
     * The item that is returned represents an item that could be selected by the user.
     * For example, if selection only occurs in items in the first column, then null is 
     * returned if the point is outside of the item. 
     * Note that the SWT.FULL_SELECTION style hint, which specifies the selection policy,
     * determines the extent of the selection.
     * </p>
     *
     * @param point the point used to locate the item
     * @return the item at the given point, or null if the point is not in a selectable item
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the point is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public TableItem getItem (Point point) {
        checkWidget();
        if (point == null) error (SWT.ERROR_NULL_ARGUMENT);
        
        int index = -1;    
        int itemHandle = OS.QTableWidget_itemAt(topHandle, point.x, point.y);
        
        if (itemHandle != 0) {
            index = OS.QTableWidget_row(topHandle, itemHandle);
        }
        
        if (index >= 0 && index < itemCount) {
            TableItem item = _getItem (index);
            item.ensureValid(index);
            return item;
        }
        
        return null;
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
    public int getItemCount () {
        checkWidget ();
        return itemCount;
    }
    
    /**
     * Returns the height of the area which would be used to
     * display <em>one</em> of the items in the receiver's.
     *
     * @return the height of one item
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getItemHeight () {
        checkWidget();
        return _getItemHeight();
    }
    
    int _getItemHeight() {
        int itemHeight = 0;
        
        if (itemCount > 0) {
            itemHeight = OS.QTableView_rowHeight(topHandle, 0);
        } else {
            // If the table doesn't have any items, rowHeight() will 
            // return 0. So set the row count temporarily to 1 to get
            // a meaningful default row height
            OS.QTableWidget_setRowCount(topHandle, 1);
            itemHeight = OS.QTableView_rowHeight(topHandle, 0);
            OS.QTableWidget_setRowCount(topHandle, 0);
        }
        
        return itemHeight;
    }
    
    /**
     * Returns a (possibly empty) array of <code>TableItem</code>s which
     * are the items in the receiver. 
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
    public TableItem [] getItems () {
        checkWidget();
        TableItem [] result = new TableItem [itemCount];
        if ((style & SWT.VIRTUAL) != 0) {
            for (int i=0; i<itemCount; i++) {
                result [i] = _getItem (i);
            }
        } else {
            System.arraycopy (items, 0, result, 0, itemCount);
        }
        return result;
    }
    
    /**
     * Returns <code>true</code> if the receiver's lines are visible,
     * and <code>false</code> otherwise.
     * <p>
     * If one of the receiver's ancestors is not visible or some
     * other condition makes the receiver not visible, this method
     * may still indicate that it is considered visible even though
     * it may not actually be showing.
     * </p>
     *
     * @return the visibility state of the lines
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean getLinesVisible() {
        checkWidget();
        return OS.QTableView_showGrid(topHandle);
    }
    
    
    /**
     * Returns an array of <code>TableItem</code>s that are currently
     * selected in the receiver. The order of the items is unspecified.
     * An empty array indicates that no items are selected.
     * <p>
     * Note: This is not the actual structure used by the receiver
     * to maintain its selection, so modifying the array will
     * not affect the receiver. 
     * </p>
     * @return an array representing the selection
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public TableItem [] getSelection () {
        int [] selection = getSelectionIndices();
        
        TableItem [] selectedItems = new TableItem [selection.length];
        
        for (int i = 0; i < selection.length; i++) {
            selectedItems[i] = items[selection[i]];
        }
        
        return selectedItems;
    }
    
    /**
     * Returns the number of selected items contained in the receiver.
     *
     * @return the number of selected items
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getSelectionCount () {
        checkWidget();
        return OS.QTableWidget_swt_selectionCount(topHandle);
    }
    
    /**
     * Returns the zero-relative index of the item which is currently
     * selected in the receiver, or -1 if no item is selected.
     *
     * @return the index of the selected item
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getSelectionIndex () {
        checkWidget();
    
        if ((style & SWT.MULTI) != 0) {
            int [] selection = getSelectionIndices();
            
            if (selection.length > 0) {
                return selection[0];
            } else {
                return -1;
            }
        } else if ((style & SWT.SINGLE) != 0 && (style & SWT.RADIO) != 0) {
            return _getSelectionIndex();
        }
        return OS.QTableWidget_currentRow(topHandle);
    }
    
    /**
     * Returns the zero-relative indices of the items which are currently
     * selected in the receiver. The order of the indices is unspecified.
     * The array is empty if no items are selected.
     * <p>
     * Note: This is not the actual structure used by the receiver
     * to maintain its selection, so modifying the array will
     * not affect the receiver. 
     * </p>
     * @return the array of indices of the selected items
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int [] getSelectionIndices () {
        checkWidget();
        if((style & SWT.SINGLE) != 0 && (style & SWT.RADIO) != 0){
            return new int[] {_getSelectionIndex()};
        }
        return OS.QTableWidget_swt_selectedItems(topHandle);
    }
    
    int _getSelectionIndex(){
        int index = 0;
       
        for(int i=0;i<itemCount;i++){
            TableItem item = getItem(i);
            if(item.getRadioSelected()){
                index = i;
                break;
            }
        }
        return index;
    }
    
    /**
     * Returns the zero-relative index of the item which is currently
     * at the top of the receiver. This index can change when items are
     * scrolled or new items are added or removed.
     *
     * @return the index of the top item
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getTopIndex () {
        checkWidget();
        return OS.QTableWidget_topIndex(topHandle);
    }
    
    boolean getWordWrap() {
        checkWidget();
        return wrap;
    }
    
    void hookEvents_pp () {
        super.hookEvents_pp ();
        
        if(!((style & SWT.SINGLE) != 0 && (style & SWT.RADIO) != 0 )){
            int itemSelectionChangedProxy = OS.SignalHandler_new(topHandle, 
                OS.QSIGNAL_TABLE_ITEMSELECTION_CHANGED);
            OS.QObject_connectOrThrow(topHandle, "itemSelectionChanged()", itemSelectionChangedProxy,
                "widgetSignal()", OS.QT_AUTOCONNECTION);
        }
        
        int currentCellChangedProxy = OS.SignalHandler_new(topHandle, 
                OS.QSIGNAL_TABLE_CURRENTCELL_CHANGED);
        OS.QObject_connectOrThrow(topHandle, "currentCellChanged(int,int,int,int)", currentCellChangedProxy,
                "widgetSignal(int,int,int,int)", OS.QT_AUTOCONNECTION);
        
        int cellChangedProxy = OS.SignalHandler_new(topHandle, 
                OS.QSIGNAL_TABLE_CELL_CHANGED);
        OS.QObject_connectOrThrow(topHandle, "cellChanged(int,int)", cellChangedProxy,
                "widgetSignal(int,int)", OS.QT_AUTOCONNECTION);
        
        int cellActivatedProxy = OS.SignalHandler_new(topHandle, 
                OS.QSIGNAL_TABLE_CELL_ACTIVATED);
        OS.QObject_connectOrThrow(topHandle, "cellActivated(int,int)", cellActivatedProxy,
                "widgetSignal(int,int)", OS.QT_AUTOCONNECTION);
    }
    
    void sendSelectionEvent() {
        int row = OS.QTableWidget_currentRow(topHandle);
        
        if (currentItem != null && row != -1) {
            Event event = new Event();
            event.widget = this;
            event.item = _getItem(row);
            event.doit = true;
            sendEvent(SWT.Selection, event, true);
        }
    }
    
    void sendCheckEvent() {
        if (currentItem != null && (style & SWT.CHECK) != 0 && checkData(currentItem)) {
            if (currentItemChecked != currentItem.getChecked()) {
                currentItemChecked = currentItem.getChecked();
                Event event = new Event();
                event.widget = this;
                event.item = currentItem;
                event.detail = SWT.CHECK;
                event.doit = true;
                sendEvent(SWT.Selection, event, true);
                
                if (event.doit == false) {
                    currentItem.setChecked(!currentItemChecked);
                    currentItemChecked = !currentItemChecked;
                }
            }
        }
    }
    
    /*
     * Order of signals
     *  1) currentCellChanged     - Currently focused cell changes
     *  2) itemSelectionChanged   - Item selection changes
     *  3) cellPressed            - A cell was pressed
     *  4) cellChanged            - Cell state was changed (checked, text, etc)
     */
    
    void qt_signal_table_itemSelectionChanged()
    {
        if (!apiSelection) {
            sendSelectionEvent();
        }
    }
    
    void qt_signal_table_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
    {
        if (currentRow != -1 && currentColumn == 0) {
            // Never let focus go to check box column, that would cause
            // problems in event handling
            OS.QTableWidget_setCurrentCell(topHandle, currentRow, 1);
        }
        
        if (currentRow != -1 && items.length > 0) {
            currentItem = _getItem(currentRow);
            if ((style & SWT.CHECK) != 0 && checkData(currentItem, currentRow)) {
                currentItemChecked = currentItem.getChecked();
            }
        }   
    }
    void qt_signal_table_cellChanged(int row, int column)
    {
        if (!apiCheck) {
            sendCheckEvent();
        }
    }
    
    boolean qt_event_paint(int widgetHandle, int x, int y, int width, int height, int regionHandle)
    {
        if (widgetHandle == handle && (style & SWT.VIRTUAL) != 0) {
            // If table items don't fill the whole vertical painted area, the row
            // found in paint coordinates may be -1. If end row == -1 then last item
            // is visible and part of the view does not have any items in it. Start
            // should never be -1 if any items exist.
            int start = OS.QTableView_rowAt(topHandle, y);
            int end = OS.QTableView_rowAt(topHandle, y+height);
            
            if (start != -1 || end != -1) {
                start = Math.max(0, start);
                if (end == -1) end = itemCount-1;
                
                for (int i = start; i <= end; i++) {
                    lastIndexOf = i;
                    checkData(_getItem(i), i);
                }
            }
        }
       
        return super.qt_event_paint(widgetHandle, x, y, width, height, regionHandle); 
    }
    
    boolean qt_event_keypress_pp( int widgetHandle, int key, int modifier, int character, int nativeScanCode )
    {
        boolean retval = super.qt_event_keypress_pp(widgetHandle, key, modifier, character, nativeScanCode);
        
        if (items != null && itemCount > 0) {
            // Do not implement wrapping at Up/Down keypress for now.
            /*if (key == OS.QT_KEY_UP && OS.QTableWidget_currentRow(topHandle) == 0) {
                OS.QTableWidget_setCurrentCell(topHandle, itemCount-1, OS.QTableWidget_currentColumn(topHandle));
            } else if (key == OS.QT_KEY_DOWN && OS.QTableWidget_currentRow(topHandle) == itemCount-1) {
                OS.QTableWidget_setCurrentCell(topHandle, 0, OS.QTableWidget_currentColumn(topHandle));
            } else*/ 
            
            if (character == itemStateChangeKey && currentItem != null) {
                
                if((style & SWT.SINGLE) != 0 && (style & SWT.RADIO) != 0 && itemCount>0){
                    OS.QAbstractButton_setChecked(currentItem.radioButtonHandle, true);
                    sendSelectionEvent();
                }else {
                    currentItem.setChecked(!currentItemChecked);
                    sendCheckEvent();
                }
                
            }
        }  
        
        return retval;
    }
    /*
     * Three Mouse events(press, move and release) are needed to select the radio button, 
     * when the user clicks(press event) on tableItem and release event is also 
     * happened without moving away(move event) from that time 
     */
    boolean qt_event_mouseButtonPress_pp(int widgetHandle, int button,
            int x, int y, int state, int buttons) {
       
        if(widgetHandle == handle && (style & SWT.SINGLE) != 0 && (style & SWT.RADIO) != 0){
            
            int itemHandle = OS.QTableWidget_itemAt(topHandle, x, y);
            
            if (itemHandle != 0) {
                oldIndex = OS.QTableWidget_row(topHandle, itemHandle);
            }
            mouseSelection = true;
        }
        return super.qt_event_mouseButtonPress_pp(widgetHandle, button, x, y, state, buttons);
    }

    boolean qt_event_mouseButtonRelease_pp(int widgetHandle, int button,
            int x, int y, int state, int buttons) {
       
        if(widgetHandle == handle && mouseSelection && (style & SWT.SINGLE) != 0 && (style & SWT.RADIO) != 0){
            // there is a chance that if we click on radio button, oldIndex is not going to update
            // in the mouseButtonPress. 
            if(oldIndex != -1){
                OS.QAbstractButton_setChecked(items[oldIndex].radioButtonHandle, true);
                sendSelectionEvent();
            } else if(currentItem != null){
                Rectangle rect = OS.QTableWidget_visualItemRect(topHandle, currentItem.topHandle()); 
                if((rect.y <= y) && (y <= (rect.y+rect.height)) && (x<=rect.x+rect.width)){
                    OS.QAbstractButton_setChecked(currentItem.radioButtonHandle, true);
                    sendSelectionEvent();
                }
            }
            mouseSelection = false;
            oldIndex = -1;
        }
        return super.qt_event_mouseButtonRelease_pp(widgetHandle, button, x, y, state, buttons);
    }

    boolean qt_event_mouseMove(int widgetHandle, int button, int x, int y,
            int state, int buttons) {
        if( mouseSelection && widgetHandle == handle && (style & SWT.SINGLE) != 0 && (style & SWT.RADIO) != 0){
            int itemHandle = OS.QTableWidget_itemAt(topHandle, x, y);
            int index = -1;
            if (itemHandle != 0) {
                index = OS.QTableWidget_row(topHandle, itemHandle);
            }
            Rectangle rect = OS.QTableWidget_visualItemRect(topHandle, currentItem.topHandle());
            if(index != oldIndex){
                if (!((rect.y <= y) && (y <= (rect.y+rect.height)) &&(x<=rect.x+rect.width))){
                    mouseSelection = false;
                    oldIndex = -1;
                }
            } else if(index == -1){
                if (!((rect.y <= y) && (y <= (rect.y+rect.height)) && (x<rect.x+rect.width))){
                    mouseSelection = false;
                }
            }
        }
        return super.qt_event_mouseMove(widgetHandle, button, x, y, state, buttons);
    }
    
    void qt_signal_released()
    {
        sendSelectionEvent();
    }
    
    void qt_signal_table_cellActivated(int row, int column)
    {
        Event event = new Event();
        event.widget = this;
        event.item = _getItem(row);
        event.doit = true;      
        sendEvent(SWT.DefaultSelection, event, true);
    }
    
    void setTraversalFlags_pp(int type, int key, int modifier,
            int character) {
        traverseDoit = false;
        traverseCancel = false;
    
        if (type == SWT.TRAVERSE_RETURN) {
            // Selecting TableItems is done with enter
            return;
        }
        
        switch (key) {
            case OS.QT_KEY_LEFT:
                if (getCurrentColumnIndex() == 0) {
                    traverseDoit = true;
                }
                break;
            case OS.QT_KEY_RIGHT:
                if (getCurrentColumnIndex() == columnCount - 1) {
                    traverseDoit = true;
                }
                break;
            case OS.QT_KEY_UP:
                if (OS.QTableWidget_currentRow(topHandle) == 0) {
                    traverseDoit = true;
                } 
                break;
            case OS.QT_KEY_DOWN:
                if (OS.QTableWidget_currentRow(topHandle) == itemCount - 1) {
                    traverseDoit = true;
                } 
                break;
            default:
                super.setTraversalFlags_pp(type, key, modifier, character);
                break;
        }
    
    }
    
    /**
     * Searches the receiver's list starting at the first column
     * (index 0) until a column is found that is equal to the 
     * argument, and returns the index of that column. If no column
     * is found, returns -1.
     *
     * @param column the search column
     * @return the index of the column
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the column is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int indexOf (TableColumn column) {
        checkWidget();
        if (column == null) error (SWT.ERROR_NULL_ARGUMENT);
        return getColumnIndex(column);
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
     *    <li>ERROR_NULL_ARGUMENT - if the item is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int indexOf (TableItem item) {
        checkWidget();
        if (item == null) error (SWT.ERROR_NULL_ARGUMENT);
        return getItemIndex(item);
    }
    
    /**
     * Returns <code>true</code> if the item is selected,
     * and <code>false</code> otherwise.  Indices out of
     * range are ignored.
     *
     * @param index the index of the item
     * @return the selection state of the item at the index
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean isSelected (int index) {
        checkWidget();
        if (!(0 <= index && index < itemCount)) return false;    
        return _getItem(index).isSelected();
    }
    
    
    void releaseChildren_pp (boolean destroy) {
        if (items != null) {
            for (int i=0; i<itemCount; i++) {
                TableItem item = items [i];
                if (item != null && !item.isDisposed ()) {
                    item.release (false);
                }
            }
            items = null;
        }
        if (columns != null) {
            for (int i=0; i<columnCount; i++) {
                TableColumn column = columns [i];
                if (column != null && !column.isDisposed ()) {
                    column.release (false);
                }
            }
            columns = null;
        }
        super.releaseChildren_pp (destroy);
    }
    
    void releaseWidget_pp () {
        super.releaseWidget_pp ();
        currentItem = null;
    }
    
    /**
     * Removes the item from the receiver at the given
     * zero-relative index.
     *
     * @param index the index for the item
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_RANGE - if the index is not between 0 and the number of elements in the list minus 1 (inclusive)</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void remove (int index) {
        checkWidget();
        if (0 > index || index > itemCount) error(SWT.ERROR_INVALID_RANGE);
        
        removeItem(index);
    }
    
    /**
     * Removes the items from the receiver which are
     * between the given zero-relative start and end 
     * indices (inclusive).
     *
     * @param start the start of the range
     * @param end the end of the range
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_RANGE - if either the start or end are not between 0 and the number of elements in the list minus 1 (inclusive)</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void remove (int start, int end) {
        checkWidget();
        if (start > end) return;
        if (!(0 <= start && start <= end && end < itemCount)) {
            error (SWT.ERROR_INVALID_RANGE);
        }
        
        int index = end;
        while (index >= start) {
            TableItem item = _getItem(index);
            if (item != null && !item.isDisposed ()) {
                item.release(false);
            }
            index--;
        }
        
        if (heightRow >= start && heightRow <= end) {
            heightRow = -1;
        }
        
        OS.QAbstractItemView_swt_removeRows(topHandle, start, end-start+1);
        
        System.arraycopy (items, end + 1, items, start, itemCount - end - 1);
        for (int i = itemCount - (end - start); i < itemCount; i++) {
            items[i] = null;
        }
        itemCount = itemCount - (end - start) - 1;
        
        // setting radio selection if the selected item is removed
        if((style & SWT.SINGLE) != 0 && (style & SWT.RADIO) != 0 && itemCount>0){
            boolean selected = false;
            for(int i=0;i<itemCount;i++){
                TableItem item = getItem(i);
                if(item.getRadioSelected()){
                    selected = true;
                    break;
                }
            }
            if(!selected){
                if(start>0){
                    OS.QAbstractButton_setChecked(items[start-1].radioButtonHandle, true);
                } else {
                    OS.QAbstractButton_setChecked(items[start+1].radioButtonHandle, true);
                }
            }
        }
    }
    
    /**
     * Removes the items from the receiver's list at the given
     * zero-relative indices.
     *
     * @param indices the array of indices of the items
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_RANGE - if the index is not between 0 and the number of elements in the list minus 1 (inclusive)</li>
     *    <li>ERROR_NULL_ARGUMENT - if the indices array is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void remove (int [] indices) {
        checkWidget();
        if (indices == null) error (SWT.ERROR_NULL_ARGUMENT);
        if (indices.length == 0) return;
        
        int [] newIndices = new int [indices.length];
        System.arraycopy (indices, 0, newIndices, 0, indices.length);
        ListUtils.sort(newIndices);
        int start = newIndices [newIndices.length - 1], end = newIndices [0];
        if (end < 0 || start >= itemCount || start < end) {
            error (SWT.ERROR_INVALID_RANGE);
        }
        int last = -1;
        for (int i = newIndices.length-1; i >= 0; i--) {
            int index = newIndices [i];
            if (index == heightRow) heightRow = -1;
            if (index != last) {
                remove(index);
                last = index;
            }
        }
    }
    
    /**
     * Removes all of the items from the receiver.
     * 
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void removeAll () {
        checkWidget();
        for (int i = 0; i < itemCount; i++) {
            TableItem item = _getItem(i);
            if (item != null && !item.isDisposed()) {
                item.release(false);
            }
        }
                  
        OS.QTableWidget_clearContents(topHandle);
        OS.QTableWidget_setRowCount(topHandle, 0);
        items = new TableItem [4];
        itemCount = 0;
        currentItem = null;
        heightRow = -1;
    }
    
    /**
     * Removes the listener from the collection of listeners who will
     * be notified when the user changes the receiver's selection.
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
     * @see #addSelectionListener(SelectionListener)
     */
    public void removeSelectionListener(SelectionListener listener) {
        checkWidget();
        if (listener == null) error (SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook (SWT.Selection, listener);
        eventTable.unhook (SWT.DefaultSelection,listener);    
    }
    
    
    /**
     * Selects the item at the given zero-relative index in the receiver. 
     * If the item at the index was already selected, it remains
     * selected. Indices that are out of range are ignored.
     *
     * @param index the index of the item to select
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void select (int index) {
        checkWidget();    
        _select(index);
    }
    
    void _select(int index) {
        if (!(0 <= index && index < itemCount)) return;
        TableItem item = _getItem(index);
        item.setSelected(true);
        if ((style & SWT.SINGLE) != 0) {
            apiSelection = true;
            int column = Math.max(1, OS.QTableWidget_currentColumn(topHandle));
            OS.QTableWidget_setCurrentCell(topHandle, index, column);
            apiSelection = false;
        }
    }
    
    /**
     * Selects the items in the range specified by the given zero-relative
     * indices in the receiver. The range of indices is inclusive.
     * The current selection is not cleared before the new items are selected.
     * <p>
     * If an item in the given range is not selected, it is selected.
     * If an item in the given range was already selected, it remains selected.
     * Indices that are out of range are ignored and no items will be selected
     * if start is greater than end.
     * If the receiver is single-select and there is more than one item in the
     * given range, then all indices are ignored.
     * </p>
     *
     * @param start the start of the range
     * @param end the end of the range
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     * 
     * @see Table#setSelection(int,int)
     */
    public void select (int start, int end) {
        checkWidget ();
        if (end < 0 || start > end || ((style & SWT.SINGLE) != 0 && start != end)) return;
        if (itemCount == 0 || start >= itemCount) return;
        
        for (int i = start; i <= end; i++) {
            _select(i);
        }
    }
    
    /**
     * Selects the items at the given zero-relative indices in the receiver.
     * The current selection is not cleared before the new items are selected.
     * <p>
     * If the item at a given index is not selected, it is selected.
     * If the item at a given index was already selected, it remains selected.
     * Indices that are out of range and duplicate indices are ignored.
     * If the receiver is single-select and multiple indices are specified,
     * then all indices are ignored.
     * </p>
     *
     * @param indices the array of indices for the items to select
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the array of indices is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     * 
     * @see Table#setSelection(int[])
     */
    public void select (int [] indices) {
        checkWidget ();
        if (indices == null) error (SWT.ERROR_NULL_ARGUMENT);
        int length = indices.length;
        if (length == 0 || ((style & SWT.SINGLE) != 0 && length > 1)) return;
        for (int i = 0; i < length; i++) {
            _select(indices[i]);
        }
    }
    
    /**
     * Selects all of the items in the receiver.
     * <p>
     * If the receiver is single-select, do nothing.
     * </p>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void selectAll () {
        checkWidget();
        if ((style & SWT.SINGLE) != 0) return;
        apiSelection = true;
        OS.QAbstractItemView_clearSelection(topHandle);
        OS.QAbstractItemView_selectAll(topHandle);
        apiSelection = false;
    }
    
    void setFocusIndex(int index) {
        checkWidget();
        if (index < 0 || index > getItemCount() - 1)
            return;
        apiSelection = true;
        int column = Math.max(1, OS
                .QTableWidget_currentColumn(topHandle));
        OS.QTableWidget_setCurrentCell(topHandle, index, column);
        apiSelection = false;
    }
    
    /**
     * Marks the receiver's header as visible if the argument is <code>true</code>,
     * and marks it invisible otherwise. 
     * <p>
     * If one of the receiver's ancestors is not visible or some
     * other condition makes the receiver not visible, marking
     * it visible may not actually cause it to be displayed.
     * </p>
     *
     * @param show the new visibility state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setHeaderVisible (boolean show) {
        checkWidget ();
        OS.QTableWidget_swt_setHeaderVisible(topHandle, true, show);
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
        if (count == itemCount) return;
        boolean isVirtual = (style & SWT.VIRTUAL) != 0;
        if (!isVirtual) setRedraw (false);
        remove (count, itemCount - 1);
        int length = Math.max (4, (count + 3) / 4 * 4);
        TableItem [] newItems = new TableItem [length];
        System.arraycopy (items, 0, newItems, 0, itemCount);
        items = newItems;
        if (isVirtual) {
            OS.QTableWidget_setRowCount(topHandle, count);
            itemCount = count;
        } else {
            for (int i=itemCount; i<count; i++) {
                new TableItem(this, SWT.NONE, i, true);
            }
        }
        if (!isVirtual) setRedraw (true);
    }
    
    /**
     * Marks the receiver's lines as visible if the argument is <code>true</code>,
     * and marks it invisible otherwise. 
     * <p>
     * If one of the receiver's ancestors is not visible or some
     * other condition makes the receiver not visible, marking
     * it visible may not actually cause it to be displayed.
     * </p>
     *
     * @param show the new visibility state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setLinesVisible (boolean show) {
        checkWidget();
        OS.QTableView_setShowGrid(topHandle, show);
    }
    
    
    /**
     * Selects the item at the given zero-relative index in the receiver. 
     * The current selection is first cleared, then the new item is selected.
     *
     * @param index the index of the item to select
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Table#deselectAll()
     * @see Table#select(int)
     */
    public void setSelection (int index) {
        checkWidget ();
        deselectAll();
        select(index);
        OS.QTableView_showRow(topHandle, index);
    }
    
    /**
     * Selects the items in the range specified by the given zero-relative
     * indices in the receiver. The range of indices is inclusive.
     * The current selection is cleared before the new items are selected.
     * <p>
     * Indices that are out of range are ignored and no items will be selected
     * if start is greater than end.
     * If the receiver is single-select and there is more than one item in the
     * given range, then all indices are ignored.
     * </p>
     * 
     * @param start the start index of the items to select
     * @param end the end index of the items to select
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Table#deselectAll()
     * @see Table#select(int,int)
     */
    public void setSelection (int start, int end) {
        checkWidget ();
        deselectAll();
        if (end < 0 || start > end || ((style & SWT.SINGLE) != 0 && start != end)) return;
        if (itemCount == 0 || start >= itemCount) return;
    
        start = Math.max (0, start);
        end = Math.min (end, itemCount - 1);
        if ((style & SWT.MULTI) != 0) {
            select (start, end);
            showSelection ();
        } else {
            _select(start);
            OS.QTableView_showRow(topHandle, start);   
        }  
    }
    
    /**
     * Selects the items at the given zero-relative indices in the receiver.
     * The current selection is cleared before the new items are selected.
     * <p>
     * Indices that are out of range and duplicate indices are ignored.
     * If the receiver is single-select and multiple indices are specified,
     * then all indices are ignored.
     * </p>
     *
     * @param indices the indices of the items to select
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the array of indices is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Table#deselectAll()
     * @see Table#select(int[])
     */
    public void setSelection (int [] indices) {
        checkWidget ();
        if (indices == null) error (SWT.ERROR_NULL_ARGUMENT);
        deselectAll ();
        int length = indices.length;
        if (length == 0 || ((style & SWT.SINGLE) != 0 && length > 1)) return;
        if ((style & SWT.MULTI) != 0) {
            select (indices);
            showSelection ();
        } else {
            _select(indices[0]);
            OS.QTableView_showRow(topHandle, indices[0]);
        }
    }
    
    
    /**
     * Sets the receiver's selection to be the given array of items.
     * The current selection is cleared before the new items are selected.
     * <p>
     * Items that are not in the receiver are ignored.
     * If the receiver is single-select and multiple items are specified,
     * then all items are ignored.
     * </p>
     *
     * @param items the array of items
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the array of items is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if one of the items has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Table#deselectAll()
     * @see Table#select(int[])
     * @see Table#setSelection(int[])
     */
    public void setSelection (TableItem [] items) {
        checkWidget ();
        if (items == null) error (SWT.ERROR_NULL_ARGUMENT);
        deselectAll();
        
        int length = items.length;
        if (!(length == 0 || ((style & SWT.SINGLE) != 0 && length > 1))) {
            boolean first = true;
            for (int i = 0; i < length; i++) {
                int index = indexOf (items [i]);
                if (index != -1) {
                    if (first) {
                        first = false;
                        OS.QTableView_showRow(topHandle, index);
                    } 
                    _select(index);
                }
            }
            showSelection ();
        }
    }
    
    /**
     * Sets the zero-relative index of the item which is currently
     * at the top of the receiver. This index can change when items
     * are scrolled or new items are added and removed.
     *
     * @param index the index of the top item
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setTopIndex (int index) {
        checkWidget();
        if (!(0 <= index && index < itemCount)) return;
        
        int currentColumn = Math.max(0, getCurrentColumnIndex());
        int itemHandle = _getItem(index).getHandle(currentColumn);
        
        OS.QTableWidget_scrollToItem(topHandle, itemHandle, OS.QT_ABSTRACTITEMVIEW_SCROLLHINT_POSITIONATTOP);
    }
    
    /*
     * Toggle wrapping of text in table items
     */
    void setWordWrap(boolean on) {
        checkWidget();
        if (wrap != on) {
            wrap = on;
            OS.QTableView_setWordWrap(topHandle, wrap);
            updateRowHeight(true);
        }
    }
    
    /**
     * Shows the column.  If the column is already showing in the receiver,
     * this method simply returns.  Otherwise, the columns are scrolled until
     * the column is visible.
     *
     * @param column the column to be shown
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the column is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if the column has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 3.0
     */
    public void showColumn (TableColumn column) {
        checkWidget ();
        if (column == null) error (SWT.ERROR_NULL_ARGUMENT);
        if (column.isDisposed()) error(SWT.ERROR_INVALID_ARGUMENT);
        if (column.parent != this) return;
        if (itemCount == 0) return;
        
        int currentRow = Math.max(0, OS.QTableWidget_currentRow(topHandle));    
        int index = Math.max(0, getColumnIndex(column));
        TableItem item = _getItem(currentRow);
        
        OS.QTableWidget_scrollToItem(topHandle, item.getHandle(index), OS.QT_ABSTRACTITEMVIEW_SCROLLHINT_ENSUREVISIBLE);
    }
    
    /**
     * Shows the item.  If the item is already showing in the receiver,
     * this method simply returns.  Otherwise, the items are scrolled until
     * the item is visible.
     *
     * @param item the item to be shown
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the item is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if the item has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Table#showSelection()
     */
    public void showItem (TableItem item) {
        checkWidget ();
        if (item == null) error (SWT.ERROR_NULL_ARGUMENT);
        if (item.isDisposed()) error (SWT.ERROR_INVALID_ARGUMENT);
        if (item.parent != this) return;
        // Avoid scrolling horizontally
        int itemHandle = item.getHandle(getCurrentColumnIndex());
        OS.QTableWidget_scrollToItem(topHandle, itemHandle, OS.QT_ABSTRACTITEMVIEW_SCROLLHINT_ENSUREVISIBLE);
    }
    
    
    /**
     * Shows the selection.  If the selection is already showing in the receiver,
     * this method simply returns.  Otherwise, the items are scrolled until
     * the selection is visible.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Table#showItem(TableItem)
     */
    public void showSelection () {
        checkWidget();
        TableItem [] selection = getSelection();
        if (selection.length == 0) return;
        showItem(selection[0]);
    }
    
    Point getPreferredClientAreaSize_pp() {
        Point size = OS.QWidget_sizeHint(topHandle);
        
        if (itemCount > 0) {
            int width = 0;
            int height = 0;

            // Check box column
            if ((style & SWT.CHECK) != 0 || (style & SWT.RADIO) != 0) {
                width += OS.QTableView_columnWidth(topHandle, 0);
            }
            
            if (columnCount > 0) {
                for (int i = 0; i < columnCount; i++) {
                    if (columns[i].useFixedWidth) {
                        width += OS.QTableView_columnWidth(topHandle, getNativeColumnIndex(i));
                    } else {
                        width += columns[i].getPreferredColumnWidth(getNativeColumnIndex(i));
                    }
                }
            } else {
                // list mode
                width += OS.QAbstractItemView_sizeHintForColumn(topHandle, 1);
            }
            
            height += _getHeaderHeight();        
            height += itemCount * _getItemHeight();
            size.x = width;
            size.y = height;
        }

        return size;
    }
}
