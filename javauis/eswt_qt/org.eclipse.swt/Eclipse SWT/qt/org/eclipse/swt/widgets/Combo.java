/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * Portion Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Lynne Kues (IBM Corp) - modified to reflect eSWT API subset
 *     Nokia Corporation - S60 implementation
 *     Nokia Corporation - Qt implementation
 *******************************************************************************/
package org.eclipse.swt.widgets;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.events.VerifyListener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Internal_GfxPackageSupport;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.TextUtils;
import org.eclipse.swt.internal.qt.WidgetConstant;
import org.eclipse.swt.internal.qt.WidgetState;

/**
 * Instances of this class are controls that allow the user to choose an item
 * from a list of items, or optionally enter a new value by typing it into an
 * editable text field. Often, <code>Combo</code>s are used in the same place
 * where a single selection <code>List</code> widget could be used but space is
 * limited. A <code>Combo</code> takes less space than a <code>List</code>
 * widget and shows similar information.
 * <p>
 * Note: Since <code>Combo</code>s can contain both a list and an editable text
 * field, it is possible to confuse methods which access one versus the other
 * (compare for example, <code>clearSelection()</code> and
 * <code>deselectAll()</code>). The API documentation is careful to indicate
 * either "the receiver's list" or the "the receiver's text field" to
 * distinguish between the two cases.
 * </p>
 * <p>
 * Note that although this class is a subclass of <code>Composite</code>, it
 * does not make sense to add children to it, or set a layout on it.
 * </p>
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>DROP_DOWN, READ_ONLY</dd>
 * <dt><b>Events:</b></dt>
 * <dd>DefaultSelection, Modify, Selection</dd>
 * </dl>
 * <p>
 * </p>
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 * 
 * @see List
 */

public class Combo extends Composite {

    int comboTextHandle; // needed for mouse events and defaultSelection

    /**
     * the operating system limit for the number of characters that the text
     * field in an instance of this class can hold
     */
    public static final int LIMIT = 32767;

    /**
     * Constructs a new instance of this class given its parent and a style
     * value describing its behavior and appearance.
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or must
     * be built by <em>bitwise OR</em>'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. The class description lists the style constants that are
     * applicable to the class. Style bits are also inherited from superclasses.
     * </p>
     * 
     * @param parent
     *            a composite control which will be the parent of the new
     *            instance (cannot be null)
     * @param style
     *            the style of control to construct
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
     * @see SWT#DROP_DOWN
     * @see SWT#READ_ONLY
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public Combo(Composite parent, int style) {
        super(parent, style);
    }

    void createHandle_pp(int index) {
        topHandle = handle = OS.QComboBox_new();
        // QComboBox to use text editor instead of button
        OS.QComboBox_setEditable(handle, true);
        OS.QComboBox_setSizeAdjustPolicy(handle,
                OS.QT_QComboBox_AdjustToMinimumContentsLength);
        // not to add edited text at the end
        OS.QComboBox_setInsertPolicy(handle, OS.QT_QComboBox_NoInsert);
        comboTextHandle = OS.QComboBox_lineEdit(handle);
        if ((style & SWT.READ_ONLY) != 0) {
            OS.QLineEdit_setReadOnly(comboTextHandle, true);
        }
        state |= WidgetState.HANDLE;
    }

    void register_pp() {
        super.register_pp();
        display.addWidget(comboTextHandle, this);
    }

    void deregister_pp() {
        super.deregister_pp();
        display.removeWidget(comboTextHandle);
    }

    void releaseHandle_pp() {
        comboTextHandle = 0;
        super.releaseHandle_pp();
    }

    /**
     * Adds the argument to the end of the receiver's list.
     * 
     * @param string
     *            the new item
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @see #add(String,int)
     */
    public void add(String string) {
        add(string, getItemCount());
    }

    /**
     * Adds the argument to the receiver's list at the given zero-relative
     * index.
     * <p>
     * Note: To add an item at the end of the list, use the result of calling
     * <code>getItemCount()</code> as the index or use <code>add(String)</code>.
     * </p>
     * 
     * @param string
     *            the new item
     * @param index
     *            the index for the item
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     *                <li>ERROR_INVALID_RANGE - if the index is not between 0
     *                and the number of elements in the list (inclusive)</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @see #add(String)
     */
    public void add(String string, int index) {
        checkWidget();
        if (string == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        int count = getItemCount();
        if (!(index <= count && index >= 0))
            error(SWT.ERROR_INVALID_RANGE);
        OS.QComboBox_insertItem(topHandle, string, index);
    }

    /**
     * Adds the listener to the collection of listeners who will be notified
     * when the receiver's text is modified, by sending it one of the messages
     * defined in the <code>ModifyListener</code> interface.
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
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @see ModifyListener
     * @see #removeModifyListener
     */
    public void addModifyListener(ModifyListener listener) {
        checkWidget();
        if (listener == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Modify, typedListener);
    }

    /**
     * Adds the listener to the collection of listeners who will be notified
     * when the receiver's text is verified, by sending it one of the messages
     * defined in the <code>VerifyListener</code> interface.
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
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @see VerifyListener
     * @see #removeVerifyListener
     */
    public void addVerifyListener(VerifyListener listener) {
        checkWidget();
        if (listener == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Verify, typedListener);
    }

    /**
     * Adds the listener to the collection of listeners who will be notified
     * when the receiver's selection changes, by sending it one of the messages
     * defined in the <code>SelectionListener</code> interface.
     * <p>
     * <code>widgetSelected</code> is called when the combo's list selection
     * changes. <code>widgetDefaultSelected</code> is typically called when
     * ENTER is pressed the combo's text area.
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
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @see SelectionListener
     * @see #removeSelectionListener
     * @see SelectionEvent
     */
    public void addSelectionListener(SelectionListener listener) {
        checkWidget();
        if (listener == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Selection, typedListener);
        addListener(SWT.DefaultSelection, typedListener);
    }

    void hookEvents_pp() {
        super.hookEvents_pp();

        int selectionSignalProxy = OS.SignalHandler_new(topHandle,
                display, OS.QSIGNAL_SELECTIONCHANGED);
        OS.QObject_connectOrThrow(topHandle,
                "currentIndexChanged(const QString&)", selectionSignalProxy,
                "widgetSignal(const QString&)", OS.QT_AUTOCONNECTION);

        int returnPressedProxy = OS.SignalHandler_new(comboTextHandle, display,
                OS.QSIGNAL_RETURN_PRESSED);
        OS.QObject_connectOrThrow(comboTextHandle, "returnPressed()",
                returnPressedProxy, "widgetSignal()", OS.QT_AUTOCONNECTION);

        int modifySignalProxy = OS.SignalHandler_new(topHandle,
                display, OS.QSIGNAL_TEXT_CHANGED);
        OS.QObject_connectOrThrow(topHandle,
                "editTextChanged(const QString&)", modifySignalProxy,
                "widgetSignal(const QString&)", OS.QT_AUTOCONNECTION);
    }

    void qt_signal_text_changed_pp(String text) {
        sendEvent(SWT.Modify);
    }

    void qt_signal_selectionChanged_pp(int selectionHandle,
            int deSelectionHandle) {
        // this is hack to select text in combo editor when changes the current
        // item in combo
        setSelection(new Point(0, OS.QLineEdit_text(comboTextHandle).length()));
        sendEvent(SWT.Selection);
    }

    void qt_signal_return_pressed_pp() {
        sendEvent(SWT.DefaultSelection);
    }

    boolean qt_event_keypress_pp(int widgetHandle, int key,
            int modifier, int character, int nativeScanCode) {
        if (super.qt_event_keypress_pp(widgetHandle, key, modifier, character, nativeScanCode))
            return true;
        
        if (!(hooks(SWT.Verify) || filters(SWT.Verify)) && getTextLimit() <= 0) {
            return false;
        }

        Event keyEv = makeKeyEvent(key, modifier, character, nativeScanCode);
        return TextUtils.handle_keypress(TextUtils.LINE_EDIT, comboTextHandle, 
            key, modifier, character, keyEv, getTextLimit(), this);

    }

    int getTraversalTypeForKey(int key, int modifier, int character) {

        int type = 0;
        switch (key) {
        case OS.QT_KEY_ENTER:
        case OS.QT_KEY_RETURN:
            type = SWT.TRAVERSE_NONE;
            break;
        default:
            type = super.getTraversalTypeForKey(key, modifier, character);
            break;
        }
        return type;
    }

    void setTraversalFlags_pp(int type, int key, int modifier,
            int character) {
        traverseDoit = false;
        traverseCancel = false;

        // If the system is about to traverse out of the Text because mnemonic
        // shortcut of some widget was pressed then it is allowed.
        // Note that if keys used to launch shortcuts overlap the keys that are
        // used
        // by Text then shortcut has priority over text with this
        // implementation.
        // However, platforms usually define the shortucts in such a way that
        // they
        // don't overlap. E.g. in X11 Alt+something.
        if (type == SWT.TRAVERSE_MNEMONIC) {
            traverseDoit = true;
            return;
        }

        switch (key) {
        case OS.QT_KEY_ENTER:
        case OS.QT_KEY_RETURN:
            traverseDoit = false;
            break;
        case OS.QT_KEY_TAB:
            traverseDoit = true;
            break;
        case OS.QT_KEY_LEFT:
        case OS.QT_KEY_RIGHT:
            traverseDoit = false;
            break;
        case OS.QT_KEY_UP:
        case OS.QT_KEY_DOWN:
            traverseDoit = true;
            break;
        default:
            break;
        }
    }

    /**
     * Sets the selection in the receiver's text field to an empty selection
     * starting just before the first character. If the text field is editable,
     * this has the effect of placing the i-beam at the start of the text.
     * <p>
     * Note: To clear the selected items in the receiver's list, use
     * <code>deselectAll()</code>.
     * </p>
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @see #deselectAll
     */
    public void clearSelection() {
        checkWidget();
        OS.QLineEdit_setCursorPosition(comboTextHandle, 0);
    }

    /**
     * Copies the selected text.
     * <p>
     * The current selection is copied to the clipboard.
     * </p>
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @since 2.1
     */
    public void copy() {
        checkWidget();
        TextUtils.copy(TextUtils.LINE_EDIT, comboTextHandle);
    }

    /**
     * Cuts the selected text.
     * <p>
     * The current selection is first copied to the clipboard and then deleted
     * from the widget.
     * </p>
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @since 2.1
     */
    public void cut() {
        checkWidget();
        TextUtils.cut(TextUtils.LINE_EDIT, comboTextHandle, this);
    }

    /**
     * Deselects the item at the given zero-relative index in the receiver's
     * list. If the item at the index was already deselected, it remains
     * deselected. Indices that are out of range are ignored.
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
        checkWidget();
        if (getSelectionIndex() == index)
            deselectAll();
    }

    /**
     * Deselects all selected items in the receiver's list.
     * <p>
     * Note: To clear the selection in the receiver's text field, use
     * <code>clearSelection()</code>.
     * </p>
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @see #clearSelection
     */
    public void deselectAll() {
        checkWidget();
        OS.QComboBox_swt_deselect(topHandle);
    }

    /**
     * Returns the item at the given, zero-relative index in the receiver's
     * list. Throws an exception if the index is out of range.
     * 
     * @param index
     *            the index of the item to return
     * @return the item at the given index
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_RANGE - if the index is not between 0
     *                and the number of elements in the list minus 1 (inclusive)
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
    public String getItem(int index) {
        checkWidget();
        int count = getItemCount();
        if (!(index < count && index >= 0))
            error(SWT.ERROR_INVALID_RANGE);

        String item = OS.QComboBox_itemText(topHandle, index);
        return item;
    }

    /**
     * Returns the number of items contained in the receiver's list.
     * 
     * @return the number of items
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public int getItemCount() {
        checkWidget();
        return OS.QComboBox_count(topHandle);
    }

    /**
     * Returns the height of the area which would be used to display
     * <em>one</em> of the items in the receiver's list.
     * 
     * @return the height of one item
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public int getItemHeight() {
        checkWidget();
        return OS.QComboBox_swt_getItemHeight(topHandle);
    }

    /**
     * Returns an array of <code>String</code>s which are the items in the
     * receiver's list.
     * <p>
     * Note: This is not the actual structure used by the receiver to maintain
     * its list of items, so modifying the array will not affect the receiver.
     * </p>
     * 
     * @return the items in the receiver's list
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public String[] getItems() {
        checkWidget();
        int count = getItemCount();
        String[] array = new String[count];
        for (int i = 0; i < count; ++i) {
            array[i] = getItem(i);
        }

        return array;
    }

    /**
     * Returns the orientation of the receiver.
     * 
     * @return the orientation style
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @since 2.1.2
     */
    public int getOrientation() {
        checkWidget();
        return style & (SWT.LEFT_TO_RIGHT | SWT.RIGHT_TO_LEFT);
    }

    /**
     * Returns a <code>Point</code> whose x coordinate is the start of the
     * selection in the receiver's text field, and whose y coordinate is the end
     * of the selection. The returned values are zero-relative. An "empty"
     * selection as indicated by the the x and y coordinates having the same
     * value.
     * 
     * @return a point representing the selection start and end
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public Point getSelection() {
        checkWidget();
        return OS.QComboBox_swt_getSelection(topHandle);
    }

    /**
     * Returns the zero-relative index of the item which is currently selected
     * in the receiver's list, or -1 if no item is selected.
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
    public int getSelectionIndex() {
        checkWidget();
        return OS.QComboBox_currentIndex(topHandle);
    }

    /**
     * Returns a string containing a copy of the contents of the receiver's text
     * field.
     * 
     * @return the receiver's text
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public String getText() {
        checkWidget();
        return TextUtils.getText(TextUtils.LINE_EDIT, comboTextHandle);
    }

    /**
     * Returns the height of the receivers's text field.
     * 
     * @return the text height
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public int getTextHeight() {
        checkWidget();
        return OS.QComboBox_swt_getTextHeight(topHandle);
    }

    /**
     * Returns the maximum number of characters that the receiver's text field
     * is capable of holding. If this has not been changed by
     * <code>setTextLimit()</code>, it will be the constant
     * <code>Combo.LIMIT</code>.
     * 
     * @return the text limit
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public int getTextLimit() {
        checkWidget();
        return OS.QLineEdit_maxLength(comboTextHandle);
    }

    /**
     * Gets the number of items that are visible in the drop down portion of the
     * receiver's list.
     * 
     * @return the number of items that are visible
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @since 3.0
     */
    public int getVisibleItemCount() {
        checkWidget();
        return OS.QComboBox_maxVisibleItems(topHandle);
    }

    /**
     * Searches the receiver's list starting at the first item (index 0) until
     * an item is found that is equal to the argument, and returns the index of
     * that item. If no item is found, returns -1.
     * 
     * @param string
     *            the search item
     * @return the index of the item
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public int indexOf(String string) {
        return indexOf(string, 0);
    }

    /**
     * Searches the receiver's list starting at the given, zero-relative index
     * until an item is found that is equal to the argument, and returns the
     * index of that item. If no item is found or the starting index is out of
     * range, returns -1.
     * 
     * @param string
     *            the search item
     * @param start
     *            the zero-relative index at which to begin the search
     * @return the index of the item
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public int indexOf(String string, int start) {
        checkWidget();
        if (string == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        int index = OS.QComboBox_findText(topHandle, string);
        if (!(0 <= start && start <= index))
            return -1;
        return index;
    }

    /**
     * Pastes text from clipboard.
     * <p>
     * The selected text is deleted from the widget and new text inserted from
     * the clipboard.
     * </p>
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @since 2.1
     */
    public void paste() {
        checkWidget();
        TextUtils.paste(TextUtils.LINE_EDIT, comboTextHandle, getTextLimit(), this);
    }

    /**
     * Removes the item from the receiver's list at the given zero-relative
     * index.
     * 
     * @param index
     *            the index for the item
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_RANGE - if the index is not between 0
     *                and the number of elements in the list minus 1 (inclusive)
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
    public void remove(int index) {
        checkWidget();
        int count = getItemCount();
        if (!(index < count && index >= 0))
            error(SWT.ERROR_INVALID_RANGE);
        OS.QComboBox_removeItem(topHandle, index);
    }

    /**
     * Removes the items from the receiver's list which are between the given
     * zero-relative start and end indices (inclusive).
     * 
     * @param start
     *            the start of the range
     * @param end
     *            the end of the range
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_RANGE - if either the start or end are
     *                not between 0 and the number of elements in the list minus
     *                1 (inclusive)</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void remove(int start, int end) {
        checkWidget();
        int count = getItemCount();
        if (!((start < count && start >= 0) && end < count))
            error(SWT.ERROR_INVALID_RANGE);
        if (start > end)
            return;
        for (int i = end; i >= start; --i) {
            OS.QComboBox_removeItem(topHandle, i);
        }
    }

    /**
     * Searches the receiver's list starting at the first item until an item is
     * found that is equal to the argument, and removes that item from the list.
     * 
     * @param string
     *            the item to remove
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     *                <li>ERROR_INVALID_ARGUMENT - if the string is not found in
     *                the list</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void remove(String string) {
        checkWidget();
        int index = indexOf(string, 0);
        if (index == -1)
            error(SWT.ERROR_INVALID_ARGUMENT);
        remove(index);
    }

    /**
     * Removes all of the items from the receiver's list.
     * <p>
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public void removeAll() {
        checkWidget();
        OS.QComboBox_clear(topHandle);
    }

    /**
     * Removes all of the items from the receiver's list and clear the contents
     * of receiver's text field.
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
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     * 
     * @see ModifyListener
     * @see #addModifyListener
     */
    public void removeModifyListener(ModifyListener listener) {
        checkWidget();
        if (listener == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null)
            return;
        eventTable.unhook(SWT.Modify, listener);
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
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     * 
     * @see SelectionListener
     * @see #addSelectionListener
     */
    public void removeSelectionListener(SelectionListener listener) {
        checkWidget();
        if (listener == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null)
            return;
        eventTable.unhook(SWT.Selection, listener);
        eventTable.unhook(SWT.DefaultSelection, listener);
    }

    /**
     * Removes the listener from the collection of listeners who will be
     * notified when the control is verified.
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
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     * 
     * @see VerifyListener
     * @see #addVerifyListener
     */
    public void removeVerifyListener(VerifyListener listener) {
        checkWidget();
        if (listener == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null)
            return;
        eventTable.unhook(SWT.Verify, listener);
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
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public void select(int index) {
        checkWidget();
        int count = getItemCount();
        if (index < 0 || index >= count)
            return;
        OS.QComboBox_setCurrentIndex(topHandle, index);
    }

    /**
     * Sets the text of the item in the receiver's list at the given
     * zero-relative index to the string argument. This is equivalent to
     * <code>remove</code>'ing the old item at the index, and then
     * <code>add</code>'ing the new item at that index.
     * 
     * @param index
     *            the index for the item
     * @param string
     *            the new text for the item
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_RANGE - if the index is not between 0
     *                and the number of elements in the list minus 1 (inclusive)
     *                </li> <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public void setItem(int index, String string) {
        checkWidget();
        int count = getItemCount();
        if (index < 0 || index >= count)
            error(SWT.ERROR_INVALID_RANGE);
        if (string == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        OS.QComboBox_setItemText(topHandle, string, index);
    }

    /**
     * Sets the receiver's list to be the given array of items.
     * 
     * @param items
     *            the array of items
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the items array is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public void setItems(String[] items) {
        checkWidget();
        if (items == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        OS.QComboBox_swt_setItems(topHandle, items);
    }

    /**
     * Sets the orientation of the receiver, which must be one of the constants
     * <code>SWT.LEFT_TO_RIGHT</code> or <code>SWT.RIGHT_TO_LEFT</code>.
     * <p>
     * 
     * @param orientation
     *            new orientation style
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     * 
     * @since 2.1.2
     */
    public void setOrientation(int orientation) {
        state &= ~WidgetState.FOLLOWS_SYSLANG_DIRECTION;
        setOrientation(topHandle, orientation);
    }

    /**
     * Sets the selection in the receiver's text field to the range specified by
     * the argument whose x coordinate is the start of the selection and whose y
     * coordinate is the end of the selection.
     * 
     * @param selection
     *            a point representing the new selection start and end
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the point is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li> <li>ERROR_THREAD_INVALID_ACCESS - if not
     *                called from the thread that created the receiver</li>
     *                </ul>
     */
    public void setSelection(Point selection) {
        checkWidget();
        if (selection == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        TextUtils.setSelection(TextUtils.LINE_EDIT, comboTextHandle, selection.x, selection.y);
    }

    /**
     * Sets the contents of the receiver's text field to the given string.
     * <p>
     * Note: The text field in a <code>Combo</code> is typically only capable of
     * displaying a single line of text. Thus, setting the text to a string
     * containing line breaks or other special characters will probably cause it
     * to display incorrectly.
     * </p>
     * 
     * @param string
     *            the new text
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setText(String string) {
        checkWidget();
        if (string == null)
            error(SWT.ERROR_NULL_ARGUMENT);

        if (getText().equals(string) == true)
            return;
        string = TextUtils.verify(TextUtils.LINE_EDIT, comboTextHandle, string, 
            this, getTextLimit(), 0, OS.QLineEdit_text(comboTextHandle).length());
        if (string == null)
            return;

        OS.QComboBox_swt_setText(topHandle, string);
    }

    /**
     * Sets the maximum number of characters that the receiver's text field is
     * capable of holding to be the argument.
     * 
     * @param limit
     *            new text limit
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_CANNOT_BE_ZERO - if the limit is zero</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setTextLimit(int limit) {
        checkWidget();
        if (limit <= 0)
            error(SWT.ERROR_CANNOT_BE_ZERO);
        if (limit > LIMIT)
            limit = LIMIT;
        OS.QLineEdit_setMaxLength(comboTextHandle, limit);
    }

    /**
     * Sets the number of items that are visible in the drop down portion of the
     * receiver's list.
     * 
     * @param count
     *            the new number of items to be visible
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @since 3.0
     */
    public void setVisibleItemCount(int count) {
        checkWidget();
        OS.QComboBox_setMaxVisibleItems(topHandle, count);
    }

    int setBounds(int x, int y, int width, int height, boolean move,
            boolean resize) {
        int comboHeight = height;
        if (resize) {
            // Height of the combo is fixed
            Point preferedSize = OS.QWidget_sizeHint(topHandle);
            comboHeight = preferedSize.y;
            if (height < preferedSize.y)
                comboHeight = height;
        }
        return super.setBounds(x, y, width, comboHeight, move, resize);
    }

    public Point computeSize(int wHint, int hHint, boolean changed) {
        checkWidget();
        if (changed)
            OS.QWidget_updateGeometry(topHandle);

        // to get the maximum row length depending on the content,
        // set size policy to QComboBox::AdjustToContents. but it has drawback
        // that
        // it doesn't allow resizing so we have to set back size policy after
        // compute size
        OS.QComboBox_setSizeAdjustPolicy(topHandle,
                OS.QT_QComboBox_AdjustToContents);
        Point sizeHint = OS.QWidget_sizeHint(topHandle);
        int width = WidgetConstant.DEFAULT_WIDTH;
        int height = WidgetConstant.DEFAULT_HEIGHT;
        if (sizeHint.x >= 0)
            width = sizeHint.x;
        if (sizeHint.y >= 0)
            height = sizeHint.y;
        if (wHint != SWT.DEFAULT)
            width = wHint;
        if (hHint != SWT.DEFAULT && hHint < sizeHint.y)
            height = hHint;

        // sets the size policy back to default so that resizing will be allowed
        OS.QComboBox_setSizeAdjustPolicy(topHandle,
                OS.QT_QComboBox_AdjustToMinimumContentsLength);
        return new Point(width, height);
    }

    public Rectangle computeTrim(int x, int y, int width, int height) {
        checkWidget();

        Point frameSize = OS.QWidget_frameSize(topHandle);
        Rectangle viewPortRect = OS.QWidget_rect(handle);
        Point pos = OS.QWidget_mapTo(handle, topHandle,
                viewPortRect.x, viewPortRect.y);
        int leftTrim = pos.x;
        int topTrim = pos.y;
        int rightTrim = frameSize.x - viewPortRect.width - leftTrim;
        int buttomTrim = frameSize.y - viewPortRect.height - topTrim;

        Point computeSize = computeSize(SWT.DEFAULT, height, false);

        return new Rectangle(x - leftTrim, y - topTrim, width + leftTrim
                + rightTrim, (height < computeSize.y ? height : computeSize.y)
                + topTrim + buttomTrim);
    }

    void setBackground_pp () {
        // View might have custom palette in case background image was inherited.
        // Default palette needs to be restored.
        int view = OS.QComboBox_view(handle);
        if(view != 0) {
            OS.QWidget_swt_unsetPalette(view);
        }
        super.setBackground_pp();
    }

    void backgroundImageInherited(Image image) {
        // Combo can't inherit the background for the view pop-up. View has to draw
        // the background image by itself.
        int palette = 0;
        int view = OS.QComboBox_view(handle);
        if(view == 0) return;
        try {
            palette = OS.QWidget_swt_palette_new(view);
            int[] bkRoles = getBackgroundImageRoles();
            for(int i = 0; i < bkRoles.length; ++i) {
                OS.QPalette_swt_setBrush(palette, bkRoles[i], Internal_GfxPackageSupport.getPixmapHandle(image));
            }
            OS.QWidget_setPalette(view, palette);
        } finally {
            OS.QPalette_delete(palette);
        }
    }
}
