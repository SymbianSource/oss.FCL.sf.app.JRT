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

import com.nokia.mj.impl.rt.support.ApplicationInfo;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.widgets.*;

/**
 * This class represents LCDUI TextBox.
 * The behavior of this editor may be different according
 * to applied constraints.
 */
public class TextBox extends Screen
{

    private TextModifyListener modListener = new TextModifyListener();

    private TextWrapper textWrapper;

    private int numLines;

    /**
     * Constructor.
     *
     * @param title - Title of the TextBox
     * @param text - default text appeared in the TextBox
     * @param maxSize - maximum number of characters to insert
     * @param constraints - text constraint (e.g. URL, EMEAIL etc)
     */
    public TextBox(String title, String text, int maxSize, int constraints)
    {
        super(title);
        textWrapper = new TextWrapper(text, maxSize, constraints);
        construct();
    }

    /**
     * Constructs custom eSWT shell for alert dialog.
     *
     * @return custom eSWT dialog shell
     */
    Shell eswtConstructShell(int style)
    {
        Shell topShell = super.eswtConstructShell(style);
        // TextBox with null title and ANY constraint, should be Full-Screen
        if(getTitle() == null && TextWrapper.getTypeConstraint(
                    textWrapper.getConstraints()) == TextField.ANY)
        {
            return topShell;
        }
        else
        {
            if(JadAttributeUtil.isValue(JadAttributeUtil.ATTRIB_NOKIA_UI_ENHANCEMENT, JadAttributeUtil.VALUE_FULLSCREEN_TEXTBOX))
            {
                return topShell;
            }
            else
            {
                return new Shell(topShell, style | SWT.DIALOG_TRIM | SWT.RESIZE);
            }
        }
    }

    /* (non-Javadoc)
     * @see Displayable#eswtConstructContent(int)
     */
    Composite eswtConstructContent(int style)
    {
        Composite ret = super.eswtConstructContent(style);
        textWrapper.eswtConstruct(ret, SWT.V_SCROLL);
        return ret;
    }

    /* (non-Javadoc)
     * @see Displayable#eswtHandleShowCurrentEvent()
     */
    void eswtHandleShowCurrentEvent()
    {
        super.eswtHandleShowCurrentEvent();
        textWrapper.setModifyListener(modListener);
        eswtSetPreferredContentSize(-1, textWrapper
                                    .getPreferredHeight(Config.TEXTBOX_MAX_VISIBLE_LINES));
    }

    /* (non-Javadoc)
     * @see Displayable#eswtHandleHideCurrentEvent()
     */
    void eswtHandleHideCurrentEvent()
    {
        super.eswtHandleHideCurrentEvent();
        textWrapper.setModifyListener(null);
    }

    /* (non-Javadoc)
     * @see Displayable#eswtHandleResizeEvent(int, int)
     */
    void eswtHandleResizeEvent(int width, int height)
    {
        super.eswtHandleResizeEvent(width, height);
        textWrapper.setBounds(getContentComp().getClientArea());
    }

    /**
     * Get current caret position.
     *
     * @return current caret position
     */
    public int getCaretPosition()
    {
        return textWrapper.getCaretPosition();
    }

    /**
     * Returns String with the content of TextBox.
     *
     * @return String with TexBox content
     */
    public String getString()
    {
        return textWrapper.getContent();
    }

    /**
     * Set new text into TextBox. Old content is substituted with newTxt
     *
     * @param newText - String to set into TextBox
     */
    public void setString(String newText)
    {
        textWrapper.setContent(newText);
    }

    /**
     * Copies TextBox content into char[] charData.
     *
     * @param charData array where to copy TextBox content
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
     * Set data from char[] array into TextBox. Previous content from TextBox is
     * substituted. Behavior is quite the same as TextBox.SetString().
     *
     * @param charData array of chars from where to copy.
     * @param offset start index in charData.
     * @param length how many characters to copy.
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
                throw new ArrayIndexOutOfBoundsException();
            }
        }
        textWrapper.setContent(extractedString);
    }

    /**
     * Inserts text into specified position.
     *
     * @param text text to insert, must not be null.
     * @param position where to insert.
     */
    public void insert(String text, int position)
    {
        textWrapper.insert(text, position);
    }

    /**
     * Inserts into TextBox range of characters from []charData array.
     *
     * @param charData array of chars to copy from.
     * @param offset start index in array to copy from.
     * @param length number of characters to copy.
     * @param position in TextBox where to insert.
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
            throw new ArrayIndexOutOfBoundsException();
        }
        textWrapper.insert(extractedString, position);
    }

    /**
     * Delete range of characters from TextBox.
     *
     * @param offset - start index in TextBox to delete from.
     * @param length number of characters to delete.
     */
    public void delete(int offset, int length)
    {
        textWrapper.delete(offset, length);
    }

    /**
     * get number if characters TextBox can contain.
     *
     * @return number of characters allowed for the TextBox.
     */
    public int getMaxSize()
    {
        return textWrapper.getMaxSize();
    }

    /**
     * Set Max number of characters. The actual maximum size
     * may be less then newMaxSize due to platform limitations.
     *
     * @param newMaxSize sets the capacity of TextBox.
     * @return maxSize that was set.
     */
    public int setMaxSize(int newMaxSize)
    {
        textWrapper.setMaxSize(newMaxSize);
        return textWrapper.getMaxSize();
    }

    /**
     * Get number of characters in the TextBox.
     *
     * @return number if inputed Characters.
     */
    public int size()
    {
        return textWrapper.getSize();
    }

    /**
     * Set constraint for the TextBox.
     *
     * @param newConstraints constraint to apply to TextBox
     */
    public void setConstraints(int newConstraints)
    {
        textWrapper.setConstraints(newConstraints);
    }

    /**
     * Get Current applied constraints.
     *
     * @return current applied constraints
     */
    public int getConstraints()
    {
        return textWrapper.getConstraints();
    }

    /**
     * Set initial input mode.
     *
     * @param inputMode input mode to set.
     */
    public void setInitialInputMode(String inputMode)
    {
        textWrapper.setInputMode(inputMode);
    }

    /**
     * Text modify listener.
     */
    class TextModifyListener implements ModifyListener
    {

        public void modifyText(ModifyEvent me)
        {
            int lines = TextWrapper.eswtGetLineCount((Control) me.widget);
            if(numLines != lines)
            {
                // the number of lines changed
                numLines = lines;
                eswtSetPreferredContentSize(-1, textWrapper
                                            .getPreferredHeight(Config.TEXTBOX_MAX_VISIBLE_LINES));
            }
        }

    }

}
