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
 * This is to test appending a choicegroup without label, also test that removing
 * the icons works ok.
 */

public class Test9 extends Form implements CommandListener
{

    //the ChoiceGroup to test
    public ChoiceGroup cg = null;
    public ChoiceGroup cg2 = null;

    //the command to go back to the ChoiceGroupTests screen
    Command cmdBack = new Command("Back", Command.BACK, 1);

    //Commands for running the tests.
    Command cmdTest1 = new Command("Add without label", Command.SCREEN, 1);
    Command cmdTest2 = new Command("Remove icons", Command.SCREEN, 1);

    Timer timer;
    private CommandOkTimerTask okTimerTask;

    /**
    *
    * @param cgType The ChoiceGroup type
    */
    public Test9(int cgType)
    {

        super("Test9");
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
        addCommand(cmdTest2);
        setCommandListener(this);
        cg = new ChoiceGroup(cgLabel, cgType);
        append(cg);
        appendElements(cg);
        cg2 = new ChoiceGroup(null, cgType);
        appendElements(cg2);

        timer = new Timer();
        okTimerTask = new CommandOkTimerTask();
    }

    private void appendElements(ChoiceGroup choicegroup)
    {
        //add two elements with image
        choicegroup.append("1st Element", Images.getInstance().smallImage);
        choicegroup.append("2nd Element", Images.getInstance().smallImage);
        //add two elements without image
        choicegroup.append("3rd Element", null);
        choicegroup.append("4th Element", null);
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
            append(cg2);
        }
        else if (c == cmdTest2)
        {
            timer.schedule(okTimerTask, 5000, 3000);
            removeCommand(cmdTest2);
        }
    }

    public void removeIcons()
    {
        for (int i = 0; i < cg.size(); i++)
        {
            if (cg.getImage(i) != null)
                cg.set(i, "Icon " + (i + 1) + " removed.", null);
            else
                cg.set(i, "" + (i + 1) + " had no Icon", null);
        }
    }

    public class CommandOkTimerTask extends TimerTask
    {
        private int count = 1;
        public void run()
        {
            if (count > 1)
            {
                cancel();
            }
            switch (count)
            {
            case 1:
                removeIcons();
                break;
            default:
                return;
            }
            count++;
        }
    }
}
