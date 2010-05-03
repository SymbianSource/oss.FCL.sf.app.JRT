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

public class TiledLayer extends Layer
{
    // The amount by which the capacity of the animated tile array is automatically
    // incremented when its size becomes greater than its capacity.
    private static final int ANIM_ARRAY_INCREMENT = 4;

    private int     iRows;
    private int     iCols;
    private int[][] iCells;
    private int[]   iAnimArray;
    private int     iAnimCount;

    public TiledLayer(int aCols, int aRows, Image aTileImage, int aTileWidth,int aTileHeight)
    {
        super(aTileImage, aTileWidth, aTileHeight);

        if (aCols<1 || aRows<1)
        {
            throw new IllegalArgumentException("number of rows or columns invalid");
        }

        iCols      = aCols;
        iRows      = aRows;
        iCells     = new int[aRows][aCols];
        iAnimArray = new int[ANIM_ARRAY_INCREMENT];
        iAnimCount = 0;
    }

    public void setStaticTileSet(Image aTileImage,int aTileWidth,int aTileHeight)
    {
        final int oTileCount = iTileCount;

        setTileImage(aTileImage, aTileWidth, aTileHeight);

        if (iTileCount < oTileCount)
        {
            fillCells(0, 0, iCols, iRows, 0);
            iAnimCount = 0;
        }
    }

    public void setCell(int aCol,int aRow,int aTileIndex)
    {
        validateTileIndex(aTileIndex);
        //
        // will throw exception if aRow or aCol invalid
        //
        iCells[aRow][aCol] = aTileIndex;
    }

    public int getCell(int aCol,int aRow)
    {
        return iCells[aRow][aCol];
    }

    public void fillCells(int aCol,int aRow,int aNumCols,int aNumRows,int aTileIndex)
    {
        if (aNumCols < 0)
        {
            throw new IllegalArgumentException("numCols is less than zero");
        }
        if (aNumRows < 0)
        {
            throw new IllegalArgumentException("numRows is less than zero");
        }
        validateTileIndex(aTileIndex);

        for (int i=aCol+aNumCols; --i>=aCol;)
        {
            iCells[aRow][i] = aTileIndex;
        }
        for (int i=aRow+aNumRows; --i>aRow;)
        {
            System.arraycopy(iCells[aRow], aCol, iCells[i], aCol, aNumCols);
        }
    }

    public int createAnimatedTile(int aTileIndex)
    {
        if (aTileIndex < 0 || aTileIndex>iTileCount)
        {
            throw new IndexOutOfBoundsException("invalid staticTileIndex");
        }

        final int index = iAnimCount;
        checkCapacity(index+1);
        iAnimArray[index]=aTileIndex;
        iAnimCount++;
        return ~index;
    }

    /**
    Grows the animated tile array if required.
    */
    private void checkCapacity(int aRequiredLength)
    {
        int length = iAnimArray.length;
        if (aRequiredLength>length)
        {
            int[] array = new int[length + ANIM_ARRAY_INCREMENT];
            System.arraycopy(iAnimArray, 0, array, 0, length);
            iAnimArray = array;
        }
    }

    public void setAnimatedTile(int aAnimIndex, int aTileIndex)
    {
        final int index = ~aAnimIndex;
        if (index>=iAnimCount || aTileIndex<0 || aTileIndex>iTileCount)
        {
            throw new IndexOutOfBoundsException("animatedTileIndex or staticTileIndex invalid");
        }

        iAnimArray[index] = aTileIndex;
    }

    public int getAnimatedTile(int aAnimIndex)
    {
        final int index = ~aAnimIndex;
        if (index >= iAnimCount)
        {
            throw new IndexOutOfBoundsException("animatedTileIndex or staticTileIndex invalid");
        }

        return iAnimArray[index];
    }

    public final int getCellWidth()
    {
        return iTileWidth;
    }

    public final int getCellHeight()
    {
        return iTileHeight;
    }

    public final int getColumns()
    {
        return iCols;
    }

    public final int getRows()
    {
        return iRows;
    }

    public final void paint(Graphics aGraphics)
    {
        if (iVisible)
        {
            aGraphics.translate(iX, iY);

            int clipX1 = aGraphics.getClipX();
            int clipY1 = aGraphics.getClipY();
            int clipX2 = clipX1 + aGraphics.getClipWidth();
            int clipY2 = clipY1 + aGraphics.getClipHeight();

            final int tw = iTileWidth;
            final int th = iTileHeight;
            final int lw = getWidth();  // layer width
            final int lh = getHeight(); // layer height

            clipX1 = (clipX1 < 0  ? 0  : clipX1);
            clipY1 = (clipY1 < 0  ? 0  : clipY1);
            clipX2 = (clipX2 > lw ? lw : clipX2);
            clipY2 = (clipY2 > lh ? lh : clipY2);

            final int begRow = clipY1 / iTileHeight;
            final int begCol = clipX1 / iTileWidth;
            final int endRow = (clipY2 - 1) / iTileHeight;
            final int endCol = (clipX2 - 1) / iTileWidth;

            int sx = begRow*th;
            int sy = begCol*tw;
            int dx;
            int dy;

            final int[][] cells = iCells;
            for (int row=begRow; row<=endRow; row++)
            {
                final int[] line = cells[row];
                for (int col=begCol; col<=endCol; col++)
                {
                    final int index = validateTileIndex(line[col]);
                    if (index >= 0)
                    {
                        dx = col * tw;
                        dy = row * th;

                        int tr = index / iTileColumns;
                        int tc = index % iTileColumns;

                        sx = tc * tw;
                        sy = tr * th;

                        aGraphics.drawRegion
                        (
                            iTileImage,
                            sx,
                            sy,
                            tw,
                            th,
                            Sprite.TRANS_NONE,
                            dx,
                            dy,
                            0
                        );
                    }
                }
            }

            aGraphics.translate(-iX, -iY);
        }
    }

    private int validateTileIndex(int aTileIndex)
    {
        if (aTileIndex > iTileCount)
        {
            throw new IndexOutOfBoundsException("invalid tileIndex");
        }
        //
        // check createAnimatedTile has been called,
        // getAnimatedTile will throw exception if hasn't.
        //
        if (aTileIndex<0)
        {
            aTileIndex = getAnimatedTile(aTileIndex);
        }

        return aTileIndex - 1;
    }

    boolean collidesCell(int aX1, int aY1, int aX2, int aY2)
    {
        // tiled layer rect in painter's coordinates
        int tlX1 = iX;
        int tlY1 = iY;
        int tlX2 = iX + getWidth();;
        int tlY2 = iY + getHeight();;

        // compute intersection of TiledLayer bounds and sprite collision
        // rectangle.
        tlX1 = Math.max(tlX1, aX1);
        tlY1 = Math.max(tlY1, aY1);
        tlX2 = Math.min(tlX2, aX2);
        tlY2 = Math.min(tlY2, aY2);

        // return false if empty intersection.
        if (!((tlX1 < tlX2) && (tlY1 < tlY2)))
        {
            return false;
        }

        // transform to tiled layer coordinates
        tlX1 -= iX;
        tlY1 -= iY;
        tlX2 -= iX;
        tlY2 -= iY;

        int startRow = (tlY1/iTileHeight);
        int startCol = (tlX1/iTileWidth);
        int endRow   = (tlY2 - 1)/iTileHeight;
        int endCol   = (tlX2 - 1)/iTileWidth;

        final int[][] cells = iCells;
        for (int row=startRow; row<=endRow; row++)
        {
            final int[] line = cells[row];
            for (int col=startCol; col<=endCol; col++)
            {
                if (line[col] != 0)
                {
                    // at least one cell is not empty
                    return true;
                }
            }
        }

        // all cells in overlap are empty
        return false;
    }

    int getLayerWidth()
    {
        return iCols * iTileWidth;
    }

    int getLayerHeight()
    {
        return iRows * iTileHeight;
    }
}
