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

import javax.microedition.lcdui.*;
import javax.microedition.lcdui.game.GameCanvas;
import javax.microedition.lcdui.game.Sprite;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
* GameCanvas non-interactive test.
*/
public class GameCanvasTest extends SWTTestCase
{

    Sprite iSprite = null;
    int [] iRGB =
    {
        0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFFFFFFFF,
        0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000,
        0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000,
        0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFFFFFFFF
    };

    /**
     * Constructor.
     */
    public GameCanvasTest()
    {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public GameCanvasTest(String sTestName)
    {
        super(sTestName);
    }

    protected void setUp() throws Exception
    {
        super.setUp();
        iSprite = new Sprite(Image.createRGBImage(iRGB, 4, 4, true));
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
        methodNames = GameCanvasTest.methodNames();
        e = methodNames.elements();
        while(e.hasMoreElements())
        {
            suite.addTest(new GameCanvasTest((String)e.nextElement()));
        }

        return suite;
    }

    public static java.util.Vector methodNames()
    {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testGraphics");
        methodNames.addElement("testPaint");
        return methodNames;
    }

    protected void runTest() throws Throwable
    {
        if(getName().equals("testGraphics")) testGraphics();
        else if(getName().equals("testPaint")) testPaint();
        else super.runTest();
    }

    /**
    * Tests following methods:<br>
    * {@link javax.microedition.lcdui.game.GameCanvas#getGraphics}<br>
    * {@link javax.microedition.lcdui.game.GameCanvas#flushGraphics()}<br>
    * {@link javax.microedition.lcdui.game.GameCanvas#flushGraphics(int,int,int,int)}<br>
    */
    public void testGraphics()
    {
        GameCanvasWithoutKeys canvas = new GameCanvasWithoutKeys();
        Graphics g = canvas.getBufferGraphics();

        assertEquals("clipX should be 0", 0, g.getClipX());
        assertEquals("clipX should be 0", 0, g.getClipY());
        assertEquals("clipX should = width", canvas.getWidth(), g.getClipWidth());
        assertEquals("clipX should = height", canvas.getHeight(), g.getClipHeight());

        assertEquals("Current color should be black", 0, g.getColor());
        assertTrue("Font should be Font.getDefaultFont", (g.getFont())
                   .equals(Font.getDefaultFont()));

        assertEquals("Stroke should be SOLID", Graphics.SOLID, g.getStrokeStyle());

        assertEquals("translateX should be 0", 0, g.getTranslateX());
        assertEquals("translateY should be 0", 0, g.getTranslateY());
    }

    /**
    * Tests following methods:<br>
    * {@link javax.microedition.lcdui.game.GameCanvas#paint(javax.microedition.lcdui.Graphics)}<br>
    */
    public void testPaint()
    {

        // This test hangs (hang probably caused by GameCanvas command buffering), backlog item has been created.
        DISABLE_TEST();

        // how it works:
        // - GameCanvas has a buffer
        // - getGraphics gives you the buffer
        // - you paint into this graphics (that is to the buffer)
        // - GameCanvas.paint(Graphics) gets called by the sys or by you to
        // paint the buffer into supplied graphics

        // testing paint - using external mutable image as screen
        GameCanvasWithoutKeys canvas = new GameCanvasWithoutKeys();
        Image image = Image.createImage(canvas.getWidth(), canvas.getHeight());
        Graphics imageGraphics = image.getGraphics();
        int[] compare = iRGB;
        testPaint("paint without clip and translation", canvas, image, imageGraphics, compare);

        // testing paint - using external mutable image as screen
        canvas = new GameCanvasWithoutKeys();
        image = Image.createImage(canvas.getWidth(), canvas.getHeight());
        imageGraphics = image.getGraphics();
        // clip
        imageGraphics.setClip(0, 0, 1, 4); // first column is in, rest is out
        int[] compare2 =
        {
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
            0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
            0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
        };
        testPaint("paint with clip", canvas, image, imageGraphics, compare2);

        // testing paint - using external mutable image as screen
        canvas = new GameCanvasWithoutKeys();
        image = Image.createImage(canvas.getWidth(), canvas.getHeight());
        imageGraphics = image.getGraphics();
        // clip
        imageGraphics.translate(0, -1);
        int[] compare3 =
        {
            0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000,
            0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000,
            0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        };
        testPaint("paint with translation", canvas, image, imageGraphics, compare3);

        // testing paint - using external mutable image as screen
        canvas = new GameCanvasWithoutKeys();
        image = Image.createImage(canvas.getWidth(), canvas.getHeight());
        imageGraphics = image.getGraphics();
        // clip
        imageGraphics.translate(0, -1);
        imageGraphics.setClip(0, 0, 2, 4); // first two columns are in, rest is out
        int[] compare4 =
        {
            0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
            0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        };
        testPaint("paint with translation and clip",
                  canvas, image, imageGraphics, compare4);

        // Test paint when graphics is null
        MyGameCanvas gameCvs1 = new MyGameCanvas(false);
        Graphics g = null;
        String msg = "";

        try
        {
            gameCvs1.paint(g);
            msg = " NullPointerException wa not thrown when graphics was set to null";
            msg = "FAILED \n" + msg;
            fail(msg);
        }
        catch (NullPointerException ne)
        {
            //test pass
        }
    }

    /**
    * Checks the painted image against provided rgb array.
    *
    * The area of interest is four by four pixels located at 0,0.
    *
    * Note that the buffer background in the area of interest will be initialized
    * to white before painting {@link #iSprite} into it at 0,0. This should be
    * taken into account when generating the array for comparison.
    *
    * @param screenGraphics The graphics object to paint the GameCanvas to. This
    *                        graphics may have clip and translation set
    * @param screen         The image object that aScreenGraphics paints to.
    * @param checkRGB        An array of integers with expected values for the
    *                        area of interest.
    */
    private void testPaint(String msg, GameCanvasWithoutKeys canvas,
                           Image screen, Graphics screenGraphics, int[] expectedRGB)
    {

        Graphics bufferGraphics = canvas.getBufferGraphics();

        // paint into gameCanvas buffer
        int color = bufferGraphics.getColor();
        bufferGraphics.setColor(0x00FFFFFF);
        bufferGraphics.fillRect(0, 0, 4, 4);
        bufferGraphics.setColor(color);

        iSprite.paint(bufferGraphics);
        canvas.flushGraphics();
        canvas.paint(screenGraphics);

        // getRGB will sync graphics
        int[] actualRGB = new int[16];
        screen.getRGB(actualRGB, 0, 4, 0, 0, 4, 4);

        // compare to the sprite image
        for(int i = 0; i < 16; i++)
        {
            if(actualRGB[i] != expectedRGB[i])
            {
                printRGB(actualRGB, 4, 4);
                printRGB(expectedRGB, 4, 4);
                fail(msg);
            }
        }
    }

    /**
     * Prints out the image to standard output. If an image contains unexpected
     * colours, i.e. not those used to form images, this method will throw an
     * exception. This behaviour provides protection against hiding colour
     * discretization problem and reporting test error.
     */
    private void printRGB(int[] data, int w, int h)
    {
        int odd = 0;
        for(int i = 0; i < h; i++)
        {
            for(int j = 0; j < w; j++)
            {
                int ind = i * w + j;
                String cha = " ";
                switch(data[ind])
                {
                case 0xFFFFFFFF:
                    cha = "W";
                    break;
                case 0x00FFFFFF:
                    cha = "w";
                    break;
                case 0xFF000000:
                    cha = "B";
                    break;
                case 0x00000000:
                    cha = "b";
                    break;
                case 0xFF888888:
                    cha = ".";
                    break;
                case 0x00888888:
                    cha = ",";
                    break;
                default:
                {
                    cha = "U";
                    odd = data[ind];
                }
                }
                System.out.print(cha);
            }
            System.out.println("");
        }
        System.out.println("");
        if(odd != 0)
        {
            fail("Unexpected colour in test image : 0x" + Integer.toHexString(odd));
        }
    }

    class GameCanvasWithKeys extends GameCanvas
    {

        public GameCanvasWithKeys()
        {
            super(false);
        }

        public Graphics getTheGraphics()
        {
            return getGraphics();
        }
    }

    class GameCanvasWithoutKeys extends GameCanvas
    {

        public GameCanvasWithoutKeys()
        {
            super(true);
        }

        public Graphics getBufferGraphics()
        {
            return getGraphics();
        }
    }

    class MyGameCanvas extends GameCanvas 
    {

        public MyGameCanvas(boolean s) 
        {
            super(s);
        }

        public Graphics getGraphics() 
        {
            return super.getGraphics();
        }
    }
}
