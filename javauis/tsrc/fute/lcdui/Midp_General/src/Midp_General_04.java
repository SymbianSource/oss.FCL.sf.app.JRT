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
 *  This is the main class for the Midp_General_04 tests.
 */

public class Midp_General_04 extends MIDlet implements CommandListener
{

    private Alert alert;
    private Display display;
    private Gauge indicator;
    private Image image;
    private Command cmdScreen = new Command("Screen cmd", Command.SCREEN, 1);
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    private String alertText = "This is a modal alert with very long text blah blah blah blah blah blah " +
                               "blah blah blah blah blah blah blah blah blah blah blah blah blah blah " +
                               "blah blah blah blah blah blah blah blah blah blah blah blah blah blah " +
                               "blah blah blah blah blah blah blah blah blah blah blah blah blah blah " +
                               "blah blah blah blah blah blah blah blah blah blah blah blah blah blah " +
                               "blah blah blah blah blah blah blah blah blah blah blah blah blah blah " +
                               "blah blah blah blah blah blah blah blah blah blah blah blah blah blah";

    public Midp_General_04()
    {
        alert = new Alert("Alert title", alertText, null, AlertType.INFO);
        alert.addCommand(cmdExit);
        alert.addCommand(cmdScreen);
        alert.setCommandListener(this);
        indicator = new Gauge(null, false, Gauge.INDEFINITE, Gauge.CONTINUOUS_RUNNING);
        alert.setIndicator(indicator);

        try
        {
            image = Image.createImage("/small.png");
        }
        catch (java.io.IOException e)
        {
        }
        alert.setImage(image);

        display = Display.getDisplay(this);
    }

    /**
     * Signals the MIDlet to start and enter the Active state.
     */
    protected void startApp()
    {
        display.setCurrent(alert);
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
        else if (c == cmdScreen)
        {
            alert.setTitle("Command run");
        }
    }
}
