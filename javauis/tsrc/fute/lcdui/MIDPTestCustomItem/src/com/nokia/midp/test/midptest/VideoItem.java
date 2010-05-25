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

package com.nokia.midp.test.midptest;

import javax.microedition.lcdui.CustomItem;
import javax.microedition.lcdui.Graphics;
import java.util.Random;

public class VideoItem extends CustomItem implements Runnable
{
    int iX = 1;
    int iDir = 1;

    Random iRandom = new Random();

    public VideoItem()
    {
        super("");
    }

    public void run()
    {
        if (iX > 500)
        {
            iDir = -1;
        }
        else if (iX <= 1)
        {
            iDir = 1;
        }

        iX += 50 * iDir;

        invalidate();
    }

    /**
     * From class CustomItem
     *
     */
    protected void paint(Graphics aGraphics, int aWidth, int aHeight)
    {
        int x = iX;
        aGraphics.setColor(iRandom.nextInt());
        aGraphics.fillRect(0, 0, x, 500);
        aGraphics.setColor(0);
        aGraphics.drawLine(x, 0, x, 500);
    }

    /**
     * From class CustomItem
     *
     */
    protected int getMinContentHeight()
    {
        int minHeight = 50;
        return minHeight;
    }

    /**
     * From class CustomItem
     *
     */
    protected int getMinContentWidth()
    {
        return iX;
    }

    /**
     * From class CustomItem
     *
     */
    protected int getPrefContentHeight(int aWidth)
    {
        int prefHeight = 50;
        return prefHeight;
    }

    /**
     * From class CustomItem
     *
     */
    protected int getPrefContentWidth(int aHeight)
    {
        return iX;
    }
}