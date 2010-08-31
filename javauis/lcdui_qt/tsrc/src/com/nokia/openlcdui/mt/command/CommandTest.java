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
package com.nokia.openlcdui.mt.command;

import junit.framework.*;

import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.TextBox;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * TEST CASE SPECIFICATION
 *
 * <br><br>Short description of the module test:
 * <li>Collection of tests to test Command's functionality.
 *
 * <br><br>
 * Created:    2008-05-02
 *
 */
public class CommandTest extends SWTTestCase
{

    private static final int MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX = 100;
    private static final int TEST_INVALID_TYPE = 43242;
    private static final int TEST_PRIORITY_1 = -54532;
    private static final int TEST_PRIORITY_2 = 4403232;
    private static final String DEFAULT_BACK_COMMAND = "Back";

    /**
     * Constructor.
     */
    public CommandTest()
    {
    }

    /**
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public CommandTest(String sTestName)
    {
        super(sTestName);
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
        methodNames = CommandTest.methodNames();
        e = methodNames.elements();
        while(e.hasMoreElements())
        {
            suite.addTest(new CommandTest((String)e.nextElement()));
        }

        return suite;
    }

    public static java.util.Vector methodNames()
    {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testConstructors");
        methodNames.addElement("testAccessors");
        methodNames.addElement("testNegativePriority");
        methodNames.addElement("testEmptyLabel");
        return methodNames;
    }

    protected void runTest() throws Throwable
    {
        if(getName().equals("testConstructors")) testConstructors();
        else if(getName().equals("testAccessors")) testAccessors();
        else if(getName().equals("testNegativePriority")) testNegativePriority();
        else if(getName().equals("testEmptyLabel")) testEmptyLabel();
        else super.runTest();
    }


    /**
     * Tests the functionality of constructors.
     */
    public void testConstructors()
    {
        // Try to create Command with null label:
        try
        {
            new Command(null, Command.OK, 0);
            fail("Command constructor didn't throw NullPointerException when label was null.");
        }
        catch(NullPointerException ex)
        {
        }

        // Try to create Command with null short label:
        try
        {
            new Command(null, "long label", Command.OK, 0);
            fail("Command constructor didn't throw NullPointerException when short label was null.");
        }
        catch(NullPointerException ex)
        {
        }

        // Try to create Command with illegal type:
        try
        {
            new Command("label", TEST_INVALID_TYPE, 0);
            fail("Command constructor didn't throw IllegalArgumentException when type was illegal.");
        }
        catch(IllegalArgumentException ex)
        {
        }
    }

    /**
     * Tests the functionality of basic accessors methods.
     */
    public void testAccessors()
    {
        String label1 = "label1";
        String longLabel1 = "long label 1";
        int type1 = Command.BACK;
        int priority1 = TEST_PRIORITY_1;

        String label2 = "label2";
        String longLabel2 = "long label 2 is very long label";
        int type2 = Command.ITEM;
        int priority2 = 0;

        String label3 = "label3";
        String longLabel3 = null;
        int type3 = Command.HELP;
        int priority3 = TEST_PRIORITY_2;

        Command c1 = new Command(label1, longLabel1, type1, priority1);
        Command c2 = new Command(label2, longLabel2, type2, priority2);
        Command c3 = new Command(label3, longLabel3, type3, priority3);

        assertEquals("c1.getLabel() returned wrong value.", label1, c1.getLabel());
        assertEquals("c2.getLabel() returned wrong value.", label2, c2.getLabel());
        assertEquals("c3.getLabel() returned wrong value.", label3, c3.getLabel());

        assertEquals("c1.getLongLabel() returned wrong value.", longLabel1, c1.getLongLabel());
        assertEquals("c2.getLongLabel() returned wrong value.", longLabel2, c2.getLongLabel());
        assertEquals("c3.getLongLabel() returned wrong value.", longLabel3, c3.getLongLabel());

        assertEquals("c1.getCommandType() returned wrong value.", type1, c1.getCommandType());
        assertEquals("c2.getCommandType() returned wrong value.", type2, c2.getCommandType());
        assertEquals("c3.getCommandType() returned wrong value.", type3, c3.getCommandType());

        assertEquals("c1.getPriority() returned wrong value.", priority1, c1.getPriority());
        assertEquals("c2.getPriority() returned wrong value.", priority2, c2.getPriority());
        assertEquals("c3.getPriority() returned wrong value.", priority3, c3.getPriority());
    }

    /**
     * Tests that negative priority value doesn't crash the application. Also
     * uses maximum and minimum priority values to make sure that those values
     * are handled without problems. This is important to test because OpenLCDUI
     * is built on top of eSWT which doesn't allow negative priority values.
     */
    public void testNegativePriority()
    {
        TextBox textBox = new TextBox(getName(), "content", MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX, 0);
        Command c1 = new Command("label1", null, Command.ITEM, -1);
        Command c2 = new Command("label2", null, Command.ITEM, Integer.MAX_VALUE);
        Command c3 = new Command("label3", null, Command.ITEM, Integer.MIN_VALUE);
        Command c4 = new Command("label4", null, Command.ITEM, 0);
        Command c5 = new Command("label5", null, Command.ITEM, Integer.MAX_VALUE - 1);
        Command c6 = new Command("label6", null, Command.ITEM, Integer.MIN_VALUE + 1);
        Command c7 = new Command("label7", null, Command.ITEM, 1);

        // OpenLCDUI implementation will create eSWT command only when
        // Command is added to displayable, and that's the place where
        // possible problems might occur:
        textBox.addCommand(c1);
        textBox.addCommand(c2);
        textBox.addCommand(c3);
        textBox.addCommand(c4);
        textBox.addCommand(c5);
        textBox.addCommand(c6);
        textBox.addCommand(c7);
    }

    /**
     * If Command is created with empty label, Command should use default label
     * instead of empty string. So this test creates a new Command with empty
     * label, and verifies the label is the default one.
     */
    public void testEmptyLabel()
    {
        Command c = new Command("", "", Command.BACK, 0);
        TextBox textBox = new TextBox(getName(), "content", MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX, 0);
        textBox.addCommand(c);
        assertEquals("c.getLabel() returned wrong value when label was empty.", DEFAULT_BACK_COMMAND, c.getLabel());
    }

}

