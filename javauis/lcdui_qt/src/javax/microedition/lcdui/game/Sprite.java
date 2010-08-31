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
 * LCDUI Sprite class.
 */
public class Sprite extends Layer
{

    /**
     * Transform constant.
     */
    public static final int TRANS_NONE = 0;

    /**
     * Transform constant.
     */
    public static final int TRANS_MIRROR_ROT180 = 1;

    /**
     * Transform constant.
     */
    public static final int TRANS_MIRROR = 2;

    /**
     * Transform constant.
     */
    public static final int TRANS_ROT180 = 3;

    /**
     * Transform constant.
     */
    public static final int TRANS_MIRROR_ROT270 = 4;

    /**
     * Transform constant.
     */
    public static final int TRANS_ROT90 = 5;

    /**
     * Transform constant.
     */
    public static final int TRANS_ROT270 = 6;

    /**
     * Transform constant.
     */
    public static final int TRANS_MIRROR_ROT90 = 7;

    private static final int TRANSFORM_REFLECT_V = 0x01;
    private static final int TRANSFORM_REFLECT_H = 0x02;
    private static final int TRANSFORM_TRANSPOSE = 0x04;
    private static final int TRANSFORM_MASK = 0x07;

    private int   frameIndex;
    private int   frameCount;
    private int[] frameArray;

    // Transformation bitmask.
    private int transform;

    // Untransformed reference pixel coordinates.
    private int referenceX;
    private int referenceY;

    // Transformed reference pixel coordinates.
    private int transReferenceX;
    private int transReferenceY;

    // Untransformed collision rectangle frame coordinates.
    private int collisionX;
    private int collisionY;
    private int collisionWidth;
    private int collisionHeight;

    // Transformed collision rectangle frame coordinates.
    private int[][] transCollision;

    // If set transformed collision rectangle has been calculated.
    private static final int TRANSFORM_COLLISION = 0x20;

    /**
     * Constructor.
     *
     * @param image Image.
     * @param frameWidth Frame's width.
     * @param frameHeight Frame's height.
     */
    public Sprite(Image image, int frameWidth, int frameHeight)
    {
        super(image, frameWidth, frameHeight);
        this.collisionWidth = frameWidth;
        this.collisionHeight = frameHeight;
        frameCount = tileCount;
    }

    /**
     * Constructor.
     *
     * @param image Image.
     */
    public Sprite(Image image)
    {
        this(image, image.getWidth(), image.getHeight());
    }

    /**
     * Constructor.
     *
     * @param from source Sprite
     */
    public Sprite(Sprite from)
    {
        super(from);
        this.frameIndex = from.frameIndex;
        this.frameCount = from.frameCount;
        this.frameArray = from.frameArray; // shallow copy
        this.referenceX = from.referenceX;
        this.referenceY = from.referenceY;
        this.transform = from.transform;
        this.transReferenceX = from.transReferenceX;
        this.transReferenceY = from.transReferenceY;

        this.collisionX = from.collisionX;
        this.collisionY = from.collisionY;
        this.collisionWidth = from.collisionWidth;
        this.collisionHeight = from.collisionHeight;
    }

    /**
     * Set image.
     *
     * @param image Image to be used.
     * @param frameWidth Frame's width.
     * @param frameHeight Frame's height.
     */
    public void setImage(Image image, int frameWidth, int frameHeight)
    {
        int oldTileCound = tileCount;
        int oldTileWidth = tileWidth;
        int oldTileHeight = tileHeight;

        setTileImage(image, frameWidth, frameHeight);
        this.transform &= TRANSFORM_MASK;
        if(tileCount < oldTileCound)
        {
            frameIndex = 0;
            frameArray = null;
        }
        if(frameArray == null)
        {
            frameCount = tileCount;
        }

        // If image's dimension has changed:
        if((tileWidth != oldTileWidth) || (tileHeight != oldTileHeight))
        {
            updateTransRefPixel(true);
            defineCollisionRectangle(0, 0, tileWidth, tileHeight);
        }
    }

    /**
     * Sets transform.
     *
     * @param transform Transform mask.
     */
    public void setTransform(int transform)
    {
        if((this.transform & TRANSFORM_MASK) != transform)
        {
            // flags have changed
            switch(transform)
            {
            case TRANS_NONE:
            case TRANS_ROT90:
            case TRANS_ROT180:
            case TRANS_ROT270:
            case TRANS_MIRROR:
            case TRANS_MIRROR_ROT90:
            case TRANS_MIRROR_ROT180:
            case TRANS_MIRROR_ROT270:
                // Ok
                break;
            default:
                throw new IllegalArgumentException(
                    MsgRepository.SPRITE_EXCEPTION_INVALID_TRANSFORM);
            }

            this.transform = transform;

            updateTransRefPixel(true);
        }
    }

    /**
     * Sets frame.
     *
     * @param frameIndex Index.
     */
    public void setFrame(int frameIndex)
    {
        if(frameIndex < 0 || frameIndex >= frameCount)
        {
            throw new IndexOutOfBoundsException(
                MsgRepository.SPRITE_EXCEPTION_INVALID_FRAME_INDEX);
        }
        this.frameIndex = frameIndex;
    }

    /**
     * Gets current frame.
     *
     * @return Index of current frame.
     */
    public int getFrame()
    {
        return frameIndex;
    }

    /**
     * Switches to the next frame.
     */
    public void nextFrame()
    {
        frameIndex = (frameIndex + frameCount + 1) % frameCount;
    }

    /**
     * Switches to the previous frame.
     */
    public void prevFrame()
    {
        frameIndex = (frameIndex + frameCount - 1) % frameCount;
    }

    /**
     * Gets frame sequence length.
     *
     * @return Length of frame sequence.
     */
    public int getFrameSequenceLength()
    {
        return frameCount;
    }

    /**
     * Gets the number of raw frames in Sprite.
     *
     * @return Number of raw frames.
     */
    public int getRawFrameCount()
    {
        return tileCount;
    }

    /**
     * Sets the sequence of frames.
     *
     * @param frameArray Array of frame indexes.
     */
    public void setFrameSequence(int[] frameArray)
    {
        int[] tempArray;
        int tempCount;
        if(frameArray == null)
        {
            // no sequence
            tempCount = tileCount;
            tempArray = null;
        }
        else
        {
            tempCount = frameArray.length;
            if(tempCount == 0)
            {
                throw new IllegalArgumentException(
                    MsgRepository.SPRITE_EXCEPTION_INVALID_SEQUENCE_LENGTH);
            }
            tempArray = new int[tempCount];
            // check and copy the sequence
            for(int i = 0; i < tempCount; i++)
            {
                if(frameArray[i] < 0 || frameArray[i] >= tileCount)
                {
                    throw new ArrayIndexOutOfBoundsException(
                        MsgRepository.SPRITE_EXCEPTION_INVALID_FRAME + i);
                }
                tempArray[i] = frameArray[i];
            }
        }
        this.frameArray = tempArray;
        this.frameCount = tempCount;
        this.frameIndex = 0;
    }

    /**
     * Defines reference pixel.
     *
     * @param x X coordinate of reference pixel.
     * @param y Y coordinate of reference pixel.
     */
    public void defineReferencePixel(int x, int y)
    {
        referenceX = x;
        referenceY = y;
        updateTransRefPixel(false);
    }

    /**
     * Gets reference pixel's x-coordinate.
     *
     * @return X coordinate.
     */
    public int getRefPixelX()
    {
        return getX() + transReferenceX;
    }

    /**
     * Gets reference pixel's y-coordinate.
     *
     * @return Y coordinate.
     */
    public int getRefPixelY()
    {
        return getY() + transReferenceY;
    }

    /**
     * Sets reference pixels position.
     *
     * @param x X-coordinate.
     * @param y Y-coordinate.
     */
    public void setRefPixelPosition(int x, int y)
    {
        setPosition(x - transReferenceX, y - transReferenceY);
    }

    /**
     * Defines collision rectangle.
     *
     * @param x X.
     * @param y Y.
     * @param width Width of the rectangle.
     * @param height Height of the rectangle.
     */
    public void defineCollisionRectangle(int x, int y, int width, int height)
    {
        if(width < 0 || height < 0)
        {
            throw new IllegalArgumentException(
                MsgRepository.SPRITE_EXCEPTION_INVALID_WIDTH_HEIGHT);
        }

        collisionX = x;
        collisionY = y;
        collisionWidth = width;
        collisionHeight = height;

        transform &= ~TRANSFORM_COLLISION;
    }

    /**
     * Updates Transform reference pixel.
     *
     * @param moveLayer if the layer movement is needed based on transRef pixel change
     */
    private void updateTransRefPixel(boolean moveLayer)
    {
        int tempX = referenceX;
        int tempY = referenceY;

        if((transform & TRANSFORM_REFLECT_V) != 0)
        {
            tempY = tileHeight - 1 - tempY;
        }
        if((transform & TRANSFORM_REFLECT_H) != 0)
        {
            tempX = tileWidth - 1 - tempX;
        }
        if((transform & TRANSFORM_TRANSPOSE) != 0)
        {
            // xor swap
            tempX ^= tempY;
            tempY ^= tempX;
            tempX ^= tempY;
        }

        if(moveLayer)
        {
            move(transReferenceX - tempX, transReferenceY - tempY);
        }

        transReferenceX = tempX;
        transReferenceY = tempY;
    }

    int getLayerHeight()
    {
        if((transform & TRANSFORM_TRANSPOSE) == 0)
        {
            return tileHeight;
        }
        else
        {
            return tileWidth;
        }
    }

    int getLayerWidth()
    {
        if((transform & TRANSFORM_TRANSPOSE) == 0)
        {
            return tileWidth;
        }
        else
        {
            return tileHeight;
        }
    }

    int getTileIndex()
    {
        if(this.frameArray != null)
        {
            return frameArray[frameIndex];
        }
        return frameIndex;
    }

    /**
     * Paints sprite.
     *
     * @param graphics Graphics.
     */
    public final void paint(Graphics graphics)
    {
        if(visible)
        {
            int index = getTileIndex();
            int row = index / tileColumns;
            int col = index % tileColumns;
            graphics.drawRegion(tileImage,
                                col * tileWidth,
                                row * tileHeight,
                                tileWidth,
                                tileHeight,
                                transform & TRANSFORM_MASK,
                                x, y,
                                0);
        }
    }

    /**
     * Checks collision with Image.
     *
     * @param image Image.
     * @param x Image's x-coordinate.
     * @param y Image's y-coordinate.
     * @param pixelLevel Is collision check done in pixel level.
     *
     * @return true if Image and this Sprite collides.
     */
    public boolean collidesWith(Image image, int x, int y, boolean pixelLevel)
    {
        if(!visible)
        {
            return false;
        }

        // Image rect in painter's coordinates:
        final int x1 = x;
        final int y1 = y;
        final int x2 = x1 + image.getWidth();
        final int y2 = y1 + image.getHeight();

        // Sprite trans collision rect in sprite frame coordinates:
        final int[][] collisionRect = getTransCollisionRect();

        // sprite trans collision rect in painter's coordinates:
        int crX1 = this.x + collisionRect[0][0];
        int crY1 = this.y + collisionRect[0][1];
        int crX2 = this.x + collisionRect[1][0];
        int crY2 = this.y + collisionRect[1][1];

        // Check boundary collision first:
        if(!(x1 < crX2 && x2 > crX1 && y1 < crY2 && y2 > crY1))
        {
            return false;
        }

        // Rectangles intersect, can return a hit if not pixel level:
        if(!pixelLevel)
        {
            return true;
        }

        // sprite trans collision rect clipped to trans frame rect,
        // in painter's coordinates:
        crX1 = Math.max(crX1, this.x);
        crY1 = Math.max(crY1, this.y);
        crX2 = Math.min(crX2, this.x + getLayerWidth());
        crY2 = Math.min(crY2, this.y + getLayerHeight());

        if(!(x1 < crX2 && x2 > crX1 && y1 < crY2 && y2 > crY1))
        {
            // All overlapping pixels are outside the bounds of the Sprite
            // frame and only in the collision rect and are considered to
            // be transparent for pixel-level collision detection.
            return false;
        }


        // sprite untrans frame rect in sprite frame coordinates.
        int sx1 = 0;
        int sy1 = 0;
        int sx2 = tileWidth;
        int sy2 = tileHeight;

        // sprite untrans frame rect interesect with untrans collision
        // rect in sprite frame coordinates.
        sx1 = Math.max(sx1, collisionX);
        sy1 = Math.max(sy1, collisionY);
        sx2 = Math.min(sx2, collisionX + collisionWidth);
        sy2 = Math.min(sy2, collisionY + collisionHeight);

        // sprite untrans collision rect (clipped to frame rect) converted
        // to sprite raw frame image coordinates
        final int tileIndex = getTileIndex();
        final int tileRow = tileIndex / tileColumns;
        final int tileCol = tileIndex % tileColumns;
        final int tempX = tileCol * tileWidth;
        final int tempY = tileRow * tileHeight;
        sx1 += tempX;
        sy1 += tempY;
        sx2 += tempX;
        sy2 += tempY;

        return com.nokia.mj.impl.nokialcdui.LCDUIInvoker.detectCollision(
                   tileImage, transform & TRANSFORM_MASK, crX1, crY1,
                   sx1, sy1, sx2, sy2,
                   image, TRANS_NONE, x, y,
                   0, 0, image.getWidth(), image.getHeight());
    }

    /**
     * Checks collision between this and an another sprite.
     *
     * @param otherSprite Sprite.
     * @param pixelLevel Is collision detected in pixel level.
     * @return True if sprites collide.
     */
    public boolean collidesWith(Sprite otherSprite, boolean pixelLevel)
    {
        // Both Sprites must be visible in order for a collision to be detected:
        if(!(visible && otherSprite.visible))
        {
            return false;
        }

        // Sprite trans collision rect in sprite frame coordinates:
        final int[][] collisionRect1 = getTransCollisionRect();
        // Sprite trans collision rect in painter's coordinates:
        int cr1x1 = this.x + collisionRect1[0][0];
        int cr1y1 = this.y + collisionRect1[0][1];
        int cr1x2 = this.x + collisionRect1[1][0];
        int cr1y2 = this.y + collisionRect1[1][1];

        // otherSprite trans collision rect in otherSprite frame coordinates;
        final int[][] collisionRect2 = otherSprite.getTransCollisionRect();
        // otherSprite trans collision rect in painter's coordinates:
        int cr2x1 = otherSprite.x + collisionRect2[0][0];
        int cr2y1 = otherSprite.y + collisionRect2[0][1];
        int cr2x2 = otherSprite.x + collisionRect2[1][0];
        int cr2y2 = otherSprite.y + collisionRect2[1][1];

        // Check boundary collision first:
        if(!(cr2x1 < cr1x2 && cr2x2 > cr1x1 && cr2y1 < cr1y2 && cr2y2 > cr1y1))
        {
            return false;
        }

        // Bounding rectangles collide, can return hit if not pixel level.
        if(!pixelLevel)
        {
            return true;
        }

        // Sprite trans collision rect interesect with trans frame
        // rect, in painter's coordinates:
        cr1x1 = Math.max(cr1x1, this.x);
        cr1y1 = Math.max(cr1y1, this.y);
        cr1x2 = Math.min(cr1x2, this.x + getLayerWidth());
        cr1y2 = Math.min(cr1y2, this.y + getLayerHeight());

        // otherSsprite trans collision rect interesect with trans frame
        // rect, in painter's coordinates:
        cr2x1 = Math.max(cr2x1, otherSprite.x);
        cr2y1 = Math.max(cr2y1, otherSprite.y);
        cr2x2 = Math.min(cr2x2, otherSprite.x + otherSprite.getLayerWidth());
        cr2y2 = Math.min(cr2y2, otherSprite.y + otherSprite.getLayerHeight());

        if(!(cr1x1 < cr2x2 && cr1x2 > cr2x1 && cr1y1 < cr2y2 && cr1y2 > cr2y1))
        {
            // All overlapping pixels are outside the bounds of the
            // Sprite frame and only in the collision rect and are
            // considered to be transparent for pixel-level collision detection.
            return false;
        }

        // Sprite untrans frame rect in sprite frame coordinates:
        int s1x1 = 0;
        int s1y1 = 0;
        int s1x2 = tileWidth;
        int s1y2 = tileHeight;

        // Sprite untrans frame rect interesect with untrans collision rect:
        s1x1 = Math.max(s1x1, collisionX);
        s1y1 = Math.max(s1y1, collisionY);
        s1x2 = Math.min(s1x2, collisionX + collisionWidth);
        s1y2 = Math.min(s1y2, collisionY + collisionHeight);

        // Sprite untrans collision rect (clipped to frame rect) converted to
        // sprite raw frame image coordinates:
        final int tileIndex = getTileIndex();
        final int tileRow = tileIndex / tileColumns;
        final int tileCol = tileIndex % tileColumns;
        final int x1 = tileCol * tileWidth;
        final int y1 = tileRow * tileHeight;
        s1x1 += x1;
        s1y1 += y1;
        s1x2 += x1;
        s1y2 += y1;


        // otherSprite untrans frame rect in sprite2 frame coordinates:
        int s2x1 = 0;
        int s2y1 = 0;
        int s2x2 = otherSprite.tileWidth;
        int s2y2 = otherSprite.tileHeight;

        // otherSprite untrans frame rect interesect with
        // untrans collision rect:
        s2x1 = Math.max(s2x1, otherSprite.collisionX);
        s2y1 = Math.max(s2y1, otherSprite.collisionY);
        s2x2 = Math.min(s2x2, otherSprite.collisionX + otherSprite.collisionWidth);
        s2y2 = Math.min(s2y2, otherSprite.collisionY + otherSprite.collisionHeight);

        // otherSprite untrans collision rect (clipped to frame rect) convert
        // to otherSprite raw frame image coordinates:
        final int tileIndex2 = otherSprite.getTileIndex();
        final int tileRow2 = tileIndex2 / otherSprite.tileColumns;
        final int tileCol2 = tileIndex2 % otherSprite.tileColumns;
        final int x2 = tileCol2 * otherSprite.tileWidth;
        final int y2 = tileRow2 * otherSprite.tileHeight;
        s2x1 += x2;
        s2y1 += y2;
        s2x2 += x2;
        s2y2 += y2;

        return com.nokia.mj.impl.nokialcdui.LCDUIInvoker.detectCollision(
                   tileImage, transform & TRANSFORM_MASK,
                   cr1x1, cr1y1,
                   s1x1, s1y1, s1x2, s1y2,

                   otherSprite.tileImage,
                   otherSprite.transform & TRANSFORM_MASK,
                   cr2x1, cr2y1,
                   s2x1, s2y1, s2x2, s2y2
               );
    }

    /**
     * Checks collision between this and TiledLayer.
     *
     * @param tiledLayer Tiled layer.
     * @param pixelLevel Is collision detected in pixel level.
     * @return True if collision occurs.
     */
    public boolean collidesWith(TiledLayer tiledLayer, boolean pixelLevel)
    {
        // The Sprite and TiledLayer must both be visible
        // in order for a collision to be detected:
        if(!visible || !tiledLayer.isVisible())
        {
            return false;
        }

        // sprite trans collision rect in sprite frame coordinates
        final int[][] collisionRect = getTransCollisionRect();
        // sprite trans collision rectangle in painter's coordinates
        int crX1 = this.x + collisionRect[0][0];
        int crY1 = this.y + collisionRect[0][1];
        int crX2 = this.x + collisionRect[1][0];
        int crY2 = this.y + collisionRect[1][1];

        if(!pixelLevel)
        {
            return tiledLayer.collidesCell(crX1, crY1, crX2, crY2);
        }

        // tiled layer rect in painter's coordinates:
        final int tlX1 = tiledLayer.getX();
        final int tlY1 = tiledLayer.getY();
        final int tlX2 = tlX1 + tiledLayer.getWidth();
        final int tlY2 = tlY1 + tiledLayer.getHeight();

        final int cellHeight = tiledLayer.getCellHeight();
        final int cellWidth  = tiledLayer.getCellWidth();

        // only interested in cells that intersect with sprite collision rect
        final int startRow = (crY1 > tlY1) ? (crY1 - tlY1) / cellHeight : 0;
        final int startCol = (crX1 > tlX1) ? (crX1 - tlX1) / cellWidth  : 0;
        final int endRow = (crY2 < tlY2) ? ((crY2 - 1 - tlY1) / cellHeight) : tiledLayer.getRows() - 1;
        final int endCol = (crX2 < tlX2) ? ((crX2 - 1 - tlX1) / cellWidth) : tiledLayer.getColumns() - 1;

        // sprite untrans frame rect in sprite frame coordinates.
        int sx1 = 0;
        int sy1 = 0;
        int sx2 = tileWidth;
        int sy2 = tileHeight;

        // sprite untrans frame rect interesect with untrans collision rect
        sx1 = Math.max(sx1, collisionX);
        sy1 = Math.max(sy1, collisionY);
        sx2 = Math.min(sx2, collisionX + collisionWidth);
        sy2 = Math.min(sy2, collisionY + collisionHeight);

        // sprite untrans clipped collision rect convert to raw frame
        // image coordinates
        final int tileIndex = getTileIndex();
        final int tileRow = tileIndex / tileColumns;
        final int tileCol = tileIndex % tileColumns;
        final int x = tileCol * tileWidth;
        final int y = tileRow * tileHeight;
        sx1 += x;
        sy1 += y;
        sx2 += x;
        sy2 += y;

        // sprite trans collision rect interesect with trans frame rect,
        // in painter's coordinates
        crX1 = Math.max(crX1, this.x);
        crY1 = Math.max(crY1, this.y);
        crX2 = Math.min(crX2, this.x + getLayerWidth());
        crY2 = Math.min(crY2, this.y + getLayerHeight());

        for(int row = startRow; row <= endRow; row++)
        {
            for(int col = startCol; col <= endCol; col++)
            {
                int tile = tiledLayer.getCell(col, row);
                if(tile != 0)    //non clear cell
                {
                    // cell rect in painter's coordinates
                    final int cellX1 = tiledLayer.getX() + col * cellWidth;
                    final int cellY1 = tiledLayer.getY() + row * cellHeight;
                    final int cellX2 = cellX1 + cellWidth;
                    final int cellY2 = cellY1 + cellHeight;

                    if(!(cellX1 < crX2 && cellX2 > crX1
                            && cellY1 < crY2 && cellY2 > crY1))
                    {
                        // All overlapping pixels are outside the bounds of the
                        // Sprite frame and only in the collision rect and
                        // are considered to be transparent for pixel-level
                        // collision detection.
                        break; //no collision, move on to next cell
                    }

                    if(tile < 0)
                    {
                        tile = tiledLayer.getAnimatedTile(tile);
                    }

                    // cell rect in TiledLayer image coordinates.
                    tile -= 1;

                    final int trow  = tile / tiledLayer.tileColumns;
                    final int tcol  = tile % tiledLayer.tileColumns;
                    final int cx1   = tcol * tiledLayer.tileWidth;
                    final int cy1   = trow * tiledLayer.tileHeight;
                    final int cx2   = cx1 + cellWidth;
                    final int cy2   = cy1 + cellHeight;

                    if(com.nokia.mj.impl.nokialcdui.LCDUIInvoker.detectCollision(
                                tileImage,
                                transform & TRANSFORM_MASK,
                                crX1, crY1,
                                sx1, sy1, sx2, sy2,

                                tiledLayer.tileImage,
                                TRANS_NONE,
                                cellX1, cellY1,
                                cx1, cy1, cx2, cy2))
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

    /**
     * Calculate transformed collision rectange in sprite co-ordinates.
     */
    private int[][] getTransCollisionRect()
    {
        if((transform & TRANSFORM_COLLISION) == 0)
        {
            if(transCollision == null)
            {
                transCollision = new int[2][2];
            }

            int x = 0;
            int y = 0;
            int w = 0;
            int h = 0;

            switch(transform & TRANSFORM_MASK)
            {
            case TRANS_NONE:
                x = collisionX;
                y = collisionY;
                break;
            case TRANS_ROT90:
                x = tileHeight - collisionY - collisionHeight;
                y = collisionX;
                break;
            case TRANS_ROT180:
                x = tileWidth - collisionX - collisionWidth;
                y = tileHeight - collisionY - collisionHeight;
                break;
            case TRANS_ROT270:
                x = collisionY;
                y = tileWidth - collisionX - collisionWidth;
                break;
            case TRANS_MIRROR:
                x = tileWidth - collisionX - collisionWidth;
                y = collisionY;
                break;
            case TRANS_MIRROR_ROT90:
                x = tileHeight - collisionY - collisionHeight;
                y = tileWidth - collisionX - collisionWidth;
                break;
            case TRANS_MIRROR_ROT180:
                x = collisionX;
                y = tileHeight - collisionY - collisionHeight;
                break;
            case TRANS_MIRROR_ROT270:
                x = collisionY;
                y = collisionX;
                break;
            default:
                break;
            }

            if((transform & TRANSFORM_TRANSPOSE) == 0)
            {
                w = collisionWidth;
                h = collisionHeight;
            }
            else
            {
                w = collisionHeight;
                h = collisionWidth;
            }

            transCollision[0][0] = x;
            transCollision[0][1] = y;
            transCollision[1][0] = x + w;
            transCollision[1][1] = y + h;

            transform |= TRANSFORM_COLLISION;
        }

        return transCollision;
    }

}
