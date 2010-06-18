/*
* Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * 1.When the element image is invalid
 */

public class Test4 extends List implements CommandListener
{

    //Command to go back to the main screen
    Command cmdBack = new Command("Back", Command.BACK, 1);

    //long label
    private final String longLabel = "This is a very long label and there is also an image";

    /**
     *
     * @param listType The List type
     */
    public Test4(int listType)
    {

        super("Test4", listType);
        String listLabel = null;
        //add Back command
        addCommand(cmdBack);
        setCommandListener(this);
        try
        {
            append(longLabel, Images.getInstance().getInvalid());
        }
        catch (IOException e)
        {
            String msg = "Could not load invalid image";
            append(msg, null);
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
        if (c == cmdBack)
            ListTests.show();
    }
}


