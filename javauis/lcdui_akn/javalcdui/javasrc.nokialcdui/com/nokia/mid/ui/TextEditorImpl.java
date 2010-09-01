/* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description: Package private class implementing the S60 interfaces
 * for TextEditor
 *
 */

// PACKAGE
package com.nokia.mid.ui;

// INTERNAL IMPORTS
import com.nokia.mid.ui.impl.LCDUIPackageInvoker;
import com.nokia.mid.ui.impl.TextEditorListenerImpl;
import com.nokia.mid.ui.impl.TextEditorContainer;

// EXTERNAL IMPORTS
import com.nokia.mj.impl.rt.legacy.NativeError;
import com.nokia.mj.impl.rt.legacy.ToolkitInvoker;
import javax.microedition.lcdui.Canvas;

/*
 *
 *
 * @since 1.4
 */
class TextEditorImpl
        extends com.nokia.mid.ui.TextEditor
        implements com.nokia.mid.ui.S60TextEditor
{
    /*
     * Indicates that all input methods are available.
     * <p>
     * Can be used in {@link #setDisabledTouchInputModes}.
     */
    public static final int TOUCH_INPUT_ALL_AVAILABLE   = 0;

    /*
     *  Handwriting recognition
     */
    public static final int TOUCH_INPUT_HWR             = 1;

    /*
     *  Virtual QWERTY keyboard
     */
    public static final int TOUCH_INPUT_VKB             = 2;

    /*
     *  Full screen QWERTY keyboard
     */
    public static final int TOUCH_INPUT_FSQ             = 4;

    /*
     *  Alphanumeric keyboard with ITU-T input
     */
    public static final int TOUCH_INPUT_ITUT            = 8;

    /*
     *  Full screen handwriting recognition
     */
    public static final int TOUCH_INPUT_FSC             = 16;

    /*
     *  Mini ITU-T for Japanese devices
     */
    public static final int TOUCH_INPUT_MINI_ITUT       = 32;

    // Private static data.
    private static final int INDICATOR_SIZE_COUNT       = 2;

    // Mask of all bits defined
    private static final int MASK =
        TOUCH_INPUT_HWR | TOUCH_INPUT_VKB | TOUCH_INPUT_FSQ |
        TOUCH_INPUT_ITUT | TOUCH_INPUT_FSC | TOUCH_INPUT_MINI_ITUT;

    // Indicates receiving of pointer events by the editor
    private boolean iTouchEnabled;

    /*
     * <P>
     * Creates a new <code>TextEditor</code> object with the given initial contents,
     * maximum size in characters, constraints and editor size in pixels.
     * </P>
     *
     * <P>
     * If the text parameter is <code>null</code>, the <code>TextEditor</code> is
     * created empty.
     * </P>
     *
     * <P>
     * The <code>maxSize</code> parameter must be greater than zero. An
     * <code>IllegalArgumentException</code> is thrown if the length of the initial
     * contents string exceeds <code>maxSize</code>. However, the implementation
     * may assign a maximum size smaller than the application had requested. If
     * this occurs, and if the length of the contents exceeds the newly assigned
     * maximum size, the contents are truncated from the end in order to fit,
     * and no exception is thrown.
     * </P>
     *
     * @param aText
     *            the initial contents, or <code>null</code> if the
     *            <code>TextEditor</code> is to be created empty
     * @param aMaxSize
     *            the maximum capacity in characters
     * @param aConstraints
     *            see the input constraints in <code>TextField</code></a>
     * @param aWidth
     *            the width of the bounding box, in pixels
     * @param aHeight
     *            the height of the bounding box, in pixels
     *
     * @throws IllegalArgumentException
     *             if <code>aMaxSize</code> is zero or less
     * @throws IllegalArgumentException
     *             if the value of the constraints parameter is invalid
     * @throws IllegalArgumentException
     *             if <code>aText</code> is illegal for the specified
     *             constraints
     * @throws IllegalArgumentException
     *             if the length of the string exceeds the requested maximum
     *             capacity
     * @throws IllegalArgumentException
     *             if the width or height is less than one pixel
     */
    TextEditorImpl(String aText, int aMaxSize, int aConstraints, int aWidth,
                   int aHeight)
    {
        super(aText, aMaxSize, aConstraints, aWidth, aHeight, false);
        // Enabling receiving pointer events
        iTouchEnabled = true;
    }

    /*
     * <P>
     * Creates a new empty <code>TextEditor</code> with the given maximum size in
     * characters, constraints and editor size as number of visible rows.
     * </P>
     *
     * <P>
     * The <code>rows</code> parameter indicates the requested number of visible
     * rows in the editor. TextEditor then assigns the initial
     * height for the editor based on the requested number of rows and width of the
     * editor.
     * </P>
     *
     * <P>
     * The <code>maxSize</code> parameter must be greater than zero. An
     * <code>IllegalArgumentException</code> is thrown if the length of the initial
     * contents string exceeds <code>maxSize</code>. However, the implementation
     * may assign a maximum size smaller than the application had requested. If
     * this occurs, and if the length of the contents exceeds the newly assigned
     * maximum size, the contents are truncated from the end in order to fit,
     * and no exception is thrown.
     * </P>
     *
     * @param aMaxSize
     *            the maximum capacity in characters
     * @param aConstraints
     *            see the input constraints in <code>TextField</code></a>
     * @param aWidth
     *            the width of the bounding box, in pixels
     * @param aRows
     *            the requested number of visible rows in the editor
     *
     * @throws IllegalArgumentException
     *             if <code>aMaxSize</code> or <code>rows</code> is zero or less
     * @throws IllegalArgumentException
     *             if the value of the constraints parameter is invalid
     * @throws IllegalArgumentException
     *             if the width is less than one pixel
     */
    TextEditorImpl(int aMaxSize, int aConstraints, int aWidth, int aRows)
    {
        super(null, aMaxSize, aConstraints, aWidth, aRows, true);
        // Enabling receiving pointer events
        iTouchEnabled = true;
    }

    /*
     * Set the parent object of this TextEditor. Typically in LCDUI the parent
     * object would be Canvas or CustomItem. Setting the parameter to null
     * removes the association to the parent.
     * <p>
     * @param aParent the parent object
     * <p>
     * @throws IllegalArgumentException
     *              if <code>aParent</code> is not a valid object with which a
     *              TextEditor can be associated
     */
    public void setParent(Object aParent)
    {
        super.setParent(aParent);
        // Update indicator location to their default position.
        if (aParent != null)
        {
            setDefaultIndicators();
        }
    }

    /*
     * <P>
     * If the default indicator location is not used then sets the drawing
     * location for input indicators relative to the <code>TextEditor</code>'s
     * parent.
     * </P>
     *
     * <P>
     * The anchor point given is relative to the upper left corner of the
     * <code>Canvas</code>. The location may be outside the
     * <code>TextEditor</code> boundaries. The z-order of the indicators is the
     * same as <code>TextEditor</code> textual content. If indicators and the
     * editor content overlap indicators are rendered on top of the editor
     * content.
     * </P>
     *
     * <P>
     * The application should first query the size of the input indicators with
     * {@link #getIndicatorSize} method to determine that the indicators will
     * not be clipped outside the available display area when drawn to the
     * requested location.
     * </P>
     *
     * <P>
     * If there are no indicators present, the usability of complex device
     * specific input methods may be compromised.
     * </P>
     *
     * @param x
     *            the x coordinate of the anchor point, in pixels.
     * @param y
     *            the y coordinate of the anchor point, in pixels.
     *
     */
    public void setIndicatorLocation(int x, int y)
    {
        synchronized (iToolkit)
        {
            NativeError.check(_setIndicatorLocation(getToolkitHandle(),
                                                    iHandle, x, y));
        }
    }

    /*
     * <P>
     * Resets the implementation provided input indicators to their default
     * position.
     * </P>
     *
     * <P>
     * This position may be outside the area of parent in case the default
     * position is in status area. In this case if the status area is missing
     * (full screen mode Canvas) the default position is inside the parent area
     * e.g. on top of the editor. When default position is in use the
     * <code>TextEditor</code> automatically positions the indicators relative
     * to the <code>TextEditor</code> even when <code>TextEditor</code> location
     * is changed. However, client is responsible of making sure indicators are
     * visible inside the parent area positioning <code>TextEditor</code> so
     * that indicators fit to the visible area e.g. on top of the
     * <code>TextEditor</code>. Positioning <code>TextEditor</code> directly on
     * top of <code>Canvas</code> may mean that indicators in the default
     * position are not visible.
     * </P>
     *
     * @throws IllegalStateException
     *             If the <code>TextEditor</code> is not added to
     *             <code>Canvas</code>
     */
    public void setDefaultIndicators()
    {
        synchronized (iToolkit)
        {
            Object parent = getParent();
            boolean fullScreen = false;

            if (parent instanceof Canvas)
            {
                fullScreen = iLCDUIPackageInvoker.isFullScreen((Canvas) parent);
            }

            NativeError.check(_setDefaultIndicators(getToolkitHandle(),
                                                    iHandle, fullScreen));
        }
    }

    /*
     * <P>
     * By default indicators visibility is set to <code>true</code> and they are
     * made visible when the associated <code>TextEditor</code> is focused.
     * </P>
     * <P>
     * If the application controls the position of the indicators, those can be
     * explicitly made not visible by calling
     * <code>setIndicatorVisibility(false)</code>. Indicators are never visible
     * if <code>TextEditor</code> itself is not visible so having indicator
     * visibility set to <code>true</code> does not make them visible unless
     * editor itself is set visible.
     * </P>
     *
     * @param visible
     *            controls indicator visibility state
     *
     * @see #setIndicatorLocation(int, int)
     * @see #setDefaultIndicators()
     */
    public void setIndicatorVisibility(boolean visible)
    {
        synchronized (iToolkit)
        {
            NativeError.check(_setIndicatorVisibility(getToolkitHandle(),
                              iHandle, visible));
        }
    }

    /*
     * Gets the size of the area needed for drawing the input indicators.
     * <p>
     * The returned array contains two integers for width (array index 0) and
     * height (array index 1) of the indicator graphics. Size (0,0) is returned
     * if the device UI does not use any input indicators in the text editors or
     * if the indicators are curretly positioned outside parent area e.g. in
     * status area. This happens if {@link #setIndicatorLocation(int, int)
     * setIndicatorLocation(int x, int y)} has not been ever called for the
     * editor, or if {@link #setDefaultIndicators() setDefaultIndicators()} has
     * been called.
     * <p>
     *
     * @return the width and height of area needed for drawing input indicators
     * @see #setDefaultIndicators()
     */
    public int[] getIndicatorSize()
    {
        int[] size = new int[INDICATOR_SIZE_COUNT];

        synchronized (iToolkit)
        {
            NativeError.check(_getIndicatorSize(getToolkitHandle(), iHandle,
                                                size));
        }
        return size;
    }

    /*
     * Disables one or multiple touch input modes from use.
     * <p>
     * User is not able to switch to the disable touch input modes. Multiple
     * touch input modes may be combined together in <code>touchInputMode</code>
     * parameter using bitwise or operator. The method does not have any impact
     * if called in non-touch device. A device may not support all touch input
     * modes specified. Specifying not supported input modes is silently
     * ignored.
     * <p>
     * The possible values are defined in the class with TOUCH_INPUT_* starting
     * constant values.
     * <p>
     *
     * @param touchInputModes
     *            bitwise or combined list of disabled touch input modes
     * @throws IllegalArgumentException
     *             if the given input modes are not valid.
     *             <p>
     * @see #setPreferredTouchMode(int)
     * @see #getDisabledTouchInputModes()
     */
    public void setDisabledTouchInputModes(int touchInputModes)
    {
        // Validate touch input modes. There must not be additional modes
        // 0 is valid.
        // See com.nokia.mid.ui.s60.TextEditor.TOUCH_INPUT_ALL_AVAILABLE.
        if ((touchInputModes & ~MASK) != 0)
        {
            throw new IllegalArgumentException();
        }

        synchronized (iToolkit)
        {
            NativeError.check(_setDisabledTouchInputModes(getToolkitHandle(),
                              iHandle, touchInputModes));
        }
    }

    /*
     * By default all supported touch input modes are available. Returns the
     * disabled touch input modes set with {@link #setDisabledTouchInputModes}.
     * <p>
     * Note that the disabled touch input modes may be device specific so this
     * method may return some modes as disabled by default.
     * <p>
     * Note that if the device does not support touch input this method returns
     * all modes.
     * <p>
     *
     * @return The disabled touch input modes.
     * @see #setDisabledTouchInputModes(int)
     * @see #setPreferredTouchMode(int)
     */
    public int getDisabledTouchInputModes()
    {
        int disabled = 0;

        synchronized (iToolkit)
        {
            disabled = _getDisabledTouchInputModes(getToolkitHandle(), iHandle);
        }

        NativeError.check(disabled);
        return disabled;
    }

    /*
     * Set the preferred touch input mode overriding the device default
     * preferred mode. User may still change the touch input mode from touch
     * input window to all available modes.
     * <p>
     * The possible values are defined in the class with TOUCH_INPUT_* starting
     * constant values.
     * <p>
     * Note that if the device does not support touch input this method has no
     * effect.
     * <p>
     *
     * @param touchInputModes
     *            a touch input mode to be set as preferred one.
     * @throws IllegalArgumentException
     *             if the input mode is not valid or if it contains multiple
     *             input modes as bit mask.
     * @see #setDisabledTouchInputModes(int)
     * @see #getPreferredTouchMode()
     *
     */
    public void setPreferredTouchMode(int touchInputModes)
    {
        int mask = touchInputModes & MASK;
        // Validate touch input mode.
        if (touchInputModes == 0 || (touchInputModes & ~MASK) != 0
                || (mask & (mask - 1)) != 0)
        {
            throw new IllegalArgumentException();
        }

        synchronized (iToolkit)
        {
            NativeError.check(_setPreferredTouchMode(getToolkitHandle(),
                              iHandle, touchInputModes));
        }
    }

    /*
     * Gets the preferred touch input mode.
     * <p>
     * Note that if the device does not support touch input this method returns
     * <code>0</code>
     * <p>
     *
     * @return The preferred touch input mode.
     *         <p>
     * @see #setPreferredTouchMode(int)
     * @see #setDisabledTouchInputModes(int)
     */
    public int getPreferredTouchMode()
    {
        int preferredMode = 0;

        synchronized (iToolkit)
        {
            preferredMode = _getPreferredTouchMode(getToolkitHandle(), iHandle);
        }

        NativeError.check(preferredMode);
        return preferredMode;
    }

    /*
     * Sets the caret in the Editor at x, y location.
     *
     * @param x
     *      The x coordinate of the wanted caret position.
     *
     * @param y
     *      The y coordinate of the wanted caret position.
     */
    public void setCaretXY(int x, int y)
    {
        synchronized (iToolkit)
        {
            NativeError.check(_setCaretXY(getToolkitHandle(),
                                          iHandle, x, y));
        }
    }

    /*
     * Specifies whether or not the editor will receive touch-events.
     * <p>
     * This is enabled by default.
     * An editor with touch-event disabled won't be able to perform any
     * touch-related functionality such as scrolling or positioning the
     * cursor. It may however still be controlled via the
     * virtual keypad/control-panel if that is enabled, or receive other +
     * input e.g. via physical keys
     * <p>
     * @param enabled
     *              true to enabled touch-event, false to disable
     */
    public void setTouchEnabled(boolean enabled)
    {
        if (iTouchEnabled != enabled)
        {
            synchronized (iToolkit)
            {
                _setTouchEnabled(getToolkitHandle(), iHandle, enabled);
                iTouchEnabled = enabled;
            }
        }
    }

    /*
     * Gets the current touch-enabled state
     * <p>
     * @return true if the editor is touch-enabled, false otherwise
     */
    public boolean isTouchEnabled()
    {
        return iTouchEnabled;
    }

    /*
     * Hidden default constructor.
     */
    private TextEditorImpl()
    {
    }

    // Private methods.

    /*
     * Sets the visibility status of the indicator of the editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aVisible The visibility status of the indicator.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setIndicatorVisibility(
        int aToolkitHandle,
        int aNativePeerHandle,
        boolean aVisible);

    /*
     * Sets the position of the indicator.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aX The x coordinate of the anchor point.
     *
     * @param aY The y coordinate of the anchor point.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setIndicatorLocation(
        int aToolkitHandle,
        int aNativePeerHandle,
        int aX,
        int aY);

    /*
     * Gets the size of the area needed for drawing the input indicators.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aSize On return, contains the size of the indicator (w,h).
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _getIndicatorSize(
        int aToolkitHandle,
        int aNativePeerHandle,
        int[] aSize);

    /*
     * Resets the default input indicators to be used.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aParentFullScreen Indicates if parent is the size of the whole
     * screen.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setDefaultIndicators(
        int aToolkitHandle,
        int aNativePeerHandle,
        boolean aParentFullScreen);

    /*
     * Sets the disabled touch input modes.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aTouchInputModes The disabled touch input modes.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setDisabledTouchInputModes(
        int aToolkitHandle,
        int aNativePeerHandle,
        int aTouchInputModes);

    /*
     * Gets the disabled touch input mode.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aTouchInputMode The preferred touch input mode.
     *
     * @return The disabled touch input modes if the operation was successful.
     * Otherwise, a system-wide error code is returned.
     */
    private native int _getDisabledTouchInputModes(
        int aToolkitHandle,
        int aNativePeerHandle);

    /*
     * Sets the preferred touch input mode.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aTouchInputMode The preferred touch input mode.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setPreferredTouchMode(
        int aToolkitHandle,
        int aNativePeerHandle,
        int aTouchInputMode);

    /*
     * Gets the preferred touch input mode.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @return The preferred touch input mode if the operation was successful.
     * Otherwise, a system-wide error code is returned.
     */
    private native int _getPreferredTouchMode(
        int aToolkitHandle,
        int aNativePeerHandle);

    /*
     * Sets the caret in the Editor at x, y location.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param x
     *      The x coordinate of the wanted caret position.
     *
     * @param y
     *      The y coordinate of the wanted caret position.
     */
    public native int _setCaretXY(
        int aToolkitHandle,
        int aNativePeerHandle,
        int x,
        int y);

    /*
     * Specifies whether or not the editor will receive touch-events.
     *
     * This is enabled by default.
     * An editor with touch-event disabled won't be able to perform any
     * touch-related functionality such as scrolling or positioning the
     * cursor. It may however still be controlled via the
     * virtual keypad/control-panel if that is enabled, or receive other +
     * input e.g. via physical keys
     *
     * @param aEnabled
     *              true to enabled touch-event, false to disable
     */
    private native int _setTouchEnabled(int aToolkitHandle,
                                        int aNativePeerHandle, boolean aEnabled);
}

// End of file
