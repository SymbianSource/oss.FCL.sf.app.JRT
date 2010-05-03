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


package com.nokia.mj.test.rms;
import j2meunit.framework.*;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import javax.microedition.rms.TestPackageProtectedMethods;
import com.nokia.mj.impl.rms.TestRecordEnumerationImpl;
import com.nokia.mj.impl.rms.TestTransaction;
import com.nokia.mj.impl.rms.TestPreparedStatement;
import com.nokia.mj.impl.rms.TestBadBehavior;


public class RmsMain extends TestCase implements InstallerMain
{
    public RmsMain() {}
    public RmsMain(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public Test suite()
    {
        TestSuite suite = new TestSuite();

        suite.addTest(new TestRunUnitTestMIDlets().suite());
        suite.addTest(new TestConcurrentAccess().suite());
        suite.addTest(new TestTransaction().suite());
        suite.addTest(new TestPreparedStatement().suite());
        suite.addTest(new TestBadBehavior().suite());
        suite.addTest(new TestPackageProtectedMethods().suite());
        suite.addTest(new TestRecordEnumerationImpl().suite());

        return suite;
    }

    public void installerMain(String[] args)
    {
        com.nokia.mj.impl.utils.OmjTestRunner.run(suite());
    }

}
