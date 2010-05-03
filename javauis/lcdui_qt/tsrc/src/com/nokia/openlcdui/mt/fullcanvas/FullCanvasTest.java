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
package com.nokia.openlcdui.mt.fullcanvas;

import junit.framework.*;

import javax.microedition.lcdui.*;
import com.nokia.openlcdui.mt.SWTTestCase;

import com.nokia.mid.ui.FullCanvas;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test: <li>Collection of tests to test
 * FullCanvas functionality. <br>
 * <br>
 * Created: 2009-02-10
 */
public class FullCanvasTest extends SWTTestCase implements CommandListener {

    /**
     * Constructor.
     */
    public FullCanvasTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public FullCanvasTest(String sTestName) {
        super(sTestName);
    }

    /**
     * Creates the test suite. You need to add a new aSuite.addTest entry for
     * any new test methods, otherwise they won't be run.
     *
     * @return New testsuite.
     */
    public static Test suite() {
		TestSuite suite = new TestSuite();

	    java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add widget tests
	    methodNames = FullCanvasTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new FullCanvasTest((String)e.nextElement()));
	    }

		return suite;
	}

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testAccessors");
        return methodNames;
    }
    
    protected void runTest() throws Throwable {
        if (getName().equals("testAccessors")) testAccessors();
        else super.runTest();
    }
    
    public void commandAction(Command com, Displayable d) {
    }

    /**
     * Tests basic functionality of FullCanvas methods.
     */
    public void testAccessors() {

        TestFullCanvas fc = new TestFullCanvas();
        Command c = new Command("test", "", Command.ITEM, 0);
        try {
            fc.addCommand(c);
            fail("No IllegalStateException is thrown.");
        }
        catch (IllegalStateException ex) {
            // OK
        }

        // Set null string and make sure an exception is thrown:
        try {
            fc.setCommandListener(this);
            fail("No IllegalStateException is thrown.");
        }
        catch (IllegalStateException ex) {
        }
    }

}

class TestFullCanvas extends FullCanvas {
    public void paint(Graphics g) {
        //
    }
}