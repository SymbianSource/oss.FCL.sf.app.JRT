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


package com.nokia.mj.impl.rt.ui.qt;

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
 * RuntimeUiQt unit tests.
 */
public class RuntimeUiQtTests extends TestCase implements UnitTestSuiteCreator
{
    public TestSuite createTestSuite(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new RuntimeUiQtTests("testQt", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((RuntimeUiQtTests)tc).testQt();
            }
        }));

        return suite;
    }

    public RuntimeUiQtTests()
    {
    }

    public RuntimeUiQtTests(String aTestName, TestMethod aTestMethod)
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

    private void testQt()
    {
        doTest(new RuntimeUiQt());
    }

    private void doTest(RuntimeUi runtimeUi)
    {
        // error message with details -> make sure both are available to the user
        ExceptionBase exc = new ExceptionBase(
            new ShortErrorMessage(),
            ShortErrorMessage.MSG_WITH_DETAILS_ID,
            null, /* no params for short msg */
            new DetailedErrorMessage(),
            DetailedErrorMessage.MSG_ID,
            null /* no params for detailed msg */);


        runtimeUi.error("MyApplication", exc);

        // error message with short description only
        exc = new ExceptionBase(
            new ShortErrorMessage(),
            ShortErrorMessage.MSG_WITHOUT_DETAILS_ID,
            null, /* no params for short msg */
            null,
            0,
            null /* no params for detailed msg */);

        runtimeUi.error("MyApplication", exc);
        
        boolean answerAvailable = false;

        // Test confirm
        ConfirmData confirmData = new ConfirmData("Confirm dialog. Select Allow", new String[] {"ALLOW", "de_ny", "answerOption3"}, 1);
        answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);
        assertTrue(answerAvailable && confirmData.getAnswer() == 0);

        confirmData = new ConfirmData("Confirm dialog. Select Deny", new String[] {"SALLI", "deny", "answerOption3"}, 1);
        answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);
        assertTrue(answerAvailable && confirmData.getAnswer() == 1);

        confirmData = new ConfirmData("Null Answer options", null, 1);
        answerAvailable = runtimeUi.confirm("Null answer options", confirmData);
        assertTrue(answerAvailable);

        confirmData = new ConfirmData("Empty string", new String[] {"", "y"}, 1);
        answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);
        assertTrue(answerAvailable);

        confirmData = new ConfirmData("Long button text", new String[] {"loonglooongloongButoonTextloongloonglonng", "DENY"}, 1);
        answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);
        assertTrue(answerAvailable);

        ConfirmData nullConf = null;
        runtimeUi.confirm("Null Application", nullConf);

        // If confirm data does not contain localized texts for button names RuntimeUI populates them.
        // Check those are localized and correct response is received.
        confirmData = new ConfirmData("Test RuntimeUI loc working. Press allow", null, 0);
        answerAvailable = runtimeUi.confirm("MyFavouriteAplication", confirmData);        
        assertTrue(answerAvailable && confirmData.getAnswer() == 0);

        runtimeUi.destroy();
    }
}
