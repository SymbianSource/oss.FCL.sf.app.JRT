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
package com.nokia.openlcdui.mt.game;

import junit.framework.*;

import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.game.TiledLayer;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * TiledLayer non interactive tests
 */
public class TiledLayerTest extends SWTTestCase {

    // data used to create Images
    // image has width 40, height 30
    private byte[] iImgData40x30 = {71, 73, 70, 56, 57, 97, 40, 0, 30, 0, -14,
            0, 0, -128, -128, -128, 36, 0, -1, 0, 0, 85, 0, 0, -86, 0, 109, -1,
            0, -110, -1, 73, 0, -1, 0, 0, -1, 33, -7, 4, 1, 0, 0, 0, 0, 44, 0,
            0, 0, 0, 40, 0, 30, 0, 2, 3, -1, 120, -70, 107, -66, 37, 18, 18,
            11, -107, -41, -114, 33, -74, 96, -32, -31, 24, 16, 86, 101, -45,
            -23, 121, 33, 51, 46, 83, 44, -49, 113, -79, 110, -121, -96, -73,
            -96, -50, 117, 62, 93, -121, -29, 9, 26, 63, -68, -59, 102, -71,
            28, 58, 111, -49, 80, -51, -110, 10, 5, 2, -122, -21, 104, -101,
            100, 76, 107, 86, -84, 118, -5, -24, 42, 104, 85, 16, 121, 77,
            -110, 78, 88, 61, 96, 39, -76, 116, 96, -53, 46, 9, -100, 33, 20,
            -122, 124, 90, 90, 86, 73, 80, -123, 68, -121, 79, -121, 3, 93, 62,
            76, 69, 66, 63, -111, 70, -111, -116, 67, -114, 125, 114, -110,
            -103, 62, 57, 115, 60, 49, 108, 118, 119, 120, 74, 55, -117, 60,
            21, -95, -93, 47, 32, 43, 11, -100, 106, -92, 113, -102, -76, 72,
            -99, -89, 12, -126, 60, -122, -119, -98, -73, 116, -120, 80, -112,
            28, 118, 118, 21, 5, -81, -66, -91, -67, -113, 78, -59, 89, -57,
            -55, -72, -55, -112, -115, -63, -60, 100, -47, -83, -120, -53, 80,
            73, 21, -107, -108, 10, 77, -103, -33, 17, 102, 10, -61, 78, 11,
            99, 91, -85, 100, -74, 127, -101, -66, -19, 119, -10, -49, 1, -45,
            -77, -109, -20, -93, -9, 89, -8, 104, -95, 91, 19, -125, -48, 35,
            113, 93, -38, 25, 0, -73, -117, -120, -68, 1, -127, -60, 72, 108,
            115, -96, -114, 42, 49, 15, 35, 102, 89, 37, 45, 11, -108, 71, 7,
            -109, -10, -96, 11, 49, 66, 64, 2, 0, 59, 0,};

    // image has width 120, height 10
    private byte[] iImgData120x10 = {71, 73, 70, 56, 57, 97, 120, 0, 10, 0,
            -14, 0, 0, -128, -128, -128, 36, 0, -1, 0, 0, 85, 0, 0, -86, 0,
            109, -1, 0, -110, -1, 73, 0, -1, 0, 0, -1, 33, -7, 4, 1, 0, 0, 0,
            0, 44, 0, 0, 0, 0, 120, 0, 10, 0, 66, 3, -75, 120, -70, -36, -2,
            38, -54, -7, 78, -71, 56, -109, -51, -77, -18, -34, -59, -127, -95,
            72, 12, 104, 42, -84, 66, -86, 10, 85, 44, 47, 44, -21, -42, 43,
            -118, -73, 105, 21, -4, -64, -119, -48, -16, 27, 2, 3, 70, -92,
            -16, -104, 68, -51, -98, -115, -99, 107, 74, -83, -18, -82, 60, 29,
            78, 91, -69, 109, 7, -46, 87, 119, 0, -115, -115, 70, -95, -13, -7,
            82, 102, 28, -117, -110, -73, 114, 72, 23, -74, -37, 97, -25, -3,
            64, -51, 77, -57, 94, 125, 88, 54, 127, 44, 73, 73, 63, 37, 24,
            -125, 48, 123, -114, 121, 96, 95, -112, 88, -126, 126, 94, -109,
            87, 40, -118, 27, -107, -115, 119, -116, 88, -113, 95, 114, 19,
            -92, 6, -114, 63, 50, -112, -114, 117, -114, -86, -110, 95, 92,
            -124, -104, 99, 43, -82, 7, 97, -85, 15, 85, 98, -77, -79, 100,
            -73, 13, -118, 108, -63, 21, 97, -90, 17, -90, -56, 68, 111, 17,
            50, -68, 117, -47, -46, -52, 76, -52, 9, 0, 59, 0};

    // image has width 20, height 60
    private byte[] iImgData20x60 = {71, 73, 70, 56, 57, 97, 20, 0, 60, 0, -14,
            0, 0, -128, -128, -128, 36, 0, -1, 0, 0, 85, 0, 0, -86, 0, 109, -1,
            0, -110, -1, 73, 0, -1, 0, 0, -1, 33, -7, 4, 1, 0, 0, 0, 0, 44, 0,
            0, 0, 0, 20, 0, 60, 0, 2, 3, -1, 120, -86, 102, 87, -123, -60, 89,
            -122, 24, -117, 57, 40, 59, -76, 88, -42, 60, -109, 87, 93, -39,
            49, 114, -27, -121, -118, 91, 43, -99, -31, -78, -54, 110, -83,
            -111, -34, 12, 10, -126, -44, -31, 2, -4, -3, -128, 66, 34, -47, 8,
            12, -90, -108, 76, 11, -14, 41, -115, 54, -109, -43, -91, -46,
            -103, -127, 106, 45, -103, 14, 97, 17, 48, -108, 71, 66, 49, -39,
            -36, 112, -92, 35, 99, 69, -7, -20, 78, -87, -27, 108, -76, 29,
            -66, -90, 11, 15, 119, 7, 115, 109, 11, 46, 11, 87, 10, 2, 115,
            -123, 39, -121, 83, 67, -117, 10, -122, -119, -113, -118, 102,
            -116, 96, -108, 92, -106, 1, -104, -101, -107, -111, 36, -103, 67,
            -96, -105, 127, 23, 3, -87, 69, -85, 68, 84, -87, 89, 94, -82, -86,
            94, -83, 93, -81, -80, 89, -78, -72, -85, -70, -84, -71, 67, 53,
            117, -108, -65, 47, 42, 79, -79, -64, 54, -57, -60, -63, -53, -68,
            -55, 12, -50, -75, -59, -62, -92, -52, 11, -86, -77, 22, 13, 1, 5,
            -119, 53, -39, 89, -36, -34, -48, 7, -31, -85, -29, -33, -40, 32,
            -30, 102, -28, -59, -25, 68, -23, -27, -15, -37, -18, 93, 40, -38,
            25, 16, -53, -26, 85, -5, -28, 28, 97, -48, -57, -88, 31, 65, 73,
            1, -121, -7, 43, 2, -48, -32, -65, -126, 84, 24, -46, 57, 83, 38,
            34, -118, -119, -36, -86, 65, 17, -44, -122, 45, -94, -58, 127, 24,
            -39, 88, -60, 16, -78, -94, 45, -119, 29, 51, 118, -103, 19, 105,
            64, 27, 110, 43, -51, -76, 124, 105, 42, 17, 75, 83, 46, 95, 118,
            58, 116, 115, 103, -50, -114, 49, 3, -52, -44, -103, 0, 0, 59, 0};

    // image has width 60, height 20
    private byte[] iImgData60x20 = {71, 73, 70, 56, 57, 97, 60, 0, 20, 0, -14,
            0, 0, -128, -128, -128, 36, 0, -1, 0, 0, 85, 0, 0, -86, 0, 109, -1,
            0, -110, -1, 73, 0, -1, 0, 0, -1, 33, -7, 4, 1, 0, 0, 0, 0, 44, 0,
            0, 0, 0, 60, 0, 20, 0, 2, 3, -26, 120, -70, -52, -10, -113, -107,
            57, -119, -91, -27, 82, 75, 112, 30, -96, 32, -128, -125, -48,
            -100, 14, 100, 72, 24, -41, 118, -101, -74, -111, 34, 125, -120,
            56, -86, 47, 120, 88, -9, 37, 96, -112, 6, -60, -115, -116, -90,
            -99, -114, -60, 108, -42, 66, 68, 90, 20, -86, -28, -64, 50, -42,
            -62, 46, -64, 125, 112, 3, -86, -16, 74, -71, -77, -102, 93, 59,
            -79, 58, -84, 108, 17, 119, 72, 35, -68, -10, -11, 118, -73, -28,
            -59, -76, -76, 31, -7, -92, 127, 80, -127, 32, 121, 60, 79, 77, 76,
            113, 126, 61, 69, 63, 79, 61, 55, -112, -123, 86, 107, 98, 117,
            108, 75, 54, 12, 64, 105, 16, -123, -111, 71, -113, -114, -117, 63,
            13, -100, 58, 117, -97, -128, 67, -126, -84, 124, -90, 79, 59, -85,
            -121, 126, 65, 70, 68, 16, 96, 118, 30, -80, 126, 115, 72, 62, -75,
            -82, -75, 34, -107, -68, -65, -74, -124, 122, -119, -93, -54, 100,
            -57, -97, -122, 66, -60, 73, 10, -105, 106, 93, -107, 16, -43, 100,
            -115, -95, -66, 11, -41, 94, -71, -70, 6, -27, -39, -26, -74, -33,
            -50, -47, -107, 86, -86, -102, 58, 61, -105, 117, -12, 119, 12, 76,
            -38, 42, -11, 74, -13, -39, -11, -24, 60, 109, -86, -95, 111, 77,
            2, 0, 59, 0};

    // create images from byte array data
    Image[] iImages = {
            Image.createImage(iImgData60x20, 0, iImgData60x20.length), // grid
            Image.createImage(iImgData40x30, 0, iImgData40x30.length), // grid
            Image.createImage(iImgData120x10, 0, iImgData120x10.length), // vertical
            Image.createImage(iImgData20x60, 0, iImgData20x60.length), // horizontal
    };

    // data used to create TiledLayer with 6 tiles.
    int iTileCount6 = 6;
    int iTileWidth6 = 20;
    int iTileHeight6 = 10;

    // data used to create TiledLayer with 12 tiles.
    int iTileCount12 = 12;
    int iTileWidth12 = 10;
    int iTileHeight12 = 10;

    // data used to create TiledLayer with 24 tiles.
    int iTileCount24 = 24;
    int iTileWidth24 = 5;
    int iTileHeight24 = 10;

    //
    // 4x2 cells
    int iCols = 4;
    int iRows = 2;
    int[][] iDefaultCells = { {0, 0, 0, 0}, {0, 0, 0, 0}};
    int[][] iSetCells = { {1, 2, 3, 4}, {1, 2, 3, 4}};


    /**
     * Constructor.
     */
    public TiledLayerTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public TiledLayerTest(String sTestName) {
        super(sTestName);
    }

    /**
     * To create the test suite. You need to add a new aSuite.addTest entry for
     * any new test methods.
     *
     * @return new testsuite.
     */
    public static Test suite() {
		TestSuite suite = new TestSuite();

	    java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add widget tests
	    methodNames = TiledLayerTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new TiledLayerTest((String)e.nextElement()));
	    }

		return suite;
	}

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testConstructors");
        methodNames.addElement("testCell");
        methodNames.addElement("testAnimatedTiles");
        methodNames.addElement("testChangeImage");
        methodNames.addElement("testFillCells");
        return methodNames;
    }
    
    protected void runTest() throws Throwable {
        if (getName().equals("testConstructors")) testConstructors();
        else if (getName().equals("testCell")) testCell();
        else if (getName().equals("testAnimatedTiles")) testAnimatedTiles();
        else if (getName().equals("testChangeImage")) testChangeImage();
        else if (getName().equals("testFillCells")) testFillCells();
        else super.runTest();
    }
    
    /**
     * 1.0 TiledLayer - Construction Test
     */
    public void testConstructors() {
        Image image = iImages[0];
        int tileWidth = iTileWidth6;
        int tileHeight = iTileHeight6;
        TiledLayer tiledLayer = null;

        // checking("Test 1.01 Construct with wrong row");
        try {
            tiledLayer = new TiledLayer(iCols, 0, image, tileWidth, tileHeight);
            fail("IllegalArgumentException expected");
        }
        catch (java.lang.IllegalArgumentException ex) {
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 1.02 Construct with wrong col");
        try {
            tiledLayer = new TiledLayer(0, iRows, image, tileWidth, tileHeight);
            fail("IllegalArgumentException expected");
        }
        catch (java.lang.IllegalArgumentException ex) {
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 1.03 Construct with tile width=0");
        try {
            tiledLayer = new TiledLayer(iCols, iRows, image, 0, tileHeight);
            fail("IllegalArgumentException expected");
        }
        catch (java.lang.IllegalArgumentException ex) {
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 1.04 Construct with tile height=0");
        try {
            int wrongWidth = image.getWidth() / 2 + 1;
            tiledLayer = new TiledLayer(iCols, iRows, image, tileWidth, 0);
            fail("IllegalArgumentException expected");
        }
        catch (java.lang.IllegalArgumentException ex) {
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 1.05 Construct with incorrect tile width for image");
        try {
            // image width is not an integer multiple of the tileWidth
            int wrongWidth = image.getWidth() / 2 + 1;
            tiledLayer = new TiledLayer(iCols, iRows, image, wrongWidth,
                    tileHeight);
            fail("IllegalArgumentException expected");
        }
        catch (java.lang.IllegalArgumentException ex) {
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 1.06 Construct with incorrect tile height for image");
        try {
            // image height is not an integer multiple of the tileHeight
            int wrongHeight = image.getHeight() / 2 + 1;
            tiledLayer = new TiledLayer(iCols, iRows, image, tileWidth,
                    wrongHeight);
            fail("IllegalArgumentException expected");
        }
        catch (java.lang.IllegalArgumentException ex) {
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 1.07 Construct with correct tile size");
        for (int i = 0; i < iImages.length; i++) {
            Image img = iImages[i];
            // single tile
            runConstructTest(img, img.getWidth(), img.getHeight(), 1);
            // vary number of tiles and way in which tiles stored
            // - grid, horizontally or vertically.
            runConstructTest(img, iTileWidth6, iTileHeight6, iTileCount6);
            runConstructTest(img, iTileWidth12, iTileHeight12, iTileCount12);
            runConstructTest(img, iTileWidth24, iTileHeight24, iTileCount24);
        }
    }

    // checks TiledLayer properties are correct after construction
    private void runConstructTest(Image image,
            int tileWidth, int tileHeight, int tileCount) {
        // vary number of rows and columns
        for (int r = 1; r < iRows; r++) {
            for (int c = 1; c < iCols; c++) {
                TiledLayer tl = new TiledLayer(c, r, image, tileWidth, tileHeight);
                assertEquals("getWidth ", tl.getWidth(), tileWidth * c);
                assertEquals("getHeight ", tl.getHeight(), tileHeight * r);
                assertEquals("getCellHeight ", tl.getCellHeight(), tileHeight);
                assertEquals("getCellWidth ", tl.getCellWidth(), tileWidth);
                assertEquals("getRows ", tl.getRows(), r);
                assertEquals("getColumns ", tl.getColumns(), c);
                int[][] defaultCells = new int[r][c];
                checkCells(tl, defaultCells);
            }
        }
    }

    /**
     * 2.0 TiledLayer - Cell Test
     */
    public void testCell() {
        Image image = iImages[0];
        int tileWidth = iTileWidth6;
        int tileHeight = iTileHeight6;
        int tileCount = iTileCount6;
        TiledLayer tiledLayer;

        // checking("Test 2.01 CellTest: wrong tile index");
        try {
            tiledLayer = new TiledLayer(iCols, iRows, image, tileWidth,
                    tileHeight);
            tiledLayer.setCell(0, 0, tileCount + 1);
            fail("IndexOutOfBoundsException expected");
        }
        catch (java.lang.IndexOutOfBoundsException ex) {
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 2.02 CellTest: col is outside the bounds of the TiledLayer grid");
        try {
            tiledLayer = new TiledLayer(iCols, iRows, image, tileWidth,
                    tileHeight);
            tiledLayer.setCell(iCols, iRows - 1, tileCount);
            fail("IndexOutOfBoundsException expected");
        }
        catch (java.lang.IndexOutOfBoundsException ex) {
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 2.03 CellTest: row is outside the bounds of the TiledLayer grid");
        try {
            tiledLayer = new TiledLayer(iCols, iRows, image, tileWidth,
                    tileHeight);
            tiledLayer.setCell(iCols - 1, iRows, tileCount);
            fail("IndexOutOfBoundsException expected");
        }
        catch (java.lang.IndexOutOfBoundsException ex) {
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 2.04 CellTest: default cells");
        tiledLayer = createTiledLayer(iCols, iRows, image, tileWidth,
                tileHeight, null);
        checkCells(tiledLayer, iDefaultCells);

        // checking("Test 2.05 CellTest: setCells");
        tiledLayer = createTiledLayer(iCols, iRows, image, tileWidth,
                tileHeight, iSetCells);
        checkCells(tiledLayer, iSetCells);

        // checking("Test 2.06 CellTest: fillCells");
        tiledLayer = createTiledLayer(iCols, iRows, image, tileWidth,
                tileHeight, null);
        tiledLayer.fillCells(0, 0, 1, 2, 1);
        tiledLayer.fillCells(1, 0, 2, 1, 2);
        tiledLayer.fillCells(2, 1, 2, 1, 3);
        int[][] fillCells = { {1, 2, 2, 0}, {1, 0, 3, 3}};
        checkCells(tiledLayer, fillCells);
    }

    /**
     * 3.0 TiledLayer - Animated Tiles Test
     */
    public void testAnimatedTiles() {
        TiledLayer tiledLayer = new TiledLayer(iCols, iRows, iImages[0],
                iTileWidth6, iTileHeight6);

        // checking("Test 3.01 Animated Tiles Test: invalid static tile index");
        try {
            tiledLayer.createAnimatedTile(iTileCount6 + 1);
            fail("IndexOutOfBoundsException expected");
        }
        catch (java.lang.IndexOutOfBoundsException ex) {
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 3.02 Animated Tiles Test: create and set animated tiles");
        for (int i = 0; i < iTileCount6 * 3; i++) {
            int tileIndex = i % iTileCount6;
            int animTile = tiledLayer.createAnimatedTile(tileIndex);
            assertEquals("index of newly created animated tile", animTile, ~i);
            assertEquals("index returned by getAnimatedTile", tiledLayer
                    .getAnimatedTile(animTile), tileIndex);
            int newTileIndex = i % (iTileCount6 / 2);
            tiledLayer.setAnimatedTile(animTile, newTileIndex);
            assertEquals("index after setAnimatedTile", tiledLayer
                    .getAnimatedTile(animTile), newTileIndex);
        }

        // checking("Test 3.03 Animated Tiles Test: set cells to animated tiles");
        tiledLayer = new TiledLayer(iCols, iRows, iImages[0], iTileWidth6,
                iTileHeight6);
        int animTile1 = tiledLayer.createAnimatedTile(0);
        int animTile2 = tiledLayer.createAnimatedTile(1);
        tiledLayer.fillCells(0, 0, 4, 1, animTile1);
        tiledLayer.fillCells(0, 1, 4, 1, animTile2);

        int[][] animCells = { {-1, -1, -1, -1}, {-2, -2, -2, -2}};
        int[][] staticCells = { {0, 0, 0, 0}, {1, 1, 1, 1}};
        checkAnimCells(tiledLayer, animCells, staticCells);

        tiledLayer.setAnimatedTile(animTile1, 2);
        tiledLayer.setAnimatedTile(animTile2, 3);
        int[][] newStaticCells = { {2, 2, 2, 2}, {3, 3, 3, 3}};
        checkAnimCells(tiledLayer, animCells, newStaticCells);
    }

    /**
     * 4.0 TiledLayer - Change Image Test
     */
    public void testChangeImage() {
        // checking("Test 4.01 Change TiledLayer image: same tiles, clear cells, no anim tiles");
        // Check: Animated tiles and cell contents preserved
        TiledLayer tiledLayer = createTiledLayer(iCols, iRows, iImages[0],
                iTileWidth12, iTileHeight12, null);
        tiledLayer.setStaticTileSet(iImages[1], iTileWidth12, iTileHeight12);
        checkCells(tiledLayer, iDefaultCells);
        checkAnimCellsPreserved(tiledLayer, iTileCount12, false);

        // checking("Test 4.02 Change TiledLayer image: same tiles, cells set, anim cells set");
        // Check: Animated tiles and cell contents preserved
        tiledLayer = createTiledLayer(iCols, iRows, iImages[0], iTileWidth12,
                iTileHeight12, iSetCells);
        createAnimatedTiles(tiledLayer, iTileCount12);
        tiledLayer.setStaticTileSet(iImages[2], iTileWidth12, iTileHeight12);
        checkCells(tiledLayer, iSetCells);
        checkAnimCellsPreserved(tiledLayer, iTileCount12, true);

        // checking("Test 4.03 Change TiledLayer image: more tiles, clear cells, no anim tiles");
        // Check: Animated tiles and cell contents preserved
        tiledLayer = createTiledLayer(iCols, iRows, iImages[0], iTileWidth12,
                iTileHeight12, null);
        tiledLayer.setStaticTileSet(iImages[3], iTileWidth24, iTileHeight24);
        checkCells(tiledLayer, iDefaultCells);
        checkAnimCellsPreserved(tiledLayer, iTileCount12, false);

        // checking("Test 4.04 Change TiledLayer image: more tiles, cells set, anim tiles");
        // Check: Animated tiles and cell contents preserved
        tiledLayer = createTiledLayer(iCols, iRows, iImages[1], iTileWidth12,
                iTileHeight12, iSetCells);
        createAnimatedTiles(tiledLayer, iTileCount12);
        tiledLayer.setStaticTileSet(iImages[2], iTileWidth24, iTileHeight24);
        checkCells(tiledLayer, iSetCells);
        checkAnimCellsPreserved(tiledLayer, iTileCount12, true);

        // checking("Test 4.05 Change TiledLayer image: less tiles, clear cells, no anim tiles");
        // Check: Contents of the grid cleared (all contain index 0) and all
        // animated tiles deleted.
        tiledLayer = createTiledLayer(iCols, iRows, iImages[1], iTileWidth12,
                iTileHeight12, null);
        tiledLayer.setStaticTileSet(iImages[3], iTileWidth6, iTileHeight6);
        checkCells(tiledLayer, iDefaultCells);
        checkAnimCellsPreserved(tiledLayer, iTileCount6, false);

        // checking("Test 4.06 Change TiledLayer image: less tiles, cells set, anim tiles");
        // Check: Contents of the grid cleared (all contain index 0) and all
        // animated tiles deleted.
        tiledLayer = createTiledLayer(iCols, iRows, iImages[2], iTileWidth12,
                iTileHeight12, iSetCells);
        createAnimatedTiles(tiledLayer, iTileCount12);
        tiledLayer.setStaticTileSet(iImages[3], iTileWidth6, iTileHeight6);
        checkCells(tiledLayer, iDefaultCells);
        checkAnimCellsPreserved(tiledLayer, iTileCount6, false);
    }

    /**
     * 5.0 TiledLayer - Fill Cells Test
     */
    public void testFillCells() {
        Image image = iImages[0];
        int tileWidth = iTileWidth6;
        int tileHeight = iTileHeight6;
        TiledLayer tiledLayer;

        // checking("Test 5.01 Number of absolute columns less than zero");
        // Check: correct exception should be thrown
        try {
            tiledLayer = new TiledLayer(iCols, iRows, image, tileWidth,
                    tileHeight);
            tiledLayer.fillCells(iCols / 2, iRows / 2, -1, iRows / 4, 1);
            fail("IllegalArgumentException expected");
        }
        catch (java.lang.IllegalArgumentException ex) {
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 5.02 Number of absolute rows less than zero");
        // Check: correct exception should be thrown
        try {
            tiledLayer = new TiledLayer(iCols, iRows, image, tileWidth,
                    tileHeight);
            tiledLayer.fillCells(iCols / 2, iRows / 2, iCols / 4, -1, 1);
            fail("IllegalArgumentException expected");
        }
        catch (java.lang.IllegalArgumentException ex) {
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 5.03 Column extends bounds of grid");
        // Check: correct exception should be thrown
        try {
            tiledLayer = new TiledLayer(iCols, iRows, image, tileWidth,
                    tileHeight);
            tiledLayer.fillCells(iCols + 1, iRows / 2, 1, 1, 1);
            fail("IndexOutOfBoundsException expected");
        }
        catch (java.lang.IndexOutOfBoundsException ex) {
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 5.04 Row extends bounds of grid");
        // Check: correct exception should be thrown
        try {
            tiledLayer = new TiledLayer(iCols, iRows, image, tileWidth,
                    tileHeight);
            tiledLayer.fillCells(iCols / 2, iRows + 1, 1, 1, 1);
            fail("IndexOutOfBoundsException expected");
        }
        catch (java.lang.IndexOutOfBoundsException ex) {
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }
    }

    // creates an animated tile for each tile and one for a clear cell
    private void createAnimatedTiles(TiledLayer aTiledLayer, int aMaxTileIndex) {
        for (int i = aMaxTileIndex; i >= 0; --i)
            aTiledLayer.createAnimatedTile(i);
    }

    // checks if animated cells have been deleted
    private void checkAnimCellsPreserved(TiledLayer tl, int aMaxTileIndex,
            boolean aPreserved) {
        if (aPreserved) {
            // check animated cells are still present
            // should be one for each tile and one for a clear cell
            for (int i = 0; i <= aMaxTileIndex; ++i) {
                assertEquals("animated cells have not been deleted",
                        aMaxTileIndex - i, tl.getAnimatedTile(~i));
            }
            assertEquals("count has not been reset", tl
                    .createAnimatedTile(aMaxTileIndex), ~(aMaxTileIndex + 1));
        }
        else {
            // check cannot get or set animated tiles
            try {
                tl.getAnimatedTile(-1);
            }
            catch (java.lang.IndexOutOfBoundsException ex) {
            }
            catch (Exception ex) {
                fail("Wrong exception " + ex);
            }

            try {
                tl.setAnimatedTile(-1, aMaxTileIndex);
            }
            catch (java.lang.IndexOutOfBoundsException ex) {
            }
            catch (Exception ex) {
                fail("Wrong exception " + ex);
            }

            // a new animated tile should have index -1
            assertEquals("count has been reset", tl
                    .createAnimatedTile(aMaxTileIndex), -1);
        }
    }

    // create TiledLayer and setCells
    private TiledLayer createTiledLayer(int cols, int rows, Image img,
            int tileWidth, int tileHeight, int[][] aCells) {
        TiledLayer tl = new TiledLayer(cols, rows, img, tileWidth, tileHeight);
        if (aCells != null) {
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++)
                    tl.setCell(j, i, aCells[i][j]);
            }
        }
        return tl;
    }

    // checks cell contents
    private void checkCells(TiledLayer tiledLayer, int[][] expectedCells) {
        for (int i = 0; i < tiledLayer.getRows(); i++) {
            for (int j = 0; j < tiledLayer.getColumns(); j++)
                assertEquals("getCell ", tiledLayer.getCell(j, i),
                        expectedCells[i][j]);
        }
    }

    // checks cell contents, use if all animated tiles
    private void checkAnimCells(TiledLayer tiledLayer, int[][] animCells,
            int[][] staticCells) {
        for (int r = 0; r < tiledLayer.getRows(); r++) {
            for (int c = 0; c < tiledLayer.getColumns(); c++) {
                int tileIndex = tiledLayer.getCell(c, r);
                assertEquals("getCell ", tileIndex, animCells[r][c]);
                assertEquals("getAnimatedTile ", tiledLayer
                        .getAnimatedTile(tileIndex), staticCells[r][c]);
            }
        }
    }

}
