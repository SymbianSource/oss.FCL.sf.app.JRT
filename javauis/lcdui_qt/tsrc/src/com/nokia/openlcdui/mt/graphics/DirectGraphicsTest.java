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

import java.io.IOException;

import javax.microedition.lcdui.*;

import com.nokia.openlcdui.mt.SWTTestCase;

import com.nokia.mid.ui.DirectGraphics;
import com.nokia.mid.ui.DirectUtils;

public class DirectGraphicsTest extends SWTTestCase {

    private TestCanvas testCanvas;

    /**
     * Constructor.
     */
    public DirectGraphicsTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public DirectGraphicsTest(String sTestName) {
        super(sTestName);
    }

    protected void setUp() throws Exception {
        testCanvas = new TestCanvas();
        Display.getDisplay(getMIDlet()).setCurrent(testCanvas);
    }

    protected void tearDown() throws Exception {
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
	    methodNames = DirectGraphicsTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new DirectGraphicsTest((String)e.nextElement()));
	    }

		return suite;
	}

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testGraphics");
        methodNames.addElement("testDrawImage");
        methodNames.addElement("testDrawGetPixels");
        methodNames.addElement("testDrawGetPixelsByte");
        methodNames.addElement("testDrawGetPixelsShort");
        methodNames.addElement("testDrawGetPixelsShort555");
        methodNames.addElement("testDrawGetPixelsShort444");
        methodNames.addElement("testDrawGetPixelsShort4444");
        methodNames.addElement("testDrawPolygon");
        methodNames.addElement("testDrawTriangle");
        methodNames.addElement("testFillPolygon");
        methodNames.addElement("testFillTriangle");
        methodNames.addElement("testGetNativePixelFormat");
        methodNames.addElement("testSetGetARGBColor");
        return methodNames;
    }
    
    protected void runTest() throws Throwable {
        if (getName().equals("testGraphics")) testCanvas.runTest(TestCanvas.TEST_GRAPHICS);
        else if (getName().equals("testDrawImage")) testCanvas.runTest(TestCanvas.TEST_DRAWIMAGE);
        else if (getName().equals("testDrawGetPixels")) testCanvas.runDrawGetPixels();
        else if (getName().equals("testDrawGetPixelsByte")) testCanvas.runDrawGetPixelsByte();
        else if (getName().equals("testDrawGetPixelsShort")) testCanvas.runDrawGetPixelsShort();
        else if (getName().equals("testDrawGetPixelsShort555")) testCanvas.runDrawGetPixelsShort555();
        else if (getName().equals("testDrawGetPixelsShort444")) testCanvas.runDrawGetPixelsShort444();
        else if (getName().equals("testDrawGetPixelsShort4444")) testCanvas.runDrawGetPixelsShort4444();
        else if (getName().equals("testDrawPolygon")) testCanvas.runTest(TestCanvas.TEST_DRAWPOLYGON);
        else if (getName().equals("testDrawTriangle")) testCanvas.runTest(TestCanvas.TEST_DRAWTRIANGLE);
        else if (getName().equals("testFillPolygon")) testCanvas.runTest(TestCanvas.TEST_FILLPOLYGON);
        else if (getName().equals("testFillTriangle")) testCanvas.runTest(TestCanvas.TEST_FILLTRIANGLE);
        else if (getName().equals("testGetNativePixelFormat")) testCanvas.runTest(TestCanvas.TEST_GETNATIVEPIXELFORMAT);
        else if (getName().equals("testSetGetARGBColor")) testCanvas.runTest(TestCanvas.TEST_SETGETARGBCOLOR);
        else super.runTest();
    }
    
    class TestCanvas extends Canvas {

        static final int TEST_NONE = 0;
        static final int TEST_GRAPHICS = 1;
        static final int TEST_DRAWIMAGE = 2;
        static final int TEST_DRAWPIXELS = 3;
        static final int TEST_GETPIXELS = 4;
        static final int TEST_DRAWPOLYGON = 5;
        static final int TEST_DRAWTRIANGLE = 6;
        static final int TEST_FILLPOLYGON = 7;
        static final int TEST_FILLTRIANGLE = 8;
        static final int TEST_GETNATIVEPIXELFORMAT = 9;
        static final int TEST_SETGETARGBCOLOR = 10;
        static final int TEST_DRAWPIXELSBYTE = 11;
        static final int TEST_GETPIXELSBYTE = 12;
        static final int TEST_DRAWPIXELSSHORT = 13;
        static final int TEST_GETPIXELSSHORT = 14;
        static final int TEST_DRAWPIXELSSHORT555 = 15;
        static final int TEST_GETPIXELSSHORT555 = 16;
        static final int TEST_DRAWPIXELSSHORT444 = 17;
        static final int TEST_GETPIXELSSHORT444 = 18;
        static final int TEST_DRAWPIXELSSHORT4444 = 19;
        static final int TEST_GETPIXELSSHORT4444 = 20;

        private int testMode = TEST_NONE;
        private boolean passed;
        private String failMsg;

        private int testWidth = 50;
        private int testHeight = 50;
        private int[] rgbDataSet = new int[testWidth * testHeight];
        private int[] rgbDataGet = new int[testWidth * testHeight];
        private short[] sRgbDataSet = new short[testWidth * testHeight];
        private short[] sRgbDataSet4444 = new short[testWidth * testHeight];
        private short[] sRgbDataGet = new short[testWidth * testHeight];
        private byte[] bRgbDataSet = new byte[testWidth * testHeight];
        private byte[] bRgbDataGet = new byte[testWidth * testHeight];
        private byte[] bRgbDataMaskSet = new byte[testWidth * testHeight];
        private byte[] bRgbDataMaskGet = new byte[testWidth * testHeight];

        public TestCanvas() {
            for (int i = 0; i< rgbDataSet.length; i++) {
                rgbDataSet[i] = 0xFFFF0000;
                bRgbDataSet[i] = (byte) 0xFF;
                bRgbDataMaskSet[i] = (byte) 0xFF;
                sRgbDataSet[i] = (short) 0x0;
                sRgbDataSet4444[i] = (short) 0xFC00;
            }
            addCommand(new Command("back", "back", Command.BACK, 0));
        }

        /**
         * Set failing message.
         */
        private void setFailMessage(String msg) {
            failMsg = msg;
            passed = (failMsg == null);
        }

        /**
         * Run the specified test.
         */
        public void runTest(int test) {
            setFailMessage(null);
            doRunTest(test);
            assertTrue(failMsg, passed);
        }

        private void doRunTest(int test) {
            testMode = test;
            repaint();
            serviceRepaints();
        }

        public void runDrawGetPixels() {
            setFailMessage(null);
            doRunTest(TEST_DRAWPIXELS);
            doRunTest(TEST_GETPIXELS);
            // TODO: QT bug: first getPixels() call returns invalid values
            setFailMessage(null);
            doRunTest(TEST_GETPIXELS);
            assertTrue(failMsg, passed);
        }

        public void runDrawGetPixelsByte() {
            setFailMessage(null);
            doRunTest(TEST_DRAWPIXELSBYTE);
            doRunTest(TEST_GETPIXELSBYTE);

            //setFailMessage(null);
            //doRunTest(TEST_GETPIXELS);
            assertTrue(failMsg, passed);
        }

        public void runDrawGetPixelsShort() {
            setFailMessage(null);
            doRunTest(TEST_DRAWPIXELSSHORT);
            doRunTest(TEST_GETPIXELSSHORT);

            //setFailMessage(null);
            //doRunTest(TEST_GETPIXELS);
            assertTrue(failMsg, passed);
        }
        public void runDrawGetPixelsShort555() {
            setFailMessage(null);
            doRunTest(TEST_DRAWPIXELSSHORT555);
            doRunTest(TEST_GETPIXELSSHORT555);

            //setFailMessage(null);
            //doRunTest(TEST_GETPIXELS);
            assertTrue(failMsg, passed);
        }
        public void runDrawGetPixelsShort444() {
            setFailMessage(null);
            doRunTest(TEST_DRAWPIXELSSHORT444);
            doRunTest(TEST_GETPIXELSSHORT444);

            //setFailMessage(null);
            //doRunTest(TEST_GETPIXELS);
            assertTrue(failMsg, passed);
        }
        public void runDrawGetPixelsShort4444() {
            setFailMessage(null);
            doRunTest(TEST_DRAWPIXELSSHORT4444);
            doRunTest(TEST_GETPIXELSSHORT4444);

            //setFailMessage(null);
            //doRunTest(TEST_GETPIXELS);
            assertTrue(failMsg, passed);
        }


        protected void paint(Graphics g) {
            try {
                DirectGraphics dg = DirectUtils.getDirectGraphics(g);
                switch (testMode) {
                    case TEST_NONE:
                        break;
                    case TEST_GRAPHICS:
                        if (dg == null) {
                            fail("DirectGraphics object is null in paint()");
                        }
                        break;
                    case TEST_DRAWIMAGE:
                        testDrawImage(dg);
                        break;
                    case TEST_DRAWPIXELS:
                        testDrawPixels(dg);
                        break;
                    case TEST_DRAWPIXELSBYTE:
                        testDrawPixelsByte(dg);
                        break;
                    case TEST_GETPIXELS:
                        testGetPixels(dg);
                        break;
                    case TEST_DRAWPIXELSSHORT:
                        testDrawPixelsShort(dg);
                        break;
                    case TEST_GETPIXELSBYTE:
                        testGetPixelsByte(dg);
                        break;
                    case TEST_DRAWPIXELSSHORT555:
                        testDrawPixelsShort555(dg);
                        break;
                    case TEST_GETPIXELSSHORT555:
                        testGetPixelsShort555(dg);
                        break;
                    case TEST_DRAWPIXELSSHORT444:
                        testDrawPixelsShort444(dg);
                        break;
                    case TEST_GETPIXELSSHORT444:
                        testGetPixelsShort444(dg);
                        break;
                    case TEST_DRAWPIXELSSHORT4444:
                        testDrawPixelsShort4444(dg);
                        break;
                    case TEST_GETPIXELSSHORT4444:
                        testGetPixelsShort4444(dg);
                        break;
                    case TEST_GETPIXELSSHORT:
                        testGetPixelsShort(dg);
                        break;
                    case TEST_DRAWPOLYGON:
                        testDrawPolygon(dg);
                        break;
                    case TEST_DRAWTRIANGLE:
                        testDrawTriangle(dg);
                        break;
                    case TEST_FILLPOLYGON:
                        testFillPolygon(dg);
                        break;
                    case TEST_FILLTRIANGLE:
                        testFillTriangle(dg);
                        break;
                    case TEST_GETNATIVEPIXELFORMAT:
                        testGetNativePixelFormat(dg);
                        break;
                    case TEST_SETGETARGBCOLOR:
                        testSetGetAlphaColor(dg);
                        break;
                    default:
                }
            }
            /*catch (AssertionFailedException afe) {
                setFailMessage(afe.getMessage());
            }*/
            catch (Exception e) {
            	setFailMessage(e.getMessage());
            }
            finally {
                testMode = TEST_NONE;
            }
        }

        private void testDrawImage(DirectGraphics dg) throws IOException {
            Image testImage = Image.createImage("image1.jpg");
            try {
                dg.drawImage(null, 0, 0, 0, 0);
                fail("NullPointerException should be thrown on null image");
            }
            catch (NullPointerException npe) {
                // Ok
            }

            try {
                dg.drawImage(testImage, 0, 0, 0, -1);
                fail("IllegalArgumentException should be thrown on invalid manipulation");
            }
            catch (IllegalArgumentException iae) {
                // Ok
            }

            try {
                dg.drawImage(testImage, 0, 0, -1, 0);
                fail("IllegalArgumentException should be thrown on invalid anchor");
            }
            catch (IllegalArgumentException iae) {
                // Ok
            }

            dg.drawImage(testImage, 0, 0, 0, 0);
            dg.drawImage(testImage, 0, 0, 0, DirectGraphics.ROTATE_90);
            dg.drawImage(testImage, 0, 0, 0,
                    DirectGraphics.ROTATE_90 | DirectGraphics.FLIP_HORIZONTAL);
        }

        private void testDrawPixels(DirectGraphics dg) {
            try {
                dg.drawPixels( (int[]) null, false, 0, 0,
                        0, 0, 0, 0,
                        0, DirectGraphics.TYPE_INT_888_RGB);
                fail("NullPointerException should be thrown on null array - drawPixels");
            }
            catch (NullPointerException npe) {
                // Ok
            }
            try {
                dg.drawPixels( rgbDataSet, false, -1, 0,
                        0, 0, 0, 0,
                        0, DirectGraphics.TYPE_INT_888_RGB);
                fail("ArrayIndexOutOfBoundsException should be thrown on invalid offset");
            }
            catch (ArrayIndexOutOfBoundsException obe) {
                // Ok
            }
            try {
                dg.drawPixels(rgbDataSet, false, 0, testWidth,
                        0, 0, testWidth, testHeight,
                        -1, DirectGraphics.TYPE_INT_888_RGB);
                fail("IllegalArgumentException should be thrown on invalid manipulation");
            }
            catch (IllegalArgumentException obe) {
                // Ok
            }
            try {
                dg.drawPixels(rgbDataSet, false, 0, testWidth,
                        0, 0, testWidth, testHeight,
                        0, -1);
                fail("IllegalArgumentException should be thrown on invalid format");
            }
            catch (IllegalArgumentException obe) {
                // Ok
            }
            // draw image to GC
            dg.drawPixels(rgbDataSet, false, 0, testWidth,
                    0, 0, testWidth, testHeight,
                    0, DirectGraphics.TYPE_INT_888_RGB);
        }

        private void testDrawPixelsByte(DirectGraphics dg) {
            try {
                dg.drawPixels( (byte[]) null, (byte[]) null, 0, 0,
                        0, 0, 0, 0,
                        0, DirectGraphics.TYPE_BYTE_1_GRAY);
                fail("NullPointerException should be thrown on null array - drawPixels");
            }
            catch (NullPointerException npe) {
                // Ok
            }

            try {
                dg.drawPixels( bRgbDataSet, bRgbDataMaskSet, -1, 0,
                        0, 0, 0, 0,
                        0, DirectGraphics.TYPE_BYTE_1_GRAY);
                fail("ArrayIndexOutOfBoundsException should be thrown on invalid offset");
            }
            catch (ArrayIndexOutOfBoundsException obe) {
                // Ok
            }

            try {
                dg.drawPixels(bRgbDataSet, bRgbDataMaskSet, 0, testWidth,
                        0, 0, testWidth, testHeight,
                        -1, DirectGraphics.TYPE_BYTE_1_GRAY);
                fail("IllegalArgumentException should be thrown on invalid manipulation");
            }
            catch (IllegalArgumentException obe) {
                // Ok
            }

            try {
                dg.drawPixels(bRgbDataSet, bRgbDataMaskSet, 0, testWidth,
                        0, 0, testWidth, testHeight,
                        0, -1);
                fail("IllegalArgumentException should be thrown on invalid format");
            }
            catch (IllegalArgumentException obe) {
                // Ok
            }

            // draw image to GC
            dg.drawPixels(bRgbDataSet, bRgbDataMaskSet, 0, testWidth,
                    0, 0, testWidth, testHeight,
                    0, DirectGraphics.TYPE_BYTE_1_GRAY);


           // while(true);
        }

        private void testDrawPixelsShort(DirectGraphics dg) {
            try {
                dg.drawPixels( (short[]) null, false, 0, 0,
                        0, 0, 0, 0,
                        0, DirectGraphics.TYPE_USHORT_565_RGB);
                fail("NullPointerException should be thrown on null array - drawPixels");
            }
            catch (NullPointerException npe) {
                // Ok
            }
            try {
                dg.drawPixels( sRgbDataSet, false, -1, 0,
                        0, 0, 0, 0,
                        0, DirectGraphics.TYPE_USHORT_565_RGB);
                fail("ArrayIndexOutOfBoundsException should be thrown on invalid offset");
            }
            catch (ArrayIndexOutOfBoundsException obe) {
                // Ok
            }
            try {
                dg.drawPixels(sRgbDataSet, false, 0, testWidth,
                        0, 0, testWidth, testHeight,
                        -1, DirectGraphics.TYPE_USHORT_565_RGB);
                fail("IllegalArgumentException should be thrown on invalid manipulation");
            }
            catch (IllegalArgumentException obe) {
                // Ok
            }
            try {
                dg.drawPixels(sRgbDataSet, false, 0, testWidth,
                        0, 0, testWidth, testHeight,
                        0, -1);
                fail("IllegalArgumentException should be thrown on invalid format");
            }
            catch (IllegalArgumentException obe) {
                // Ok
            }
            // draw image to GC
            dg.drawPixels(sRgbDataSet, false, 0, testWidth,
                    0, 0, testWidth, testHeight,
                    0, DirectGraphics.TYPE_USHORT_565_RGB);
        }

        private void testDrawPixelsShort555(DirectGraphics dg) {
            dg.drawPixels(sRgbDataSet, false, 0, testWidth,
                    0, 0, testWidth, testHeight,
                    0, DirectGraphics.TYPE_USHORT_555_RGB);
        }

        private void testGetPixelsShort555(DirectGraphics dg) {
            dg.getPixels(sRgbDataGet, 0, testWidth,
                    0, 0, testWidth, testHeight,
                    DirectGraphics.TYPE_USHORT_555_RGB);

            // compare gotten data with previously set data
            for (int i = 0; i < sRgbDataSet.length; i++) {
                if (sRgbDataGet[i] != sRgbDataSet[i]) {
                    fail("RGB Data mismatch at " + i + " : set "
                            + sRgbDataSet[i] + " get " + sRgbDataGet[i]);
                }
            }
        }

        private void testDrawPixelsShort444(DirectGraphics dg) {
            dg.drawPixels(sRgbDataSet, false, 0, testWidth,
                    0, 0, testWidth, testHeight,
                    0, DirectGraphics.TYPE_USHORT_444_RGB);
        }

        private void testGetPixelsShort444(DirectGraphics dg) {
            dg.getPixels(sRgbDataGet, 0, testWidth,
                    0, 0, testWidth, testHeight,
                    DirectGraphics.TYPE_USHORT_444_RGB);

            // compare gotten data with previously set data
            for (int i = 0; i < sRgbDataSet.length; i++) {
                if (sRgbDataGet[i] != sRgbDataSet[i]) {
                    fail("RGB Data mismatch at " + i + " : set "
                            + sRgbDataSet[i] + " get " + sRgbDataGet[i]);
                }
            }
        }

        private void testDrawPixelsShort4444(DirectGraphics dg) {
            dg.drawPixels(sRgbDataSet4444, false, 0, testWidth,
                    0, 0, testWidth, testHeight,
                    0, DirectGraphics.TYPE_USHORT_4444_ARGB);
        }

        private void testGetPixelsShort4444(DirectGraphics dg) {
            dg.getPixels(sRgbDataGet, 0, testWidth,
                    0, 0, testWidth, testHeight,
                    DirectGraphics.TYPE_USHORT_4444_ARGB);

            // compare gotten data with previously set data
            for (int i = 0; i < sRgbDataSet4444.length; i++) {
                if (sRgbDataGet[i] != sRgbDataSet4444[i]) {
                    fail("RGB Data mismatch at " + i + " : set "
                            + sRgbDataSet4444[i] + " get " + sRgbDataGet[i]);
                }
            }
        }

        private void testGetPixels(DirectGraphics dg) {
            try {
                dg.getPixels( (int[]) null, 0, 1,
                        0, 0, 1, 1,
                        DirectGraphics.TYPE_INT_888_RGB);
                fail("IllegalArgumentException should be thrown on null array - getPixels");
            }
            catch (IllegalArgumentException iae) {
                // Ok
            }

            // get data from GC
            dg.getPixels(rgbDataGet, 0, testWidth,
                    0, 0, testWidth, testHeight,
                    DirectGraphics.TYPE_INT_888_RGB);

            // compare gotten data with previously set data

            for (int i = 0; i < rgbDataSet.length; i++) {
                if (rgbDataGet[i] != rgbDataSet[i]) {
                    fail("RGB Data mismatch at " + i + " : set "
                            + rgbDataSet[i] + " get " + rgbDataGet[i]);
                }
            }
        }

        private void testGetPixelsByte(DirectGraphics dg) {
            try {
                dg.getPixels( (byte[]) null, (byte []) null,0, 1,
                        0, 0, 1, 1,
                        DirectGraphics.TYPE_BYTE_1_GRAY);
                fail("IllegalArgumentException should be thrown on null array - getPixels");
            }
            catch (IllegalArgumentException iae) {
                // Ok
            }

            // get data from GC
            dg.getPixels(bRgbDataGet, bRgbDataMaskGet,0, testWidth,
                    0, 0, testWidth, testHeight,
                    DirectGraphics.TYPE_BYTE_1_GRAY);

            // compare gotten data with previously set data
            for (int i = 0; i < (bRgbDataSet.length / 8); i++) {
                if (bRgbDataGet[i] != bRgbDataSet[i]) {
                    fail("RGB Data mismatch at " + i + " : set "
                            + bRgbDataSet[i] + " get " + bRgbDataGet[i]);
                }
                if (i != 0 && ((i % 4) == 0)) {
                    i = i + 4;
                }
            }

        }

        private void testGetPixelsShort(DirectGraphics dg) {
            try {
                dg.getPixels( (short[]) null, 0, 1,
                        0, 0, 1, 1,
                        DirectGraphics.TYPE_USHORT_565_RGB);
                fail("IllegalArgumentException should be thrown on null array - getPixels");
            }
            catch (IllegalArgumentException iae) {
                // Ok
            }

            // get data from GC
            dg.getPixels(sRgbDataGet, 0, testWidth,
                    0, 0, testWidth, testHeight,
                    DirectGraphics.TYPE_USHORT_565_RGB);

            // compare gotten data with previously set data
            for (int i = 0; i < sRgbDataSet.length; i++) {
                if (sRgbDataGet[i] != sRgbDataSet[i]) {
                    fail("RGB Data mismatch at " + i + " : set "
                            + sRgbDataSet[i] + " get " + sRgbDataGet[i]);
                }
            }
        }

        private void testDrawPolygon(DirectGraphics dg) {
            try {
                dg.drawPolygon(null, 0, null, 0, 1, 0);
                fail("NullPointerException should be thrown on null arrays");
            }
            catch (NullPointerException npe) {
                // Ok
            }

            int[] xP = new int[] {10, 20, 30, 40};
            int[] yP = new int[] {40, 30, 20, 10};

            try {
                dg.drawPolygon(xP, 0, yP, 0, 5, 0);
                fail("ArrayIndexOutOfBoundsException should be thrown on "
                        + "invalid nPoints");
            }
            catch (ArrayIndexOutOfBoundsException obe) {
                // Ok
            }

            try {
                dg.drawPolygon(xP, 4, yP, 3, 4, 0);
                fail("ArrayIndexOutOfBoundsException should be thrown on "
                        + "invalid offsets");
            }
            catch (ArrayIndexOutOfBoundsException obe) {
                // Ok
            }

            dg.drawPolygon(xP, 0, yP, 0, 4, 0);
            dg.drawPolygon(xP, 1, yP, 1, 3, 0);
            dg.drawPolygon(xP, 2, yP, 2, 2, 0);
        }

        private void testDrawTriangle(DirectGraphics dg) {
            dg.drawTriangle(10, 10, 20, 20, 30, 0, 0);
        }

        private void testFillPolygon(DirectGraphics dg) {
            try {
                dg.fillPolygon(null, 0, null, 0, 1, 0);
                fail("NullPointerException should be thrown on null arrays");
            }
            catch (NullPointerException npe) {
                // Ok
            }

            int[] xP = new int[] {10, 20, 30, 40};
            int[] yP = new int[] {40, 30, 20, 10};

            try {
                dg.fillPolygon(xP, 0, yP, 0, 5, 0);
                fail("ArrayIndexOutOfBoundsException should be thrown on "
                        + "invalid nPoints");
            }
            catch (ArrayIndexOutOfBoundsException obe) {
                // Ok
            }

            try {
                dg.fillPolygon(xP, 4, yP, 3, 4, 0);
                fail("ArrayIndexOutOfBoundsException should be thrown on "
                        + "invalid offsets");
            }
            catch (ArrayIndexOutOfBoundsException obe) {
                // Ok
            }

            dg.fillPolygon(xP, 0, yP, 0, 4, 0);
            dg.fillPolygon(xP, 1, yP, 1, 3, 0);
            dg.fillPolygon(xP, 2, yP, 2, 2, 0);
        }

        private void testFillTriangle(DirectGraphics dg) {
            dg.fillTriangle(10, 10, 20, 20, 30, 0, 0);
        }

        private void testGetNativePixelFormat(DirectGraphics dg) {
            dg.getNativePixelFormat();
        }

        private void testSetGetAlphaColor(DirectGraphics dg) {
            dg.setARGBColor(0xAAFF1166);
            if (dg.getAlphaComponent() != 0xAA) {
                fail("Alpha component mismatch");
            }
        }

    }

}
