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
package com.nokia.openlcdui.mt.item;

import junit.framework.*;

import javax.microedition.lcdui.*;
import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * TEST CASE SPECIFICATION
 *
 * <br><br>Short description of the module test:
 * <li> Collection of tests to test Item's functionality.
 *
 * <br><br>
 * Created:    2008-06-06
 */
public class ItemTest extends SWTTestCase implements ItemStateListener
{

    private static final int NOTIFY_STATE_CHANGED_DELAY = 100;

    private static final int INVALID_ITEM_LAYOUT = 4;

    private static final int PREFERRED_WIDTH = 150;
    private static final int PREFERRED_HEIGHT = 100;
    private static final int BIG_PREFERRED_WIDTH = 25000;
    private static final int BIG_PREFERRED_HEIGHT = 12000;
    private static final int INVALID_PREFERRED_WIDTH = -127;
    private static final int INVALID_PREFERRED_HEIGHT = -2;

    private Display display;

    private Item latestStateListenerItem;

    /**
     * Constructor.
     */
    public ItemTest()
    {
    }

    /**
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public ItemTest(String sTestName)
    {
        super(sTestName);
    }

    /**
     * Any pre-test setup can be done here
     */
    protected void setUp() throws Exception
    {
        display = Display.getDisplay(getMIDlet());
    }

    /**
     * Creates the test suite. You need to add a new aSuite.addTest antry for
     * any new test methods, otherwise they won't be run.
     * @return New testsuite.
     */
    public static Test suite()
    {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
        java.util.Enumeration e;

        // Add widget tests
        methodNames = ItemTest.methodNames();
        e = methodNames.elements();
        while(e.hasMoreElements())
        {
            suite.addTest(new ItemTest((String)e.nextElement()));
        }

        return suite;
    }

    public static java.util.Vector methodNames()
    {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testLayoutAccessors");
        methodNames.addElement("testCommandAccessors");
        methodNames.addElement("testLabelAccessors");
        methodNames.addElement("testDimensions");
        methodNames.addElement("testNotifyStateChanged");
        methodNames.addElement("testMinimumSizes");
        return methodNames;
    }

    protected void runTest() throws Throwable
    {
        if(getName().equals("testLayoutAccessors")) testLayoutAccessors();
        else if(getName().equals("testCommandAccessors")) testCommandAccessors();
        else if(getName().equals("testLabelAccessors")) testLabelAccessors();
        else if(getName().equals("testDimensions")) testDimensions();
        else if(getName().equals("testNotifyStateChanged")) testNotifyStateChanged();
        else if(getName().equals("testMinimumSizes")) testMinimumSizes();
        else super.runTest();
    }

    /**
     * Tests the basic functionality of the layout accessor methods.
     */
    public void testLayoutAccessors()
    {
        boolean testPassed = true;
        String testMsg = "";
        boolean exceptionThrown = false;

        StringItem item = new StringItem("", "");

        //Set correct layout and verify that same layout is returned
        //from getter:
        item.setLayout(Item.LAYOUT_LEFT | Item.LAYOUT_VCENTER);
        if(item.getLayout() != (Item.LAYOUT_LEFT | Item.LAYOUT_VCENTER))
        {
            testPassed = false;
            testMsg = " invalid layout returned.";
        }

        //Set incorrect layout and verify that exception is thrown:
        try
        {
            item.setLayout(INVALID_ITEM_LAYOUT);
        }
        catch(IllegalArgumentException ex)
        {
            exceptionThrown = true;
        }

        if(!exceptionThrown)
        {
            testPassed = false;
            testMsg = " No exception when setting invalid layout.";
        }

        //Verify the layout is not changed:
        if(item.getLayout() != (Item.LAYOUT_LEFT | Item.LAYOUT_VCENTER))
        {
            testPassed = false;
            testMsg = " invalid layout returned after exception.";
        }

        assertTrue(getName() + " failed, " + testMsg, testPassed);
    }

    /**
     * Tests basic functionality of command-related accessors
     * methods. Command adding and removing should be tested
     * in ui-robot tests more carefully. Here is just made sure that
     * invalid or valid calls won't break anything and exceptions
     * are thrown when expected.
     */
    public void testCommandAccessors()
    {
        boolean testPassed = true;
        String testMsg = "";
        boolean exceptionThrown = false;

        StringItem item = new StringItem("", "");
        Command c1 = new Command("label1", null, Command.ITEM, 0);
        Command c2 = new Command("label2", null, Command.ITEM, 0);
        Command c3 = new Command("label3", null, Command.ITEM, 0);

        //Try to add null command:
        try
        {
            item.addCommand(null);
        }
        catch(NullPointerException ex)
        {
            exceptionThrown = true;
        }
        if(!exceptionThrown)
        {
            testPassed = false;
            testMsg = " No exception when adding null command.";
        }

        //Try to add command:
        item.addCommand(c1);

        //Try to add already existing command:
        item.addCommand(c1);

        //Try to add multiple commands:
        item.addCommand(c2);

        //Try to remove null command:
        item.removeCommand(null);

        //Try to remove non-existing command:
        item.removeCommand(c3);

        //Try to remove command:
        item.removeCommand(c1);

        //Try to set null default command when default command
        //doesn't exist:
        item.setDefaultCommand(null);

        //Try to set existing command to default command:
        item.setDefaultCommand(c2);

        //Try to set non-existing command to default command:
        item.setDefaultCommand(c3);

        //Try to remove default command:
        item.setDefaultCommand(null);

        assertTrue(getName() + " failed, " + testMsg, testPassed);
    }

    /**
     * Tests basic functionality of set and get label methods.
     * It would be useful to run test like this for all subclasses
     * of item because label is used in different eSWT components and
     * behaviour may vary when using null as a label.
     */
    public void testLabelAccessors()
    {
        boolean testPassed = true;
        String testMsg = "";

        StringItem item = new StringItem("", "");

        //Try to set label:
        item.setLabel("test label 1");

        //Try to get label and verify it's the one previously set:
        if(!item.getLabel().equals("test label 1"))
        {
            testPassed = false;
            testMsg = " Invalid label returned.";
        }

        //Try to set null label and verify it is null when queried:
        item.setLabel(null);
        if(item.getLabel() != null)
        {
            testPassed = false;
            testMsg = " non-null label returned.";
        }

        assertTrue(getName() + " failed, " + testMsg, testPassed);
    }

    /**
     * Tests the basic functionality of item dimension related methods.
     */
    public void testDimensions()
    {
        boolean testPassed = true;
        String testMsg = "";
        boolean exceptionThrown = false;

        StringItem item = new StringItem("123", "456");

        //Call setPreferredSize with illegal values and verify that
        //exception is thrown:
        try
        {
            item.setPreferredSize(INVALID_PREFERRED_WIDTH, 0);
        }
        catch(IllegalArgumentException ex)
        {
            exceptionThrown = true;
        }
        if(!exceptionThrown)
        {
            testPassed = false;
            testMsg = " No exception when setting preferred size with"
                      + " invalid width";
        }
        try
        {
            item.setPreferredSize(0, INVALID_PREFERRED_HEIGHT);
        }
        catch(IllegalArgumentException ex)
        {
            exceptionThrown = true;
        }
        if(!exceptionThrown)
        {
            testPassed = false;
            testMsg = " No exception when setting preferred size with"
                      + " invalid height";
        }

        //Lock preferred width and height and verify that correct
        //values are returned when queried:
        //(Note that system is allowed to use some other values than
        //set here but this test assumes that values returned are same)
        item.setPreferredSize(PREFERRED_WIDTH, PREFERRED_HEIGHT);

        if(item.getPreferredWidth() != PREFERRED_WIDTH)
        {
            testPassed = false;
            testMsg = " Invalid preferred width returned.";
        }

        if(item.getPreferredHeight() != PREFERRED_HEIGHT)
        {
            testPassed = false;
            testMsg = " Invalid preferred height returned.";
        }

        // Set item's preferred size very big and make sure the
        // width is not that big. (There should be limit for maximum
        // width but no limit for height)
        item.setPreferredSize(BIG_PREFERRED_WIDTH,
                              BIG_PREFERRED_HEIGHT);

        if(item.getPreferredWidth() == BIG_PREFERRED_WIDTH)
        {
            testPassed = false;
            testMsg = " Too big preferred width allowed.";
        }

        assertTrue(getName() + " failed, " + testMsg, testPassed);
    }

    /**
     * Test the functionality of the method notifyStateChanged().
     * After each notifyStateChanged()-call there's short delay
     * because the method is asynchronous.
     *
     */
    public void testNotifyStateChanged()
    {
        boolean testPassed = true;
        String testMsg = "";
        boolean exceptionThrown = false;

        StringItem item = new StringItem("123", "456");

        //Make sure that the method under test throws exception
        //if called when item is not owned by a form.
        try
        {
            item.notifyStateChanged();
        }
        catch(IllegalStateException ex)
        {
            exceptionThrown = true;
        }
        if(!exceptionThrown)
        {
            testPassed = false;
            testMsg = " No exception when calling notifyStateChanged"
                      + " and item is not owned by a form.";
        }

        assertTrue(getName() + " failed, " + testMsg, testPassed);

        //Test method when state listener in form is null:
        latestStateListenerItem = null;
        Form form = new Form("form");
        form.append(item);
        item.notifyStateChanged();
        //block(NOTIFY_STATE_CHANGED_DELAY);

        assertNull("notifyStateChanged() called listener but listener not"
                   + "added to Form.", latestStateListenerItem);

        //Add listener and then test method again:
        form.setItemStateListener(this);
        item.notifyStateChanged();
        block(NOTIFY_STATE_CHANGED_DELAY);

        assertEquals("notifyStateChanged() failed.",
                     item, latestStateListenerItem);
    }

    /**
     * Tests the functionality of the methods getMinimumHeight()
     * and getMinimumWidth().
     */
    public void testMinimumSizes()
    {
        int w;
        int h;

        //Test ImageItem with null image and without label:
        ImageItem imageItem1 = new ImageItem(null, null, 0, "altText");
        w = imageItem1.getMinimumWidth();
        h = imageItem1.getMinimumHeight();
        //If image is null, then the values should be same as returned from
        //Display's getBestImageWidth/Height (+ label if present):
        assertEquals(
            "ImageItem with null image returned invalid minimum width.",
            w, display.getBestImageWidth(Display.CHOICE_GROUP_ELEMENT));
        assertEquals(
            "ImageItem with null image returned invalid minimum height.",
            h, display.getBestImageHeight(Display.CHOICE_GROUP_ELEMENT));
        //print("Minimum size of null ImageItem without label and "
        //        + "with altText was: w=" + w + ", h=" + h);

        //Test ImageItem with image:
        Image image = null;
        try
        {
            image = Image.createImage("100x100.png");
        }
        catch(Exception e)
        {
            fail("Exception - " + e.getMessage());
        }
        ImageItem imageItem2 = new ImageItem("label", image, 0, "altText");
        w = imageItem2.getMinimumWidth();
        h = imageItem2.getMinimumHeight();
        //print("Minimum size of 100x100 ImageItem with label was: w="
        //        + w + ", h=" + h);
        assertTrue("Width not greater than zero.", w > 0);
        assertTrue("Height not greater than zero.", h > 0);

        //Test ImageItem with image and without label:
        ImageItem imageItem3 = new ImageItem(null, image, 0, "altText");
        w = imageItem3.getMinimumWidth();
        h = imageItem3.getMinimumHeight();
        //print("Minimum size of 100x100 ImageItem without label was: w="
        //        + w + ", h=" + h);
        assertTrue("Width not greater than zero.", w > 0);
        assertTrue("Height not greater than zero.", h > 0);


        //Test StringItem without label:
        StringItem stringItem1 = new StringItem(null, "text");
        w = stringItem1.getMinimumWidth();
        h = stringItem1.getMinimumHeight();
        //print("Minimum size of StringItem without label was: w="
        //        + w + ", h=" + h);
        assertTrue("Width not greater than zero.", w > 0);
        assertTrue("Height not greater than zero.", h > 0);


        //Test StringItem with label:
        StringItem stringItem2 = new StringItem("label", "text");
        w = stringItem2.getMinimumWidth();
        h = stringItem2.getMinimumHeight();
        //print("Minimum size of StringItem with label was: w="
        //        + w + ", h=" + h);
        assertTrue("Width not greater than zero.", w > 0);
        assertTrue("Height not greater than zero.", h > 0);

    }


    /**
     * Receives events when item's state has changed.
     * @param item Item which state has changed.
     */
    public void itemStateChanged(Item item)
    {
        latestStateListenerItem = item;
    }
}
