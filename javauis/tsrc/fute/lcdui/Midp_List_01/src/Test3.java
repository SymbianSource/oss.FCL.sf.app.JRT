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
 * 1.When the element label is longer than the image.
 */

public class Test3 extends List implements CommandListener
{

    //Command to go back to the main screen
    Command cmdBack = new Command("Back", Command.BACK, 1);

    //the longLabel added to this List
    private final String longLabel = "This is a very long label and there is also an image";

    /**
     *
     * @param listType The List type
     */
    public Test3(int listType)
    {
        super("Test3", listType);
        //add Back command
        addCommand(cmdBack);
        setCommandListener(this);

        //append long label with image to this list
        append(longLabel, Images.getInstance().smallImage);
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


