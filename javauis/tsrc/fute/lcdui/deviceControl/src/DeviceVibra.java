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

public class DeviceVibra extends MIDlet  implements CommandListener, Runnable
{

    private  Display display = null;
    private Thread thread = null;
    private Canvas canvas = null;
    private Command exitCommand = new Command("Exit", Command.EXIT, 1);
    private Command startTest = new Command("Start Test", Command.SCREEN, 3);


    public DeviceVibra()
    {
        canvas = new TestingCanvas();
        display = Display.getDisplay(this);
        canvas.addCommand(startTest);
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
            display.vibrate(3000);

            Thread.sleep(3000);

            display.flashBacklight(3000);
            display.vibrate(5000);

            Thread.sleep(8000);

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

        if (c == startTest)
        {
            thread = new Thread(this);
            thread.start();
        }
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
