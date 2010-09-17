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
package com.nokia.openlcdui.mt.gauge;

import junit.framework.*;

import javax.microedition.lcdui.*;
import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li>Collection of tests to test Gauge's API functionality. <br>
 * <br>
 */
public class GaugeTest extends SWTTestCase
{

    private static String label = "Label";

    private static final int POSITIVE_INT = 100;
    private static final int NEGATIVE_INT = -100;

    /**
     * Constructor.
     */
    public GaugeTest()
    {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public GaugeTest(String sTestName)
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
     * @return New testsuite.
     */
    public static Test suite()
    {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
        java.util.Enumeration e;

        // Add widget tests
        methodNames = GaugeTest.methodNames();
        e = methodNames.elements();
        while(e.hasMoreElements())
        {
            suite.addTest(new GaugeTest((String)e.nextElement()));
        }

        return suite;
    }

    public static java.util.Vector methodNames()
    {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testAccessors");
        methodNames.addElement("gaugeCurrentValuesTest");
        return methodNames;
    }

    protected void runTest() throws Throwable
    {
        if(getName().equals("testAccessors")) testAccessors();
        else if(getName().equals("gaugeCurrentValuesTest")) gaugeCurrentValuesTest();
        else super.runTest();
    }
    /**
     * Tests the gauge accessor methods.
     */
    public void testAccessors()
    {
        Gauge gauge = new Gauge(null, false, POSITIVE_INT, 0);

        assertTrue("Should be noninteractive", !gauge.isInteractive());
        assertEquals("Maxvalues don't match", POSITIVE_INT,
                     gauge.getMaxValue());
        assertEquals("Labels don't match", null, gauge.getLabel());

        gauge.setLabel(label);
        assertEquals("Labels don't match", label, gauge.getLabel());

        gauge.setValue(NEGATIVE_INT);
        assertEquals("Values mismatch", 0, gauge.getValue());

        gauge.setValue(POSITIVE_INT * 2);
        assertEquals("Values mismatch", POSITIVE_INT, gauge.getValue());

        gauge.setValue(0);
        try
        {
            gauge.setMaxValue(NEGATIVE_INT);
            fail("1. IllegalArgumentException expected");
        }
        catch(IllegalArgumentException iae)
        {
        }

        gauge.setMaxValue(Gauge.INDEFINITE);

        try
        {
            gauge.setValue(NEGATIVE_INT);
            fail("2. IllegalArgumentException expected");
        }
        catch(IllegalArgumentException iae)
        {
        }

        try
        {
            gauge.setValue(Gauge.CONTINUOUS_IDLE);
            gauge.setValue(Gauge.INCREMENTAL_IDLE);
            gauge.setValue(Gauge.CONTINUOUS_RUNNING);
            gauge.setValue(Gauge.INCREMENTAL_UPDATING);
        }
        catch(IllegalArgumentException iae)
        {
            fail("3. IllegalArgumentException thrown " + iae.getMessage());
        }

        try
        {
            gauge.setValue(POSITIVE_INT);
            fail("4. IllegalArgumentException expected");
        }
        catch(IllegalArgumentException iae)
        {
        }

        // Test minimum bounds, non-interactive:
        gauge.setLabel(null);
        assertTrue("Minimum height was zero or negative, case 1.",
                   gauge.getMinimumHeight() > 0);
        //print("non-interactive minimum height: "
        //        + gauge.getMinimumHeight());
        assertTrue("Minimum width was zero or negative, case 1.",
                   gauge.getMinimumWidth() > 0);
        //print("non-interactive minimum width: "
        //        + gauge.getMinimumWidth());

        // Minimum bounds, interactive:
        Gauge gauge2 = new Gauge(null, true, POSITIVE_INT, 0);
        assertTrue("Minimum height was zero or negative, case 2.",
                   gauge2.getMinimumHeight() > 0);
        //print("interactive minimum height: "
        //        + gauge2.getMinimumHeight());
        assertTrue("Minimum width was zero or negative, case 2.",
                   gauge2.getMinimumWidth() > 0);
        //print("interactive minimum width: "
        //        + gauge2.getMinimumWidth());

        // Minimum bounds, non-interactive with label:
        Gauge gauge3 = new Gauge("label", false, POSITIVE_INT, 0);
        assertTrue("Minimum height was zero or negative, case 3.",
                   gauge3.getMinimumHeight() > 0);
        //print("labeled non-interactive minimum height: "
        //        + gauge3.getMinimumHeight());
        assertTrue("Minimum width was zero or negative, case 3.",
                   gauge3.getMinimumWidth() > 0);
        //print("labeled non-interactive minimum width: "
        //        + gauge3.getMinimumWidth());

        // Minimum bounds, interactive with label:
        Gauge gauge4 = new Gauge("label", true, POSITIVE_INT, 0);
        assertTrue("Minimum height was zero or negative, case 4.",
                   gauge4.getMinimumHeight() > 0);
        //print("labeled interactive minimum height: "
        //        + gauge4.getMinimumHeight());
        assertTrue("Minimum width was zero or negative, case 4.",
                   gauge4.getMinimumWidth() > 0);
        //print("labeled interactive minimum width: "
        //        + gauge4.getMinimumWidth());
    }

    /**
     * Tests Non-interactive INDEFINITE Gauge with different current values.
     * <p>
    * Test passes if current values of the Gauge will not be changed.
     * <p>
    * Test fails if current values of the Gauge will be changed.
    * <p>
     */
    public void gaugeCurrentValuesTest()
    {
        Gauge  gaugeObj;
        int[]  values = {Gauge.CONTINUOUS_IDLE, Gauge.CONTINUOUS_RUNNING,
                         Gauge.INCREMENTAL_IDLE, Gauge.INCREMENTAL_UPDATING};

        for (int i=0; i != values.length; ++i) 
        {
            gaugeObj = new Gauge("MT Gauge", false, Gauge.INDEFINITE, values[i]);

            gaugeObj.setMaxValue(Gauge.INDEFINITE);

            if (gaugeObj.getMaxValue() != Gauge.INDEFINITE) 
            {
                String s = "Test FAILED\n Passed: Gauge.INDEFINITE ("+
                Gauge.INDEFINITE+"), got:" + gaugeObj.getMaxValue();
                fail("1. "+s);
            }

            if (gaugeObj.getValue() != values[i]) 
            {
                String s = "Test FAILED\n Current value was changed. Was: " +
                values[i] + ", became: "+gaugeObj.getValue();
                fail("2. "+s);
            }
        }
    }
}
