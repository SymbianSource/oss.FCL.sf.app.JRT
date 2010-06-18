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
 * This class is to test the List:
 * With small images and one line element text
 *
 */

public class Test5 extends List implements CommandListener
{
    //Command to go back to the main screen
    Command cmdBack = new Command("Back", Command.BACK, 1);

    //the elements of this list
    String str[] =
    {
        "1st Element should be shown in two lines if it is too big to fit in two line text should be wrapped",
        "2nd Element should be shown in two lines if it is too big to fit in two line text should be wrapped",
        "3rd Element should be shown in two lines if it is too big to fit in two line text should be wrapped",
        "4th Element should be shown in two lines if it is too big to fit in two line text should be wrapped",
        "5th Element should be shown in two lines if it is too big to fit in two line text should be wrapped",
        "6th Element should be shown in two lines if it is too big to fit in two line text should be wrapped",
        "7th Element should be shown in two lines if it is too big to fit in two line text should be wrapped",
        "8th Element should be shown in two lines if it is too big to fit in two line text should be wrapped",
        "9th Element should be shown in two lines if it is too big to fit in two line text should be wrapped",
        "10th Element should be shown in two lines if it is too big to fit in two line text should be wrapped",
        "11th Element should be shown in two lines if it is too big to fit in two line text should be wrapped"
    };

    public Test5(int listType)
    {
        super("Test5", listType);
        //add Command
        addCommand(cmdBack);
        setCommandListener(this);
        addElements();
        setFitPolicy(List.TEXT_WRAP_ON);
    }
    /**
     * appends the list elements with associated images
     */
    private void addElements()
    {
        for (int i= 0; i< str.length; i++)
            append(str[i], Images.getInstance().bigImage);
    }
    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {
        if (c == cmdBack)
            ListTests.show();
    }
}


