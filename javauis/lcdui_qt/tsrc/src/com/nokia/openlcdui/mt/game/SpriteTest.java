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
import javax.microedition.lcdui.game.Sprite;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * Sprite non interactive tests
 *
 * @resource com/symbian/tests/javax/microedition/lcdui/game/fly_small.png
 * @resource com/symbian/tests/javax/microedition/lcdui/game/fly_large.png
 */
public class SpriteTest extends SWTTestCase
{

    // data used to create Sprite with 6 frames.
    int iFrameCount6 = 6;
    int iFrameWidth6 = 20;
    int iFrameHeight6 = 10;
    int[] iDefaultSeq6 = {0, 1, 2, 3, 4, 5};
    int[] iCustomSeq6 = {0, 1, 0, 3, 0, 5, 0, 2, 0};

    // data used to create Sprite with 12 frames.
    int iFrameCount12 = 12;
    int iFrameWidth12 = 10;
    int iFrameHeight12 = 10;
    int[] iDefaultSeq12 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    int[] iCustomSeq12 = {0, 1, 0, 3, 0, 5, 0, 8, 0};

    // data used to create Sprite with 24 frames.
    int iFrameCount24 = 24;
    int iFrameWidth24 = 5;
    int iFrameHeight24 = 10;
    int[] iDefaultSeq24 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                           15, 16, 17, 18, 19, 20, 21, 22, 23
                          };
    int[] iCustomSeq24 = {0, 1, 1};

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
                                    -109, -10, -96, 11, 49, 66, 64, 2, 0, 59, 0,
                                   };

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
                                     50, -68, 117, -47, -46, -52, 76, -52, 9, 0, 59, 0
                                    };

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
                                    58, 116, 115, 103, -50, -114, 49, 3, -52, -44, -103, 0, 0, 59, 0
                                   };

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
                                    2, 0, 59, 0
                                   };

    // create images from byte array data
    private Image[] iImages =
    {
        Image.createImage(iImgData60x20, 0, iImgData60x20.length), // grid
        Image.createImage(iImgData40x30, 0, iImgData40x30.length), // grid
        Image.createImage(iImgData120x10, 0, iImgData120x10.length), // vertical
        Image.createImage(iImgData20x60, 0, iImgData20x60.length), // horizontal
    };

    /**
     * Constructor.
     */
    public SpriteTest()
    {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public SpriteTest(String sTestName)
    {
        super(sTestName);
    }

    /**
     * To create the test suite. You need to add a new aSuite.addTest entry for
     * any new test methods.
     *
     * @return new testsuite.
     */
    public static Test suite()
    {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
        java.util.Enumeration e;

        // Add widget tests
        methodNames = SpriteTest.methodNames();
        e = methodNames.elements();
        while(e.hasMoreElements())
        {
            suite.addTest(new SpriteTest((String)e.nextElement()));
        }

        return suite;
    }

    public static java.util.Vector methodNames()
    {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testConstructors");
        methodNames.addElement("testFrameSequences");
        methodNames.addElement("testFrameSequenceRotation");
        methodNames.addElement("testRotations");
        methodNames.addElement("testChangeImage");
        return methodNames;
    }

    protected void runTest() throws Throwable
    {
        if(getName().equals("testConstructors")) testConstructors();
        else if(getName().equals("testFrameSequences")) testFrameSequences();
        else if(getName().equals("testFrameSequenceRotation")) testFrameSequenceRotation();
        else if(getName().equals("testRotations")) testRotations();
        else if(getName().equals("testChangeImage")) testChangeImage();
        else super.runTest();
    }


    /**
     * 1.0 Sprite - Construction Test
     */
    public void testConstructors()
    {
        Image image = iImages[0];
        int frameWidth = iFrameWidth6;
        int frameHeight = iFrameHeight6;
        Sprite sprite;

        // checking("Test 1.01 Construct with frame width=0");
        try
        {
            sprite = new Sprite(image, 0, frameHeight);
            fail("IllegalArgumentException expected");
        }
        catch(IllegalArgumentException ex)
        {
        }
        catch(Exception ex)
        {
            fail("Wrong exception " + ex);
        }

        // checking("Test 1.02 Construct with frame height=0");
        try
        {
            sprite = new Sprite(image, frameWidth, 0);
            fail("IllegalArgumentException expected");
        }
        catch(IllegalArgumentException ex)
        {
        }
        catch(Exception ex)
        {
            fail("Wrong exception " + ex);
        }

        // checking("Test 1.03 Construct with incorrect frame width for image");
        try
        {
            // image width is not an integer multiple of the frameWidth
            int wrongWidth = image.getWidth() / 2 + 1;
            sprite = new Sprite(image, wrongWidth, frameHeight);
            fail("IllegalArgumentException expected");
        }
        catch(IllegalArgumentException ex)
        {
        }
        catch(Exception ex)
        {
            fail("Wrong exception " + ex);
        }

        // checking("Test 1.04 Construct with incorrect frame height for image");
        try
        {
            // image height is not an integer multiple of the frameHeight
            int wrongHeight = image.getHeight() / 2 + 1;
            sprite = new Sprite(image, frameWidth, wrongHeight);
            fail("IllegalArgumentException expected");
        }
        catch(IllegalArgumentException ex)
        {
        }
        catch(Exception ex)
        {
            fail("Wrong exception " + ex);
        }

        // checking("Test 1.05 Construct with correct frame size");
        for(int i = 0; i < iImages.length; i++)
        {
            Image img = iImages[i];
            // single frame
            runConstructTest(img, img.getWidth(), img.getHeight(), 1);
            // vary number of frames and way in which frames stored - grid,
            // horizontally or vertically.
            runConstructTest(img, iFrameWidth6, iFrameHeight6, iFrameCount6);
            runConstructTest(img, iFrameWidth12, iFrameHeight12, iFrameCount12);
            runConstructTest(img, iFrameWidth24, iFrameHeight24, iFrameCount24);
        }

        // checking("Test 1.06 Construct from another Sprite");
        sprite = new Sprite(image, frameWidth, frameHeight);
        sprite.setPosition(5, 6);
        sprite.defineReferencePixel(2, 2);
        sprite.setFrameSequence(iCustomSeq6);
        sprite.setFrame(1);
        assertTrue("Is the sprite visible by default?", sprite.isVisible());
        sprite.setVisible(false);
        sprite.setTransform(Sprite.TRANS_ROT180);
        sprite.defineCollisionRectangle(0, 0, 2, 2);

        // check all attributes of the clone and sprite are the same
        Sprite clone = new Sprite(sprite);
        assertEquals("getWidth ", clone.getWidth(), sprite.getWidth());
        assertEquals("getHeight ", clone.getHeight(), sprite.getHeight());
        assertEquals("getX()", clone.getX(), sprite.getX());
        assertEquals("getY()", clone.getY(), sprite.getY());
        assertTrue("isVisible()", clone.isVisible() == sprite.isVisible());
        assertEquals("getRawFrameCount ", clone.getRawFrameCount(), sprite
                     .getRawFrameCount());
        assertEquals("getFrame()", clone.getFrame(), sprite.getFrame());
        assertEquals("getFrameSequenceLength()",
                     clone.getFrameSequenceLength(), sprite.getFrameSequenceLength());
        assertEquals("getRefPixelX()", clone.getRefPixelX(), sprite
                     .getRefPixelX());
        assertEquals("getRefPixelY()", clone.getRefPixelY(), sprite
                     .getRefPixelY());
        assertTrue("isVisible()", clone.isVisible() == sprite.isVisible());
        // check frame sequence
        for(int i = 0; i < iCustomSeq6.length; i++)
        {
            int frOriginal = sprite.getFrame();
            int frClone = clone.getFrame();
            assertEquals("frame sequence, frame:" + i, frOriginal, frClone);
            sprite.nextFrame();
            clone.nextFrame();
        }
    }

    private void runConstructTest(Image img, int frameWidth, int frameHeight,
                                  int frameCount)
    {
        Sprite sprite = new Sprite(img, frameWidth, frameHeight);
        assertEquals("getRawFrameCount ", frameCount, sprite.getRawFrameCount());
        assertEquals("getWidth ", frameWidth, sprite.getWidth());
        assertEquals("getHeight ", frameHeight, sprite.getHeight());
    }

    /**
     * 2.0 Sprite - Frame Sequence Test
     */
    public void testFrameSequences()
    {
        int aFrameWidth = 4;
        int aFrameHeight = 4;
        int[] argb = new int[48];

        // the image will contain three frames
        // first 4x4 frame is black, second frame is white,
        // third frame has vertical stripes white/black/white/black
        for(int i = 0; i < 48; i++)
        {
            int x = i % 12;
            if(x < 4)
            {
                argb[i] = 0xFF000000;
            }
            else if(x < 8)
            {
                argb[i] = 0xFFFFFFFF;
            }
            else
            {
                int c = x % 2;
                if(c == 0)
                    argb[i] = 0xFFFFFFFF;
                else
                    argb[i] = 0xFF000000;
            }
        }

        Image aImage = Image.createRGBImage(argb, aFrameWidth * 3,
                                            aFrameHeight, true);
        Sprite sprite = new Sprite(aImage, aFrameWidth, aFrameHeight);

        // check methods
        // checking("2.01 Checking default frame sequence");

        // default frame sequence
        // checking("  2.01.01 Frame sequence length");
        assertEquals(3, sprite.getFrameSequenceLength());

        // default raw frame count
        // checking("  2.01.02 Raw frame count");
        assertEquals(3, sprite.getRawFrameCount());

        // checking("  2.01.03 First frame");
        assertEquals(0, sprite.getFrame());
        verifyImage(sprite, 0); // 0 defines the frame to check

        // nextFrame
        // checking("  2.01.04 nextFrame");
        sprite.nextFrame();
        assertEquals(1, sprite.getFrame());
        verifyImage(sprite, 1); // 0 defines the frame to check

        sprite.nextFrame();
        assertEquals(2, sprite.getFrame());
        verifyImage(sprite, 2); // 0 defines the frame to check

        sprite.nextFrame();
        assertEquals(0, sprite.getFrame());
        verifyImage(sprite, 0); // 0 defines the frame to check

        // setFrame
        // checking("  2.01.05 setFrame");
        sprite.setFrame(1);
        assertEquals(1, sprite.getFrame());
        verifyImage(sprite, 1); // 0 defines the frame to check

        // prevFrame
        // checking("  2.01.06 prevFrame");
        sprite.prevFrame();
        assertEquals(0, sprite.getFrame());
        verifyImage(sprite, 0); // 0 defines the frame to check

        sprite.prevFrame();
        assertEquals(2, sprite.getFrame());
        verifyImage(sprite, 2); // 0 defines the frame to check

        sprite.prevFrame();
        assertEquals(1, sprite.getFrame());
        verifyImage(sprite, 1); // 0 defines the frame to check

        // custom frame sequence
        // checking("2.02 Checking custom frame sequence");

        // checking("  2.02.01 setFrameSequence - invalid parameters - array len is 0");
        try
        {
            sprite.setFrameSequence(new int[0]);
            fail("IllegalArgumentException expected");
        }
        catch(IllegalArgumentException e)
        {
        } // pass
        catch(Exception ex)
        {
            fail("Wrong exception " + ex);
        }

        int[] newFs = {0, 2, -1};
        // checking("  2.02.02 setFrameSequence - invalid parameters - array contains negative numbers");
        try
        {
            sprite.setFrameSequence(newFs);
            fail("ArrayIndexOutOfBoundsException expected");
        }
        catch(ArrayIndexOutOfBoundsException e)
        {
        } // pass
        catch(Exception ex)
        {
            fail("Wrong exception " + ex);
        }

        int[] newFs1 = {0, 5, 1};
        newFs = newFs1;
        // checking("  2.02.03 setFrameSequence - invalid parameters - array contains too large frame index ");
        try
        {
            sprite.setFrameSequence(newFs);
            fail("ArrayIndexOutOfBoundsException expected");
        }
        catch(ArrayIndexOutOfBoundsException e)
        {
        } // pass
        catch(Exception ex)
        {
            fail("Wrong exception " + ex);
        }

        int[] tmpFs2 = {0, 2, 1, 0, 2, 1};
        newFs = tmpFs2;
        // checking("  2.02.04 setFrameSequence - valid parameters");
        try
        {
            sprite.setFrameSequence(newFs);
        }
        catch(Exception ex)
        {
            fail("Wrong exception " + ex);
        }

        int[] newFs2 = {0, 2, 1};
        newFs = newFs2;
        // checking("  2.02.04a setFrameSequence - valid sequence of different length");
        try
        {
            sprite.setFrameSequence(newFs);
        }
        catch(Exception ex)
        {
            fail("Wrong exception " + ex);
        }

        // is passed null, frame sequence must be reset to default
        // checking("  2.02.05 setFrameSequence - null argument");
        try
        {
            try
            {
                sprite.setFrameSequence(null);
            }
            catch(Exception ex)
            {
                fail("Exception " + ex);
            }
            // checking("  2.02.05 Frame 0");
            assertEquals(0, sprite.getFrame());
            verifyImage(sprite, 0); // 0 defines the frame to check

            sprite.nextFrame();
            // checking("  2.02.05 Frame 1");
            assertEquals(1, sprite.getFrame());
            verifyImage(sprite, 1); // 0 defines the frame to check

            sprite.nextFrame();
            // checking("  2.02.05 Frame 2");
            assertEquals(2, sprite.getFrame());
            verifyImage(sprite, 2); // 0 defines the frame to check

            sprite.nextFrame();
            // checking("  2.02.05 Frame 0");
            assertEquals(0, sprite.getFrame());
            verifyImage(sprite, 0); // 0 defines the frame to check

        }
        catch(Exception exx)
        {
            exx.printStackTrace();
            fail("Exception " + exx);
        }

        // checking("  2.02.05 Setting back to custom");
        // set it back to custom
        try
        {
            sprite.setFrameSequence(newFs);
        }
        catch(Exception ex)
        {
            fail("Exception " + ex);
        }

        // checking("  2.02.06 Frame sequence length");
        assertEquals(3, sprite.getFrameSequenceLength());

        // raw frame count?
        // checking("  2.02.07 Raw frame count");
        assertEquals(3, sprite.getRawFrameCount());

        // checking("  2.02.08 First frame");
        assertEquals(0, sprite.getFrame());
        verifyImage(sprite, 0); // 0 defines the frame to check

        // setFrame
        // checking("  2.02.09 setFrame");
        sprite.setFrame(1);
        assertEquals(1, sprite.getFrame());
        verifyImage(sprite, 2); // 0 defines the frame to check

        // prevFrame
        // checking("  2.02.10 prevFrame");
        sprite.prevFrame();
        assertEquals(0, sprite.getFrame());
        verifyImage(sprite, 0); // 0 defines the frame to check

        sprite.prevFrame();
        assertEquals(2, sprite.getFrame());
        verifyImage(sprite, 1); // 0 defines the frame to check

        sprite.prevFrame();
        assertEquals(1, sprite.getFrame());
        verifyImage(sprite, 2); // 0 defines the frame to check
    }

    private void verifyImage(Sprite aSprite, int aInd)
    {
        // checking( "Verifying image :" + aInd );
        Image image = Image.createImage(4, 4);
        Graphics g = image.getGraphics();
        aSprite.setPosition(0, 0);
        aSprite.paint(g);
        int[] rgb = new int[16];
        image.getRGB(rgb, 0, 4, 0, 0, 4, 4);
        // compare the data
        if(aInd < 2)
        {
            // solid black or white
            int color;
            if(aInd == 0)
            {
                color = 0xFF000000;
            }
            else
            {
                color = 0xFFFFFFFF;
            }

            for(int j = 0; j < 16; j++)
            {
                if(rgb[j] != color)
                {
                    // failed
                    printRGB(rgb, 4, 4);
                }
                assertEquals("Rgb[" + j + "]", color, rgb[j]);
            }
        }
        else if(aInd == 2)
        {
            for(int j = 0; j < 16; j++)
            {
                int color;
                if((j % 2) == 0)
                {
                    color = 0xFFFFFFFF;
                }
                else
                {
                    color = 0xFF000000;
                }
                if(rgb[j] != color)
                {
                    // failed
                    printRGB(rgb, 4, 4);
                }
                assertEquals("Rgb[" + j + "]", color, rgb[j]);
            }
        }
    }

    /**
     * 2.1 Sprite - Frame Sequence Rotation Test
     */
    public void testFrameSequenceRotation()
    {
        int aFrameWidth = 4;
        int aFrameHeight = 4;

        // frame1 black
        int[] argb1 = new int[16];
        for(int i = 0; i < 16; i++)
        {
            argb1[i] = 0xFF000000;
        }

        // frame 2 white
        int[] argb2 = new int[16];
        for(int i = 0; i < 16; i++)
        {
            argb2[i] = 0xFFFFFFFF;
        }

        // frame 3 black / white
        int[] argb3 = new int[16];
        for(int i = 0; i < 16; i++)
        {
            argb3[i] = 0xFF000000;
        }
        argb3[5] = 0xFFFFFFFF;
        argb3[6] = 0xFFFFFFFF;
        argb3[9] = 0xFFFFFFFF;
        argb3[10] = 0xFFFFFFFF;

        // frame 4 white / black
        int[] argb4 = new int[16];
        for(int i = 0; i < 16; i++)
        {
            argb4[i] = 0xFFFFFFFF;
        }
        argb4[5] = 0xFF000000;
        argb4[6] = 0xFF000000;
        argb4[9] = 0xFF000000;
        argb4[10] = 0xFF000000;

        // top left 4x4 is black
        // top right 4x4 is white
        // bottom left 4x4 is black with white 2x2 centre
        // bottom right 4x4 is white with black 2x2 centre
        Image image = createImage2(argb1, argb2, argb3, argb4, aFrameWidth,
                                   aFrameHeight);

        Sprite sprite = new Sprite(image, aFrameWidth, aFrameHeight);

        // check methods
        // checking("2.10 Checking default frame sequence");
        checkTransformedImage(sprite, argb1, argb2, argb3, argb4);

        // checking("2.11 trans TRANS_ROT90");
        sprite.setTransform(Sprite.TRANS_ROT90);
        checkTransformedImage(sprite, argb1, argb2, argb3, argb4);

        // checking("2.12 TRANS_ROT180");
        sprite.setTransform(Sprite.TRANS_ROT180);
        checkTransformedImage(sprite, argb1, argb2, argb3, argb4);

        // checking("2.13 TRANS_ROT270");
        sprite.setTransform(Sprite.TRANS_ROT270);
        checkTransformedImage(sprite, argb1, argb2, argb3, argb4);

        // checking("2.14 TRANS_MIRROR");
        sprite.setTransform(Sprite.TRANS_MIRROR);
        checkTransformedImage(sprite, argb1, argb2, argb3, argb4);

        // checking("2.15 TRANS_MIRROR_ROT90");
        sprite.setTransform(Sprite.TRANS_MIRROR_ROT90);
        checkTransformedImage(sprite, argb1, argb2, argb3, argb4);

        // checking("2.16 TRANS_MIRROR_ROT180");
        sprite.setTransform(Sprite.TRANS_MIRROR_ROT180);
        checkTransformedImage(sprite, argb1, argb2, argb3, argb4);

        // checking("2.17 TRANS_MIRROR_ROT270");
        sprite.setTransform(Sprite.TRANS_MIRROR_ROT270);
        checkTransformedImage(sprite, argb1, argb2, argb3, argb4);
    }

    /**
     * Checks correct frames are displayed even when transformed. The frames
     * displayed should always be the same, irrespective of transform.
     */
    private void checkTransformedImage(Sprite aSprite, int[] aData0,
                                       int[] aData1, int[] aData2, int[] aData3)
    {
        aSprite.setFrame(0);
        verifyImage2(aSprite, aData0);
        aSprite.setFrame(1);
        verifyImage2(aSprite, aData1);
        aSprite.setFrame(2);
        verifyImage2(aSprite, aData2);
        aSprite.setFrame(3);
        verifyImage2(aSprite, aData3);
    }

    /**
     * Creates a 8x8 image
     */
    private Image createImage2(int[] aData1, int[] aData2, int[] aData3,
                               int[] aData4, int aFrameWidth, int aFrameHeight)
    {
        Image frame1 = Image.createRGBImage(aData1, aFrameWidth, aFrameHeight,
                                            true);
        Image frame2 = Image.createRGBImage(aData2, aFrameWidth, aFrameHeight,
                                            true);
        Image frame3 = Image.createRGBImage(aData3, aFrameWidth, aFrameHeight,
                                            true);
        Image frame4 = Image.createRGBImage(aData4, aFrameWidth, aFrameHeight,
                                            true);

        Image frameImage = Image.createImage(aFrameWidth * 2, aFrameHeight * 2);
        Graphics g = frameImage.getGraphics();
        g.drawImage(frame1, 0, 0, Graphics.TOP | Graphics.LEFT);
        g.drawImage(frame2, aFrameWidth, 0, Graphics.TOP | Graphics.LEFT);
        g.drawImage(frame3, 0, aFrameHeight, Graphics.TOP | Graphics.LEFT);
        g.drawImage(frame4, aFrameWidth, aFrameHeight, Graphics.TOP
                    | Graphics.LEFT);
        return frameImage;
    }

    /**
     * Checks images used in frame sequence rotation tests aData is the RGB data
     * used to create the Sprite frame
     */
    private void verifyImage2(Sprite aSprite, int[] aData)
    {
        // draw Sprite to mutable Image
        Image image = Image.createImage(4, 4);
        Graphics g = image.getGraphics();
        // Position will have been altered by rotation
        aSprite.setPosition(0, 0);
        aSprite.paint(g);

        // get RGB
        int[] rgb = new int[16];
        image.getRGB(rgb, 0, 4, 0, 0, 4, 4);

        int length = rgb.length;
        if(length != aData.length)
        {
            // failed
            printRGB(rgb, 4, 4);
        }
        assertEquals("length", rgb.length, aData.length);

        for(int i = 0; i < length; i++)
        {
            if(rgb[i] != aData[i])
            {
                // failed
                printRGB(rgb, 4, 4);
            }
            assertEquals("i=" + i, rgb[i], aData[i]);
        }

    }

    /**
     * 3.0 Sprite - Rotation Test
     */
    public void testRotations()
    {
        // checking("Test 3.01 Rotate Sprite with no reference pixel");
        doRotation(0, 0);

        // checking("Test 3.02 Rotate Sprite with internal reference pixel");
        doRotation(1, 1);

        // checking("Test 3.03 Rotate Sprite with internal reference pixel");
        doRotation(2, 2);

        // checking("Test 3.04 Rotate Sprite with external reference pixel");
        doRotation(5, 5);

        // checking("Test 3.05 Rotate Sprite with external reference pixel");
        doRotation(5, -5);

        // checking("Test 3.06 Rotate Sprite with external reference pixel");
        doRotation(-5, 5);

        // checking("Test 3.07 Rotate Sprite with external reference pixel");
        doRotation(-5, -5);

    }

    /**
     * This method tests sprite transforms - rotation and mirroring. The tests
     * are based on a known 4x4 image. The parameters aRefX and aRefY are used
     * in defineReferencePixel call. This method tests each transform in terms
     * of:<br>
     * - Reference point preservation<br>
     * - Sprite TL correctness after transform<br>
     * - Rotation and mirroring (pixel by pixel checking)<br>
     * The test is designed to catch one-out errors.
     */
    private void doRotation(int aRefX, int aRefY)
    {
        int[] transforms = {Sprite.TRANS_NONE, Sprite.TRANS_ROT90,
                            Sprite.TRANS_ROT180, Sprite.TRANS_ROT270, Sprite.TRANS_MIRROR,
                            Sprite.TRANS_MIRROR_ROT90, Sprite.TRANS_MIRROR_ROT180,
                            Sprite.TRANS_MIRROR_ROT270
                           };
        String[] transformNames = {"TRANS_NONE", "TRANS_ROT90", "TRANS_ROT180",
                                   "TRANS_ROT270", "TRANS_MIRROR", "TRANS_MIRROR_ROT90",
                                   "TRANS_MIRROR_ROT180", "TRANS_MIRROR_ROT270"
                                  };
        Image image = null;
        Sprite sprite = null;

        // The image
        // bWWb
        // WbWb
        // WWbb
        // Wbbb
        int[] imageData = {0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000,
                           0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF,
                           0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFFFFFFFF, 0xFF000000,
                           0xFF000000, 0xFF000000
                          };

        int[][] transData =
        {
            {
                // TRANS_NONE
                0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000,
                0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF, 0xFF000000,
                0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0xFF000000,
                0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFF000000
            },
            {
                // TRANS_ROT90
                0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000,
                0xFF000000, 0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF,
                0xFF000000, 0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF,
                0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
            },
            {
                // TRANS_ROT180
                0xFF000000, 0xFF000000, 0xFF000000, 0xFFFFFFFF,
                0xFF000000, 0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF,
                0xFF000000, 0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF,
                0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000
            },
            {
                // TRANS_ROT270
                0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
                0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0xFF000000,
                0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF, 0xFF000000,
                0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
            },
            {
                // TRANS_MIRROR
                0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000,
                0xFF000000, 0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF,
                0xFF000000, 0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF,
                0xFF000000, 0xFF000000, 0xFF000000, 0xFFFFFFFF
            },
            {
                // TRANS_MIRROR_ROT90
                0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
                0xFF000000, 0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF,
                0xFF000000, 0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF,
                0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000
            },
            {
                // TRANS_MIRROR_ROT180
                0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFF000000,
                0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0xFF000000,
                0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF, 0xFF000000,
                0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000
            },
            {
                // TRANS_MIRROR_ROT270
                0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF, 0xFF000000,
                0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0xFF000000,
                0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000
            }
        };

        try
        {
            // make images and sprites
            // checking("createRGBImage");
            image = Image.createRGBImage(imageData, 4, 4, true);
        }
        catch(Exception ex)
        {
            fail("Exception " + ex);
        }

        for(int i = 0; i < transforms.length; i++)
        {
            sprite = new Sprite(image, 4, 4);
            sprite.defineReferencePixel(aRefX, aRefY);
            sprite.setRefPixelPosition(20, 20);

            Point okTopLeft = transform(transforms[i], new Point(sprite.getX(),
                                        sprite.getY()), new Point(sprite.getRefPixelY(), sprite
                                                .getRefPixelX()));

            sprite.setTransform(transforms[i]);
            Image rgb = Image.createImage(40, 40);
            sprite.paint(rgb.getGraphics());

            Point spriteTopLeft = new Point(sprite.getX(), sprite.getY());

            // is the top left point ok?
            // checking("is the top left point ok?");
            if(okTopLeft.x != spriteTopLeft.x
                    || okTopLeft.y != spriteTopLeft.y)
            {
                rotationTestFailure("Sprite's TL pixel has not been preserved. It is at : ("
                                    + spriteTopLeft.x
                                    + ","
                                    + spriteTopLeft.y
                                    + "), should be at ("
                                    + okTopLeft.x
                                    + ","
                                    + okTopLeft.y
                                    + ")."
                                    + "\nReference at ("
                                    + sprite.getRefPixelX()
                                    + ","
                                    + sprite.getRefPixelY()
                                    + ").\n"
                                    + "Transform "
                                    + transformNames[i]);
            }
            // did the reference pixel change position?
            // checking("did the reference pixel change position?");
            if(sprite.getRefPixelX() != 20 || sprite.getRefPixelY() != 20)
            {
                rotationTestFailure("Sprite's reference pixel has not been preserved. It is at : ("
                                    + sprite.getRefPixelX()
                                    + ","
                                    + sprite.getRefPixelY()
                                    + "), should be at (20,20).\n"
                                    + "Sprite location ("
                                    + spriteTopLeft.x
                                    + ","
                                    + spriteTopLeft.y
                                    + ").\n"
                                    + "Transform " + transformNames[i]);
            }

            // get a 4x4 image and compare
            // checking("get a 4x4 image and compare");
            int[] comp = new int[16];
            rgb.getRGB(comp, 0, 4, spriteTopLeft.x, spriteTopLeft.y, 4, 4);
            for(int j = 0; j < 16; j++)
            {
                if(comp[j] != transData[i][j])
                {
                    // checking("Comparison failed while testing transform "
                    // + transformNames[i] +" on pixel " + j );
                    // checking("Should be : " );
                    printRGB(transData[transforms[i]], 4, 4);
                    // checking("Is : " );
                    printRGB(comp, 4, 4);
                    rotationTestFailure("Comparison failed while testing transform "
                                        + transformNames[i] + " on pixel " + j);
                }
            }
        }

    }

    private boolean iReportRotationErrors = true;

    private void rotationTestFailure(String aMessage)
    {
        // checking("rotationTestFailure " + aMessage);
        if(iReportRotationErrors)
        {
            fail(aMessage);
        }
        else
        {
            System.out.println("\n\n" + aMessage);
        }
    }

    /**
     * Find the location of the TL of transformed sprite. This method assumes
     * 4x4 sprites.
     *
     * @param transform Transform code as defined in Sprite
     * @param pos TL of the untransformed sprite
     * @param sprite transformed sprite
     * @return the location upper-left corner of the transformed sprite
     */
    private Point transform(int aTransform, Point aPos, Point aRefPix)
    {
        switch(aTransform)
        {
        case Sprite.TRANS_NONE:
        {
            return new Point(aPos.x, aPos.y);
        }
        case Sprite.TRANS_ROT90:
        {
            // sprite top-left
            int xp = aPos.x;
            int yp = aPos.y;
            int xr = aRefPix.x;
            int yr = aRefPix.y;
            int dx = xp - xr;
            int dy = yp - yr;
            int x = xr - dy - 3;
            int y = yr + dx;
            return new Point(x, y);
        }
        case Sprite.TRANS_ROT180:
        {
            Point tmp = transform(Sprite.TRANS_ROT90, aPos, aRefPix);
            return transform(Sprite.TRANS_ROT90, tmp, aRefPix);
        }
        case Sprite.TRANS_ROT270:
        {
            Point tmp = transform(Sprite.TRANS_ROT180, aPos, aRefPix);
            return transform(Sprite.TRANS_ROT90, tmp, aRefPix);
        }
        case Sprite.TRANS_MIRROR:
        {
            int nx = 2 * aRefPix.x - aPos.x - 3;
            int ny = aPos.y;
            return new Point(nx, ny);
        }
        case Sprite.TRANS_MIRROR_ROT90:
        {
            Point tmp = transform(Sprite.TRANS_MIRROR, aPos, aRefPix);
            return transform(Sprite.TRANS_ROT90, tmp, aRefPix);
        }
        case Sprite.TRANS_MIRROR_ROT180:
        {
            Point tmp = transform(Sprite.TRANS_MIRROR, aPos, aRefPix);
            return transform(Sprite.TRANS_ROT180, tmp, aRefPix);
        }
        case Sprite.TRANS_MIRROR_ROT270:
        {
            Point tmp = transform(Sprite.TRANS_MIRROR, aPos, aRefPix);
            return transform(Sprite.TRANS_ROT270, tmp, aRefPix);
        }
        default:
            return null;
        }
    }

    class Point
    {
        public int x, y;

        public Point(int x, int y)
        {
            this.x = x;
            this.y = y;
        }
    }

    /**
     * 4.0 Sprite - Change Image Test
     */
    public void testChangeImage()
    {
        int currentFrame = 3;

        // checking("Test 4.01 Change Sprite image: same frames, default sequence");
        // Check: The current frame will be unchanged
        // Check: If no custom frame sequence is defined, the default frame
        // sequence
        // will be updated to be the default frame sequence for the new frame
        // set
        Sprite sprite = createSprite(iImages[0], iFrameWidth12, iFrameHeight12,
                                     null, currentFrame);
        sprite.setImage(iImages[1], iFrameWidth12, iFrameHeight12);
        checkFrames(sprite, iDefaultSeq12, currentFrame);

        // checking("Test 4.02 Change Sprite image: same frames, custom sequence");
        // Check The current frame will be unchanged
        // Check If a custom frame sequence has been defined, it will remain
        // unchanged.
        sprite = createSprite(iImages[0], iFrameWidth12, iFrameHeight12,
                              iCustomSeq12, currentFrame);
        sprite.setImage(iImages[2], iFrameWidth12, iFrameHeight12);
        checkFrames(sprite, iCustomSeq12, currentFrame);

        // checking("Test 4.03 Change Sprite image: more frames, default sequence");
        // Check The current frame will be unchanged
        // Check If no custom frame sequence is defined, the default frame
        // sequence
        // will be updated to be the default frame sequence for the new frame
        // set
        sprite = createSprite(iImages[0], iFrameWidth12, iFrameHeight12, null,
                              currentFrame);
        sprite.setImage(iImages[3], iFrameWidth24, iFrameHeight24);
        checkFrames(sprite, iDefaultSeq24, currentFrame);

        // checking("Test 4.04 Change Sprite image: more frames, custom sequence");
        // Check The current frame will be unchanged
        // Check If a custom frame sequence has been defined, it will remain
        // unchanged.
        sprite = createSprite(iImages[1], iFrameWidth12, iFrameHeight12,
                              iCustomSeq12, currentFrame);
        sprite.setImage(iImages[2], iFrameWidth24, iFrameHeight24);
        checkFrames(sprite, iCustomSeq12, currentFrame);

        // checking("Test 4.05 Change Sprite image: less frames, default sequence");
        // Check The current frame will be reset to entry 0
        // Check revert to the default frame sequence for the new frame set.
        sprite = createSprite(iImages[1], iFrameWidth12, iFrameHeight12, null,
                              currentFrame);
        sprite.setImage(iImages[3], iFrameWidth6, iFrameHeight6);
        checkFrames(sprite, iDefaultSeq6, 0);

        // checking("Test 4.06 Change Sprite image: less frames, custom sequence");
        // Check The current frame will be reset to entry 0
        // Check default sequence updated
        sprite = createSprite(iImages[2], iFrameWidth12, iFrameHeight12,
                              iCustomSeq12, currentFrame);
        sprite.setImage(iImages[3], iFrameWidth6, iFrameHeight6);
        checkFrames(sprite, iDefaultSeq6, 0);
    }

    private Sprite createSprite(Image aImage, int aFrameWidth,
                                int aFrameHeight, int[] aCustomSeq, int aCurrentFrame)
    {
        Sprite sprite = new Sprite(aImage, aFrameWidth, aFrameHeight);
        if(aCustomSeq != null)
            sprite.setFrameSequence(aCustomSeq);
        sprite.setFrame(aCurrentFrame);
        return sprite;
    }

    // checks current frame and frame sequence length
    private void checkFrames(Sprite sprite, int[] expectedSeq,
                             int expectedCurrentFrame)
    {
        assertEquals("Current frame", expectedCurrentFrame, sprite.getFrame());
        assertEquals("getFrameSequenceLength()", expectedSeq.length, sprite
                     .getFrameSequenceLength());
    }

    /**
     * Utility methods used by all tests
     */
    private void printRGB(int[] aData, int aWidth, int aHeight)
    {
        // checking("printRGB:");
        StringBuffer sb = new StringBuffer(aWidth);
        for(int i = 0; i < aHeight; i++)
        {
            sb.setLength(0);
            for(int j = 0; j < aWidth; j++)
            {
                int ind = i * aWidth + j;
                String cha = " ";
                switch(aData[ind])
                {
                case 0xFFFFFFFF:
                    cha = "W";
                    break; // opaque white
                case 0x00FFFFFF:
                    cha = "w";
                    break; // trans white
                case 0xFF000000:
                    cha = "B";
                    break; // opaque black
                case 0x00000000:
                    cha = "b";
                    break; // trans black
                case 0xFFFF0000:
                    cha = "R";
                    break; // opaque red
                case 0xFF00FF00:
                    cha = "G";
                    break; // opaque green
                case 0xFF0000FF:
                    cha = "Bl";
                    break;// opaque blue
                default:
                {
                    sb.append("[" + Integer.toHexString(aData[ind]) + "]");
                }
                }
                sb.append(cha);
            }
            // checking(sb.toString());
        }
    }

}
