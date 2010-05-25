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
 * This class is to test List:
 * 1.When it has only one element with associated image
 *
 */

public class Test6 extends List implements CommandListener
{

    //Command to go back to the screen
    Command cmdBack = new Command("Back", Command.BACK, 1);

    //elements which are added to this list
    String str[] = { "2ndElement", "3rdElement", "4thElement",
                     "5thElement", "6thElement", "7thElement", "8thElement",
                     "9thElement", "10thElement"
                   };

    /**
    *
    * @param listType The List type
    */
    public Test6(int listType)
    {

        super("Test6", listType);
        //add Commands
        addCommand(cmdBack);
        setCommandListener(this);

        append("1Element", Images.getInstance().smallImage);
        addElements();
    }

    /**
    * appends the list elements without image
     */
    private void addElements()
    {
        for (int i= 0; i< 9; i++)
            append(str[i], null);
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


