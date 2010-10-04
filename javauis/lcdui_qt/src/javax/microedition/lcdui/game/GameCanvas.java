/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
package javax.microedition.lcdui.game;

import javax.microedition.lcdui.Graphics;

import com.nokia.mj.impl.nokialcdui.LCDUIInvoker;

/**
 * GameCanvas class.
 */
public abstract class GameCanvas extends javax.microedition.lcdui.Canvas
{

    /**
     * Bit representing UP key. (1 << Canvas.UP)
     */
    public static final int UP_PRESSED = 0x0002;

    /**
     * Bit representing DOWN key. (1 << Canvas.DOWN)
     */
    public static final int DOWN_PRESSED = 0x0040;

    /**
     * Bit representing LEFT key. (1 << Canvas.LEFT)
     */
    public static final int LEFT_PRESSED = 0x0004;

    /**
     * Bit representing RIGHT key. (1 << Canvas.RIGHT)
     */
    public static final int RIGHT_PRESSED = 0x0020;

    /**
     * Bit representing FIRE key. (1 << Canvas.FIRE)
     */
    public static final int FIRE_PRESSED = 0x0100;

    /**
     * Bit representing GAME_A key. (1 << Canvas.GAME_A)
     */
    public static final int GAME_A_PRESSED = 0x0200;

    /**
     * Bit representing GAME_B key. (1 << Canvas.GAME_B)
     */
    public static final int GAME_B_PRESSED = 0x0400;

    /**
     * Bit representing GAME_C key. (1 << Canvas.GAME_C)
     */
    public static final int GAME_C_PRESSED = 0x0800;

    /**
     * Bit representing GAME_D key. (1 << Canvas.GAME_D)
     */
    public static final int GAME_D_PRESSED = 0x1000;

    /**
     * Constructor.
     *
     * @param suppressKeyEvents supress game key events.
     */
    protected GameCanvas(boolean suppressKeyEvents)
    {
        super();
        LCDUIInvoker.initGameCanvas(this, suppressKeyEvents);
    }

    /**
     * Get frameBuffer's Graphics object.
     *
     * @return frameBuffer's Graphics
     */
    protected Graphics getGraphics()
    {
        return (Graphics) LCDUIInvoker.getGraphics(this);
    }

    /**
     * Get game key states.
     *
     * @return game key states
     */
    public int getKeyStates()
    {
        return LCDUIInvoker.getKeyStates(this);
    }

    /**
     * By default this renders the frameBuffer at (0,0).
     *
     * @param aGraphics Graphics object
     * @throws NullPointerException if aGraphics is null
     */
    public void paint(javax.microedition.lcdui.Graphics aGraphics)
    {
        // Graphics can not be null
        if (aGraphics == null)
        {
            throw new NullPointerException();
        }

        // If the Graphics doesn't belong to the frame buffer
        LCDUIInvoker.renderGraphics(this, aGraphics);
    }

    /**
     * Flushes the frameBuffer's region to the display.
     *
     * @param x clip X
     * @param y clip Y
     * @param w clip width
     * @param h clip height
     */
    public void flushGraphics(int x, int y, int w, int h)
    {
        if(w > 0 && h > 0)
        {
            LCDUIInvoker.flushGraphics(this, x, y, w, h);
        }
    }

    /**
     * Flushes the frameBuffer to the display.
     */
    public void flushGraphics()
    {
        LCDUIInvoker.flushGraphics(this, 0, 0, getWidth(), getHeight());
    }

}
