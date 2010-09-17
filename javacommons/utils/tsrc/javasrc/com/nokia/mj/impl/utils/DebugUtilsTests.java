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


package com.nokia.mj.impl.utils;

import com.nokia.mj.impl.utils.DebugUtils;

import com.nokia.mj.impl.rt.test.UnitTestSuiteCreator;
import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * DebugUtils unit tests.
 */
public class DebugUtilsTests extends TestCase implements UnitTestSuiteCreator
{
    // Create the test suite.
    public TestSuite createTestSuite(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new DebugUtilsTests("TestStackTrace", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((DebugUtilsTests)tc).testStackTrace();
            }
        }));

        return suite;
    }

    public DebugUtilsTests()
    {
    }

    public DebugUtilsTests(String aTestName, TestMethod aTestMethod)
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

    public void testStackTrace()
    {
        try
        {
            String res = DebugUtils.getStackTrace(null);
            assertTrue("Not null: " + res, res == null);

            Exception e1 = new Exception("Test 42");
            res = DebugUtils.getStackTrace(e1);
            boolean ok = res.indexOf("at com.nokia.mj.impl.utils.DebugUtilsTests.testStackTrace") >= 0;
            assertTrue("Exception didn't contain: " + res, ok);

        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }
}
