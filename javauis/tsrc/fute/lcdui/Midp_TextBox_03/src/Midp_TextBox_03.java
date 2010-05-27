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
 * This class is to test TextBox test case:
 * -TextBox remembers the cursor position when the control loses focus and regains
 * later.
 *
 */

public class Midp_TextBox_03 extends MIDlet implements CommandListener
{

    //This is to create popup TextBox
    public TextBox tb1 = null;

    private List dummyList = null;

    private String[] listStrs = { "1st element", "2nd element", "3rd element", "4th element",
                                  "5th element", "6th element"
                                };

    //Commands
    private Command cmdNext = new Command("Next", Command.SCREEN, 1);
    private Command cmdBack = new Command("Back", Command.BACK, 1);
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    private final int MAX_SIZE = 50;

    public Midp_TextBox_03()
    {
        tb1 = new TextBox("Test TextBox", null, MAX_SIZE, TextField.ANY);
        tb1.addCommand(cmdNext);
        tb1.addCommand(cmdExit);
        tb1.setCommandListener(this);

        dummyList = new List("Dummy List", List.IMPLICIT, listStrs, null);
        dummyList.addCommand(cmdBack);
        dummyList.setCommandListener(this);
    }

    /**
     * Signals the MIDlet to start and enter the Active state.
     */
    protected void startApp()
    {
        Display.getDisplay(this).setCurrent(tb1);
    }

    /**
     * This method handles command invocations
     *
     * @param c The Command
     * @param d The Displayable
     */
    public void commandAction(Command c, Displayable d)
    {
        if (c == cmdNext)
            Display.getDisplay(this).setCurrent(dummyList);
        else if (c == cmdBack)
            Display.getDisplay(this).setCurrent(tb1);
        else if (c == cmdExit)
        {
            destroyApp(false);
            notifyDestroyed();
        }
    }

    /**
     *  Signals the MIDlet to terminate and enter the Destroyed state.
     *
     */
    protected void destroyApp(boolean unconditional)
    {
    }

    /**
     *  Signals the MIDlet to stop and enter the Paused state.
     */
    protected void pauseApp()
    {
    }
}

