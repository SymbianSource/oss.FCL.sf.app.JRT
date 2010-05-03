/*
* Copyright (c) 2007 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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


import com.nokia.mj.impl.rt.legacy.ToolkitInvoker;

public class Sprite extends Layer
{
    public static final int TRANS_NONE          = 0;
    public static final int TRANS_MIRROR_ROT180 = 1;
    public static final int TRANS_MIRROR        = 2;
    public static final int TRANS_ROT180        = 3;
    public static final int TRANS_MIRROR_ROT270 = 4;
    public static final int TRANS_ROT90         = 5;
    public static final int TRANS_ROT270        = 6;
    public static final int TRANS_MIRROR_ROT90  = 7;

    //
    // Transformation component reflections and mask
    //
    private static final int TRANSFORM_REFLECT_V = 0x01;
    private static final int TRANSFORM_REFLECT_H = 0x02;
    private static final int TRANSFORM_TRANSPOSE = 0x04;
    private static final int TRANSFORM_MASK      = 0x07;

    //
    // Indicator flags for transformed coordinates.
    //
    //if set transformed reference pixel has been calculated
    private static final int TRANSFORM_REF       = 0x10;
    //if set transformed collision rectangle has been calculated
    private static final int TRANSFORM_COLLISION = 0x20;

    //
    // State bitmask containing transform and transformed data flags.
    //
    private int     iState;

    //
    // Frame sequence
    //
    private int[]   iSequenceArray; // immutable, may be null.
    private int     iSequenceCount;
    private int     iSequenceIndex;


    //
    // Coordinates of reference pixel relative to top left corner
    // of the untransformed sprite in untransformed frame coords.
    //
    private int     iReferenceX;
    private int     iReferenceY;

    //
    // Collision rectangle in untransformed frame coordinates.
    //
    private int     iCollisionX;
    private int     iCollisionY;
    private int     iCollisionWidth;
    private int     iCollisionHeight;

    //
    // Coordinates of reference pixel relative to top left corner
    // of the transformed Sprite in transformed frame coords.
    //
    private int[]   iTransRefPixel;

    //
    // Collision rectangle in transformed Sprite coordinates
    //
    private int[][] iTransCollision;

    private final ToolkitInvoker iInvoker;

    public Sprite(Image aFrameImage, int aFrameWidth, int aFrameHeight)
    {
        super(aFrameImage, aFrameWidth,aFrameHeight);

        iCollisionWidth = aFrameWidth;
        iCollisionHeight= aFrameHeight;
        iSequenceCount  = iTileCount;
        iInvoker        = ToolkitInvoker.getToolkitInvoker();
    }

    public Sprite(Image aImage)
    {
        this(aImage, aImage.getWidth(), aImage.getHeight());
    }

    //
    // duplicate state from aSprite
    //
    public Sprite(Sprite aSprite)
    {
        super(aSprite);
        iState              = aSprite.iState;
        iSequenceArray      = aSprite.iSequenceArray;
        iSequenceCount      = aSprite.iSequenceCount;
        iSequenceIndex      = aSprite.iSequenceIndex;
        iReferenceX         = aSprite.iReferenceX;
        iReferenceY         = aSprite.iReferenceY;
        iCollisionX         = aSprite.iCollisionX;
        iCollisionY         = aSprite.iCollisionY;
        iCollisionWidth     = aSprite.iCollisionWidth;
        iCollisionHeight    = aSprite.iCollisionHeight;
        iTransRefPixel      = aSprite.iTransRefPixel;
        iInvoker            = aSprite.iInvoker;
    }

    public void setImage(Image aImage,int aFrameWidth,int aFrameHeight)
    {
        final int oTileCount  = iTileCount;
        final int oTileWidth  = iTileWidth;
        final int oTileHeight = iTileHeight;

        setTileImage(aImage, aFrameWidth, aFrameHeight);

        // drop transformed coords
        iState &= TRANSFORM_MASK;

        if ((iTileWidth != oTileWidth) || (iTileHeight != oTileHeight))
        {
            defineCollisionRectangle(0, 0, iTileWidth, iTileHeight);
        }

        if (iTileCount < oTileCount)
        {
            iSequenceArray = null;
            iSequenceIndex = 0;
        }

        if (null == iSequenceArray)
        {
            iSequenceCount = iTileCount;
        }
    }

    public int getRawFrameCount()
    {
        return iTileCount;
    }

    public void setFrame(int aSequenceIndex)
    {
        if (aSequenceIndex < 0 || aSequenceIndex >= iSequenceCount)
        {
            throw new IndexOutOfBoundsException("sequenceIndex outside frame sequence");
        }
        iSequenceIndex = aSequenceIndex;
    }

    public final int getFrame()
    {
        return iSequenceIndex;
    }

    public void nextFrame()
    {
        if (iSequenceIndex++ == iSequenceCount-1)
        {
            iSequenceIndex=0;
        }

        if (null == iSequenceArray)   // default frame sequence
        {
            if (iSequenceIndex == iTileCount)
            {
                iSequenceIndex = 0;
            }
        }
    }

    public void prevFrame()
    {
        if (iSequenceIndex-- == 0)
        {
            iSequenceIndex = iSequenceCount-1;
        }
    }

    public int getFrameSequenceLength()
    {
        return iSequenceCount;
    }

    public void setFrameSequence(int[] aSequenceArray)
    {
        int[] array;
        int   count;

        if (aSequenceArray == null)
        {
            count = iTileCount;
            array = null;
        }
        else
        {
            count = aSequenceArray.length;
            if (count==0)
            {
                throw new IllegalArgumentException("sequence length 0");
            }

            //
            // check all valid frames
            //
            array = new int[count];
            for (int i=count; --i>=0;)
            {
                final int index = aSequenceArray[i];
                if (index < 0 || index >= iTileCount)
                {
                    throw new ArrayIndexOutOfBoundsException("invalid frame index");
                }
                array[i] = index;
            }
        }

        iSequenceArray = array;
        iSequenceCount = count;
        iSequenceIndex = 0;
    }

    public void defineReferencePixel(int aX, int aY)
    {
        iReferenceX = aX;
        iReferenceY = aY;
        iState &= ~TRANSFORM_REF;
    }

    public int getRefPixelX()
    {
        final int[] point = getTransRefPixel();
        return iX + point[0];
    }

    public int getRefPixelY()
    {
        final int[] point = getTransRefPixel();
        return iY + point[1];
    }

    public void setRefPixelPosition(int aX, int aY)
    {
        final int[] point = getTransRefPixel();
        setPosition(aX-point[0], aY-point[1]);
    }

    public void defineCollisionRectangle(int aX, int aY, int aWidth, int aHeight)
    {
        if (aWidth<0 || aHeight<0)
        {
            throw new IllegalArgumentException("invalid width or height");
        }

        iCollisionX      = aX;
        iCollisionY      = aY;
        iCollisionWidth  = aWidth;
        iCollisionHeight = aHeight;

        iState &= ~TRANSFORM_COLLISION;
    }

    public void setTransform(int aTransform)
    {
        if ((iState & TRANSFORM_MASK) == aTransform)
        {
            // nothing to do.
            return;
        }

        final int[] oldTransRefPixel = getTransRefPixel();//iTransRefPixel returned
        final int oldX = oldTransRefPixel[0];
        final int oldY = oldTransRefPixel[1];

        switch (aTransform)
        {
        case TRANS_NONE:
        case TRANS_ROT90:
        case TRANS_ROT180:
        case TRANS_ROT270:
        case TRANS_MIRROR:
        case TRANS_MIRROR_ROT90:
        case TRANS_MIRROR_ROT180:
        case TRANS_MIRROR_ROT270:
            break; //transform ok
        default:
            throw new IllegalArgumentException("invalid transform");
        }

        // forget transformed data.
        iState = aTransform;

        // reset position
        final int[] newTransRefPixel = getTransRefPixel();
        iX = iX + oldX - newTransRefPixel[0];
        iY = iY + oldY - newTransRefPixel[1];
    }

    public final boolean collidesWith(Image aImage, int aX, int aY, boolean aPixelLevel)
    {
        if (!iVisible)
        {
            return false;
        }


        // aImage rect in painter's coordinates
        final int x1 = aX;
        final int y1 = aY;
        final int x2 = x1 + aImage.getWidth();
        final int y2 = y1 + aImage.getHeight();

        // sprite trans collision rect in sprite frame coordinates
        final int[][] collisionRect = getTransCollisionRect();
        // sprite trans collision rect in painter's coordinates
        int crX1 = iX + collisionRect[0][0];
        int crY1 = iY + collisionRect[0][1];
        int crX2 = iX + collisionRect[1][0];
        int crY2 = iY + collisionRect[1][1];

        // Check boundary collision first.
        if (!(x1 < crX2 && x2 > crX1 && y1 < crY2 && y2 > crY1))
        {
            return false;
        }

        // Rectangles intersect, can return a hit if not pixel level.
        if (!aPixelLevel)
        {
            return true;
        }


        // sprite trans collision rect clipped to trans frame rect, in painter's coordinates
        crX1 = Math.max(crX1, iX);
        crY1 = Math.max(crY1, iY);
        crX2 = Math.min(crX2, iX + getLayerWidth());
        crY2 = Math.min(crY2, iY + getLayerHeight());

        if (!(x1 < crX2 && x2 > crX1 && y1 < crY2 && y2 > crY1))
        {
            // All overlapping pixels are outside the bounds of the Sprite frame and only in the collision
            // rect and are considered to be transparent for pixel-level collision detection.
            return false;
        }


        // sprite untrans frame rect in sprite frame coordinates.
        int sx1 = 0;
        int sy1 = 0;
        int sx2 = iTileWidth;
        int sy2 = iTileHeight;

        // sprite untrans frame rect interesect with untrans collision rect in sprite frame coordinates.
        sx1 = Math.max(sx1, iCollisionX);
        sy1 = Math.max(sy1, iCollisionY);
        sx2 = Math.min(sx2, iCollisionX + iCollisionWidth);
        sy2 = Math.min(sy2, iCollisionY + iCollisionHeight);

        // sprite untrans collision rect (clipped to frame rect) converted to sprite raw frame image coordinates
        final int tileIndex = getTile();
        final int tileRow = tileIndex / iTileColumns;
        final int tileCol = tileIndex % iTileColumns;
        final int x = tileCol * iTileWidth;
        final int y = tileRow * iTileHeight;
        sx1 += x;
        sy1 += y;
        sx2 += x;
        sy2 += y;


        // aImage rect in aImage coordinates.
        final int ix1 = 0;
        final int iy1 = 0;
        final int ix2 = aImage.getWidth();
        final int iy2 = aImage.getHeight();

        return iInvoker.imageCollision(
                   iTileImage,
                   sx1, sy1, sx2, sy2,
                   iState & TRANSFORM_MASK,
                   crX1,crY1,
                   aImage, ix1, iy1, ix2, iy2,
                   TRANS_NONE,
                   aX,aY
               );
    }

    public final boolean collidesWith(Sprite aSprite, boolean aPixelLevel)
    {
        if (!(iVisible && aSprite.iVisible))
        {
            return false;
        }


        // sprite1 trans collision rect in sprite1 frame coordinates
        final int[][] collisionRect1 = getTransCollisionRect();
        // sprite1 trans collision rect in painter's coordinates
        int cr1x1 = iX + collisionRect1[0][0];
        int cr1y1 = iY + collisionRect1[0][1];
        int cr1x2 = iX + collisionRect1[1][0];
        int cr1y2 = iY + collisionRect1[1][1];


        // sprite2 trans collision rect in sprite2 frame coordinates
        final int[][] collisionRect2 = aSprite.getTransCollisionRect();
        // sprite2 trans collision rect in painter's coordinates
        int cr2x1 = aSprite.iX + collisionRect2[0][0];
        int cr2y1 = aSprite.iY + collisionRect2[0][1];
        int cr2x2 = aSprite.iX + collisionRect2[1][0];
        int cr2y2 = aSprite.iY + collisionRect2[1][1];


        // Check boundary collision first.
        if (!(cr2x1 < cr1x2 && cr2x2 > cr1x1 && cr2y1 < cr1y2 && cr2y2 > cr1y1))
        {
            return false;
        }

        // Bounding rectangles collide, can return hit if not pixel level.
        if (!aPixelLevel)
        {
            return true;
        }


        // sprite1 trans collision rect interesect with trans frame rect, in painter's coordinates
        cr1x1 = Math.max(cr1x1, iX);
        cr1y1 = Math.max(cr1y1, iY);
        cr1x2 = Math.min(cr1x2, iX + getLayerWidth());
        cr1y2 = Math.min(cr1y2, iY + getLayerHeight());

        // sprite2 trans collision rect interesect with trans frame rect, in painter's coordinates
        cr2x1 = Math.max(cr2x1, aSprite.iX);
        cr2y1 = Math.max(cr2y1, aSprite.iY);
        cr2x2 = Math.min(cr2x2, aSprite.iX + aSprite.getLayerWidth());
        cr2y2 = Math.min(cr2y2, aSprite.iY + aSprite.getLayerHeight());

        if (!(cr1x1 < cr2x2 && cr1x2 > cr2x1 && cr1y1 < cr2y2 && cr1y2 > cr2y1))
        {
            // All overlapping pixels are outside the bounds of the Sprite frame and only in the collision
            // rect and are considered to be transparent for pixel-level collision detection.
            return false;
        }


        // sprite1 untrans frame rect in sprite1 frame coordinates.
        int s1x1 = 0;
        int s1y1 = 0;
        int s1x2 = iTileWidth;
        int s1y2 = iTileHeight;

        // sprite1 untrans frame rect interesect with untrans collision rect
        s1x1 = Math.max(s1x1, iCollisionX);
        s1y1 = Math.max(s1y1, iCollisionY);
        s1x2 = Math.min(s1x2, iCollisionX + iCollisionWidth);
        s1y2 = Math.min(s1y2, iCollisionY + iCollisionHeight);

        // sprite1 untrans collision rect (clipped to frame rect) converted to sprite1 raw frame image coordinates
        final int tileIndex = getTile();
        final int tileRow = tileIndex / iTileColumns;
        final int tileCol = tileIndex % iTileColumns;
        final int x1 = tileCol * iTileWidth;
        final int y1 = tileRow * iTileHeight;
        s1x1 += x1;
        s1y1 += y1;
        s1x2 += x1;
        s1y2 += y1;


        // sprite2 untrans frame rect in sprite2 frame coordinates.
        int s2x1 = 0;
        int s2y1 = 0;
        int s2x2 = aSprite.iTileWidth;
        int s2y2 = aSprite.iTileHeight;

        // sprite2 untrans frame rect interesect with untrans collision rect
        s2x1 = Math.max(s2x1, aSprite.iCollisionX);
        s2y1 = Math.max(s2y1, aSprite.iCollisionY);
        s2x2 = Math.min(s2x2, aSprite.iCollisionX + aSprite.iCollisionWidth);
        s2y2 = Math.min(s2y2, aSprite.iCollisionY + aSprite.iCollisionHeight);

        // sprite2 untrans collision rect (clipped to frame rect) convert to sprite2 raw frame image coordinates
        final int tileIndex2 = aSprite.getTile();
        final int tileRow2 = tileIndex2 / aSprite.iTileColumns;
        final int tileCol2 = tileIndex2 % aSprite.iTileColumns;
        final int x2 = tileCol2 * aSprite.iTileWidth;
        final int y2 = tileRow2 * aSprite.iTileHeight;
        s2x1 += x2;
        s2y1 += y2;
        s2x2 += x2;
        s2y2 += y2;

        return iInvoker.imageCollision(
                   iTileImage, s1x1, s1y1, s1x2, s1y2,
                   iState & TRANSFORM_MASK,
                   cr1x1,cr1y1,
                   aSprite.iTileImage, s2x1, s2y1, s2x2, s2y2,
                   aSprite.iState & TRANSFORM_MASK,
                   cr2x1,cr2y1
               );
    }

    public final boolean collidesWith(TiledLayer aTiledLayer, boolean aPixelLevel)
    {
        if (!iVisible || !aTiledLayer.isVisible())
        {
            return false;
        }

        // sprite trans collision rect in sprite frame coordinates
        final int[][] collisionRect = getTransCollisionRect();
        // sprite trans collision rectangle in painter's coordinates
        int crX1 = iX + collisionRect[0][0];
        int crY1 = iY + collisionRect[0][1];
        int crX2 = iX + collisionRect[1][0];
        int crY2 = iY + collisionRect[1][1];

        if (!aPixelLevel)
        {
            return aTiledLayer.collidesCell(crX1,crY1,crX2,crY2);
        }


        // tiled layer rect in painter's coordinates
        final int tlX1 = aTiledLayer.getX();
        final int tlY1 = aTiledLayer.getY();
        final int tlX2 = tlX1 + aTiledLayer.getWidth();
        final int tlY2 = tlY1 + aTiledLayer.getHeight();

        final int cellHeight = aTiledLayer.getCellHeight();
        final int cellWidth  = aTiledLayer.getCellWidth();

        // only interested in cells that intersect with sprite collision rect
        final int startRow  = (crY1 > tlY1) ? (crY1 - tlY1)/cellHeight : 0;
        final int startCol  = (crX1 > tlX1) ? (crX1 - tlX1)/cellWidth  : 0;
        final int endRow    = (crY2 < tlY2) ? ((crY2-1 - tlY1)/cellHeight) : aTiledLayer.getRows()-1;
        final int endCol    = (crX2 < tlX2) ? ((crX2-1 - tlX1)/cellWidth) : aTiledLayer.getColumns()-1;

        // sprite untrans frame rect in sprite frame coordinates.
        int sx1 = 0;
        int sy1 = 0;
        int sx2 = iTileWidth;
        int sy2 = iTileHeight;

        // sprite untrans frame rect interesect with untrans collision rect
        sx1 = Math.max(sx1, iCollisionX);
        sy1 = Math.max(sy1, iCollisionY);
        sx2 = Math.min(sx2, iCollisionX + iCollisionWidth);
        sy2 = Math.min(sy2, iCollisionY + iCollisionHeight);

        // sprite untrans clipped collision rect convert to raw frame image coordinates
        final int tileIndex = getTile();
        final int tileRow = tileIndex / iTileColumns;
        final int tileCol = tileIndex % iTileColumns;
        final int x = tileCol * iTileWidth;
        final int y = tileRow * iTileHeight;
        sx1 += x;
        sy1 += y;
        sx2 += x;
        sy2 += y;

        // sprite trans collision rect interesect with trans frame rect, in painter's coordinates
        crX1 = Math.max(crX1, iX);
        crY1 = Math.max(crY1, iY);
        crX2 = Math.min(crX2, iX + getLayerWidth());
        crY2 = Math.min(crY2, iY + getLayerHeight());

        final Image image1 = iTileImage;
        final Image image2 = aTiledLayer.iTileImage;

        for (int row=startRow; row<=endRow; row++)
        {
            for (int col=startCol; col<=endCol; col++)
            {
                int tile = aTiledLayer.getCell(col,row);
                if (tile != 0) //non clear cell
                {
                    // cell rect in painter's coordinates
                    final int cellX1 = aTiledLayer.getX() + col * cellWidth;
                    final int cellY1 = aTiledLayer.getY() + row * cellHeight;
                    final int cellX2 = cellX1 + cellWidth;
                    final int cellY2 = cellY1 + cellHeight;

                    if (!(cellX1 < crX2 && cellX2 > crX1 && cellY1 < crY2 && cellY2 > crY1))
                    {
                        // All overlapping pixels are outside the bounds of the Sprite frame and only in the collision
                        // rect and are considered to be transparent for pixel-level collision detection.
                        break; //no collision, move on to next cell
                    }

                    if (tile < 0)
                    {
                        tile = aTiledLayer.getAnimatedTile(tile);
                    }

                    // cell rect in TiledLayer image coordinates.
                    tile -= 1;

                    final int trow  = tile / aTiledLayer.iTileColumns;
                    final int tcol  = tile % aTiledLayer.iTileColumns;
                    final int cx1   = tcol * aTiledLayer.iTileWidth;
                    final int cy1   = trow * aTiledLayer.iTileHeight;
                    final int cx2   = cx1 + cellWidth;
                    final int cy2   = cy1 + cellHeight;

                    if (iInvoker.imageCollision(
                                image1, sx1, sy1, sx2, sy2,
                                iState & TRANSFORM_MASK,
                                crX1,crY1,
                                image2, cx1, cy1, cx2, cy2,
                                TRANS_NONE,
                                cellX1,cellY1
                            )
                       )
                    {
                        return true;
                    }
                    // else no pixel collision, move on to next cell
                }
                // else clear cell - no collision, move on to next cell
            }
        }

        return false;
    }

    public final void paint(Graphics aGraphics)
    {
        if (iVisible)
        {
            final int tileIndex = getTile();
            final int tileRow = tileIndex / iTileColumns;
            final int tileCol = tileIndex % iTileColumns;
            final int sx = tileCol * iTileWidth;
            final int sy = tileRow * iTileHeight;
            final int sw = iTileWidth;
            final int sh = iTileHeight;
            final int dx = iX;
            final int dy = iY;
            final int transform = iState & TRANSFORM_MASK;
            aGraphics.drawRegion
            (
                iTileImage,
                sx,
                sy,
                sw,
                sh,
                transform,
                dx,
                dy,
                0
            );
        }
    }

    private int[] getTransRefPixel()
    {
        if (iTransRefPixel == null)
        {
            iTransRefPixel = new int[2];
        }

        if ((iState & TRANSFORM_REF) == 0)
        {
            int x = iReferenceX;
            int y = iReferenceY;

            if ((iState & TRANSFORM_REFLECT_V) != 0)
            {
                y = iTileHeight - 1 - y;
            }

            if ((iState & TRANSFORM_REFLECT_H) != 0)
            {
                x = iTileWidth - 1 - x;
            }

            if ((iState & TRANSFORM_TRANSPOSE) != 0)
            {
                int tmp = x;
                x = y;
                y = tmp;
            }

            iTransRefPixel[0]=x;
            iTransRefPixel[1]=y;

            iState |= TRANSFORM_REF;
        }

        return iTransRefPixel;
    }

    /**
    Calculate transformed collision rectange in sprite co-ordinates.
    */
    private int[][] getTransCollisionRect()
    {
        if ((iState & TRANSFORM_COLLISION) == 0)
        {
            if (iTransCollision == null)
            {
                iTransCollision = new int[2][2];
            }

            int x = 0;
            int y = 0;
            int w = 0;
            int h = 0;

            switch (iState & TRANSFORM_MASK)
            {
            case TRANS_NONE:
                x = iCollisionX;
                y = iCollisionY;
                break;
            case TRANS_ROT90:
                x = iTileHeight-iCollisionY-iCollisionHeight;
                y = iCollisionX;
                break;
            case TRANS_ROT180:
                x = iTileWidth-iCollisionX-iCollisionWidth;
                y = iTileHeight-iCollisionY-iCollisionHeight;
                break;
            case TRANS_ROT270:
                x = iCollisionY;
                y = iTileWidth-iCollisionX-iCollisionWidth;
                break;
            case TRANS_MIRROR:
                x = iTileWidth-iCollisionX-iCollisionWidth;
                y = iCollisionY;
                break;
            case TRANS_MIRROR_ROT90:
                x = iTileHeight-iCollisionY-iCollisionHeight;
                y = iTileWidth-iCollisionX-iCollisionWidth;
                break;
            case TRANS_MIRROR_ROT180:
                x = iCollisionX;
                y = iTileHeight-iCollisionY-iCollisionHeight;
                break;
            case TRANS_MIRROR_ROT270:
                x = iCollisionY;
                y = iCollisionX;
                break;
            }

            if ((iState & TRANSFORM_TRANSPOSE) == 0)
            {
                w = iCollisionWidth;
                h = iCollisionHeight;
            }
            else
            {
                w = iCollisionHeight;
                h = iCollisionWidth;
            }

            iTransCollision[0][0] = x;
            iTransCollision[0][1] = y;
            iTransCollision[1][0] = x + w;
            iTransCollision[1][1] = y + h;

            iState |= TRANSFORM_COLLISION;
        }

        return iTransCollision;
    }

    int getLayerWidth()
    {
        if ((iState & TRANSFORM_TRANSPOSE) == 0)
        {
            return iTileWidth;
        }
        else
        {
            return iTileHeight;
        }
    }

    int getLayerHeight()
    {
        if ((iState & TRANSFORM_TRANSPOSE) == 0)
        {
            return iTileHeight;
        }
        else
        {
            return iTileWidth;
        }
    }

    int getTile()
    {
        if (null != iSequenceArray)
        {
            return iSequenceArray[iSequenceIndex];
        }
        return iSequenceIndex;
    }
}
