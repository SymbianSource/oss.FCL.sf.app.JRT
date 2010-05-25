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
public class Test2 implements CommandListener
{

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

    private TestCanvas testCanvas = null;

    Timer timer;

    private CommandOkTimerTask okTimerTask;

    private String commandDisplay1 = null;
    private String commandDisplay2 = null;
    private String commandDisplay3 = null;
    /**
     *
     */
    public Test2()
    {
        timer = new Timer();
        testCanvas = new TestCanvas();
        okTimerTask = new CommandOkTimerTask();
        commandDisplay1 = new String("Result: ");
        commandDisplay2 = new String("-SK: truncated Back_with_very_long_label");
        commandDisplay3 = new String("");
        testCanvas.addCommand(cmdBackLong);

        Midp_Command_02.getDisplay().setCurrent(testCanvas);
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
            if (count == 11)
            {
                cancel();
            }
            switch (count)
            {
            case 1:
                testCanvas.removeCommand(cmdBackLong);
                testCanvas.addCommand(cmdBack);
                testCanvas.addCommand(cmdCancel);
                commandDisplay1 = new String("Result: ");
                commandDisplay2 = new String("-SK: Cancel");
                commandDisplay3 = new String("+SK: Back");
                testCanvas.repaint();
                break;
            case 2:
                testCanvas.removeCommand(cmdCancel);
                testCanvas.addCommand(cmdStop);
                commandDisplay1 = new String("Result: ");
                commandDisplay2 = new String("-SK: Stop");
                commandDisplay3 = new String("+SK: Back");
                testCanvas.repaint();
                break;
            case 3:
                testCanvas.removeCommand(cmdStop);
                testCanvas.addCommand(cmdExit);
                commandDisplay1 = new String("Result: ");
                commandDisplay2 = new String("-SK: Back");
                commandDisplay3 = new String("+SK: Exit");
                testCanvas.repaint();
                break;
            case 4:
                testCanvas.removeCommand(cmdStop);
                testCanvas.addCommand(cmdStopLong);
                testCanvas.addCommand(cmdCancel);
                commandDisplay1 = new String("Result: ");
                commandDisplay2 = new String("-SK: truncated Stop_with_very_long_label");
                commandDisplay3 = new String("+SK: Options: Cancel, Back, Exit");
                testCanvas.repaint();
                break;
            case 5:
                testCanvas.removeCommand(cmdStopLong);
                testCanvas.removeCommand(cmdBack);
                testCanvas.removeCommand(cmdExit);
                testCanvas.removeCommand(cmdStopLong);
                commandDisplay1 = new String("Result: ");
                commandDisplay2 = new String("-SK: Cancel");
                commandDisplay3 = new String("");
                testCanvas.repaint();
                break;
            case 6:
                testCanvas.addCommand(cmdStop);
                commandDisplay1 = new String("Result: ");
                commandDisplay2 = new String("-SK: Stop");
                commandDisplay3 = new String("+SK: Cancel");
                testCanvas.repaint();
                break;
            case 7:
                testCanvas.removeCommand(cmdStop);
                testCanvas.addCommand(cmdExit);
                commandDisplay1 = new String("Result: ");
                commandDisplay2 = new String("-SK: Cancel");
                commandDisplay3 = new String("+SK: Exit");
                testCanvas.repaint();
                break;
            case 8:
                testCanvas.addCommand(cmdStop);
                commandDisplay1 = new String("Result: ");
                commandDisplay2 = new String("-SK: Stop");
                commandDisplay3 = new String("+SK: Options: Cancel, Exit");
                testCanvas.repaint();
                break;
            case 9:
                testCanvas.removeCommand(cmdCancel);
                testCanvas.removeCommand(cmdExit);
                commandDisplay1 = new String("Result: ");
                commandDisplay2 = new String("-SK: Stop");
                commandDisplay3 = new String("");
                testCanvas.repaint();
                break;
            case 10:
                testCanvas.addCommand(cmdExit);
                commandDisplay1 = new String("Result: ");
                commandDisplay2 = new String("-SK: Stop");
                commandDisplay3 = new String("+SK: Exit");
                testCanvas.repaint();
                break;
            case 11:
                testCanvas.removeCommand(cmdStop);
                testCanvas.addCommand(cmdReturn);
                commandDisplay1 = new String("Result: ");
                commandDisplay2 = new String("-SK: Exit");
                commandDisplay3 = new String("");
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

        int width, height;

        public TestCanvas()
        {
            width = this.getWidth();
            height = this.getHeight();
        }

        protected void sizeChanged(int w,int h)
        {
            this.width = w;
            this.height = h;
        }

        protected void paint(Graphics g)
        {
            g.setColor(255,255,255);
            g.fillRect(0,0,width,height);
            g.setColor(0,0,0);

            g.drawString(commandDisplay1, 10, 15, Graphics.TOP | Graphics.LEFT);
            g.drawString(commandDisplay2, 10, 35, Graphics.TOP | Graphics.LEFT);
            g.drawString(commandDisplay3, 10, 55, Graphics.TOP | Graphics.LEFT);
            return;
        }

    }
}


