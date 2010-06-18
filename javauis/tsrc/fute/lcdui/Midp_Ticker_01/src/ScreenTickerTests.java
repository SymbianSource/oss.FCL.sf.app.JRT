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
 * This class will call the series of tests to test Ticker
 *
 */

public class ScreenTickerTests extends Form implements CommandListener
{

    //the reference to the MIDlet to go to the different screens
    private static Midp_Ticker_01 parent = null;

    //the this instance
    private static ScreenTickerTests instance = null;

    //the command to invoke Test1
    private Command cmdTest1 = new Command("Test1", Command.SCREEN, 1);

    //the command to invoke Test2
    private Command cmdTest2 = new Command("Test2", Command.SCREEN, 1);

    //the command to invoke Test3
    private Command cmdTest3 = new Command("Test3", Command.SCREEN, 1);

    //the command to invoke Test3
    private Command cmdTest4 = new Command("Test4", Command.SCREEN, 1);

    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    /**
     * This constructor will display a Form with MIDlet name as it's title.
     * And adds the Test Commands to this Form.
     *
     * @param parent parent The parent MIDlet of this class
     */
    public ScreenTickerTests(Midp_Ticker_01 parent)
    {

        //set the title of this form to be the name of the MIDlet
        super(parent.getClass().getName());
        this.parent = parent;

        //add test commands
        addCommand(cmdTest1);
        addCommand(cmdTest2);
        addCommand(cmdTest3);
        addCommand(cmdTest4);
        addCommand(cmdExit);
        setCommandListener(this);
        if (instance == null)
        {
            instance = this;
        }
        show();
    }

    /**
     *  Sets this to be the current screen
     *
     *@return    The Parent value
     */
    public static MIDlet getParent()
    {
        return parent;
    }

    /**
     *  Show this screen
     */
    public static void show()
    {
        Display.getDisplay(parent).setCurrent(instance);
    }

    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {
        if (c == cmdTest1)
            Display.getDisplay(parent).setCurrent(new Test1(this));
        else if (c== cmdTest2)
            Display.getDisplay(parent).setCurrent(new Test2(this));
        else if (c == cmdTest3)
            Display.getDisplay(parent).setCurrent(new Test3(this));
        else if (c == cmdTest4)
            Display.getDisplay(parent).setCurrent(new Test4(this));
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
}
