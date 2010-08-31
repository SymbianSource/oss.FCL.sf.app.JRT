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
package com.nokia.openlcdui.mt.displayable;

import junit.framework.*;

import javax.microedition.lcdui.*;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> Collection of tests to test Displayable's functionality. <br>
 * <br>
 * Created: 2008-04-21
 */
public class DisplayableTest extends SWTTestCase
{

    private static final int MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX = 100;

    private Display display;

    /**
     * Constructor.
     */
    public DisplayableTest()
    {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public DisplayableTest(String sTestName)
    {
        super(sTestName);
    }

    protected void setUp() throws Exception
    {
        display = Display.getDisplay(getMIDlet());
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
        methodNames = DisplayableTest.methodNames();
        e = methodNames.elements();
        while(e.hasMoreElements())
        {
            suite.addTest(new DisplayableTest((String)e.nextElement()));
        }

        return suite;
    }

    public static java.util.Vector methodNames()
    {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testAddNullCommand");
        methodNames.addElement("testRemoveCommandCrash");
        methodNames.addElement("testAccessors");
        methodNames.addElement("testNullAndEmptyTitle");
        methodNames.addElement("testAddRemoveCommandWithoutListener");
        return methodNames;
    }

    protected void runTest() throws Throwable
    {
        if(getName().equals("testAddNullCommand")) testAddNullCommand();
        else if(getName().equals("testRemoveCommandCrash")) testRemoveCommandCrash();
        else if(getName().equals("testAccessors")) testAccessors();
        else if(getName().equals("testNullAndEmptyTitle")) testNullAndEmptyTitle();
        else if(getName().equals("testAddRemoveCommandWithoutListener")) testAddRemoveCommandWithoutListener();
        else super.runTest();
    }


    /**
     * Test that addCommand throws NullPointerException if parameter is null.
     */
    public void testAddNullCommand()
    {
        TextBox textBox = new TextBox(getName(), "content",
                                      MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX, 0);

        try
        {
            textBox.addCommand(null);
            fail("NullPointerException should be thrown");
        }
        catch(NullPointerException ex)
        {
            // Ok
        }
    }

    /**
     * Make some tests for invalid operations on Commands.
     */
    public void testRemoveCommandCrash()
    {
        TextBox textBox = new TextBox(getName(), "content",
                                      MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX, 0);
        Command c1 = new Command("item1", Command.ITEM, 0);
        Command c2 = new Command("item2", Command.ITEM, 0);

        // Test that nothing happens when removeCommand is called with null
        // parameter and there are no commands added:
        textBox.removeCommand(null);

        // Test that nothing happens when removeCommand is called but parameter
        // doesn't match any existing command because there are no commands:
        textBox.removeCommand(c1);

        // Test that nothing happens when removeCommand is called but parameter
        // doesn't match any existing command:
        textBox.addCommand(c1);
        textBox.removeCommand(c2);

        // Test that nothing happens when removeCommand is called with null
        // parameter and there are commands added:
        textBox.removeCommand(null);
    }

    /**
     * Tests the basic functionality of the accessor methods.
     */
    public void testAccessors()
    {
        TextBox textBox = new TextBox(getName(), "content",
                                      MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX, 0);

        Ticker ticker = new Ticker("test ticker");

        // No ticker should exist by default:
        if(textBox.getTicker() != null)
        {
            fail("getTicker() returned non-null value by default.");
        }

        // Test to set ticker and verify that the correct ticker is returned
        // when queried.
        textBox.setTicker(ticker);
        if(textBox.getTicker() != ticker)
        {
            fail("getTicker() returned null or wrong ticker.");
        }

        // Test title set in constructor:
        if(!textBox.getTitle().equals(getName()))
        {
            fail("Invalid initial title returned.");
        }

        // Test to set title and verify results:
        textBox.setTitle("newTitle");
        if(!textBox.getTitle().equals("newTitle"))
        {
            fail("Invalid title returned.");
        }

        // Test to remove title and verify results:
        textBox.setTitle(null);
        if(textBox.getTitle() != null)
        {
            fail("title not removed.");
        }

        // Set displayable current and make sure the method isShown()
        // returns correct value:
        display.setCurrent(textBox);
        block(200);

        if(!textBox.isShown())
        {
            fail("isShown() returned false even when displayable is visible.");
        }
        textBox.setTicker(null);
    }

    /**
     * Tests to set and get null and empty string to title and verifies that
     * returned value is always what was set.
     */
    public void testNullAndEmptyTitle()
    {
        TextBox textBox = new TextBox(getName(), "content",
                                      MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX, 0);

        // Use empty string as title and verify getTitle()'s return value:
        textBox.setTitle("");
        if(!textBox.getTitle().equals(""))
        {
            fail("failed to use empty string as title.");
        }

        // Test to set title to null and verify results:
        textBox.setTitle(null);
        if(textBox.getTitle() != null)
        {
            fail("title not null when expected.");
        }
    }

    /**
     * Test to add command to displayable and then remove it without setting
     * command listener before or after.
     */
    public void testAddRemoveCommandWithoutListener()
    {
        TextBox textBox = new TextBox(getName(), "content",
                                      MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX, 0);

        Command c1 = new Command("CommandForDisplayable", Command.CANCEL, 1);

        textBox.addCommand(c1);
        textBox.removeCommand(c1);
    }

}

