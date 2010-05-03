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
package com.nokia.openlcdui.mt.image;

import junit.framework.*;

import java.io.IOException;

import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.game.Sprite;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * Unit tests for LCDUI Image class.
 */
public class ImageTest extends SWTTestCase {

    /**
     * Constructor.
     */
    public ImageTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public ImageTest(String sTestName) {
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
	    methodNames = ImageTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new ImageTest((String)e.nextElement()));
	    }

		return suite;
	}

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testCreate");
        methodNames.addElement("testCreateFromFile");
        methodNames.addElement("testCreateFromImage");
        methodNames.addElement("testCreateFromARGB");
        methodNames.addElement("testCreateFromDataBuffer");
        methodNames.addElement("testCreateFromImageTrans");
        methodNames.addElement("testGetRGB");
        return methodNames;
    }
    
    protected void runTest() throws Throwable {
        if (getName().equals("testCreate")) testCreate();
        else if (getName().equals("testCreateFromFile")) testCreateFromFile();
        else if (getName().equals("testCreateFromImage")) testCreateFromImage();
        else if (getName().equals("testCreateFromARGB")) testCreateFromARGB();
        else if (getName().equals("testCreateFromDataBuffer")) testCreateFromDataBuffer();
        else if (getName().equals("testCreateFromImageTrans")) testCreateFromImageTrans();
        else if (getName().equals("testGetRGB")) testGetRGB();
        else super.runTest();
    }
    
    
    /**
     * Tests for image creation.
     * Tests Image.createImage(int width, int height).
     */
    public void testCreate() {

        Image testImage = null;
        final int testSize = 20;
        // Test IllegalArgumentException
        try {
            testImage = Image.createImage(0, -1);
            fail("No IllegalArgumentException calling "
                    + "Image.createImage(0, -1); in testCreateImage1");
        }
        catch (IllegalArgumentException e) {
        	// Ok
        }

        // Test valid inputs
        try {
            testImage = Image.createImage(testSize, testSize);
        }
        catch (Exception e) {
            fail("Unexpected exception calling Image.createImage("
                    + testSize + ", " + testSize
                    + ") in testCreateImage1:" + e);
        }

        assertTrue("Image.createImage(int, int) "
                   + "returned null in testCreateImage1",
                   testImage != null);

        assertTrue("Image.createImage(" + testSize + ", " + testSize
                   + "): testImage.getWidth() does not"
                   + " match initial width in testCreateImage1",
                   testSize == testImage.getWidth());

        assertTrue("Image.createImage(" + testSize + ", " + testSize
                   + "): testImage.getHeight() does "
                   + "not match initial height in testCreateImage1",
                   testSize == testImage.getHeight());

        assertTrue("Image.isMutable() returned false for "
                   + "file-based Image in testCreateImage2, "
                   + "expected - true",
                   testImage.isMutable());

        try {
            testImage.getGraphics();
        }
        catch (Exception e) {
            fail("Unexpected exception on Image.getGraphics() "
                  + "for immutable Image in testCreateImage2: " + e);
        }


//        try {
//            testImage = Image.createImage(maxTestSize, maxTestSize);
//        }
//        catch (Exception e) {
//            fail("Unexpected exception calling Image.createImage("
//                 + maxTestSize + ", " + maxTestSize
//                 + ") in testCreateImage1");
//        }
//
//        assertTrue("Image.createImage(" + maxTestSize + ", " + maxTestSize
//                   + "): testImage.getWidth() does not"
//                   + " match initial width in testCreateImage1",
//                   maxTestSize == testImage.getWidth());
//
//        assertTrue("Image.createImage(" + maxTestSize + ", " + maxTestSize
//                   + "): testImage.getHeight() does not match "
//                   +  "initial height in testCreateImage1",
//                   maxTestSize == testImage.getHeight());

    }

    /**
     *  Tests for image creation from a file.
     *  Tests Image.createImage(String fileName)
     */
    public void testCreateFromFile() {

        Image testImage = null;

        // Test NullPointerException
        try {
            testImage = Image.createImage((String) null);
            fail("NullPointerException not thrown in createImage(null)");
        }
        catch (NullPointerException ioE) {
        	// Ok
        }
        catch (Exception e) {
            fail("Unexpected exception calling "
                 + "Image.createImage(null) in testCreateImage2: " + e);
        }

        // Test createImage for non-existing file
        String invalidName = "12345678.90";
        try {
            testImage = Image.createImage(invalidName);
            fail("IOException not thrown when calling Image.createImage(invalidName)");
        }
        catch (IOException ioE) {
        	// Ok
        }
        catch (Exception e) {
            fail("Unexpected exception calling "
                 + "Image.createImage(invalidName) in testCreateImage2: " + e);
        }

        // Test createImage for the file that contains invalid data
        /*String invalidFile = "invalid.jpg";
        try {
            testImage = Image.createImage(invalidFile);
            fail("IOException not thrown when calling Image.createImage(invalidFile)");
        }
        catch (IOException ioE) {
        	// Ok
        }
        catch (Exception e) {
            fail("Unexpected exception calling "
                 + "Image.createImage(invalidFile) in testCreateImage2:" + e);
        }*/

        // Test createImage for the file that contains valid data
        String imgFile = "image1.jpg";
        try {
            testImage = Image.createImage(imgFile);
        }
        catch (IOException ioE) {
            fail("I/O exception in testCreateImage2");
        }
        catch (Exception e) {
            fail("Unexpected exception in testCreateImage2: " + e);
        }

        assertTrue("Image.createImage(String) returned "
                   + "null in testCreateImage2",
                   testImage != null);

        assertTrue("Image.isMutable() returned true"
                   + " for file-based Image in testCreateImage2, "
                   + "expected - false",
                   !testImage.isMutable());

        try {
            Graphics g = testImage.getGraphics();
            fail("IllegalStateException not thrown for Image.getGraphics with immutable Image");
        }
        catch (IllegalStateException e) {
        	// Ok
        }
        catch (Exception e) {
            fail("Unexpected exception on Image.getGraphics() "
                  + "for immutable Image in testCreateImage2: " + e);
        }
    }

    /**
     *  Tests for image creation from another image.
     *  Tests Image.createImage(Image img)
     */
    public void testCreateFromImage() {
        Image srcImage = null;
        Image testImage = null;

        // Test NullPointerException
        try {
            testImage = Image.createImage((Image) null);
            fail("NullPointerException not thrown in createImage(null)");
        }
        catch (NullPointerException ioE) {
        	// Ok
        }
        catch (Exception e) {
            fail("Unexpected exception calling "
                 + "Image.createImage(null) in testCreateImage3: " + e);
        }

        final int testSize = 20;
        try {
            srcImage = Image.createImage(testSize, testSize);
        }
        catch (Exception e) {
            fail("Unexpected exception calling Image.createImage("
                    + testSize + ", " + testSize
                    + ") in testCreateImage3: " + e);
        }

        try {
            testImage = Image.createImage(srcImage);
        }
        catch (Exception e) {
            fail("Unexpected exception calling Image.createImage(Image)"
                    + " in testCreateImage3: " + e);
        }

        assertTrue("Image.createImage(Image) returned null in testCreateImage3",
                testImage != null);

        if (testImage.isMutable()) {
            fail("Image must be immutable in testCreateImage3");
        }
    }

    /**
     * Tests for image creation from ARGB array.
     * Tests Image.createRGBImage(int[] rgb,
                                   int width,
                                   int height,
                                   boolean processAlpha);
     */
    public void testCreateFromARGB() {

        final int[] argb = {
            0xFFFFFFFF,
            0x00000000,
            0x01010101,
            0xA0A0A0A0,
        };
        final int w = 4;
        final int h = 1;

        Image testImage = null;

        // Test NullPoiterException
		try {
			testImage = Image.createRGBImage(null, w, h, true);
			fail("NullPointerException not thrown on Image.createRGBImage(null, w, h, true)");
		}
		catch (NullPointerException e) {
			// Ok
		}
		catch (Exception e) {
			fail("Unexpected exception calling "
					+ "Image.createRGBImage(null, w, h, true);"
					+ " in testCreateRGBImage: " + e);
		}

		// Test IllegalArgumentException
		try {
			testImage = Image.createRGBImage(argb, 0, -1, true);
			fail("IllegalArgumentException not thrown on Image.createRGBImage(argb, 0, -1, true)");
		}
		catch (IllegalArgumentException e) {
			// Ok
		}
		catch (Exception e) {
			fail("Unexpected exception "
					+ "calling Image.createRGBImage(argb, 0, -1, true); "
					+ "in testCreateRGBImage: " + e);
		}

		// Test ArrayIndexOutOfBoundsException
		try {
			testImage = Image.createRGBImage(argb, 2 * w, h, true);
			fail("ArrayIndexOutOfBoundsException not thrown on Image.createRGBImage(argb, 2 * w, h, true)");
		}
		catch (ArrayIndexOutOfBoundsException e) {
			// Ok
		}
        catch (Exception e) {
            fail("Unexpected exception calling "
                 + "Image.createRGBImage(argb, 2 * w, h, true); "
                 + "in testCreateRGBImage: " + e);
        }

        // Test valid inputs
        try {
            testImage = Image.createRGBImage(argb, w, h, true);
        }
        catch (Exception e) {
            fail("Unexpected exception calling Image.createImage(Image)"
                    + " in testCreateRGBImage: " + e);
        }

        assertTrue("Image.createRGBImage() returned null", testImage != null);
    }


    /**
     *  Tests for image creation from an encoded data buffer.
     *  Tests Image.createImage(byte[] data, int offset, int dataLength).
     */
    public void testCreateFromDataBuffer() {

    	DISABLE_TEST();
        Image testImage = null;
        String imgFile = "/image1.jpg";
        int dataLen = 0;
        byte[] dataBuffer = null;

        // Test NullPointerException
        try {
            testImage = Image.createImage(null, 10, 10);
            fail("NullPointerException not thrown on Image.createImage(null, 10, 10)");
        }
        catch (NullPointerException e) {
        	// Ok
        }
        catch (Exception e) {
            fail("Unspecified exception in "
                 + "Image.createImage(null, 10, 10) in testCreateImage5: " + e);
        }

        try {
            java.io.InputStream fis = Image.class.getResourceAsStream(imgFile);
            dataLen = fis.available();
            dataBuffer = new byte[dataLen];
            fis.read(dataBuffer);
            fis.close();
        }
        catch (Exception e) {
            fail("Exception reading a buffer in testCreateImage5: " + e);
        }

        // Test ArrayIndexOutOfBoundsException
        try {
            testImage = Image.createImage(dataBuffer, 0, dataLen * 2);
            fail("ArrayIndexOutOfBoundsException not thrown on Image.createImage(dataBuffer, 0, dataLen*2)");
        }
        catch (ArrayIndexOutOfBoundsException e) {
        	// Ok
        }
        catch (Exception e) {
            fail("Unspecified exception in "
                 + "Image.createImage(dataBuffer, 0, dataLen*2)"
                 + " in testCreateImage5: " + e);
        }

        // Test IllegalArgumentException

        // create some invalid data
        byte[] invalidBuffer = new byte[dataLen];
        System.arraycopy(dataBuffer, 0, invalidBuffer, 0, dataLen);
        for (int i = 0; i < dataLen; i += 5) {
            invalidBuffer[i] = (byte) (System.currentTimeMillis() % 255);
        }

        try {
            testImage = Image.createImage(invalidBuffer, 0, dataLen);
            fail("IllegalArgumentException not thrown on Image.createImage(invalidBuffer, 0, dataLen)");
        }
        catch (IllegalArgumentException  e) {
        	// Ok
        }
        catch (Exception e) {
            fail("Unexpected exception in "
                 + "Image.createImage(invalidBuffer, 0, dataLen) "
                 + "in testCreateImage5: " + e);
        }

        // Test valid inputs
        try {
            testImage = Image.createImage(dataBuffer, 0, dataLen);
        }
        catch (Exception e) {
            fail("Unexpected exception: " + e);
        }

        assertTrue("Image.createImage(byte[], int, int) "
                   + "returned null in testCreateImage5",
                   testImage != null);

        assertTrue("Image.isMutable() returned true "
                   + "for buffer-based Image in testCreateImage5, "
                   + "expected - false",
                   !testImage.isMutable());

        try {
            Graphics g = testImage.getGraphics();
            fail("IllegalStateException not thrown on Image.getGraphics for immutable Image");
        }
        catch (IllegalStateException e) {
        	// Ok
        }
        catch (Exception e) {
            fail("Unexpected exception on Image.getGraphics() "
                  + "for immutable Image in testCreateImage5: " + e);
        }
    }

    /**
     *  Tests for image creation as a transformed part of another image.
     *  Tests Image.createImage(Image img, int x, int y,
     *                          int w, int h, int transform).
     */
    public void testCreateFromImageTrans() {

        Image srcImage = null;
        Image testImage = null;
        final int INVALID_TRANSFORM = 0xfff;

        String imgFile = "image1.jpg";
        try {
            try {
                srcImage = Image.createImage(imgFile);
            }
            catch (IOException ioE) {
                fail("I/O exception in testCreateImage6");
            }

            // Test null source image
            try {
                testImage = Image.createImage(null, 10, 10,
                                              20, 30, Sprite.TRANS_NONE);

                fail("NullPointerException not thrown for null source image");
            }
            catch (NullPointerException e) {
            	// Ok
            }

            // Test illegal width/height < 0 values
            try {
                testImage = Image.createImage(srcImage, 10, 10,
                                              -1, 0, Sprite.TRANS_NONE);
                fail("IllegalArgumentException not thrown for width/height < 0");
            }
            catch (IllegalArgumentException e) {
            	// Ok
            }

            // Test invalid region bounds
            try {
                testImage = Image.createImage(srcImage, -10, -10,
                                              20, 30, Sprite.TRANS_NONE);
                fail("IllegalArgumentException not thrown for "
                     + "negative bounds invalid region");
            }
            catch (IllegalArgumentException e) {
            	// Ok
            }

            try {
                testImage = Image.createImage(srcImage, 10, 10,
                                              2000, 30, Sprite.TRANS_NONE);
                fail("IllegalArgumentException not thrown "
                     + "for too big invalid region");
            }
            catch (IllegalArgumentException e) {
            	// Ok
            }

            // Test invalid transform value
            try {
                testImage = Image.createImage(srcImage, 10, 10,
                                              20, 30, INVALID_TRANSFORM);
                fail("IllegalArgumentException not thrown "
						+ "for illegal transform ");
            }
            catch (IllegalArgumentException e) {
            	// Ok
            }

            testImage = Image.createImage(srcImage, 10, 10,
                                          20, 30, Sprite.TRANS_NONE);
        }
        catch (Exception e) {
            fail("Unexpected exception in testCreateImage6: " + e);
            e.printStackTrace();
        }

        assertTrue("Image.createImage(Image img, int x, int y, int w, "
                    + "int h, int transform) returned null", testImage != null);
    }

    /**
     *  Tests for Image.getRGB method.
     *  Tests public void getRGB(int[] rgbData,
     *              int offset,
     *              int scanlength,
     *              int x,
     *              int y,
     *              int width,
     *              int height)
     */
    public void testGetRGB() {

        String imgFile = "image1.jpg";

        int[] longImgData;
        int[] shortImgData;

        Image srcImage = null;

        try {
            try {
                srcImage = Image.createImage(imgFile);
            }
            catch (IOException ioE) {
                fail("Unable to load test image in testGetRGB");
            }

            longImgData = new int[srcImage.getWidth() * srcImage.getHeight()];
            shortImgData = new int[srcImage.getWidth()
                                   * srcImage.getHeight() / 10];

            // Test NullPointerException
            try {
                srcImage.getRGB(null, 10, 10, 10,  10, 10, 10);
                fail("NullPointerException not thrown in testGetRGB");
            }
            catch (NullPointerException e) {
            	// Ok
            }

            // Test IllegalArgumentException on invalid bounds
            try {
                srcImage.getRGB(longImgData, 0, 500, 0,  0, 1000, 1000);
                fail("IllegalArgumentException not thrown on invalid bounds (case1)");
            }
            catch (IllegalArgumentException e) {
            	// Ok
            }

            // Test IllegalArgumentException if abs(scanlength)  < width
            int regionWidth = 50;
            try {
                srcImage.getRGB(longImgData, 0,
                                regionWidth - 1, 0,  0, regionWidth, 10);
                fail("IllegalArgumentException not thrown on invalid bounds (case2)");
            }
            catch (IllegalArgumentException e) {
            	// Ok
            }

            try {
                srcImage.getRGB(longImgData, 0,
                                -regionWidth + 1, 0,  0, regionWidth, 10);
                fail("IllegalArgumentException not thrown on invalid bounds (case3)");
            }
            catch (IllegalArgumentException e) {
            	// Ok
            }

            // Test ArrayIndexOutOfBoundsException
            try {
                int imgWidth = srcImage.getWidth();
                int imgHeight = srcImage.getHeight();
                srcImage.getRGB(shortImgData, 0,
                                imgWidth, 0, 0, imgWidth, imgHeight);

                fail("ArrayIndexOutOfBoundsException not thrown");
            }
            catch (ArrayIndexOutOfBoundsException e) {
            	// Ok
            }

            // Test valid inputs
            int imgWidth = srcImage.getWidth();
            int imgHeight = srcImage.getHeight();
            srcImage.getRGB(longImgData, 0,
                            imgWidth, 0, 0, imgWidth, imgHeight);

            assertTrue("Image.getRGB returned no RGB data with scanLength > 0",
                       longImgData[0] != 0);

            // TODO: Enable when CG supports that

//            srcImage.getRGB(longImgData, longImgData.length - 1,
//                    -imgWidth, 0, 0, imgWidth, imgHeight);
//
//            assertTrue("Image.getRGB returned no RGB data with scanLength < 0",
//                    longImgData[longImgData.length - 1] != 0);
        }
        catch (Exception e) {
            fail("Unexpected exception in testGetRGB: " + e);
            e.printStackTrace();
        }
    }

}
