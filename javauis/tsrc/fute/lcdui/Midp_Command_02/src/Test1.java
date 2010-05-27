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
public class Test1 implements CommandListener
{

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

    private TestCanvas testCanvas = null;
    int width, height;

    public String commandDisplay1 = null;
    public String commandDisplay2 = null;

    /**
     * This constructor creates cg with the elements inside and adds Commands to
     * add and remove elements from the cg.The cg is appended to this form.
     * Displays the current screen to be this form
     */
    public Test1()
    {
        timer = new Timer();
        testCanvas = new TestCanvas();
        okTimerTask = new CommandOkTimerTask();
        testCanvas.addCommand(cmdExit);
        testCanvas.setCommandListener(this);
        Midp_Command_02.getDisplay().setCurrent(testCanvas);
        width = testCanvas.getWidth();
        height = testCanvas.getHeight();
        commandDisplay1 = new String("Midp_Command_02 MIDlet");
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
            commandDisplay1 = new String("Command: ");
            commandDisplay2 = new String(c.getLabel());
            testCanvas.repaint();
        }
        if (c == cmdReturn)
        {
            Midp_Command_02.goBack();
        }
        else
        {
            commandDisplay1 = new String("Command: ");
            commandDisplay2 = new String(c.getLabel());
            testCanvas.repaint();
        }
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
                testCanvas.removeCommand(cmdExit);
                testCanvas.addCommand(cmdOk);
                break;
            case 2:
                testCanvas.removeCommand(cmdOk);
                testCanvas.addCommand(cmdScreen);
                break;
            case 3:
                testCanvas.removeCommand(cmdScreen);
                testCanvas.addCommand(cmdItem);
                break;
            case 4:
                testCanvas.removeCommand(cmdItem);
                testCanvas.addCommand(cmdCancel);
                break;
            case 5:
                testCanvas.removeCommand(cmdCancel);
                testCanvas.addCommand(cmdBack);
                break;
            case 6:
                testCanvas.removeCommand(cmdBack);
                testCanvas.addCommand(cmdStop);
                break;
            case 7:
                testCanvas.removeCommand(cmdStop);
                testCanvas.addCommand(cmdHelp);
                testCanvas.addCommand(cmdReturn);
                break;
            default:
                return;
            }
            count++;
            System.out.println("count:" + count);
        }
    }
    public class TestCanvas extends Canvas
    {

        protected void paint(Graphics g)
        {
            g.setColor(255,255,255);
            g.fillRect(0,0,width,height);
            g.setColor(0,0,0);
            g.drawString(commandDisplay1, 0, 20, Graphics.TOP | Graphics.LEFT);
            g.drawString(commandDisplay2, 10, 40, Graphics.TOP | Graphics.LEFT);
            return;
        }

    }

}

