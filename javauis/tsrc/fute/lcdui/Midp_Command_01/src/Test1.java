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



public class Test1 extends Form implements CommandListener
{

    //StringItem to show the label of the Command
    private StringItem str = null;

    //the Commands
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);
    private Command cmdOk = new Command("Ok", Command.OK, 1);
    private Command cmdScreen = new Command("Screen", Command.SCREEN, 1);
    private Command cmdItem = new Command("Item_With_Very_Long_Label", Command.ITEM, 1);
    private Command cmdCancel = new Command("Cancel", Command.CANCEL, 1);
    private Command cmdBack = new Command("Back_With_Very_Long_Label", Command.BACK, 1);
    private Command cmdStop = new Command("Stop", Command.STOP, 1);
    private Command cmdHelp = new Command("Help", Command.HELP, 1);

    //command to return to main screen
    private Command cmdReturn = new Command("Return", Command.SCREEN, 1);

    Timer timer;
    private CommandOkTimerTask okTimerTask;

    /**
     *
     */
    public Test1()
    {
        super("Test1");
        timer = new Timer();
        okTimerTask = new CommandOkTimerTask();
        str = new StringItem("Command:", null);
        addCommand(cmdExit);
        append(str);
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
        if (c == cmdExit)
        {
            timer.schedule(okTimerTask, 2000, 3000);
            str.setText(c.getLabel());
        }
        else if (c == cmdReturn)
        {
            Midp_Command_01.goBack();
        }
        str.setText(c.getLabel());
    }

    public class CommandOkTimerTask extends TimerTask
    {
        private int count = 1;
        public void run()
        {
            if (count == 8)
            {
                cancel();
            }
            switch (count)
            {
            case 1:
                removeCommand(cmdExit);
                addCommand(cmdOk);
                break;
            case 2:
                removeCommand(cmdOk);
                addCommand(cmdScreen);
                break;
            case 3:
                removeCommand(cmdScreen);
                addCommand(cmdItem);
                break;
            case 4:
                removeCommand(cmdItem);
                addCommand(cmdCancel);
                break;
            case 5:
                removeCommand(cmdCancel);
                addCommand(cmdBack);
                break;
            case 6:
                removeCommand(cmdBack);
                addCommand(cmdStop);
                break;
            case 7:
                removeCommand(cmdStop);
                addCommand(cmdHelp);
                addCommand(cmdReturn);
                break;
            default:
                return;
            }
            count++;
        }
    }
}


