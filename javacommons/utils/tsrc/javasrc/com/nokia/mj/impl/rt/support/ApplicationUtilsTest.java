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

import java.security.Permission;


import com.nokia.mj.impl.rt.test.UnitTestSuiteCreator;
import com.nokia.mj.impl.rt.support.ShutdownListener;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.test.ApplicationUtilsImpl;
import com.nokia.mj.impl.utils.Uid;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;



/**
 * ApplicationUtils unit tests.
 */
public class ApplicationUtilsTest extends TestCase implements UnitTestSuiteCreator
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


        if (testSelector.charAt(0) != '0')
        {
            suite.addTest(new ApplicationUtilsTest("testShutdowListeners", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((ApplicationUtilsTest)tc).testShutdowListeners();
                }
            }));
        }

        if (testSelector.charAt(1) != '0')
        {
            suite.addTest(new ApplicationUtilsTest("testShutdowListeners 2", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((ApplicationUtilsTest)tc).testShutdowListeners2();
                }
            }));
        }

        if (testSelector.charAt(2) != '0')
        {
            suite.addTest(new ApplicationUtilsTest("testOtherCalls", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((ApplicationUtilsTest)tc).testOtherCalls();
                }
            }));
        }


        if (testSelector.charAt(3) != '0')
        {
            suite.addTest(new ApplicationUtilsTest("testEmptyWaiter", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((ApplicationUtilsTest)tc).testEmptyWaiter();
                }
            }));
        }

        if (testSelector.charAt(4) != '0')
        {

            suite.addTest(new ApplicationUtilsTest("testWaiter", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((ApplicationUtilsTest)tc).testWaiter();
                }
            }));
        }

        if (testSelector.charAt(5) != '0')
        {

            suite.addTest(new ApplicationUtilsTest("testWaiterNoStart", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((ApplicationUtilsTest)tc).testWaiterNoStart();
                }
            }));
        }

        if (testSelector.charAt(6) != '0')
        {

            suite.addTest(new ApplicationUtilsTest("testWaiterNotifyBeforeStart", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((ApplicationUtilsTest)tc).testWaiterNotifyBeforeStart();
                }
            }));
        }

        if (testSelector.charAt(7) != '0')
        {

            suite.addTest(new ApplicationUtilsTest("testAppUtilsNotExist", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((ApplicationUtilsTest)tc).testAppUtilsNotExist();
                }
            }));
        }

        return suite;

    }

    public ApplicationUtilsTest()
    {
    }

    public ApplicationUtilsTest(String aTestName, TestMethod aTestMethod)
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

    private static class ShutdownListenerImpl implements ShutdownListener
    {
        public int mShutDownCallCount = 0;
        public void shuttingDown()
        {
            mShutDownCallCount++;
        }
    }
    // Test cases

    private void testShutdowListeners()
    {
        try
        {
            final int count = 25;
            ApplicationUtils appUtils = ApplicationUtils.getInstance();
            ShutdownListenerImpl[] sls = new ShutdownListenerImpl[count];
            for (int i = 0; i < count; ++i)
            {
                sls[i] = new ShutdownListenerImpl();
                appUtils.addShutdownListener(sls[i]);
            }
            ApplicationUtilsImpl.doShutdownImpl();
            for (int i = 0; i < count; ++i)
            {
                assertTrue(i + " call count incorrect: "+ sls[i].mShutDownCallCount,
                           sls[i].mShutDownCallCount == 1);
            }

            for (int i = 0; i < count; ++i)
            {
                sls[i] = new ShutdownListenerImpl();
                appUtils.addShutdownListener(sls[i]);
            }
            appUtils.removeShutdownListener(sls[count-1]);
            ApplicationUtilsImpl.doShutdownImpl();
            for (int i = 0; i < count - 1; ++i)
            {
                assertTrue(i + " call count incorrect: "+ sls[i].mShutDownCallCount,
                           sls[i].mShutDownCallCount == 1);
            }
            assertTrue((count -1) + " call count incorrect: "+ sls[count -1].mShutDownCallCount,
                       sls[count -1].mShutDownCallCount == 0);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private void testShutdowListeners2()
    {
        try
        {
            ApplicationUtils appUtils = ApplicationUtils.getInstance();
            appUtils.removeShutdownListener(new ShutdownListenerImpl());
            try
            {
                appUtils.removeShutdownListener(null);
                assertTrue("No exception: ", false);
            }
            catch (NullPointerException ne)
            {
            }
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private void checkCallCounts(String info, int ind)
    {
        assertTrue("ApplicationUtils." + info + " failed " + ApplicationUtilsImpl.print(),
                   ApplicationUtilsImpl.checkCalls(ind));
    }

    public class TestPermission extends Permission
    {

        public TestPermission()
        {
            super("TestPermission");
        }

        public String getActions()
        {
            return "TestPermission Actions";
        }

        public boolean equals(Object obj)
        {
            return false;
        }

        public int hashCode()
        {
            return 0;
        }
        public boolean implies(Permission permission)
        {
            return true;
        }
    }

    private void testOtherCalls()
    {
        try
        {
            ApplicationUtils appUtils = ApplicationUtils.getInstance();
            appUtils.notifyExitCmd();
            checkCallCounts("notifyExitCmd()", ApplicationUtilsImpl.NOTIFY_CALL);

            ApplicationUtilsImpl.clear();
            appUtils.uiDisposed();
            checkCallCounts("uiDisposed()", ApplicationUtilsImpl.UI_DISPOSED_CALL);

            ApplicationUtilsImpl.clear();
            appUtils.pauseApplication();
            checkCallCounts("pauseApplication()", ApplicationUtilsImpl.PAUSE_CALL);

            ApplicationUtilsImpl.clear();
            appUtils.pauseApplication();
            checkCallCounts("pauseApplication()", ApplicationUtilsImpl.PAUSE_CALL);

            ApplicationUtilsImpl.clear();
            appUtils.resumeApplication();
            checkCallCounts("resumeApplication()", ApplicationUtilsImpl.RESUME_CALL);

            ApplicationUtilsImpl.clear();
            Permission p1 = new TestPermission();
            appUtils.checkPermission(p1);
            assertTrue("ApplicationUtils.checkPermission(Permission) failed ",
                       p1 == ApplicationUtilsImpl.mPermission);
            checkCallCounts("checkPermission()", ApplicationUtilsImpl.PERMISSION_CALL1);

            Permission p2 = new TestPermission();
            Uid uid = Uid.createUid("[12345678]");
            ApplicationUtilsImpl.clear();
            appUtils.checkPermission(uid, p2);
            checkCallCounts("checkPermission2()", ApplicationUtilsImpl.PERMISSION_CALL2);
            assertTrue("ApplicationUtils.checkPermission(Permission2) failed ",
                       p2 == ApplicationUtilsImpl.mPermission);

            assertTrue("ApplicationUtils.checkPermission(Uid) failed ",
                       uid.equals(ApplicationUtilsImpl.mUid));


        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private static class TestWaiter extends Thread
    {
        private static TestWaiter[] mTestWaiters;
        private boolean mWasReleased = false;
        private Boolean mCanContinue = null;

        private static void createWaiters(int count)
        {
            mTestWaiters = new TestWaiter[count];
            for (int i = 0; i < mTestWaiters.length; ++i)
            {
                mTestWaiters[i] = new TestWaiter();
                mTestWaiters[i].start();
            }
        }

        private static boolean check(boolean canContinue)
        {
            for (int i = 0; i < mTestWaiters.length; ++i)
            {
                if (!mTestWaiters[i].mWasReleased)
                {
                    System.err.println("ID " + i + ": " + mTestWaiters[i].toString());
                    return false;
                }
                if (mTestWaiters[i].mCanContinue == null ||
                        mTestWaiters[i].mCanContinue.booleanValue() != canContinue)
                {
                    System.err.println("ID " + i + ": " + mTestWaiters[i].toString());
                    return false;
                }
            }
            return true;
        }

        private static boolean checkWaiting(boolean isWaiting)
        {
            for (int i = 0; i < mTestWaiters.length; ++i)
            {
                if (mTestWaiters[i].mWasReleased == isWaiting)
                {
                    System.err.println("ID " + i + ": " + mTestWaiters[i].toString());
                    return false;
                }
            }
            return true;
        }

        public String toString()
        {
            StringBuffer sb = new StringBuffer();
            sb.append("State: \n");
            sb.append("  mWasReleased " + mWasReleased);
            sb.append("\n  mCanContinue " + mCanContinue);
            return sb.toString();
        }

        public void run()
        {
            boolean canContinue = ApplicationUtils.getInstance().waitStart();
            mCanContinue = new Boolean(canContinue);
            mWasReleased = true;
        }
    }

    private void testEmptyWaiter()
    {
        try
        {
            ApplicationUtilsImpl.releaseWaiterImpl(true);
            ApplicationUtilsImpl.releaseWaiterImpl(false);

        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private void testWaiter()
    {
        try
        {
            TestWaiter.createWaiters(5);
            Thread.sleep(100);
            assertTrue("releaseWaiterImpl(true) not waiting.", TestWaiter.checkWaiting(true));
            ApplicationUtilsImpl.releaseWaiterImpl(true);
            Thread.sleep(100);
            assertTrue("releaseWaiterImpl(true) failed.", TestWaiter.check(true));

        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private void testWaiterNoStart()
    {
        try
        {
            TestWaiter.createWaiters(5);
            Thread.sleep(100);
            assertTrue("releaseWaiterImpl(true) not waiting.", TestWaiter.checkWaiting(true));
            ApplicationUtilsImpl.releaseWaiterImpl(false);
            Thread.sleep(100);
            assertTrue("releaseWaiterImpl(true) failed.", TestWaiter.check(false));

        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private void testWaiterNotifyBeforeStart()
    {
        try
        {
            ApplicationUtilsImpl.releaseWaiterImpl(true);
            TestWaiter.createWaiters(5);
            Thread.sleep(100);
            assertTrue("releaseWaiterImpl(true) not waiting.", TestWaiter.checkWaiting(false));
            assertTrue("releaseWaiterImpl(true) failed.", TestWaiter.check(true));

        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private void testAppUtilsNotExist()
    {
        try
        {
            ApplicationUtilsImpl.releaseWaiterImpl(true);
            assertTrue("No exception.", false);
        }
        catch (Error re)
        {
            int ind = re.toString().indexOf("Not able to instantiate class com.nokia.mj.impl.rt.test2.ApplicationUtilsImpl");
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

