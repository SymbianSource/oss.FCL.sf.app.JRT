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
 *  This is the main class for the Midp_General_03 tests.
 */

public class Midp_General_03 extends MIDlet implements CommandListener
{

    private List emptyList;
    private Display display;
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    public Midp_General_03()
    {
        emptyList = new List("List", List.EXCLUSIVE);
        emptyList.addCommand(cmdExit);
        emptyList.setCommandListener(this);
        display = Display.getDisplay(this);
    }

    /**
     * Signals the MIDlet to start and enter the Active state.
     */
    protected void startApp()
    {
        display.setCurrent(emptyList);
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
            destroyApp(false);
            notifyDestroyed();
        }
    }
}
