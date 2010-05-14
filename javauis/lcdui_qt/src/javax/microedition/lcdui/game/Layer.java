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
import javax.microedition.lcdui.Image;

/**
 * LCDUI Layer class.
 */
public abstract class Layer
{

    int x;
    int y;
    boolean visible;

    Image tileImage;

    int tileWidth;
    int tileHeight;

    int tileCount;
    int tileColumns;

    /**
     * Package level constructor prevents Layer being subclassed. This is
     * consistent with the MIDP 2.0 RI.
     */
    Layer(Image tileImage, int frameWidth, int frameHeight)
    {
        visible = true;
        setTileImage(tileImage, frameWidth, frameHeight);
    }

    Layer(Layer from)
    {
        x = from.x;
        y = from.y;
        visible = from.visible;
        tileImage = from.tileImage;
        tileWidth = from.tileWidth;
        tileHeight = from.tileHeight;
        tileCount = from.tileCount;
        tileColumns = from.tileColumns;
    }

    public void setPosition(int x, int y)
    {
        this.x = x;
        this.y = y;
    }

    public void move(int deltaX, int deltaY)
    {
        x += deltaX;
        y += deltaY;
    }

    public final int getX()
    {
        return x;
    }

    public final int getY()
    {
        return y;
    }

    public final int getWidth()
    {
        return getLayerWidth();
    }

    public final int getHeight()
    {
        return getLayerHeight();
    }

    public void setVisible(boolean visible)
    {
        this.visible = visible;
    }

    public final boolean isVisible()
    {
        return visible;
    }

    public abstract void paint(Graphics graphics);

    abstract int getLayerWidth();

    abstract int getLayerHeight();

    /**
     * Set the source image and tile size. checks that the image is completely
     * tiled. computes the tile count.
     */
    void setTileImage(Image tileImage, int tileWidth, int tileHeight)
    {
        if(tileWidth > 0 && tileHeight > 0)
        {
            final int imageWidth = tileImage.getWidth();
            final int imageHeight = tileImage.getHeight();

            int rows = imageHeight / tileHeight;
            int cols = imageWidth / tileWidth;

            int totalHeight = rows * tileHeight;
            int totalWidth = cols * tileWidth;

            if(totalHeight == imageHeight && totalWidth == imageWidth)
            {
                this.tileImage = tileImage;
                this.tileWidth = tileWidth;
                this.tileHeight = tileHeight;
                this.tileCount = rows * cols;
                this.tileColumns = cols;
                return;
            }
        }
        throw new IllegalArgumentException(
            MsgRepository.LAYER_EXCEPTION_INVALID_TILE_WIDTH_HEIGHT);
    }

    /**
     * Is Layer visible in Graphics clip region.
     */
    final boolean isVisible(Graphics graphics, int pX, int pY)
    {
        int x1 = x + pX;
        int y1 = y + pY;
        int x2 = x1 + getWidth();
        int y2 = y1 + getHeight();
        int clipX1 = graphics.getClipX();
        int clipY1 = graphics.getClipY();
        int clipX2 = clipX1 + graphics.getClipWidth();
        int clipY2 = clipY1 + graphics.getClipHeight();

        return !(clipX2 <= x1 || clipY2 <= y1 || clipX1 >= x2 || clipY1 >= y2);
    }

}
