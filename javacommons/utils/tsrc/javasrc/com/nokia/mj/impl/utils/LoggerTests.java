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

import com.nokia.mj.impl.utils.Logger;

import com.nokia.mj.impl.rt.test.UnitTestSuiteCreator;
import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * DebugUtils unit tests.
 */
public class LoggerTests extends TestCase implements UnitTestSuiteCreator
{
    static
    {
        if ("unexiting".equals(System.getProperty("com.nokia.jvm.port")))
        {
            System.out.println("NOTE!! IT IS EXPECTED THAT THIS TEST WILL " +
                               "LEAD TO BIG AMOUNT OF TRACES.");
        }

    }
    // Create the test suite.
    public TestSuite createTestSuite(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new LoggerTests("Test Log writings", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LoggerTests)tc).testLogWritings();
            }
        }));

        return suite;
    }

    public LoggerTests()
    {
    }

    public LoggerTests(String aTestName, TestMethod aTestMethod)
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

    public void testLogWritings()
    {
        // Check the log writings manually from JavaLocation.log file.
        try
        {
            Logger.ELOG(Logger.EJavaLocation, "This is Logger.ELOG without exception");
            Logger.ELOG(Logger.EJavaLocation, "This is Logger.ELOG with exception", new Exception("Logger.ELOG"));
            Logger.ELOG(Logger.EJavaLocation, "This is Logger.ELOG with null exception", null);
            Logger.ELOG(Logger.EJavaLocation, null);

            Logger.WLOG(Logger.EJavaLocation, "This is Logger.WLOG without exception");
            Logger.WLOG(Logger.EJavaLocation, "This is Logger.WLOG with exception", new Exception("Logger.WLOG"));
            Logger.WLOG(Logger.EJavaLocation, "This is Logger.WLOG with null exception", null);
            Logger.WLOG(Logger.EJavaLocation, null);

            Logger.PLOG(Logger.EJavaLocation, "This is Logger.PLOG without exception");
            Logger.PLOG(Logger.EJavaLocation, "This is Logger.PLOG with exception", new Exception("Logger.PLOG"));
            Logger.PLOG(Logger.EJavaLocation, "This is Logger.PLOG with null exception", null);
            Logger.PLOG(Logger.EJavaLocation, null);

            Logger.ILOG(Logger.EJavaLocation, "This is Logger.ILOG without exception");
            Logger.ILOG(Logger.EJavaLocation, "This is Logger.ILOG with exception", new Exception("Logger.PLOG"));
            Logger.ILOG(Logger.EJavaLocation, "This is Logger.ILOG with null exception", null);
            Logger.ILOG(Logger.EJavaLocation, null);

            Logger.JELOG(Logger.EJavaLocation, "This is Logger.JELOG");
            Logger.JELOG(Logger.EJavaLocation, null);

            Logger.HLOG(Logger.EJavaLocation, "This is Logger.HLOG");
            Logger.HLOG(Logger.EJavaLocation, null);

            testLOG(Logger.EError, "Logger.EError");
            testLOG(Logger.EWarning, "Logger.EWarning");
            testLOG(Logger.EInfoPrd, "Logger.EInfoPrd");
            testLOG(Logger.EInfo, "Logger.EInfo");
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private void testLOG(int level, String str)
    {
            Logger.LOG(Logger.EJavaLocation, level, "This is Logger.LOG (" + str + ") without exception");
            Logger.LOG(Logger.EJavaLocation, level, "This is Logger.LOG (" + str + ") with exception", new Exception(str));
            Logger.LOG(Logger.EJavaLocation, level, "This is Logger.LOG (" + str + ") with null exception", null);
            Logger.LOG(Logger.EJavaLocation, level, null);
    }
}
