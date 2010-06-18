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
 * 1.When the element image is larger than the layout it fits
 */

public class Test2 extends List implements CommandListener
{

    //command to go Back to the main screen
    Command cmdBack = new Command("Back", Command.BACK, 1);

    /**
     *
     * @param listType The List type
     */
    public Test2(int listType)
    {

        super("Test2", listType);
        //add Back command
        addCommand(cmdBack);
        setCommandListener(this);

        //append big image to this list
        append("Big Image", Images.getInstance().bigImage);
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


