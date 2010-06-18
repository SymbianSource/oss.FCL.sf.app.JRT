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


package com.nokia.mj.impl.rt.midp;

import java.util.Hashtable;

import com.nokia.mj.impl.installer.utils.InstallerMain;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * SystemProperty unit tests.
 */
public class UtilTests extends TestCase implements InstallerMain
{

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new UtilTests("testSplit", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((UtilTests)tc).testSplit();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);

    }

    public UtilTests()
    {
    }

    public UtilTests(String aTestName, TestMethod aTestMethod)
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

    private void testSplit()
    {
        try
        {
            Hashtable ht = MidletLifeCycle.split(null, null);
            assertTrue("Fail1 "+ ht, ht != null && ht.isEmpty());

            ht = MidletLifeCycle.split(null, "");
            assertTrue("Fail2 "+ ht, ht != null && ht.isEmpty());

            ht = MidletLifeCycle.split("test", null);
            assertTrue("Fail3 "+ ht, ht != null && ht.size() == 1 && ((String)ht.get("test")). equals(""));

            ht = MidletLifeCycle.split("test", "");
            assertTrue("Fail4 "+ ht, ht != null && ht.size() == 1 && ((String)ht.get("test")). equals(""));

            ht = MidletLifeCycle.split("   ", ",");
            assertTrue("Fail5 "+ ht, ht != null && ht.isEmpty());

            ht = MidletLifeCycle.split("Test", ",");
            assertTrue("Fail6 "+ ht, ht != null && ht.size() == 1 && ((String)ht.get("Test")). equals(""));

            ht = MidletLifeCycle.split(" Test ", ",");
            assertTrue("Fail7 "+ ht, ht != null && ht.size() == 1 && ((String)ht.get("Test")). equals(""));

            ht = MidletLifeCycle.split("Test,", ",");
            assertTrue("Fail8 "+ ht, ht != null && ht.size() == 1 && ((String)ht.get("Test")). equals(""));

            ht = MidletLifeCycle.split("   Test,   ", ",");
            assertTrue("Fail9 "+ ht, ht != null && ht.size() == 1 && ((String)ht.get("Test")). equals(""));

            ht = MidletLifeCycle.split("   Test1;  test2 ;  test3;  ", ";");
            assertTrue("Fail10 "+ ht, ht != null && ht.size() == 3 && ((String)ht.get("Test1")). equals("") &&
                                                                      ((String)ht.get("test2")). equals("") && 
                                                                      ((String)ht.get("test3")). equals(""));

            ht = MidletLifeCycle.split("   Test1,  test2 ;  test3  ", ";");
            assertTrue("Fail11 "+ ht, ht != null && ht.size() == 2 && ((String)ht.get("Test1,  test2")). equals("") &&
                                                                      ((String)ht.get("test3")). equals(""));


        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }
}
