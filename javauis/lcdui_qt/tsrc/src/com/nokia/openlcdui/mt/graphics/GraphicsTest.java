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
package com.nokia.openlcdui.mt.graphics;

import junit.framework.*;

import javax.microedition.lcdui.*;
import javax.microedition.lcdui.game.Sprite;

import com.nokia.openlcdui.mt.SWTTestCase;

public class GraphicsTest extends SWTTestCase {

    private TestCanvas testCanvas;

    /**
     * Constructor.
     */
    public GraphicsTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public GraphicsTest(String sTestName) {
        super(sTestName);
    }

    protected void setUp() throws Exception {
        super.setUp();
        testCanvas = new TestCanvas();
        Display.getDisplay(getMIDlet()).setCurrent(testCanvas);
    }

    protected void tearDown() throws Exception {
        super.tearDown();
        testCanvas = null;
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
	    methodNames = GraphicsTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new GraphicsTest((String)e.nextElement()));
	    }

		return suite;
	}

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testGraphics");
        methodNames.addElement("testTranslate");
        methodNames.addElement("testColor");
        methodNames.addElement("testClip");
        methodNames.addElement("testDrawLine");
        methodNames.addElement("testFillRect");
        methodNames.addElement("testDrawRect");
        methodNames.addElement("testDrawRoundRect");
        methodNames.addElement("testFillRoundRect");
        methodNames.addElement("testFillArc");
        methodNames.addElement("testDrawArc");
        methodNames.addElement("testDrawString");
        methodNames.addElement("testDrawImage");
        methodNames.addElement("testStrokeStyle");
        methodNames.addElement("testDrawRGB");
        methodNames.addElement("testFillTriangle");
        methodNames.addElement("testCopyArea");
        methodNames.addElement("testDrawRegion");
        return methodNames;
    }
    
    protected void runTest() throws Throwable {
        if (getName().equals("testGraphics")) testCanvas.test(TestCanvas.TEST_GRAPHICS);
        else if (getName().equals("testTranslate")) testCanvas.test(TestCanvas.TEST_TRANSLATE);
        else if (getName().equals("testColor")) testCanvas.test(TestCanvas.TEST_COLOR);
        else if (getName().equals("testClip")) testCanvas.test(TestCanvas.TEST_CLIP);
        else if (getName().equals("testDrawLine")) testCanvas.test(TestCanvas.TEST_DRAWLINE);
        else if (getName().equals("testFillRect")) testCanvas.test(TestCanvas.TEST_FILLRECT);
        else if (getName().equals("testDrawRect")) testCanvas.test(TestCanvas.TEST_DRAWRECT);
        else if (getName().equals("testDrawRoundRect")) testCanvas.test(TestCanvas.TEST_DRAWROUNDRECT);
        else if (getName().equals("testFillRoundRect")) testCanvas.test(TestCanvas.TEST_FILLROUNDRECT);
        else if (getName().equals("testFillArc")) testCanvas.test(TestCanvas.TEST_FILLARC);
        else if (getName().equals("testDrawArc")) testCanvas.test(TestCanvas.TEST_DRAWARC);
        else if (getName().equals("testDrawString")) testCanvas.test(TestCanvas.TEST_DRAWSTRING);
        else if (getName().equals("testDrawImage")) testCanvas.test(TestCanvas.TEST_DRAWIMAGE);
        else if (getName().equals("testStrokeStyle")) testCanvas.test(TestCanvas.TEST_STROKESTYLE);
        else if (getName().equals("testDrawRGB")) {DISABLE_TEST();testCanvas.test(TestCanvas.TEST_DRAWRGB);}
        else if (getName().equals("testFillTriangle")) testCanvas.test(TestCanvas.TEST_FILLTRIANGLE);
        else if (getName().equals("testCopyArea")) testCanvas.test(TestCanvas.TEST_COPYAREA);
        else if (getName().equals("testDrawRegion")) testCanvas.test(TestCanvas.TEST_DRAWREGION);
        else super.runTest();
    }

    // /**
    // * Test method for {@link javax.microedition.lcdui.Graphics#getFont()}.
    // */
    // public final void testGetFont() {
    // fail("Not yet implemented"); // TODO
    // }
    //
    // /**
    // * Test method for {@link
    // *
    // javax.microedition.lcdui.Graphics#setFont(org.eclipse.swt.graphics.Font)}.
    // */
    // public final void testSetFont() {
    // fail("Not yet implemented"); // TODO
    // }
    //

    class TestCanvas extends Canvas {

        static final int TEST_NONE = 0;
        static final int TEST_GRAPHICS = 1;
        static final int TEST_COLOR = 2;
        static final int TEST_DRAWLINE = 3;
        static final int TEST_FILLRECT = 4;
        static final int TEST_CLIP = 5;
        static final int TEST_COPYAREA = 6;
        static final int TEST_DRAWROUNDRECT = 7;
        static final int TEST_DRAWRECT = 8;
        static final int TEST_STROKESTYLE = 9;
        static final int TEST_TRANSLATE = 10;
        static final int TEST_FILLROUNDRECT = 11;
        static final int TEST_FILLARC = 12;
        static final int TEST_DRAWARC = 13;
        static final int TEST_FILLTRIANGLE = 14;
        static final int TEST_DRAWSTRING = 15;
        static final int TEST_DRAWIMAGE = 16;
        static final int TEST_DRAWRGB = 17;
        static final int TEST_DRAWREGION = 18;

        private int testMode = TEST_NONE;

        private boolean passed;

        private String failMsg;

        public TestCanvas() {
        };

        private void setFailMessage(String msg) {
            failMsg = msg;
            passed = (failMsg == null);
        }

        public void test(int mode) {
            setFailMessage(null);
            testMode = mode;
            testCanvas.repaint();
            testCanvas.serviceRepaints();
            assertTrue(failMsg, passed);
        }

        protected void paint(Graphics g) {
            try {
                switch (testMode) {
                    case TEST_NONE:
                        break;

                    case TEST_GRAPHICS:
                        if (g == null) {
                            fail("Graphics object in Canvas.paint() is null");
                        }
                        break;

                    case TEST_COLOR:
                        testColor(g);
                        break;

                    case TEST_DRAWLINE:
                        testDrawLine(g);
                        break;

                    case TEST_FILLRECT:
                        testFillRect(g);
                        break;

                    case TEST_CLIP:
                        testClip(g);
                        break;

                    case TEST_COPYAREA:
                        testCopyArea(g);
                        break;

                    case TEST_DRAWROUNDRECT:
                        testDrawRoundRect(g);
                        break;

                    case TEST_DRAWRECT:
                        testDrawRect(g);
                        break;

                    case TEST_STROKESTYLE:
                        testStrokeStyle(g);
                        break;

                    case TEST_TRANSLATE:
                        testTranslate(g);
                        break;

                    case TEST_FILLROUNDRECT:
                        testFillRoundRect(g);
                        break;

                    case TEST_FILLARC:
                        testFillArc(g);
                        break;

                    case TEST_DRAWARC:
                        testDrawArc(g);
                        break;

                    case TEST_FILLTRIANGLE:
                        testFillTriangle(g);
                        break;

                    case TEST_DRAWSTRING:
                        testDrawString(g);
                        break;

                    case TEST_DRAWIMAGE:
                        testDrawImage(g);
                        break;

                    case TEST_DRAWRGB:
                        testDrawRGB(g);
                        break;
                    case TEST_DRAWREGION:
                        testDrawRegion(g);
                        break;
                    default:
                }
            }
            /*catch (AssertionFailedException afe) {
                setFailMessage(afe.getMessage());
            }*/
            catch (Exception e) {
                setFailMessage("Unhandled exception" + e.getMessage());
            }
            testMode = TEST_NONE;
        }

        private void testDrawImage(Graphics g) {
            String imgFile = "image1.jpg";
            Image testImage = null;

            try {
                testImage = Image.createImage(imgFile);
            }
            catch (Exception e) {
                fail("Unable to load " + imgFile + " image for test");
            }

            g.drawImage(testImage, 0, 0, 0);
            g.drawImage(testImage, Integer.MAX_VALUE, Integer.MIN_VALUE, 0);
            g.drawImage(testImage, 50, 50, Graphics.HCENTER | Graphics.VCENTER);

            try {
                g.drawImage(null, 0, 0, 0);
                fail("NullPointerException not thrown at Graphics.drawImage");
            }
            catch (NullPointerException e) {
                // ok
            }

            // Test invalid anchors
            try {
                g.drawImage(testImage, 50, 50,
                        Graphics.HCENTER | Graphics.LEFT);
                g.drawImage(testImage, 50, 50, Graphics.VCENTER | Graphics.LEFT
                        | Graphics.TOP);

                g.drawImage(testImage, 50, 50, Graphics.RIGHT | Graphics.LEFT);

                fail("IllegalArgumentException not thrown at Graphics.drawImage");
            }
            catch (IllegalArgumentException e) {
                // ok
            }
        }

        private void testDrawString(Graphics g) {
            // Tests for Graphics.drawString
            g.drawString("TestString", 10, 10, Graphics.TOP | Graphics.LEFT);

            try {
                g.drawString(null, 0, 0, Graphics.TOP | Graphics.LEFT);
                fail("NullPointerException not thrown "
                        + "as expected in Graphics.drawString()");
            }
            catch (NullPointerException e) {
                // Ok
            }

            try {
                g.drawString("TestString", 10, 0, Graphics.TOP
                        | Graphics.BOTTOM);
                fail("IllegalArgumentException not thrown "
                        + "as expected in Graphics.drawString()");
            }
            catch (IllegalArgumentException e) {
                // Ok
            }

            // Tests for Graphics.drawSubString

            g.drawSubstring("TestString", 3, 5, 10, 10, Graphics.TOP
                    | Graphics.LEFT);

            try {
                g.drawSubstring(null, 2, 4, 10, 10, Graphics.TOP
                        | Graphics.LEFT);
                fail("NullPointerException not thrown "
                        + "as expected in Graphics.drawSubString()");
            }
            catch (NullPointerException e) {
                // Ok
            }

            try {
                g.drawSubstring("TestString", 0, 4, 10, 0, Graphics.LEFT
                        | Graphics.RIGHT);
                fail("IllegalArgumentException not "
                        + "thrown as expected in Graphics.drawSubString()");
            }
            catch (IllegalArgumentException e) {
                // Ok
            }

            try {
                g.drawSubstring("TestString", 5, 20, 0, 0, Graphics.LEFT
                        | Graphics.RIGHT);
                fail("StringIndexOutOfBoundsException "
                        + "not thrown as expected in "
                        + "Graphics.drawSubString()");
            }
            catch (StringIndexOutOfBoundsException e) {
                // Ok
            }

            // Tests for Graphics.drawChar

            // g.drawChar('W', 10, 10, Graphics.TOP | Graphics.LEFT);
            // g.drawChar('\0', 10, 10, Graphics.TOP | Graphics.LEFT);
            // g.drawChar('\n', 10, 10, Graphics.TOP | Graphics.LEFT);
            // g.drawChar('\t', 10, 10, Graphics.TOP | Graphics.LEFT);
            // g.drawChar('\u00FE', 10, 30, Graphics.TOP | Graphics.LEFT);

            try {
                g.drawChar('X', 10, 0, Graphics.TOP | Graphics.BASELINE
                        | Graphics.LEFT | Graphics.HCENTER);
                fail("IllegalArgumentException not "
                        + "thrown as expected in Graphics.drawChar()");
            }
            catch (IllegalArgumentException e) {
                // Ok
            }

            // Tests for Graphics.drawChars

            char[] testChars = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r',
                    'l', 'd',};

            g.drawChars(testChars, 0, 5, 10, 10, Graphics.TOP | Graphics.LEFT);

            try {
                g.drawChars(testChars, 5, 3, 10, 0, Graphics.TOP
                        | Graphics.BASELINE | Graphics.LEFT | Graphics.HCENTER);
                fail("IllegalArgumentException not "
                        + "thrown as expected in Graphics.drawChars()");
            }
            catch (IllegalArgumentException e) {
                // Ok
            }

            try {
                g.drawChars(null, 1, 3, 10, 0, Graphics.TOP | Graphics.LEFT);
                fail("NullPointerException "
                        + "not thrown as expected in Graphics.drawChars()");
            }
            catch (NullPointerException e) {
                // Ok
            }

            try {
                g.drawChars(testChars, 5, 20, 10, 0, Graphics.TOP
                        | Graphics.LEFT);
                fail("ArrayIndexOutOfBoundsException not"
                        + " thrown as expected in Graphics.drawChars()");
            }
            catch (ArrayIndexOutOfBoundsException e) {
                // Ok
            }
        }

        private void testFillTriangle(Graphics g) {
            g.fillTriangle(0, 0, 0, 0, 0, 0);
            g.fillTriangle(0, 0, -100, 20, 50, -35);
            g.fillTriangle(Integer.MIN_VALUE, Integer.MIN_VALUE,
                    Integer.MIN_VALUE, Integer.MAX_VALUE, Integer.MAX_VALUE,
                    Integer.MAX_VALUE);
        }

        private void testFillArc(Graphics g) {
            g.fillArc(10, 10, 40, 20, 20, 300);
            g.fillArc(10, 10, -40, -20, 20, 300);
            g.fillArc(10, 10, 40, 20, 20, -300);
            g.fillArc(10, 10, 40, 20, -520, -300);
        }

        private void testDrawArc(Graphics g) {
            g.drawArc(10, 10, 40, 20, 20, 300);
            g.drawArc(10, 10, -40, -20, 20, 300);
            g.drawArc(10, 10, 40, 20, 20, -300);
            g.drawArc(10, 10, 40, 20, -520, -300);
        }

        private void testTranslate(Graphics g) {
            int translateX = 10;
            int translateY = 12;

            g.translate(translateX, translateY);

            int returnX = g.getTranslateX();
            if (returnX != translateX) {
                fail("Graphics.translate() and "
                        + "Graphics.getTranslateX() don't match - expected:"
                        + translateX + " , received:" + returnX);
            }

            if (g.getTranslateY() != translateY) {
                fail("Graphics.translate() and "
                        + "Graphics.getTranslateY() don't match" + translateY
                        + " , received:" + g.getTranslateY());
            }
        }

        private void testStrokeStyle(Graphics g) {
            int strokeStyle1 = Graphics.DOTTED;
            int strokeStyle2 = Graphics.SOLID;

            int strokeStyleInvalid = -1;

            g.setStrokeStyle(strokeStyle1);
            if (g.getStrokeStyle() != strokeStyle1) {
                fail("Graphics.setStrokeStyle() "
                        + "and Graphics.getStrokeStyle() don't match");
            }

            g.setStrokeStyle(strokeStyle2);
            if (g.getStrokeStyle() != strokeStyle2) {
                fail("Graphics.setStrokeStyle() "
                        + "and Graphics.getStrokeStyle() don't match");
            }

            try {
                g.setStrokeStyle(strokeStyleInvalid);
                fail("Graphics.setStrokeStyle(): "
                        + "no exception thrown on invalid stroke style");
            }
            catch (IllegalArgumentException e) {
                // Ok
            }
        }

        private void testDrawRect(Graphics g) {
            g.drawRect(0, 0, 120, 200);
            g.drawRect(-10, -20, -120, -200);
            g.drawRect(-10, -20, 120, 200);
        }

        private void testDrawRoundRect(Graphics g) {
            g.drawRoundRect(0, 0, 120, 200, 10, 5);
            g.drawRoundRect(-10, -20, -120, -200, -10, -5);
            g.drawRoundRect(-10, -20, 120, 200, 0, 0);
        }

        private void testCopyArea(Graphics g) {
            final int fromX = 10;
            final int fromY = 15;
            final int width = 22;
            final int height = 33;
            final int destX = 55;
            final int destY = 66;

            // Test IllegalStateException when the destination
            // of copyArea is the display device.
            try {
                g.copyArea(fromX, fromY, width, height, destX, destY,
                        Graphics.TOP | Graphics.LEFT);

                fail("IllegalStateException not thrown as expected in "
                        + "testCopyArea");
            }
            catch (IllegalStateException e) {
                // Ok
            }

            final int imgW = 100;
            final int imgH = 200;

            Image testImg = Image.createImage(imgW, imgH);
            Graphics testGraphics = testImg.getGraphics();

            // Test IllegalArgumentException when the region
            // exceeds the size of the image.
            try {
                testGraphics.copyArea(fromX, fromY, imgW + 1, height, destX,
                        destY, Graphics.TOP | Graphics.LEFT);

                fail("IllegalArgumentException not thrown as "
                        + "expected in testCopyArea");
            }
            catch (IllegalArgumentException e) {
                // Ok
            }

            testGraphics.copyArea(fromX, fromY, width, height, destX, destY,
                    Graphics.TOP | Graphics.LEFT);
        }

        private void testClip(Graphics g) {
            final int clipX = 10;
            final int clipY = 20;
            final int clipW = 40;
            final int clipH = 50;

            g.setClip(clipX, clipY, clipW, clipH);

            if (g.getClipX() != clipX) {
                fail("Graphics.setClip() "
                        + "and Graphics.getClipX() don't match");
            }

            if (g.getClipY() != clipY) {
                fail("Graphics.setClip() "
                        + "and Graphics.getClipY() don't match");
            }

            if (g.getClipWidth() != clipW) {
                fail("Graphics.setClip() "
                        + "and Graphics.getClipWidth() don't match");
            }

            if (g.getClipHeight() != clipH) {
                fail("Graphics.setClip() "
                        + "and Graphics.getClipHeight() don't match");
            }

            final int newClipX = 15;
            final int newClipY = 25;
            final int newClipW = 60;
            final int newClipH = 70;

            g.clipRect(newClipX, newClipY, newClipW, newClipH);

            if (g.getClipX() != newClipX) {
                fail("Graphics.clipRect() "
                        + "and Graphics.getClipX() don't match");
            }

            if (g.getClipY() != newClipY) {
                fail("Graphics.clipRect() "
                        + "and Graphics.getClipY() don't match");
            }

            if (g.getClipWidth() != clipW - (newClipX - clipX)) {
                fail("Graphics.clipRect() "
                        + "and Graphics.getClipWidth() don't match");
            }

            if (g.getClipHeight() != clipH - (newClipY - clipY)) {
                fail("Graphics.clipRect() "
                        + "and Graphics.getClipHeight() don't match");
            }
        }

        private void testDrawLine(Graphics g) {
            g.drawLine(0, 0, 10, 10);
            g.setColor(255, 0, 0);
            g.drawLine(Integer.MIN_VALUE, Integer.MIN_VALUE, Integer.MAX_VALUE,
                    Integer.MAX_VALUE);
        }

        private void testFillRect(Graphics g) {
            g.fillRect(0, 0, 10, 10);
            g.fillRect(0, 0, -10, -10);
            g.setColor(255, 0, 0);
            g.fillRect(Integer.MIN_VALUE, Integer.MIN_VALUE,
                    Integer.MAX_VALUE, Integer.MAX_VALUE);
        }

        private void testFillRoundRect(Graphics g) {
            g.fillRoundRect(0, 0, 10, 10, 3, 3);
            g.fillRoundRect(0, 0, -10, -10, -5, -3);
            g.setColor(255, 0, 0);
            g.fillRoundRect(Integer.MIN_VALUE, Integer.MIN_VALUE,
                    Integer.MAX_VALUE, Integer.MAX_VALUE,
                    Integer.MAX_VALUE, Integer.MAX_VALUE);
        }

        private void testColor(Graphics g) {
            boolean passed1 = false, passed2 = false, passed3 = false;

            // test InvalidArgumentException
            try {
                g.setColor(256, 255, 255);
            }
            catch (Exception e) {
                if (e instanceof IllegalArgumentException) {
                    passed1 = true;
                }
            }

            try {
                g.setColor(0, 256, 255);
            }
            catch (Exception e) {
                if (e instanceof IllegalArgumentException) {
                    passed2 = true;
                }
            }

            try {
                g.setColor(0, 255, 256);
            }
            catch (Exception e) {
                if (e instanceof IllegalArgumentException) {
                    passed3 = true;
                }
            }

            if (!(passed1 & passed2 & passed3)) {
                fail("IllegalArgumentException " + "not thrown for invalid "
                        + "input in Graphics.setColor()");
            }

            int red = 45;
            int green = 139;
            int blue = 200;

            g.setColor(red, green, blue);

            if (g.getRedComponent() != red) {
                fail("Graphics.setColor(int, int, int) and"
                        + " Graphics.getRedComponent() do not match");
            }

            if (g.getGreenComponent() != green) {
                fail("Graphics.setColor(int, int, int) and"
                        + " Graphics.getGreenComponent() do not match");
            }

            if (g.getBlueComponent() != blue) {
                fail("Graphics.setColor(int, int, int) and"
                        + "Graphics.getBlueComponent() do not match");
            }

            int color = red << 16 | green << 8 | blue; // 0x00RRGGBB

            g.setColor(0xAB << 24 | color); // 0x(dirt)RRGGBB
            int testColor = g.getColor();

            if (testColor != color) {
                fail("Graphics.setColor(int) "
                        + "and Graphics.getColor() do not match");
            }

            if (g.getRedComponent() != red) {
                fail("Graphics.setColor(int)"
                        + "and Graphics.getRedComponent() do not match");
            }

            if (g.getGreenComponent() != green) {
                fail("Graphics.setColor(int) "
                        + "and Graphics.getGreenComponent()" + " do not match");
            }

            if (g.getBlueComponent() != blue) {
                fail("Graphics.setColor(int) "
                        + "and Graphics.getBlueComponent() do not match");
            }

            int gray = 152;
            g.setGrayScale(gray);

            if (g.getGrayScale() != gray) {
                fail("Graphics.setGrayScale(int) "
                        + "and Graphics.getGrayScale() do not match");
            }
        }

        private void testDrawRGB(Graphics g) {
        	
            // Test NullPointerException
            try {
                g.drawRGB(null, 0, 1, 0, 0, 1, 1, true);
                fail("NullPointerException " + "not thrown in testDrawRGB");
            }
            catch (NullPointerException e) {
            }

            // Test ArrayIndexOutOfBoundsException
            int[] rgb = {0x00000000, 0x11111111, 0x22222222, 0x33333333,
                         0x44444444, 0x55555555, 0x66666666, 0x77777777};
            try {
                g.drawRGB(rgb, 0, 4, 0, 0, 4, 4, true);
                fail("ArrayIndexOutOfBoundsException "
                        + "not thrown in testDrawRGB. 1");
            }
            catch (ArrayIndexOutOfBoundsException e) {
                // Ok
            }

            try {
                g.drawRGB(rgb, 0, 16, 0, 0, 4, 2, true);
                fail("ArrayIndexOutOfBoundsException "
                        + "not thrown in testDrawRGB. 2");
            }
            catch (ArrayIndexOutOfBoundsException e) {
                // Ok
            }
        }

        private void testDrawRegion(Graphics g) {
            // Test NullPointerException
            try {
                g.drawRegion(null, 0, 0, 1, 1, 0, 0, 0, 0);
                fail("NullPointerException " + "not thrown in testDrawRegion");
            }
            catch (NullPointerException e) {
                // Ok
            }

            // Test IllegalArgumentException for the same
            // source and destination

            Image srcImage = Image.createImage(30, 30);
            Graphics srcImageGraphics = srcImage.getGraphics();
            try {
                srcImageGraphics.drawRegion(srcImage, 0, 0, 1, 1,
                        Sprite.TRANS_NONE, 0, 0, 0);

                fail("IllegalArgumentException for the same source "
                        + "and destination not thrown in testDrawRegion");
            }
            catch (IllegalArgumentException e) {
                // Ok
            }

            // Test IllegalArgumentException for illegal transform
            try {
                g.drawRegion(srcImage, 0, 0, 1, 1,
                // invalid transform
                        Sprite.TRANS_NONE + 100, 0, 0, 0);

                fail("IllegalArgumentException not thrown for invalid "
                        + "transform in testDrawRegion");
            }
            catch (IllegalArgumentException e) {
                // Ok
            }

            // Test IllegalArgumentException for illegal anchor
            try {
                g.drawRegion(srcImage, 0, 0, 1, 1,
                        // invalid transform
                                Sprite.TRANS_NONE, 0, 0, Graphics.LEFT
                                        | Graphics.RIGHT);

                fail("IllegalArgumentException not thrown for invalid "
                        + "anchor in testDrawRegion");
            }
            catch (IllegalArgumentException e) {
                // Ok
            }

            // Test IllegalArgumentException for illegal region
            try {
                g.drawRegion(srcImage, -1, 0, 1, 1,
                // invalid transform
                        Sprite.TRANS_NONE, 0, 0, Graphics.LEFT);

                fail("IllegalArgumentException not thrown for invalid "
                        + "region in testDrawRegion");
            }
            catch (IllegalArgumentException e) {
                // Ok
            }

            // Test valid inputs
            g.drawRegion(srcImage, 10, 10, 5, 5, Sprite.TRANS_MIRROR_ROT270, 1,
                    1, Graphics.TOP | Graphics.LEFT);
        }

    }

}
