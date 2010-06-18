/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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

public class Test7 extends List implements CommandListener
{
    //Command to go back to the main screen
    Command cmdBack = new Command("Back", Command.BACK, 1);

    //the elements of this list
    String str[] = {"1stElement should be shown in two lines and text should be truncated", "2ndElement", "3rdElement", "4thElement",
                    "5thElement", "6thElement", "7thElement", "8thElement",
                    "9thElement", "10thElement"
                   };

    public Test7(int listType)
    {
        super("Test7", listType);
        //add Command
        addCommand(cmdBack);
        setCommandListener(this);
        addElements();
    }
    /**
     * appends the list elements with associated images
     */
    private void addElements()
    {
        for (int i= 0; i< 10; i++)
            append(str[i], Images.getInstance().smallImage);
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


