/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 * import midp classes.
 */
import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;

import java.io.*;
/**
 * This is to test the below ChoiceGroup test case:
 *1.elements with different fonts
 *
 */

public class Test8 extends List implements CommandListener
{

    //the instance of this
    private Test8 instance = null;
    //Command to go back to the screen
    Command cmdOK = new Command("Ok", Command.SCREEN, 1);
    Command cmdBack = new Command("Back", Command.BACK, 1);
    Command cmdSmallImage = new Command("Add small image", Command.SCREEN, 1);
    Command cmdBigImage = new Command("Add big image", Command.SCREEN, 1);
    Command cmdRemoveImage = new Command("Remove image", Command.SCREEN, 1);
    Command cmdNullFont = new Command("Set null font", Command.SCREEN, 1);
    Command cmdShowFont = new Command("Show font", Command.SCREEN, 1);
    Command cmdWrapOn = new Command("Wrap on", Command.SCREEN, 1);
    Command cmdWrapOff = new Command("Wrap off", Command.SCREEN, 1);
    //elements which are added to this list
    String str[] =
    {
        "2nd: ", "3rd: ", "4th: ",
        "5th: ", "6th: ", "7th: ", "8th: ",
        "9th: ", "10th: "
    };

    /**
    *
    * @param listType The List type
    */

    public Test8(int listType)
    {
        super("Test8", listType);
        addCommand(cmdOK);
        setCommandListener(this);
        append("Without image", null);
        append("With small image", null);
        append("With big image", null);
    }

    public void RealTest8(int image)
    {
        deleteAll();
        removeCommand(cmdOK);
        //add Commands
        addCommand(cmdBack);
        addCommand(cmdShowFont);
        addCommand(cmdSmallImage);
        addCommand(cmdBigImage);
        addCommand(cmdRemoveImage);
        addCommand(cmdNullFont);
        addCommand(cmdWrapOn);

        setCommandListener(this);
        if (image == 0)
            append("Default: ",null);
        else if (image == 1)
            append("Default: ", Images.getInstance().smallImage);
        else if (image == 2)
            append("Default: ", Images.getInstance().bigImage);

        addElements();
        setFont(1, Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_MEDIUM));
        setFont(2, Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD, Font.SIZE_MEDIUM));
        setFont(3, Font.getFont(Font.FACE_SYSTEM, Font.STYLE_ITALIC, Font.SIZE_MEDIUM));
        setFont(4, Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD|Font.STYLE_ITALIC, Font.SIZE_MEDIUM));

        setFont(5, Font.getFont(Font.FACE_MONOSPACE, Font.STYLE_UNDERLINED|Font.STYLE_ITALIC, Font.SIZE_SMALL));
        setFont(6, Font.getFont(Font.FACE_MONOSPACE, Font.STYLE_BOLD, Font.SIZE_LARGE));
        setFont(7, Font.getFont(Font.FACE_PROPORTIONAL, Font.STYLE_ITALIC, Font.SIZE_MEDIUM));
        setFont(8, Font.getFont(Font.FACE_PROPORTIONAL, Font.STYLE_BOLD, Font.SIZE_LARGE));

        if (instance == null)
            instance = this;
    }

    /**
     * appends the list elements without image
     */
    private void addElements()
    {
        for (int i= 0; i< 9; i++)
            append(str[i], null);
    }

    private String fontInfo(int element)
    {
        if (element == -1)
        {
            return Font.getDefaultFont().getHeight()+" "+
                   ListTests.whatSize(Font.getDefaultFont().getSize())+" "+
                   ListTests.whatStyle(Font.getDefaultFont().getStyle())+" "+
                   ListTests.whatFace(Font.getDefaultFont().getFace());
        }
        else
        {
            return this.getFont(element).getHeight()+" "+
                   ListTests.whatSize(this.getFont(element).getSize())+" "+
                   ListTests.whatStyle(this.getFont(element).getStyle())+" "+
                   ListTests.whatFace(this.getFont(element).getFace());
        }
    }
    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {
        if (c == cmdOK)
        {
            RealTest8(getSelectedIndex());
        }
        else if (c == cmdBack)
        {
            ListTests.show();
        }
        else if (c == cmdSmallImage)
        {
            instance.set(this.getSelectedIndex(), this.getString(getSelectedIndex()),Images.getInstance().smallImage);
        }
        else if (c == cmdBigImage)
        {
            instance.set(this.getSelectedIndex(), this.getString(getSelectedIndex()),Images.getInstance().bigImage);
        }
        else if (c == cmdRemoveImage)
        {
            instance.set(this.getSelectedIndex(), this.getString(getSelectedIndex()),null);
        }
        else if (c == cmdNullFont)
        {
            instance.setFont(this.getSelectedIndex(),null);
        }
        else if (c == cmdShowFont)
        {
            set(this.getSelectedIndex(), this.getString(this.getSelectedIndex()) + " " + fontInfo(this.getSelectedIndex()),
                this.getImage(getSelectedIndex()));
        }
        else if (c == cmdWrapOn)
        {
            removeCommand(cmdWrapOn);
            addCommand(cmdWrapOff);
            setFitPolicy(Choice.TEXT_WRAP_ON);
        }
        else if (c == cmdWrapOff)
        {
            removeCommand(cmdWrapOff);
            addCommand(cmdWrapOn);
            setFitPolicy(Choice.TEXT_WRAP_OFF);
        }

    }
}






