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


package com.nokia.mj.impl.utils.concurrent;

import java.util.Enumeration;
import java.util.Vector;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.utils.DebugUtils;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * BufferedReader unit tests.
 */
public class ConcurrentTests extends TestCase implements InstallerMain
{
    private int mMaxThreadId;
    private int mReleaseThreadId;
    private int mNextReleasedThreadId;
    private int mRunningThreadCount;
    private Semaphore mSem;
    private Object mLockEnd;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        createDumperThread(false);
        TestSuite suite = new TestSuite(this.getClass().getName());


        suite.addTest(new ConcurrentTests("testLockBasic", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ConcurrentTests)tc).testLockBasic();
            }
        }));

        suite.addTest(new ConcurrentTests("testLockFair", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ConcurrentTests)tc).testLockFair();
            }
        }));


        suite.addTest(new ConcurrentTests("testLockFair2", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ConcurrentTests)tc).testLockFair2();
            }
        }));

        suite.addTest(new ConcurrentTests("testLockUnFair", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ConcurrentTests)tc).testLockUnFair();
            }
        }));

        suite.addTest(new ConcurrentTests("testLockUnFair2", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ConcurrentTests)tc).testLockUnFair2();
            }
        }));

        suite.addTest(new ConcurrentTests("testLockUnFair3", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ConcurrentTests)tc).testLockUnFair3();
            }
        }));


        suite.addTest(new ConcurrentTests("testCondition1", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ConcurrentTests)tc).testCondition1();
            }
        }));

        suite.addTest(new ConcurrentTests("testCondition10_10", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ConcurrentTests)tc).testCondition10_10();
            }
        }));


        suite.addTest(new ConcurrentTests("testCondition5_10", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ConcurrentTests)tc).testCondition5_10();
            }
        }));



        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);

    }

    public ConcurrentTests()
    {
    }

    public ConcurrentTests(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    private void createDumperThread(boolean doCreate)
    {
        if (doCreate)
        {
            new Thread(new Runnable(){
                public void run()
                {
                    threadSleep(5000);
                }
            }).start();
        }
    }


    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    // End j2meunit test framework setup

    protected void setUp()
    {
    }

    protected void tearDown()
    {
    }


    private synchronized void incThreadCount()
    {
        mRunningThreadCount++;
    }

    private synchronized void decThreadCount()
    {
        mRunningThreadCount--;
    }

    private Vector /*String*/ mTrace = new Vector();

    private void trace(String str)
    {
        trace(str, false);
    }

    private void trace(String str, boolean doTrace)
    {
        if (doTrace)
        {
        //System.out.println(str);
        mTrace.addElement(str);
        }
    }


    private void printTrace()
    {
        Enumeration enumer = mTrace.elements();
        while (enumer.hasMoreElements())
        {
            String s = (String)enumer.nextElement();
            System.out.println(s);
        }
        mTrace.removeAllElements();
    }



    // Test cases

    /** LOCK TESTS*/

    private void testLockBasic()
    {
        try
        {
            Semaphore sem = new Semaphore(1, true);
            sem.acquire();
            sem.release();
            sem.acquire();
            sem.release();
            sem.acquire();
            sem.release();
            sem = null;

            Semaphore sem2 = new Semaphore(1, false);
            sem2.acquire();
            sem2.release();
            sem2.acquire();
            sem2.release();
            sem2.acquire();
            sem2.release();
        }
        catch (Throwable t)
        {
            assertTrue(t.toString(), false);
            t.printStackTrace();
        }
    }


    private void testLockFair()
    {
        mRunningThreadCount = 0;
        mNextReleasedThreadId = 1;
        mReleaseThreadId = 10;
        mMaxThreadId = 10;

        try
        {
            
            testLock(true);
        }
        catch (Throwable t)
        {
            assertTrue(t.toString(), false);
            t.printStackTrace();
        }
    }

    private void testLockFair2()
    {
        mRunningThreadCount = 0;
        mNextReleasedThreadId = 1;
        mReleaseThreadId = 5;
        mMaxThreadId = 10;

        try
        {
            
            testLock(true);
        }
        catch (Throwable t)
        {
            assertTrue(t.toString(), false);
            t.printStackTrace();
        }
    }

    private void testLockUnFair()
    {
        mRunningThreadCount = 0;
        mNextReleasedThreadId = 1;
        mReleaseThreadId = 10;
        mMaxThreadId = 10;

        try
        {
            
            testLock(false);
        }
        catch (Throwable t)
        {
            assertTrue(t.toString(), false);
            t.printStackTrace();
        }
    }

    private void testLockUnFair2()
    {
        mRunningThreadCount = 0;
        mNextReleasedThreadId = 1;
        mReleaseThreadId = 5;
        mMaxThreadId = 10;

        try
        {
            
            testLock(false);
        }
        catch (Throwable t)
        {
            assertTrue(t.toString(), false);
            t.printStackTrace();
        }
    }

    private void testLockUnFair3()
    {
        try
        {
            mSem = new Semaphore(1, false);
            mSem.acquire();
            new Thread(new Runnable()
            {
                public void run()
                {
                    threadSleep(100);
                    mSem.release();
                }
            }).start();
            mSem.acquire();
            mSem.release();
            mSem.acquire();
            new Thread(new Runnable()
            {
                public void run()
                {
                    threadSleep(100);
                    mSem.release();
                }
            }).start();
            mSem.acquire();
            mSem.release();
        }
        catch (Throwable t)
        {
            assertTrue(t.toString(), false);
            t.printStackTrace();
        }
    }




    private class LockTestThread extends Thread
    {
        private int mId;
        private boolean mCheck;
        
        private LockTestThread(int id, boolean check)
        {
            mId = id;
            mCheck = check;
        }
        public void run() 
        {
            threadSleep(20);
            trace("run: "+mId);
            incThreadCount();
            if (mId < mMaxThreadId)
            {
                // Need to create additional thread.
                trace("creating new thread: "+mId);
                new LockTestThread(mId+1, mCheck).start();
            }
            if (mReleaseThreadId == mId)
            {
                synchronized (mLockEnd)
                {
                    trace("notify: "+mId);
                    mLockEnd.notify();
                }
            }
            trace("Getting lock: "+mId);
            mSem.acquire();
            trace("Got lock: "+mId);
            if (mCheck)
            {
                String errorTxt = "Incorrect release order. mId: "+ mId + 
                                  ", next: " +mNextReleasedThreadId;
                assertTrue(errorTxt, mNextReleasedThreadId == mId);
            }
            mNextReleasedThreadId++;
            trace("unlock: "+mId);
            mSem.release();
            decThreadCount();
            trace("done: "+mId);
        }
    }

    private void threadSleep(int ms)
    {
        try
        {
            Thread.sleep(ms);
        }
        catch (Exception e)
        {
        }
    }
    private void testLock(boolean isFair) throws Exception
    {
        mSem = new Semaphore(1,isFair);
        mSem.acquire();
        new LockTestThread(1, isFair).start();
        mLockEnd = new Object();
        synchronized (mLockEnd)
        {
            // Wait that all the threads has been started.
            trace("Wait()");
            mLockEnd.wait();
            trace("wait ok()");
        }
        mSem.release();
        trace("unlock after wait");
        for (int i = 0; i < 5; ++i)
        {
            threadSleep(100);
            trace("Woke from sleep.");
            if (mRunningThreadCount == 0)
            {
                break;
            }
        }
        printTrace();
        assertTrue("Threads still running: " + mRunningThreadCount, mRunningThreadCount == 0);
    }

    /** CONDITION TESTS*/
    private ConditionObject mCondition;

    private void testCondition1()
    {
        mRunningThreadCount = 0;
        mNextReleasedThreadId = 1;
        mReleaseThreadId = 1;
        mMaxThreadId = 1;

        try
        {
            testCondtion();
        }
        catch (Throwable t)
        {
            assertTrue(t.toString(), false);
            t.printStackTrace();
        }
    }

    private void testCondition10_10()
    {
        mRunningThreadCount = 0;
        mNextReleasedThreadId = 1;
        mReleaseThreadId = 10;
        mMaxThreadId = 10;

        try
        {
            testCondtion();
        }
        catch (Throwable t)
        {
            assertTrue(t.toString(), false);
            t.printStackTrace();
        }
    }

    private void testCondition5_10()
    {
        mRunningThreadCount = 0;
        mNextReleasedThreadId = 1;
        mReleaseThreadId = 5;
        mMaxThreadId = 10;

        try
        {
            testCondtion();
        }
        catch (Throwable t)
        {
            assertTrue(t.toString(), false);
            t.printStackTrace();
        }
    }


    private class ConditionTestThread extends Thread
    {
        private int mId;
        
        private ConditionTestThread(int id)
        {
            mId = id;
        }
        public void run() 
        {
            threadSleep(20);
            trace("run: "+mId);
            incThreadCount();

            if (mReleaseThreadId == mId)
            {
                synchronized (mLockEnd)
                {
                    trace("notify: "+mId);
                    mLockEnd.notify();
                }
                threadSleep(50);
            }

            if (mId < mMaxThreadId)
            {
                // Need to create additional thread.
                trace("creating new thread: "+mId);
                new ConditionTestThread(mId+1).start();
            }

            trace("Going to await: "+mId);
            mCondition.await();
            trace("out from await: "+mId);
            decThreadCount();
            trace("done: "+mId);
        }
    }

    private void testCondtion() throws Exception
    {
        mCondition = new ConditionObject();
        new ConditionTestThread(1).start();

        mLockEnd = new Object();
        synchronized (mLockEnd)
        {
            // Wait that all the threads has been started.
            trace("wait()-->");
            mLockEnd.wait();
            trace("<--wait()");
        }

        assertTrue("Not correct amount of threads waiting: " + mRunningThreadCount, mRunningThreadCount == mReleaseThreadId);
        if (mMaxThreadId == 1)
        {
            threadSleep(100);
        }
        trace("before signal()");
        mCondition.signal();
        trace("after signal()");
        for (int i = 0; i < 5; ++i)
        {
            threadSleep(100);
            trace("Woke from sleep.");
            if (mRunningThreadCount == 0)
            {
                break;
            }
        }
        printTrace();
        assertTrue("Threads still running: " + mRunningThreadCount, mRunningThreadCount == 0);
    }
}
