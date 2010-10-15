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

package com.nokia.mj.impl.rt.support;



import com.nokia.mj.impl.rt.test.UnitTestSuiteCreator;
import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

import com.nokia.mj.impl.rt.support.JvmInternal;

/**
 * Finalizer unit tests.
 */
public class FinalizerTest extends TestCase implements UnitTestSuiteCreator
{

    // Begin j2meunit test framework setup
    public TestSuite createTestSuite(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new FinalizerTest("testFinalizerSingleClass", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((FinalizerTest)tc).testFinalizerSingleClass();
            }
        }));

        suite.addTest(new FinalizerTest("testFinalizerManyClassesSingleObject", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((FinalizerTest)tc).testFinalizerManyClassesSingleObject();
            }
        }));

        suite.addTest(new FinalizerTest("testFinalizerManyClasses", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((FinalizerTest)tc).testFinalizerManyClasses();
            }
        }));

        return suite;
    }

    public FinalizerTest()
    {
    }

    public FinalizerTest(String aTestName, TestMethod aTestMethod)
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

    private static class Test1 extends FinalizableBase
    {
    }

    private static class Test2 extends FinalizableBase
    {
    }

    private static class Test3 extends FinalizableBase
    {
    }

    private static class Test4 extends FinalizableBase
    {
    }

    private static class Test5 extends FinalizableBase
    {
    }

    private static class Test6 extends FinalizableBase
    {
    }

    private void testFinalizerSingleClass()
    {
        try
        {
            new Test1();
            System.gc();
            JvmInternal.runFinalization();
            StringBuffer sb = new StringBuffer();
            boolean ok = FinalizerStatistics.getNonfinalizedObjects(sb);
            assertTrue(sb.toString(), ok);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }
    private void testFinalizerManyClassesSingleObject()
    {
        try
        {
            new Test1();
            new Test2();
            new Test3();
            new Test4();
            new Test5();
            new Test6();
            System.gc();
            JvmInternal.runFinalization();
            StringBuffer sb = new StringBuffer();
            boolean ok = FinalizerStatistics.getNonfinalizedObjects(sb);
            assertTrue(sb.toString(), ok);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private void testFinalizerManyClasses()
    {
        try
        {
            for (int i = 0; i < 10; ++i)
            {
                new Test1();
                new Test2();
                new Test3();
                new Test4();
                new Test5();
                new Test6();
            }
            System.gc();
            JvmInternal.runFinalization();
            StringBuffer sb = new StringBuffer();
            boolean ok = FinalizerStatistics.getNonfinalizedObjects(sb);
            assertTrue(sb.toString(), ok);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

}
