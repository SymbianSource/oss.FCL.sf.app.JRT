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

/**
 * This class is to test the List:
 *
 */

public class Test2 extends List implements CommandListener
{

    //the command to go back to the ListTests screen
    Command cmdBack = new Command("Back", Command.BACK, 1);

    //elements to this list
    private final String element1 = "1st Element should be shown in two lines if it is too big to fit in two line text should be wrapped";
    private final String element2 ="2nd Element should be shown in two lines if it is too big to fit in two line text should be wrapped";
    private final String element3 ="3rd Element should be shown in two lines if it is too big to fit in two line text should be wrapped";
    private final String element4 ="4th Element should be shown in two lines if it is too big to fit in two line text should be wrapped";
    private final String element5 ="5th Element should be shown in two lines if it is too big to fit in two line text should be wrapped";
    private final String element6 ="6th Element should be shown in two lines if it is too big to fit in two line text should be wrapped";
    private final String element7 ="7th Element should be shown in two lines if it is too big to fit in two line text should be wrapped";
    private final String element8 ="8th Element should be shown in two lines if it is too big to fit in two line text should be wrapped";
    private final String element9 ="9th Element should be shown in two lines if it is too big to fit in two line text should be wrapped";
    private final String element10 ="10th Element should be shown in two lines if it is too big to fit in two line text should be wrapped";
    private final String element11 ="11th Element should be shown in two lines if it is too big to fit in two line text should be wrapped";

    public Test2(int listType)
    {
        super("Test2", listType);
        //append elements to this list
        append(element1, null);
        append(element2, null);
        append(element3, null);
        append(element4, null);
        append(element5, null);
        append(element6, null);
        append(element7, null);
        append(element8, null);
        append(element9, null);
        append(element10, null);
        append(element11, null);
        //add commands to this list
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
            ListTests.show();
    }
}


