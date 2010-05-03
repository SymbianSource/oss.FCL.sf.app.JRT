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

import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.game.*;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
* Performs series of non-interactive tests on
* {@link javax.microedition.lcdui.game.LayerManager} .
*/
public class LayerManagerTest extends SWTTestCase {

	/**
	* Layer manager to be used for all tests.
	*	Constructed during the first test, destroyed at cleanup.
	*/
	LayerManager iLayerManager = null;

	// image data
	int [] iArgbDataWhite = new int[400];
	int [] iArgbDataBlack = new int[400];

	Sprite iSpriteWhite = null;
	Sprite iSpriteBlack = null;

	private static final int BLACK = 0xFF000000;
	private static final int WHITE = 0xFFFFFFFF;
	private static final int GREEN = 0xFF00FF00;


	// Image data for creating a sprite
	private byte[] iImgData40x30 =
		{
		71, 73, 70, 56, 57, 97, 40, 0, 30, 0, -14, 0, 0, -128, -128, -128, 36, 0, -1, 0, 0,
		85, 0, 0, -86, 0, 109, -1, 0, -110, -1, 73, 0, -1, 0, 0, -1, 33, -7, 4, 1, 0, 0,
		0, 0, 44, 0, 0, 0, 0, 40, 0, 30, 0, 2, 3, -1, 120, -70, 107, -66, 37, 18, 18, 11,
		-107, -41, -114, 33, -74, 96, -32, -31, 24, 16, 86, 101, -45, -23, 121, 33, 51,
		46, 83, 44, -49, 113, -79, 110, -121, -96, -73, -96, -50, 117, 62, 93, -121, -29,
		9, 26, 63, -68, -59, 102, -71, 28, 58, 111, -49, 80, -51, -110, 10, 5, 2, -122,
		-21, 104, -101, 100, 76, 107, 86, -84, 118, -5, -24, 42, 104, 85, 16, 121, 77,
		-110, 78, 88, 61, 96, 39, -76, 116, 96, -53, 46, 9, -100, 33, 20, -122, 124, 90,
		90, 86, 73, 80, -123, 68, -121, 79, -121, 3, 93, 62, 76, 69, 66, 63, -111, 70,
		-111, -116, 67, -114, 125, 114, -110, -103, 62, 57, 115, 60, 49, 108, 118, 119,
		120, 74, 55, -117, 60, 21, -95, -93, 47, 32, 43, 11, -100, 106, -92, 113, -102,
		-76, 72, -99, -89, 12, -126, 60, -122, -119, -98, -73, 116, -120, 80, -112, 28,
		118, 118, 21, 5, -81, -66, -91, -67, -113, 78, -59, 89, -57, -55, -72, -55, -112,
		-115, -63, -60, 100, -47, -83, -120, -53, 80, 73, 21, -107, -108, 10, 77, -103,
		-33, 17, 102, 10, -61, 78, 11, 99, 91, -85, 100, -74, 127, -101, -66, -19, 119,
		-10, -49, 1, -45, -77, -109, -20, -93, -9, 89, -8, 104, -95, 91, 19, -125, -48,
		35, 113, 93, -38, 25, 0, -73, -117, -120, -68, 1, -127, -60, 72, 108, 115, -96,
		-114, 42, 49, 15, 35, 102, 89, 37, 45, 11, -108, 71, 7, -109, -10, -96, 11, 49,
		66, 64, 2, 0, 59, 0,
		};

	private byte[] iImgData120x10 =
		{
		71, 73, 70, 56, 57, 97, 120, 0, 10, 0, -14, 0, 0, -128, -128, -128, 36, 0,
		-1, 0, 0, 85, 0, 0, -86, 0, 109, -1, 0, -110, -1, 73, 0, -1, 0, 0, -1, 33,
		-7, 4, 1, 0, 0, 0, 0, 44, 0, 0, 0, 0, 120, 0, 10, 0, 66, 3, -75, 120, -70,
		-36, -2, 38, -54, -7, 78, -71, 56, -109, -51, -77, -18, -34, -59, -127, -95,
		72, 12, 104, 42, -84, 66, -86, 10, 85, 44, 47, 44, -21, -42, 43, -118, -73,
		105, 21, -4, -64, -119, -48, -16, 27, 2, 3, 70, -92, -16, -104, 68, -51, -98,
		-115, -99, 107, 74, -83, -18, -82, 60, 29, 78, 91, -69, 109, 7, -46, 87, 119,
		0, -115, -115, 70, -95, -13, -7, 82, 102, 28, -117, -110, -73, 114, 72, 23,
		-74, -37, 97, -25, -3, 64, -51, 77, -57, 94, 125, 88, 54, 127, 44, 73, 73,
		63, 37, 24, -125, 48, 123, -114, 121, 96, 95, -112, 88, -126, 126, 94, -109,
		87, 40, -118, 27, -107, -115, 119, -116, 88, -113, 95, 114, 19, -92, 6, -114,
		63, 50, -112, -114, 117, -114, -86, -110, 95, 92, -124, -104, 99, 43, -82, 7,
		97, -85, 15, 85, 98, -77, -79, 100, -73, 13, -118, 108, -63, 21, 97, -90, 17,
		-90, -56, 68, 111, 17, 50, -68, 117, -47, -46, -52, 76, -52, 9, 0, 59, 0
		};

    /**
     * Constructor.
     */
    public LayerManagerTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public LayerManagerTest(String sTestName) {
        super(sTestName);
    }

    protected void setUp() throws Exception {
        super.setUp();
        for (int i = 0; i < 400; i++) {
            iArgbDataWhite[i] = WHITE; // fully opaque white
            iArgbDataBlack[i] = BLACK; // fully opaque black
        }

        // Images are black and white inline with spec to allow tests
        // for devices without color capabilities
        iSpriteWhite = new Sprite(Image.createRGBImage(iArgbDataWhite, 20, 20,
                true));
        iSpriteBlack = new Sprite(Image.createRGBImage(iArgbDataBlack, 20, 20,
                true));
        // set sprite positions
        iSpriteWhite.setPosition(0, 0);
        iSpriteBlack.setPosition(10, 10);
    }

	protected void tearDown() throws Exception {
        super.tearDown();
        iLayerManager = null;
        iArgbDataBlack = null;
        iArgbDataWhite = null;
        iSpriteBlack = null;
        iSpriteWhite = null;
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
	    methodNames = LayerManagerTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new LayerManagerTest((String)e.nextElement()));
	    }

		return suite;
	}
    
    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testLayers");
        methodNames.addElement("testViewWindow");
        methodNames.addElement("testPaint");
        return methodNames;
    }
    
    protected void runTest() throws Throwable {
        if (getName().equals("testLayers")) testLayers();
        else if (getName().equals("testViewWindow")) testViewWindow();
        else if (getName().equals("testPaint")) testPaint();
        else super.runTest();
    }

    /**
	* Tests construction and Layer management methods. Specifically these
	* methods are tested:<br>
	* {@link javax.microedition.lcdui.game.LayerManager#LayerManager} (trivial)<br>
	* {@link javax.microedition.lcdui.game.LayerManager#append}<br>
	* {@link javax.microedition.lcdui.game.LayerManager#insert}<br>
	* {@link javax.microedition.lcdui.game.LayerManager#remove}<br>
	* {@link javax.microedition.lcdui.game.LayerManager#getLayerAt}<br>
	* {@link javax.microedition.lcdui.game.LayerManager#getSize}<br>
	*/
	public void testLayers() {
        // make a layer (sprite) for testing
        Image img = Image.createImage(iImgData40x30, 0, iImgData40x30.length);
        Image img2 = Image.createImage(iImgData120x10, 0, iImgData120x10.length);
        Sprite sprite = new Sprite(img);
        Sprite anotherSprite = new Sprite(img2);

        try {
            iLayerManager = new LayerManager();
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        try {
            iLayerManager.insert(null, 0);
            fail("NullPointerException expected");
        }
        catch (NullPointerException ex) {
            // pass
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 1.03 Insert layer - invalid index ");
        try {
            iLayerManager.insert(sprite, -1);
            fail("IndexOutOfBoundsException expected");
        }
        catch (IndexOutOfBoundsException ex) {
            // pass
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 1.04 Insert layer - invalid index");
        try {
            iLayerManager.insert(sprite, 0);
            assertEquals(1, iLayerManager.getSize());
            // sprite should not be removed if insert fails
            iLayerManager.insert(sprite, -1);
            fail("IndexOutOfBoundsException expected");
        }
        catch (IndexOutOfBoundsException ex) {
            // pass
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 1.04 Insert layer - layer not removed");
        assertEquals(1, iLayerManager.getSize());

        // checking("Test 1.05 Insert layer - invalid index");
        try {
            iLayerManager.insert(sprite, 0);
            assertEquals(1, iLayerManager.getSize());
            // sprite should not be removed if insert fails
            iLayerManager.insert(sprite, 2);
            fail("IndexOutOfBoundsException expected");
        }
        catch (IndexOutOfBoundsException ex) {
            // pass
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }
        // checking("Test 1.05 Insert layer - layer not removed");
        assertEquals(1, iLayerManager.getSize());

        // checking("Test 1.06 Insert layer / getSize composite test");
        try {
            iLayerManager.insert(sprite, 0);
            // verify size
            assertEquals(1, iLayerManager.getSize());
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 1.07 Remove layer / getSize composite test");
        try {
            iLayerManager.remove(sprite);
            // verify size
            assertEquals(0, iLayerManager.getSize());
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 1.08 Append / getSize");
        try {
            iLayerManager.append(sprite);
            // verify size
            assertEquals(1, iLayerManager.getSize());
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 1.09 Insert same layer / remove composite test");
        try {
            iLayerManager.insert(sprite, 0);
            // verify size
            assertEquals(1, iLayerManager.getSize());
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 1.10 getLayerAt test - invalid index");
        try {
            iLayerManager.getLayerAt(-1);
            fail("IndexOutOfBoundsException expected");
        }
        catch (IndexOutOfBoundsException ex) {
            // pass
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 1.11 getLayerAt test ");
        try {
            iLayerManager.append(anotherSprite);
            Layer layer = iLayerManager.getLayerAt(1);
            assertTrue(anotherSprite.equals(layer));
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // Additional MSA 0.80 checks (from section 6.2.3.3)
        // checking("Test 1.12 Insert layer - layer index too large");
        Image img3 = Image.createImage(iImgData40x30, 0, iImgData40x30.length);
        Image img4 = Image.createImage(iImgData120x10, 0, iImgData120x10.length);
        Image img5 = Image.createImage(iImgData120x10, 0, iImgData120x10.length);
        Sprite yetAnotherSprite = new Sprite(img3);
        Sprite andAnotherSprite = new Sprite(img4);
        Sprite theLastSprite = new Sprite(img5);

        // Add some more layers
        try {
            iLayerManager.append(yetAnotherSprite);
            iLayerManager.append(andAnotherSprite);
            assertEquals(4, iLayerManager.getSize());
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // Try to add a layer at one index too high
        try {
            iLayerManager.insert(theLastSprite, 5);
            fail("IndexOutOfBoundsException expected");
        }
        catch (IndexOutOfBoundsException ex) {
            // pass
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // Try to add it at the end
        try {
            iLayerManager.insert(theLastSprite, 4);
            assertEquals(5, iLayerManager.getSize());
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // Try to move anotherSprite past the end
        try {
            iLayerManager.insert(anotherSprite, 5);
            fail("IndexOutOfBoundsException expected");
        }
        catch (IndexOutOfBoundsException ex) {
            // pass
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // Try to move anotherSprite to the end
        try {
            iLayerManager.insert(anotherSprite, 4);
            assertEquals(5, iLayerManager.getSize());
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }
    }

	/**
     * Checks for implementation specific issues in
     * {@link javax.microedition.lcdui.game.LayerManager#setViewWindow}.
     */
    public void testViewWindow() {
        iLayerManager = new LayerManager();
        // checking("Test 2.01 Invoking with invalid parameter values - aWidth");
        try {
            iLayerManager.setViewWindow(10, 10, -1, 10);
            fail("IllegalArgumentException expected");
        }
        catch (IllegalArgumentException ex) {
            // pass
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 2.02 Invoking with invalid parameter values - aHeight");
        try {
            iLayerManager.setViewWindow(10, 10, 10, -1);
            fail("IllegalArgumentException expected");
        }
        catch (IllegalArgumentException ex) {
            // pass
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 2.03 View window");
        iLayerManager = new LayerManager();
        try {
            // Paint few one-color sprites, then analyse the pixels to test
            // Z-order.

            // This will be our background, mutable image for painting
            Image target = Image.createImage(50, 50);

            Graphics g = target.getGraphics();
            g.setColor(GREEN);
            g.fillRect(0, 0, 50, 50);

            // add the sprites
            iLayerManager.append(iSpriteWhite);
            iLayerManager.insert(iSpriteBlack, 0); // on top

            // Apply the view window
            iLayerManager.setViewWindow(-20, 0, 50, 50);

            // paint
            iLayerManager.paint(g, 0, 0);

            // check colours
            int[] rgbData = new int[400];
            target.getRGB(rgbData, 0, 20, 20, 0, 20, 20);

            // characteristic pixels are at 5,5 (white) and 15,15(black)
            checkRGB("5,5", rgbData[5 + 5 * 20], WHITE, g);
            checkRGB("15,15", rgbData[15 + 15 * 20], BLACK, g);
            // test for one out errors at 9,9 and 9,10, 10,9 (white) and
            // 10,10(black)
            checkRGB("9,9", rgbData[9 + 9 * 20], WHITE, g);
            checkRGB("9,10", rgbData[9 + 10 * 20], WHITE, g);
            checkRGB("10,9", rgbData[10 + 9 * 20], WHITE, g);
            checkRGB("10,10", rgbData[10 + 10 * 20], BLACK, g);
        }
        catch (Exception ex) {
            fail("Exception " + ex);
        }

        // checking("Test 2.04 Test clipping - are layers outside the view window painted");
        iLayerManager = new LayerManager();
        try {
            // This will be our background, mutable image for painting
            Image target = Image.createImage(50, 50);

            Graphics g = target.getGraphics();
            g.setColor(GREEN);
            g.fillRect(0, 0, 50, 50);

            // add the sprites,
            iLayerManager.append(iSpriteWhite);
            iLayerManager.insert(iSpriteBlack, 0); // on top

            // set the viewWindow
            iLayerManager.setViewWindow(5, 5, 10, 10);

            // paint
            iLayerManager.paint(g, 5, 5);

            // check colours
            int[] rgbData = new int[400];
            target.getRGB(rgbData, 0, 20, 0, 0, 20, 20);

            // print out the image fo verification
            // printRGB(rgbData, 20, 20, g);

            // check if anything is painted outside the clip
            checkRGB("4,4", rgbData[4 + 4 * 20], GREEN, g);
            checkRGB("4,5", rgbData[4 + 5 * 20], GREEN, g);
            checkRGB("5,4", rgbData[5 + 4 * 20], GREEN, g);
            checkRGB("15,14", rgbData[15 + 14 * 20], GREEN, g);
            checkRGB("14,15", rgbData[14 + 15 * 20], GREEN, g);
            checkRGB("15,15", rgbData[15 + 15 * 20], GREEN, g);
            // this is the top-left corner of the painted area, should be white
            checkRGB("5,5", rgbData[5 + 5 * 20], WHITE, g);
            // this is the bottom-right corner of the painted area, should be
            // black
            checkRGB("14,14", rgbData[14 + 14 * 20], WHITE, g);
        }
        catch (Exception ex) {
            fail("Exception " + ex);
        }

        // checking("Test 2.05 Preservation of Graphics translation during paint");
        iLayerManager = new LayerManager();
        try {
            // Paint few one-color sprites, then analyse the pixels to test
            // Z-order.

            // This will be our background, mutable image for painting
            Image target = Image.createImage(50, 50);

            Graphics g = target.getGraphics();
            g.setColor(GREEN);
            g.fillRect(0, 0, 50, 50);

            // add the sprites,
            iLayerManager.append(iSpriteWhite);
            iLayerManager.insert(iSpriteBlack, 0); // on top

            // translate
            g.translate(-20, 10);

            // set the viewWindow
            iLayerManager.setViewWindow(5, 5, 10, 10);

            // paint
            iLayerManager.paint(g, 20, 20);

            assertEquals(-20, g.getTranslateX());
            assertEquals(10, g.getTranslateY());
        }
        catch (Exception ex) {
            fail("Exception " + ex);
        }

        // checking("Test 2.06 Preservation of Graphics clipping during paint");
        iLayerManager = new LayerManager();
        try {
            // This will be our background, mutable image for painting
            Image target = Image.createImage(50, 50);

            Graphics g = target.getGraphics();
            g.setColor(GREEN);
            g.fillRect(0, 0, 50, 50);

            // add the sprites,
            iLayerManager.append(iSpriteWhite);
            iLayerManager.insert(iSpriteBlack, 0); // on top

            // set clip
            g.setClip(25, 26, 20, 21);

            // set the viewWindow
            iLayerManager.setViewWindow(5, 5, 10, 10);

            // paint
            iLayerManager.paint(g, 20, 20);

            assertEquals(20, g.getClipWidth());
            assertEquals(21, g.getClipHeight());
            assertEquals(25, g.getClipX());
            assertEquals(26, g.getClipY());
        }
        catch (Exception ex) {
            fail("Exception " + ex);
        }

        // checking("Test 2.07 Preservation of Graphics translation and clipping during paint");
        iLayerManager = new LayerManager();
        try {
            // This will be our background, mutable image for painting
            Image target = Image.createImage(50, 50);

            Graphics g = target.getGraphics();
            g.setColor(GREEN);
            g.fillRect(0, 0, 50, 50);

            // add the sprites,
            iLayerManager.append(iSpriteWhite);
            iLayerManager.insert(iSpriteBlack, 0); // on top

            iLayerManager.setViewWindow(5, 5, 10, 10);

            // set clip
            g.setClip(25, 26, 20, 21);

            // set translation
            g.translate(-20, 10); // clip now 45,16,20,21

            // paint
            iLayerManager.paint(g, 50, 30);

            assertEquals(-20, g.getTranslateX());
            assertEquals(10, g.getTranslateY());
        }
        catch (Exception ex) {
            fail("Exception " + ex);
        }
    }

    /**
     * Checks for implementation specific issues in
     * {@link javax.microedition.lcdui.game.LayerManager#paint}.
     */
    public void testPaint() {
        // make sure we use clean instance of LayerManager
        iLayerManager = new LayerManager();

        // checking("Test 3.01 Testing invocation with invalid parameter values - g==null");
        try {
            iLayerManager.paint(null, 0, 10);
            fail("NullPointerException expected");
        }
        catch (NullPointerException ex) {
            // pass
        }
        catch (Exception ex) {
            fail("Wrong exception " + ex);
        }

        // checking("Test 3.02 Z order");
        iLayerManager = new LayerManager();
        try {
            // Paint few one-color sprites, then analyse the pixels to test
            // Z-order.

            // This will be our background, mutable image for painting
            Image target = Image.createImage(50, 50);

            Graphics g = target.getGraphics();
            g.setColor(GREEN);
            g.fillRect(0, 0, 50, 50);

            // add the sprites,
            iLayerManager.append(iSpriteWhite);
            iLayerManager.insert(iSpriteBlack, 0); // on top

            // paint
            iLayerManager.paint(g, 0, 0);

            // check colours
            int[] rgbData = new int[400];
            target.getRGB(rgbData, 0, 20, 0, 0, 20, 20);

            // characteristic pixels are at 5,5 (white) and 15,15(black)
            checkRGB("5,5", rgbData[5 + 5 * 20], WHITE, g);
            checkRGB("15,15", rgbData[15 + 15 * 20], BLACK, g);
            // test for one out errors at 9,9 and 9,10, 10,9 (white) and
            // 10,10(black)
            checkRGB("9,9", rgbData[9 + 9 * 20], WHITE, g);
            checkRGB("9,10", rgbData[9 + 10 * 20], WHITE, g);
            checkRGB("10,9", rgbData[10 + 9 * 20], WHITE, g);
            checkRGB("10,10", rgbData[10 + 10 * 20], BLACK, g);
        }
        catch (Exception ex) {
            ex.printStackTrace();
            fail("Exception " + ex);
        }

        // checking("Test 3.03 Graphics context translation");
        iLayerManager = new LayerManager();
        try {
            // Paint few one-color sprites, then analyse the pixels to test
            // Z-order.

            // This will be our background, mutable image for painting
            Image target = Image.createImage(50, 50);

            Graphics g = target.getGraphics();
            g.setColor(GREEN);
            g.fillRect(0, 0, 50, 50);

            // add the sprites,
            iLayerManager.append(iSpriteWhite);
            iLayerManager.insert(iSpriteBlack, 0); // on top

            // paint
            iLayerManager.paint(g, 20, 0);

            // check colours
            int[] rgbData = new int[400];
            target.getRGB(rgbData, 0, 20, 20, 0, 20, 20);

            // characteristic pixels are at 5,5 (white) and 15,15(black)
            checkRGB("5,5", rgbData[5 + 5 * 20], WHITE, g);
            checkRGB("15,15", rgbData[15 + 15 * 20], BLACK, g);
            // test for one out errors at 9,9 and 9,10, 10,9 (white) and
            // 10,10(black)
            checkRGB("9,9", rgbData[9 + 9 * 20], WHITE, g);
            checkRGB("9,10", rgbData[9 + 10 * 20], WHITE, g);
            checkRGB("10,9", rgbData[10 + 9 * 20], WHITE, g);
            checkRGB("10,10", rgbData[10 + 10 * 20], BLACK, g);
        }
        catch (Exception ex) {
            fail("Exception " + ex);
        }

        // checking("Test 3.04 Preservation of Graphics translation during paint");
        iLayerManager = new LayerManager();
        try {
            // Paint few one-color sprites, then analyse the pixels to test
            // Z-order.

            // This will be our background, mutable image for painting
            Image target = Image.createImage(50, 50);

            Graphics g = target.getGraphics();
            g.setColor(GREEN);
            g.fillRect(0, 0, 50, 50);

            // add the sprites,
            iLayerManager.append(iSpriteWhite);
            iLayerManager.insert(iSpriteBlack, 0); // on top

            // translate
            g.translate(-20, 10);

            // paint
            iLayerManager.paint(g, 20, 20);

            assertEquals(-20, g.getTranslateX());
            assertEquals(10, g.getTranslateY());
        }
        catch (Exception ex) {
            fail("Exception " + ex);
        }

        // checking("Test 3.05 Preservation of Graphics clipping during paint");
        iLayerManager = new LayerManager();
        try {
            // This will be our background, mutable image for painting
            Image target = Image.createImage(50, 50);

            Graphics g = target.getGraphics();
            g.setColor(GREEN);
            g.fillRect(0, 0, 50, 50);

            // add the sprites,
            iLayerManager.append(iSpriteWhite);
            iLayerManager.insert(iSpriteBlack, 0); // on top

            // set clip
            g.setClip(5, 6, 10, 11);

            // paint
            iLayerManager.paint(g, 20, 20);

            assertEquals(10, g.getClipWidth());
            assertEquals(11, g.getClipHeight());
            assertEquals(5, g.getClipX());
            assertEquals(6, g.getClipY());
        }
        catch (Exception ex) {
            fail("Exception " + ex);
        }

        // checking("Test 3.06 Test clipping - are layers outside the clip painted");
        iLayerManager = new LayerManager();
        try {
            // Paint few one-color sprites, then analyse the pixels to test
            // Z-order.

            // This will be our background, mutable image for painting
            Image target = Image.createImage(50, 50);

            Graphics g = target.getGraphics();
            g.setColor(GREEN);
            g.fillRect(0, 0, 50, 50);

            // add the sprites,
            iLayerManager.append(iSpriteWhite);
            iLayerManager.insert(iSpriteBlack, 0); // on top

            // set the clip
            g.setClip(5, 5, 10, 10);

            // paint
            iLayerManager.paint(g, 0, 0);

            // check colours
            int[] rgbData = new int[400];
            target.getRGB(rgbData, 0, 20, 0, 0, 20, 20);

            // print out the image fo verification
            // printRGB(rgbData,20,20,g);

            // check if anything is painted outside the clip
            checkRGB("4,4", rgbData[4 + 4 * 20], GREEN, g);
            checkRGB("4,5", rgbData[4 + 5 * 20], GREEN, g);
            checkRGB("5,4", rgbData[5 + 4 * 20], GREEN, g);
            checkRGB("15,14", rgbData[15 + 14 * 20], GREEN, g);
            checkRGB("14,15", rgbData[14 + 15 * 20], GREEN, g);
            checkRGB("15,15", rgbData[15 + 15 * 20], GREEN, g);
            // this is the top-left corner of the painted area, should be white
            checkRGB("5,5", rgbData[5 + 5 * 20], WHITE, g);
            // this is the bottom-right corner of the painted area, should be
            // black
            checkRGB("14,14", rgbData[14 + 14 * 20], BLACK, g);
        }
        catch (Exception ex) {
            fail("Exception " + ex);
        }

        // checking("Test 3.07 Test clipping - are layers outside the view window painted");
        iLayerManager = new LayerManager();
        try {
            // Paint few one-color sprites, then analyse the pixels to test
            // Z-order.
            // This will be our background, mutable image for painting
            Image target = Image.createImage(50, 50);

            Graphics g = target.getGraphics();
            g.setColor(GREEN);
            g.fillRect(0, 0, 50, 50);

            // add the sprites,
            iLayerManager.append(iSpriteWhite);
            iLayerManager.insert(iSpriteBlack, 0); // on top

            // set the view window
            iLayerManager.setViewWindow(5, 5, 10, 10);

            // paint
            iLayerManager.paint(g, 5, 5);

            // check colours
            int[] rgbData = new int[400];
            target.getRGB(rgbData, 0, 20, 0, 0, 20, 20);

            // print out the image for verification
            // printRGB(rgbData, 20, 20, g);

            // check if anything is painted outside the view window
            // int green = g.getDisplayColor(GREEN);
            checkRGB("4,4", rgbData[4 + 4 * 20], GREEN, g);
            checkRGB("4,5", rgbData[4 + 5 * 20], GREEN, g);
            checkRGB("5,4", rgbData[5 + 4 * 20], GREEN, g);
            checkRGB("15,14", rgbData[15 + 14 * 20], GREEN, g);
            checkRGB("14,15", rgbData[14 + 15 * 20], GREEN, g);
            checkRGB("15,15", rgbData[15 + 15 * 20], GREEN, g);
            // this is the top-left corner of the painted area, should be white
            checkRGB("5,5", rgbData[5 + 5 * 20], WHITE, g);
            // this is the bottom-right corner of the painted area, should be
            // black
            // checkRGB("14,14", rgbData[14 + 14 * 20], BLACK, g);
        }
        catch (Exception ex) {
            fail("Exception " + ex);
        }

        // checking("Test 3.08 Test when Graphics object already translated");
        iLayerManager = new LayerManager();

        // Paint few one-color sprites, then analyse the pixels to test
        // Z-order.

        // This will be our background, mutable image for painting
        Image target = Image.createImage(50, 50);

        Graphics g = target.getGraphics();
        g.setColor(GREEN);
        g.fillRect(0, 0, 50, 50);

        // add the sprites,
        iLayerManager.append(iSpriteWhite);
        iLayerManager.insert(iSpriteBlack, 0); // on top

        // translate graphics
        g.translate(-5, -5);

        // paint
        iLayerManager.paint(g, 0, 0);

        // check colours
        int[] rgbData = new int[400];
        target.getRGB(rgbData, 0, 20, 0, 0, 20, 20);

        // print out the image fo verification
        // printRGB(rgbData, 20, 20, g);

        // check if all went fine
        checkRGB("4,4", rgbData[4 + 4 * 20], WHITE, g);
        checkRGB("4,5", rgbData[4 + 5 * 20], WHITE, g);
        checkRGB("5,4", rgbData[5 + 4 * 20], WHITE, g);
        // this is the top-left corner of the painted area, should be white
        checkRGB("5,5", rgbData[5 + 5 * 20], BLACK, g);
    }

	private void checkRGB(String aMsg, int aActualColor, int aExpectedColor,
            Graphics aGraphics) {
        int expected = aGraphics.getDisplayColor(aExpectedColor);
        int actual = aActualColor & 0x00FFFFFF;
        if (actual != expected) {
            assertEquals(aMsg, Integer.toHexString(expected),
                    Integer.toHexString(actual));
        }
    }

	/**
     * Prints out the image to standard output. If an image contains unexpected
     * colours, i.e. not those used to form images, this method will throw an
     * exception. This behaviour provides protection against hiding colour
     * discretization problem and reporting test error.
     */
    private void printRGB(int[] data, int w, int h, Graphics g) {
        final int white = g.getDisplayColor(WHITE);
        final int black = g.getDisplayColor(BLACK);
        final int green = g.getDisplayColor(GREEN);

        int odd = 0;
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                int ind = i * w + j;
                String cha = " ";
                int actual = data[ind] & 0x00FFFFFF;
                if (actual == white) {
                    cha = "W";
                }
                else if (actual == black) {
                    cha = "B";
                }
                else if (actual == green) {
                    cha = ".";
                }
                else {
                    cha = "U";
                    odd = data[ind];
                }
                System.out.print(cha);
            }
            System.out.println("");
        }
        System.out.println("");
        if (odd != 0) {
            fail("Unexpected color in test image : 0x" + Integer.toHexString(odd));
        }
    }

}
