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
 * This class is to test the Ticker behaviour:
 * 1.When TextField is focussed
 * 2.On TextField when editing indicators are changed
 * 3.On TextBox when editing indicators are changed
 *
 */

public class Test4 extends Form implements CommandListener
{

    //the reference to the MIDlet
    private MIDlet parent = null;

    //ScreenFormTests reference
    private ScreenTickerTests st = null;

    //the testTicker1 to test on different screens
    private Ticker testTicker = null;

    // The TextBox to which Ticker is added
    private TextBox tb = null;

    //The TextField which is added to Form
    private TextField tf = null;

    //The Guage whic is added to Form
    private Gauge g = null;

    //command to show TextBox
    private Command cmdShowTextBox = new Command("Show TextBox", Command.SCREEN, 1);

    //command to go back to this Screen
    private Command cmdBack = new Command("Back", Command.BACK, 1);

    //command to go back to ScreenTickerTests Screen
    private Command cmdReturn = new Command("Return", Command.SCREEN, 1);

    /**
    *
    * @param st
    */
    public Test4(ScreenTickerTests st)
    {

        //title of the Form
        super("Test Form");
        this.st = st;
        testTicker = new Ticker("Test Ticker");

        //get the parent MIDlet
        this.parent = ScreenTickerTests.getParent();
        tb = new TextBox("Ticker to TextBox", null, 100, TextField.ANY);
        tb.setTicker(testTicker);
        tb.addCommand(cmdBack);
        tb.setCommandListener(this);
        tf = new TextField("TextField", null, 30, TextField.ANY);
        g = new Gauge("Interactive Gauge:", true, 20, 10);
        append(g);
        append(tf);
        setTicker(testTicker);

        //add Commands
        addCommand(cmdShowTextBox);
        addCommand(cmdReturn);

        //set the listener
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
        if (c == cmdShowTextBox)
            Display.getDisplay(parent).setCurrent(tb);
        else if (c == cmdReturn)
            Display.getDisplay(parent).setCurrent(st);
        else if (c == cmdBack)
            Display.getDisplay(parent).setCurrent(this);
    }
}
