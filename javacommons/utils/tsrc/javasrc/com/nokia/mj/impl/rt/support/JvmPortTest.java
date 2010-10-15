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
import com.nokia.mj.impl.rt.support.ShutdownListener;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.rt.support.JvmInternal;
import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

import com.nokia.mj.impl.rt.support.prot.ProtectedTest;
/**
 * JvmPort unit tests.
 */
public class JvmPortTest extends TestCase implements UnitTestSuiteCreator
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
            suite.addTest(new JvmPortTest("testLoadSystemLibrary", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JvmPortTest)tc).testLoadSystemLibrary();
                }
            }));
        }


        if (testSelector.charAt(0) != '0')
        {
            suite.addTest(new JvmPortTest("testLoadApplicationClass", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JvmPortTest)tc).testLoadApplicationClass();
                }
            }));
        }

        if (testSelector.charAt(1) != '0')
        {
            suite.addTest(new JvmPortTest("testSetThreadAsDaemon", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JvmPortTest)tc).testSetThreadAsDaemon();
                }
            }));
        }


        if (testSelector.charAt(0) != '0')
        {
            suite.addTest(new JvmPortTest("testGetResourceAsNativeMemory", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JvmPortTest)tc).testGetResourceAsNativeMemory();
                }
            }));
        }

        if (testSelector.charAt(2) != '0')
        {
            suite.addTest(new JvmPortTest("testEnableRuntimeExit", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JvmPortTest)tc).testEnableRuntimeExit();
                }
            }));
        }



        if (testSelector.charAt(0) != '0')
        {
            suite.addTest(new JvmPortTest("testDisableRuntimeExit", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JvmPortTest)tc).testDisableRuntimeExit();
                }
            }));
        }


        if (testSelector.charAt(3) != '0')
        {
            suite.addTest(new JvmPortTest("testExitVm", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JvmPortTest)tc).testExitVm();
                }
            }));
        }

        if (testSelector.charAt(0) != '0')
        {
            suite.addTest(new JvmPortTest("testYoungGenerationGc", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JvmPortTest)tc).testYoungGenerationGc();
                }
            }));
        }

        if (testSelector.charAt(4) != '0')
        {
            suite.addTest(new JvmPortTest("testFinalization", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JvmPortTest)tc).testFinalization();
                }
            }));
        }

        if (testSelector.charAt(0) != '0')
        {
            suite.addTest(new JvmPortTest("testSetThreadEventListener", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JvmPortTest)tc).testSetThreadEventListener();
                }
            }));
        }

        if (testSelector.charAt(8) != '0')
        {
            suite.addTest(new JvmPortTest("testAddRestrictedPackagePrefixes", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JvmPortTest)tc).testAddRestrictedPackagePrefixes();
                }
            }));
        }

        if (testSelector.charAt(0) != '0')
        {
            suite.addTest(new JvmPortTest("testAddProtectedPackagePrefixes", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JvmPortTest)tc).testAddProtectedPackagePrefixes();
                }
            }));
        }


        if (testSelector.charAt(5) != '0')
        {
            suite.addTest(new JvmPortTest("testAppendToClassPath", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JvmPortTest)tc).testAppendToClassPath();
                }
            }));
        }


        if (testSelector.charAt(6) != '0')
        {
            suite.addTest(new JvmPortTest("testShrinkJavaHeapToMinimum", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JvmPortTest)tc).testShrinkJavaHeapToMinimum();
                }
            }));
        }

        if (testSelector.charAt(7) != '0')
        {
            suite.addTest(new JvmPortTest("testExpandJavaHeap", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JvmPortTest)tc).testExpandJavaHeap();
                }
            }));
        }


        return suite;

    }

    public JvmPortTest()
    {
    }

    public JvmPortTest(String aTestName, TestMethod aTestMethod)
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

    private void testLoadSystemLibrary()
    {
        try
        {
            //************ Jvm.loadSystemLibrary ***************************
            Jvm.loadSystemLibrary("javacommonutilstest");

            try
            {
                Jvm.loadSystemLibrary("nonexisting");
                assertTrue("Call should have failed", false);
            }
            catch (Error e)
            {
                int ind = e.toString().indexOf("Not able to load library nonexisting.");
                boolean ok = ind >= 0;
                if (!ok)
                {
                    // Accept also java.lang.ExceptionInInitializerError.
                    ok = e.toString().startsWith("java.lang.UnsatisfiedLinkError");
//                    ok = e instanceof UnsatisfiedLinkError;
                }
                assertTrue(e.toString(),  ok);
            }
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private void testLoadApplicationClass()
    {
        try
        {
            //************ Jvm.loadApplicationClass ***************************
            javax.microedition.rms.RecordComparator obj = (javax.microedition.rms.RecordComparator)
                    Jvm.loadApplicationClass("com.nokia.mj.impl.test.jvmporttest.TestClass").newInstance();
            assertTrue("Object was null", obj != null);
            int res = obj.compare(null, null);
            assertTrue("Incorrect result: " + res, res == 42);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private boolean isCldc()
    {
        String cldcProp = System.getProperty("com.nokia.jvm.port");
        if (cldcProp != null && cldcProp.equals("j9.JvmPortCldc"))
        {
            return true;
        }
        return false;

    }
    private void testSetThreadAsDaemon()
    {
        try
        {
            //************ Jvm.setThreadAsDaemon ***************************
            Thread th = new Thread(new Runnable()
            {
                public void run()
                {
                    try
                    {
                        Thread.sleep(2000);
                    }
                    catch (Exception re)
                    {
                    }
                }
            });
            if (isCldc())
            {
                try
                {
                    Jvm.setThreadAsDaemon(th, false);
                }
                catch (RuntimeException re)
                {
                    assertTrue(re.toString(), re.toString().indexOf("Setting thread as non-daemon not supported") >= 0);
                }
            }
            else
            {
                Jvm.setThreadAsDaemon(th, false);
            }
            Jvm.setThreadAsDaemon(th, true);
            th.start();
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private static final String jarFile = "c:\\java\\jvmportapp.jar";
    private static final String jarFile2 = "c:\\java\\jvmportapp2.jar";

    private void testGetResourceAsNativeMemory()
    {
        if (!isCldc())
        {
            return;
        }
        try
        {
            //************ Jvm.getResourceAsNativeMemory ***************************
            NativeMemoryBlock nativeMemory = Jvm.getResourceAsNativeMemory(null, "myres/info.txt");
            assertTrue("NativeMemoryBlock was null", nativeMemory != null);

            NativeMemoryBlock nativeMemory2 = Jvm.getResourceAsNativeMemory(null, "myres/info2.txt");
            assertTrue("NativeMemoryBlock2 was null", nativeMemory2 != null);

            String res = _getResource(nativeMemory.getPointer(), nativeMemory.getSize());
            assertTrue("Incorrect content1: " + res, res.equals("jvmPort(info): The quick brown fox jumps over the lazy dog"));

            res = _getResource(nativeMemory2.getPointer(), nativeMemory2.getSize());
            assertTrue("Incorrect content2 " + res, res.equals("jvmPort(info2): The quick brown fox jumps over the lazy dog"));

            nativeMemory.freeMemory();
            nativeMemory2.freeMemory();

            NativeMemoryBlock nativeMemory3 = Jvm.getResourceAsNativeMemory(jarFile2, "myres/info.txt");
            assertTrue("NativeMemoryBlock3 was null", nativeMemory3 != null);

            NativeMemoryBlock nativeMemory4 = Jvm.getResourceAsNativeMemory(jarFile2, "myres/info2.txt");
            assertTrue("NativeMemoryBlock4 was null", nativeMemory4 != null);

            res = _getResource(nativeMemory3.getPointer(), nativeMemory3.getSize());
            assertTrue("Incorrect content3: " + res, res.equals("jvmPort2(info): The quick brown fox jumps over the lazy dog"));

            res = _getResource(nativeMemory4.getPointer(), nativeMemory4.getSize());
            assertTrue("Incorrect content4 " + res, res.equals("jvmPort2(info2): The quick brown fox jumps over the lazy dog"));

            nativeMemory3.freeMemory();
            nativeMemory4.freeMemory();

            NativeMemoryBlock nativeMemory5 = Jvm.getResourceAsNativeMemory(jarFile2 + "t", "myres/info.txt");
            assertTrue("NativeMemoryBlock5 was not null", nativeMemory5 == null);

            NativeMemoryBlock nativeMemory6 = Jvm.getResourceAsNativeMemory(jarFile2 + "t", "myres/info2.txt");
            assertTrue("NativeMemoryBlock6 was not null", nativeMemory6 == null);


        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private void testEnableRuntimeExit()
    {
        try
        {
            //************ JvmInternal.enableRuntimeExit ***************************
            if (isCldc())
            {
                JvmInternal.enableRuntimeExit();
            }
            System.exit(40);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private void testDisableRuntimeExit()
    {
        if (!isCldc())
        {
            return;
        }

        try
        {
            //************ JvmInternal.disableRuntimeExit ***************************
            JvmInternal.disableRuntimeExit();
            try
            {
                System.exit(-123);
                assertTrue("System exit should have failed", false);
            }
            catch (SecurityException se)
            {
            }
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
        JvmInternal.enableRuntimeExit();
    }

    private void testExitVm()
    {
        try
        {
            //************ JvmInternal.exitVm ***************************
            JvmInternal.exitVm(42);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private void testYoungGenerationGc()
    {
        try
        {
            //************ JvmInternal.runYoungGenerationGc ***************************
            Runtime rt = Runtime.getRuntime();
            {
                new FinalizeClass();
                new FinalizeClass();
                new FinalizeClass();
            }
            long startFree = rt.freeMemory();
            boolean supported = JvmInternal.runYoungGenerationGc();
            long endFree = rt.freeMemory();
            if (supported)
            {
                assertTrue("endFree (" + endFree + ") <= startFree (" + startFree + ")", startFree < endFree);
            }
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private static class FinalizeClass
    {
        private static boolean mFinalized = false;

        static
        {
            JvmInternal.enableFinalization(JvmPortTest.FinalizeClass.class);
        }

        protected final void finalize()
        {
            mFinalized = true;
        }
    }

    private void testFinalization()
    {
        try
        {
            //************ JvmInternal.runYoungGenerationGc ***************************
            FinalizeClass fClass = new FinalizeClass();
            fClass = null;
            System.gc();
            JvmInternal.runFinalization();
            assertTrue("Class was not finalized", FinalizeClass.mFinalized);

        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private class TestListener implements ThreadEventListener
    {
        private int startCount = 0;
        private int diedCount = 0;
        private int uncaughtCount = 0;
        private Throwable throwable;
        private Thread throwngThread;
        private Thread newThread;
        private Thread parentThread;
        private Thread dyingThread;

        public synchronized void threadStarting(Thread newThread, Thread parentThread)
        {
            startCount++;
            this.newThread = newThread;
            this.parentThread = parentThread;
        }
        public synchronized void threadDied(Thread thread)
        {
            diedCount++;
            dyingThread = thread;
        }
        public synchronized void uncaughtException(Thread thread, Throwable t)
        {
            throwngThread = thread;
            throwable = t;
            uncaughtCount++;
        }

    }
    private void testSetThreadEventListener()
    {
        try
        {
            Thread currentThread = Thread.currentThread();
            //************ JvmInternal.setThreadEventListener ***************************
            TestListener listener = new TestListener();
            JvmInternal.setThreadEventListener(listener);
            Thread th = new Thread(new Runnable()
            {
                public void run() {}
            });
            th.start();
            Thread.sleep(50);
            assertTrue("new thread not correct(1)", th == listener.newThread);
            assertTrue("parent thread not correct(1)", currentThread == listener.parentThread);
            assertTrue("died thread not correct(1)", th == listener.dyingThread);

            th = new Thread(new Runnable()
            {
                public void run() {}
            });
            th.start();
            Thread.sleep(50);
            assertTrue("new thread not correct(2)", th == listener.newThread);
            assertTrue("parent thread not correct(2)", currentThread == listener.parentThread);
            assertTrue("died thread not correct(2)", th == listener.dyingThread);

            th = new Thread(new Runnable()
            {
                public void run() {}
            });
            th.start();
            Thread.sleep(50);
            assertTrue("new thread not correct(3)", th == listener.newThread);
            assertTrue("parent thread not correct(3)", currentThread == listener.parentThread);
            assertTrue("died thread not correct(3)", th == listener.dyingThread);

            final RuntimeException re = new RuntimeException("ex");
            th = new Thread(new Runnable()
            {
                public void run()
                {
                    throw re;
                }
            });
            th.start();
            Thread.sleep(150);
            assertTrue("new thread not correct(4)", th == listener.newThread);
            assertTrue("parent thread not correct(4)", currentThread == listener.parentThread);
            assertTrue("died thread not correct(4)", th == listener.dyingThread);
            assertTrue("Throwable not correct(4)", re == listener.throwable);

            assertTrue("Start count not correct: " + listener.startCount, listener.startCount == 4);
            assertTrue("End count not correct: " + listener.diedCount, listener.diedCount == 4);
            assertTrue("Throwable count not correct: " + listener.uncaughtCount, listener.uncaughtCount == 1);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private void testAddRestrictedPackagePrefixes()
    {
        if (!isCldc())
        {
            return;
        }
        try
        {
            //************ JvmInternal.addRestrictedPackagePrefixes ***************************

            javax.microedition.rms.RecordComparator obj0 = (javax.microedition.rms.RecordComparator)
                    Jvm.loadApplicationClass("com.nokia.mj.impl.test.jvmporttest2.TestClass").newInstance();
            assertTrue("Object was null", obj0 != null);
            int res = obj0.compare(null, new byte[1]);
            assertTrue("Restricted test failed: " + res, res == 0);

            JvmInternal.addRestrictedPackagePrefixes(new String[] {"com.nokia.mj.impl.rt.support.restricted."});

            javax.microedition.rms.RecordComparator obj = (javax.microedition.rms.RecordComparator)
                    Jvm.loadApplicationClass("com.nokia.mj.impl.test.jvmporttest2.TestClass").newInstance();
            assertTrue("Object was null", obj != null);
            res = obj.compare(new byte[1], null);
            assertTrue("Restricted test failed2: " + res, res == 0);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private void testAddProtectedPackagePrefixes()
    {
        if (!isCldc())
        {
            return;
        }
        try
        {
            //************ JvmInternal.addProtectedPackagePrefixes ***************************
            javax.microedition.rms.RecordComparator obj0 = (javax.microedition.rms.RecordComparator)
                    Jvm.loadApplicationClass("com.nokia.mj.impl.test.jvmporttest.TestClass").newInstance();
            assertTrue("Object was null", obj0 != null);
            int res = obj0.compare(null, new byte[1]);
            assertTrue("Protected test failed: " + res, res == 0);

            JvmInternal.addProtectedPackagePrefixes(new String[] {"com.nokia.mj.impl.rt.support.prot."});

            javax.microedition.rms.RecordComparator obj = (javax.microedition.rms.RecordComparator)
                    Jvm.loadApplicationClass("com.nokia.mj.impl.test.jvmporttest.TestClass").newInstance();
            assertTrue("Object was null", obj != null);
            res = obj.compare(new byte[1], null);
            assertTrue("Protected test failed2: " + res, res == 0);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private void testAppendToClassPath()
    {
        try
        {
            //************ JvmInternal.appendToClassPath ***************************
            javax.microedition.rms.RecordComparator obj = null;
            try
            {
                obj = (javax.microedition.rms.RecordComparator)
                      Jvm.loadApplicationClass("com.nokia.mj.impl.test.jvmporttest.TestClass").newInstance();
                assertTrue("Shouldn't be found.", false);
            }
            catch (ClassNotFoundException cnfe) {}

            try
            {
                obj = (javax.microedition.rms.RecordComparator)
                      Jvm.loadApplicationClass("com.nokia.mj.impl.test.jvmporttest2.TestClass").newInstance();
                assertTrue("Shouldn't be found2.", false);
            }
            catch (ClassNotFoundException cnfe)
            {
            }

            JvmInternal.appendToClassPath(jarFile);
            obj = (javax.microedition.rms.RecordComparator)
                  Jvm.loadApplicationClass("com.nokia.mj.impl.test.jvmporttest.TestClass").newInstance();
            assertTrue("Object was null", obj != null);
            int res = obj.compare(null, null);
            assertTrue("Incorrect result: " + res, res == 42);

            try
            {
                obj = (javax.microedition.rms.RecordComparator)
                      Jvm.loadApplicationClass("com.nokia.mj.impl.test.jvmporttest2.TestClass").newInstance();
                assertTrue("Shouldn't be found3.", false);
            }
            catch (ClassNotFoundException cnfe)
            {
            }

            JvmInternal.appendToClassPath(jarFile2);
            obj = (javax.microedition.rms.RecordComparator)
                  Jvm.loadApplicationClass("com.nokia.mj.impl.test.jvmporttest.TestClass").newInstance();
            assertTrue("Object was null 2", obj != null);
            res = obj.compare(null, null);
            assertTrue("Incorrect result(2): " + res, res == 42);

            obj = (javax.microedition.rms.RecordComparator)
                  Jvm.loadApplicationClass("com.nokia.mj.impl.test.jvmporttest2.TestClass").newInstance();
            assertTrue("Object was null 3", obj != null);
            res = obj.compare(null, null);
            assertTrue("Incorrect result(3): " + res, res == 43);

        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }


    private void testShrinkJavaHeapToMinimum()
    {
        try
        {
            //************ JvmInternal.shrinkJavaHeapToMinimum ***************************
            Object[] o = new Object[10000];
            assertTrue("Alocation failed: " + o.length, o.length > 0);

            Runtime rt = Runtime.getRuntime();
            long startTotal = rt.totalMemory();
            o = null;
            int shrinkAmount = JvmInternal.shrinkJavaHeapToMinimum();
            long endTotal = rt.totalMemory();
            assertTrue("Heap shrink failed: " + shrinkAmount, shrinkAmount > 0);
            assertTrue("Heap shrink failed, total mem was not reduced. endTotal: " + endTotal + ", startTotal: " + startTotal, endTotal < startTotal);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private void testExpandJavaHeap()
    {
        try
        {
            //************ JvmInternal.expandJavaHeap ***************************
            Runtime rt = Runtime.getRuntime();
            long startTotal = rt.totalMemory();
            final int expandSize = 1024 * 1024;
            int expanded = JvmInternal.expandJavaHeap(expandSize);
            assertTrue("Didn't expand as expected: " + expanded, expandSize == expanded);
            long endTotal = rt.totalMemory();
            assertTrue("Runtime.totalMemory not what expected. startTotal: " + startTotal + ", endTotal: " + endTotal, (endTotal - startTotal) == expandSize);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private native String _getResource(long ptr, int size);

}


