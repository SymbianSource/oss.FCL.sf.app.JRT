/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;

/**
 * This class tests fillArc() method functionality.
 */

public class CanvasTests_01 extends Canvas implements CommandListener
{

    // Reference to MIDlet
    private MIDlet parent = null;
    // Commands for the canvas
    private Command cmdReset = new Command("Reset values", Command.SCREEN, 1);
    private Command cmdSize = new Command("Toggle size", Command.SCREEN, 1);
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    private int startAngle = 0;
    private int arcAngle = 0;
    private int size = 30;

    /**
     *
     * @param parent The parent MIDlet
     */
    public CanvasTests_01(MIDlet parent)
    {
        this.parent = parent;
        addCommand(cmdReset);
        addCommand(cmdSize);
        addCommand(cmdExit);
        setCommandListener(this);

        setFullScreenMode(true);
        Display.getDisplay(parent).setCurrent(this);
    }

    protected void paint(Graphics g)
    {
        g.setGrayScale(255);
        g.fillRect(0, 0, getWidth(), getHeight());
        int height = g.getFont().getHeight();

        g.setGrayScale(0);
        g.drawString("fillArc function test", 0, 0, Graphics.TOP|Graphics.LEFT);

        g.drawString("fillArc(30,60," + size + "," + size + "," + startAngle + "," + arcAngle + ")", 0, 30, Graphics.TOP|Graphics.LEFT);
        g.fillArc(30, 60, size, size, startAngle, arcAngle);
    }

    public void commandAction(Command c, Displayable s)
    {
        if(c == cmdReset)
        {
            startAngle = 0;
            arcAngle = 0;
            size = 30;
        }
        else if(c == cmdSize)
        {
            size += 10;
            if(size > 300)
                size = 10;
        }
        else if(c == cmdExit)
        {
            parent.notifyDestroyed();
        }
    }

    protected void keyPressed(int kyCode)
    {
        handleKeyPress(kyCode);
    }

    protected void keyRepeated(int kyCode)
    {
        handleKeyPress(kyCode);
    }

    protected void handleKeyPress(int kyCode)
    {
        if(getGameAction(kyCode) == Canvas.UP)
        {
            startAngle++;
        }
        else if(getGameAction(kyCode) == Canvas.DOWN)
        {
            startAngle--;
        }
        else if(getGameAction(kyCode) == Canvas.LEFT)
        {
            arcAngle--;
        }
        else if(getGameAction(kyCode) == Canvas.RIGHT)
        {
            arcAngle++;
        }
        else if(getGameAction(kyCode) == Canvas.GAME_A)
        {
            size += 10;
            if(size > 300)
                size = 10;
        }
        repaint();
        serviceRepaints();
    }
}
