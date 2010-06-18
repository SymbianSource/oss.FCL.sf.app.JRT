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
import java.util.*;

/**
 */

public class Test8 extends Form implements CommandListener
{

    //StringItem to show the label of the Command
    private StringItem str = null;

    //the Commands
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    //command to return to main screen
    private Command cmdReturn = new Command("Return", Command.SCREEN, 1);

    /**
     *
     */
    public Test8()
    {
        super("Test8");
        append("There is EXIT_COMMAND in RSK and Return in Options");
        addCommand(cmdExit);
        addCommand(cmdReturn);
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
        if (c == cmdExit)
        {
            Midp_Command_01.goBack();
        }
        else if (c == cmdReturn)
        {
            Midp_Command_01.goBack();
        }
    }
}


