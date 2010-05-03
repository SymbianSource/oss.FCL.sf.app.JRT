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
package javax.microedition.lcdui;

import org.eclipse.ercp.swt.mobile.ConstrainedText;
import org.eclipse.ercp.swt.mobile.TextExtension;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.*;

/**
 * Dynamic Text component implementation over the TextExtension/ConstrainedText
 * widgets as a wrapper.
 */
class TextWrapper {

    private String text;
    private String inputMode;
    private int constraints;
    private int caret;
    private int size;
    private int maxSize;

    private Control control; // owned
    private int style;
    private int width = -1;
    private int height = -1;
    private int x = -1;
    private int y = -1;
    private Color fgCol; // owned
    private Color bgCol; // owned
    private boolean visible = true;
    private boolean focused = true;

    private Font font;   // not owned
    private ModifyListener modifyListener; // not owned
    private SelectionListener selectionListener; // not owned

    private String retSelectedStr;
    private int retLineHeight;
    private int retLineCount;
    private int retTopPixel;

    /**
     * Constructor.
     *
     * @param aText
     * @param aMaxSize
     * @param aConstraints
     */
    TextWrapper(String aText, int aMaxSize, int aConstraints) {
        setMaxSize(aMaxSize);
        setConstraints(aConstraints);
        setContent(aText);
    }

    /**
     * Construct eSWT control on specified composite.
     *
     * @param parent composite to create on
     * @param addStyle additional style
     */
    void construct(final Composite parent, final int addStyle) {
        ESWTUIThreadRunner.safeSyncExec(new Runnable() {
            public void run() {
                eswtConstruct(parent, addStyle);
            }
        });
    }

    /**
     * Construct eSWT control on specified composite.
     *
     * @param parent composite to create on
     * @param addStyle additional style
     */
    void eswtConstruct(Composite parent, int addStyle) {
        style = addStyle | SWT.MULTI | SWT.WRAP;
        if (parent != null) {
            eswtStoreStateAndDispose();
            eswtRestoreStateAndCreate(parent);
        }
        else {
            Logger.warning("Trying to construct TextWrapper with null parent");
        }
    }

    /**
     * Delete subset of characters from content.
     *
     * @param offset
     * @param length
     */
    void delete(int offset, int length) {
        if ((offset + length) > getSize()) {
            throw new StringIndexOutOfBoundsException(
                    MsgRepository.TEXT_EXCEPTION_ARRAY_INDEX_OUT_OF_BOUNDS);
        }
        StringBuffer sb = new StringBuffer(getContent());
        sb.delete(offset, offset + length);
        setContent(sb.toString());
    }

    /**
     * Dispose eSWT control.
     */
    void dispose() {
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    eswtStoreStateAndDispose();
                }
            });
        }
    }

    /**
     * Get caret position.
     */
    int getCaretPosition() {
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    caret = eswtGetCaretPosition(control);
                }
            });
        }
        return caret;
    }

    /**
     * Get constraints.
     */
    int getConstraints() {
        return constraints;
    }

    /**
     * Get text content.
     */
    String getContent() {
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    text = eswtGetContent(control);
                }
            });
        }
        return text;
    }

    /**
     * Gets the height of this Text control in pixels.
     */
    int getHeight() {
        return height;
    }

    /**
     * Get input mode.
     */
    String getInputMode() {
        return inputMode;
    }

    /**
     * Get line count.
     */
    int getLineCount() {
        retLineCount = 1;
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    retLineCount = eswtGetLineCount(control);
                }
            });
        }
        return retLineCount;
    }

    /**
     * Get line height (in pixels).
     */
    int getLineHeight() {
        retLineHeight = 1;
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    retLineHeight = eswtGetLineHeight(control);
                }
            });
        }
        return retLineHeight;
    }

    /**
     * Get maximum size (in characters).
     */
    int getMaxSize() {
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    maxSize = eswtGetMaxSize(control);
                }
            });
        }
        return maxSize;
    }

    /**
     * Get the preferred height (in pixels) of the text control.
     *
     * @param maxVisibleLines maximum visible lines
     */
    int getPreferredHeight(int maxVisibleLines) {
        // lineCount (1 .. max) * lineHeight
        return Math.min(Math.max(1, getLineCount()), maxVisibleLines)
                * getLineHeight();
    }

    /**
     * Get selected text content.
     */
    String getSelectedContent() {
        retSelectedStr = "";
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    retSelectedStr = eswtGetSelectedContent(control);
                }
            });
        }
        return retSelectedStr;
    }

    /**
     * Get content's length.
     */
    int getSize() {
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    size = eswtGetSize(control);
                }
            });
        }
        return size;
    }

    /**
     * Get visible top pixel position relative to whole content.
     */
    int getTopPixelPosition() {
        retTopPixel = 0;
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    if (control instanceof TextExtension) {
                        retTopPixel = ((TextExtension) control).getTopPixel();
                    }
                    // ConstrainedText does not scroll -> value is 0
                }
            });
        }
        return retTopPixel;
    }

    /**
     * Gets the width of this Text control (in pixels).
     */
    int getWidth() {
        return width;
    }

    /**
     * Insert text content.
     *
     * @param aText text
     * @param aPosition position where to instert
     */
    void insert(String aText, int aPosition) {
        if (aText == null) {
            throw new NullPointerException(
                    MsgRepository.TEXT_EXCEPTION_TXT_IS_NULL);
        }
        StringBuffer sb = new StringBuffer(getContent());
        if (aPosition < 0) {
            sb.insert(0, aText);
        }
        else if (aPosition > sb.length()) {
            sb.append(aText);
        }
        else {
            sb.insert(aPosition, aText);
        }
        setContent(sb.toString());
    }

    /**
     * Set background color.
     *
     * @param alpha alpha component (currently not used)
     * @param red color component
     * @param green color component
     * @param blue color component
     */
    void setBackgroundColor(final int alpha, final int red, final int green, final int blue) {
        ESWTUIThreadRunner.safeSyncExec(new Runnable() {
            public void run() {
                if (bgCol != null) {
                    bgCol.dispose();
                    bgCol = null;
                }
                bgCol = new Color(ESWTUIThreadRunner.getInstance().getDisplay(),
                        red, green, blue);
            }
        });
    }

    /**
     * Set the bounds of the Text control.
     *
     * @param aBounds bounding rectangle
     */
    void setBounds(Rectangle aBounds) {
        if (aBounds.width < 0 || aBounds.height < 0) {
            throw new IllegalArgumentException(
                    MsgRepository.TEXT_EXCEPTION_INVALID_SIZE);
        }
        x = aBounds.x;
        y = aBounds.y;
        width = aBounds.width;
        height = aBounds.height;
        if (control != null && width >= 0 && height >= 0) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    control.setBounds(x, y, width, height);
                }
            });
        }
    }

    /**
     * Set caret position.
     *
     * @param aPosition position
     */
    void setCaretposition(int aPosition) {
        caret = aPosition;
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    eswtSetSelection(control, caret, caret);
                }
            });
        }
    }

    /**
     * Set constraints. Note: this might reconstruct the eSWT text control.
     *
     * @param aConstraints text input constraints
     */
    void setConstraints(int aConstraints) {
        if (!isValidConstraints(aConstraints)) {
            throw new IllegalArgumentException(
                    MsgRepository.TEXT_EXCEPTION_INVALID_CONSTRAINTS);
        }
        constraints = aConstraints;
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    eswtRestoreStateAndCreate(eswtStoreStateAndDispose());
                }
            });
        }
    }

    /**
     * Set text content.
     *
     * @param aText new content
     */
    void setContent(String aText) {
        if (aText == null) {
            text = "";
        }
        else {
            if (aText.length() > maxSize) {
                throw new IllegalArgumentException(
                        MsgRepository.TEXT_EXCEPTION_MAX_SIZE_EXCEEDED);
            }
            if (!isValidText(aText, constraints)) {
                throw new IllegalArgumentException(
                        MsgRepository.TEXT_EXCEPTION_WRONG_TEXT);
            }
            text = aText;
        }
        size = text.length();
        caret = text.length();
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    eswtSetContent(control, text);
                    eswtSetSelection(control, caret, caret);
                }
            });
        }
    }

    /**
     * Set text font.
     *
     * @param aFont new font
     */
    void setFont(Font aFont) {
        font = aFont;
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    if (font != null) {
                        // set custom font
                        control.setFont(Font.getESWTFont(font));
                    }
                    else {
                        // set default font
                        control.setFont(
                                Font.getESWTFont(Font.getDefaultFont()));
                    }
                }
            });
        }
    }

    /**
     * Set this Text control focused.
     *
     * @param aFocused
     */
    void setFocused(boolean aFocused) {
        focused = aFocused;
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    eswtSetFocused(control, focused);
                }
            });
        }
    }

    /**
     * Set foreground color.
     *
     * @param alpha alpha component (currently not used)
     * @param red color component
     * @param green color component
     * @param blue color component
     */
    void setForegroundColor(final int alpha, final int red, final int green, final int blue) {
        ESWTUIThreadRunner.safeSyncExec(new Runnable() {
            public void run() {
                if (fgCol != null) {
                    fgCol.dispose();
                    fgCol = null;
                }
                fgCol = new Color(ESWTUIThreadRunner.getInstance().getDisplay(),
                        red, green, blue);
            }
        });
    }

    /**
     * Set input mode.
     *
     * @param inputMode
     */
    void setInputMode(String aInputMode) {
        inputMode = aInputMode;
        if (control != null && control instanceof TextExtension) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    eswtSetInputMode(control, inputMode, constraints);
                }
            });
        }
    }

    /**
     * Set maximum size (in characters).
     *
     * @param aMaxSize
     */
    void setMaxSize(int aMaxSize) {
        if (aMaxSize < 1) {
            throw new IllegalArgumentException(
                    MsgRepository.TEXT_EXCEPTION_INVALID_MAX_SIZE);
        }

        String content = getContent();
        if (content != null && aMaxSize < content.length()) {
            // we have to truncate content - validates the new text
            setContent(content.substring(0, aMaxSize));
        }

        maxSize = aMaxSize;
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    eswtSetMaxSize(control, maxSize);
                }
            });
        }
    }

    /**
     * Set the modify listener;
     *
     * @param aListener modify listener
     */
    void setModifyListener(final ModifyListener aListener) {
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    eswtRemoveModListener(control, modifyListener);
                    eswtAddModListener(control, aListener);
                }
            });
        }
        modifyListener = aListener;
    }

    /**
     * Set the selection listener.
     *
     * @param aListener selection listener
     */
    void setSelectionListener(final SelectionListener aListener) {
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    eswtRemoveSelListener(control, selectionListener);
                    eswtAddSelListener(control, aListener);
                }
            });
        }
        selectionListener = aListener;
    }

    /**
     * Set the location of the eSWT control.
     *
     * @param aX
     * @param aY
     */
    void setPosition(int aX, int aY) {
        x = aX;
        y = aY;
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    control.setLocation(x, y);
                }
            });
        }
    }

    /**
     * Set the selection in the eSWT text control.
     *
     * @param sta selection start
     * @param end selection end
     */
    void setSelection(final int sta, final int end) {
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    eswtSetSelection(control, sta, end);
                }
            });
        }
    }

    /**
     * Set the size of the eSWT control.
     *
     * @param width
     * @param height
     */
    void setSize(int aWidth, int aHeight) {
        if (aWidth < 0 || aHeight < 0) {
            throw new IllegalArgumentException(
                    MsgRepository.TEXT_EXCEPTION_INVALID_SIZE);
        }
        width = aWidth;
        height = aHeight;
        if (control != null && width >= 0 && height >= 0) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    control.setSize(width, height);
                }
            });
        }
    }

    /**
     * Set the visibility of the eSWT control.
     */
    void setVisible(boolean aVisible) {
        visible = aVisible;
        if (control != null) {
            ESWTUIThreadRunner.safeSyncExec(new Runnable() {
                public void run() {
                    control.setVisible(visible);
                }
            });
        }
    }

    /**
     * Restores a stored state and creates eSWT text control.
     *
     * @param parent parent composite
     */
    private void eswtRestoreStateAndCreate(Composite parent) {
        if (control == null) {
            control = eswtConstructText(parent, style, constraints);
            eswtSetInputMode(control, inputMode, constraints);
            eswtSetMaxSize(control, maxSize);
            eswtSetContent(control, text);
            eswtSetSelection(control, caret, caret);
            if (width >= 0 && height >= 0) {
                control.setBounds(x, y, width, height);
            }
            if (fgCol != null) {
                control.setForeground(fgCol);
            }
            if (bgCol != null) {
                control.setBackground(bgCol);
            }
            if (font != null) {
                control.setFont(Font.getESWTFont(font));
            }
            control.setVisible(visible);
            eswtSetFocused(control, focused);
            eswtAddModListener(control, modifyListener);
            eswtAddSelListener(control, selectionListener);
        }
    }

    /**
     * Stores the current state and disposes the eSWT text control.
     *
     * @return the control's parent composite.
     */
    private Composite eswtStoreStateAndDispose() {
        Composite parent = null;
        if (control != null) {
            parent = control.getParent();
            text = eswtGetContent(control);
            caret = eswtGetCaretPosition(control);
            eswtRemoveModListener(control, modifyListener);
            eswtRemoveSelListener(control, selectionListener);
            control.dispose();
            control = null;
        }
        return parent;
    }

    /**
     * Add modify listener.
     *
     * @param control text control
     * @param ltnr listener
     */
    static void eswtAddModListener(Control control, ModifyListener ltnr) {
        if (control != null && ltnr != null) {
            if (control instanceof TextExtension) {
                ((TextExtension) control).addModifyListener(ltnr);
            }
            else {
                ((ConstrainedText) control).addModifyListener(ltnr);
            }
        }
    }

    /**
     * Add selection listener.
     *
     * @param control text control
     * @param ltnr listener
     */
    static void eswtAddSelListener(Control control, SelectionListener ltnr) {
        if (control != null && ltnr != null) {
            if (control instanceof TextExtension) {
                ((TextExtension) control).addSelectionListener(ltnr);
            }
            else {
                ((ConstrainedText) control).addSelectionListener(ltnr);
            }
        }
    }

    /**
     * Construct text widget with given parameters.
     *
     * @param parent
     * @param aStyle
     * @param aConstraints
     * @return
     */
    static Control eswtConstructText(Composite parent, int aStyle, int aConstraints) {
        Control ret = null;

        int style = aStyle;
        int extractedFlag = aConstraints & ~TextField.CONSTRAINT_MASK;
        int extractedConstraint = aConstraints & TextField.CONSTRAINT_MASK;

        if ((extractedFlag & TextField.PASSWORD) == TextField.PASSWORD) {
            // Text class will remove incompatible flags for SINGLE
            style |= SWT.SINGLE | SWT.PASSWORD;
        }
        if ((extractedFlag & TextField.UNEDITABLE) == TextField.UNEDITABLE) {
            style |= SWT.READ_ONLY;
        }

        if (extractedConstraint == TextField.NUMERIC) {
            ret = new ConstrainedText(parent, style, ConstrainedText.NUMERIC);
        }
        else if (extractedConstraint == TextField.DECIMAL) {
            ret = new ConstrainedText(parent, style, ConstrainedText.DECIMAL);
        }
        else if (extractedConstraint == TextField.PHONENUMBER) {
            ret = new ConstrainedText(parent, style, ConstrainedText.PHONENUMBER);
        }
        else {
            ret = new TextExtension(parent, style);
        }
        return ret;
    }

    /**
     * Get caret position.
     *
     * @param control text control
     */
    static int eswtGetCaretPosition(Control control) {
        int ret = 0;
        if (control != null) {
            if (control instanceof TextExtension) {
                ret = ((TextExtension) control).getCaretPosition();
            }
            else {
                ret = ((ConstrainedText) control).getCaretPosition();
            }
        }
        return ret;
    }

    /**
     * Get caret line number.
     *
     * @param control text control
     */
    static int eswtGetCaretLine(Control control) {
        int ret = 0;
        if (control != null) {
            if (control instanceof TextExtension) {
                ret = ((TextExtension) control).getCaretLineNumber();
            }
        }
        return ret;
    }

    /**
     * Get content.
     *
     * @param control text control
     */
    static String eswtGetContent(Control control) {
        String ret = "";
        if (control != null) {
            if (control instanceof TextExtension) {
                ret = ((TextExtension) control).getText();
            }
            else {
                ret = ((ConstrainedText) control).getText();
            }
        }
        return ret;
    }

    /**
     * Get line count.
     *
     * @param control text control
     */
    static int eswtGetLineCount(Control control) {
        int ret = 1;
        if (control != null) {
            if (control instanceof TextExtension) {
                ret = ((TextExtension) control).getLineCount();
            }
        }
        return ret;
    }

    /**
     * Get line height.
     *
     * @param control text control
     */
    static int eswtGetLineHeight(Control control) {
        int ret = 0;
        if (control != null) {
            if (control instanceof TextExtension) {
                ret = ((TextExtension) control).getLineHeight();
            }
            else {
                ret = ((ConstrainedText) control).getSize().y;
            }
        }
        return ret;
    }

    /**
     * Get maximum content size.
     *
     * @param control text control
     */
    static int eswtGetMaxSize(Control control) {
        int ret = 0;
        if (control != null) {
            if (control instanceof TextExtension) {
                ret = ((TextExtension) control).getTextLimit();
            }
            else {
                ret = ((ConstrainedText) control).getTextLimit();
            }
        }
        return ret;
    }

    /**
     * Get selected content.
     *
     * @param control text control
     */
    static String eswtGetSelectedContent(Control control) {
        String ret = "";
        if (control != null) {
            if (control instanceof TextExtension) {
                ret = ((TextExtension) control).getSelectionText();
            }
            else {
                // TODO: eSWT support required - get selection in ConstrainedText
                ret = ((ConstrainedText) control).getText();
            }
        }
        return ret;
    }

    /**
     * Get content size.
     *
     * @param control text control
     */
    static int eswtGetSize(Control control) {
        int ret = 0;
        if (control != null) {
            if (control instanceof TextExtension) {
                ret = ((TextExtension) control).getCharCount();
            }
            else {
                ret = ((ConstrainedText) control).getCharCount();
            }
        }
        return ret;
    }

    /**
     * Remove modify listener.
     *
     * @param control text control
     * @param ltnr listener
     */
    static void eswtRemoveModListener(Control control, ModifyListener ltnr) {
        if (control != null && ltnr != null) {
            if (control instanceof TextExtension) {
                ((TextExtension) control).removeModifyListener(ltnr);
            }
            else {
                ((ConstrainedText) control).removeModifyListener(ltnr);
            }
        }
    }

    /**
     * Remove modify listener.
     *
     * @param control text control
     * @param ltnr listener
     */
    static void eswtRemoveSelListener(Control control, SelectionListener ltnr) {
        if (control != null && ltnr != null) {
            if (control instanceof TextExtension) {
                ((TextExtension) control).removeSelectionListener(ltnr);
            }
            else {
                ((ConstrainedText) control).removeSelectionListener(ltnr);
            }
        }
    }

    /**
     * Set content.
     *
     * @param control text control
     * @param text content
     */
    static void eswtSetContent(Control control, String text) {
        if (control != null) {
            if (control instanceof TextExtension) {
                ((TextExtension) control).setText(text);
            }
            else {
                ((ConstrainedText) control).setText(text);
            }
        }
    }

    /**
     * Set text control focus on/off.
     *
     * @param control text control
     * @param focus
     */
    static void eswtSetFocused(Control control, boolean focus) {
        if (control != null) {
            if (focus) {
                control.setFocus();
            }
            else {
                control.getParent().forceFocus();
            }
        }
    }

    /**
     * Set input mode.
     *
     * @param control text control
     * @param inputMode input mode
     * @param aConstraints constraints
     */
    static void eswtSetInputMode(Control control, String inputMode,
            int aConstraints) {
        if (control != null && control instanceof TextExtension) {
            TextExtension te = (TextExtension) control;
            int capitalize = getCapitalize(aConstraints);
            if (inputMode == null) {
                te.setInitialInputMode(TextExtension.TEXTCASE,
                        "UCB_BASIC_LATIN");
            }
            else if (inputMode.equals("MIDP_UPPERCASE_LATIN")) {
                if (capitalize > 0 && !isUrlEmailSet(aConstraints)) {
                    te.setInitialInputMode(capitalize, "UCB_BASIC_LATIN");
                }
                else {
                    te.setInitialInputMode(TextExtension.UPPERCASE,
                            "UCB_BASIC_LATIN");
                }
            }
            else if (inputMode.equals("MIDP_LOWERCASE_LATIN")) {
                if (capitalize > 0 && !isUrlEmailSet(aConstraints)) {
                    te.setInitialInputMode(capitalize, "UCB_BASIC_LATIN");
                }
                else {
                    te.setInitialInputMode(TextExtension.LOWERCASE,
                            "UCB_BASIC_LATIN");
                }
            }
            else {
                te.setInitialInputMode(TextExtension.TEXTCASE, inputMode);
            }
        }
    }

    /**
     * Set maximum size.
     *
     * @param control text control
     * @param maxSize maximum size
     */
    static void eswtSetMaxSize(Control control, int maxSize) {
        if (control != null) {
            if (control instanceof TextExtension) {
                ((TextExtension) control).setTextLimit(maxSize);
            }
            else {
                ((ConstrainedText) control).setTextLimit(maxSize);
            }
        }
    }

    /**
     * Set selection.
     *
     * @param control text control
     * @param sta start index
     * @param end end index
     */
    static void eswtSetSelection(Control control, int sta, int end) {
        if (control != null) {
            if (control instanceof TextExtension) {
                ((TextExtension) control).setSelection(sta, end);
            }
            else {
                ((ConstrainedText) control).setSelection(sta, end);
            }
        }
    }

    /**
     * Update vertical scroll bar.
     *
     * @param control text control
     */
    static void eswtUpdateVScrollbar(Control control) {
        if (control != null) {
            if ((control.getStyle() & SWT.V_SCROLL) == SWT.V_SCROLL) {
                if (control instanceof TextExtension) {
                    TextExtension te = (TextExtension) control;
                    ScrollBar sb = te.getVerticalBar();
                    if (sb != null) {
                        int height = te.getLineCount() * te.getLineHeight();
                        sb.setVisible(te.getSize().y < height);
                    }
                }
            }
        }
    }

    /**
     * Get eSWT capitalize style flags for TextExtension.
     *
     * @param aConstraints constraints
     */
    static int getCapitalize(int aConstraints) {
        int ret = 0;
        int extractedFlag = aConstraints & ~TextField.CONSTRAINT_MASK;
        if ((extractedFlag & TextField.INITIAL_CAPS_WORD)
                == TextField.INITIAL_CAPS_WORD) {
            ret = TextExtension.TITLECASE;
        }
        if ((extractedFlag & TextField.INITIAL_CAPS_SENTENCE)
                == TextField.INITIAL_CAPS_SENTENCE) {
            ret = TextExtension.TEXTCASE;
        }
        return ret;
    }

    /**
     * Is Url or Email constraints flags set.
     *
     * @param aConstraints constraints
     */
    static boolean isUrlEmailSet(int aConstraints) {
        int extractedConstraint = aConstraints & TextField.CONSTRAINT_MASK;
        return (extractedConstraint == TextField.EMAILADDR
                || extractedConstraint == TextField.URL);
    }

    /**
     * Validates input constraints.
     *
     * @param aConstraints constraints to check.
     * @return true if constraints are OK, false otherwise.
     */
    static boolean isValidConstraints(int aConstraints) {
        int smallestFlag;
        int highestFlag;
        smallestFlag = TextField.PASSWORD - 1;
        highestFlag = TextField.PASSWORD + TextField.UNEDITABLE
                + TextField.SENSITIVE + TextField.NON_PREDICTIVE
                + TextField.INITIAL_CAPS_WORD + TextField.INITIAL_CAPS_SENTENCE
                + 1;

        int typeConstraint = getTypeConstraint(aConstraints);
        if (typeConstraint == TextField.ANY
                || typeConstraint == TextField.EMAILADDR
                || typeConstraint == TextField.NUMERIC
                || typeConstraint == TextField.PHONENUMBER
                || typeConstraint == TextField.DECIMAL
                || typeConstraint == TextField.URL) {
            int extractedFlag = aConstraints & ~TextField.CONSTRAINT_MASK;
            if (extractedFlag == 0
                    || ((extractedFlag > smallestFlag)
                         && (extractedFlag < highestFlag))) {
                return true;
            }
        }
        return false;
    }

    /**
     * Get constraint type.
     *
     * @param aConstraints constraints
     */
    static int getTypeConstraint(int aConstraints) {
        return aConstraints & TextField.CONSTRAINT_MASK;
    }

    /**
     * Checks if the text meets the requirements from constraint.
     *
     * @param aText
     * @return
     */
    static boolean isValidText(String aText, int aConstraints) {
        return TextFieldLayouter.checkText(aConstraints, aText);
    }

}
