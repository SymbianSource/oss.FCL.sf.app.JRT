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
 * This class is to test the below Command test case:
 * -Screen:Command mapping to context sensitive options menu
 *
 */

public class Test7 extends Form implements CommandListener, ItemCommandListener
{

    //the StringItem to show the result of each test case
    private StringItem str = null;

    //Commands
    private Command cmdOk = new Command("Ok", Command.OK, 1);

    private Command cmdItem = new Command("Item", Command.ITEM, 1);

    //command to return to main screen
    private Command cmdReturn = new Command("Return", Command.SCREEN, 1);

    //command to go back to the previous screen
    private Command cmdBack = new Command("Back", Command.BACK, 1);

    //TextField to show ITEM command is called
    private TextField tf = null;

    //dummy Form to test OK command is called
    private Form dummyForm = null;
    Timer timer;
    private CommandOkTimerTask okTimerTask;

    public Test7()
    {
        super("Test7");
        timer = new Timer();
        okTimerTask = new CommandOkTimerTask();
        str = new StringItem(null, "Context sensitive options menu and Options menu contains" + " "+
                             cmdOk.getLabel()+ ". " + "\n Press selection key to activate Ok command.");
        append(str);
        addCommand(cmdOk);
        setCommandListener(this);
        dummyForm = new Form("Dummy Form");
        tf = new TextField("Dummy", null, 50, TextField.ANY);
        dummyForm.append(tf);
        dummyForm.addCommand(cmdBack);
        dummyForm.setCommandListener(this);
        tf.addCommand(cmdItem);
        tf.setItemCommandListener(this);
    }

    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {
        if (c == cmdReturn)
            Midp_Command_01.goBack();
        else if (c == cmdOk)
            Midp_Command_01.getDisplay().setCurrent(dummyForm);
        else if (c == cmdBack)
        {
            Midp_Command_01.getDisplay().setCurrent(this);
            timer.schedule(okTimerTask, 2000, 6000);
        }
    }

    public void commandAction(Command c , Item i)
    {
        if (c == cmdItem)
            tf.setString("Item Command is called");
    }

    public class CommandOkTimerTask extends TimerTask
    {
        private int count = 1;
        public void run()
        {
            if (count == 3)
            {
                cancel();
            }
            switch (count)
            {
            case 1:
                addCommand(cmdOk);
                addCommand(cmdItem);
                str.setText("Context sensitive options menu and Options menu contains" + " " + "commands " +
                            "in the order: "+cmdOk.getLabel() + " "+ "and" + " "+cmdItem.getLabel());
                break;
            case 2:
                removeCommand(cmdItem);
                removeCommand(cmdOk);
                str.setText("Commands are removed");
                break;
            case 3:
                addCommand(cmdReturn);
                str.setText("Choose Options->Return to go to the begining");
                break;
            default:
                return;
            }
            count++;
        }
    }
}


