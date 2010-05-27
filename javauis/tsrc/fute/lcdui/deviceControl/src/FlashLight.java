/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

/*****
 *  Flash Light
 */
import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;
import java.io.*;

public class FlashLight extends MIDlet  implements CommandListener, Runnable
{

    private  Display display = null;
    private Thread thread = null;
    private Canvas canvas = null;
    private Command exitCommand = new Command("Exit", Command.EXIT, 1);
    private Command stopCommand = new Command("Stop Flashing", Command.SCREEN, 1);
    private Command startTest10 = new Command("Start Test 10s", Command.SCREEN, 3);
    private Command startTest60 = new Command("Start Test 60s", Command.SCREEN, 3);
    private Command startTest120 = new Command("Start Test 120s", Command.SCREEN, 3);
    private Command startTest240 = new Command("Start Test 240s", Command.SCREEN, 3);
    private Command currentCommand = null;


    public FlashLight()
    {
        canvas = new TestingCanvas();
        display = Display.getDisplay(this);
        canvas.addCommand(startTest10);
        canvas.addCommand(startTest60);
        canvas.addCommand(startTest120);
        canvas.addCommand(startTest240);
        canvas.addCommand(stopCommand);
        canvas.addCommand(exitCommand);
        canvas.setCommandListener(this);
    }

    /** * */
    public void displayToForeGround()
    {
        display.setCurrent(canvas);
    }

    public void startApp()
    {
        displayToForeGround();
    }
    /** * Pause */
    public void pauseApp() {}

    /** * Destroy must cleanup everything. */
    public void destroyApp(boolean unconditional) { }

    /** *
     *  From runnable
     *  */
    public void run()
    {
        try
        {
            System.out.println("Thread started ..... ");
            if (currentCommand == startTest10)
                display.flashBacklight(10000);
            else if (currentCommand == startTest60)
                display.flashBacklight(60000);
            else if (currentCommand == startTest120)
                display.flashBacklight(120000);
            else if (currentCommand == startTest240)
                display.flashBacklight(240000);

            Thread.sleep(3000);

        }
        catch (Exception e)
        {
            System.out.println("E x c e p t t i o n in run" + e.toString());
        }
    }

    /**** Handle the command */
    public void commandAction(Command c, Displayable s)
    {

        if (c == exitCommand)
        {
            destroyApp(true);
            notifyDestroyed();
        }
        if (c == stopCommand)
        {
            display.flashBacklight(0);
        }
        currentCommand = c;
        thread = new Thread(this);
        thread.start();

    }// end of commandAction

    /**
     *
     */
    class TestingCanvas extends Canvas
    {
        protected void paint(Graphics g)
        {

            g.drawString("Device control", 5, 5, Graphics.TOP|Graphics.LEFT);
        }
    }
}

