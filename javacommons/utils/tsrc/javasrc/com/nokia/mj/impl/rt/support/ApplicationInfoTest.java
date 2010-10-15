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
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.rt.test.ApplicationInfoImpl;
import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * ApplicationInfo unit tests.
 */
public class ApplicationInfoTest extends TestCase implements UnitTestSuiteCreator
{

    // Begin j2meunit test framework setup
    public TestSuite createTestSuite(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        String testSelector = "0000000000000000";
        if (args.length > 1 && args[1] != null)
        {
            testSelector = args[1]  + "0000000000000000";
        }


        if (testSelector.charAt(0) == '0')
        {
            suite.addTest(new ApplicationInfoTest("testAppInfo", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((ApplicationInfoTest)tc).testAppInfo();
                }
            }));
        }

        if (testSelector.charAt(0) != '0')
        {
            suite.addTest(new ApplicationInfoTest("testAppInfoNotExist", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((ApplicationInfoTest)tc).testAppInfoNotExist();
                }
            }));
        }

        return suite;

    }

    public ApplicationInfoTest()
    {
    }

    public ApplicationInfoTest(String aTestName, TestMethod aTestMethod)
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

    private void check(String excpected, String received)
    {
        assertTrue("Expected: "+ excpected + ", Received: " + received,
                   excpected.equals(received));
    }

    private void testAppInfo()
    {
        try
        {
            ApplicationInfo appInfo = ApplicationInfo.getInstance();
            check(appInfo.getRuntimeType(), ApplicationInfoImpl.TEST_RT_TYPE);
            check(appInfo.getProtectionDomain(), ApplicationInfoImpl.TEST_PROTECTION_DOMAIN);
            check(appInfo.getUid().toString(), ApplicationInfoImpl.TEST_APP_UID.toString());
            check(appInfo.getSuiteUid().toString(), ApplicationInfoImpl.TEST_APP_SUITE_UID.toString());
            check(appInfo.getSuiteName(), ApplicationInfoImpl.TEST_APP_SUITE_NAME);
            check(appInfo.getName(), ApplicationInfoImpl.TEST_APP_NAME);
            check(appInfo.getVendor(), ApplicationInfoImpl.TEST_APP_VENDOR);
            check(appInfo.getVersion(), ApplicationInfoImpl.TEST_APP_VERSION);
            check(appInfo.getRootPath(), ApplicationInfoImpl.TEST_PATH);
            check(appInfo.getMainClass(), ApplicationInfoImpl.MAIN_CLASS);
            check(appInfo.getAttribute(ApplicationInfoImpl.TEST_KEY), ApplicationInfoImpl.TEST_VALUE);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private void testAppInfoNotExist()
    {
        try
        {
            ApplicationInfo.getInstance().getSuiteName();
            assertTrue("No exception.", false);
        }
        catch (Error re)
        {
            int ind = re.toString().indexOf("Not able to instantiate class com.nokia.mj.impl.rt.test2.ApplicationInfoImpl");
            boolean ok = ind >= 0;
            if (!ok)
            {
                // Accept also java.lang.ExceptionInInitializerError.
                ok = re.toString().equals("java.lang.ExceptionInInitializerError");
            }
            assertTrue(re.toString(),  ok);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

}

