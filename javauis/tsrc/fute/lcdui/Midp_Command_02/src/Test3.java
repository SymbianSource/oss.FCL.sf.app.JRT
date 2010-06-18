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
 * -Commands with same type and same priority
 */

public class Test3 implements CommandListener
{

    //the Commands with same type
    private Command cmdOk1 = new Command("Ok1", Command.OK, 1);
    private Command cmdOk2 = new Command("Ok2", Command.OK, 1);


    private Command cmdScreen1 = new Command("Screen1", Command.SCREEN, 1);
    private Command cmdScreen2 = new Command("Screen2", Command.SCREEN, 1);

    private Command cmdItem1 = new Command("Item1", Command.ITEM, 1);
    private Command cmdItem2 = new Command("Item2", Command.ITEM, 1);

    private Command cmdCancel1 = new Command("Cancel1", Command.CANCEL, 1);
    private Command cmdCancel2 = new Command("Cancel2", Command.CANCEL, 1);

    private Command cmdBack1 = new Command("Back1", Command.BACK, 1);
    private Command cmdBack2 = new Command("Back2", Command.BACK, 1);

    private Command cmdStop1 = new Command("Stop1", Command.STOP, 1);
    private Command cmdStop2 = new Command("Stop2", Command.STOP, 1);

    private Command cmdHelp1 = new Command("Help1", Command.HELP, 1);
    private Command cmdHelp2 = new Command("Help2", Command.HELP, 1);

    //command to return to main screen
    private Command cmdReturn = new Command("Return", Command.SCREEN, 1);

    Timer timer;
    private CommandOkTimerTask okTimerTask;

    private TestCanvas testCanvas;

    private String commandDisplay1;
    private String commandDisplay2;
    int width, height;

    /**
     *
     */
    public Test3()
    {
        timer = new Timer();
        testCanvas = new TestCanvas();
        okTimerTask = new CommandOkTimerTask();
        commandDisplay1 = new String("Result: None");
        Midp_Command_02.getDisplay().setCurrent(testCanvas);
        width = testCanvas.getWidth();
        height = testCanvas.getHeight();
        timer.schedule(okTimerTask, 2000, 6000);
        testCanvas.repaint();
        testCanvas.setCommandListener(this);
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
            Midp_Command_02.goBack();
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
                testCanvas.addCommand(cmdOk1);
                testCanvas.addCommand(cmdOk2);
                commandDisplay1 = new String("Result (in this order):");
                commandDisplay2 = new String("Options list: Ok1, Ok2");
                testCanvas.repaint();
                break;
            case 2:
                testCanvas.removeCommand(cmdOk1);
                testCanvas.removeCommand(cmdOk2);
                testCanvas.addCommand(cmdScreen1);
                testCanvas.addCommand(cmdScreen2);
                commandDisplay1 = new String("Result (in this order):");
                commandDisplay2 = new String("Options list: Screen1, Screen2");
                testCanvas.repaint();
                break;
            case 3:
                testCanvas.removeCommand(cmdScreen1);
                testCanvas.removeCommand(cmdScreen2);
                testCanvas.addCommand(cmdItem1);
                testCanvas.addCommand(cmdItem2);
                commandDisplay1 = new String("Result (in this order):");
                commandDisplay2 = new String("Options list: Item1, Item2");
                testCanvas.repaint();
                break;
            case 4:
                testCanvas.removeCommand(cmdItem1);
                testCanvas.removeCommand(cmdItem2);
                testCanvas.addCommand(cmdCancel1);
                testCanvas.addCommand(cmdCancel2);
                commandDisplay1 = new String("Result:");
                commandDisplay2 = new String("Options list: Cancel2, RSK: Cancel1");
                testCanvas.repaint();
                break;
            case 5:
                testCanvas.removeCommand(cmdCancel1);
                testCanvas.removeCommand(cmdCancel2);
                testCanvas.addCommand(cmdBack1);
                testCanvas.addCommand(cmdBack2);
                commandDisplay1 = new String("Result:");
                commandDisplay2 = new String("Options list: Back2, RSK: Back1");
                testCanvas.repaint();
                break;
            case 6:
                testCanvas.removeCommand(cmdBack1);
                testCanvas.removeCommand(cmdBack2);
                testCanvas.addCommand(cmdStop1);
                testCanvas.addCommand(cmdStop2);
                commandDisplay1 = new String("Result:");
                commandDisplay2 = new String("Options list: Stop2, RSK: Stop1");
                testCanvas.repaint();
                break;
            case 7:
                testCanvas.removeCommand(cmdStop1);
                testCanvas.removeCommand(cmdStop2);
                testCanvas.addCommand(cmdHelp1);
                testCanvas.addCommand(cmdHelp2);
                commandDisplay1 = new String("Result (in this order):");
                commandDisplay2 = new String("Options list: Help1, Help2");
                testCanvas.repaint();
                break;
            case 8:
                testCanvas.removeCommand(cmdHelp1);
                testCanvas.removeCommand(cmdHelp2);
                testCanvas.addCommand(cmdReturn);
                commandDisplay1 = new String("");
                commandDisplay2 = new String("Choose Options->Return");
                testCanvas.repaint();
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

            g.drawString(commandDisplay1, 0, 5, Graphics.TOP | Graphics.LEFT);
            g.drawString(commandDisplay2, 7, 20, Graphics.TOP | Graphics.LEFT);
            return;
        }
    }
}


