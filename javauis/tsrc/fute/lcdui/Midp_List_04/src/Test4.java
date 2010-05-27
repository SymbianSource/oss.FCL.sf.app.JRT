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
 * With big images and one line element text
 *
 */

public class Test4 extends List implements CommandListener
{
    //Command to go back to the main screen
    Command cmdBack = new Command("Back", Command.BACK, 1);

    //the elements of this list
    String str[] = {"1st Element", "2nd Element", "3rd Element", "4th Element",
                    "5th Element", "6th Element", "7th Element", "8th Element",
                    "9th Element", "10th Element"
                   };

    public Test4(int listType)
    {
        super("Test4", listType);
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


