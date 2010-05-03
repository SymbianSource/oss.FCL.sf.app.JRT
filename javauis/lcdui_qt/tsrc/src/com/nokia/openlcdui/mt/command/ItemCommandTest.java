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

import javax.microedition.lcdui.*;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * TEST CASE SPECIFICATION
 *
 * <br><br>Short description of the module test:
 * <li>Collection of tests to test Item Command's functionality.
 *
 * <br><br>
 * Created:    2008-08-04
 *
 */
public class ItemCommandTest extends SWTTestCase {

    /**
     * Constructor.
     */
    public ItemCommandTest() {
    }

    /**
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public ItemCommandTest(String sTestName) {
        super(sTestName);
    }

    /**
     * Creates the test suite. You need to add a new aSuite.addTest antry for
     * any new test methods, otherwise they won't be run.
     * @return New testsuite.
     */
    public static Test suite() {
    	TestSuite suite = new TestSuite();

	    java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add widget tests
	    methodNames = ItemCommandTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new ItemCommandTest((String)e.nextElement()));
	    }

		return suite;
    }

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testAddCommand");
        methodNames.addElement("testRemoveCommand");
        return methodNames;
    }
    
    protected void runTest() throws Throwable {
        if (getName().equals("testAddCommand")) testAddCommand();
        else if (getName().equals("testRemoveCommand")) testRemoveCommand();
        else super.runTest();
    }
    
    /**
     * Tests to add command to form item.
     */
    public void testAddCommand() {
        Form form = new Form("form");
        StringItem button1 = new StringItem("", "click!", Item.BUTTON);
        Command ok = new Command("Ok", "", Command.OK, 0);
        button1.addCommand(ok);
        form.append(button1);
    }

    /**
     * Tests to remove command from form item.
     */
    public void testRemoveCommand() {
        Form form = new Form("form");
        StringItem button1 = new StringItem("", "click!", Item.BUTTON);
        StringItem button2 = new StringItem("", "click2!", Item.BUTTON);
        Command ok = new Command("Ok", "", Command.OK, 0);
        Command ok2 = new Command("Ok2", "", Command.OK, 0);

        form.append(button1);

        //Add command and then remove it:
        button1.addCommand(ok);
        button1.removeCommand(ok);

        form.append(button2);

        //Add same command to two items and remove those commands:
        button1.addCommand(ok);
        button2.addCommand(ok);

        button1.removeCommand(ok);
        button2.removeCommand(ok);

        //Add two commands to same item and then remove them:
        button1.addCommand(ok);
        button1.addCommand(ok2);
        button1.removeCommand(ok);
        button1.removeCommand(ok2);

        //Same as before but different order:
        button1.addCommand(ok);
        button1.addCommand(ok2);
        button1.removeCommand(ok2);
        button1.removeCommand(ok);

        //Add two commands to two different items and then remove them:
        button1.addCommand(ok);
        button2.addCommand(ok2);
        button1.removeCommand(ok);
        button2.removeCommand(ok2);

        //Add two commands to two items so that both items have two commands
        //and then remove all commands:
        button1.addCommand(ok);
        button1.addCommand(ok2);
        button2.addCommand(ok);
        button2.addCommand(ok2);
        button1.removeCommand(ok);
        button1.removeCommand(ok2);
        button2.removeCommand(ok);
        button2.removeCommand(ok2);

        //Remove null command:
        button1.removeCommand(null);

        //Remove command that is not in item:
        button1.removeCommand(ok);
    }
}

