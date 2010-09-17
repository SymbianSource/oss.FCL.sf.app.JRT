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


package com.nokia.mj.impl.coreuitest;


import com.nokia.mj.impl.coreui.CoreUi;
import com.nokia.mj.impl.coreuitest.CoreUiImpl;
import com.nokia.mj.impl.rt.support.JvmInternal;
import com.nokia.mj.impl.utils.Uid;


import com.nokia.mj.impl.rt.test.UnitTestSuiteCreator;
import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * SystemProperty unit tests.
 */
public class CoreUiTests extends TestCase implements UnitTestSuiteCreator
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
            suite.addTest(new CoreUiTests("AbNormalTests", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((CoreUiTests)tc).abNormalTests();
                }
            }));
        }

        if (testSelector.charAt(1) != '0')
        {
            suite.addTest(new CoreUiTests("AbNormalTests2", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((CoreUiTests)tc).abNormalTests2();
                }
            }));
        }

        if (testSelector.charAt(2) != '0')
        {
            suite.addTest(new CoreUiTests("NormalTests", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((CoreUiTests)tc).normalTests();
                }
            }));
        }

        return suite;

    }

    public CoreUiTests()
    {
    }

    public CoreUiTests(String aTestName, TestMethod aTestMethod)
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

    private void abNormalTests()
    {
        System.out.println("abNormalTests");
        // Don't set the com.nokia.coreui property yet.
        try
        {
            boolean res = CoreUi.connectToUi();
            assertTrue("Fail1, got: "+ res, res);
            CoreUi.createUi(null, false);

            CoreUi.createUi(null, false);
            CoreUi.shutdownRequest();
            CoreUi.foregroundRequest();

            res = CoreUi.isUiInForeground();
            assertTrue("Fail2, got: "+ res, res);
            CoreUi.hideApplication(false);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }


    private void abNormalTests2()
    {
        System.out.println("abNormalTests2");
        // Set the com.nokia.coreui property to point nonexisitng class.
        try
        {
            JvmInternal.setSystemProperty("com.nokia.coreui", "nonvalid");
            boolean res = CoreUi.connectToUi();
            assertTrue("No exception.", false);
        }
        catch (Error re)
        {
            int ind = re.toString().indexOf("Not able to instantiate class com.nokia.mj.impl.nonvalid.CoreUiImpl");
            boolean ok = ind >= 0;
            if (!ok)
            {
                // Accept also java.lang.ExceptionInInitializerError.
                ok = re.toString().equals("java.lang.ExceptionInInitializerError");
            }
            assertTrue(re.toString(),  ok);
        }
        catch (Throwable t2)
        {
            t2.printStackTrace();
            assertTrue(t2.toString(), false);
        }
    }

    private void normalTests()
    {
        // Set the com.nokia.coreui property to point valid class.
        System.out.println("normalTests");
        try
        {
            JvmInternal.setSystemProperty("com.nokia.coreui", "coreuitest");
            boolean res = CoreUi.connectToUi();
            assertTrue("Fail1, got: "+ res, res);

            // Set a null value to UID and check that an exception is thrown.
            try
            {
                CoreUi.createUi(null, false);
                assertTrue("No exception.", false);
            }
            catch (NullPointerException ne)
            {
            }

            Uid uid = Uid.createUid("[12345678]");
            CoreUi.createUi(uid, true);
            CoreUiImpl.createUiImplCheck(uid, true);
            CoreUi.createUi(uid, false);
            CoreUiImpl.createUiImplCheck(uid, false);

            CoreUi.shutdownRequest();

            CoreUi.foregroundRequest();

            res = CoreUi.isUiInForeground();
            assertTrue("Fail2, got: "+ res, res);

            CoreUi.hideApplication(true);
            CoreUiImpl.hideApplicationImplCheck(true);
            CoreUi.hideApplication(false);
            CoreUiImpl.hideApplicationImplCheck(false);

            CoreUiImpl.checkCounters(new int[] {1,2,1,1,1,2});
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }

    }

}



