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

import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;
import java.io.*;

/**
 * This class is to test the below test cases:
 *
 */

public class CanvasTests extends Canvas implements CommandListener
{

    //reference to MIDlet
    private Midp_Canvas_02 parent = null;
    private String info = null;
    private boolean iToggle = true;
    private Command cmd = new Command("Repaint", Command.SCREEN, 1);
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);
    /**
     *
     * @param parent The parent MIDlet
     */
    public CanvasTests(Midp_Canvas_02 parent)
    {
        this.parent = parent;
        addCommand(cmd);
        addCommand(cmdExit);
        setCommandListener(this);

        Display.getDisplay(parent).setCurrent(this);
    }

    protected void paint(Graphics g)
    {
        if (info == "repaint")
        {
            g.setGrayScale(100);
        }
        else
        {
            g.setGrayScale(255);
        }
        g.fillRect(0, 0, getWidth(), getHeight());
        g.setGrayScale(0);
        info = info +" "+ getWidth()+" "+ getHeight();
        g.drawString(info , 0, 0, Graphics.TOP|Graphics.LEFT);

    }

    protected void sizeChanged(int w, int h)
    {
        info = "sizeChanged called";
    }

    protected void showNotify()
    {
        info = "showNotify called";
    }

    protected void keyPressed(int keyCode)
    {
        if (keyCode == KEY_NUM8)
        {
            if (iToggle)
            {
                setFullScreenMode(iToggle);
                iToggle = false;
            }
            else if (!iToggle)
            {
                setFullScreenMode(iToggle);
                iToggle = true;
            }
        }
        else if (keyCode == KEY_NUM1)
        {
            info = "repaint";
            repaint(0,0,50,50);
            serviceRepaints();
        }
        else if (keyCode == KEY_NUM3)
        {
            info = "repaint";
            repaint(getWidth()-50,0,50,50);
            serviceRepaints();
        }
        else if (keyCode == KEY_NUM7)
        {
            info = "repaint";
            repaint(0,getHeight()-50,50,50);
            serviceRepaints();
        }
        else if (keyCode == KEY_NUM5)
        {
            info = "repaint";
            repaint((getWidth()/2)-25,(getHeight()/2)-25,50,50);
            serviceRepaints();
        }
        else if (keyCode == KEY_NUM9)
        {
            info = "repaint";
            repaint(getWidth()-50,getHeight()-50,50,50);
            serviceRepaints();
        }
    }

    public void commandAction(Command c, Displayable s)
    {
        if (c == cmd)
        {
            info = "repaint";
            repaint(20,20,50,50);
            serviceRepaints();
        }
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
}



