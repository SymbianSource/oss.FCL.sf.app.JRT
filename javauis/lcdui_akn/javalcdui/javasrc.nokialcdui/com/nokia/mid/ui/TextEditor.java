/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Public class defining the interface for MIDP Text Editor
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
import com.nokia.mid.ui.CanvasItem;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.CustomItem;
import javax.microedition.lcdui.TextField;
import java.util.Enumeration;
import com.nokia.mj.impl.rt.support.Finalizer;

/**
 * <P>
 * A <code>TextEditor</code> is an editable text component that is used with a
 * parent object; with <code>Canvas</code> or <code>CustomItem</code>. The
 * <code>TextEditor</code> maybe set to and removed from <code>Canvas</code>
 * through <code>setParent()</code> method.
 * </P>
 *
 * <P>
 * The implementation presents the <code>TextEditor</code> with minimal
 * decoration; only the caret is shown at the requested text insertion position.
 * It is the responsibility of the application to draw any additional decoration
 * like focus highlight, border or scroll bar. The animation of the caret, e.g.
 * blinking, is handled by the TextEditor implementation.
 * </P>
 *
 * <P>
 * <code>TextEditor</code> supports <em>input constraints</em> identically to
 * <code>TextField</code>. See <a href="TextField.html#constraints">input
 * constraints</a> section in the <code>TextField</code> for the definition of
 * these constants. In addition <code>TextEditor</code> has the same concept of
 * <em>actual contents</em> and <em>displayed contents</em> as
 * <code>TextField</code>; these are described in the same <a
 * href="TextField.html#constraints">input constraints</a> section.
 * </P>
 *
 * <P>
 * <code>TextEditor</code> supports <em>input modes</em> identically to
 * <code>TextField</code>. See <a href="TextField.html#modes">input modes</a>
 * section in the <code>TextField</code> for the definition of these constants.
 * </P>
 *
 * <P>
 * The text may contain <A HREF="Form.html#linebreak">line breaks</A>. The
 * display of the text must break accordingly and the user must be able to enter
 * line break characters.
 * </P>
 *
 * <P>
 * The TextEditor provides necessary interaction for example for touch input
 * (handwriting recognition) on touch screen devices and selection of additional
 * characters. The input on touch devices requires additional windows of top of
 * the editor window which depending on the implementation may obscure the
 * parent completely.
 * </P>
 *
 * <P>
 * Application can add a TextEditorListener to the <code>TextEditor</code> for
 * example for keeping track of user navigation (caret movement) and other
 * content changes such as text selection. The events are sent on all occasions
 * that cause the the caret position to change (including but not limited to
 * text typing by user, programmatic text insertion, navigation within the
 * <code>TextEditor</code> content, and text deletion). The events must be sent
 * to the application prior to they have effect on the editor - for example an
 * event indicating caret movement must be available for the application before
 * the implementation actually moves the caret in the editor.
 * </P>
 *
 * <P>
 * When the <code>TextEditor</code> has focus all the key events that are not
 * consumed by <code>TextEditor</code> with the current constraints or mapped to
 * Commands by the implementation are sent to <code>Canvas</code>.
 * </P>
 *
 * <P>
 * Touch pointer events are not delivered to parent object from the area of the
 * visible TextEditor.
 * </P>
 *
 * <P>
 * The implementation will scroll the <code>TextEditor</code> content
 * automatically on user interaction and when the application calls setCaret or
 * setSelection methods. For example if a user clicks down on the last visible
 * row the <code>TextEditor</code> content is scrolled accordingly by one row.
 * However the Java platform implementation does not draw any scroll bars, this
 * is left to the application.
 * </P>
 *
 * @since 1.4
 */

public class TextEditor
        extends CanvasItem
{

    // Protected data for extending classes.

    // LCDUI Toolkit object.
    Object iToolkit;

    // Native handle
    int iHandle;

    // LCDUI package invoker.
    LCDUIPackageInvoker iLCDUIPackageInvoker;

    // Change listener. May be NULL.
    TextEditorListenerImpl iListener;

    // Private data

    // LCDUI Toolkit invoker object.
    private ToolkitInvoker iToolkitInvoker;
    // The current set of constraints of this text editor component.
    private int iConstraints;

    // Initial input mode
    private java.lang.String iCharacterSubset;

    // The current maximum size of this text editor component.
    private int iMaxSize;

    // The current font of this text editor component
    private Font iFont;

    // Indicates receiving of pointer events by the editor
    private boolean iTouchEnabled;

    // Text editor container for handling editor focusing.
    private static TextEditorContainer iEditorContainer;

    // Color indicators. Must be in sync with MMIDTextEditor::TColorType
    private static final int COLOR_BACKGROUND = 0;
    private static final int COLOR_FOREGROUND = 1;
    private static final int COLOR_HIGHLIGHT_BACKGROUND = 2;
    private static final int COLOR_HIGHLIGHT_FOREGROUND = 3;

    // The maximum value for all colors.
    private static final int COLOR_MAX_VALUE = 255;

    // Static block for creating editor container.
    static
    {
        iEditorContainer = new TextEditorContainer();
    }

    private Finalizer mFinalizer;

    /**
     * <P>
     * Creates a new <code>TextEditor</code> object with the given initial
     * contents, maximum size in characters, constraints and editor size in
     * pixels.
     * </P>
     *
     * <P>
     * If the text parameter is <code>null</code>, the <code>TextEditor</code>
     * is created empty.
     * </P>
     *
     * <P>
     * The <code>maxSize</code> parameter must be greater than zero. An
     * <code>IllegalArgumentException</code> is thrown if the length of the
     * initial contents string exceeds <code>maxSize</code>. However, the
     * implementation may assign a maximum size smaller than the application had
     * requested. If this occurs, and if the length of the contents exceeds the
     * newly assigned maximum size, the contents are truncated from the end in
     * order to fit, and no exception is thrown.
     * </P>
     *
     * <P>
     * On Series60-devices, if the MIDlet is manufacturer or operator-signed,
     * the object returned by this method will also implement the
     * com.nokia.mid.ui.s60.TextEditor
     * </P>
     *
     * @param text
     *            the initial contents, or <code>null</code> if the
     *            <code>TextEditor</code> is to be created empty
     * @param maxSize
     *            the maximum capacity in characters
     * @param constraints
     *            see the input constraints in <code>TextField</code></a>
     * @param width
     *            the width of the bounding box, in pixels
     * @param height
     *            the height of the bounding box, in pixels
     * @return Instance of TextEditor
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
    public static TextEditor createTextEditor(
        String text,
        int maxSize,
        int constraints,
        int width,
        int height)
    {
        return new TextEditorImpl(text, maxSize, constraints, width, height);
    }

    /**
     * <P>
     * Creates a new empty <code>TextEditor</code> with the given maximum size
     * in characters, constraints and editor size as number of visible rows.
     * </P>
     *
     * <P>
     * The <code>rows</code> parameter indicates the requested number of visible
     * rows in the editor. TextEditor then assigns the initial height for the
     * editor based on the requested number of rows and width of the editor.
     * </P>
     *
     * <P>
     * The <code>maxSize</code> parameter must be greater than zero. An
     * <code>IllegalArgumentException</code> is thrown if the length of the
     * initial contents string exceeds <code>maxSize</code>. However, the
     * implementation may assign a maximum size smaller than the application had
     * requested. If this occurs, and if the length of the contents exceeds the
     * newly assigned maximum size, the contents are truncated from the end in
     * order to fit, and no exception is thrown.
     * </P>
     *
     * <P>
     * On Series60-devices, if the MIDlet is manufacturer or operator-signed,
     * the object returned by this method will also implement the
     * com.nokia.mid.ui.s60.TextEditor
     * </P>
     *
     * @param maxSize
     *            the maximum capacity in characters
     * @param constraints
     *            see the input constraints in <code>TextField</code></a>
     * @param width
     *            the width of the bounding box, in pixels
     * @param rows
     *            the requested number of visible rows in the editor
     * @return Instance of TextEditor
     *
     * @throws IllegalArgumentException
     *             if <code>aMaxSize</code> or <code>aRows</code> is zero or
     *             less
     * @throws IllegalArgumentException
     *             if the value of the constraints parameter is invalid
     * @throws IllegalArgumentException
     *             if the length of the string exceeds the requested maximum
     *             capacity
     * @throws IllegalArgumentException
     *             if the width is less than one pixel
     */
    public static TextEditor createTextEditor(
        int maxSize,
        int constraints,
        int width,
        int rows)
    {
        return new TextEditorImpl(maxSize, constraints, width, rows);
    }

    /**
     * Sets this TextEditor focused or removes keyboard focus.
     * Calling setFocus(true) enables text editing from keys as the delivery
     * of needed key events will be targeted to the TextEditor instead of
     * the parent object (e.g. Canvas). Focus needs to be explicitly set to
     * the TextEditor by application. When Canvas or CustomItem does not have
     * any focused TextEditor all key and pointer events are delivered to
     * normal key delivery methods of parent. Necessary key events are
     * captured by TextEditor only when it has focus.
     * <P>
     * The setFocus affects to the parent key event delivery in following
     * way: If all TextEditors are unfocused the key event delivery of parent
     * works as if there are not any TextEditors in it, only when a focus is
     * set via this method to a TextEditor the key event delivery is modified.
     * The focused editor starts to capture the necessary key events and these
     * key events are not delivered to parent. The set of key events captured
     * depends on TextEditor implementation and device HW but in most
     * implementations nearly all device keys are captured by the editor for
     * text insertion, input mode changes and caret move functionalities.
     * Especially applications should not assume to get key events from keys
     * mapped to game actions as most probably the same keys are used to
     * navigate the text caret within the editor. The keys that are used for
     * Command launching in Canvas are available for applications on focused
     * TextEditor and these keys either launch commands or send low-level
     * key events as normally.
     * <P>
     * Setting focus does not cause any visual focus indication by the
     * implementation other than showing the caret. Any other change in the
     * visual appearance of the editor in focused state is the responsibility
     * of the application; this can be done for example by drawing a focus
     * border around the TextEditor or by changing the background color or
     * transparency of the editor.
     * <P>
     * If this TextEditor has already been focused earlier and the editor
     * contents has not changed after previous unfocusing, then after calling
     * setFocus again the caret position should be retained.
     * <P>
     * If there already is another focused TextEditor on the Canvas, the
     * focus is first removed from that TextEditor before setting this
     * TextEditor focused.
     * <P>
     * Calling setFocus(false) disables key based text editing and returns
     * the delivery of key events and pointer events to the underlying Canvas.
     *
     * @param focused
     *            keyboard focus of the TextEditor
     *
     * @throws java.lang.IllegalStateException
     *             If the TextEditor is not added to Canvas
     *
     * @see #hasFocus()
     */

    public void setFocus(boolean focused)
    {
        synchronized (iToolkit)
        {
            checkParent();

            // If focus is set, adjust the focus of other known editors.
            if (focused)
            {
                // Remove focus from other editors in this parent
                Enumeration editors = iEditorContainer.getEditors(iParent);

                while (editors.hasMoreElements())
                {
                    TextEditor editor = (TextEditor) editors.nextElement();

                    // Do not remove focus from this editor if it is on.
                    if (editor != this)
                    {
                        editor.setFocus(false);
                    }
                }
            }

            NativeError.check(_setFocusState(getToolkitHandle(), iHandle,
                                             focused));
        }
    }

    /**
     * Returns the focus state of TextEditor.
     *
     * @return the focus state of the editor
     * @see #setFocus(boolean focused)
     */
    public boolean hasFocus()
    {
        boolean focusState;

        synchronized (iToolkit)
        {
            checkParent();
            focusState = _getFocusState(getToolkitHandle(), iHandle);
        }

        return focusState;
    }

    /**
     * Set the parent object of this <code>TextEditor</code>.
     * <P>
     * Typically the parent object would be Canvas. Setting the parameter to
     * null removes the association to the parent. If
     * <code>setParent(null)</code> is called for a <code>TextEditor</code> yet
     * not having any parent or <code>setParent(parent)</code> is called with
     * the same parent, the call is silently ignored.
     * <P>
     *
     * @param parent
     *            the parent object
     *
     * @throws IllegalArgumentException
     *             if parent is not a valid object with which a
     *             <code>TextEditor</code> can be associated, or if CanvasItem
     *             is already set to another another parent
     */
    public void setParent(java.lang.Object parent)
    {
        // Ignore argument check if parent is set to null.
        // Custom item support will be added in future, currently it's
        // unsupported.
        if (((parent != null) && !(parent instanceof Canvas)) ||
                ((parent != null) && (iParent != null) && (iParent != parent)))
        {
            throw new IllegalArgumentException(ERROR_NOT_A_VALID_PARENT_OBJECT);
        }

        int parentHandle = 0;

        if (parent != iParent)
        {
            // Lock this object so that parent cannot be adjusted from
            // other threads.
            synchronized (iToolkit)
            {
                // Toolkit invoker can be used to get proper handles.
                if (parent instanceof Canvas)
                {
                    // Note that canvas is shown inside a displayable so in
                    // this case we use the handle to the displayble object.
                    parentHandle = iToolkitInvoker.getDisplayableHandle(parent);
                }
                else if (parent instanceof CustomItem)
                {
                    parentHandle = iToolkitInvoker.itemGetHandle(parent);
                }

                // parentHandle = 0 indicates removal of the parent.
                int error =
                    _setParent(getToolkitHandle(), iHandle, parentHandle);

                // Check if an error occured when setting the parent object.
                NativeError.check(error);

                // Operation was successful. Store parent and register
                // to container. Remove from container and previous parent.
                if (iParent != null)
                {
                    iEditorContainer.removeEditor(this, iParent);
                }
                // Store the editor with new parent if not null.
                if (parent != null)
                {
                    iEditorContainer.addEditor(this, parent);
                }

                iParent = parent;
            }
        }
    }

    /**
     * Sets the size of this <code>TextEditor</code> in pixels.
     *
     * @param width
     *            width in pixels
     * @param height
     *            height in pixels
     *
     * @throws IllegalArgumentException
     *             if the width or height is less than one pixel
     */
    public void setSize(int width, int height)
    {
        // Validate width and row count
        if (width < 1 || height < 1)
        {
            throw new IllegalArgumentException(ERROR_GIVEN_ARGUMENTS_NOT_VALID);
        }

        synchronized (iToolkit)
        {
            NativeError.check(_setSize(getToolkitHandle(), iHandle, width,
                                       height));

            // Operation was a success, store size.
            iWidth = width;
            iHeight = height;
        }
    }

    /**
     * <P>
     * Sets the rendering position of this <code>TextEditor</code>. The anchor
     * point given is relative to the upper left corner of the parent,
     * <code>Canvas</code>.
     * </P>
     *
     * <P>
     * The <code>TextEditor</code> may be placed fully off or partially of the
     * visible area of the parent, <code>Canvas</code>, by the
     * <code>setPosition</code> method; in this case the <code>TextEditor</code>
     * is just partly visible.
     * </P>
     *
     * @param x
     *            the x coordinate of the anchor point, in pixels.
     * @param y
     *            the y coordinate of the anchor point, in pixels.
     * @throws IllegalStateException
     *             if a valid parent object has not been set.
     * @see #setParent(java.lang.Object parent)
     */
    public void setPosition(int x, int y)
    {
        synchronized (iToolkit)
        {
            checkParent();

            NativeError.check(_setPosition(getToolkitHandle(), iHandle, x, y));

            // Store current position.
            iPositionX = x;
            iPositionY = y;
        }
    }

    /**
     * <P>
     * Sets the visibility value of <code>TextEditor</code>. Initially
     * <code>TextEditor</code> is not visible so it must be explicitly set to
     * visible in order it to appear on UI.
     * </P>
     *
     * <P>
     * Setting visibility to true shows the editor with its content, but without
     * the caret. If the editor does not have any visible content and does not
     * have any background color set then this method effectively does not cause
     * any visual change in the display. If the editor is already visible
     * calling <code>setVisible(true)</code> does nothing.
     * </P>
     *
     * <P>
     * Setting the visibility to false hides the editor and its content. If the
     * <code>TextEditor</code> has focus then the focus is removed. If the
     * editor is already hidden calling <code>setVisible(false)</code> does
     * nothing.
     * </P>
     *
     * @param visible
     *            visibility of the <code>TextEditor</code>
     * @throws IllegalStateException
     *             if a valid parent object has not been set.
     * @see #setParent(java.lang.Object parent)
     */
    public void setVisible(boolean visible)
    {
        synchronized (iToolkit)
        {
            checkParent();

            NativeError
            .check(_setVisible(getToolkitHandle(), iHandle, visible));

            iVisible = visible;
        }
    }

    /**
     * <P>
     * Sets the Z-position, or the elevation, of the item.
     * <p>
     * The elevation decides the stacking order of neighboring items. An item of
     * high Z-position will be drawn on top of an item with a lower Z-position
     * if they share the same parent item.
     * <p>
     * The z-position is unique for each item meaning that changing a Z-position
     * of an item may change the Z-position of the items that share the same
     * parent item. The Z-position does not affect the item's size in any way.
     * <p>
     * When items are added with {@link CanvasItem#setParent} they will get a
     * Z-position that is increased by 1 from the item that is the top most item
     * at that time.
     * </P>
     *
     * @param z
     *            the Z-position of the item.
     *
     * @throws IllegalArgumentException
     *             If <code>z</code> < 0
     * @throws IllegalStateException
     *             if a valid parent object has not been set.
     *
     * @see #getZPosition()
     * @see #setParent(java.lang.Object parent)
     */
    public void setZPosition(int z)
    {
        if (z < 0)
        {
            throw new IllegalArgumentException();
        }

        synchronized (iToolkit)
        {
            checkParent();

            NativeError.check(_setZPosition(getToolkitHandle(), iHandle, z));
        }
    }

    /**
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

    /**
     * Gets the current touch-enabled state
     * <p>
     * @return true if the editor is touch-enabled, false otherwise
     */
    public boolean isTouchEnabled()
    {
        return iTouchEnabled;
    }

    /**
     * <P>
     * Returns the Z-position, or the elevation, of the item. The Z-position
     * decides the stacking order of neighboring items.
     * </P>
     *
     * @return the Z-position of the item.
     *
     * @throws IllegalStateException
     *             if a valid parent object has not been set.
     *
     * @see #setZPosition(int)
     * @see #setParent(java.lang.Object parent)
     */
    public int getZPosition()
    {
        int positionZ = -1;

        synchronized (iToolkit)
        {
            checkParent();

            positionZ = _getZPosition(getToolkitHandle(), iHandle);
        }

        NativeError.check(positionZ);
        return positionZ;
    }

    /**
     * Gets the line margin height in this <code>TextEditor</code> in pixels.
     * <p>
     * Gets the possible line margin height that editor has in addition to the
     * normal font height (<code>Font getHeight()</code>). The returned value is
     * 0 if the editor does not have any additional line margins to the text
     * height returned from <code>getHeight()</code> of Font set to the editor.
     * <p>
     *
     * @return the line margin height in pixels
     *
     * @see #setParent(java.lang.Object parent)
     */
    public int getLineMarginHeight()
    {
        int lineMarginHeight = 0;

        if (iParent != null)
        {
            synchronized (iToolkit)
            {
                // Get the line margin height from the native side.
                lineMarginHeight =
                    _getLineMarginHeight(getToolkitHandle(), iHandle);
            }

            // Check if the line margin height was not returned correctly.
            NativeError.check(lineMarginHeight);
        }
        return lineMarginHeight;
    }

    /**
     * Gets the whole content height in this <code>TextEditor</code> in pixels.
     * <p>
     *
     * The returned value must include the height of the whole content in the
     * editor, not just the height of the visible content.
     * <p>
     *
     * Note that the method returns 0, if the editor has no parent.
     * <p>
     *
     * @return the height of the whole content in the editor in pixels
     *
     * @see #setParent(java.lang.Object parent)
     */
    public int getContentHeight()
    {
        int contentHeight = 0;
        if (iParent != null)
        {
            synchronized (iToolkit)
            {
                // Get the content height from the native side.
                contentHeight = _getContentHeight(getToolkitHandle(), iHandle);
            }

            // Check if the size was not returned correctly.
            NativeError.check(contentHeight);
        }
        return contentHeight;
    }

    /**
     * <P>
     * Sets the index of the caret. The caret can be used to indicate a position
     * in the text.
     * </P>
     *
     * <P>
     * The Java platform implementation must scroll the content of the
     * <code>TextEditor</code> automatically so that the caret is within the
     * visible area. If the caret is set above the current position the content
     * should be scrolled so that the caret is on the top most visible row. If
     * the caret is set below the current position the content should be
     * scrolled so that the caret is on the lowest visible row.
     * </P>
     *
     * @param index
     *            the character index before which to place the caret
     *
     * @throws StringIndexOutOfBoundsException
     *             if <code>index</code> does not specify a valid range within
     *             the content of the <code>TextEditor</code>
     */
    public void setCaret(int index)
    {
        // New position must be in range of the current content.
        if (index < 0 || index > size())
        {
            throw new StringIndexOutOfBoundsException(
                ERROR_GIVEN_ARGUMENTS_NOT_VALID);
        }

        synchronized (iToolkit)
        {
            NativeError.check(_setCaret(getToolkitHandle(), iHandle, index));
        }
    }

    /**
     * Gets the current position of the caret in the editor.
     *
     * @return the current caret position, <code>0</code> if at the beginning
     */
    public int getCaretPosition()
    {
        int caretPosition = 0;

        synchronized (iToolkit)
        {
            caretPosition = _getCaretPosition(getToolkitHandle(), iHandle);
        }

        // Verify that the operation was a success.
        NativeError.check(caretPosition);
        return caretPosition;
    }

    /**
     * <P>
     * Gets the topmost pixel position of the topmost visible line in the
     * editor.
     * </P>
     *
     * <P>
     * The returned y coordinate value is relative to the whole content height,
     * not just the visible part.
     * </P>
     *
     * <P>
     * This method can be used by the application together with the
     * <code>getContentHeight</code>, height of the font,
     * <code>getLineMarginHeight</code>, and <code>getCaretPosition</code>
     * methods in drawing custom visual cues like a scroll bar or other content
     * sensitive pop-ups.
     * </P>
     *
     * <P>
     * Note that the method returns 0, if the editor has no parent.
     * </P>
     *
     * @return the topmost pixel position of the visible content.
     */
    public int getVisibleContentPosition()
    {
        int ypos = 0;

        if (iParent != null)
        {
            synchronized (iToolkit)
            {
                ypos = _getVisibleContentPosition(getToolkitHandle(), iHandle);
            }

            // Verify that the operation was a success.
            NativeError.check(ypos);
        }
        return ypos;
    }

    /**
     * <P>
     * Gets the font being used in rendering the text content in this
     * <code>TextEditor</code>.
     * </P>
     *
     * @return the font being used in this <code>TextEditor</code>
     */
    public Font getFont()
    {
        synchronized (iToolkit)
        {
            return iFont == null ? Font.getDefaultFont() : iFont;
        }
    }

    /**
     * <P>
     * Sets the application preferred font for rendering the text content in
     * this <code>TextEditor</code>. Setting the font is a hint to the
     * implementation, and the implementation may disregard the requested font.
     * </P>
     *
     * <P>
     * The <code>font</code> parameter must be a valid <code>Font</code> object
     * or <code>null</code>. If the <code>font</code> parameter is
     * <code>null</code>, the implementation must use its default font to render
     * the text content.
     * </P>
     *
     * @param font
     *            the application preferred font to be used in this
     *            <code>TextEditor</code>
     */
    public void setFont(Font font)
    {
        if (font == null)
        {
            font = Font.getDefaultFont();
        }

        int[] newSize = new int[2];

        synchronized (iToolkit)
        {
            NativeError.check(_setFont(getToolkitHandle(), iHandle,
                                       iLCDUIPackageInvoker.getFontHandle(font), newSize));

            iFont = font;

            iWidth = newSize[0];
            iHeight = newSize[1];
        }
    }

    /**
     * Gets the background color and alpha of this <code>TextEditor</code>.
     *
     * @return the background color
     */
    public int getBackgroundColor()
    {
        int color = 0;

        synchronized (iToolkit)
        {
            color = _getColor(getToolkitHandle(), iHandle,
                              COLOR_BACKGROUND);
        }
        return color;
    }

    /**
     * Gets the foreground color and alpha of this <code>TextEditor</code>
     *
     * @return the foreground color
     */
    public int getForegroundColor()
    {
        int color = 0;

        synchronized (iToolkit)
        {
            color = _getColor(getToolkitHandle(), iHandle,
                              COLOR_FOREGROUND);
        }
        return color;
    }

    /**
     * Sets the background color and alpha of this <code>TextEditor</code> to
     * the specified values. The default background color for editor is fully
     * transparent white.
     *
     * @param color
     *            the color
     */
    public void setBackgroundColor(int color)
    {
        int alpha = (color >> 24) & 0xff;
        int red   = (color >> 16) & 0xff;
        int green = (color >>  8) & 0xff;
        int blue  = (color) & 0xff;

        doSetColor(alpha, red, green, blue, COLOR_BACKGROUND);
    }

    /**
     * Sets the foreground color and alpha of this <code>TextEditor</code> to
     * the specified values. The content, e.g. text, in the editor must be drawn
     * with this color. The default foreground color is fully opaque black.
     *
     * @param color
     *            the color
     */
    public void setForegroundColor(int color)
    {
        int alpha = (color >> 24) & 0xff;
        int red   = (color >> 16) & 0xff;
        int green = (color >>  8) & 0xff;
        int blue  = (color) & 0xff;

        doSetColor(alpha, red, green, blue, COLOR_FOREGROUND);
    }

    /**
     * Sets the highlight background color. This impacts text background color
     * for the selected text.
     * The text in a current selection range will be
     * rendered using given color value. The default highlight background
     * color is fully opaque black.
     *
     * This method is not supported on S40 platform.
     *
     * @param color
     *            the color
     */
    public void setHighlightBackgroundColor(int color)
    {
        int alpha = (color >> 24) & 0xff;
        int red   = (color >> 16) & 0xff;
        int green = (color >>  8) & 0xff;
        int blue  = (color) & 0xff;

        doSetColor(alpha, red, green, blue, COLOR_HIGHLIGHT_BACKGROUND);
    }

    /**
     * Sets the highlight foreground color. This impacts text color for the
     * selected text.
     * The text in a current selection range will be rendered
     * using given color value. The default highlight foreground color is fully
     * opaque white.
     *
     * This method is not supported on S40 platform.
     *
     * @param color
     *            the color
     */
    public void setHighlightForegroundColor(int color)
    {
        int alpha = (color >> 24) & 0xff;
        int red   = (color >> 16) & 0xff;
        int green = (color >>  8) & 0xff;
        int blue  = (color) & 0xff;

        doSetColor(alpha, red, green, blue, COLOR_HIGHLIGHT_FOREGROUND);
    }

    /**
     * Sets the content of the <code>TextEditor</code> as a string. The set
     * string replaces any previous content in the editor.
     *
     * @param content
     *            the new content of the <code>TextEditor</code> as string,
     *            <code>null</code> empties the <code>TextEditor</code>
     *
     * @throws IllegalArgumentException
     *             if <code>aContent</code> is illegal for the current input
     *             constraints
     * @throws IllegalArgumentException
     *             if the given text would exceed the current maximum capacity
     *             of the editor
     */
    public void setContent(String content)
    {
        // Verify that text is not invalid.
        iLCDUIPackageInvoker.checkText(content, iMaxSize);

        synchronized (iToolkit)
        {
            NativeError
            .check(_setContent(getToolkitHandle(), iHandle, content));
        }
    }

    /**
     * Gets the string content in the <code>TextEditor</code>.
     *
     * @return The editor current content
     */
    public String getContent()
    {
        String content = null;
        int[] error = new int[1];

        synchronized (iToolkit)
        {
            content = _getContent(getToolkitHandle(), iHandle, error);
        }

        NativeError.check(error[0]);
        return content;
    }

    /**
     * <P>
     * Inserts a string into the content of the <code>TextEditor</code>.
     * </P>
     *
     * <P>
     * The string is inserted just prior to the character indicated by the
     * <code>position</code> parameter, where zero specifies the first character
     * of the content in the <code>TextEditor</code>. If <code>position</code>
     * is less than or equal to zero, the insertion occurs at the beginning of
     * the content. If <code>position</code> is greater than or equal to the
     * current size of the content, the insertion occurs immediately after the
     * end of the content.
     * </P>
     *
     * <P>
     * The current size of the contents is increased by the number of inserted
     * characters. The resulting string must fit within the current maximum
     * capacity.
     * </P>
     *
     * @param text
     *            the <code>String</code> to be inserted
     * @param position
     *            the position at which insertion is to occur
     *
     * @throws IllegalArgumentException
     *             if the resulting content would be illegal for the current
     *             input constraints
     * @throws IllegalArgumentException
     *             if the insertion would exceed the current maximum capacity
     * @throws NullPointerException
     *             if <code>text</code> is <code>null</code>
     */
    public void insert(String text, int position)
    {
        // Check illegal arguments. Note that NullPointerException is thrown
        // correctly if text is null when text.length() is called.
        if (text.length() + size() > iMaxSize)
        {
            throw new IllegalArgumentException(ERROR_GIVEN_ARGUMENTS_NOT_VALID);
        }

        synchronized (iToolkit)
        {
            NativeError.check(_insert(getToolkitHandle(), iHandle, text,
                                      position));
        }
    }

    /**
     * <P>
     * Deletes characters from the <code>TextEditor</code>.
     * </P>
     *
     * <P>
     * The <code>offset</code> and <code>length</code> parameters must specify a
     * valid range of characters within the contents of the
     * <code>TextEditor</code>. The <code>offset</code> parameter must be within
     * the range <code>[0..(size())]</code>, inclusive. The <code>length</code>
     * parameter must be a non-negative integer such that
     * <code>(offset + length) &lt;= size()</code>.
     * </P>
     *
     * @param offset
     *            the beginning of the region to be deleted
     * @param length
     *            the number of characters to be deleted
     *
     * @throws IllegalArgumentException
     *             if the resulting contents would be illegal for the current
     *             input constraints
     * @throws StringIndexOutOfBoundsException
     *             if <code>offset</code> and <code>length</code> do not specify
     *             a valid range within the content of the
     *             <code>TextEditor</code>
     */
    public void delete(int offset, int length)
    {
        iLCDUIPackageInvoker.checkOffset(offset, length, size());

        synchronized (iToolkit)
        {
            NativeError.check(_delete(getToolkitHandle(), iHandle, offset,
                                      length));
        }
    }

    /**
     * Returns the maximum size (number of characters) that can be stored in
     * this <code>TextEditor</code>.
     *
     * @return the maximum size in characters
     */
    public int getMaxSize()
    {
        return iMaxSize;
    }

    /**
     * Sets the maximum size (number of characters) that can be contained in
     * this <code>TextEditor</code>. If the current content of the
     * <code>TextEditor</code> is larger than the new <code>maxSize</code>, the
     * content is truncated to fit.
     *
     * @param maxSize
     *            the new maximum size
     *
     * @return assigned maximum capacity - may be smaller than requested.
     *
     * @throws IllegalArgumentException
     *             if <code>maxSize</code> is zero or less.
     * @throws IllegalArgumentException
     *             if the contents after truncation would be illegal for the
     *             current input constraints
     */
    public int setMaxSize(int maxSize)
    {
        if (maxSize <= 0)
        {
            throw new IllegalArgumentException(ERROR_GIVEN_ARGUMENTS_NOT_VALID);
        }

        int newMaxSize = 0;

        synchronized (iToolkit)
        {
            newMaxSize = _setMaxSize(getToolkitHandle(), iHandle, maxSize);
            NativeError.check(newMaxSize);

            // Operation was successful, store maximum size.
            iMaxSize = newMaxSize;
        }
        return newMaxSize;
    }

    /**
     * Gets the number of characters that are currently stored in this
     * <code>TextEditor</code>.
     *
     * @return the number of characters
     */
    public int size()
    {
        int size = 0;

        synchronized (iToolkit)
        {
            // Get the size from the native side.
            size = _size(getToolkitHandle(), iHandle);
        }

        // Check if the size was not returned correctly.
        NativeError.check(size);
        return size;
    }

    /**
     * Sets the input constraints of this <code>TextEditor</code>. If the
     * current content of this <code>TextEditor</code> do not match the new
     * constraints, the content is set to empty.
     *
     * @param constraints
     *            see <a href="TextField.html#constraints">input constraints</a>
     *
     * @throws IllegalArgumentException
     *             if the value of the constraints parameter is invalid
     */
    public void setConstraints(int constraints)
    {
        // Validate constraints through TextComponentInvoker.
        iLCDUIPackageInvoker.checkConstraints(constraints);

        synchronized (iToolkit)
        {
            NativeError.check(_setConstraints(getToolkitHandle(), iHandle,
                                              constraints));

            iConstraints = constraints;
        }
    }

    /**
     * Gets the current input constraints of this <code>TextEditor</code>.
     *
     * @return the current constraints value (see <a
     *         href="TextField.html#constraints">input constraints</a>)
     */
    public int getConstraints()
    {
        return iConstraints;
    }

    /**
     * <P>
     * Sets a hint to the implementation as to the input mode that should be
     * used when the user initiates editing of this <code>TextEditor</code>. The
     * <code>characterSubset</code> parameter names a subset of Unicode
     * characters that is used by the implementation to choose an initial input
     * mode. If <code>null</code> is passed, the implementation should choose a
     * default input mode.
     * </P>
     *
     * <P>
     * See <a href="TextField#modes">Input Modes</a> for a full explanation of
     * input modes.
     * </P>
     *
     * @param characterSubset
     *            a string naming a Unicode character subset, or
     *            <code>null</code>
     */
    public void setInitialInputMode(String characterSubset)
    {
        synchronized (iToolkit)
        {
            NativeError.check(_setInitialInputMode(getToolkitHandle(), iHandle,
                                                   characterSubset));

            iCharacterSubset = characterSubset;
        }
    }

    /**
     * Returns the initial input mode set to the editor, or null if no initial
     * input mode has been set.
     *
     * @return the initial input mode as a String, or null if no initial input
     *         mode is set.
     *
     * @see #setInitialInputMode(String characterSubset )
     */
    public String getInitialInputMode()
    {
        return iCharacterSubset;
    }

    /**
     * <P>
     * Sets a selection on a range of text in the <code>TextEditor</code>
     * content. The implementation should highlight the selection visually. A
     * selection may be set with this method or by user interaction. If there
     * already is a selection set, it is replaced by this new selection.
     * </P>
     *
     * <P>
     * The caret must be automatically set directly after the set selection. The
     * Java platform implementation must scroll the content of the
     * <code>TextEditor</code> automatically so that the caret is within the
     * visible area and as much as possible of the selection is visible in the
     * <code>TextEditor</code>.
     * </P>
     *
     * @param index
     *            the index of the first character to be selected.
     * @param length
     *            the length of the selection in characters, 0 length clears the
     *            selection.
     *
     * @throws StringIndexOutOfBoundsException
     *             if <code>index</code> and <code>length</code> do not specify
     *             a valid range within the content of the
     *             <code>TextEditor</code>
     */
    public void setSelection(int index, int length)
    {
        iLCDUIPackageInvoker.checkOffset(index, length, size());

        synchronized (iToolkit)
        {
            NativeError.check(_setSelection(getToolkitHandle(), iHandle, index,
                                            length));
        }
    }

    /**
     * Gets the currently selected content in the <code>TextEditor</code>. A
     * selection may be set with setSelection method or by user interaction. If
     * selection is not set non-null empty string is returned.
     *
     * @return the currently selected content
     */
    public String getSelection()
    {
        String selection = null;
        int[] error = new int[1];

        synchronized (iToolkit)
        {
            selection = _getSelection(getToolkitHandle(), iHandle, error);
        }

        NativeError.check(error[0]);
        return selection;
    }

    /**
     * <P>
     * Sets a listener for content changes in this <code>TextEditor</code>,
     * replacing any previous <code>TextEditorListener</code>.
     * </P>
     *
     * <P>
     * A <code>null</code> reference is allowed and has the effect of removing
     * any existing <code>TextEditorListener</code> from this
     * <code>TextEditor</code>.
     * </P>
     *
     * <P>
     * Callbacks to the TextEditorListener may throw exceptions, but they must
     * be ignored.
     * </P>
     *
     * @param listener
     *            the new listener, or <code>null</code>
     */
    public void setTextEditorListener(TextEditorListener listener)
    {
        TextEditorListenerImpl newListener = null;
        int listenerHandle = 0;

        // Create new listener implementation if listener has been defined.
        if (listener != null)
        {
            newListener = new TextEditorListenerImpl(this, listener);
            listenerHandle = newListener.getHandle();
        }

        synchronized (iToolkit)
        {
            NativeError.check(_setListener(getToolkitHandle(), iHandle,
                                           listenerHandle));
        }

        iListener = newListener;
    }

    /**
     * Returns the multiline state of the <code>TextEditor</code>.
     *
     * @return the multiline state
     */
    public boolean isMultiline()
    {
        boolean multiline = false;

        synchronized (iToolkit)
        {
            multiline = _isMultiline(getToolkitHandle(), iHandle);
        }

        return multiline;
    }

    /**
     * Sets the editor to be either multi-line (true) or single-line (false).
     * TextEditor is by default single-line editor, which meeans that user
     * is not possible to insert line breaks to the editor. A possible Enter
     * (or similar) key event should be passed in this case to the parent object
     * as a normal key event for client to handle it as appropriately. A single-line
     * editor will have horizontal scrolling of text if it is possible to enter text
     * that does not fit to the editor. This may happen if maximum size is large enough
     * for the width of the editor.
     * In multi-line editor user is able to insert line breaks but also word
     * wrapping is enabled automatically on word boundaries.
     *
     * @param aMultiline
     *             true if multi-line editor, false if single-line editor
     *
     * @throws IllegalStateException if a valid parent object has not been set.
     */
    public void setMultiline(boolean aMultiline)
    {
        synchronized (iToolkit)
        {
            // Parent must be set
            checkParent();

            NativeError.check(_setMultiline(getToolkitHandle(), iHandle,
                                            aMultiline));
        }
    }

    /*
     * Disposes the Landmark native peer object, if the handles are valid.
     * Invalid (negative) handles indicate that their creation failed in the
     * first place.
     */
    final void registeredFinalize()
    {
        synchronized (iToolkit)
        {
            if (iHandle > 0)
            {
                _dispose(getToolkitHandle(), iHandle);
                iHandle = 0;

                if (iParent != null)
                {
                    // Remove from editor container.
                    iEditorContainer.removeEditor(this, iParent);
                }
            }
        }
    }

    /*
     * Returns the native side handle to the LCDUI Toolkit.
     *
     * @return The native side handle to the LCDUI Toolkit.
     */
    synchronized int getToolkitHandle()
    {
        return iToolkitInvoker.toolkitGetHandle(iToolkit);
    }

    /*
     * Package private default constructor.
     */
    TextEditor()
    {
    }

    /*
     * Package private constructor.
     *
     * Allocates the needed native side resources.
     *
     * @param aContent The content of the editor.
     *
     * @param maxSize The maximum Size of the editor.
     *
     * @param constraints The constraints of the editor.
     *
     * @param aWidth The width of the editor in pixels.
     *
     * @param aHeight The height of the editor in pixels on in rows.
     *
     * @param aHeighInRows If <code>true</code>, height is measured as row
     * count.
     *
     * @throws IllegalArgumentException if the width or height is less than one
     * pixel
     */
    TextEditor(
        String aContent,
        int maxSize,
        int constraints,
        int aWidth,
        int aHeight,
        boolean aHeightInRows)
    {
        // Get text component invoker.
        iLCDUIPackageInvoker = LCDUIPackageInvoker.getInvoker();
        // Check that the give the given text is valid.
        checkValid(aContent, maxSize, constraints);

        // Validate width and height.
        if (aWidth < 1 || aHeight < 1)
        {
            throw new IllegalArgumentException(ERROR_GIVEN_ARGUMENTS_NOT_VALID);
        }

        // Toolkit invoker is needed for accessing javax.microedition.lcdui
        // package
        iToolkitInvoker = ToolkitInvoker.getToolkitInvoker();
        iToolkit = iToolkitInvoker.getToolkit();

        int handle = 0;

        // The size of the editor must be known after construction. Use return
        // values to store the height and width of the editor after
        // construction.
        int[] size = new int[2];

        synchronized (iToolkit)
        {
            // Create native peer object for this Java object.
            handle =
                _createNativePeer(getToolkitHandle(), maxSize, aWidth, aHeight,
                                  aHeightInRows, size);
        }

        // Check if construction failed and throw out of memory error.
        if (handle <= NativeError.KErrNone)
        {
            throw new OutOfMemoryError();
        }

        // Operation was a success, store size.
        iWidth = size[0];
        iHeight = size[1];
        iMaxSize = maxSize;

        // Enabling receiving pointer events
        iTouchEnabled = true;

        // Sets parent to null
        iParent = null;

        // Construction was successful. Store handle and register for
        // finalization.
        iHandle = handle;
        mFinalizer = new Finalizer()
        {
            public void finalizeImpl()
            {
                registeredFinalize();
            }
        };

        // Set the constraints of the editor. The content is now empty.
        setConstraints(constraints);

        // Set font to Java default font.
        setFont(Font.getDefaultFont());

        // Set the text. This now throws an exception if the content is not
        // valid for the current set of constraints.
        setContent(aContent);
    }

    // Private methods.

    /*
     * Checks that the given arguments are valid.
     *
     * @param text The text to check.
     *
     * @param maxSize The maximum size to check.
     *
     * @param constraints The constraints to check.
     *
     * @throws java.lang.IllegalArgumentException if the arguments are not
     * valid.
     */
    private void checkValid(String text, int maxSize, int constraints)
    {
        try
        {
            iLCDUIPackageInvoker.checkText(text, maxSize);
            iLCDUIPackageInvoker.checkConstraints(constraints);
        }
        catch (IllegalArgumentException e)
        {
            throw new IllegalArgumentException(ERROR_GIVEN_ARGUMENTS_NOT_VALID);
        }
    }

    /*
     * Sets the specified color for the specified color type.
     *
     * @param alpha the alpha component of the color being set within range
     * <code>0-255</code>
     *
     * @param red the red component of the color being set within range
     * <code>0-255</code>
     *
     * @param green the green component of the color being set within range
     * <code>0-255</code>
     *
     * @param blue the blue component of the color being set within range
     * <code>0-255</code>
     *
     * @param aColorType the type of the color to be set.
     *
     * @throws IllegalArgumentException if any of the parameters is outside the
     * range of <code>0-255</code>
     */
    private void doSetColor(
        int alpha,
        int red,
        int green,
        int blue,
        int aColorType)
    {
        if ((alpha < 0 || alpha > COLOR_MAX_VALUE)
                || (red < 0 || red > COLOR_MAX_VALUE)
                || (green < 0 || green > COLOR_MAX_VALUE)
                || (blue < 0 || blue > COLOR_MAX_VALUE))
        {
            throw new IllegalArgumentException(ERROR_GIVEN_ARGUMENTS_NOT_VALID);
        }

        int[] color = new int[] { red, green, blue, alpha };

        synchronized (iToolkit)
        {
            NativeError.check(_setColor(getToolkitHandle(), iHandle, color,
                                        aColorType));
        }
    }

    // Native methods

    /*
     * Creates the native side peer object for this TextEditor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param maxSize The maximum size of the content in the text editor.
     *
     * @param aWidth The new width of this text editor.
     *
     * @param aHeight The new hight of this text editor.
     *
     * @param aHeightInRows If <code>true</code> height is measured in rows.
     *
     * @param aSize On return, contains the size of the editor as width and
     * height.
     *
     * @return A handle to the the native side peer object or a system-wide
     * error code.
     */
    private native int _createNativePeer(
        int aToolkitHandle,
        int maxSize,
        int aWidth,
        int aHeight,
        boolean aHeightInRows,
        int[] aSize);

    /*
     * Disposes the native side peer object.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     */
    private native void _dispose(int aToolkitHandle, int aNativePeerHandle);

    /*
     * Sets the size of this text editor using the given width and height.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aWidth The new width of this text editor.
     *
     * @param aHeight The new hight of this text editor.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setSize(
        int aToolkitHandle,
        int aNativePeerHandle,
        int aWidth,
        int aHeight);

    /*
     * Disposes the native side peer object.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aParentHandle A handle to the parent object.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setParent(
        int aToolkitHandle,
        int aNativePeerHandle,
        int aParentHandle);

    /*
     * Sets the focus state of text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aFocused Indicates whether the text editor should be focused or
     * not.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setFocusState(
        int aToolkitHandle,
        int aNativePeerHandle,
        boolean aFocusState);

    /*
     * Gets focus state of the text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aFocused Indicates whether the text editor should be focused or
     * not.
     *
     * @return focus state.
     */
    private native boolean _getFocusState(int aToolkitHandle,
                                          int aNativePeerHandle);

    /*
     * Gets the multiline setting of TextEditor.
     *
     * @return the multiline state.
     */
    private native boolean _isMultiline(int aToolkitHandle,
                                        int aNativePeerHandle);

    /*
     * Sets the text editor visible.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aVisible The visibility status of this text editor.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setVisible(
        int aToolkitHandle,
        int aNativePeerHandle,
        boolean aVisible);

    /*
     * Sets the text editor's position.
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
    private native int _setPosition(
        int aToolkitHandle,
        int aNativePeerHandle,
        int aX,
        int aY);

    /*
     * Sets the text input constraints of this text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param constraints The new constraints for the editor.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setConstraints(
        int aToolkitHandle,
        int aNativePeerHandle,
        int constraints);

    /*
     * Sets the editor to be either multi-line (true) or single-line (false).
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aMultiline True if multi-line editor, false if single-line editor.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     *         a system-wide error code is returned.
     */
    private native int _setMultiline(
        int aToolkitHandle,
        int aNativePeerHandle,
        boolean aMultiline);

    /*
     * Sets the initial input mode of this text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param characterSubset The character subset.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setInitialInputMode(
        int aToolkitHandle,
        int aNativePeerHandle,
        String characterSubset);

    /*
     * Deletes the content in the text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param offset The beginning of the range to be deleted.
     *
     * @param length The lenght of the range to be deleted.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _delete(
        int aToolkitHandle,
        int aNativePeerHandle,
        int offset,
        int length);

    /*
     * Returns the number of characters in the text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param The number of characters in the text editor or a system-wide error
     * code if the operation failed.
     */
    private native int _size(int aToolkitHandle, int aNativePeerHandle);

    /*
     * Returns the line margin height in the text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @return The line margin height in the text editor or a system-wide error
     * code if the operation failed.
     */
    private native int _getLineMarginHeight(
        int aToolkitHandle,
        int aNativePeerHandle);

    /*
     * Returns the number of characters in the text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @return The full content hight in the text editor or a system-wide error
     * code if the operation failed.
     */
    private native int _getContentHeight(
        int aToolkitHandle,
        int aNativePeerHandle);

    /*
     * Sets the content of this text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aContent The new content of this text editor.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setContent(
        int aToolkitHandle,
        int aNativePeerHandle,
        String aContent);

    /*
     * Inserts content to this text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aContent The content to be inserted to this text editor.
     *
     * @param position The position to which the content should be inserted.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _insert(
        int aToolkitHandle,
        int aNativePeerHandle,
        String aContent,
        int position);

    /*
     * Gets the content of this text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aError On return contains the error code for the operation.
     *
     * @return The content of the editor as a string.
     */
    private native String _getContent(
        int aToolkitHandle,
        int aNativePeerHandle,
        int[] aError);

    /*
     * Sets the maximum size of this text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param maxSize The new maximum size of this text editor.
     *
     * @return The maximum size of the editor if the operation was successful.
     * Otherwise, a system-wide error code is returned.
     */
    private native int _setMaxSize(
        int aToolkitHandle,
        int aNativePeerHandle,
        int maxSize);

    /*
     * Returns the current position of the caret in the text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param The current position of the caret or a system-wide error code if
     * the operation failed.
     */
    private native int _getCaretPosition(
        int aToolkitHandle,
        int aNativePeerHandle);

    /*
     * Returns the topmost pixel's position of the visible content.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param The topmost pixel's position of the visible content or a
     * system-wide error code if the operation failed.
     */
    private native int _getVisibleContentPosition(
        int aToolkitHandle,
        int aNativePeerHandle);

    /*
     * Sets a new position for the caret in the text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param index The new position of the caret.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setCaret(
        int aToolkitHandle,
        int aNativePeerHandle,
        int index);

    /*
     * Sets a new color for the specified color type in the text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aColor An array containing new color value (r,g,b,a).
     *
     * @param aColorType Type of color to set.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setColor(
        int aToolkitHandle,
        int aNativePeerHandle,
        int[] aColor,
        int aColorType);

    /*
     * Gets a color of the specified color type from the text editor.
     *
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aColorType Type of color to set.
     *
     * @return color in RGBA
     */
    private native int _getColor(
        int aToolkitHandle,
        int aNativePeerHandle,
        int aColorType);

    /*
     * Selects the specified range of text in the text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param index The beginning of the range to be selected.
     *
     * @param length The lenght of the range to be selected.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setSelection(
        int aToolkitHandle,
        int aNativePeerHandle,
        int index,
        int length);

    /*
     * Gets the current selection of this text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aError On return contains the error code for the operation.
     *
     * @return The selected content of the editor as a string.
     */
    private native String _getSelection(
        int aToolkitHandle,
        int aNativePeerHandle,
        int[] aError);

    /*
     * Sets the specified font for the text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aFont The application preferred font to be used in this
     * TextEditor.
     *
     * @param aNewSize On return, contains the new size of the editor.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setFont(
        int aToolkitHandle,
        int aNativePeerHandle,
        int aFont,
        int[] aNewSize);

    /*
     * Sets a listener for this text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param listenerHandle A handle to the listener.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setListener(
        int aToolkitHandle,
        int aNativePeerHandle,
        int listenerHandle);

    /*
     * Sets the elevation of this text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @param aZ The new z-position.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private native int _setZPosition(
        int aToolkitHandle,
        int aNativePeerHandle,
        int aZ);

    /*
     * Gets the elevation of this text editor.
     *
     * @param aToolkitHandle A handle to the LCDUI toolkit.
     *
     * @param aNativePeerHandle A handle to the native side peer object.
     *
     * @return The elevation if the operation was successful. Otherwise, a
     * system-wide error code is returned.
     */
    private native int _getZPosition(int aToolkitHandle, int aNativePeerHandle);

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
