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
//import com.nokia.mj.impl.installer.utils.InstallerMain;

public class AllTests extends TestCase //implements InstallerMain
{
    public AllTests() {}
    public AllTests(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public Test suite()
    {
        TestSuite suite = new TestSuite();

        suite.addTest(new RecordStoreTest().suite());
        suite.addTest(new TestRecordStore().suite());
        suite.addTest(new TestRecordEnumeration().suite());
        suite.addTest(new TestMultithread().suite());
        suite.addTest(new TestExceptions().suite());

        return suite;
    }

    public void installerMain(String[] args)
    {
//        com.nokia.mj.impl.utils.OmjTestRunner.run( suite() );
    }

}
