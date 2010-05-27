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
 * This is the main class for the Midp_Command_02 tests.
 */

public class Midp_Command_02 extends MIDlet implements CommandListener
{

    //reference to the parent MIDlet to go to the different screens
    private static Midp_Command_02 instance = null;
    Display display = null;
    List listTests = null;

    private Command cmdOk = new Command("Ok", Command.OK, 1);
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    private String[] tests = {"Test1", "Test2", "Test3", "Test4", "Test5", "Test6", "Test7"};

    public Midp_Command_02()
    {
        instance = this;
        display = Display.getDisplay(this);
        listTests = new List("Command Tests", List.IMPLICIT, tests, null);
        listTests.addCommand(cmdOk);
        listTests.addCommand(cmdExit);
        listTests.setCommandListener(this);
    }

    /**
     * Signals the MIDlet to start and enter the Active state.
     */
    public void startApp()
    {
        display.setCurrent(listTests);
    }

    public void commandAction(Command c, Displayable d)
    {
        int pos = listTests.getSelectedIndex();
        String element = listTests.getString(pos);
        if (c == cmdOk || c == List.SELECT_COMMAND)
        {
            if (element.equals(tests[0]))
                new Test1();
            else if (element.equals(tests[1]))
                new Test2();
            else if (element.equals(tests[2]))
                new Test3();
            else if (element.equals(tests[3]))
                new Test4();
            else if (element.equals(tests[4]))
                new Test5();
            else if (element.equals(tests[5]))
                new Test6();
            else if (element.equals(tests[6]))
                new Test7();
        }
        else if (c == cmdExit)
        {
            destroyApp(false);
            notifyDestroyed();
        }
    }

    public static Display getDisplay()
    {
        return instance.display;
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
    * This method returns us to the initial list screen.
    */
    public static void goBack()
    {
        instance.startApp();
    }
}

