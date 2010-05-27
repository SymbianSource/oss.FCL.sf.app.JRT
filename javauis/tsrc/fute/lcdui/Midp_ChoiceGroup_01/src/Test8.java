/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * This is to test deletion of element with or without icon. Main purpose
 * is to run this on Popup ChoiceGroup when the list is open.
 */

public class Test8 extends Form implements CommandListener
{

    //the ChoiceGroup to test
    public ChoiceGroup cg = null;

    //the command to go back to the ChoiceGroupTests screen
    Command cmdBack = new Command("Back", Command.BACK, 1);

    //Commands for running the tests.
    Command cmdTest1 = new Command("Test1, 5 sec delay", Command.SCREEN, 1);

    Timer timer;
    private CommandOkTimerTask okTimerTask;

    /**
    *
    * @param cgType The ChoiceGroup type
    */
    public Test8(int cgType)
    {

        super("Test8");
        String cgLabel = null;
        //set the ChoiceGroup label according to the type of ChoiceGroup "cg"
        if (cgType == ChoiceGroup.MULTIPLE)
            cgLabel = "MULTIPLE";
        else if (cgType == ChoiceGroup.EXCLUSIVE)
            cgLabel = "EXCLUSIVE";
        else if (cgType == ChoiceGroup.POPUP)
            cgLabel = "POPUP";
        addCommand(cmdBack);
        addCommand(cmdTest1);
        setCommandListener(this);
        cg = new ChoiceGroup(cgLabel, cgType);
        append(cg);
        //add two elements with image
        cg.append("1st Element", Images.getInstance().smallImage);
        cg.append("2nd Element", Images.getInstance().smallImage);
        //add two elements without image
        cg.append("3rd Element", null);
        cg.append("4th Element", null);

        timer = new Timer();
        okTimerTask = new CommandOkTimerTask();
    }

    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {
        if (c == cmdBack)
            ChoiceGroupTests.show();
        else if (c == cmdTest1)
        {
            timer.schedule(okTimerTask, 5000, 3000);
            removeCommand(cmdTest1);
        }
    }

    public class CommandOkTimerTask extends TimerTask
    {
        private int count = 1;
        public void run()
        {
            if (count > 4)
            {
                cancel();
            }
            switch (count)
            {
            case 1:
                // Delete "1st Element"
                cg.delete(0);
                break;
            case 2:
                // Delete "3rd Element"
                cg.delete(1);
                break;
            case 3:
                // Delete "4th Element"
                cg.delete(1);
                break;
            case 4:
                // Delete "2nd Element"
                cg.delete(0);
                break;
            default:
                return;
            }
            count++;
        }
    }
}
