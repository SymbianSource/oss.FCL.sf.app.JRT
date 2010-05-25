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
 * -Same Commands with different priority
 */

public class Test4 extends Form implements CommandListener
{

    //the StringItem to show the result of each test case
    private StringItem str = null;

    //same Commands with different type
    private Command cmdOk1 = new Command("Ok1", Command.OK, 10);
    private Command cmdOk2 = new Command("Ok2", Command.OK, 5);


    private Command cmdScreen1 = new Command("Screen1", Command.SCREEN, 10);
    private Command cmdScreen2 = new Command("Screen2", Command.SCREEN, 5);

    private Command cmdItem1 = new Command("Item1", Command.ITEM, 10);
    private Command cmdItem2 = new Command("Item2", Command.ITEM, 5);

    private Command cmdCancel1 = new Command("Cancel1", Command.CANCEL, 10);
    private Command cmdCancel2 = new Command("Cancel2", Command.CANCEL, 5);

    private Command cmdBack1 = new Command("Back1", Command.BACK, 10);
    private Command cmdBack2 = new Command("Back2", Command.BACK, 5);

    private Command cmdStop1 = new Command("Stop1", Command.STOP, 10);
    private Command cmdStop2 = new Command("Stop2", Command.STOP, 5);

    private Command cmdHelp1 = new Command("Help1", Command.HELP, 10);
    private Command cmdHelp2 = new Command("Help2", Command.HELP, 5);

    //command to return to main screen
    private Command cmdReturn = new Command("Return", Command.SCREEN, 1);

    Timer timer;
    private CommandOkTimerTask okTimerTask;

    /**
     *
     */
    public Test4()
    {
        super("Test4");
        timer = new Timer();
        okTimerTask = new CommandOkTimerTask();
        str = new StringItem("Result:", "None");
        append(str);
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
            if (count == 8)
            {
                cancel();
            }
            switch (count)
            {
            case 1:
                addCommand(cmdOk1);
                addCommand(cmdOk2);
                str.setText("Ok2 and Ok1 in this order on Options list");
                break;
            case 2:
                removeCommand(cmdOk1);
                removeCommand(cmdOk2);
                addCommand(cmdScreen1);
                addCommand(cmdScreen2);
                str.setText("Screen2 and Screen1 in this order on Options list");
                break;
            case 3:
                removeCommand(cmdScreen1);
                removeCommand(cmdScreen2);
                addCommand(cmdItem1);
                addCommand(cmdItem2);
                str.setText("Item2 and Item1 in this order on Options list");
                break;
            case 4:
                removeCommand(cmdItem1);
                removeCommand(cmdItem2);
                addCommand(cmdCancel1);
                addCommand(cmdCancel2);
                str.setText("Cancel2 on right soft key and Cancel1 on Options list");
                break;
            case 5:
                removeCommand(cmdCancel1);
                removeCommand(cmdCancel2);
                addCommand(cmdBack1);
                addCommand(cmdBack2);
                str.setText("Back2 on right soft key and Back1 on Options list");
                break;
            case 6:
                removeCommand(cmdBack1);
                removeCommand(cmdBack2);
                addCommand(cmdStop1);
                addCommand(cmdStop2);
                str.setText("Stop2 on right soft key and Stop1 on Options list");
                break;
            case 7:
                removeCommand(cmdStop1);
                removeCommand(cmdStop2);
                addCommand(cmdHelp1);
                addCommand(cmdHelp2);
                str.setText("Help2 and Help1 in this order on Options list");
                break;
            case 8:
                removeCommand(cmdHelp1);
                removeCommand(cmdHelp2);
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


