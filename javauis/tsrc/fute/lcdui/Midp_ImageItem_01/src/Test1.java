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
 * This is the class to test ImageItems tests:
 * 1.Big image which exceed the width of the screen
 * 2.Big image which exceed the height of the screen
 */

public class Test1 extends Form implements CommandListener
{

    //Command to go back to the ImageItemTests screen
    Command cmdBack = new Command("Back", Command.BACK, 1);

    //ImageItem to test
    private ImageItem imgItem = null;

    //text for imgItem
    private final String text = "Test1 Image";

    public Test1(Image img)
    {
        super("ImageItem tests");
        //create imgItem and append to the Form
        imgItem = new ImageItem("ImageItem", img,
                                ImageItem.LAYOUT_DEFAULT, text);
        //append imgItem to this Form
        append(imgItem);

        //add Commands
        addCommand(cmdBack);
        setCommandListener(this);
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
            ImageItemTests.show();
    }
}


