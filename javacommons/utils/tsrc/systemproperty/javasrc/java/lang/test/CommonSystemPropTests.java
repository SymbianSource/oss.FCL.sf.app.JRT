/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

package java.lang.test;

import com.nokia.mj.impl.installer.utils.InstallerMain;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;
import java.lang.System;

public class CommonSystemPropTests extends TestCase implements InstallerMain
{

    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new CommonSystemPropTests("getPropertyTests", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((CommonSystemPropTests)tc).getPropertyTests();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public CommonSystemPropTests() {}

    public CommonSystemPropTests(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    private class TestFailedException extends RuntimeException
    {
        public TestFailedException()
        {
            super();
        }
        public TestFailedException(String s)
        {
            super(s);
        }
    }// end TestFailedException

    /**
     * TEST CASE 1
     */
    public void getPropertyTests()
    {
        try
        {
            System.out.println("EXECUTING TEST CASE 1...");
            //# 1.1
            System.out.println("# 1.1");

            String configProperty = System.getProperty("microedition.configuration");
            System.out.println("microedition.configuration: " + configProperty);
            if (0 != configProperty.compareTo("CLDC-1.1"))
                throw new TestFailedException("Incorrect configProperty: " + configProperty);

            String localeProperty = System.getProperty("microedition.locale");
            if (null == localeProperty)
                throw new TestFailedException("localeProperty is null");
            System.out.println("microedition.locale: " + localeProperty);

            String platformProperty = System.getProperty("microedition.platform");
            if (null == platformProperty)
                throw new TestFailedException("platformProperty is null");
            System.out.println("microedition.platform: " + platformProperty);

            System.out.println("TEST CASE 1 WAS EXECUTED SUCCESSFULLY");
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }//end getPropertyTests()

}//end class RuntimeSystemPropTests

