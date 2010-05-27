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
 * -Screen:Commands with long and short labels
 */

public class Test6 extends Form implements CommandListener
{

    //the StringItem to show the result of each test case
    private StringItem str = null;

    //Commands with null label
    private Command cmdOk = new Command("Ok", "Ok label", Command.OK, 1);

    private Command cmdScreen = new Command("Screen", "Screen with long label", Command.SCREEN, 1);

    private Command cmdItem = new Command("Item", "Item with long label", Command.ITEM, 1);

    private Command cmdCancel = new Command("Cancel", "Cancel with very long label", Command.CANCEL, 1);

    private Command cmdBack = new Command("Back", "Back with very long label", Command.BACK, 1);

    private Command cmdStop = new Command("Stop", "Stop with very long label", Command.STOP, 1);

    private Command cmdHelp = new Command("Help", "Help with long label", Command.HELP, 1);

    private Command cmdExit = new Command("Exit", "Exit with very long label", Command.EXIT, 1);

    private Command cmdExtra = new Command("Extra", "Extra command for Options menu", Command.SCREEN,1);
    //command to return to main screen
    private Command cmdReturn = new Command("Return", Command.SCREEN, 1);

    Timer timer;
    private CommandOkTimerTask okTimerTask;

    /**
     *
     */
    public Test6()
    {
        super("Test6");
        timer = new Timer();
        okTimerTask = new CommandOkTimerTask();
        str = new StringItem("Result:", "None");
        append(str);
        addCommand(cmdExit);
        addCommand(cmdExtra);
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
            if (count == 9)
            {
                cancel();
            }
            switch (count)
            {
            case 1:
                addCommand(cmdOk);
                str.setText("The label of Command in OPTIONS MENU: " + " "+ cmdOk.getLongLabel());
                break;
            case 2:
                removeCommand(cmdOk);
                addCommand(cmdScreen);
                str.setText("The label of Command in OPTIONS MENU: " + " "+ cmdScreen.getLongLabel());
                break;
            case 3:
                removeCommand(cmdScreen);
                addCommand(cmdItem);
                str.setText("The label of Command in OPTIONS MENU: " + " "+ cmdItem.getLongLabel());
                break;
            case 4:
                removeCommand(cmdItem);
                addCommand(cmdCancel);
                str.setText("The label of Command is " + " "+ cmdCancel.getLabel());
                break;
            case 5:
                removeCommand(cmdCancel);
                addCommand(cmdBack);
                str.setText("The label of Command is " + " "+ cmdBack.getLabel());
                break;
            case 6:
                removeCommand(cmdBack);
                addCommand(cmdStop);
                str.setText("The label of Command is " + " "+ cmdStop.getLabel());
                break;
            case 7:
                removeCommand(cmdStop);
                addCommand(cmdHelp);
                str.setText("The label of Command in OPTIONS MENU: " + " "+ cmdHelp.getLongLabel());
                break;
            case 8:
                removeCommand(cmdHelp);
                addCommand(cmdExit);
                str.setText("The label of Command is " + " "+ cmdExit.getLabel());
                break;
            case 9:
                removeCommand(cmdExit);
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


