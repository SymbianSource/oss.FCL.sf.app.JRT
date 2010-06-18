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
 *  Flash Back Light
 */
import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;
import java.io.*;

public class FlashBackLight extends MIDlet  implements CommandListener, Runnable
{

    private  Display display = null;
    private Thread thread = null;
    private Canvas canvas = null;
    private String deviceControlMessage = "Device Control";
    private Command exitCommand = new Command("Exit", Command.EXIT, 1);
    private Command backLightCommnad = new Command("Back Light", Command.SCREEN, 3);
    private Command vibraCommnad = new Command("Start Vibra", Command.SCREEN, 3);
    private Command vibraStopCommnad = new Command("Stop Vibra", Command.SCREEN, 3);
    private Command currentCommand = null;

    public FlashBackLight()
    {
        canvas = new TestingCanvas();
        display = Display.getDisplay(this);
        canvas.addCommand(backLightCommnad);
        canvas.addCommand(vibraCommnad);
        canvas.addCommand(vibraStopCommnad);
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
            if (currentCommand == backLightCommnad)
                display.flashBacklight(10000);
            else if (currentCommand == vibraCommnad)
                display.vibrate(10000);
            else
                display.flashBacklight(1000);
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
        if (c == vibraStopCommnad)
        {
            display.vibrate(0);
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
        protected void keyPressed(int keyCode)
        {
            if (getGameAction(keyCode) == FIRE || keyCode == KEY_NUM1 || keyCode == KEY_NUM3)
            {
                display.flashBacklight(1000);
            }
            else if (getGameAction(keyCode) == GAME_A || getGameAction(keyCode) == GAME_B
                     || getGameAction(keyCode) == GAME_C || getGameAction(keyCode) == GAME_D)
            {
                display.flashBacklight(2000);
            }
            else if (keyCode == KEY_NUM0)
            {
                display.flashBacklight(0);
            }
            else if (keyCode == KEY_NUM8)
            {
                display.vibrate(0);
            }
        }
    }
}
