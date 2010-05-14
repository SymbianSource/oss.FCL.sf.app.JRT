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
package com.nokia.openlcdui.mt.stringitem;

import junit.framework.*;

import javax.microedition.lcdui.*;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * TEST CASE SPECIFICATION
 *
 * <br><br>Short description of the module test:
 * <li> Collection of tests to test StringItem's functionality.
 *
 * <br><br>
 * Created:    2008-06-09
 */
public class StringItemTest extends SWTTestCase
{

    private static final int INVALID_APPEARANCE_MODE = 3;

    /**
     * Constructor.
     */
    public StringItemTest()
    {
    }

    /**
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public StringItemTest(String sTestName)
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
     * @return New testsuite.
     */
    public static Test suite()
    {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
        java.util.Enumeration e;

        // Add widget tests
        methodNames = StringItemTest.methodNames();
        e = methodNames.elements();
        while(e.hasMoreElements())
        {
            suite.addTest(new StringItemTest((String)e.nextElement()));
        }

        return suite;
    }

    public static java.util.Vector methodNames()
    {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testAccessors");
        methodNames.addElement("testMinimumSize");
        return methodNames;
    }

    protected void runTest() throws Throwable
    {
        if(getName().equals("testAccessors")) testAccessors();
        else if(getName().equals("testMinimumSize")) testMinimumSize();
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

        //Create new StringItem with invalid appearance mode
        //and check that exception is thrown:
        try
        {
            StringItem item = new StringItem("", "", INVALID_APPEARANCE_MODE);
        }
        catch(IllegalArgumentException ex)
        {
            exceptionThrown = true;
        }

        if(!exceptionThrown)
        {
            testPassed = false;
            testMsg = " No exception when creating StringItem with"
                      + " invalid appearance mode.";
        }

        //Create different kind of StringItems:
        StringItem item2 = new StringItem(null, null);
        StringItem item3 = new StringItem(null, "testText");
        StringItem item4 = new StringItem("testLabel", null);
        StringItem item5 = new StringItem("testLabel", "testText");

        //Verify getText() after constructor:
        if(item2.getText() != null)
        {
            testPassed = false;
            testMsg = " getText() didn't return null when expected.";
        }

        if(!item3.getText().equals("testText"))
        {
            testPassed = false;
            testMsg = " getText() didn't return correct value.";
        }

        //Set text and verify getText():
        item2.setText("testText2");
        if(!item2.getText().equals("testText2"))
        {
            testPassed = false;
            testMsg = " getText() didn't return correct value after setText().";
        }

        //Set null text and verify getText():
        item2.setText(null);
        if(item2.getText() != null)
        {
            testPassed = false;
            testMsg = " getText() didn't return correct value after setText()"
                      + " with null parameter. Instead it returned \""
                      + item2.getText() + "\".";
        }

        //Make sure that getAppearanceMode returns correct value:
        if(item2.getAppearanceMode() != Item.PLAIN)
        {
            testPassed = false;
            testMsg = " getAppearanceMode() returned incorrect value.";
        }

        //Get font and make sure it is not null:
        if(item2.getFont() == null)
        {
            testPassed = false;
            testMsg = " font was null by default.";
        }

        //Set null font and then verify that font returned is not null
        //(in that case StringItem should use default font instead of null):
        item2.setFont(null);
        if(item2.getFont() == null)
        {
            testPassed = false;
            testMsg = " font was null when default expected.";
        }

        assertTrue(getName() + " failed, " + testMsg, testPassed);
    }


    /**
     * Tests the minimum size of different type of StringItems.
     */
    public void testMinimumSize()
    {
        StringItem stringItem1 = new StringItem(
            "label", "text", StringItem.PLAIN);
        StringItem stringItem2 = new StringItem(null, "text", StringItem.PLAIN);
        StringItem stringItem3 = new StringItem(
            null, "text", StringItem.BUTTON);
        stringItem3.addCommand(new Command("c", Command.ITEM, 0));
        StringItem stringItem4 = new StringItem(
            null, "text", StringItem.HYPERLINK);
        stringItem4.addCommand(new Command("c", Command.ITEM, 0));
        StringItem stringItem5 = new StringItem(
            null, "very long hyperlink text that should be wrapped to many"
            + " lines.", StringItem.HYPERLINK);
        stringItem5.addCommand(new Command("c", Command.ITEM, 0));

        assertTrue("Minimum height was zero or negative, case 1.",
                   stringItem1.getMinimumHeight() > 0);
        //print("Plain with label, minimum height: "
        //        + stringItem1.getMinimumHeight());
        assertTrue("Minimum width was zero or negative, case 1.",
                   stringItem1.getMinimumWidth() > 0);
        //print("Plain with label, minimum width: "
        //        + stringItem1.getMinimumWidth());

        assertTrue("Minimum height was zero or negative, case 2.",
                   stringItem2.getMinimumHeight() > 0);
        //print("Plain without label, minimum height: "
        //        + stringItem2.getMinimumHeight());
        assertTrue("Minimum width was zero or negative, case 2.",
                   stringItem2.getMinimumWidth() > 0);
        //print("Plain without label, minimum width: "
        //        + stringItem2.getMinimumWidth());

        assertTrue("Minimum height was zero or negative, case 3.",
                   stringItem3.getMinimumHeight() > 0);
        //print("Button, minimum height: "
        //        + stringItem3.getMinimumHeight());
        assertTrue("Minimum width was zero or negative, case 3.",
                   stringItem3.getMinimumWidth() > 0);
        //print("Button, minimum width: "
        //        + stringItem3.getMinimumWidth());

        assertTrue("Minimum height was zero or negative, case 4.",
                   stringItem4.getMinimumHeight() > 0);
        //print("Short hyperlink, minimum height: "
        //        + stringItem4.getMinimumHeight());
        assertTrue("Minimum width was zero or negative, case 4.",
                   stringItem4.getMinimumWidth() > 0);
        //print("Short hyperlink, minimum width: "
        //        + stringItem4.getMinimumWidth());

        assertTrue("Minimum height was zero or negative, case 5.",
                   stringItem5.getMinimumHeight() > 0);
        //print("Long hyperlink, minimum height: "
        //        + stringItem5.getMinimumHeight());
        assertTrue("Minimum width was zero or negative, case 5.",
                   stringItem5.getMinimumWidth() > 0);
        //print("Long hyperlink, minimum width: "
        //        + stringItem5.getMinimumWidth());
    }
}
