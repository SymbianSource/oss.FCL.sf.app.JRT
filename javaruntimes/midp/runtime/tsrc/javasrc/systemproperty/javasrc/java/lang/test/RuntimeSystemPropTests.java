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

public class RuntimeSystemPropTests extends TestCase implements InstallerMain
{

    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new RuntimeSystemPropTests("getPropertyTests", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RuntimeSystemPropTests)tc).getPropertyTests();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public RuntimeSystemPropTests() {}

    public RuntimeSystemPropTests(String sTestName, TestMethod rTestMethod)
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

            String hostNameProperty = System.getProperty("microedition.hostname");
            System.out.println("hostNameProperty: " + hostNameProperty);
            if (0 != hostNameProperty.compareTo("localhost"))
                throw new TestFailedException("Incorrect hostNameProperty: " + hostNameProperty);

            String jtwiVersionProperty = System.getProperty("microedition.jtwi.version");
            System.out.println("jtwiVersionProperty: " + jtwiVersionProperty);
            if (0 != jtwiVersionProperty.compareTo("1.0"))
                throw new TestFailedException("Incorrect jtwiVersionProperty: " + jtwiVersionProperty);

            String msaVersionProperty = System.getProperty("microedition.msa.version");
            System.out.println("msaVersionProperty: " + msaVersionProperty);
            if (0 != msaVersionProperty.compareTo("1.0-SUBSET"))
                throw new TestFailedException("Incorrect msaVersionProperty: " + msaVersionProperty);

            String profilesProperty = System.getProperty("microedition.profiles");
            System.out.println("profilesProperty: " + profilesProperty);
            if (0 != profilesProperty.compareTo("MIDP-2.1"))
                throw new TestFailedException("Incorrect profilesProperty: " + profilesProperty);

            System.out.println("TEST CASE 1 WAS EXECUTED SUCCESSFULLY");
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }//end getPropertyTests()

}//end class RuntimeSystemPropTests

