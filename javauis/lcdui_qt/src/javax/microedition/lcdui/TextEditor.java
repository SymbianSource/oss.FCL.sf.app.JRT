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
* Description: 
*
*/
/*
 * - TextField modifier flag: single_line (if not present some constraint
 *     combinations may still be implicitly single line)
 * - TextField should define how caret moves after programmatic manipulation
 *     of the content; e.g. if text is inserted the caret should be in the
 *     end of the inserted text
 * - methods for adding / removing TextEditor (one or more) to
 *     Canvas / CustomItem
 * - z-order of added TextEditors (e.g. if they overlap)
 *
 * - any Command issues left after TextEditor is no longer an Item?
 */

/*
 *  $HeadURL$
 *
 *
 *  Copyright (c) 2008 Nokia.  All rights reserved.
 *
 */

package javax.microedition.lcdui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.*;

/**
 * <P>
 * A <code>TextEditor</code> is an editable text component that is draws on
 * top of a graphics surface; in LCDUI on top of <code>Canvas</code> or
 * <code>CustomItem</code> (including <code>IdleItem</code>). The
 * TextEditor must be presented as an own layer on top of the Canvas and the
 * paint cycle of Canvas must be independent of that; i.e. MIDlet does not see
 * the TextEditor content via the Graphics and the TextEditor must not rely on
 * the framebuffer of Canvas.
 * </P>
 * <P>
 * The implementation should initially present the TextEditor with minimal
 * decoration; only the caret should be shown at the requested text insertion
 * position. It is then the responsibility of the application to draw any
 * additional decoration like focus highlight, border or scroll bar.
 * </P>
 * <P>
 * The application controls whether the input indicators are shown or not and
 * the drawing position of the indicators on the display. If the application
 * sets no location for the indicators, the implementation must not draw any
 * indicators. If the application chooses the implementation to show input
 * indicators, the application can control the drawing colors of the indicators.
 * It should be noted that if there are no indicators are shown to the user, the
 * usability maybe diminished especially with complex input methods.
 * </P>
 * <P>
 * <code>TextEditor</code> supports <em>input constraints</em> identically
 * to <code>TextField</code>. See <a href="TextField.html#constraints">input
 * constraints</a> section in the <code>TextField</code> for the definition
 * of these constants. In addition <code>TextEditor</code> has the same
 * concept of <em>actual contents</em> and <em>displayed contents</em> as
 * <code>TextField</code>; these are described in the same <a
 * href="TextField.html#constraints">input constraints</a> section.
 * </P>
 * <P>
 * <code>TextEditor</code> supports <em>input modes</em> identically to
 * <code>TextField</code>. See <a href="TextField.html#modes">input modes</a>
 * section in the <code>TextField</code> for the definition of these
 * constants.
 * </P>
 * <P>
 * The text may contain <A HREF="Form.html#linebreak">line breaks</A>. The
 * display of the text must break accordingly and the user must be able to enter
 * line break characters.
 * </P>
 * <P>
 * The implementation should provide necessary interaction for example for pen
 * input (handwriting recognition) on touch screen devices and selection of
 * additional characters. Any implementation provided visuals should be drawn
 * with the drawing colors and transparency defined by the application.
 * </P>
 * <P>
 * Application can add a TextEditorChangeListener to the TextEditor for example
 * for keeping track of user navigation (caret movement) and other content
 * changes such as text selection. The events are sent on all occasions that
 * cause the the caret position to change (including but not limited to text
 * typing by user, programmatic text insertion, navigation within the TextEditor
 * content, and text deletion). The events must be sent to the application prior
 * to they have effect on the editor - for example an event indicating caret
 * movement must be available for the application before the implementation
 * actually moves the caret in the editor.
 * </P>
 * <P>
 * When the TextEditor has focus all the key events that are not consumed by
 * TextEditor or mapped to Commands by the implementation are sent to Canvas.
 * </P>
 *
 * @since 3.0
 */

public class TextEditor {

    private static final int BITSPERPIXEL8 = 255;

    private TextEditorChangeListener changelistener;

    private TextModifyListener modListener = new TextModifyListener();
    private TextSelectionListener selListener = new TextSelectionListener();

    private TextWrapper textWrapper;
    private Object parent;

    /**
     * Creates a new <code>TextEditor</code> object with the given initial
     * contents, maximum size in characters, constraints and editor size in
     * pixels. If the text parameter is <code>null</code>, the
     * <code>TextEditor</code> is created empty. The <code>maxSize</code>
     * parameter must be greater than zero. An
     * <code>IllegalArgumentException</code> is thrown if the length of the
     * initial contents string exceeds <code>maxSize</code>. However, the
     * implementation may assign a maximum size smaller than the application had
     * requested. If this occurs, and if the length of the contents exceeds the
     * newly assigned maximum size, the contents are truncated from the end in
     * order to fit, and no exception is thrown.
     *
     * @param text the initial contents, or <code>null</code> if the
     *            <code>TextEditor</code> is to be created empty
     * @param maxSize the maximum capacity in characters
     * @param constraints see the input constraints in TextField
     * @param width the width of the bounding box, in pixels
     * @param height the height of the bounding box, in pixels
     * @throws IllegalArgumentException if <code>maxSize</code> is zero or
     *             less
     * @throws IllegalArgumentException if the value of the constraints
     *             parameter is invalid
     * @throws IllegalArgumentException if <code>text</code> is illegal for
     *             the specified constraints
     * @throws IllegalArgumentException if the length of the string exceeds the
     *             requested maximum capacity
     * @throws IllegalArgumentException if the width or height is less than zero
     */
    public TextEditor(String text, int maxSize, int constraints, int width,
            int height) {
        textWrapper = new TextWrapper(text, maxSize, constraints);
        textWrapper.setFocused(false);
        textWrapper.setModifyListener(modListener);
        textWrapper.setSelectionListener(selListener);
        textWrapper.setPosition(0, 0);
        textWrapper.setSize(width, height);
    }

    /**
     * Set the parent of this TextEditor. Null parameter removes it.
     *
     * @param newParent parent
     */
    public void setParent(Object newParent) {
        if (newParent != null) {
            if (newParent instanceof Canvas) {
                // constructing on another composite disposes the old one
                textWrapper.construct(((Canvas) newParent).getContentComp(),
                        SWT.NONE);
                parent = newParent;
            }
        }
        else {
            textWrapper.dispose();
            parent = newParent;
        }
    }

    /**
     * Deletes characters from the <code>TextEditor</code>. The
     * <code>offset</code> and <code>length</code> parameters must specify a
     * valid range of characters within the contents of the
     * <code>TextEditor</code>. The <code>offset</code> parameter must be
     * within the range <code>[0..(size())]</code>, inclusive. The
     * <code>length</code> parameter must be a non-negative integer such that
     * <code>(offset + length) &lt;= size()</code>.
     *
     * @param offset the beginning of the region to be deleted
     * @param length the number of characters to be deleted
     * @throws IllegalArgumentException if the resulting contents would be
     *             illegal for the current input constraints
     * @throws StringIndexOutOfBoundsException if <code>offset</code> and
     *             <code>length</code> do not specify a valid range within the
     *             content of the <code>TextEditor</code>
     */
    public void delete(int offset, int length) {
        textWrapper.delete(offset, length);
    }

    /**
     * <AU>note: Modified method name</AU> Gets the current position of the
     * caret in the editor in pixels. The returned x and y coordinates are
     * relative to the upper left corner of the editor, and are returned in this
     * order. The x value must mark horizontally the middlemost pixel of the
     * caret and the y value must mark vertically the lowest pixel of the caret.
     *
     * @return the x and y coordinates of the current caret position
     */
    public int[] getCaretPixelPosition() {
        // TODO: implement this
        return null;
    }

    /**
     * Gets the current position of the caret in the editor.
     *
     * @return the current caret position, <code>0</code> if at the beginning
     */
    public int getCaretPosition() {
        return textWrapper.getCaretPosition();
    }

    /**
     * Gets the current input constraints of this <code>TextEditor</code>.
     *
     * @return the current constraints value (see <a
     *         href="TextField.html#constraints">input constraints</a>)
     */
    public int getConstraints() {
        return textWrapper.getConstraints();
    }

    /**
     * Gets a string on a range of the content in the <code>TextEditor</code>.
     * The range is defined by the given index and length.
     *
     * @param index the index of the first character to be returned
     * @param length the number of the characters to be returned
     * @return the editor current content
     */
    public String getContent(int index, int length) {
        String content = textWrapper.getContent();
        return content.substring(Math.max(0, index),
                Math.min(index + length, content.length()));
    }

    /**
     * Gets the whole content height in this TextEditor in pixels. The returned
     * value must include the height of the whole content in the editor, not
     * just the height of the visible content.
     *
     * @return the height of the whole content in the editor in pixels
     */
    public int getContentHeight() {
        return textWrapper.getLineCount() * textWrapper.getLineHeight();
    }

    /**
     * Gets the height of this TextEditor in pixels.
     *
     * @return height in pixels
     */
    public int getHeight() {
        return textWrapper.getHeight();
    }

    /**
     * Gets the size of the area needed for drawing the implementation provided
     * input indicators. The returned width and height must include all pixels
     * that are changed if an input indicator is drawn, and are returned in this
     * order. <AU> Size (0,0) is returned if device UI does not use input
     * indicators in text editors.</AU>
     *
     * @return the width and height of area needed for drawing input indicators
     */
    public int[] getIndicatorSize() {
        // TODO: implement this
        return null;
    }

    /**
     * Gets the line height in this TextEditor in pixels.
     *
     * @return the line height in pixels
     */
    public int getLineHeight() {
        return textWrapper.getLineHeight();
    }

    /**
     * Returns the maximum size (number of characters) that can be stored in
     * this <code>TextEditor</code>.
     *
     * @return the maximum size in characters
     */
    public int getMaxSize() {
        return textWrapper.getMaxSize();
    }

    /**
     * Gets the number of visible rows.
     *
     * @return the number of visible rows in the editor
     */
    public int getRows() {
        return textWrapper.getHeight() / textWrapper.getLineHeight();
    }

    /**
     * Gets the currently selected content in the TextEditor. A selection may be
     * set with setSelection method or by user interaction.
     *
     * @return the currently selected content
     */
    public String getSelection() {
        return textWrapper.getSelectedContent();
    }

    /**
     * Gets the width of this TextEditor in pixels.
     *
     * @return width in pixels
     */
    public int getWidth() {
        return textWrapper.getWidth();
    }

    /**
     * Gets the topmost pixel position of the topmost visible line in the
     * editor. The returned y coordinate value must be relative to the whole
     * content height, not just the visible part. This method can be used by the
     * application together with the getContentHeight, getLineHeight and
     * getCaretPosition methods in drawing custom visual cues like a scroll bar
     * or other content sensitive pop-ups.
     *
     * @return the topmost pixel position of the visible content
     */
    public int getVisibleContentPosition() {
        return textWrapper.getTopPixelPosition();
    }

    /**
     * Sets this TextEditor hidden. Hides the editor and its content. <AU>If the
     * editor is already hidden does nothing.</AU> If the application has
     * requested the implementation to draw input indicators, and they are
     * visible on the display, the indicators are made invisible by the call of
     * this method. <AU>
     *
     * @throws IllegalStateException If the TextEditor is not added to Canvas or
     *             CustomItem
     * @see #show() </AU>
     */
    public void hide() {
        textWrapper.setVisible(false);
    }

    /**
     * Inserts a string into the content of the <code>TextEditor</code>. The
     * string is inserted just prior to the character indicated by the
     * <code>position</code> parameter, where zero specifies the first
     * character of the content in the <code>TextEditor</code>. If
     * <code>position</code> is less than or equal to zero, the insertion
     * occurs at the beginning of the content. If <code>position</code> is
     * greater than or equal to the current size of the content, the insertion
     * occurs immediately after the end of the content. The current size of the
     * contents is increased by the number of inserted characters. The resulting
     * string must fit within the current maximum capacity.
     *
     * @param text the <code>String</code> to be inserted
     * @param position the position at which insertion is to occur
     * @throws IllegalArgumentException if the resulting content would be
     *             illegal for the current input constraints
     * @throws IllegalArgumentException if the insertion would exceed the
     *             current maximum capacity
     * @throws NullPointerException if <code>text</code> is <code>null</code>
     */
    public void insert(String text, int position) {
        textWrapper.insert(text, position);
    }

    /**
     * Removes the focus from this TextEditor. <AU> Calling
     * <code>removeFocus</code> disables text editing and returns the delivery
     * of key events and pointer events to the underlying Canvas. See method
     * setFocus for description on how focus affects key and pointer event
     * delivery. </AU> Removing focus does not cause any visual change by the
     * implementation other than removing the caret and, if visible, any input
     * indicators. Any other change in the visual appearance of the editor when
     * the focus is removed is the responsibility of the application; this can
     * be done for example by changing the background color or transparency of
     * the editor.
     *
     * @see #setFocus()
     */
    public void removeFocus() {
        textWrapper.setFocused(false);
    }

    /**
     * Sets the background color and alpha of this TextEditor to the specified
     * values. The set values apply also to any other related implementation
     * provided input visuals; for example to the implementation provided
     * additional character table or handwriting recognition panel.
     *
     * @param alpha the alpha component of the color being set in range
     *            <code>0-255</code>
     * @param red the red component of the color being set in range
     *            <code>0-255</code>
     * @param green the green component of the color being set in range
     *            <code>0-255</code>
     * @param blue the blue component of the color being set in range
     *            <code>0-255</code>
     * @throws IllegalArgumentException if any parameter is outside of the range
     *             <code>0-255</code>
     */
    public void setBackgroundColor(int alpha, int red, int green, int blue) {
        if ((alpha < 0 || alpha > BITSPERPIXEL8)
                || (red < 0 || red > BITSPERPIXEL8)
                || (green < 0 || green > BITSPERPIXEL8)
                || (blue < 0 || blue > BITSPERPIXEL8)) {
            throw new IllegalArgumentException();
        }
        textWrapper.setBackgroundColor(alpha, red, green, blue);
    }

    /**
     * Sets the index of the caret. The caret can be used to indicate a position
     * in the text.
     *
     * @param index the character index before which to place the caret
     */
    public void setCaret(int index) {
        textWrapper.setCaretposition(index);
    }

    /**
     * Sets the input constraints of this <code>TextEditor</code>. If the
     * current content of this <code>TextEditor</code> do not match the new
     * constraints, the content is set to empty.
     *
     * @param constraints see <a href="TextField.html#constraints">input
     *            constraints</a>
     * @throws IllegalArgumentException if the value of the constraints
     *             parameter is invalid
     */
    public void setConstraints(int constraints) {
        textWrapper.setConstraints(constraints);
    }

    /**
     * Sets the content of the <code>TextEditor</code> as a string. The set
     * string replaces any previous content in the editor.
     *
     * @param text the new content of the <code>TextEditor</code> as string,
     *            <code>null</code> empties the <code>TextEditor</code>
     * @throws IllegalArgumentException if <code>text</code> is illegal for
     *             the current input constraints
     * @throws IllegalArgumentException if the given text would exceed the
     *             current maximum capacity of the editor
     */
    public void setContent(String text) {
        textWrapper.setContent(text);
    }

    /**
     * Sets this TextEditor focused. <AU> Calling setFocus enables text editing
     * and consumes the delivery of needed key events or pointer events from the
     * underlying Canvas. Focus needs to be explicitly set to the TextEditor,
     * for example, based on Canvas or CustomItem key events or pointer events.
     * When Canvas or CustomItem does not have any focused TextEditor all key
     * and pointer events are delivered to normal Canvas key and pointer event
     * delivery methods. Necessary key and pointer events are captured by
     * TextEditor only when it has focus. The setFocus affects to the
     * Canvas/CustomItem pointer event delivery in following way: Unfocused
     * TextEditors do not consume pointer events and they are delivered to
     * Canvas/CustomItem. When user taps top of unfocused TextEditor the normal
     * pointer event methods of Canvas are called and application may choose
     * based on these to focus the TextEditor. If the editor is focused with
     * setFocus it starts to consume the pointer events targeted inside the
     * TextEditor area in the contained Canvas, however, taps outside the area
     * are delivered to Canvas and based on those events the application may
     * choose to unfocus the still focused TextEditor calling removeFocus(). The
     * setFocus affects to the Canvas/CustomItem key event delivery in following
     * way: If all TextEditors are unfocused the key event delivery of Canvas
     * works as if there isn't any TextEditors in it, only when a focus is set
     * via this method to a TextEditor the key event delivery is modified. The
     * focused editor starts to capture the necessary key events and these key
     * events are not delivered to containing Canvas/CustomItem. The set of key
     * events captured depends on TextEditor implementation and device HW but in
     * most implementations nearly all device keys are captured by the editor
     * for text insertion, input mode changes and caret move functionalities.
     * Especially applications should not assume to get key events from keys
     * mapped to game actions as most probably the same keys are used to
     * navigate the text caret within the editor. The keys that are used for
     * Command launching in Canvas are available for applications on focused
     * TextEditor and these keys either launch commands or send low-level key
     * events as normally. </AU> Setting focus does not cause any visual focus
     * indication by the implementation other than showing the caret and, if the
     * application has requested, drawing the input indicators to the requested
     * location on the display. Any other change in the visual appearance of the
     * editor in focused state is the responsibility of the application; this
     * can be done for example by drawing a focus border around the TextEditor
     * or by changing the background color or transparency of the editor. If
     * this TextEditor has already been focused earlier and the editor contents
     * has not changed after previous unfocusing, then after calling setFocus
     * again the caret position should be retained. If there already is another
     * focused TextEditor on the Canvas, the focus is first removed from that
     * TextEditor before setting this TextEditor focused. <AU>
     *
     * @throws IllegalStateException If the TextEditor is not added to Canvas or
     *             CustomItem
     * @see #removeFocus() </AU>
     */
    public void setFocus() {
        if (parent == null) {
            throw new IllegalStateException();
        }
        textWrapper.setFocused(true);
    }

    /**
     * Sets the application preferred font for rendering the text content in
     * this <code>TextEditor</code>. Setting the font is a hint to the
     * implementation, and the implementation may disregard the requested font.
     * The <code>font</code> parameter must be a valid <code>Font</code>
     * object or <code>null</code>. If the <code>font</code> parameter is
     * <code>null</code>, the implementation must use its default font to
     * render the text content.
     *
     * @param font the application preferred font to be used in this TextEditor
     */
    public void setFont(Font font) {
        textWrapper.setFont(font);
    }

    /**
     * Sets the foreground color and alpha of this TextEditor to the specified
     * values. The content, e.g. text, in the editor must be drawn with this
     * color.
     *
     * @param alpha the alpha component of the color being set in range
     *            <code>0-255</code>
     * @param red the red component of the color being set in range
     *            <code>0-255</code>
     * @param green the green component of the color being set in range
     *            <code>0-255</code>
     * @param blue the blue component of the color being set in range
     *            <code>0-255</code>
     * @throws IllegalArgumentException if any parameter is outside of the range
     *             <code>0-255</code>
     */
    public void setForegroundColor(int alpha, int red, int green, int blue) {
        if ((alpha < 0 || alpha > BITSPERPIXEL8)
                || (red < 0 || red > BITSPERPIXEL8)
                || (green < 0 || green > BITSPERPIXEL8)
                || (blue < 0 || blue > BITSPERPIXEL8)) {
            throw new IllegalArgumentException();
        }
        textWrapper.setForegroundColor(alpha, red, green, blue);
    }

    /**
     * Sets the drawing location for implementation provided input indicators on
     * the display. The anchor point given is relative to the upper left corner
     * of the rendering surface. The location may be outside the TextEditor
     * boundaries. The application should first query the size of the input
     * indicators with getIndicatorSize method to determine that the indicators
     * will not be clipped outside the available display area when drawn to the
     * requested location. <AU>If this method is not called no indicators are
     * present and usability of complex device specific input methods may be
     * compromised.</AU>
     *
     * @param x the x coordinate of the anchor point, in pixels.
     * @param y the y coordinate of the anchor point, in pixels.
     */
    public void setIndicatorLocation(int x, int y) {
        // TODO: implement this
    }

    /**
     * Sets a hint to the implementation as to the input mode that should be
     * used when the user initiates editing of this <code>TextEditor</code>.
     * The <code>characterSubset</code> parameter names a subset of Unicode
     * characters that is used by the implementation to choose an initial input
     * mode. If <code>null</code> is passed, the implementation should choose
     * a default input mode. See <a href="TextField#modes">Input Modes</a> for
     * a full explanation of input modes.
     *
     * @param inputMode a string naming a Unicode character subset, or
     *            <code>null</code>
     */
    public void setInitialInputMode(String inputMode) {
        textWrapper.setInputMode(inputMode);
    }

    /**
     * Sets the maximum size (number of characters) that can be contained in
     * this <code>TextEditor</code>. If the current content of the
     * <code>TextEditor</code> is larger than the new <code>maxSize</code>,
     * the content is truncated to fit.
     *
     * @param maxSize the new maximum size
     * @return assigned maximum capacity - may be smaller than requested.
     * @throws IllegalArgumentException if <code>maxSize</code> is zero or
     *             less.
     * @throws IllegalArgumentException if the contents after truncation would
     *             be illegal for the current input constraints
     */
    public int setMaxSize(int maxSize) {
        textWrapper.setMaxSize(maxSize);
        return textWrapper.getMaxSize();
    }

    /**
     * Sets the rendering position of this TextEditor. The anchor point given is
     * relative to the upper left corner of the rendering surface.
     *
     * @param x the x coordinate of the anchor point, in pixels.
     * @param y the y coordinate of the anchor point, in pixels.
     */
    public void setPosition(int x, int y) {
        textWrapper.setPosition(x, y);
    }

    /**
     * Sets the number of visible rows. <AU>The height of the TextField gets
     * modified depending the text row height.</AU>
     *
     * @param rows the number of visible rows in the editor
     * @throws IllegalArgumentException if rows is zero or less
     */
    public void setRows(int rows) {
        if (rows <= 0) {
            throw new IllegalArgumentException();
        }
        textWrapper.setSize(textWrapper.getWidth(),
                textWrapper.getLineHeight() * rows);
    }

    /**
     * Sets a selection on a range of text in the TextEditor content. The
     * implementation should highlight the selection visually. A selection may
     * be set with this method or by user interaction. If there already is a
     * selection set, it is replaced by this new selection.
     *
     * @param index the index of the first character to be selected.
     * @param length the length of the selection in characters.
     */
    public void setSelection(int index, int length) {
        textWrapper.setSelection(index, index + length);
    }

    /**
     * Sets the size of this TextEditor in pixels.
     *
     * @param width width in pixels
     * @param height height in pixels
     * @throws IllegalArgumentException if the width or height is less than zero
     */
    public void setSize(int width, int height) {
        textWrapper.setSize(width, height);
    }

    /**
     * Sets a listener for content changes in this <code>TextEditor</code>,
     * replacing any previous <code>TextEditorChangeListener</code>. A
     * <code>null</code> reference is allowed and has the effect of removing
     * any existing <code>TextEditorChangeListener</code> from this
     * <code>TextEditor</code>.
     *
     * @param aChangelistener the new listener, or <code>null</code>
     */
    public void setTextEditorChangeListener(
            TextEditorChangeListener aChangelistener) {
        changelistener = aChangelistener;
    }

    /**
     * Sets this TextEditor visible. Shows the editor with its content, but
     * without the caret. If the editor does not have any visible content and
     * does not have any background color set then this method effectively does
     * not cause any visual change in the display. If the application has
     * requested the implementation to draw input indicators, they are not made
     * visible by the call of this method. The indicators are made visible only
     * when actual editing is enabled by the call of setFocus() method. <AU>
     *
     * @throws IllegalStateException If the TextEditor is not added to Canvas or
     *             CustomItem
     * @see #hide() </AU>
     */
    public void show() {
        if (parent == null) {
            throw new IllegalStateException();
        }
        textWrapper.setVisible(true);
    }

    /**
     * Gets the number of characters that are currently stored in this
     * <code>TextEditor</code>.
     *
     * @return the number of characters
     */
    public int size() {
        return textWrapper.getSize();
    }

    /**
     * Text modify listener.
     */
    class TextModifyListener implements ModifyListener {

        public void modifyText(ModifyEvent me) {
            if (changelistener != null) {
                changelistener.textChanged(0, textWrapper.getSize());
            }
        }

    }

    /**
     * Text selection listener.
     */
    class TextSelectionListener implements SelectionListener {

        public void widgetDefaultSelected(SelectionEvent se) {
            Logger.verbose("TE - DefSel: " + se);
        }

        public void widgetSelected(SelectionEvent se) {
            Logger.verbose("TE - Sel: " + se);
        }

    }

}
