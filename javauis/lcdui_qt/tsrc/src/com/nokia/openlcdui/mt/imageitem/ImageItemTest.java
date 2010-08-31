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
package com.nokia.openlcdui.mt.imageitem;

import junit.framework.*;

import javax.microedition.lcdui.ImageItem;
import javax.microedition.lcdui.Item;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * TEST CASE SPECIFICATION
 *
 * <br><br>Short description of the module test:
 * <li> Collection of tests to test ImageItem's functionality.
 *
 * <br><br>
 * Created:    2008-06-09
 */
public class ImageItemTest extends SWTTestCase
{

    private static final int INVALID_APPEARANCE_MODE = 3;
    private static final int INVALID_LAYOUT = 4;

    /**
     * Constructor.
     */
    public ImageItemTest()
    {
    }

    /**
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public ImageItemTest(String sTestName)
    {
        super(sTestName);
    }

    /**
     * Any pre-test setup can be done here
     */
    protected void setUp() throws Exception
    {
    }

    /**
     * Creates the test suite. You need to add a new aSuite.addTest antry for
     * any new test methods, otherwise they won't be run.
     *
     * @return New testsuite.
     */
    public static Test suite()
    {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
        java.util.Enumeration e;

        // Add widget tests
        methodNames = ImageItemTest.methodNames();
        e = methodNames.elements();
        while(e.hasMoreElements())
        {
            suite.addTest(new ImageItemTest((String)e.nextElement()));
        }

        return suite;
    }

    public static java.util.Vector methodNames()
    {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testAccessors");
        return methodNames;
    }

    protected void runTest() throws Throwable
    {
        if(getName().equals("testAccessors")) testAccessors();
        else super.runTest();
    }

    /**
     * Tests the basic functionality of the accessor methods.
     */
    public void testAccessors()
    {
        boolean testPassed = true;
        String testMsg = "";
        boolean exceptionThrown = false;

        //Create new ImageItem with illegal layout directive:
        try
        {
            ImageItem item = new ImageItem(
                "", null, INVALID_LAYOUT, "", Item.PLAIN);
        }
        catch(IllegalArgumentException ex)
        {
            exceptionThrown = true;
        }

        if(!exceptionThrown)
        {
            testPassed = false;
            testMsg = " No exception when creating ImageItem with"
                      + " illegal layout directive.";
        }

        //Create new ImageItem with invalid appearance mode:
        exceptionThrown = false;
        try
        {
            ImageItem item = new ImageItem(
                "", null, Item.LAYOUT_DEFAULT, "", INVALID_APPEARANCE_MODE);
        }
        catch(IllegalArgumentException ex)
        {
            exceptionThrown = true;
        }

        if(!exceptionThrown)
        {
            testPassed = false;
            testMsg = " No exception when creating ImageItem with"
                      + " invalid appearance mode.";
        }

        //Create some new ImageItems with varied parameters:
        ImageItem item2 = new ImageItem(
            null, null, Item.LAYOUT_DEFAULT, "altText", Item.PLAIN);
        ImageItem item3 = new ImageItem(
            "label", null, Item.LAYOUT_EXPAND, null, Item.BUTTON);
        ImageItem item4 = new ImageItem(
            "label", null, Item.LAYOUT_CENTER | Item.LAYOUT_NEWLINE_AFTER,
            "altText", Item.HYPERLINK);

        //Try to get alt text and change it and get it again:
        if(item3.getAltText() != null)
        {
            testPassed = false;
            testMsg = " Alternate text not null when expected.";
        }
        item3.setAltText("altTestText");
        if(!item3.getAltText().equals("altTestText"))
        {
            testPassed = false;
            testMsg = " Invalid alternate text returned.";
        }

        //Try to get appearance mode:
        if(item3.getAppearanceMode() != Item.BUTTON)
        {
            testPassed = false;
            testMsg = " Invalid appearance mode returned.";
        }

        //Try to get layout directive:
        if(item4.getLayout()
                != (Item.LAYOUT_CENTER | Item.LAYOUT_NEWLINE_AFTER))
        {
            testPassed = false;
            testMsg = " Invalid layout returned.";
        }

        //Try to set layout and verify it is set correctly:
        item4.setLayout(Item.LAYOUT_RIGHT | Item.LAYOUT_SHRINK);
        if(item4.getLayout()
                != (Item.LAYOUT_RIGHT | Item.LAYOUT_SHRINK))
        {
            testPassed = false;
            testMsg = " Invalid layout returned after setting new layout.";
        }

        assertTrue(getName() + " failed, " + testMsg, testPassed);
    }
}
