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

import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.Graphics;

public abstract class Layer
{
    int             iX;
    int             iY;
    boolean         iVisible;

    Image           iTileImage;
    int             iTileWidth;
    int             iTileHeight;
    int             iTileCount;
    int             iTileColumns;

    /**
     * Package level constructor prevents Layer being subclassed.
     */
    Layer(Image aTileImage, int aTileWidth, int aTileHeight)
    {
        iVisible = true;
        setTileImage(aTileImage, aTileWidth, aTileHeight);
    }

    Layer(Layer aTemplate)
    {
        iX          = aTemplate.iX;
        iY          = aTemplate.iY;
        iVisible    = aTemplate.iVisible;
        iTileImage  = aTemplate.iTileImage;
        iTileWidth  = aTemplate.iTileWidth;
        iTileHeight = aTemplate.iTileHeight;
        iTileCount  = aTemplate.iTileCount;
        iTileColumns= aTemplate.iTileColumns;
    }

    public void setPosition(int aX,int aY)
    {
        iX = aX;
        iY = aY;
    }

    public void move(int aDx,int aDy)
    {
        iX += aDx;
        iY += aDy;
    }

    public final int getX()
    {
        return iX;
    }

    public final int getY()
    {
        return iY;
    }

    public final int getWidth()
    {
        return getLayerWidth();
    }

    public final int getHeight()
    {
        return getLayerHeight();
    }

    public void setVisible(boolean aVisible)
    {
        iVisible=aVisible;
    }

    public final boolean isVisible()
    {
        return iVisible;
    }

    public abstract void paint(Graphics aGraphics);

    /**
     * Set the source image and tile size. checks that the image is completely
     * tiled. computes the tile count.
     */
    void setTileImage(Image aTileImage, int aTileWidth, int aTileHeight)
    {
        final int imageWidth  = aTileImage.getWidth();
        final int imageHeight = aTileImage.getHeight();

        if (aTileWidth>0 && aTileHeight>0)
        {
            int rows = imageHeight / aTileHeight;
            int cols = imageWidth / aTileWidth;

            int totalHeight = rows * aTileHeight;
            int totalWidth  = cols * aTileWidth;

            if (totalHeight == imageHeight && totalWidth == imageWidth)
            {
                iTileImage  = aTileImage;
                iTileWidth  = aTileWidth;
                iTileHeight = aTileHeight;
                iTileCount  = rows * cols;
                iTileColumns = iTileImage.getWidth() / iTileWidth;
                return;
            }
        }

        throw new IllegalArgumentException("invalid frameWidth or frameHeight");
    }

    abstract int getLayerWidth();
    abstract int getLayerHeight();
}
