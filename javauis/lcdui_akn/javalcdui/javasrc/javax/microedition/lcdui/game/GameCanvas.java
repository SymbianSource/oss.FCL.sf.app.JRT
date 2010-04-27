/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.Graphics;

import com.nokia.mj.impl.rt.legacy.ToolkitInvoker;

public abstract class GameCanvas extends Canvas
{
    public static final int UP_PRESSED      = 0x0002;
    public static final int DOWN_PRESSED    = 0x0040;
    public static final int LEFT_PRESSED    = 0x0004;
    public static final int RIGHT_PRESSED   = 0x0020;
    public static final int FIRE_PRESSED    = 0x0100;
    public static final int GAME_A_PRESSED  = 0x0200;
    public static final int GAME_B_PRESSED  = 0x0400;
    public static final int GAME_C_PRESSED  = 0x0800;
    public static final int GAME_D_PRESSED  = 0x1000;

    ToolkitInvoker  iInvoker;
    Image           iFrameBuffer;

    protected GameCanvas(boolean aSuppressKeyEvents)
    {
        super();
        iInvoker = ToolkitInvoker.getToolkitInvoker();
        iFrameBuffer = (Image)iInvoker.canvasInitGameCanvas(this, aSuppressKeyEvents);
    }

    protected Graphics getGraphics()
    {
        return (Graphics)iInvoker.canvasGetBufferGraphics(this);
    }

    public int getKeyStates()
    {
        return iInvoker.canvasGetKeyStates(this);
    }

    public void paint(Graphics aGraphics)
    {
        // Graphics can not be null
        if (aGraphics == null)
        {
            throw new NullPointerException();
        }

        //
        // NOTE: If aGraphics was obtained from the frame buffer this
        // method does not render the frame buffer.
        //
        iInvoker.canvasRenderGraphics(this, aGraphics);
    }

    public void flushGraphics(int aX, int aY, int aWidth, int aHeight)
    {
        if (aWidth<=0 || aHeight<=0)
        {
            return;
        }
        try
        {
            iInvoker.canvasFlushGraphics(this, aX, aY, aWidth, aHeight);
        }
        catch (Exception e)
        {
        }
    }

    public void flushGraphics()
    {
        // negative width and height indicates not clipped
        iInvoker.canvasFlushGraphics(this, 0, 0, -1, -1);
    }
}

