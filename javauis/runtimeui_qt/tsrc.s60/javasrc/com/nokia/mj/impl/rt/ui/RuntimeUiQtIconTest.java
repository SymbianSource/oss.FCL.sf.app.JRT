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
* Description: Tests for RuntimeUI confirmation dialog icons.
*
*/


package com.nokia.mj.impl.rt.ui;

import com.nokia.mj.impl.rt.test.UnitTestSuiteCreator;
import com.nokia.mj.impl.utils.exception.ExceptionBase;
import com.nokia.mj.impl.rt.ui.qt.RuntimeUiQt;
import com.nokia.mj.impl.rt.ui.RuntimeUi;
import com.nokia.mj.impl.rt.ui.ConfirmData;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * RuntimeUiQt icon tests.
 */
public class RuntimeUiQtIconTest extends TestCase implements UnitTestSuiteCreator
{
    public TestSuite createTestSuite(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new RuntimeUiQtIconTest("testIcons", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RuntimeUiQtIconTest)tc).testIcons();
            }
        }));

        return suite;
    }

    public RuntimeUiQtIconTest()
    {
    }

    public RuntimeUiQtIconTest(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    // End j2meunit test framework setup

    protected void setUp()
    {
    }

    protected void tearDown()
    {
    }

    private void testIcons()
    {
        doTest(new RuntimeUiQt());
    }

    private void doTest(RuntimeUi runtimeUi)
    {
        boolean answerAvailable = false;
        
        // Check untrusted icon.
        ConfirmData confirmData = new ConfirmData(
            "Check untrusted icon. If wrong choose DENY", new String[] {"allow", "deny"}, 1);
        runtimeUi.setIdentified(false);
        answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);
                
        assertTrue(answerAvailable && confirmData.getAnswer() == 0);

        // Check trusted icon.
        confirmData = new ConfirmData(
            "Check trusted icon. If wrong choose DENY", new String[] {"allow", "deny"}, 1);
        runtimeUi.setIdentified(true);
        answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);
        assertTrue(answerAvailable && confirmData.getAnswer() == 0);

        runtimeUi.destroy();
    }
}
