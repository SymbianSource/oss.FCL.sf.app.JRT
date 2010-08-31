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
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Internal_GfxPackageSupport;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.WidgetState;

/**
 * Instances of this class represent a selectable user interface object that
 * issues notification when pressed and released. 
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>CHECK, PUSH, RADIO, TOGGLE, FLAT</dd>
 * <dd>UP, DOWN, LEFT, RIGHT, CENTER</dd>
 * <dt><b>Events:</b></dt>
 * <dd>Selection</dd>
 * </dl>
 * <p>
 * Note: Only one of the styles CHECK, PUSH, RADIO, and TOGGLE 
 * may be specified.
 * </p><p>
 * Note: Only one of the styles LEFT, RIGHT, and CENTER may be specified.
 * </p><p>
 * Note: Only one of the styles UP, DOWN, LEFT, and RIGHT may be specified
 * </p><p>
 * IMPORTANT: This class is intended to be subclassed <em>only</em>
 * within the SWT implementation.
 * </p>
 */
public class Button extends Control {
    int alignment;
    Image image;

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
     * @see SWT#CHECK
     * @see SWT#PUSH
     * @see SWT#RADIO
     * @see SWT#TOGGLE
     * @see SWT#FLAT
     * @see SWT#LEFT
     * @see SWT#RIGHT
     * @see SWT#CENTER
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public Button (Composite parent, int style) {
        super (parent, checkStyle (style));
    }

    static int checkStyle (int style) {
        style = checkBits (style, SWT.PUSH, SWT.CHECK, SWT.RADIO, SWT.TOGGLE, 0, 0);
        if ((style & (SWT.PUSH | SWT.TOGGLE)) != 0) {
            return checkBits (style, SWT.CENTER, SWT.LEFT, SWT.RIGHT, 0, 0, 0);
        }
        if ((style & (SWT.CHECK | SWT.RADIO)) != 0) {
            return checkBits (style, SWT.LEFT, SWT.RIGHT, SWT.CENTER, 0, 0, 0);
        }

        return style;
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notified when the control is selected by the user, by sending
     * it one of the messages defined in the <code>SelectionListener</code>
     * interface.
     * <p>
     * <code>widgetSelected</code> is called when the control is selected by the user.
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
    public void addSelectionListener (SelectionListener listener) {
        checkWidget ();
        if (listener == null) error (SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener (listener);
        addListener (SWT.Selection,typedListener);
    }

    void createHandle_pp (int index) {

        int bits = SWT.TOGGLE | SWT.CHECK | SWT.RADIO | SWT.PUSH;

        switch (style & bits) {
        case SWT.CHECK:
            handle = OS.QCheckBox_new();
            break;
        case SWT.RADIO:
            handle = OS.QRadioButton_new();
            if((getParent().style & SWT.NO_RADIO_GROUP) != 0) {
                OS.QAbstractButton_setAutoExclusive(handle, false);
            }
            break;
        case SWT.TOGGLE:
            // fall through
        case SWT.PUSH:
            // fall through
        default:
            handle = OS.QPushButton_new();
            if((style & SWT.TOGGLE) != 0) {
                OS.QAbstractButton_setCheckable(handle, true);      
            }
            if((style & SWT.FLAT) != 0) {
                OS.QPushButton_setFlat(handle, true);
            }
            break;
        }
        topHandle = handle;
        state |= WidgetState.HANDLE;
    }

    void hookEvents_pp () {
        int signalProxy = OS.SignalHandler_new(handle, OS.QSIGNAL_CLICKED);
        OS.QObject_connectOrThrow(handle, "clicked()", signalProxy, "widgetSignal()", OS.QT_AUTOCONNECTION);
    }

    /**
     * Returns a value which describes the position of the
     * text or image in the receiver. The value will be one of
     * <code>LEFT</code>, <code>RIGHT</code> or <code>CENTER</code>
     *
     * @return the alignment 
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getAlignment () {
        checkWidget ();

        if ((alignment & SWT.LEFT) != 0) return SWT.LEFT;
        if ((alignment & SWT.CENTER) != 0) return SWT.CENTER;
        if ((alignment & SWT.RIGHT) != 0) return SWT.RIGHT;

        return SWT.LEFT;
    }

    /**
     * Returns the receiver's image if it has one, or null
     * if it does not.
     *
     * @return the receiver's image
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Image getImage () {
        checkWidget ();
        return image;
    }

    String getNameText () {
        return getText ();
    }

    /**
     * Returns <code>true</code> if the receiver is selected,
     * and false otherwise.
     * <p>
     * When the receiver is of type <code>CHECK</code> or <code>RADIO</code>,
     * it is selected when it is checked. When it is of type <code>TOGGLE</code>,
     * it is selected when it is pushed in. If the receiver is of any other type,
     * this method returns false.
     *
     * @return the selection state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean getSelection () {
        checkWidget ();
        if ((style & (SWT.CHECK | SWT.RADIO | SWT.TOGGLE)) == 0) return false;
        return OS.QAbstractButton_isChecked(handle);
    }

    /**
     * Returns the receiver's text, which will be an empty
     * string if it has never been set 
     *
     * @return the receiver's text
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public String getText () {
        checkWidget();
        return OS.QAbstractButton_text(handle);
    }

    void qt_swt_event_focusWasGained() {
        super.qt_swt_event_focusWasGained();
        if(display == null || display.isDisposed()) return;
        
        // This button overrides the default button when focused
        if ((style & SWT.PUSH) != 0) {
            menuShell ().setDefaultButton (this, false);
        }
    }

    void qt_swt_event_focusWasLost() {
        super.qt_swt_event_focusWasLost();
        if(display == null || display.isDisposed()) return;

        // Restore the original default button if this button was overriding it
        if ((style & SWT.PUSH) != 0 && OS.QPushButton_isDefault(handle)) {
            menuShell ().setDefaultButton (null, false);
        }
    }

    void qt_signal_clicked() {
        sendEvent (SWT.Selection);
    }

    void releaseWidget_pp () {
        super.releaseWidget_pp ();
        image = null;
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
    public void removeSelectionListener (SelectionListener listener) {
        checkWidget();
        if (listener == null) error (SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook (SWT.Selection, listener);
    }

    /**
     * Controls how text, images will be displayed
     * in the receiver. The argument should be one of
     * <code>LEFT</code>, <code>RIGHT</code> or <code>CENTER</code>
     *
     * @param alignment the new alignment 
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setAlignment (int alignment) {
        checkWidget ();
        this.alignment = alignment;
    }

    /**
     * Sets the receiver's image to the argument, which may be
     * <code>null</code> indicating that no image should be displayed.
     * 
     * @param image the image to display on the receiver (may be <code>null</code>)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the image has been disposed</li>
     * </ul> 
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setImage (Image image) {
        checkWidget ();
        if (image != null && image.isDisposed()) error(SWT.ERROR_INVALID_ARGUMENT);
        this.image = image;
        if (image != null ) {
            Rectangle iconRect = image.getBounds();
            OS.QAbstractButton_setIconSize(handle, iconRect.width, iconRect.height);
            OS.QAbstractButton_setIcon(handle, Internal_GfxPackageSupport.getIconHandle(image));
        } else {
            OS.QAbstractButton_setIcon(handle, Internal_GfxPackageSupport.getNullIconHandle());
        }
    }

    /**
     * Sets the selection state of the receiver, if it is of type <code>CHECK</code>, 
     * <code>RADIO</code>, or <code>TOGGLE</code>.
     *
     * <p>
     * When the receiver is of type <code>CHECK</code> or <code>RADIO</code>,
     * it is selected when it is checked. When it is of type <code>TOGGLE</code>,
     * it is selected when it is pushed in.
     *
     * @param selected the new selection state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setSelection (boolean selected) {
        checkWidget();
        if ((style & (SWT.CHECK | SWT.RADIO | SWT.TOGGLE)) == 0) return;
        OS.QAbstractButton_setChecked(handle, selected);
    }

    /**
     * Sets the receiver's text.
     * <p>
     * This method sets the button label.  The label may include
     * the mnemonic character but must not contain line delimiters.
     * </p>
     * <p>
     * Mnemonics are indicated by an '&amp;' that causes the next
     * character to be the mnemonic.  When the user presses a
     * key sequence that matches the mnemonic, a selection
     * event occurs. On most platforms, the mnemonic appears
     * underlined but may be emphasized in a platform specific
     * manner.  The mnemonic indicator character '&amp;' can be
     * escaped by doubling it in the string, causing a single
     * '&amp;' to be displayed.
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
     */
    public void setText (String string) {
        checkWidget ();
        if (string == null) error (SWT.ERROR_NULL_ARGUMENT);
        OS.QAbstractButton_setText(handle, string);
    }

    boolean setRadioFocus (boolean tabbing) {
        if ((style & SWT.RADIO) == 0 || !getSelection ()) return false;
        return tabbing ? setTabItemFocus () : setFocus (tabbing ? OS.QT_TABFOCUSREASON : OS.QT_OTHERFOCUSREASON);
    }

    boolean isTabGroup () {
        // The default behavior is to traverse to a Control which accepts focus
        // by tabbing. In the case of a radio button group only the radio button
        // which is selected accepts focus by tabbing. Mobile devices often
        // don't have more than arrow keys for traversing which are used for tab
        // group traversal. It must be possible to traverse to any of the radio
        // buttons within the group. Therefore, this behavior is overridden so
        // that all buttons accept focus from tab group traversal.
        if((style & SWT.RADIO) != 0) {
            return true;
            
        }
        else {
            return super.isTabGroup();
        }
    }
    
    Control getShortcutHandler() {
        // Button shortcut events are just let through to the native Button. 
        // This will happen if we don't return any handler. 
        return null;
    }

    void setDefault (boolean value) {
        if ((style & SWT.PUSH) == 0) return;
        OS.QPushButton_setDefault(handle, value);
    }
}
