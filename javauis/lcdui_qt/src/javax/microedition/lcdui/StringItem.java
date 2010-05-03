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

import org.eclipse.swt.graphics.Point;

/**
 * Item representing a non-editable string.
 */
public class StringItem extends Item {

    private String text;
    private int appearanceMode;
    private Font font;

    /**
     * Constructor, creates a new StringItem with appearance mode PLAIN.
     *
     * @param label Label of StringItem.
     * @param txt Text of StringItem.
     */
    public StringItem(String label, String txt) {
        this(label, txt, Item.PLAIN);
    }

    /**
     * Constructor.
     *
     * @param label Label of the item, or null if no label.
     * @param txt Textual content of the item, or null if no initial textual
     *            content.
     * @param appMode Appearance mode of this item. This must be one of the
     *            appearance modes defined in Item-class.
     * @throws IllegalArgumentException If appearance mode is not one defined in
     *             Item-class.
     */
    public StringItem(String label, String txt, int appMode) {
        if ((appMode != Item.PLAIN) && (appMode != Item.BUTTON)
                && (appMode != Item.HYPERLINK)) {
            throw new IllegalArgumentException(
                    MsgRepository.STRINGITEM_EXCEPTION_INVALID_APPMODE);
        }

        appearanceMode = appMode;
        setLabel(label);
        setText(txt);
        font = Font.getDefaultFont();
    }

    /**
     * Gets the textual content of this item.
     *
     * @return Text of the item. Could return null if the text is empty.
     */
    public String getText() {
        return text;
    }

    /**
     * Sets the text of this item.
     *
     * @param newTxt Text to be set. If null, text is set to be empty.
     */
    public void setText(String newTxt) {
        if ((newTxt == null) || (newTxt.equals(""))) {
            text = null;
        }
        else {
            text = newTxt;
        }
        updateParent(UPDATE_SIZE_CHANGED);
    }

    /**
     * Gets current appearance mode.
     *
     * @return The one of appearance modes defined in Item-class.
     */
    public int getAppearanceMode() {
        return appearanceMode;
    }

    /**
     * Gives hint which font this item should use. System may ignore this
     * request.
     *
     * @param f Font to be used. If null, system will use default font for this
     *            item.
     */
    public void setFont(Font f) {
        font = (f == null ? Font.getDefaultFont() : f);
        updateParent(UPDATE_SIZE_CHANGED);
    }

    /**
     * Gets font that is set by calling setFont()-method. Note that returned
     * font is not necessary the font used with this item.
     *
     * @return Font set by setFont()-method.
     */
    public Font getFont() {
        return font;
    }

    /* (non-Javadoc)
     * @see midp3javax.microedition.lcdui.Item#isFocusable()
     */
    boolean isFocusable() {
        return (getNumCommands() > 0);
    }

    /**
     * Calculates minimum size of this item.
     *
     * @return Minimum size.
     */
    Point calculateMinimumSize() {
        return StringItemLayouter.calculateMinimumBounds(this);
    }

    /**
     * Calculates preferred size of this item.
     *
     * @return Preferred size.
     */
    Point calculatePreferredSize() {
        return StringItemLayouter.calculatePreferredBounds(this);
    }

    /**
     * Return layout with optional custom flags.
     *
     * @return layout directive
     */
    int internalGetLayout() {
        if (hasLabel() && !isSizeLocked()) {
            return super.internalGetLayout() | Item.LAYOUT_NEWLINE_BEFORE;
        }
        else {
            return super.internalGetLayout();
        }
    }

}
