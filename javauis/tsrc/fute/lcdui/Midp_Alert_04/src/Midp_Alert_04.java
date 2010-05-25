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
 *
 */

public class Midp_Alert_04 extends MIDlet implements CommandListener
{

    private Alert alert;
    private Display display;
    private Command cmd1 = new Command("Add indicator", Command.SCREEN, 1);
    private Command cmdS = new Command("set string", Command.SCREEN, 1);
    private Command cmdT = new Command("set type", Command.SCREEN, 1);
    private Command cmd2 = new Command("Remove indicator", Command.SCREEN, 1);
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    private IndicatorRunnable indicator;


    public Midp_Alert_04()
    {
        alert = new Alert("Alert with Indicator", "Indefinite-Incremental", null, AlertType.INFO);
        indicator = new IndicatorRunnable();
        alert.setIndicator(indicator);
        alert.addCommand(cmdS);
        alert.addCommand(cmdT);
        alert.addCommand(cmd1);
        alert.addCommand(cmd2);
        alert.addCommand(cmdExit);
        alert.setCommandListener(this);
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
        indicator.setRun();
    }

    /**
     *  Signals the MIDlet to stop and enter the Paused state.
     */
    protected void pauseApp()
    {
    }

    public void commandAction(Command c, Displayable d)
    {
        if (c == cmd1)
        {
            try
            {
                alert.setIndicator(indicator);
            }
            catch (IllegalArgumentException iae)
            {
                Display.getDisplay(this).setCurrent(new Alert("Exception","Gauge in wrong state", null, AlertType.INFO));
            }
            catch (Exception ex)
            {
                Display.getDisplay(this).setCurrent(new Alert("Exception",ex.toString(), null, AlertType.INFO));
            }
        }
        else if (c == cmd2)
            alert.setIndicator(null);

        else if (c == cmdS)
            alert.setString("Text is Set");

        else if (c == cmdT)
            alert.setType(AlertType.ALARM);

        else if (c == cmdExit)
        {
            destroyApp(false);
            notifyDestroyed();
        }
    }
}
