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


package com.nokia.mj.test.comms;

import com.nokia.mj.impl.installer.utils.InstallerMain;

import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

public class CommsMain extends TestCase implements InstallerMain
{
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestCommsException().suite());
        suite.addTest(new TestCommsMessage().suite());
        suite.addTest(new TestCommsEndpoint().suite());
        suite.addTest(new TestCommsServerEndpoint().suite());

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public CommsMain() {}
    public CommsMain(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    protected void setUp() {}
    protected void tearDown() {}
}
