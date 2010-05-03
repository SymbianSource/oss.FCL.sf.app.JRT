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
/*
 *  Licensed to the Apache Software Foundation (ASF) under one or more
 *  contributor license agreements.  See the NOTICE file distributed with
 *  this work for additional information regarding copyright ownership.
 *  The ASF licenses this file to You under the Apache License, Version 2.0
 *  (the "License"); you may not use this file except in compliance with
 *  the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/**
 * TimerTask tests adapted to Java ME from Apache Harmony (Open Source Java SE)
 * Main changes:
 *  - junit to j2meunit related changes
 */

package com.nokia.mj.test.java.util;

import java.util.Timer;
import java.util.TimerTask;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestSuite;
import j2meunit.framework.TestMethod;

public class TimerTaskTest extends TestCase
{
    Object sync = new Object(), start = new Object();

    public TimerTaskTest()
    {
    }

    public TimerTaskTest(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new TimerTaskTest("test_Constructor", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TimerTaskTest) tc).test_Constructor();
            }
        }));

        aSuite.addTest(new TimerTaskTest("test_cancel", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TimerTaskTest) tc).test_cancel();
            }
        }));

        aSuite.addTest(new TimerTaskTest("test_scheduledExecutionTime", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TimerTaskTest) tc).test_scheduledExecutionTime();
            }
        }));

        aSuite.addTest(new TimerTaskTest("test_run", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TimerTaskTest) tc).test_run();
            }
        }));

        aSuite.addTest(new TimerTaskTest("test_runThrows", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TimerTaskTest) tc).test_runThrows();
            }
        }));

        return aSuite;
    }

    /**
     * Warning: These tests have the possibility to leave a VM hanging if the
     * Timer is not cancelled.
     */
    class TimerTestTask extends TimerTask
    {
        private int wasRun = 0;

        // Set this to true to see normal tests fail (or hang possibly)
        // The default is false and needs to be set by some tests
        private boolean sleepInRun = false;

        public void run()
        {
            synchronized (this)
            {
                wasRun++;
            }
            synchronized (start)
            {
                start.notify();
            }
            if (sleepInRun)
            {

                try
                {
                    Thread.sleep(200);
                }
                catch (InterruptedException e)
                {
                }
            }
            synchronized (sync)
            {
                sync.notify();
            }
        }

        public synchronized int wasRun()
        {
            return wasRun;
        }

        public void sleepInRun(boolean value)
        {
            sleepInRun = value;
        }
    }

    /**
     * @tests java.util.TimerTask#TimerTask()
     */
    public void test_Constructor()
    {
        // Ensure the constructor does not fail
        new TimerTestTask();
    }

    /**
     * @tests java.util.TimerTask#cancel()
     */
    public void test_cancel()
    {
        Timer t = null;
        try
        {
            // Ensure cancel returns false if never scheduled
            TimerTestTask testTask = new TimerTestTask();
            assertTrue("Unsheduled tasks should return false for cancel()",
                       !testTask.cancel());

            // Ensure cancelled task never runs
            t = new Timer();
            testTask = new TimerTestTask();
            t.schedule(testTask, 500);
            assertTrue("TimerTask should not have run yet", testTask.cancel());
            t.cancel();

            // Ensure cancelling a task which has already run returns true
            t = new Timer();
            testTask = new TimerTestTask();
            t.schedule(testTask, 50);
            while (testTask.wasRun() == 0)
            {
                try
                {
                    Thread.sleep(150);
                }
                catch (InterruptedException e)
                {
                }
            }
            assertFalse(
                "TimerTask.cancel() should return false if task has run",
                testTask.cancel());
            assertFalse(
                "TimerTask.cancel() should return false if called a second time",
                testTask.cancel());
            t.cancel();

            // Ensure cancelling a repeated execution task which has never run
            // returns true
            t = new Timer();
            testTask = new TimerTestTask();
            t.schedule(testTask, 500, 500); // should never run
            assertTrue(
                "TimerTask.cancel() should return true if sheduled for repeated execution even if not run",
                testTask.cancel());
            t.cancel();

            // Ensure cancelling a repeated execution task which HAS run returns
            // true
            t = new Timer();
            testTask = new TimerTestTask();
            t.schedule(testTask, 50, 50);
            while (testTask.wasRun() == 0)
            {
                try
                {
                    Thread.sleep(100);
                }
                catch (InterruptedException e)
                {
                }
            }
            assertTrue(
                "TimerTask.cancel() should return true if sheduled for repeated execution and run",
                testTask.cancel());
            t.cancel();

            // Ensure calling cancel a second returns false
            t = new Timer();
            testTask = new TimerTestTask();
            t.schedule(testTask, 5000); // Should never run
            assertTrue(
                "TimerTask.cancel() should return true if task has never run",
                testTask.cancel());
            assertFalse(
                "TimerTask.cancel() should return false if called a second time",
                testTask.cancel());
            t.cancel();

            // Ensure cancelling a task won't cause deadlock
            t = new Timer();
            testTask = new TimerTestTask();
            testTask.sleepInRun(true);
            synchronized (start)
            {
                t.schedule(testTask, 0);
                try
                {
                    start.wait();
                    Thread.sleep(50);
                }
                catch (InterruptedException e)
                {
                }
            }
            assertFalse("TimerTask should have been cancelled", testTask
                        .cancel());
            t.cancel();
        }
        finally
        {
            if (t != null)
                t.cancel();
        }
    }

    /**
     * @tests java.util.TimerTask#scheduledExecutionTime()
     */
    public void test_scheduledExecutionTime()
    {
        Timer t = null;
        try
        {
            // Ensure scheduledExecutionTime is roughly right
            t = new Timer();
            TimerTestTask testTask = new TimerTestTask();
            t.schedule(testTask, 100);
            long time = System.currentTimeMillis() + 100;
            synchronized (sync)
            {
                try
                {
                    sync.wait(500);
                }
                catch (InterruptedException e)
                {
                }
            }
            long scheduledExecutionTime = testTask.scheduledExecutionTime();
            assertTrue(scheduledExecutionTime <= time);
            t.cancel();

            // Ensure scheduledExecutionTime is the last scheduled time
            t = new Timer();
            testTask = new TimerTestTask();
            t.schedule(testTask, 100, 500);
            long estNow = System.currentTimeMillis() + 100;
            // Will wake in 100, and every 500 run again
            // We want to try to get it after it's run at least once but not
            // twice
            synchronized (sync)
            {
                try
                {
                    sync.wait(500);
                }
                catch (InterruptedException e)
                {
                }
            }
            scheduledExecutionTime = testTask.scheduledExecutionTime();
            assertTrue(scheduledExecutionTime <= estNow);
            t.cancel();
        }
        finally
        {
            if (t != null)
                t.cancel();
        }

    }

    /**
     * @tests java.util.TimerTask#run()
     */
    public void test_run()
    {
        Timer t = null;
        try
        {
            // Ensure a new task is never run
            TimerTestTask testTask = new TimerTestTask();
            try
            {
                Thread.sleep(200);
            }
            catch (InterruptedException e)
            {
            }
            assertEquals("TimerTask.run() method should not have been called",
                         0, testTask.wasRun());

            // Ensure a task is run
            t = new Timer();
            testTask = new TimerTestTask();
            t.schedule(testTask, 200);
            while (testTask.wasRun() < 1)
            {
                try
                {
                    Thread.sleep(400);
                }
                catch (InterruptedException e)
                {
                }
            }
            assertFalse(testTask.cancel());
            t.cancel();
        }
        finally
        {
            if (t != null)
                t.cancel();
        }
    }

    private void assertFalse(String aStr, boolean aBool)
    {
        assertTrue(aStr, !aBool);
    }
    private void assertFalse(boolean aBool)
    {
        assertTrue(!aBool);
    }

    /**
     * @tests java.util.TimerTask#run() throws exception
     */
    public void test_runThrows()
    {
        Timer t = null;
        class TestTask extends TimerTestTask
        {
            public void run()
            {
                super.run();
                // throws exception
                byte[] array = null;
                array[1] = 0;
            }
        }
        try
        {
            // Ensure a task is run only once if exception is thrown in run()
            t = new Timer();
            TestTask testTask = new TestTask();

            t.schedule(testTask, 50, 50);
            try
            {
                Thread.sleep(300);
            }
            catch (InterruptedException e)
            {
            }
            t.cancel();
            assertEquals(testTask.wasRun(), 1);
        }
        finally
        {
            if (t != null)
                t.cancel();
        }
    }

}
