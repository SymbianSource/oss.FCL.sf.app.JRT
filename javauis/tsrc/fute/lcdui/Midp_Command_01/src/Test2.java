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
 *
 */
public class Test2 extends Form implements CommandListener
{

    //the StringItem to show the result of each test case
    private StringItem str = null;

    //the Commands
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);
    private Command cmdOk = new Command("Ok", Command.OK, 1);
    private Command cmdScreen = new Command("Screen", Command.SCREEN, 1);
    private Command cmdItem = new Command("Item_with_very_long_Label", Command.ITEM, 1);
    private Command cmdCancel = new Command("Cancel", Command.CANCEL, 1);
    private Command cmdBackLong = new Command("Back_with_very_long_Label", Command.BACK, 1);
    private Command cmdBack = new Command("Back", Command.BACK, 1);
    private Command cmdStop = new Command("Stop", Command.STOP, 1);
    private Command cmdStopLong = new Command("Stop_with_very_long_Label", Command.STOP, 1);
    private Command cmdHelp = new Command("Help", Command.HELP, 1);

    //command to return to main screen
    private Command cmdReturn = new Command("Return", Command.SCREEN, 1);

    Timer timer;
    private CommandOkTimerTask okTimerTask;

    /**
     *
     */
    public Test2()
    {
        super("Test2");
        timer = new Timer();
        okTimerTask = new CommandOkTimerTask();
        str = new StringItem("Result:", "-SK: truncated Back_with_very_long_label");
        append(str);
        addCommand(cmdBackLong);

        timer.schedule(okTimerTask, 2000, 6000);
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
        if (c == cmdReturn)
            Midp_Command_01.goBack();
    }

    public class CommandOkTimerTask extends TimerTask
    {
        private int count = 1;
        public void run()
        {
            if (count == 11)
            {
                cancel();
            }
            switch (count)
            {
            case 1:
                removeCommand(cmdBackLong);
                addCommand(cmdBack);
                addCommand(cmdCancel);
                str.setText("-SK: Cancel\n"+"+SK: Back");
                break;
            case 2:
                removeCommand(cmdCancel);
                addCommand(cmdStop);
                str.setText("-SK: Stop\n"+"+SK: Back");
                break;
            case 3:
                removeCommand(cmdStop);
                addCommand(cmdExit);
                str.setText("-SK: Back\n"+"+SK: Exit");
                break;
            case 4:
                addCommand(cmdStopLong);
                addCommand(cmdCancel);
                str.setText("-SK: truncated Stop_with_very_long_label\n"+"+SK: Options menu commands\n"+"Cancel,Back and Exit");
                break;
            case 5:
                removeCommand(cmdStopLong);
                removeCommand(cmdBack);
                removeCommand(cmdExit);
                removeCommand(cmdStopLong);
                str.setText("-SK: Cancel");
                break;
            case 6:
                addCommand(cmdStop);
                str.setText("-SK: Stop\n"+"+SK: Cancel");
                break;
            case 7:
                removeCommand(cmdStop);
                addCommand(cmdExit);
                str.setText("-SK: Cancel\n"+"+SK: Exit");
                break;
            case 8:
                addCommand(cmdStop);
                str.setText("-SK: Stop\n"+"+SK: Options menu commands\n"+"Cancel and Exit");
                break;
            case 9:
                removeCommand(cmdCancel);
                removeCommand(cmdExit);
                str.setText("-SK: Stop");
                break;
            case 10:
                addCommand(cmdExit);
                str.setText("-SK: Stop\n"+"+SK: Exit");
                break;
            case 11:
                removeCommand(cmdStop);
                addCommand(cmdReturn);
                str.setText("-SK: Exit");
                break;
            default:
                return;
            }
            count++;
        }
    }
}


