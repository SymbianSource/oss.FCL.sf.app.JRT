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


package com.nokia.mj.test.java.util;

import com.nokia.mj.impl.installer.utils.InstallerMain;

import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

public class AllTests extends TestCase implements InstallerMain
{
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TimerTaskTest().suite());
        suite.addTest(new TimerTest().suite());

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public AllTests() {}
    public AllTests(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    protected void setUp() {}
    protected void tearDown() {}
}
