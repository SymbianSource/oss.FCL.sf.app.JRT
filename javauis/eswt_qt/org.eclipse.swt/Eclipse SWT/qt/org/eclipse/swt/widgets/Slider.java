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
import org.eclipse.swt.internal.qt.WidgetState;
import org.eclipse.swt.events.SelectionListener;

/**
 * Instances of this class are selectable user interface objects that represent
 * a range of positive, numeric values.
 * <p>
 * At any given moment, a given slider will have a single 'selection' that is
 * considered to be its value, which is constrained to be within the range of
 * values the slider represents (that is, between its <em>minimum</em> and
 * <em>maximum</em> values).
 * </p>
 * <p>
 * Typically, sliders will be made up of five areas:
 * <ol>
 * <li>an arrow button for decrementing the value</li>
 * <li>a page decrement area for decrementing the value by a larger amount</li>
 * <li>a <em>thumb</em> for modifying the value by mouse dragging</li>
 * <li>a page increment area for incrementing the value by a larger amount</li>
 * <li>an arrow button for incrementing the value</li>
 * </ol>
 * Based on their style, sliders are either <code>HORIZONTAL</code> (which have
 * a left facing button for decrementing the value and a right facing button for
 * incrementing it) or <code>VERTICAL</code> (which have an upward facing button
 * for decrementing the value and a downward facing buttons for incrementing
 * it).
 * </p>
 * <p>
 * On some platforms, the size of the slider's thumb can be varied relative to
 * the magnitude of the range of values it represents (that is, relative to the
 * difference between its maximum and minimum values). Typically, this is used
 * to indicate some proportional value such as the ratio of the visible area of
 * a document to the total amount of space that it would take to display it. SWT
 * supports setting the thumb size even if the underlying platform does not, but
 * in this case the appearance of the slider will not change.
 * </p>
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>HORIZONTAL, VERTICAL</dd>
 * <dt><b>Events:</b></dt>
 * <dd>Selection</dd>
 * </dl>
 * <p>
 * Note: Only one of the styles HORIZONTAL and VERTICAL may be specified.
 * </p>
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 * 
 * @see ScrollBar
 */
public class Slider extends Control {

    /**
     * This is used for storing the latest value that user has set the Slider
     * using setSelection(). If user changes the value of the Slider with
     * keys/mouse we get valueChanged() signal in which we send a
     * SelectionEvent. However, we get the same signal also after user sets the
     * selection using setSelection(). In that case we must not send a selection
     * event.
     */
    int     value          = -1;

    /**
     * QSlider does not have thumb support, so we emulate the API behavior in
     * Java side.
     */
    int     thumb          = 1;

    /**
     * This is used to store the information about key/mouse action that may
     * cause Slider value to change.
     */
    int     selectionCause = 0;

    /**
     * This is set to true when a DRAG event occurs, so that we can send a drag
     * end event when user releases the mouse.
     */
    boolean dragOccurred   = false;

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
     * @param parent a composite control which will be the parent of the new
     *               instance (cannot be null)
     * @param style the style of control to construct
     * 
     * @exception IllegalArgumentException <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the parent is null</li> 
     *                </ul>
     * @exception SWTException <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *                <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     *                </ul>
     * 
     * @see SWT#HORIZONTAL
     * @see SWT#VERTICAL
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public Slider(Composite parent, int style) {
        super(parent, checkStyle(style));
    }

    /**
     * Adds the listener to the collection of listeners who will be notified
     * when the user changes the receiver's value, by sending it one of the
     * messages defined in the <code>SelectionListener</code> interface.
     * <p>
     * When <code>widgetSelected</code> is called, the event object detail field
     * contains one of the following values: <code>SWT.NONE</code> - for the end
     * of a drag. <code>SWT.DRAG</code>. <code>SWT.HOME</code>.
     * <code>SWT.END</code>. <code>SWT.ARROW_DOWN</code>.
     * <code>SWT.ARROW_UP</code>. <code>SWT.PAGE_DOWN</code>.
     * <code>SWT.PAGE_UP</code>. <code>widgetDefaultSelected</code> is not
     * called.
     * </p>
     * 
     * @param listener the listener which should be notified when the user changes
     *                 the receiver's value
     * 
     * @exception IllegalArgumentException <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     *                </ul>
     * @exception SWTException  <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *                </ul>
     * 
     * @see SelectionListener
     * @see #removeSelectionListener
     * @see SelectionEvent
     */
    public void addSelectionListener(SelectionListener listener) {
        checkWidget();
        if (listener == null) error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Selection, typedListener);
        addListener(SWT.DefaultSelection, typedListener);
    }

    static int checkStyle(int style) {
        return checkBits(style, SWT.HORIZONTAL, SWT.VERTICAL, 0, 0, 0, 0);
    }

    void createHandle_pp(int index) {
        state |= WidgetState.HANDLE;
        topHandle = handle = OS.QSlider_new();

        if ((style & SWT.VERTICAL) != 0) {
            OS.QSlider_setOrientation(handle, OS.QT_VERTICAL);
        } else {
            OS.QSlider_setOrientation(handle, OS.QT_HORIZONTAL);
        }
    }

    void hookEvents_pp() {
        int signalProxy = OS.SignalHandler_new(handle, 
                OS.QSIGNAL_SLIDER_CHANGED);
        OS.QObject_connectOrThrow(handle, "valueChanged(int)", signalProxy,
                "widgetSignal(int)", OS.QT_AUTOCONNECTION);
    }

    /**
     * Returns the amount that the receiver's value will be modified by when the
     * up/down (or right/left) arrows are pressed.
     * 
     * @return the increment
     * 
     * @exception SWTException <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *                </ul>
     */
    public int getIncrement() {
        checkWidget();
        return OS.QSlider_singleStep(handle);
    }

    /**
     * Returns the maximum value which the receiver will allow.
     * 
     * @return the maximum
     * 
     * @exception SWTException <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *                </ul>
     */
    public int getMaximum() {
        checkWidget();
        return OS.QSlider_maximum(handle);
    }

    /**
     * Returns the minimum value which the receiver will allow.
     * 
     * @return the minimum
     * 
     * @exception SWTException <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *                </ul>
     */
    public int getMinimum() {
        checkWidget();
        return OS.QSlider_minimum(handle);
    }

    /**
     * Returns the amount that the receiver's value will be modified by when the
     * page increment/decrement areas are selected.
     * 
     * @return the page increment
     * 
     * @exception SWTException <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *                </ul>
     */
    public int getPageIncrement() {
        checkWidget();
        return OS.QSlider_pageStep(handle);
    }

    /**
     * Returns the 'selection', which is the receiver's value.
     * 
     * @return the selection
     * 
     * @exception SWTException <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *                </ul>
     */
    public int getSelection() {
        checkWidget();
        return OS.QSlider_value(handle);
    }

    /**
     * Returns the size of the receiver's thumb relative to the difference
     * between its maximum and minimum values.
     * 
     * @return the thumb value
     * 
     * @exception SWTException <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *                </ul>
     */
    public int getThumb() {
        checkWidget();
        return thumb;
    }

    /**
     * Removes the listener from the collection of listeners who will be
     * notified when the user changes the receiver's value.
     * 
     * @param listener the listener which should no longer be notified
     * 
     * @exception IllegalArgumentException <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     *                </ul>
     * @exception SWTException <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *                </ul>
     * 
     * @see SelectionListener
     * @see #addSelectionListener
     */
    public void removeSelectionListener(SelectionListener listener) {
        checkWidget();
        if (listener == null) error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook(SWT.Selection, listener);
        eventTable.unhook(SWT.DefaultSelection, listener);
    }

    /**
     * Sets the amount that the receiver's value will be modified by when the
     * up/down (or right/left) arrows are pressed to the argument, which must be
     * at least one.
     * 
     * @param value the new increment (must be greater than zero)
     * 
     * @exception SWTException <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *                </ul>
     */
    public void setIncrement(int value) {
        checkWidget();
        if (value < 1) {
            return;
        }
        OS.QSlider_setSingleStep(handle, value);
    }

    /**
     * Sets the maximum. If this value is negative or less than or equal to the
     * minimum, the value is ignored. If necessary, first the thumb and then the
     * selection are adjusted to fit within the new range.
     * 
     * @param value the new maximum, which must be greater than the current
     *              minimum
     * 
     * @exception SWTException <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *                </ul>
     */
    public void setMaximum(int value) {
        checkWidget();
        int minimum = OS.QSlider_minimum(handle);

        if (value >= 0 && value > minimum) {
            thumb = Math.min(thumb, value - minimum);
            setValue(Math.min(OS.QSlider_value(handle), value - thumb));
            OS.QSlider_setMaximum(handle, value);
        }
    }

    /**
     * Sets the minimum value. If this value is negative or greater than or
     * equal to the maximum, the value is ignored. If necessary, first the thumb
     * and then the selection are adjusted to fit within the new range.
     * 
     * @param value the new minimum
     * 
     * @exception SWTException <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *                </ul>
     */
    public void setMinimum(int value) {
        checkWidget();
        if (value < 0) {
            return;
        }

        int maximum = OS.QSlider_maximum(handle);
        if (value >= maximum) {
            return;
        }
        this.thumb = Math.min(thumb, maximum - value);
        setValue(Math.max(OS.QSlider_value(handle), value));
        OS.QSlider_setMinimum(handle, value);
    }

    /**
     * Sets the amount that the receiver's value will be modified by when the
     * page increment/decrement areas are selected to the argument, which must
     * be at least one.
     * 
     * @param value the page increment (must be greater than zero)
     * 
     * @exception SWTException <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *                </ul>
     */
    public void setPageIncrement(int value) {
        checkWidget();
        if (value < 1) {
            return;
        }
        OS.QSlider_setPageStep(handle, value);
    }

    /**
     * Sets the 'selection', which is the receiver's value, to the argument
     * which must be greater than or equal to zero.
     * 
     * @param value the new selection (must be zero or greater)
     * 
     * @exception SWTException <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *                </ul>
     */
    public void setSelection(int value) {
        checkWidget();
        // Adjust the selection to range [minimum, maximum-thumb]
        setValue(Math.max(Math.min(value, OS.QSlider_maximum(handle) - thumb),
                OS.QSlider_minimum(handle)));
    }

    /**
     * Sets the size of the receiver's thumb relative to the difference between
     * its maximum and minimum values. This new value will be ignored if it is
     * less than one, and will be clamped if it exceeds the receiver's current
     * range.
     * 
     * @param value the new thumb value, which must be at least one and not larger
     *              than the size of the current range
     * 
     * @exception SWTException <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *                </ul>
     */
    public void setThumb(int value) {
        checkWidget();
        if (value < 1) {
            return;
        }

        int selection = OS.QSlider_value(handle);
        int maximum = OS.QSlider_maximum(handle);
        value = Math.min(value, maximum - OS.QSlider_minimum(handle));

        this.thumb = value;
        setValue(Math.min(selection, maximum - value));
    }

    /**
     * Sets the receiver's selection, minimum value, maximum value, thumb,
     * increment and page increment all at once.
     * <p>
     * Note: This is similar to setting the values individually using the
     * appropriate methods, but may be implemented in a more efficient fashion
     * on some platforms.
     * </p>
     * 
     * @param selection the new selection value
     * @param minimum the new minimum value
     * @param maximum the new maximum value
     * @param thumb the new thumb value
     * @param increment the new increment value
     * @param pageIncrement the new pageIncrement value
     * 
     * @exception SWTException <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     *                </ul>
     */
    public void setValues(int selection, int minimum, int maximum, int thumb,
            int increment, int pageIncrement) {
        checkWidget();
        if (minimum < 0) return;
        if (maximum < 0) return;
        if (thumb < 1) return;
        if (increment < 1) return;
        if (pageIncrement < 1) return;
        thumb = Math.min(thumb, maximum - minimum);
        this.thumb = thumb;
        OS.QSlider_setMaximum(handle, maximum);
        OS.QSlider_setMinimum(handle, minimum);
        OS.QSlider_setSingleStep(handle, increment);
        OS.QSlider_setPageStep(handle, pageIncrement);
        setValue(Math.min(Math.max(selection, minimum), maximum - thumb));
    }

    void setValue(int value) {
        this.value = value;
        OS.QSlider_setValue(handle, value);
    }

    boolean qt_event_mouseButtonRelease_pp(int widgetHandle, int button, 
            int x, int y, int state, int buttons) {
        this.selectionCause = SWT.NONE;

        /**
         * If user was dragging (i.e. we have sent drag events), send a drag end
         * when mouse button is released
         */
        if (this.dragOccurred) {
            this.dragOccurred = false;
            sendSliderSelectionEvent(this.selectionCause);
        }

        return super.qt_event_mouseButtonRelease_pp(widgetHandle, button, x,
                y, state, buttons);
    }

    boolean qt_event_mouseMove(int widgetHandle, int button, int x,
            int y, int state, int buttons) {
        this.selectionCause = SWT.DRAG;
        return super
                .qt_event_mouseMove(widgetHandle, button, x, y, state, buttons);
    }

    boolean qt_event_keypress_pp(int widgetHandle, int key, int modifier, int character, int nativeScanCode) {
    
        /**
         * The following keys can cause Slider selection to change, save the key
         * code here so that we can use it later when creating the selection
         * event.
         */
        switch (key) {
            case OS.QT_KEY_LEFT:
            case OS.QT_KEY_RIGHT:
            case OS.QT_KEY_UP:
            case OS.QT_KEY_DOWN:
            case OS.QT_KEY_HOME:
            case OS.QT_KEY_END:
            case OS.QT_KEY_PAGEUP:
            case OS.QT_KEY_PAGEDOWN:
                this.selectionCause = Display.translateKey(key, false);
                break;
            default:
                this.selectionCause = SWT.NONE;
                break;
        }
        
        return super.qt_event_keypress_pp(widgetHandle, key, modifier, character, nativeScanCode);
    }
    
    void setTraversalFlags_pp(int type, int key, int modifier,
            int character) {
        traverseDoit = true;
        traverseCancel = true;

        /**
         * The following keys cause the slider value to change, so those keys
         * cannot be used for traversing by default: - Left/right moves a
         * horizontal slider by one step - Up/down moves a vertical slider by
         * one step - Page up/down moves by one page - Home/end moves to
         * beginning/end of the slider
         */
        switch (key) {
            case OS.QT_KEY_LEFT:
            case OS.QT_KEY_RIGHT:
                if ((style & SWT.HORIZONTAL) != 0) {
                    traverseDoit = false;
                }
                traverseCancel = false;
                break;
            case OS.QT_KEY_UP:
            case OS.QT_KEY_DOWN:
                if ((style & SWT.VERTICAL) != 0) {
                    traverseDoit = false;
                }
                traverseCancel = false;
                break;
            case OS.QT_KEY_PAGEUP:
            case OS.QT_KEY_PAGEDOWN:
            case OS.QT_KEY_HOME:
            case OS.QT_KEY_END:
                traverseDoit = false;
                traverseCancel = false;
                break;
            default:
                break;
        }

    }

    void qt_signal_slider_changed(int value) {
        /**
         * If Slider value has changed without the user calling some Slider API
         * function that causes the value to change, conclude that user
         * interaction has caused the slider value to change and send a
         * selection event.
         */
        if (this.value != value) {
            this.value = value;

            if (this.selectionCause == SWT.DRAG) {
                this.dragOccurred = true;
            }

            sendSliderSelectionEvent(this.selectionCause);
        }
    }

    void sendSliderSelectionEvent(int detail) {
        Event event = new Event();
        event.widget = this;
        event.detail = detail;

        sendEvent(SWT.Selection, event, true);
    }

}
