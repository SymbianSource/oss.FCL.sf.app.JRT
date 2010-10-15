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
 *     Nokia Corporation - S60 implementation
 *     Nokia Corporation - QT implementation
 *******************************************************************************/

package org.eclipse.ercp.swt.mobile;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Scrollable;
import org.eclipse.swt.widgets.TypedListener;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Internal_GfxPackageSupport;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.internal.qt.ListUtils;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.PackageProxy;
import org.eclipse.swt.internal.qt.WidgetState;

/**
 * Instances of this class represent a selectable user interface object that
 * displays a list of items consisting of text and icons from a data model. Each
 * list item may include combinations of heading text, heading icons, detail
 * text, and detail icons. The layout and display of the various text and icons
 * is variable depending upon the style and modifier constants passed to the
 * constructor. Only one LB_STYLE_xxxx layout style constant may be specified.
 * The LB_STYLE_NO_HEADING_TEXT style displays detail text on a single line per
 * item. The LB_STYLE_1LINE_ITEM style displays heading text next to detail text
 * in a single row. The LB_STYLE_2LINE_ITEM style displays heading text above
 * detail text in a two line cell.
 * <p>
 * The following diagrams show the general layout for each layout style:
 * <p>
 * LB_STYLE_NO_HEADING_TEXT <Table width=200 border=1>
 * <tr>
 * <td>Detail Text</td>
 * </tr>
 * <tr>
 * <td>Detail Text</td>
 * </tr>
 * <tr>
 * <td>Detail Text</td>
 * </tr>
 * <tr>
 * <td>Detail Text</td>
 * </tr>
 * </table>
 * <p>
 * LB_STYLE_1LINE_ITEM <Table width=200 border=1>
 * <tr>
 * <td>Heading Text</td>
 * <td>Detail Text</td>
 * </tr>
 * <tr>
 * <td>Heading Text</td>
 * <td>Detail Text</td>
 * </tr>
 * <tr>
 * <td>Heading Text</td>
 * <td>Detail Text</td>
 * </tr>
 * <tr>
 * <td>Heading Text</td>
 * <td>Detail Text</td>
 * </tr>
 * </table>
 * <p>
 * LB_STYLE_2LINE_ITEM <Table width=200 border=1>
 * <tr>
 * <td><b>Heading Text </b> <br>
 * Detail Text</td>
 * </tr>
 * <tr>
 * <td><b>Heading Text </b> <br>
 * Detail Text</td>
 * </tr>
 * <tr>
 * <td><B>Heading Text </b> <br>
 * Detail Text</td>
 * </tr>
 * </table>
 * <p>
 * The specific placement of item elements is locale and platform-dependent.
 * LB_MOD_xxxx style modifiers are hints which are implemented in a
 * platform-dependent fashion or perhaps not at all. They may be specified by
 * or-ing any number of LB_MOD_xxxx constants with one selected layout style.
 * Heading and detail icons are optionally displayed on any of the style layouts
 * by specifying the LB_MOD_SHOW_HEADING_ICONS or LB_MOD_SHOW_DETAILS_ICONS
 * modifiers. Heading and detail icons, if both displayed, are separated within
 * the layout to indicate that they are distinct from each other. The
 * LB_MOD_SHOW_SELECTION_NUMBER modifier is a hint for the ListBox to display a
 * single digit next to each item which indicates that pressing that number
 * selects the item. A list may be SINGLE or MULTI select. However, due to
 * typical layout contraints, MULTI select is a hint that is guaranteed to be
 * implemented only when no style modifiers are used.
 * <p>
 * The following is a possible layout for LB_STYLE_1LINE_ITEM |
 * LB_MOD_SHOW_DETAIL_ICONS | LB_MOB_SHOW_SELECTION_NUMBER: <Table width=200
 * border=1>
 * <tr>
 * <td>1</td>
 * <td>Heading</td>
 * <td>Detail Text</td>
 * <td>icon</td>
 * </tr>
 * <tr>
 * <td>2</td>
 * <td>Heading</td>
 * <td>Detail Text</td>
 * <td>icon</td>
 * </tr>
 * <tr>
 * <td>3</td>
 * <td>Heading</td>
 * <td>Detail Text</td>
 * <td>icon</td>
 * </tr>
 * <tr>
 * <td>4</td>
 * <td>Heading</td>
 * <td>Detail Text</td>
 * <td>icon</td>
 * </tr>
 * </table>
 *
 * <p>
 * The model data is provided by an array of ListBoxItems. The array is not
 * copied. The array is used until the ListBox is destroyed or a new data model
 * is set.
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed. On some
 * platforms, the fonts of the heading and detail texts are defined by the
 * layout styles. ListBox allows setting the fonts even if the underlying
 * platform does not, but in this case the appearance of the control will not
 * change.
 * <p>
 * <dl>
 * <dt><b>Styles: </b></dt>
 * <dd>SINGLE, MULTI</dd>
 * <p>
 * Note: Only one of SINGLE and MULTI may be specified.
 * <p>
 * </dl>
 * <dl>
 * <dt><b>Layout Styles: </b></dt>
 * <dd>LB_STYLE_NO_HEADING_TEXT</dd>
 * <dd>LB_STYLE_1LINE_ITEM</dd>
 * <dd>LB_STYLE_2LINE_ITEM</dd>
 * <p>
 * Note: Only one layout style may be specified. If more than one layout style
 * is specified, the actual style used is implementation dependent.
 * <p>
 * <dl>
 * <dt><b>Layout Style Modifiers: </b></dt>
 * <dd>LB_MOD_SHOW_SELECTION_NUMBER</dd>
 * <dd>LB_MOD_SHOW_HEADING_ICONS</dd>
 * <dd>LB_MOD_SHOW_DETAIL_ICONS</dd>
 * </dl>
 * <p>
 * Note: Style modifiers are hints. Any number of modifier styles may be
 * combined with a layout style
 * </dl>
 * <p>
 * <dt><b>Events: </b></dt>
 * <dd>Selection, DefaultSelection</dd>
 * </dl>
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 *
 * @see ListBoxItem
 */
public class ListBox extends Scrollable {
    private static final int LISTBASE_SHOW_CHECKBOX = 0x80;

    private static final int LISTTYPE_STRINGLIST = 1;
    private static final int LISTTYPE_SORTEDLIST = 2;
    private static final int LISTTYPE_LISTVIEW = 4;
    private static final int LISTTYPE_LISTBOX = 8;

    private int layoutStyle;
    private int dataModelHandle;
    private int selectionModelHandle;
    private int listType;
    private boolean undoSelectionChangePhase = false;
    private int delegateHandle;
    private Font headingFont;
    private Image[][] detailImages;
    private Image[][] headingImages;
    private ListBoxItem []items;
    /**
     * single line item, 1 column (default)
     */
    public static final int LB_STYLE_NO_HEADING_TEXT = 0;

    /**
     * single line item, 2 columns
     */
    public static final int LB_STYLE_1LINE_ITEM = 1;

    /**
     * double line item, 1 column with heading and detail combined
     */
    public static final int LB_STYLE_2LINE_ITEM = 2;

    /**
     * shows a single digit number aligned with each item which may be used to
     * select the item.
     */
    public static final int LB_MOD_SHOW_SELECTION_NUMBER = 0x10;

    /**
     * shows icon associated with heading text
     */
    public static final int LB_MOD_SHOW_HEADING_ICONS = 0x20;

    /**
     * shows icons associated with detail text
     */
    public static final int LB_MOD_SHOW_DETAIL_ICONS = 0x40;

    static final class ListBoxPackageProxy extends PackageProxy {
        public void createHandle(int index) {
            ((ListBox)w).createHandle(index);
        }
        public void qt_signal_selectionChanged(int selectionHandle, int deSelectionHandle) {
            ((ListBox)w).qt_signal_selectionChanged(selectionHandle, deSelectionHandle);
        }
        public void qt_signal_abstractitemview_activated(int row, int column) {
            ((ListBox)w).qt_signal_abstractitemview_activated(row, column);
        }
        public void releaseHandle() {
            ((ListBox)w).releaseHandle();
        }
        public void releaseWidget() {
            ((ListBox)w).releaseWidget();
        }
        public void setTraversalFlags(int type, int key, int modifier, int character) {
            ((ListBox)w).setTraversalFlags(type, key, modifier, character);
        }
        public Point getPreferredClientAreaSize() {
            return ((ListBox)w).getPreferredClientAreaSize();
        }
        public void hookEvents() {
            ((ListBox)w).hookEvents();
        }
    }

    /**
     * Constructs a new instance of this class given its parent, a style value
     * describing its selection behavior, and a style value describing its
     * layout.
     * <p>
     * The <code>style</code> value is either one of the style constants
     * defined in class <code>SWT</code> which is applicable to instances of
     * this class, or must be built by <em>bitwise OR</em> 'ing together (that
     * is, using the <code>int</code> "|" operator) two or more of those
     * <code>SWT</code> style constants. Style bits are also inherited from
     * superclasses.
     * <p>
     * The <code>layoutStyle</code> value is one or more of the class layout
     * style constants which may be combined by <em>bitwise OR</em> 'ing. Use
     * of styles not applicaple to this class are ignored.
     * </p>
     *
     * @param parent
     *            a composite control which will be the parent of the new
     *            instance (cannot be null)
     * @param style
     *            the style of control to construct
     *
     * @param layoutStyle
     *            the layout style of control to construct
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
     * @see SWT#SINGLE
     * @see SWT#MULTI
     */
    public ListBox(Composite parent, int style, int layoutStyle) {
        super(parent, checkStyle(style), checkLayoutStyle(layoutStyle),
                new ListBoxPackageProxy(), false);

        this.layoutStyle = layoutStyle;
        // show check box only for S60
        if (OS.windowServer == OS.WS_SYMBIAN_S60) {
            this.layoutStyle |= LISTBASE_SHOW_CHECKBOX;
        }
        initList();

        delegateHandle = OS.ListBoxItemDelegate_new(topHandle());
        /*
         * after next line, QListView owns item delegate
         */
        OS.QListView_setItemDelegate(topHandle(), delegateHandle);
    }

    static int checkLayoutStyle(int layoutStyle) {
        return Internal_PackageSupport.checkBits(layoutStyle, LB_STYLE_NO_HEADING_TEXT,
                LB_STYLE_1LINE_ITEM, LB_STYLE_2LINE_ITEM, 0, 0, 0);
    }

    static int checkStyle(int style) {
        return Internal_PackageSupport.checkBits(style, SWT.SINGLE, SWT.MULTI, 0, 0, 0, 0);
    }

    void createHandle(int index) {
        int scrollAreaHandle = OS.QListView_new();
        Internal_PackageSupport.setScrollAreaHandle(this, scrollAreaHandle);
        Internal_PackageSupport.setFrameHandle(this, scrollAreaHandle);
        Internal_PackageSupport.setHandle(this, OS.QAbstractScrollArea_viewPort(scrollAreaHandle));
        Internal_PackageSupport.setTopHandle(this, scrollAreaHandle);
        Internal_PackageSupport.setState(this, Internal_PackageSupport.state(this) | WidgetState.HANDLE);
    }

    /**
     * Adds the listener to the collection of listeners who will be notified
     * when the receiver's selection changes, by sending it one of the messages
     * defined in the SelectionListener interface.
     * <p>
     * <code>widgetSelected</code> is called when the selection changes. <br>
     * <code>widgetDefaultSelected</code> is typically called when selection
     * is finalized.
     *
     * @param listener
     *            instance called when selection events occur
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     * @exception SWTError
     *                <ul>
     *                <li>ERROR_ITEM_NOT_ADDED - if the operation fails because
     *                of an operating system failure</li>
     *                </ul>
     *
     * @see ListBox#removeSelectionListener
     * @see SelectionListener
     */
    public void addSelectionListener(SelectionListener listener) {
        listBase_addSelectionListener(listener);
    }

    /**
     * Deselects the item at the given zero-relative index in the receiver. If
     * the item at the index was already deselected, it remains deselected.
     * Indices that are out of range are ignored.
     *
     * @param index
     *            the index of the item to deselect
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void deselect(int index) {
        listBase_deselect(index);
    }

    /**
     * Deselects the items at the given zero-relative indices in the receiver.
     * If the items at the given zero-relative indexes in the receiver are
     * selected, they are deselected. If they were not selected, they remain
     * deselected. The range of the indices is inclusive. Indices that are out
     * of range are ignored.
     *
     * @param start
     *            the start index of the items to deselect
     * @param end
     *            the end index of the items to deselect
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void deselect(int start, int end) {
        listBase_deselect(start, end);
    }

    /**
     * Deselects the items at the given zero-relative indices in the receiver.
     * If the items at the given zero-relative indexes in the receiver are
     * selected, they are deselected. If they were not selected, they remain
     * deselected. Indices that are out of range and duplicate indices are
     * ignored.
     *
     * @param indices
     *            the array of indices for the items to deselect
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the set of indices is null
     *                </li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void deselect(int[] indices) {
        listBase_deselect(indices);
    }

    /**
     * Deselects all selected items in the receiver.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void deselectAll() {
        listBase_deselectAll();
    }

    /**
     * Returns the zero-relative index of the item which currently has the focus
     * in the receiver, or -1 if no item has focus.
     *
     * @return the index of the selected item
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public int getFocusIndex() {
        return listBase_getFocusIndex();
    }

    /**
     * Returns the number of items currently selected.
     * <p>
     *
     * @return count of selected items
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     * @exception SWTError
     *                <ul>
     *                <li>ERROR_CANNOT_GET_COUNT - if the operation fails
     *                because of an operating system failure</li>
     *                </ul>
     *
     * @see #getSelectionIndices
     */
    public int getSelectionCount() {
        return listBase_getSelectionCount();
    }

    /**
     * Returns the zero-relative indices of the items which are currently
     * selected in the receiver. The array is empty if no items are selected.
     * <p>
     * Note: This is not the actual structure used by the receiver to maintain
     * its selection, so modifying the array will not affect the receiver.
     * </p>
     *
     * @return the array of indices of the selected items
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * @exception SWTError
     *                <ul>
     *                <li>ERROR_CANNOT_GET_SELECTION - if the operation fails
     *                because of an operating system failure</li>
     *                </ul>
     */
    public int[] getSelectionIndices() {
        return listBase_getSelectionIndices();
    }

    /*
     * @see Scrollable.getPrefferedClientAreaSize
     */
    Point getPreferredClientAreaSize() {
        final int count = this.listBase_getItemCount();
        if (count < 1) {
            return Internal_PackageSupport.getPreferredClientAreaSize(this);
        }
        int x = OS.QAbstractItemView_sizeHintForColumn(topHandle(), 0);
        int y = 0;
        for (int i = 0; i < count; i++) {
            y += OS.QAbstractItemView_sizeHintForIndex(topHandle(), 0,
                    0).y;
        }
        return new Point(x, y);
    }

    void hookEvents() {
        if (selectionModelHandle != 0) {
            int signalProxy = OS.SignalHandler_new(topHandle(), OS.QSIGNAL_SELECTIONCHANGED);
            OS.QObject_connectOrThrow(
                    selectionModelHandle,
                    "selectionChanged( const QItemSelection&, const QItemSelection& )",
                    signalProxy,
                    "widgetSignal( const QItemSelection&, const QItemSelection& )",
                    OS.QT_AUTOCONNECTION);

            int itemActivatedSignalProxy = OS.SignalHandler_new(topHandle(), OS.QSIGNAL_ABSTRACTITEMVIEW_ACTIVATED);
            OS.QObject_connectOrThrow(topHandle(),
                    "activated(const QModelIndex& )", itemActivatedSignalProxy,
                    "widgetSignal(const QModelIndex&)", OS.QT_AUTOCONNECTION);
        }
    }

    void initList() {
        /*
         * QListView owns created data model
         */
        dataModelHandle = createDataModel();
        OS.QAbstractItemView_setModel(topHandle(), dataModelHandle);

        OS.QAbstractItemView_setSelectionBehavior(topHandle(),
                OS.QT_ABSTRACTITEMVIEW_SELECTIONBEHAVIOR_ROWS);

        int selectionMode = OS.QT_ABSTRACTITEMVIEW_SELECTIONMODE_SINGLE;
        if ((style() & SWT.MULTI) != 0) {
            selectionMode = OS.QT_ABSTRACTITEMVIEW_SELECTIONMODE_MULTI;
        }
        OS.QAbstractItemView_setSelectionMode(topHandle(),
                selectionMode);

        selectionModelHandle = OS
                .QAbstractItemView_selectionModel(topHandle());

        hookEvents();
    }

    void listBase_addSelectionListener(SelectionListener listener) {
        checkWidget();
        if (listener == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Selection, typedListener);
        addListener(SWT.DefaultSelection, typedListener);
    }

    void listBase_deselect(int index) {
        checkWidget();
        if (index < 0 || index > listBase_getItemCount() - 1)
            return;
        internalSelect(index, OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_DESELECT,
                false, false);
    }

    void listBase_deselect(int start, int end) {
        checkWidget();

        int count = listBase_getItemCount();
        if (start > count - 1 || end < 0 || start > end)
            return;

        start = Math.max(0, start);
        end = Math.min(end, count - 1);

        internalSelect(start, end,
                OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_DESELECT, false, false);
    }

    void listBase_deselect(int[] indices) {
        checkWidget();
        if (indices == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        int length = indices.length;
        if (length == 0)
            return;
        int index;
        int count = listBase_getItemCount();
        for (int i = 0; i < length; i++) {
            index = indices[i];
            if (index < 0 || index > count - 1)
                continue;
            internalSelect(indices[i],
                    OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_DESELECT, false,
                    false);
        }
    }

    void listBase_deselectAll() {
        checkWidget();
        OS.QItemSelectionModel_clearSelection(selectionModelHandle);
    }

    int listBase_getFocusIndex() {
        checkWidget();
        int indexHandle = OS
                .QItemSelectionModel_currentIndex(selectionModelHandle);
        int row = OS.QModelIndex_row(indexHandle);
        OS.QModelIndex_delete(indexHandle);
        return row;
    }

    String[] listBase_getItems() {
        checkWidget();
        int count = listBase_getItemCount();
        if (count < 1)
            return new String[] {};
        String[] strings = OS.ListModel_itemStrings(dataModelHandle);
        /**
         * It seems that 'String[0]' != 'String{} when String[0] is returned
         * from JNI call.
         */
        if (strings == null || strings == new String[0]) {
            strings = new String[] {};
        }
        return strings;
    }

    int listBase_getSelectionCount() {
        checkWidget();
        int[] indices = OS.QItemSelectionModel_selectedRows(
                selectionModelHandle, 0);
        if (indices != null) {
            return indices.length;
        } else {
            return 0;
        }
    }

    String[] listBase_getSelection() {
        checkWidget();
        int[] indices = listBase_getSelectionIndices();
        if (indices.length < 1)
            return new String[] {};
        String[] result = new String[indices.length];
        for (int i = 0; i < indices.length; i++) {
            result[i] = listBase_getItem(indices[i]);
        }
        return result;
    }

    int[] listBase_getSelectionIndices() {
        checkWidget();
        int[] indices = OS.QItemSelectionModel_selectedRows(
                selectionModelHandle, 0);
        if (indices == null /* || indices == new int[0] */) {
            indices = new int[] {};
        }
        ListUtils.sort(indices);
        return indices;
    }

    void listBase_refreshItem(int index) {
        checkWidget();
        if (index < 0 || index >= listBase_getItemCount())
            SWT.error(SWT.ERROR_INVALID_RANGE);
        if(items[index]==null){
        	OS.ListModel_setItemContentsToNull(dataModelHandle, index);
        }
        int indexHandle = OS.QAbstractItemModel_index(dataModelHandle, index,
                0, 0);
        OS.QAbstractItemView_update(topHandle(), indexHandle);
        OS.QModelIndex_delete(indexHandle);
    }

    void listBase_refreshList() {
        checkWidget();
        for (int i = 0; i < listBase_getItemCount(); i++) {
            listBase_refreshItem(i);
        }
    }

    void listBase_removeSelectionListener(SelectionListener listener) {
        removeListener(SWT.Selection, listener);
        removeListener(SWT.DefaultSelection, listener);
    }

    void listBase_select(int index) {
        checkWidget();
        if (index < 0 || index > listBase_getItemCount() - 1)
            return;
        int command = OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT;
        if ((style() & SWT.SINGLE) != 0) {
            command |= OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_CLEAR;
        }
        internalSelect(index, command, false, false);
    }

    void listBase_select(int start, int end) {
        checkWidget();
        if (start == end) {
            listBase_select(start);
            return;
        }

        int count = listBase_getItemCount();
        if (start > count - 1 || end < 0 || start > end)
            return;

        start = Math.max(0, start);
        end = Math.min(end, count - 1);

        if ((style() & SWT.MULTI) == 0)
            return;

        internalSelect(start, end,
                OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT, false, false);
    }

    void listBase_select(int[] indices) {
        checkWidget();
        if (indices == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        int length = indices.length;
        if (length == 1) {
            listBase_select(indices[0]);
            return;
        }
        if (length == 0 || (style() & SWT.MULTI) == 0)
            return;
        int index;
        int count = listBase_getItemCount();
        for (int i = 0; i < length; i++) {
            index = indices[i];
            if (index < 0 || index > count - 1)
                continue;
            internalSelect(index,
                    OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT, false,
                    false);
        }
    }

    void listBase_selectAll() {
        checkWidget();
        if ((style() & SWT.MULTI) == 0 && listBase_getItemCount() != 1)
            return;
        internalSelect(0, listBase_getItemCount() - 1,
                OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT, false, false);
    }

    boolean listBase_isSelected(int index) {
        checkWidget();
        int indexHandle = OS.QAbstractItemModel_index(dataModelHandle, index,
                0, 0);
        if (indexHandle == 0)
            return false;
        boolean selected = OS.QItemSelectionModel_isSelected(
                selectionModelHandle, indexHandle);
        OS.QModelIndex_delete(indexHandle);
        return selected;
    }

    void listBase_setSelection(int index) {
        checkWidget();
        if (index < 0 || index > listBase_getItemCount() - 1) {
            listBase_deselectAll();
            return;
        }
        int command = OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_CLEAR
                | OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT
                | OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_CURRENT;
        internalSelect(index, command, true, true);
    }

    void listBase_setSelection(int start, int end) {
        checkWidget();
        if (start == end) {
            listBase_setSelection(start);
            return;
        }

        listBase_deselectAll();

        int count = listBase_getItemCount();
        if (start > count - 1 || end < 0 || start > end)
            return;

        start = Math.max(0, start);
        end = Math.min(end, count - 1);

        if ((style() & SWT.MULTI) == 0)
            return;

        internalSelect(start, end,
                OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT, true, true);
    }

    void listBase_setSelection(int[] indices) {
        checkWidget();
        if (indices == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        listBase_deselectAll();
        int length = indices.length;
        if (length == 0 || ((style() & SWT.MULTI) == 0 && length != 1))
            return;

        int[] newIndices = new int[indices.length];
        System.arraycopy(indices, 0, newIndices, 0, indices.length);
        ListUtils.sort(newIndices);

        int index;
        int count = listBase_getItemCount();
        for (int i = 0; i < length; i++) {
            index = newIndices[i];
            if (index < 0 || index > count - 1)
                continue;
            if (i == 0) {
                internalSelect(index,
                        OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT, true,
                        true);
            } else {
                internalSelect(index,
                        OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT, false,
                        false);
            }
        }
    }

    void listBase_setSelection(String[] items) {
        checkWidget();
        if (items == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);

        listBase_deselectAll();

        int length = items.length;
        if (length == 0 || ((style() & SWT.MULTI) == 0 && length != 1))
            return;
        int count = listBase_getItemCount();
        if (count < 1)
            return;
        int index;
        int minIndex = count;
        for (int i = 0; i < length; i++) {
            if (items[i] == null)
                SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            index = 0;
            while (index < count) {
                index = listBase_indexOf(items[i], index);
                if (index < 0)
                    break;
                if (index > minIndex) {
                    internalSelect(index,
                            OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT,
                            false, false);
                } else {
                    if (minIndex < count) {
                        internalSelect(minIndex,
                                OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT,
                                false, false);
                    }
                    minIndex = index;
                }
                index++;
            }
        }
        if (minIndex < count) {
            internalSelect(minIndex,
                    OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT, true, true);
        }
    }

    void listBase_showSelection() {
        checkWidget();
        int[] indexes = listBase_getSelectionIndices();
        if (indexes == null || indexes.length < 1)
            return;
        int indexHandle = OS.QAbstractItemModel_index(dataModelHandle,
                indexes[0], 0, 0);
        OS.QAbstractItemView_scrollTo(topHandle(), indexHandle,
                OS.QT_ABSTRACTITEMVIEW_SCROLLHINT_ENSUREVISIBLE);
        OS.QModelIndex_delete(indexHandle);
    }

    void internalSelect(int index, int command, boolean ensureVisible,
            boolean setFocus) {
        int indexHandle = OS.QAbstractItemModel_index(dataModelHandle, index,
                0, 0);
        if (setFocus) {
            OS.QItemSelectionModel_setCurrentIndex(selectionModelHandle,
                    indexHandle, command);
        } else {
            OS.QItemSelectionModel_select(selectionModelHandle, indexHandle,
                    command);
        }
        if (ensureVisible) {
            OS.QAbstractItemView_scrollTo(topHandle(), indexHandle,
                    OS.QT_ABSTRACTITEMVIEW_SCROLLHINT_ENSUREVISIBLE);
        }
        OS.QModelIndex_delete(indexHandle);
    }

    void internalSelect(int start, int end, int command,
            boolean ensureVisible, boolean setFocus) {
        int startIndexHandle = OS.QAbstractItemModel_index(dataModelHandle,
                start, 0, 0);
        int endIndexHandle = OS.QAbstractItemModel_index(dataModelHandle, end,
                0, 0);
        OS.QItemSelectionModel_select(selectionModelHandle, startIndexHandle,
                endIndexHandle, command);
        if (ensureVisible) {
            OS.QAbstractItemView_scrollTo(topHandle(),
                    startIndexHandle,
                    OS.QT_ABSTRACTITEMVIEW_SCROLLHINT_POSITIONATTOP);
        }
        if (setFocus) {
            OS.QItemSelectionModel_setCurrentIndex(selectionModelHandle,
                    startIndexHandle,
                    OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_CURRENT);
        }

        OS.QModelIndex_delete(startIndexHandle);
        OS.QModelIndex_delete(endIndexHandle);
    }

    boolean getTraversalDoIt(int type, int key, int modifier,
            int character) {
        int currentIndex = listBase_getFocusIndex();
        boolean firstHasFoucs = (currentIndex == 0);
        boolean lastHasFoucs = (currentIndex == listBase_getItemCount() - 1);

        boolean doIt = (firstHasFoucs && (type & (SWT.TRAVERSE_ARROW_PREVIOUS | SWT.TRAVERSE_TAB_PREVIOUS)) != 0)
                || (lastHasFoucs
                        && (type & (SWT.TRAVERSE_ARROW_NEXT | SWT.TRAVERSE_TAB_NEXT)) != 0 || (type & SWT.TRAVERSE_MNEMONIC) != 0);
        return doIt;
    }

    void setTraversalFlags(int type, int key, int modifier,
            int character) {
        boolean traverseDoit = getTraversalDoIt(type, key, modifier, character);
        Internal_PackageSupport.setTraverseDoIt(this, traverseDoit);
        if ((type & SWT.TRAVERSE_MNEMONIC) != 0) {
            Internal_PackageSupport.setTraverseCancel(this, true);
        } else {
            Internal_PackageSupport.setTraverseCancel(this, traverseDoit);
        }
    }

    void qt_signal_selectionChanged(int selectionHandle,
            int deSelectionHandle) {
        if (undoSelectionChangePhase == false) {
            Event event = new Event();
            Internal_PackageSupport.sendEvent(this, SWT.Selection, event);
            if (event.doit == false) {
                // undo change
                undoSelectionChangePhase = true;
                OS.QItemSelectionModel_select__set(selectionModelHandle,
                        selectionHandle,
                        OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_DESELECT);
                OS.QItemSelectionModel_select__set(selectionModelHandle,
                        deSelectionHandle,
                        OS.QT_ITEMSELECTIONMODEL_SELECTIONFLAGS_SELECT);
                undoSelectionChangePhase = false;
            } else {
                // update check states
                if (OS.windowServer == OS.WS_SYMBIAN_S60) {
                    OS.ListModel_setCheckState(dataModelHandle,
                            selectionHandle, OS.QT_CHECKED);
                    OS.ListModel_setCheckState(dataModelHandle,
                            deSelectionHandle, OS.QT_UNCHECKED);
                }
            }
        }
    }

    void qt_signal_abstractitemview_activated(int row, int column) {
        Internal_PackageSupport.qt_signal_abstractitemview_activated(this, row, column);
        Internal_PackageSupport.sendEvent(this, SWT.DefaultSelection);
    }

    int listBase_getItemCount() {
        checkWidget();
        return OS.QAbstractItemModel_rowCount(dataModelHandle);
    }

    int listBase_getItemHeight() {
        checkWidget();
        return OS.ListModel_itemHeight(dataModelHandle);
    }

    void listBase_add(String string) {
        checkWidget();
        if (string == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        int count = listBase_getItemCount();
        OS.ListModel_beginInsertRows(dataModelHandle, 0, count, count);
        OS.ListModel_append(dataModelHandle, string);
        OS.ListModel_endInsertRows(dataModelHandle);
    }

    String listBase_getItem(int index) {
        checkWidget();
        int count = listBase_getItemCount();
        if (index < 0 || index > count - 1)
            SWT.error(SWT.ERROR_INVALID_RANGE);
        return OS.ListModel_itemString(dataModelHandle, index);
    }

    int listBase_indexOf(String string, int start) {
        checkWidget();
        if (string == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (start < 0 || start > listBase_getItemCount() - 1)
            return -1;
        return OS.ListModel_indexOf(dataModelHandle, string, start);
    }

    void listBase_remove(int index) {
        checkWidget();
        int count = listBase_getItemCount();
        if (index < 0 || index > count - 1)
            SWT.error(SWT.ERROR_INVALID_RANGE);
        int indexHandle = OS.QAbstractItemModel_index(dataModelHandle, index,
                0, 0);
        OS.ListModel_beginRemoveRows(dataModelHandle, 0, index, index);
        OS.ListModel_remove(dataModelHandle, index);
        OS.ListModel_endRemoveRows(dataModelHandle);
        OS.QModelIndex_delete(indexHandle);
    }

    void listBase_remove(String string) {
        checkWidget();
        if (string == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        int index = listBase_indexOf(string, 0);
        if (index < 0)
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        listBase_remove(index);
    }

    void listBase_remove(int start, int end) {
        checkWidget();
        if (start > end)
            return;
        int count = listBase_getItemCount();
        if (start < 0 || end > count - 1)
            SWT.error(SWT.ERROR_INVALID_RANGE);
        OS.ListModel_beginRemoveRows(dataModelHandle, 0, start, end);
        for (int index = end; index >= start; index--) {
            int indexHandle = OS.QAbstractItemModel_index(dataModelHandle,
                    index, 0, 0);
            OS.ListModel_remove(dataModelHandle, index);
            OS.QModelIndex_delete(indexHandle);
        }
        OS.ListModel_endRemoveRows(dataModelHandle);
    }

    void listBase_remove(int[] indices) {
        checkWidget();
        if (indices == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        int removeCount = indices.length;
        if (removeCount < 1)
            return;

        int[] newIndices = new int[indices.length];
        System.arraycopy(indices, 0, newIndices, 0, indices.length);
        ListUtils.sort(newIndices);
        int count = listBase_getItemCount();
        int start = newIndices[0], end = newIndices[newIndices.length - 1];
        if (start < 0 || end > count - 1) {
            SWT.error(SWT.ERROR_INVALID_RANGE);
        }
        int previousIndex = -1;
        for (int i = removeCount - 1; i > -1; i--) {
            if (newIndices[i] == previousIndex)
                continue;
            listBase_remove(newIndices[i]);
            previousIndex = newIndices[i];
        }
    }

    void listBase_removeAll() {
        checkWidget();
        int count = listBase_getItemCount();
        if (count < 1)
            return;
        OS.ListModel_beginRemoveRows(dataModelHandle, 0, 0, count - 1);
        OS.ListModel_clearList(dataModelHandle);
        OS.ListModel_endRemoveRows(dataModelHandle);
    }

    void listBase_setItems(String[] items) {
        checkWidget();
        if (items == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);

        listBase_removeAll();
        int count = items.length;
        if (count < 1)
            return;
        OS.ListModel_beginInsertRows(dataModelHandle, 0, 0, count - 1);
        for (int i = 0; i < count; i++) {
            if (items[i] == null)
                SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            OS.ListModel_append(dataModelHandle, items[i]);
        }
        OS.ListModel_endInsertRows(dataModelHandle);
    }

    int listBase_getTopIndex() {
        // TODO: consider the situation on listview icon mode, rigt to left
        checkWidget();
        if (listBase_getItemCount() < 1)
            return 0;

        return OS.QAbstractItemView_swt_indexAt(topHandle(), 1, 1);
    }

    void listBase_setTopIndex(int index) {
        checkWidget();
        if (index < 0 || index > listBase_getItemCount() - 1)
            return;
        int indexHandle = OS.QAbstractItemModel_index(dataModelHandle, index,
                0, 0);
        OS.QAbstractItemView_scrollTo(topHandle(), indexHandle,
                OS.QT_ABSTRACTITEMVIEW_SCROLLHINT_POSITIONATTOP);
        OS.QModelIndex_delete(indexHandle);
    }

    /**
     * Notifies this ListBox that the data for the item at the given index has
     * been updated and the item display needs to be refreshed.
     *
     * @param index
     *            an index into the data model array
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_RANGE - if the index is not between 0
     *                and the number of elements in the array minus 1
     *                (inclusive)</li>
     *                </ul>
     *
     * @see ListBox#refreshList
     */
    public void refreshItem(int index) {
        listBase_refreshItem(index);
    }

    /**
     * Notifies this ListBox that multiple items may have been updated and the
     * entire list display needs to be refreshed.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     *
     * @see ListBox#refreshItem
     */
    public void refreshList() {
        listBase_refreshList();
    }

    /**
     * Removes the listener from the collection of listeners who are notified
     * when the receiver's selection changes.
     *
     * @param listener
     *            instance called when selection events occur
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     * @exception SWTError
     *                <ul>
     *                <li>ERROR_ITEM_NOT_REMOVED - if the operation fails
     *                because of an operating system failure</li>
     *                </ul>
     *
     * @see ListBox#addSelectionListener
     * @see SelectionListener
     */
    public void removeSelectionListener(SelectionListener listener) {
        this.listBase_removeSelectionListener(listener);
    }

    /**
     * Sets the font for the heading text of an item. If the argument is null,
     * the default font will be used.
     *
     * @param font
     *            the new font or null
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the argument has been
     *                disposed</li>
     *                </ul>
     *
     * #see ListBox#setFont
     */
    public void setHeadingFont(Font font) {
        checkWidget();
        if (delegateHandle == 0
                || (layoutStyle & (LB_STYLE_1LINE_ITEM | LB_STYLE_2LINE_ITEM)) == 0)
            /*
             * do not go continue if there is no delegate or no heading text
             * layout
             */
            return;
        if (headingFont == null && font == null)
            return;

        headingFont = font;

        int fontHandle = 0;
        if (font != null) {
            if (font.isDisposed())
                SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            fontHandle = font.handle;
        }
        OS.ListModel_layoutAboutToBeChanged(dataModelHandle);
        OS.ListBoxItemDelegate_setHeadingFont(delegateHandle, fontHandle);
        OS.ListModel_layoutChanged(dataModelHandle);
    }

    /**
     * Selects the item at the given zero-relative index in the receiver's list.
     * If the item at the index was already selected, it remains selected.
     * Indices that are out of range are ignored.
     *
     * @param index
     *            the index of the item to select
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void select(int index) {
        listBase_select(index);
    }

    /**
     * Selects the items in the range specified by the given zero-relative
     * indices in the receiver. The range of indices is inclusive. The current
     * selection is not cleared before the new items are selected.
     * <p>
     * If an item in the given range is not selected, it is selected. If an item
     * in the given range was already selected, it remains selected. Indices
     * that are out of range are ignored and no items will be selected if start
     * is greater than end. If the receiver is single-select and there is more
     * than one item in the given range, then all indices are ignored.
     *
     * @param start
     *            the start of the range
     * @param end
     *            the end of the range
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *
     * @see #setSelection(int,int)
     */
    public void select(int start, int end) {
        listBase_select(start, end);
    }

    /**
     * Selects the items at the given zero-relative indices in the receiver. The
     * current selection is not cleared before the new items are selected.
     * <p>
     * If the item at a given index is not selected, it is selected. If the item
     * at a given index was already selected, it remains selected. Indices that
     * are out of range and duplicate indices are ignored. If the receiver is
     * single-select and multiple indices are specified, then all indices are
     * ignored.
     *
     * @param indices
     *            the array of indices for the items to select
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the array of indices is null
     *                </li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *
     * @see #setSelection(int[])
     */
    public void select(int[] indices) {
        listBase_select(indices);
    }

    /**
     * Selects all of the items in the receiver.
     * <p>
     * If the receiver is single-select, do nothing.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void selectAll() {
        listBase_selectAll();
    }

    /**
     * Establishes the data model for this ListBox. The provided array is used
     * for the life of the ListBox or until a new data model is set. Elements of
     * a ListBoxItem which are <code>null</code> or disposed are not
     * displayed, leaving a blank area within the layout.
     *
     * @param items
     *            an array of ListBoxItems or <code>null</code>
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the items is null</li>
     *                </ul>
     *
     * @see ListBoxItem
     */
    public void setDataModel(ListBoxItem[] items) {
        checkWidget();
        if (items == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);

        if (listBase_getItemCount() > 0) {
            OS.ListModel_clearList(dataModelHandle);
        }

        int count = items.length;
        if (count < 1) {
            headingImages = null;
            detailImages = null;
            return;
        }
        this.items = items;
        headingImages = new Image[count][];
        detailImages = new Image[count][];
        OS.ListModel_beginInsertRows(dataModelHandle, 0, 0, count - 1);
        int[] detailImageHandles;
        int[] headingImageHandles;
        int imageCount;
        for (int i = 0; i < count; i++) {
            if (items[i]!=null && items[i].detailIcons != null) {
                imageCount = items[i].detailIcons.length;
                detailImageHandles = new int[imageCount];
                for (int j = 0; j < imageCount; j++) {
                    detailImageHandles[j] = Internal_GfxPackageSupport.getImageHandle(items[i].detailIcons[j]);
                }
            } else {
                detailImageHandles = null;
            }

            if ( items[i]!=null && items[i].headingIcons != null) {
                imageCount = items[i].headingIcons.length;
                headingImageHandles = new int[imageCount];
                for (int j = 0; j < imageCount; j++) {
                    headingImageHandles[j] = Internal_GfxPackageSupport.getImageHandle(items[i].headingIcons[j]);
                }
            } else {
                headingImageHandles = null;
            }
            if(items[i]!=null){
            	OS.ListModel_append(dataModelHandle, items[i].detailText,
                    detailImageHandles,
                    detailImageHandles != null ? detailImageHandles.length : 0,
                    items[i].headingText, headingImageHandles,
                    headingImageHandles != null ? headingImageHandles.length
                            : 0);
            	headingImages[i] = items[i].getHeadingIcons();
            	detailImages[i] = items[i].getDetailIcons();
        	} else {
        		OS.ListModel_append(dataModelHandle, null, detailImageHandles,
                        detailImageHandles != null ? detailImageHandles.length : 0,
                        null, headingImageHandles, headingImageHandles != null ? headingImageHandles.length : 0);
        		headingImages[i] = null;
            	detailImages[i] = null;
        	}
        }
        OS.ListModel_endInsertRows(dataModelHandle);
    }

    /**
     * Selects the item at the given zero-relative index in the receiver. If the
     * item at the index was already selected, it remains selected. The current
     * selected is first cleared, then the new items are selected. Index that is
     * out of range are ignored.
     *
     * @param index
     *            the index of the item to select
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setSelection(int index) {
        listBase_setSelection(index);
    }

    /**
     * Selects the items in the range specified by the given zero-relative
     * indices in the receiver. The range of indices is inclusive. The current
     * selection is cleared before the new items are selected.
     * <p>
     * Indices that are out of range are ignored and no items will be selected
     * if start is greater than end. If the receiver is single-select and there
     * is more than one item in the given range, then all indices are ignored.
     *
     * @param start
     *            the start index of the items to select
     * @param end
     *            the end index of the items to select
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *
     */
    public void setSelection(int start, int end) {
        listBase_setSelection(start, end);
    }

    /**
     * Selects the items at the given zero-relative indices in the receiver. The
     * current selection is cleared before the new items are selected.
     * <p>
     * Indices that are out of range and duplicate indices are ignored. If the
     * receiver is single-select and multiple indices are specified, then all
     * indices are ignored.
     *
     * @param indices
     *            the indices of the items to select
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the array of indices is null
     *                </li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *
     * @see #deselectAll()
     * @see #select(int[])
     */
    public void setSelection(int[] indices) {
        listBase_setSelection(indices);
    }

    /**
     * Shows the selection. If the selection is already showing in the receiver,
     * this method simply returns. Otherwise, the items are scrolled until the
     * selection is visible. When multiple items are selected, the selection of
     * which item becomes visible is implementation-dependent.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void showSelection() {
        listBase_showSelection();
    }

    void releaseHandle() {
        Internal_PackageSupport.releaseHandle(this);
        dataModelHandle = 0;
        selectionModelHandle = 0;
    }

    void releaseWidget() {
        Internal_PackageSupport.releaseWidget(this);
        headingImages = null;
        detailImages = null;
    }

    int createDataModel() {
        listType = LISTTYPE_LISTBOX;
        return OS.ListModel_new(layoutStyle, topHandle(), listType);
    }

    private final int topHandle() {
        return Internal_PackageSupport.topHandle(this);
    }

    private final int style() {
        return Internal_PackageSupport.style(this);
    }
}
