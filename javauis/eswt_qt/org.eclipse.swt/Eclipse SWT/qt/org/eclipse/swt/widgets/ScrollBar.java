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
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.WidgetState;

/**
 * Instances of this class are selectable user interface objects that represent
 * a range of positive, numeric values.
 * <p>
 * At any given moment, a given scroll bar will have a single 'selection' that
 * is considered to be its value, which is constrained to be within the range of
 * values the scroll bar represents (that is, between its <em>minimum</em> and
 * <em>maximum</em> values).
 * </p>
 * <p>
 * Typically, scroll bars will be made up of five areas:
 * <ol>
 * <li>an arrow button for decrementing the value</li>
 * <li>a page decrement area for decrementing the value by a larger amount</li>
 * <li>a <em>thumb</em> for modifying the value by mouse dragging</li>
 * <li>a page increment area for incrementing the value by a larger amount</li>
 * <li>an arrow button for incrementing the value</li>
 * </ol>
 * Based on their style, scroll bars are either <code>HORIZONTAL</code> (which
 * have a left facing button for decrementing the value and a right facing
 * button for incrementing it) or <code>VERTICAL</code> (which have an upward
 * facing button for decrementing the value and a downward facing buttons for
 * incrementing it).
 * </p>
 * <p>
 * On some platforms, the size of the scroll bar's thumb can be varied relative
 * to the magnitude of the range of values it represents (that is, relative to
 * the difference between its maximum and minimum values). Typically, this is
 * used to indicate some proportional value such as the ratio of the visible
 * area of a document to the total amount of space that it would take to display
 * it. SWT supports setting the thumb size even if the underlying platform does
 * not, but in this case the appearance of the scroll bar will not change.
 * </p>
 * <p>
 * Scroll bars are created by specifying either <code>H_SCROLL</code>,
 * <code>V_SCROLL</code> or both when creating a <code>Scrollable</code>. They
 * are accessed from the <code>Scrollable</code> using
 * <code>getHorizontalBar</code> and <code>getVerticalBar</code>.
 * </p>
 * <p>
 * Note: Scroll bars are not Controls. On some platforms, scroll bars that
 * appear as part of some standard controls such as a text or list have no
 * operating system resources and are not children of the control. For this
 * reason, scroll bars are treated specially. To create a control that looks
 * like a scroll bar but has operating system resources, use <code>Slider</code>
 * .
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
 * @see Slider
 * @see Scrollable
 * @see Scrollable#getHorizontalBar
 * @see Scrollable#getVerticalBar
 */
public class ScrollBar extends Widget {
    Scrollable parent;
    /*
     * In QT, thumb'size and page increment are the same thing. Setting page
     * increment with a value other than thumb's size would ruins the
     * consistency of scrollbar. So, page increment value is only stored in java
     * side and does not affect native scrollbar
     */
    int pageIncrement = 1;
    
    int eventDetail = SWT.NONE;

    ScrollBar(Scrollable parent, int style) {
        super(parent,
                checkBits(style, SWT.HORIZONTAL, SWT.VERTICAL, 0, 0, 0, 0));
        this.parent = parent;
        checkOrientation(parent);
        createWidget(0);
    }

    void createHandle_pp(int index) {
        if (parent.scrollAreaHandle == 0)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);

        if ((style & SWT.H_SCROLL) != 0) {
            handle = OS.QAbstractScrollArea_horizontalScrollBar(parent.scrollAreaHandle);
        } else {
            handle = OS.QAbstractScrollArea_verticalScrollBar(parent.scrollAreaHandle);
        }

        if (handle == 0) {
            SWT.error(SWT.ERROR_UNSPECIFIED);
        }
        topHandle = handle;
        state |= WidgetState.HANDLE;
    }

    void releaseParent_pp() {
        super.releaseParent_pp();

        if (parent.horizontalBar == this) {
            OS.QAbstractScrollArea_setHorizontalScrollBarPolicy(
                    parent.scrollAreaHandle, OS.QT_SCROLLBARALWAYSOFF);
            parent.horizontalBar = null;
        } else if (parent.verticalBar == this) {
            OS.QAbstractScrollArea_setVerticalScrollBarPolicy(
                    parent.scrollAreaHandle, OS.QT_SCROLLBARALWAYSOFF);
            parent.verticalBar = null;
        }
    }

    void destroyWidget() {
        releaseHandle_pp();
        /*
         * no need to delete native scrollbar since ScrollBar does not own
         * native scrollbar
         */
    }

    void releaseWidget_pp() {
        super.releaseWidget_pp();
        parent = null;
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
     * @param listener
     *            the listener which should be notified when the user changes
     *            the receiver's value
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

    /**
     * Removes the listener from the collection of listeners who will be
     * notified when the user changes the receiver's value.
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
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
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
     * Returns the amount that the receiver's value will be modified by when the
     * up/down (or right/left) arrows are pressed.
     * 
     * @return the increment
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public int getIncrement() {
        checkWidget();
        return OS.QAbstractSlider_singleStep(handle);
    }

    /**
     * Returns the maximum value which the receiver will allow.
     * 
     * @return the maximum
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public int getMaximum() {
        checkWidget();
        return OS.QAbstractSlider_maximum(handle)
                + OS.QAbstractSlider_pageStep(handle);
    }

    /**
     * Returns the minimum value which the receiver will allow.
     * 
     * @return the minimum
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public int getMinimum() {
        checkWidget();
        return OS.QAbstractSlider_minimum(handle);
    }

    /**
     * Returns the amount that the receiver's value will be modified by when the
     * page increment/decrement areas are selected.
     * 
     * @return the page increment
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public int getPageIncrement() {
        checkWidget();
        return pageIncrement;
    }

    /**
     * Returns the receiver's parent, which must be a Scrollable.
     * 
     * @return the receiver's parent
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public Scrollable getParent() {
        checkWidget();
        return parent;
    }

    /**
     * Returns the single 'selection' that is the receiver's value.
     * 
     * @return the selection
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public int getSelection() {
        checkWidget();
        return OS.QAbstractSlider_value(handle);
    }

    /**
     * Returns a point describing the receiver's size. The x coordinate of the
     * result is the width of the receiver. The y coordinate of the result is
     * the height of the receiver.
     * 
     * @return the receiver's size
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public Point getSize() {
        checkWidget();
        if(!isVisible()) parent.updateQLayouts();
        return OS.QWidget_frameSize(handle);
    }

    /**
     * Returns the size of the receiver's thumb relative to the difference
     * between its maximum and minimum values.
     * 
     * @return the thumb value
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @see ScrollBar
     */
    public int getThumb() {
        checkWidget();
        return OS.QAbstractSlider_pageStep(handle);
    }

    /**
     * Returns <code>true</code> if the receiver is enabled, and
     * <code>false</code> otherwise. A disabled control is typically not
     * selectable from the user interface and draws with an inactive or "grayed"
     * look.
     * 
     * @return the receiver's enabled state
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @see #isEnabled
     */
    public boolean getEnabled() {
        checkWidget();
        return OS.QWidget_isEnabled(handle);
    }

    /**
     * Returns <code>true</code> if the receiver is visible, and
     * <code>false</code> otherwise.
     * <p>
     * If one of the receiver's ancestors is not visible or some other condition
     * makes the receiver not visible, this method may still indicate that it is
     * considered visible even though it may not actually be showing.
     * </p>
     * 
     * @return the receiver's visibility state
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public boolean getVisible() {
        checkWidget();
        return OS.QWidget_isVisible(handle);
    }

    /**
     * Returns <code>true</code> if the receiver is enabled and all of the
     * receiver's ancestors are enabled, and <code>false</code> otherwise. A
     * disabled control is typically not selectable from the user interface and
     * draws with an inactive or "grayed" look.
     * 
     * @return the receiver's enabled state
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @see #getEnabled
     */
    public boolean isEnabled() {
        checkWidget();
        return OS.QWidget_isEnabled(handle) && parent.isEnabled();
    }

    /**
     * Returns <code>true</code> if the receiver is visible and all of the
     * receiver's ancestors are visible and <code>false</code> otherwise.
     * 
     * @return the receiver's visibility state
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * 
     * @see #getVisible
     */
    public boolean isVisible() {
        checkWidget();
        return OS.QWidget_isVisible(handle) && parent.isVisible();
    }

    /**
     * Enables the receiver if the argument is <code>true</code>, and disables
     * it otherwise. A disabled control is typically not selectable from the
     * user interface and draws with an inactive or "grayed" look.
     * 
     * @param enabled
     *            the new enabled state
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setEnabled(boolean enabled) {
        checkWidget();
        if (enabled) {
            state &= ~WidgetState.DISABLED;
        } else {
            state |= WidgetState.DISABLED;
        }
        OS.QWidget_setEnabled(handle, enabled);
    }

    /**
     * Sets the amount that the receiver's value will be modified by when the
     * up/down (or right/left) arrows are pressed to the argument, which must be
     * at least one.
     * 
     * @param value
     *            the new increment (must be greater than zero)
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setIncrement(int value) {
        checkWidget();
        if (value < 1) {
            return;
        }
        OS.QAbstractSlider_setSingleStep(handle, value);
    }

    /**
     * Sets the maximum. If this value is negative or less than or equal to the
     * minimum, the value is ignored. If necessary, the selection are adjusted
     * to fit within the new range.
     * 
     * @param value
     *            the new maximum
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setMaximum(int value) {
        checkWidget();

        if (value < 0)
            return;
        int min = OS.QAbstractSlider_minimum(handle);
        if (value <= min)
            return;

        int qMaxValue = value - OS.QAbstractSlider_pageStep(handle);
        if (qMaxValue < min) {
            /*
             * thumb is too big to fit between minimum and maximum shrink thumb
             */
            OS.QAbstractSlider_setPageStep(handle, value - min);
            /*
             * adjust maximum value
             */
            qMaxValue = min;
        }

        if (OS.QAbstractSlider_value(handle) > qMaxValue) {
            /*
             * adjust selection
             */
            OS.QAbstractSlider_setValue(handle, qMaxValue);
        }
        OS.QAbstractSlider_setMaximum(handle, qMaxValue);

    }

    /**
     * Sets the minimum value. If this value is negative or greater than or
     * equal to the maximum, the value is ignored. If necessary, first the thumb
     * and then the selection are adjusted to fit within the new range.
     * 
     * @param value
     *            the new minimum
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setMinimum(int value) {
        checkWidget();
        if (value < 0) {
            return;
        }

        int qMax = OS.QAbstractSlider_maximum(handle);
        int pageStep = OS.QAbstractSlider_pageStep(handle);
        if (value >= (qMax + pageStep)) {
            return;
        }

        if ((qMax + pageStep - value) < pageStep) {
            /*
             * thumb is too big to fit between minimum and maximum shrink thumb
             */
            OS.QAbstractSlider_setPageStep(handle, qMax + pageStep - value);
            /*
             * adjust qt maximum value
             */
            OS.QAbstractSlider_setMaximum(handle, value);
        }

        if (OS.QAbstractSlider_value(handle) < value) {
            /*
             * adjust selection
             */
            OS.QAbstractSlider_setValue(handle, value);
        }
        OS.QAbstractSlider_setMinimum(handle, value);
    }

    /**
     * Sets the amount that the receiver's value will be modified by when the
     * page increment/decrement areas are selected to the argument, which must
     * be at least one.
     * 
     * @param value
     *            the page increment (must be greater than zero)
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setPageIncrement(int value) {
        checkWidget();
        if (value < 1) {
            return;
        }
        /*
         * In QT, thumb'size and page increment are the same thing. Setting page
         * increment with a value other than thumb's size would ruins the
         * consistency of scrollbar. So, this API only stores value in java side
         * and does not change anything to native scrollbar
         */
        pageIncrement = value;
    }

    /**
     * Sets the single <em>selection</em> that is the receiver's value to the
     * argument which must be greater than or equal to zero.
     * 
     * @param selection
     *            the new selection (must be zero or greater)
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setSelection(int value) {
        checkWidget();
        int min = OS.QAbstractSlider_minimum(handle);
        if (value < min) {
            value = min;
        } else {
            int max = OS.QAbstractSlider_maximum(handle);
            if (value > max) {
                value = max;
            }
        }
        OS.QAbstractSlider_setValue(handle, value);
    }

    /**
     * Sets the size of the receiver's thumb relative to the difference between
     * its maximum and minimum values. This new value will be ignored if it is
     * less than one, and will be clamped if it exceeds the receiver's current
     * range.
     * 
     * @param value
     *            the new thumb value, which must be at least one and not larger
     *            than the size of the current range
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setThumb(int value) {
        checkWidget();
        if (value < 1) {
            return;
        }
        int pageStep = OS.QAbstractSlider_pageStep(handle);
        if (value == pageStep)
            return;

        int qMax = OS.QAbstractSlider_maximum(handle);
        int range = qMax + pageStep - OS.QAbstractSlider_minimum(handle);
        if (value > range) {
            /*
             * decrease thumb value
             */
            value = range;
        }
        int newQMaximum = qMax - (value - pageStep);
        if (OS.QAbstractSlider_value(handle) > newQMaximum) {
            /*
             * decrease selection value
             */
            OS.QAbstractSlider_setValue(handle, newQMaximum);
        }

        OS.QAbstractSlider_setPageStep(handle, value);

        /*
         * adjust maximum value
         */
        OS.QAbstractSlider_setMaximum(handle, newQMaximum);

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
     * @param selection
     *            the new selection value
     * @param minimum
     *            the new minimum value
     * @param maximum
     *            the new maximum value
     * @param thumb
     *            the new thumb value
     * @param increment
     *            the new increment value
     * @param pageIncrement
     *            the new pageIncrement value
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setValues(int selection, int minimum, int maximum, int thumb,
            int increment, int pageIncrement) {
        checkWidget();
        if (minimum >= 0) {
            OS.QAbstractSlider_setMinimum(handle, minimum);
        } else {
            minimum = OS.QAbstractSlider_minimum(handle);
        }

        int oldThumb = OS.QAbstractSlider_pageStep(handle);
        boolean validMaximum = true;
        if (maximum <= minimum) {
            validMaximum = false;
            maximum = OS.QAbstractSlider_maximum(handle) + oldThumb;
        }

        if (thumb > 0) {
            if (thumb > maximum - minimum) {
                /*
                 * decrease thumb size
                 */
                thumb = maximum - minimum;
            }
            OS.QAbstractSlider_setPageStep(handle, thumb);
        } else if (oldThumb > maximum - minimum) {
            /*
             * Thumb value itself is ignored. But Thumb needs to be adjusted to
             * fit maximum and minimum values
             */
            thumb = maximum - minimum;
            OS.QAbstractSlider_setPageStep(handle, thumb);
        } else {
            /*
             * Thumb value is ignored and current thumb size keeps unchanged
             */
            thumb = oldThumb;
        }

        if (validMaximum) {
            OS.QAbstractSlider_setMaximum(handle, maximum - thumb);
        }

        if (selection < minimum) {
            /*
             * increase selection
             */
            selection = minimum;

        } else if (selection > maximum - thumb) {
            /*
             * decrease selection
             */
            selection = maximum - thumb;
        }

        OS.QAbstractSlider_setValue(handle, selection);

        if (increment > 0) {
            OS.QAbstractSlider_setSingleStep(handle, increment);
        }

        if (pageIncrement > 0) {
            this.pageIncrement = pageIncrement;
        }
    }

    /**
     * Marks the receiver as visible if the argument is <code>true</code>, and
     * marks it invisible otherwise.
     * <p>
     * If one of the receiver's ancestors is not visible or some other condition
     * makes the receiver not visible, marking it visible may not actually cause
     * it to be displayed.
     * </p>
     * 
     * @param visible
     *            the new visibility state
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setVisible(boolean visible) {
        checkWidget();
        OS.QWidget_setVisible(handle, visible);
    }

    void hookEvents_pp() {
        super.hookEvents_pp();
        int signalProxy = OS.SignalHandler_new(handle, 
                OS.QSIGNAL_SLIDER_CHANGED);
        OS.QObject_connectOrThrow(handle, "valueChanged(int)", signalProxy,
                "widgetSignal(int)", OS.QT_AUTOCONNECTION);
        signalProxy = OS.SignalHandler_new(handle, 
                OS.QSIGNAL_SLIDER_ACTION_TRIGGERED);
        OS.QObject_connectOrThrow(handle, "actionTriggered(int)", signalProxy,
                "widgetSignal(int)", OS.QT_AUTOCONNECTION);
        signalProxy = OS.SignalHandler_new(handle, 
                OS.QSIGNAL_SLIDER_RELEASED);
        OS.QObject_connectOrThrow(handle, "sliderReleased()", signalProxy,
                "widgetSignal()", OS.QT_AUTOCONNECTION);
    }

    void qt_signal_slider_action_triggered(int action) {
        switch (action) {
        case OS.QT_SLIDERMOVE:
            eventDetail = SWT.DRAG;
            break;
        case OS.QT_SLIDERTOMINIMUM:
            eventDetail = SWT.HOME;
            break;
        case OS.QT_SLIDERTOMAXIMUM:
            eventDetail = SWT.END;
            break;
        case OS.QT_SLIDERSINGLESTEPADD:
            eventDetail = SWT.ARROW_DOWN;
            break;
        case OS.QT_SLIDERSINGLESTEPSUB:
            eventDetail = SWT.ARROW_UP;
            break;
        case OS.QT_SLIDERPAGESTEPUSB:
            eventDetail = SWT.PAGE_UP;
            break;
        case OS.QT_SLIDERPAGESTEPADD:
            eventDetail = SWT.PAGE_DOWN;
            break;
        default:
            eventDetail = SWT.NONE;
        }
    }

    void qt_signal_slider_changed(int value) {
        sendSelectionEvent(eventDetail);
        eventDetail = SWT.NONE;
    }
   
    void qt_signal_slider_released() {
        sendSelectionEvent(SWT.NONE);
    }

    void sendSelectionEvent(int detail) {
        Event event = new Event();
        event.widget = this;
        event.detail = detail;
        sendEvent(SWT.Selection, event, true);
    }

}
