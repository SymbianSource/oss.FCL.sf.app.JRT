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
import javax.microedition.lcdui.game.Sprite;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * Layer non-interactive tests.<br>
 * Tests Layer methods visible through Sprite.
 */
public class LayerTest extends SWTTestCase
{

    Image iImage = null;

    Sprite iSprite = null;

    /**
     * Constructor.
     */
    public LayerTest()
    {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public LayerTest(String sTestName)
    {
        super(sTestName);
    }

    protected void setUp() throws Exception
    {
        super.setUp();
        int[] rgb =
        {
            0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFFFFFFFF,
            0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000,
            0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000,
            0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFFFFFFFF
        };
        iImage = Image.createRGBImage(rgb, 4, 4, true);
        iSprite = new Sprite(iImage);
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
        methodNames = LayerTest.methodNames();
        e = methodNames.elements();
        while(e.hasMoreElements())
        {
            suite.addTest(new LayerTest((String)e.nextElement()));
        }

        return suite;
    }

    public static java.util.Vector methodNames()
    {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testSprites");
        methodNames.addElement("testVisible");
        return methodNames;
    }

    protected void runTest() throws Throwable
    {
        if(getName().equals("testSprites")) testSprites();
        else if(getName().equals("testVisible")) testVisible();
        else super.runTest();
    }


    public void testSprites()
    {
        // checking("Test 1.01 getWidth");
        assertEquals(4, iSprite.getWidth());

        // checking("Test 1.02 getHeight");
        assertEquals(4, iSprite.getHeight());

        // checking("Test 2.01 setPosition");
        iSprite.setPosition(5, 10);

        // checking("Test 2.02 getX");
        assertEquals(5, iSprite.getX());

        // checking("Test 2.03 getY");
        assertEquals(10, iSprite.getY());

        // checking("Test 2.04 move");
        iSprite.move(10, 5);

        // checking("Test 2.05 getX");
        assertEquals(15, iSprite.getX());

        // checking("Test 2.06 getY");
        assertEquals(15, iSprite.getY());

        // checking("Test 3.01 setVisible(true) / isVisible");
        iSprite.setVisible(true);
        assertTrue(iSprite.isVisible());

        // checking("Test 3.02 setVisible(false) / isVisible");
        iSprite.setVisible(false);
        assertTrue(!iSprite.isVisible());
    }

    /**
     * 4.0 visible test. This is in addition to
     */
    public void testVisible()
    {
        // checking("Test 4.0 set/isVisible test");
        iSprite = new Sprite(iImage);
        assertTrue("Is the sprite visible by default?", iSprite.isVisible());

        iSprite.setVisible(false);
        assertTrue("setVisible(false)", !iSprite.isVisible());

        iSprite.setVisible(true);
        assertTrue("setVisible(true)", iSprite.isVisible());
    }

}
