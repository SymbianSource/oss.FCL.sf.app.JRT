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
import com.nokia.mj.impl.rt.test.JvmPortTest;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.rt.support.JvmInternal;
import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * JvmPort basic unit tests. Testing that the delegation works.
 */
public class JvmPortBasicTest extends TestCase implements UnitTestSuiteCreator
{

    // Begin j2meunit test framework setup
    public TestSuite createTestSuite(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new JvmPortBasicTest("testJvm", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JvmPortBasicTest)tc).testJvm();
            }
        }));

        suite.addTest(new JvmPortBasicTest("testVmPortNotExist", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JvmPortBasicTest)tc).testVmPortNotExist();
            }
        }));


        return suite;

    }

    public JvmPortBasicTest()
    {
    }

    public JvmPortBasicTest(String aTestName, TestMethod aTestMethod)
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

    private void checkCallCounts(String info, int ind)
    {

        assertTrue(info + " failed " + JvmPortTest.print(),
                   JvmPortTest.checkCalls(ind));

    }

    private void checkObjects(String info, Object obj1, Object obj2, Object res)
    {

        boolean ok = JvmPortTest.mObj1 == obj1;
        if (!ok && obj1 != null)
        {
            ok = obj1.equals(JvmPortTest.mObj1);
        }
        assertTrue(info + " failed. obj1 " + obj1 + " != " + " mObj1 " + JvmPortTest.mObj1, ok);

        ok = JvmPortTest.mObj2 == obj2;
        if (!ok && obj2 != null)
        {
            ok = obj2.equals(JvmPortTest.mObj2);
        }
        assertTrue(info + " failed. obj2 " + obj2 + " != " + " mObj2 " + JvmPortTest.mObj2, ok);

        ok = JvmPortTest.mReturnObj == res;
        if (!ok && res != null)
        {
            ok = res.equals(JvmPortTest.mReturnObj);
        }
        assertTrue(info + " failed. res " + res + " != " + " mReturnObj " + JvmPortTest.mReturnObj, ok);

    }
    // Test cases

    private static final String sysProp = "com.nokia.jvm.port";

    private void setPort(String port)
    {
        JvmPortTest.setSystemPropertyImpl(sysProp, port);
        JvmPortTest.resetInstance();
    }
    private void testJvm()
    {
        final String testProp = "test.JvmPortTest";
        String origPort = System.getProperty(sysProp);
        try
        {
            Object testObject1 = new String("testlib");
            Object testObject2 = null;
            Object returnObject = null;
            String methodName;
            //************ Jvm.loadSystemLibrary ***************************
            setPort(testProp);
            Jvm.loadSystemLibrary((String)testObject1);
            setPort(origPort);
            methodName = "Jvm.loadSystemLibrary()";
            checkCallCounts(methodName, JvmPortTest.LOADSYSTEMLIBRARY_CALL);
            checkObjects(methodName, testObject1, null, null);
            JvmPortTest.clear();

            //************ Jvm.loadApplicationClass ***************************
            testObject1 = new String("testClass");
            JvmPortTest.mReturnObj = getClass();
            setPort(testProp);
            returnObject = Jvm.loadApplicationClass((String)testObject1);
            methodName = "Jvm.loadApplicationClass()";
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.LOADAPPLICATIONCLASS_CALL);
            checkObjects(methodName, testObject1, null, returnObject);
            JvmPortTest.clear();

            JvmPortTest.mThrowable = new ClassNotFoundException();
            setPort(testProp);
            try
            {
                returnObject = Jvm.loadApplicationClass((String)testObject1);
                setPort(origPort);
                assertTrue(methodName + ", no Excpetion", false);
            }
            catch (ClassNotFoundException cnfe)
            {
                setPort(origPort);
                assertTrue(methodName + ", wrong exception: " + cnfe,
                           cnfe == JvmPortTest.mThrowable);
            }
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.LOADAPPLICATIONCLASS_CALL);
            JvmPortTest.clear();

            //************ Jvm.setThreadAsDaemon ***************************
            testObject1 = new Thread(new Runnable()
            {
                public void run() {}
            });
            testObject2 = new Boolean(true);
            setPort(testProp);
            Jvm.setThreadAsDaemon((Thread)testObject1, ((Boolean)testObject2).booleanValue());
            methodName = "Jvm.setThreadAsDaemon()";
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.SETTHREADASDAEMON_CALL);
            checkObjects(methodName, testObject1, testObject2, null);
            JvmPortTest.clear();

            JvmPortTest.mThrowable = new IllegalThreadStateException();
            setPort(testProp);
            try
            {
                Jvm.setThreadAsDaemon((Thread)testObject1, ((Boolean)testObject2).booleanValue());
                setPort(origPort);
                assertTrue(methodName + ", no Excpetion", false);
            }
            catch (IllegalThreadStateException itse)
            {
                setPort(origPort);
                assertTrue(methodName + ", wrong exception: " + itse,
                           itse == JvmPortTest.mThrowable);
            }
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.SETTHREADASDAEMON_CALL);
            JvmPortTest.clear();

            JvmPortTest.mThrowable = new SecurityException();
            setPort(testProp);
            try
            {
                Jvm.setThreadAsDaemon((Thread)testObject1, ((Boolean)testObject2).booleanValue());
                setPort(origPort);
                assertTrue(methodName + ", no Excpetion", false);
            }
            catch (SecurityException se)
            {
                setPort(origPort);
                assertTrue(methodName + ", wrong exception: " + se,
                           se == JvmPortTest.mThrowable);
            }
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.SETTHREADASDAEMON_CALL);
            JvmPortTest.clear();

            //************ Jvm.getResourceAsNativeMemory ***************************
            setPort(testProp);
            testObject1 = new String("jarPath");
            testObject2 = new String("resName");
            JvmPortTest.mReturnObj = new NativeMemoryBlock()
            {
                public long getPointer()
                {
                    return 0;
                }
                public int getSize()
                {
                    return 0;
                }
                public void freeMemory() {}
            };
            returnObject = Jvm.getResourceAsNativeMemory((String)testObject1, (String)testObject2);
            methodName = "Jvm.getResourceAsNativeMemory()";
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.GETRESOURCEASNATIVEMEMORY_CALL);
            checkObjects(methodName, testObject1, testObject2, returnObject);
            JvmPortTest.clear();

            //************ JvmInternal.enableRuntimeExit ***************************
            setPort(testProp);
            JvmInternal.enableRuntimeExit();
            methodName = "JvmInternal.enableRuntimeExit()";
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.ENABLERUNTIMEEXIT_CALL);
            checkObjects(methodName, null, null, null);
            JvmPortTest.clear();

            //************ JvmInternal.disableRuntimeExit ***************************
            setPort(testProp);
            JvmInternal.disableRuntimeExit();
            methodName = "JvmInternal.disableRuntimeExit()";
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.DISABLERUNTIMEEXIT_CALL);
            checkObjects(methodName, null, null, null);
            JvmPortTest.clear();

            //************ JvmInternal.exitVm ***************************
            setPort(testProp);
            testObject1 = new Integer(42);
            JvmInternal.exitVm(((Integer)testObject1).intValue());
            methodName = "JvmInternal.exitVm()";
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.EXITVM_CALL);
            checkObjects(methodName, testObject1, null, null);
            JvmPortTest.clear();

            //************ JvmInternal.enableFinalization ***************************
            setPort(testProp);
            testObject1 = getClass();
            JvmInternal.enableFinalization((Class)testObject1);
            methodName = "JvmInternal.enableFinalization()";
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.ENABLEFINALIZATION_CALL);
            checkObjects(methodName, testObject1, null, null);
            JvmPortTest.clear();

            //************ JvmInternal.runFinalization ***************************
            setPort(testProp);
            JvmInternal.runFinalization();
            methodName = "JvmInternal.runFinalization()";
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.RUNFINALIZATION_CALL);
            checkObjects(methodName, null, null, null);
            JvmPortTest.clear();

            //************ JvmInternal.runYoungGenerationGc ***************************

            setPort(testProp);
            JvmPortTest.mReturnObj = new Boolean(true);
            boolean res = JvmInternal.runYoungGenerationGc();
            returnObject = new Boolean(res);
            methodName = "JvmInternal.runYoungGenerationGc()";
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.RUNYOUNGGENERATIONGC_CALL);
            checkObjects(methodName, null, null, returnObject);
            JvmPortTest.clear();


            //************ JvmInternal.setThreadEventListener ***************************
            setPort(testProp);
            testObject1 = new ThreadEventListener()
            {
                public void threadStarting(Thread newThread, Thread parentThread) {}
                public void threadDied(Thread thread) {}
                public void uncaughtException(Thread thread, Throwable e) {}
            };
            JvmInternal.setThreadEventListener((ThreadEventListener)testObject1);
            methodName = "JvmInternal.setThreadEventListener()";
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.SETTHREADEVENTLISTENER_CALL);
            checkObjects(methodName, testObject1, null, null);
            JvmPortTest.clear();

            //************ JvmInternal.addRestrictedPackagePrefixes ***************************
            setPort(testProp);
            testObject1 = new String[] {"package1, package2, package3"};
            JvmInternal.addRestrictedPackagePrefixes((String[])testObject1);
            methodName = "JvmInternal.addRestrictedPackagePrefixes()";
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.ADDRESTRICTEDPACKAGEPREFIXES_CALL);
            checkObjects(methodName, testObject1, null, null);
            JvmPortTest.clear();

            JvmPortTest.mThrowable = new SecurityException();
            setPort(testProp);
            try
            {
                JvmInternal.addRestrictedPackagePrefixes((String[])testObject1);
                setPort(origPort);
                assertTrue(methodName + ", no Excpetion", false);
            }
            catch (SecurityException se)
            {
                setPort(origPort);
                assertTrue(methodName + ", wrong exception: " + se,
                           se == JvmPortTest.mThrowable);
            }
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.ADDRESTRICTEDPACKAGEPREFIXES_CALL);
            JvmPortTest.clear();

            //************ JvmInternal.addProtectedPackagePrefixes ***************************
            setPort(testProp);
            testObject1 = new String[] {"package4, package5, package6"};
            JvmInternal.addProtectedPackagePrefixes((String[])testObject1);
            methodName = "JvmInternal.addProtectedPackagePrefixes()";
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.ADDPROTECTEDPACKAGEPREFIXES_CALL);
            checkObjects(methodName, testObject1, null, null);
            JvmPortTest.clear();

            JvmPortTest.mThrowable = new SecurityException();
            setPort(testProp);
            try
            {
                JvmInternal.addProtectedPackagePrefixes((String[])testObject1);
                setPort(origPort);
                assertTrue(methodName + ", no Excpetion", false);
            }
            catch (SecurityException se)
            {
                setPort(origPort);
                assertTrue(methodName + ", wrong exception: " + se,
                           se == JvmPortTest.mThrowable);
            }
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.ADDPROTECTEDPACKAGEPREFIXES_CALL);
            JvmPortTest.clear();

            //************ JvmInternal.appendToClassPath ***************************
            setPort(testProp);
            testObject1 = new String("class to append");
            JvmInternal.appendToClassPath((String)testObject1);
            methodName = "JvmInternal.appendToClassPath()";
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.APPENDTOCLASSPATH_CALL);
            checkObjects(methodName, testObject1, null, null);
            JvmPortTest.clear();

            //************ JvmInternal.shrinkJavaHeapToMinimum ***************************
            setPort(testProp);
            JvmPortTest.mReturnObj = new Integer(22);
            int res2 = JvmInternal.shrinkJavaHeapToMinimum();
            returnObject = new Integer(res2);
            methodName = "JvmInternal.shrinkJavaHeapToMinimum()";
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.SHRINKJAVAHEAPTOMINIMUM_CALL);
            checkObjects(methodName, null, null, returnObject);
            JvmPortTest.clear();

            //************ JvmInternal.expandJavaHeap ***************************
            setPort(testProp);
            testObject1 = new Integer(33);
            JvmPortTest.mReturnObj = new Integer(44);
            int res3 = JvmInternal.expandJavaHeap(((Integer)testObject1).intValue());
            returnObject = new Integer(res3);
            methodName = "JvmInternal.expandJavaHeap()";
            setPort(origPort);
            checkCallCounts(methodName, JvmPortTest.EXPANDJAVAHEAP_CALL);
            checkObjects(methodName, testObject1, null, returnObject);
            JvmPortTest.clear();

            //************ END***************************
            assertTrue("Calls missed", JvmPortTest.allCalled());

        }
        catch (Throwable t)
        {
            setPort(origPort);
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
        setPort(origPort);
    }

    private void testVmPortNotExist()
    {
        System.out.println("NOTE!! IT IS EXPECTED THAT THIS TEST WILL LEAD TO SOME ERROR LOGS.");
        final String testProp = "test.JvmPortTest";
        String origPort = System.getProperty(sysProp);
        try
        {
            setPort("TestPort");
            JvmInternal.enableRuntimeExit();
            setPort(origPort);
            assertTrue("No exception.", false);
        }
        catch (RuntimeException re)
        {
            setPort(origPort);
            int ind = re.toString().indexOf("Not able to instantiate class com.nokia.mj.impl.rt.TestPort");
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

        try
        {
            setPort(origPort);
        }
        catch (Throwable t)
        {
        }
    }
}
