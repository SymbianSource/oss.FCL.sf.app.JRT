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
 */

public class Test7 implements CommandListener
{

    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    //command to return to main screen
    private Command cmdReturn = new Command("Return", Command.SCREEN, 1);

    private TestCanvas testCanvas = null;
    int width, height;

    public String commandDisplay1 = null;
    public String commandDisplay2 = null;

    /**
     * This constructor creates cg with the elements inside and adds Commands to
     * add and remove elements from the cg.The cg is appended to this form.
     * Displays the current screen to be this form
     */
    public Test7()
    {
        testCanvas = new TestCanvas();

        testCanvas.addCommand(cmdExit);
        testCanvas.addCommand(cmdReturn);
        testCanvas.setCommandListener(this);
        commandDisplay1 = new String("Midp_Command_02 MIDlet");
        commandDisplay2 = new String("Exit on RSK and Return in Options");
        Midp_Command_02.getDisplay().setCurrent(testCanvas);
        width = testCanvas.getWidth();
        height = testCanvas.getHeight();

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
            Midp_Command_02.goBack();
        }
        if (c == cmdReturn)
        {
            Midp_Command_02.goBack();
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

