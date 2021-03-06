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

import java.io.*;
/**
 * This is the class to test Commands to Alerts
 */

public class Test2 extends Form implements CommandListener
{

    //the parent MIDlet
    private MIDlet parent = null;

    //the Alert title
    private TextField title = new TextField("Title:", "Alert Title", 500, TextField.ANY);

    //Alert text
    private TextField text = new TextField("Text", "Alert text", 500, TextField.ANY);

    //the Command strings added to alertCommands ChoiceGroup
    private final String[] CMDS = {"One Command", "Two Commands", "Four Commands", "Zero Commands"};

    //the ChoiceGroup to select the number of Commands to the Alert
    private ChoiceGroup alertCommands = new ChoiceGroup("Alert with:",
            ChoiceGroup.EXCLUSIVE, CMDS, null);

    //the Command to show the Alert
    private Command cmdShowAlert = new Command("Show alert", Command.SCREEN, 1);

    private Alert newAlert;
    //Commands to Alert
    private Command cmd1 = new Command("cmd1",Command.SCREEN, 1);
    private Command cmd2 = new Command("cmd2",Command.SCREEN, 1);
    private Command cmd3 = new Command("cmd3",Command.SCREEN, 1);
    private Command cmd4 = new Command("cmd4",Command.SCREEN, 1);

    public Test2()
    {
        super("AlertTests");
        append(title);
        append(text);
        append(alertCommands);
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
        int index = alertCommands.getSelectedIndex();
        String element = alertCommands.getString(index);

        if (c == cmdShowAlert)
        {
            newAlert = new Alert(title.getString(), text.getString(), null, AlertType.INFO);

            if (element.equals(CMDS[0]))
            {
                newAlert.addCommand(cmd1);
            }
            else if (element.equals(CMDS[1]))
            {
                newAlert.addCommand(cmd1);
                newAlert.addCommand(cmd2);
            }
            else if (element.equals(CMDS[2]))
            {
                newAlert.addCommand(cmd1);
                newAlert.addCommand(cmd2);
                newAlert.addCommand(cmd3);
                newAlert.addCommand(cmd4);
            }
            else if (element.equals(CMDS[3]))
            {
                newAlert.removeCommand(cmd1);
                newAlert.removeCommand(cmd2);
                newAlert.removeCommand(cmd3);
                newAlert.removeCommand(cmd4);
            }
            Display.getDisplay(parent).setCurrent(newAlert);
        }
    }
}
