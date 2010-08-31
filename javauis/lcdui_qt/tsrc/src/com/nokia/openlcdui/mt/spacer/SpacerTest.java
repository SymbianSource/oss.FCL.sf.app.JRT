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
package com.nokia.openlcdui.mt.spacer;

import junit.framework.*;
import javax.microedition.lcdui.*;

import junit.framework.Test;
import junit.framework.TestSuite;
import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li>Collection of tests to test Spacer's API functionality.<br>
 *
 * @created 1.8.2008
 */
public class SpacerTest extends SWTTestCase
{

    private static final int WIDTH = 100;
    private static final int HEIGHT = 100;

    /**
     * Constructor.
     */
    public SpacerTest()
    {
    }

    /**
     * Constructor.
     *
     * @param sTestName name of the test
     * @param rTestMethod TestMethod used
     */
    public SpacerTest(String sTestName)
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
     * To create the test suite. You need to add a new aSuite.addTest entry for
     * any new test methods.
     *
     * @return New TestSuite.
     */
    public static Test suite()
    {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
        java.util.Enumeration e;

        // Add widget tests
        methodNames = SpacerTest.methodNames();
        e = methodNames.elements();
        while(e.hasMoreElements())
        {
            suite.addTest(new SpacerTest((String)e.nextElement()));
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
     * Test method.
     */
    public void testAccessors()
    {
        Spacer spacer;

        try
        {
            spacer = new Spacer(-1, -1);
            fail("no IllegalArgumentException is thrown when parameters are"
                 + " incorrect in constructor");
        }
        catch(IllegalArgumentException e)
        {
            //OK
        }
        spacer = new Spacer(WIDTH, HEIGHT);
        Command ok = new Command("Ok", "", Command.ITEM, 0);
        try
        {
            spacer.addCommand(ok);
            fail("no IllegalStateException is thrown when adding command");
        }
        catch(IllegalStateException e)
        {
            //OK
        }
        try
        {
            spacer.setDefaultCommand(ok);
            fail("no IllegalStateException is thrown when setting default "
                 + " command");
        }
        catch(IllegalStateException e)
        {
            //OK
        }
        try
        {
            spacer.setLabel("some label");
            fail("no IllegalStateException is thrown when setting label");
        }
        catch(IllegalStateException e)
        {
            //OK
        }
        try
        {
            spacer.setMinimumSize(-1, -1);
            fail("no IllegalArgumentException is thrown when setting minimum "
                 + "size less then 0");
        }
        catch(IllegalArgumentException e)
        {
            //OK
        }
    }
}
