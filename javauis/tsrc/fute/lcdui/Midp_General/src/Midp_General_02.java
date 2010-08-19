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
 *  This is the main class for the Midp_General_02 tests.
 */

public class Midp_General_02 extends MIDlet implements CommandListener
{

    TextBox tb;
    Display display;
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    String theText = "This is a TextBox with very long text blah blah blah blah blah blah" +
                     "blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah" +
                     "blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah" +
                     "blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah" +
                     "blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah" +
                     "blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah" +
                     "blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah" +
                     "This is the last sentence.";

    public Midp_General_02()
    {
        tb = new TextBox("TextBox", theText, 1000, TextField.ANY);
        tb.addCommand(cmdExit);
        tb.setCommandListener(this);
        display = Display.getDisplay(this);
    }

    /**
     * Signals the MIDlet to start and enter the Active state.
     */
    protected void startApp()
    {
        display.setCurrent(tb);
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
