/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.rt;


import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.rt.support.JvmInternal;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * SystemProperty unit tests.
 */
public class SystemPropertyTests extends TestCase implements InstallerMain
{

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new SystemPropertyTests("setPropTests", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SystemPropertyTests)tc).setPropTests();
            }
        }));

        suite.addTest(new SystemPropertyTests("staticPropTests", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SystemPropertyTests)tc).staticPropTests();
            }
        }));

        suite.addTest(new SystemPropertyTests("dynamicPropTests", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SystemPropertyTests)tc).dynamicPropTests();
            }
        }));

 
        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);

    }

    public SystemPropertyTests()
    {
    }

    public SystemPropertyTests(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    // End j2meunit test framework setup

    protected void setUp()
    {
    }

    protected void tearDown()
    {
    }

    // Test cases

    private static final String testPropertyKey1 = "test.property.key1";
    private static final String testPropertyKey2 = "test.property.key2";
    private static final String testPropertyKey3 = "test.property.key3";
    private static final String testPropertyKey4 = "test.property.key4";
    private static final String testPropertyVal1 = "test.property.val1";
    private static final String testPropertyVal2 = "test.property.val2";
    private static final String testPropertyVal3 = "test.property.val3";
    private static final String testPropertyVal4 = "test.property.val4";
    private static final String testPropertyVal5 = "test.property.val5";
    private static final String testPropertyVal6 = "test.property.val6";

    private void setPropTests()
    {
        try
        {
            // Check that the system property is null in the beginning of the
            // test.
            String res = System.getProperty(testPropertyKey1);
            assertTrue("Fail1, got: "+ res, res == null);

            // Set the system property and check that it was correctly set.
            JvmInternal.setSystemProperty(testPropertyKey1, testPropertyVal1);
            res = System.getProperty(testPropertyKey1);
            assertTrue("Fail2, got: "+ res, testPropertyVal1.equals(res));

            // Set a new value to same system property and check that it was correctly set.
            JvmInternal.setSystemProperty(testPropertyKey1, testPropertyVal2);
            res = System.getProperty(testPropertyKey1);
            assertTrue("Fail3, got: "+ res, testPropertyVal2.equals(res));

            // Set a null value to same system property and check that an 
            // exception is thrown and the value is not changed.
            try
            {
                JvmInternal.setSystemProperty(testPropertyKey1, null);
                assertTrue("No exception1: "+ res, false);
            }
            catch (NullPointerException ne)
            {
            }
            res = System.getProperty(testPropertyKey1);
            assertTrue("Fail4, got: "+ res, testPropertyVal2.equals(res));

            // Check that the user property is null in the beginning of the
            // test.
            res = System.getProperty(testPropertyKey2);
            assertTrue("Fail5, got: "+ res, res == null);

            // Set the user property and check that it was correctly set.
            JvmInternal.setUserProperty(testPropertyKey2, testPropertyVal3);
            res = System.getProperty(testPropertyKey2);
            assertTrue("Fail6, got: "+ res, testPropertyVal3.equals(res));

            // Set a new value to same user property and check that it was correctly set.
            JvmInternal.setUserProperty(testPropertyKey2, testPropertyVal4);
            res = System.getProperty(testPropertyKey2);
            assertTrue("Fail7, got: "+ res, testPropertyVal4.equals(res));

            // Set a null value to same user property and check that an 
            // exception is thrown and the value is not changed.
            try
            {
                JvmInternal.setSystemProperty(testPropertyKey2, null);
                assertTrue("No exception2: "+ res, false);
            }
            catch (NullPointerException ne)
            {
            }
            res = System.getProperty(testPropertyKey2);
            assertTrue("Fail8, got: "+ res, testPropertyVal4.equals(res));

            // Set the same system and user property and check that the user property 
            // doesn't override the system property.
            JvmInternal.setSystemProperty(testPropertyKey3, testPropertyVal5);
            JvmInternal.setUserProperty(testPropertyKey3, testPropertyVal6);
            res = System.getProperty(testPropertyKey3);
            assertTrue("Fail9, got: "+ res, testPropertyVal5.equals(res));

        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private void staticPropTests()
    {
        try
        {
            // Check some random static system properties.
            String res = System.getProperty("microedition.jtwi.version");
            assertTrue("Fail01, got: "+ res, "1.0".equals(res));

            res = System.getProperty("microedition.msa.version");
            assertTrue("Fail01, got: "+ res, "1.1-SUBSET".equals(res));

            res = System.getProperty("microedition.profiles");
            assertTrue("Fail01, got: "+ res, "MIDP-2.1".equals(res));
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }


    private static final String testDynPropertyKey1 = "test.dyn.property.key1";
    private static final String testDynPropertyKey2 = "test.dyn.property.key2";
    private static final String testDynPropertyVal1 = testDynPropertyKey1 + " DynamicValue (";
    private static final String testDynPropertyVal2 = testDynPropertyKey2 + " DynamicValue2 (1)";

    private void dynamicPropTests()
    {
        try
        {
            JvmInternal.setSystemProperty(testDynPropertyKey1, ":test.Property");
            JvmInternal.setSystemProperty(testDynPropertyKey2, ":test.Property2");

            // Check that dynamic property is changing per each call.
            String res = System.getProperty(testDynPropertyKey1);
            assertTrue("Fail_21, got: "+ res, (testDynPropertyVal1+"1)").equals(res));
            res = System.getProperty(testDynPropertyKey1);
            assertTrue("Fail_22, got: "+ res, (testDynPropertyVal1+"2)").equals(res));
            res = System.getProperty(testDynPropertyKey1);
            assertTrue("Fail_23, got: "+ res, (testDynPropertyVal1+"3)").equals(res));
            res = System.getProperty(testDynPropertyKey1);
            assertTrue("Fail_24, got: "+ res, (testDynPropertyVal1+"4)").equals(res));

            // Check that static dynamic property is set in first call, but doesn't change
            // in leading calls.
            res = System.getProperty(testDynPropertyKey2);
            assertTrue("Fail_25, got: "+ res, testDynPropertyVal2.equals(res));
            res = System.getProperty(testDynPropertyKey2);
            assertTrue("Fail_26, got: "+ res, testDynPropertyVal2.equals(res));
            res = System.getProperty(testDynPropertyKey2);
            assertTrue("Fail_27, got: "+ res, testDynPropertyVal2.equals(res));
            res = System.getProperty(testDynPropertyKey2);
            assertTrue("Fail_28, got: "+ res, testDynPropertyVal2.equals(res));

        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }
}
