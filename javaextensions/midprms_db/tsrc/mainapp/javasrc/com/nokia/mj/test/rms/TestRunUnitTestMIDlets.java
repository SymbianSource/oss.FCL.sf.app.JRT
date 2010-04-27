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
* Description: TestRunUnitTestMIDlets
*
*/


package com.nokia.mj.test.rms;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

import javax.microedition.rms.*;
import com.nokia.mj.impl.rms.*;

public class TestRunUnitTestMIDlets extends TestCase
{

    public Test suite()
    {
        TestSuite suite = new TestSuite();

        suite.addTest(new TestRunUnitTestMIDlets("testRunMIDlets", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRunUnitTestMIDlets) tc).testRunMIDlets();
            }
        }));

        return suite;
    }

    public TestRunUnitTestMIDlets() {}
    public TestRunUnitTestMIDlets(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void testRunMIDlets()
    {
        System.out.println("TestRunUnitTestMIDlets.testRunMIDlets()");
        String storeName = "testRunMIDlets";
        try
        {
            AppLauncher.runApp("j2meunitomj.MIDletTestRunner", "Nokia", "SharedStore");
            AppLauncher.runApp("j2meunitomj.MIDletTestRunner", "Nokia", "RmsUnitTests");
        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("Unexpected exception " + e);
        }
    }
}
