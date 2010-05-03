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

public class TiledLayer extends Layer {

    // Animated tile array increment
    private static final int ANIM_ARRAY_INCREMENT = 4;

    private int rows;
    private int cols;

    private int[][] cells;
    private int[] animArray;
    private int animCount;

    public TiledLayer(int cols, int rows, Image tileImage, int tileWidth, int tileHeight) {
        super(tileImage, tileWidth, tileHeight);

        if (cols < 1 || rows < 1) {
            throw new IllegalArgumentException(
                    MsgRepository.TILEDLAYER_EXCEPTION_INVALID_DIMENSIONS);
        }

        this.cols = cols;
        this.rows = rows;
        cells = new int[rows][cols];
        animArray = new int[ANIM_ARRAY_INCREMENT];
        animCount = 0;
    }

    public void setStaticTileSet(Image tileImage, int tileWidth, int tileHeight) {
        final int oldTileCount = tileCount;

        setTileImage(tileImage, tileWidth, tileHeight);

        // If the new static tile set has as many or more tiles, then the
        // animated tiles and cell contents will be preserved.
        // If not, the contents of the grid will be cleared (all cells will
        // contain index 0) and all animated tiles will be deleted.
        //
        if (tileCount < oldTileCount) {
            fillCells(0, 0, cols, rows, 0);
            animCount = 0;
        }
    }

    public void setCell(int col, int row, int aTileIndex) {
        validateTileIndex(aTileIndex);
        cells[row][col] = aTileIndex;
    }

    public int getCell(int col, int row) {
        return cells[row][col];
    }

    public void fillCells(int col, int row, int numCols, int numRows, int tileIndex) {
        if (numCols < 0) {
            throw new IllegalArgumentException(
                    MsgRepository.TILEDLAYER_EXCEPTION_INVALID_NUMBER_OF_COLUMNS);
        }
        if (numRows < 0) {
            throw new IllegalArgumentException(
                    MsgRepository.TILEDLAYER_EXCEPTION_INVALID_NUMBER_OF_ROWS);
        }
        validateTileIndex(tileIndex);

        for (int i = col + numCols; --i >= col;) {
            cells[row][i] = tileIndex;
        }
        for (int i = row + numRows; --i > row;) {
            System.arraycopy(cells[row], col, cells[i], col, numCols);
        }
    }

    public int createAnimatedTile(int tileIndex) {
        if (tileIndex < 0 || tileIndex > tileCount) {
            throw new IndexOutOfBoundsException(
                    MsgRepository.TILEDLAYER_EXCEPTION_INVALID_TILE_INDEX);
        }

        final int index = animCount;
        checkCapacity(index + 1);
        animArray[index] = tileIndex;
        animCount++;
        return ~index;
    }

    /**
     * Grows the animated tile array if required.
     */
    private void checkCapacity(int requiredLength) {
        int length = animArray.length;
        if (requiredLength > length) {
            int[] array = new int[length + ANIM_ARRAY_INCREMENT];
            System.arraycopy(animArray, 0, array, 0, length);
            animArray = array;
        }
    }

    public void setAnimatedTile(int animIndex, int tileIndex) {
        final int index = ~animIndex;
        if (index >= animCount) {
            throw new IndexOutOfBoundsException(
                    MsgRepository.TILEDLAYER_EXCEPTION_INVALID_ANIMTILE_INDEX);
        }
        if (tileIndex < 0 || tileIndex > tileCount) {
            throw new IndexOutOfBoundsException(
                    MsgRepository.TILEDLAYER_EXCEPTION_INVALID_TILE_INDEX);
        }
        animArray[index] = tileIndex;
    }

    public int getAnimatedTile(int animIndex) {
        final int index = ~animIndex;
        if (index >= animCount) {
            throw new IndexOutOfBoundsException(
                    MsgRepository.TILEDLAYER_EXCEPTION_INVALID_ANIMTILE_INDEX);
        }
        return animArray[index];
    }

    public final int getCellWidth() {
        return tileWidth;
    }

    public final int getCellHeight() {
        return tileHeight;
    }

    public final int getColumns() {
        return cols;
    }

    public final int getRows() {
        return rows;
    }

    public final void paint(Graphics aGraphics) {
        if (visible) {
            aGraphics.translate(x, y);

            int clipX1 = aGraphics.getClipX();
            int clipY1 = aGraphics.getClipY();
            int clipX2 = clipX1 + aGraphics.getClipWidth();
            int clipY2 = clipY1 + aGraphics.getClipHeight();

            final int tw = tileWidth;
            final int th = tileHeight;
            final int lw = getWidth(); // layer width
            final int lh = getHeight(); // layer height

            clipX1 = (clipX1 < 0 ? 0 : clipX1);
            clipY1 = (clipY1 < 0 ? 0 : clipY1);
            clipX2 = (clipX2 > lw ? lw : clipX2);
            clipY2 = (clipY2 > lh ? lh : clipY2);

            final int begRow = clipY1 / tileHeight;
            final int begCol = clipX1 / tileWidth;
            final int endRow = (clipY2 - 1) / tileHeight;
            final int endCol = (clipX2 - 1) / tileWidth;

            int sx = begRow * th;
            int sy = begCol * tw;
            int dx;
            int dy;

            for (int row = begRow; row <= endRow; row++) {
                for (int col = begCol; col <= endCol; col++) {
                    final int index = validateTileIndex(cells[row][col]);
                    if (index >= 0) {
                        // Future performace improvement suggestion: accumulate
                        // region
                        //
                        // We have a visible tile in the clip rect, if its
                        // bounding rect is disjoint from our accumulated rect,
                        // we draw what we have accumulated so far and reset our
                        // accumulation rect, else we just add this tile to the
                        // accumulation rect.
                        //
                        // This helps with the not totally uncommon case of
                        // cells in the tiled layer containing adjacent tiles
                        // from the tile image. Accumulating the tiles turns
                        // multiple bitblt's into a single bitblt, saving a bit
                        // on setup/teardown/data transfer.

                        dx = col * tw;
                        dy = row * th;

                        int tr = index / tileColumns;
                        int tc = index % tileColumns;

                        sx = tc * tw;
                        sy = tr * th;

                        aGraphics.drawRegion(tileImage,
                                sx, sy,
                                tw, th,
                                Sprite.TRANS_NONE,
                                dx, dy, 0);
                    }
                }
            }

            aGraphics.translate(-x, -y);
        }
    }

    private int validateTileIndex(int tileIndex) {
        if (tileIndex > tileCount) {
            throw new IndexOutOfBoundsException(
                    MsgRepository.TILEDLAYER_EXCEPTION_INVALID_TILE_INDEX);
        }
        // check createAnimatedTile has been called, getAnimatedTile will throw
        // exception if hasn't.
        if (tileIndex < 0) {
            tileIndex = getAnimatedTile(tileIndex);
        }
        return tileIndex - 1;
    }

    boolean collidesCell(int aX1, int aY1, int aX2, int aY2) {
        // tiled layer rect in painter's coordinates
        int tlX1 = x;
        int tlY1 = y;
        int tlX2 = x + getWidth();
        int tlY2 = y + getHeight();

        // compute intersection of TiledLayer bounds and sprite collision
        // rectangle.
        tlX1 = Math.max(tlX1, aX1);
        tlY1 = Math.max(tlY1, aY1);
        tlX2 = Math.min(tlX2, aX2);
        tlY2 = Math.min(tlY2, aY2);

        // return false if empty intersection.
        if (!((tlX1 < tlX2) && (tlY1 < tlY2))) {
            return false;
        }

        // transform to tiled layer coordinates
        tlX1 -= x;
        tlY1 -= y;
        tlX2 -= x;
        tlY2 -= y;

        int startRow = (tlY1 / tileHeight);
        int startCol = (tlX1 / tileWidth);
        int endRow = (tlY2 - 1) / tileHeight;
        int endCol = (tlX2 - 1) / tileWidth;

        for (int row = startRow; row <= endRow; row++) {
            for (int col = startCol; col <= endCol; col++) {
                if (cells[row][col] != 0) {
                    // at least one cell is not empty
                    return true;
                }
            }
        }

        // all cells in overlap are empty
        return false;
    }

    int getLayerWidth() {
        return cols * tileWidth;
    }

    int getLayerHeight() {
        return rows * tileHeight;
    }
}
