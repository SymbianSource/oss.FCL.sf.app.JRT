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
 * This is the main class for the Midp_Command_01 tests.
 */

public class Midp_Command_01 extends MIDlet implements CommandListener
{

    //reference to the parent MIDlet to go to the different screens
    private static Midp_Command_01 instance = null;
    Display display = null;
    List listTests = null;

    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    private String[] tests = {"Test1", "Test2", "Test3", "Test4", "Test5", "Test6", "Test7", "Test8"};

    public Midp_Command_01()
    {
        instance = this;
        display = Display.getDisplay(this);
        listTests = new List("Command Tests", List.IMPLICIT, tests, null);
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

    public Displayable getTestObject(String classString)
    {
        String testClassString = classString;
        String exceptionText = "";
        try
        {
            return (Displayable) Class.forName(testClassString).newInstance();
        }
        catch (ClassNotFoundException e)
        {
            exceptionText = "Test not available: " + testClassString;
            return new Alert(null, exceptionText, null, null);
        }
        catch (IllegalAccessException e)
        {
            exceptionText = "IllegalAccessException:\n" + testClassString;
            return new Alert("Error", exceptionText, null, null);
        }
        catch (InstantiationException e)
        {
            exceptionText = "InstantiationException:\n" + testClassString;
            return new Alert("Error", exceptionText, null, null);
        }
    }

    public void commandAction(Command c, Displayable d)
    {
        int pos = listTests.getSelectedIndex();
        if (c == List.SELECT_COMMAND)
        {
            if (pos < 0 || pos > tests.length) return;
            display.setCurrent(getTestObject(tests[pos]));
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

