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

/**
 * This class is to test the below test cases:
 * -Key codes of standard or default keys
 * -Key codes of additional keys
 * -Key codes to game actions mapping
 *
 */

public class CanvasTests extends Canvas implements CommandListener
{

    //reference to MIDlet
    private Midp_Canvas_01 parent = null;
    private Command cmdGameActions = new Command("Game actions info", Command.SCREEN, 1);
    private Command cmdBack = new Command("Back", Command.BACK, 1);
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    private String  keyCode = "KeyCode: None";
    private String keyName = "KeyName: None";
    private String gameAction = "GameAction KeyCode: None";

    /**
     *
     * @param parent The parent MIDlet
     */
    public CanvasTests(Midp_Canvas_01 parent)
    {
        this.parent = parent;
        addCommand(cmdGameActions);
        addCommand(cmdExit);
        setCommandListener(this);
        Display.getDisplay(parent).setCurrent(this);
    }

    protected void paint(Graphics g)
    {
        g.setGrayScale(255);
        g.fillRect(0, 0, getWidth(), getHeight());
        g.setGrayScale(0);
        int height = g.getFont().getHeight();
        g.drawString("Canvas key codes test", 0, 0, Graphics.TOP|Graphics.LEFT);
        g.drawString(keyCode, 0, height, Graphics.TOP|Graphics.LEFT);
        g.drawString(keyName, 0, height*2, Graphics.TOP|Graphics.LEFT);
        g.drawString(gameAction, 0, height*4, Graphics.TOP|Graphics.LEFT);
    }

    protected void keyPressed(int kyCode)
    {
        keyCode = "KeyCode: "+ kyCode;
        keyName = "KeyName: "+getKeyName(kyCode);
        gameAction = "GameAction KeyCode: " + getGameAction(kyCode);
        repaint();
        serviceRepaints();
    }

    private String getGameActionsInfo()
    {
        String info = "Default game\naction mapping:\n" +
                      "ACTION: code (name)\n" +
                      "FIRE: " + getActionString(Canvas.FIRE) +
                      "UP: " + getActionString(Canvas.UP) +
                      "DOWN: " + getActionString(Canvas.DOWN) +
                      "LEFT: " + getActionString(Canvas.LEFT) +
                      "RIGHT: " + getActionString(Canvas.RIGHT) +
                      "GAME_A: " + getActionString(Canvas.GAME_A) +
                      "GAME_B: " + getActionString(Canvas.GAME_B) +
                      "GAME_C: " + getActionString(Canvas.GAME_C) +
                      "GAME_D: " + getActionString(Canvas.GAME_D);
        return info;
    }

    private String getActionString(int action)
    {
        return ("" + getKeyCode(action) + " (" + (getGameAction(getKeyCode(action)))+")\n");
    }

    public void commandAction(Command c, Displayable s)
    {
        if (c == cmdGameActions)
        {
            Form form = new Form("Default key codes Info");
            form.addCommand(cmdBack);
            form.setCommandListener(this);
            form.append(getGameActionsInfo());
            Display.getDisplay(parent).setCurrent(form);
        }
        else if (c == cmdBack)
        {
            Display.getDisplay(parent).setCurrent(this);
        }
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
}



