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

import java.io.*;
/**
 * This is the class to test Commands to Alerts
 */

public class Test4 extends Form implements CommandListener
{

    //the parent MIDlet
    private MIDlet parent = null;

    //the Alert title
    private TextField title = new TextField("Title:", "Alert Title", 500, TextField.ANY);

    //Alert text
    private TextField text = new TextField("Text", "Alert text", 500, TextField.ANY);

    //the Command to show the Alert
    private Command cmdShowAlert = new Command("Show alert", Command.SCREEN, 1);

    private Alert newAlert;

    //Commands to Alert
    private Command cmd1 = new Command("cmd1",Command.SCREEN, 1);
    private Command cmd2 = new Command("cmd2",Command.SCREEN, 1);
    private Command cmd3 = new Command("cmd3",Command.SCREEN, 1);
    private Command cmd4 = new Command("cmd4",Command.SCREEN, 1);
    Timer timer;
    private CommandOkTimerTask okTimerTask;


    public Test4()
    {
        super("AlertTests");
        timer = new Timer();
        okTimerTask = new CommandOkTimerTask();
        append(title);
        append(text);
        newAlert = new Alert(title.getString(), text.getString(), null, AlertType.INFO);
        newAlert.setTimeout(8000);
        addCommand(cmdShowAlert);
        setCommandListener(this);

        parent = AlertTests.getParent();
    }

    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {

        if (c == cmd1)
        {
            newAlert.addCommand(cmd2);
        }
        if (c == cmdShowAlert)
        {
            newAlert.addCommand(cmd1);
            newAlert.setCommandListener(this);
            Display.getDisplay(parent).setCurrent(newAlert);
            timer.schedule(okTimerTask, 2000, 1000);
        }
    }
    public class CommandOkTimerTask extends TimerTask
    {
        private int count = 1;
        public void run()
        {
            if (count == 4)
            {
                cancel();
            }
            switch (count)
            {
            case 1:
                newAlert.addCommand(cmd2);
                break;
            case 2:
                newAlert.addCommand(cmd3);
                break;
            case 3:
                newAlert.addCommand(cmd4);
                break;
            default:
                return;
            }
            count++;
            System.out.println("count:" + count);
        }
    }
}
