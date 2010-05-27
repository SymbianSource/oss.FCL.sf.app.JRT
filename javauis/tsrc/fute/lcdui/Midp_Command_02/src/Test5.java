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
 * -Canvas:If Command label is empty default label is used
 */

public class Test5 implements CommandListener
{

    //Commands with null label
    private Command cmdOk = new Command("", Command.OK, 1);

    private Command cmdScreen = new Command("", Command.SCREEN, 1);

    private Command cmdItem = new Command("", Command.ITEM, 1);

    private Command cmdCancel = new Command("", Command.CANCEL, 1);

    private Command cmdBack = new Command("", Command.BACK, 1);

    private Command cmdStop = new Command("", Command.STOP, 1);

    private Command cmdHelp = new Command("", Command.HELP, 1);

    private Command cmdExit = new Command("", Command.EXIT, 1);

    //command to return to main screen
    private Command cmdReturn = new Command("Return", Command.SCREEN, 1);

    Timer timer;
    private CommandOkTimerTask okTimerTask;
    private String commandDisplay1 = null;
    private String commandDisplay2 = null;
    private int type;
    private TestCanvas testCanvas;
    int width, height;

    /**
     *
     */
    public Test5()
    {
        timer = new Timer();
        testCanvas = new TestCanvas();
        okTimerTask = new CommandOkTimerTask();
        commandDisplay1 = new String("Result: None");
        Midp_Command_02.getDisplay().setCurrent(testCanvas);
        width = testCanvas.getWidth();
        height = testCanvas.getHeight();
        timer.schedule(okTimerTask, 2000, 6000);
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
            if (count == 9)
            {
                cancel();
            }
            switch (count)
            {
            case 1:
                testCanvas.addCommand(cmdOk);
                commandDisplay1 = new String("Options list: ");
                commandDisplay2 = new String("default label for type OK");

                testCanvas.repaint();
                break;
            case 2:
                testCanvas.removeCommand(cmdOk);
                testCanvas.addCommand(cmdScreen);
                commandDisplay1 = new String("Options list: ");
                commandDisplay2 = new String("default label for type SCREEN");
                testCanvas.repaint();
                break;
            case 3:
                testCanvas.removeCommand(cmdScreen);
                testCanvas.addCommand(cmdItem);
                commandDisplay1 = new String("Options list: ");
                commandDisplay2 = new String("default label for type ITEM");
                testCanvas.repaint();
                break;
            case 4:
                testCanvas.removeCommand(cmdItem);
                testCanvas.addCommand(cmdCancel);
                commandDisplay1 = new String("RSK: ");
                commandDisplay2 = new String("default label for type CANCEL");
                testCanvas.repaint();
                break;
            case 5:
                testCanvas.removeCommand(cmdCancel);
                testCanvas.addCommand(cmdBack);
                commandDisplay1 = new String("RSK: ");
                commandDisplay2 = new String("default label for type BACK");
                testCanvas.repaint();
                break;
            case 6:
                testCanvas.removeCommand(cmdBack);
                testCanvas.addCommand(cmdStop);
                commandDisplay1 = new String("RSK: ");
                commandDisplay2 = new String("default label for type STOP");
                testCanvas.repaint();
                break;
            case 7:
                testCanvas.removeCommand(cmdStop);
                testCanvas.addCommand(cmdHelp);
                commandDisplay1 = new String("Options list: ");
                commandDisplay2 = new String("default label for type HELP");
                testCanvas.repaint();
                break;
            case 8:
                testCanvas.removeCommand(cmdHelp);
                testCanvas.addCommand(cmdExit);
                commandDisplay1 = new String("RSK: ");
                commandDisplay2 = new String("default label for type EXIT");
                testCanvas.repaint();
                break;
            case 9:
                testCanvas.removeCommand(cmdExit);
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

            g.drawString(commandDisplay1, 7, 5, Graphics.TOP | Graphics.LEFT);
            g.drawString(commandDisplay2, 7, 20, Graphics.TOP | Graphics.LEFT);
            return;
        }
    }

}


