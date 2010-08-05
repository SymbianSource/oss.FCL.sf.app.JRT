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
 * Item representing an editable TextField.
 */
public class TextField extends Item
{
    /**
     * Constant value shows that any text is allowed.
     */
    public static final int ANY = 0;
    /**
     * Constant value shows that email address is allowed.
     */
    public static final int EMAILADDR = 1;
    /**
     * Constant value shows that numeric values allowed.
     */
    public static final int NUMERIC = 2;
    /**
     * Constant value shows that phone number format is allowed.
     */
    public static final int PHONENUMBER = 3;
    /**
     * Constant value shows that URL is allowed.
     */
    public static final int URL = 4;
    /**
     * Constant value shows that decimal values allowed.
     */
    public static final int DECIMAL = 5;
    /**
     * Constant value shows that password is allowed.
     */
    public static final int PASSWORD = 65536; // 0x10000
    /**
     * Constant value shows that user is not to allow modify text.
     */
    public static final int UNEDITABLE = 131072; // 0x20000
    /**
     * Constant value shows that sensitive text is allowed.
     */
    public static final int SENSITIVE = 262144; // 0x40000
    /**
     * Constant value shows that input text is most likely not in predictive
     * dictionary.
     */
    public static final int NON_PREDICTIVE = 524288; // 0x80000
    /**
     * Constant value shows that every word will start from capital letter.
     */
    public static final int INITIAL_CAPS_WORD = 1048576; // 0x100000
    /**
     * Constant value shows that every sentence will start with Cap.
     */
    public static final int INITIAL_CAPS_SENTENCE = 2097152; // 0x200000
    /**
     * the mask to detect constraint mode.
     */
    public static final int CONSTRAINT_MASK = 65535; // 0xFFFF

    /**
     * If TextField is changed, reasons for Re-layouting.
     */
	static final int UPDATE_REASON_DELETE = UPDATE_ITEM_MAX << 1;
	static final int UPDATE_REASON_INSERT = UPDATE_ITEM_MAX << 2;
	static final int UPDATE_CHARS = UPDATE_ITEM_MAX << 3;
	static final int UPDATE_CONSTRAINTS = UPDATE_ITEM_MAX << 4;
	static final int UPDATE_INITIALINPUTMODE = UPDATE_ITEM_MAX << 5;
	static final int UPDATE_MAXSIZE = UPDATE_ITEM_MAX << 6;
	static final int UPDATE_STRING = UPDATE_ITEM_MAX << 7;


    private TextWrapper textWrapper;

    private int linesCount = 1;
    private int maxVisibleLines = 1;

    /**
     * Creates new TextField item.
     *
     * @param header label of the TextField.
     * @param txt initial text.
     * @param maxSize maximum number of characters allowed.
     * @param constraints TextField constraints.
     */
    public TextField(String header, String txt, int maxSize, int constraints)
    {
        setLabel(header);
        textWrapper = new TextWrapper(txt, maxSize, constraints);
    }

    /**
     * Get current caret's position.
     *
     * @return index in TextField where caret is.
     */
    public int getCaretPosition()
    {
        return textWrapper.getCaretPosition();
    }

    /**
     * Get content of TextField.
     *
     * @return String which represents content of TextField;
     */
    public String getString()
    {
        return textWrapper.getContent();
    }

    /**
     * Set text into TextField, replacing previous content.
     *
     * @param newTxt text to set.
     */
    public void setString(String newTxt)
    {
        textWrapper.setContent(newTxt);
        linesCount = 0;
        updateParent(UPDATE_STRING);
    }

    /**
     * Copy content of TextField into array charaData.
     *
     * @param charData array where to copy.
     * @return number of copied characters.
     */
    public int getChars(char[] charData)
    {
        if(charData == null)
        {
            throw new NullPointerException(
                MsgRepository.TEXT_EXCEPTION_ARRAY_IS_NULL);
        }
        if(charData.length < getString().length())
        {
            throw new ArrayIndexOutOfBoundsException(
                MsgRepository.TEXT_EXCEPTION_ARRAY_IS_TOO_SHORT);
        }
        String content = textWrapper.getContent();
        content.getChars(0, content.length(), charData, 0);
        return content.length();
    }

    /**
     * Set content of charData into TextField.
     *
     * @param charData array of chars to set.
     * @param offset index in charData to start copy from.
     * @param length number of character to copy into TextField.
     */
    public void setChars(char[] charData, int offset, int length)
    {
        String extractedString = null;
        if(charData != null)
        {
            try
            {
                extractedString = new String(charData, offset, length);
            }
            catch(IndexOutOfBoundsException e)
            {
                throw new ArrayIndexOutOfBoundsException(
                    MsgRepository.TEXT_EXCEPTION_ARRAY_INDEX_OUT_OF_BOUNDS);
            }
        }
        textWrapper.setContent(extractedString);
        linesCount = 0;
        updateParent(UPDATE_CHARS);
    }

    /**
     * Insert text into content of TextField.
     *
     * @param newTxt string to insert.
     * @param position where to insert.
     */
    public void insert(String newTxt, int position)
    {
        textWrapper.insert(newTxt, position);
        linesCount = 0;
        updateParent(UPDATE_REASON_INSERT);
    }

    /**
     * Insert into TextField subset of characters from char[] array.
     *
     * @param charData array of characters where to copy from.
     * @param offset start index in charData[].
     * @param length number of characters to copy.
     * @param position index in TextField where to insert to.
     */
    public void insert(char[] charData, int offset, int length, int position)
    {
        if(charData == null)
        {
            throw new NullPointerException(
                MsgRepository.TEXT_EXCEPTION_ARRAY_IS_NULL);
        }
        String extractedString = null;
        try
        {
            extractedString = new String(charData, offset, length);
        }
        catch(IndexOutOfBoundsException e)
        {
            throw new ArrayIndexOutOfBoundsException(
                MsgRepository.TEXT_EXCEPTION_ARRAY_INDEX_OUT_OF_BOUNDS);
        }
        textWrapper.insert(extractedString, position);
        linesCount = 0;
        updateParent(UPDATE_REASON_INSERT);
    }

    /**
     * Delete subset of characters from TextField.
     *
     * @param offset start index where to start deletion.
     * @param length number of characters to delete.
     */
    public void delete(int offset, int length)
    {
        textWrapper.delete(offset, length);
        linesCount = 0;
        updateParent(UPDATE_REASON_DELETE);
    }

    /**
     * Get number of characters allowed for input.
     *
     * @return number of characters which TextField can contain.
     */
    public int getMaxSize()
    {
        return textWrapper.getMaxSize();
    }

    /**
     * Set the maximum size of TextField.
     *
     * @param newMaxSize size to set.
     * @return max size that was set, depending on implementation may be smaller
     *         then requested.
     */
    public int setMaxSize(int newMaxSize)
    {
        textWrapper.setMaxSize(newMaxSize);
        linesCount = 0;
        updateParent(UPDATE_MAXSIZE);
        return textWrapper.getMaxSize();
    }

    /**
     * Get number of characters stored in TextField.
     *
     * @return current content length.
     */
    public int size()
    {
        return textWrapper.getSize();
    }

    /**
     * Set constraints for TextField.
     *
     * @param newConstraints constraints to set.
     */
    public void setConstraints(int newConstraints)
    {
        textWrapper.setConstraints(newConstraints);
        updateParent(UPDATE_CONSTRAINTS);
        if(!textWrapper.isValidText(getString() , textWrapper.getTypeConstraint(newConstraints)))
    	          setString("");
        updateParent(UPDATE_STRING | UPDATE_SIZE_CHANGED);
    }

    /**
     * Get current constraints active in TextField.
     *
     * @return current constraints.
     */
    public int getConstraints()
    {
        return textWrapper.getConstraints();
    }

    /**
     * Set initial input mode for TextField.
     *
     * @param inputMode input mode to set.
     */
    public void setInitialInputMode(String inputMode)
    {
        textWrapper.setInputMode(inputMode);
        updateParent(UPDATE_INITIALINPUTMODE);
    }

    /**
     * Get Initial input mode.
     *
     * @return inputMode currently set for TextField.
     */
    String getInitialInputMode()
    {
        return textWrapper.getInputMode();
    }

    /**
     * Calculates minimum size of this item.
     *
     * @return Minimum size.
     */
    Point calculateMinimumSize()
    {
        return TextFieldLayouter.calculateMinimumBounds(this);
    }

    /**
     * Calculates preferred size of this item.
     *
     * @return Preferred size.
     */
    Point calculatePreferredSize()
    {
        return TextFieldLayouter.calculatePreferredBounds(this);
    }

    /**
     * Return layout with optional custom flags.
     *
     * @return layout directive
     */
    int internalGetLayout()
    {
        return super.internalGetLayout(); // | Item.LAYOUT_NEWLINE_BEFORE;
    }

    /**
     * @see javax.microedition.lcdui.Item#isFocusable()
     */
    boolean isFocusable()
    {
        return true;
    }

    /**
     * Update caret position.
     *
     * @param newPosition new caret position.
     */
    void internalSetCaretPosition(int newPosition)
    {
        textWrapper.setCaretposition(newPosition);
    }

    /**
     * Set String when user interacts with TextField.
     *
     * @param newText new text.
     * @return true if this resulted in a change
     */
    boolean internalSetString(String newText)
    {
        if(!textWrapper.getContent().equals(newText))
        {
            textWrapper.setContent(newText);
            return true;
        }
        return false;
    }

    /**
     * Set number of lines for the TextField.
     *
     * @param newLinesCount new number lines.
     */
    void internalSetLinesCount(int newLinesCount)
    {
        linesCount = newLinesCount;
    }

    /**
     * Get number of lines currently in TextField;
     *
     * @return lines count.
     */
    int internalGetLinesCount()
    {
        return linesCount;
    }

    /**
     * Set number of maximum visible lines.
     *
     * @param newMax maximum visible lines allowed for TextField.
     */
    void internalSetMaxVisibleLines(int newMax)
    {
        maxVisibleLines = newMax;
    }

    /**
     * Get number maximum visible lines allowed for TextField.
     *
     * @return number of maximum visible lines to display on the Form.
     */
    int internalGetMaxVisibleLines()
    {
        return maxVisibleLines;
    }

}
